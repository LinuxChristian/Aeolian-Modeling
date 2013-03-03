%  This program is free software: you can redistribute it and/or modify
%  it under the terms of the GNU General Public License as published by
%  the Free Software Foundation, either version 3 of the License, or
%  (at your option) any later version.
%
%  This program is distributed in the hope that it will be useful,
%  but WITHOUT ANY WARRANTY; without even the implied warranty of
%  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%  GNU General Public License for more details.
%
%  You should have received a copy of the GNU General Public License
%  along with this program.  If not, see <http://www.gnu.org/licenses/>.
%
% DESCRIPTION:
% The following C-code computes dune migration and aeolian processes
% using the Werner or Momiji algorithms.
%
% You can find the orginal article at: http://www.emin.geog.ucl.ac.uk/~hmomiji/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mex.h"
#include "nrutil.h"
#include "nrutil.c"

void avalance_ero(int **Z,int ny,int nx,int *ii,int *jj,int di[],int dj[],float dd[])
{
  /*function avalance_ero performs avalanching at erosion site*/


  int k;
  int i,j,imax,jmax;
  float dh,maxdh = 0.0;
  float thres = 2.0;

  for (k=0;k<8;k++) {

    i = (*ii) + di[k];
    j = (*jj) + dj[k];

    /*boundaries*/
    if (i<0) i = ny + i;
    if (i>=ny) i = i - ny;
    if (j<0) j = nx + j;
    if (j>=nx) j = j - nx;

    dh = (float) (Z[i][j]-Z[(*ii)][(*jj)])/dd[k];
    if (dh > maxdh) {
      maxdh = dh;
      imax = i;
      jmax = j;
    }

  }/*k*/

  /*avalance*/
  if (maxdh > thres) {

    Z[(*ii)][(*jj)] += 1;
    Z[imax][jmax] -= 1;
    (*ii) = imax;
    (*jj) = jmax;
    
    avalance_ero(Z,ny,nx,ii,jj,di,dj,dd);
  }

}

void avalance_depo(int **Z,int ny,int nx,int *ii,int *jj,int di[],int dj[],float dd[])
{
  /*function avalance_depo performs avalancing at deposition site*/

  int k;
  int i,j,imax,jmax;
  float dh,maxdh = 0.0;
  float thres = 2.0;

  for (k=0;k<8;k++) {

    i = (*ii) + di[k];
    j = (*jj) + dj[k];

    /*boundaries*/
    if (i<0) i = ny + i;
    if (i>=ny) i = i - ny;
    if (j<0) j = nx + j;
    if (j>=nx) j = j - nx;

    dh = (float) (Z[(*ii)][(*jj)]-Z[i][j])/dd[k];
    if (dh > maxdh) {
      maxdh = dh;
      imax = i;
      jmax = j;
    }

  }/*k*/

  /*avalance*/
  if (maxdh > thres) {

    Z[(*ii)][(*jj)] -= 1;
    Z[imax][jmax] += 1;
    (*ii) = imax;
    (*jj) = jmax;
    
    avalance_depo(Z,ny,nx,ii,jj,di,dj,dd);
  }

}


int inshade(int **Z,int nx,int ny,int *ii,int *jj,int dlj,int dli)
{
  /*function inshade returns 1 if cell is in shadow zone, 0 otherwise*/

  int i,shade;
  int js;

  i = 1; shade = 0;
  while ((i<=50) && (shade == 0)) {
    js = (*jj) - i; if (js < 0) js = nx + js;
    if (Z[(*ii)][js] >= (Z[(*ii)][(*jj)] + i)) shade = 1;
    i += 1;
  }

  return(shade);

  };
};


void mad(int **Z,int ny,int nx,float pns,float ps,int dlj,int *ii,int *jj,int *depo,int di[],int dj[],float dd[],int speedup,float dlc,float dlcc,int h_avg,int dli)
{
  /*function mad moves and depisits a sand slap*/

  int shade,dh;
  shade = 0;

  /*move slap */
  (*jj) += dlj; 
  if ((*jj) >= nx) (*jj) = (*jj) - nx;  
  if ((*jj) < 0) (*jj) = (*jj) + nx;

  /* Take care of speedup */
  if (speedup) {
  dh = Z[(*ii)][(*jj)]-h_avg;
  if (dh > 0) {
      (*jj) += (int) dlc*dh + dlcc*dh*dh; 
  } else {
    (*jj) += (int) dlc*dh; 
  };

  /* Check boundries */
  if ((*jj) >= nx) (*jj) = (*jj) - nx;
  if ((*jj) < 0) (*jj) = (*jj) + nx;

  /*shadow zone?*/
  shade = inshade(Z,nx,ny,ii,jj,dlj,dli);

  /*if in shadow zone*/
  if (shade > 0) {
    Z[(*ii)][(*jj)] += 1;
    (*depo) = 1;
    avalance_depo(Z,ny,nx,ii,jj,di,dj,dd);
  }
  /*if sand exist*/
  else {
    if (Z[(*ii)][(*jj)] > 0) {
      ran = (float)rand()/(float)RAND_MAX;
      if (ran <= ps) {
	Z[(*ii)][(*jj)] += 1;
	(*depo) = 1;
	avalance_depo(Z,ny,nx,ii,jj,di,dj,dd);
      }
    }
    /*if no sand exist*/
    else {
      ran = (float)rand()/(float)RAND_MAX;
      if (ran <= pns) {
	Z[(*ii)][(*jj)] += 1;
	(*depo) = 1;
	avalance_depo(Z,ny,nx,ii,jj,di,dj,dd);
      }
    }
  }/*else*/

}/*mad*/


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {

  if (nrhs < 5) {
    mexErrMsgTxt("Too few input! \n");
    return;
  };

  int ii,jj;
  int i,j,k;
  int ny,nx,N,Nc,depo,speedup,wind2d;
  int dlj,dli,h_avg,x0,y0,sx,sy,erode,shadowCheck;
  float pns,ps,dlcc,dlc;
  double *Zin,*Zout;
  int **Z;
  FILE *fg;

  int di[] = {1,1,1,0,-1,-1,-1,0};
  int dj[] = {-1,0,1,1,1,0,-1,-1};
  float dd[] = {1.4142,1.0,1.4142,1.0,1.4142,1.0,1.4142,1.0};


  /********** Read input ****************/
  Zin = mxGetPr(prhs[0]);
  ny = mxGetM(prhs[0]);
  nx = mxGetN(prhs[0]);
  N = nx*ny;
  
  /* Default only Werner */
  speedup=0;
  dlj=0;
  dli=0;
  dlc=0;
  dlcc=0;

  /********** Read input depended variables ******/

  /* Pure Werner model */
  /* No speedup */
  if (nrhs == 6) {
    Nc = (int) (*mxGetPr(prhs[1]));
    dlj = (int) (*mxGetPr(prhs[2]));
    pns = (float) (*mxGetPr(prhs[3]));
    ps = (float) (*mxGetPr(prhs[4]));
    shadowCheck = (int) (*mxGetPr(prhs[5]));
  };

  /* Werner + wind speedup */
  if (nrhs == 9) {
    Nc = (int) (*mxGetPr(prhs[1]));
    dlj = (int) (*mxGetPr(prhs[2]));
    pns = (float) (*mxGetPr(prhs[3]));
    ps = (float) (*mxGetPr(prhs[4]));
    dlc = (float) (*mxGetPr(prhs[5]));
    dlcc = (float) (*mxGetPr(prhs[6]));
    h_avg = (int) (*mxGetPr(prhs[7]));
    speedup = 1;
    shadowCheck = (int) (*mxGetPr(prhs[8]));
  };

  if (nrhs == 7 || nrhs > 9) {
    mexErrMsgTxt("Wrong number of input! \n");
    return;
  };

  /********** Allocate and initiate ********/
  /*allocate*/
  Z = imatrix(0,ny-1,0,nx-1);

  /*initialize*/
  for (i=0;i<ny;i++)
    for (j=0;j<nx;j++) 
      Z[i][j] = (int)Zin[j*ny+i];

  /********** Loop cykles ******************/  
  for (k=0;k<Nc;k++) {

    /*perform cycle*/
    for (i=1;i<=N;i++) {

      /*random row number*/
      ii = rand()%ny;

      /*random column number*/ 
      jj = rand()%nx;
    
      /* should I do erosion in shadow? */
      erode = 1; /* Default Werner setup */
      if (shadowCheck) {
	erode = (inshade(Z,nx,ny,&ii,&jj,dlj,dli)==0) ? 1 : 0;
      };

      /*if sand exist and not in shadow zone*/
      if ((Z[ii][jj] > 0)&&(erode)) {
	
	/*erode*/
	Z[ii][jj] -= 1;
	avalance_ero(Z,ny,nx,&ii,&jj,di,dj,dd);

	depo = 0;
	while (depo == 0) mad(Z,ny,nx,pns,ps,dlj,&ii,&jj,&depo,di,dj,dd,speedup,dlc,dlcc,h_avg,wind2d,dli);
	
      }/*if sand*/

    }/*i*/

  }/*k*/

  /********** Output *************/
  plhs[0]=mxCreateDoubleMatrix(ny,nx,mxREAL); /*allokering af output.*/
  Zout=mxGetPr(plhs[0]);
  for (i=0;i<ny;i++) for (j=0;j<nx;j++) Zout[ny*j+i] = (double) Z[i][j];

  return;

}

