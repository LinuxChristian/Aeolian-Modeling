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
% This file is used to run a lot of different setups. The main computations are
% done in the werner.c file. This matlab routine is just a wrapper to make running
% several tests easy.

clear all
close all
clc

% Constants
%sigma_x = 25;
%sigma_y = 25;
%A = 25;

%for i=1:x_size
%    for j=1:y_size
%    Z(i,j) = round((A-1)*exp(-((i-x_size/2).^2/(2*sigma_x.^2) + (j-y_size/2).^2/(2*sigma_y.^2)))+1);
%    end
%end
x_size = 500;
y_size = 500;
Z = ones(x_size,y_size)*5; 
%round((rand(x_size,y_size)*9)+1);
%Z(150:250,150:250) = ones(101,101)*25;
%Topo = zeros(size(Z));
%Topo(:,200:250) = ones(x_size,51)*15;

if 0
  % M. Momijt - Setup
  x_size = 500;
  y_size = 500;
  Z = ones(x_size,y_size)*100;
  dlc = 0.5;

for i=1:50
	Z(:,i+50) = Z(:,i+50)+i*2;
end

for i=1:25
	Z(:,i+100) = Z(:,i+100)+(100-i*4);
end
end

%plot(Z(200,:))

Nc=1;
dlx=5;
dly=(5:-1:0);
dlc = 0.4;
dlcc = 0.002;
shadowCheck = 1;
pns = 0.4;
ps=0.6;
runs = 3000;
sHeightMin = 4;
sHeightMax = 8;
direct = 1;
xDirection = 1;
yDirection = -1;
yIndex = 0;
debug=0;
disp('Starting run');
%colormap(gray)
%colormap([1 1 0]);
colormap([0.76 0.6 0.42]); % desert
%colormap([0.93 0.79 0.69]); % desert sand
%colormap([0.76 0.70 0.50]); % sand
%colormap([0.98 0.84 0.65]); % sunset
%colormap([255 224 100]./255);
h_avg = mean(mean(Z));

RotWind = 1;
RotPlot = 3;

%csvwrite('Matrix.txt',Z);
for i=1:runs
%    newdirec = round(rand*4);

if (RotWind==1)
  Z = rot90(Z,2);
  RotPlot = 3;
  else
%  Z = rot90(Z,0);
  RotPlot=0;
end
    
    if (yDirection>0)
       yIndex = yIndex-1;
    else
       yIndex = yIndex+1;
    end
        
    if (yIndex == 1)
        yDirection = -1;
    elseif (yIndex == length(dly))
        yDirection = 1;
    end
    
% No speedup or 2D wind | 5 parameters
%   Z = werner(Z,Nc,dlx,pns,ps,shadowCheck); 

% Only speedup | 7 parameters
 Z = werner(Z,Nc,dlx,pns,ps,dlc,dlcc,h_avg,shadowCheck);

% Only 2D wind | 6 parameters
% Z = werner(Z,Nc,dlx,dly(yIndex),pns,ps);

% Both 2D wind and speedup | 8 parameters
% Z = werner(Z,Nc,dlx,dly(yIndex),pns,ps,dlc,dlcc,h_avg);

     disp(['Iter ',num2str(i)]);%,' Wind direction [',num2str(dlx),',',num2str(dly(yIndex)),']']);
    
%    direct=4-newdirec;
if (RotPlot==0)
  RotWind=1;
 else
   Z = rot90(Z,2);
   RotWind=0;
end
    
 %   if (mod(i,100)==1)
        % Rotate back
     %   subplot(2,2,[1 2])
%       surfl(Z);
%hold on
%plot(i*100+Z(200,:),'k')

%view(2)
        %if (i==1)
%        axis off
        %hold on
        %contour(Z);
        %caxis([50 200]);
 %       lightangle(-90,15)
  %      annotation('textarrow',[0.1 0.7],[0.60 0.53],'string','Wind','FontSize',14);
   %     shading flat;
    %    view(15,87);
        %end
        %colorbar;
     %   title(['Iter ',num2str(i)],'FontSize',12,'FontWeight','bold')
        
      %  subplot(2,2,[3 4])
      %  plot(1:x_size,Z(250,:));
      %  ylim([950 1050])
      %  xlabel('Length');
      %  ylabel('Dune height');
        
      %  M(i) = getframe(gcf);
        
if (1)
        % Plot one line
	filename = 'Momiji/HeightMap/Linear/gray';
         if (i<10)
	   imwrite(uint8(Z),[filename,'_000',num2str(i),'.png'],'png');
	   %print('-dpng',[filename,'_000',num2str(i),'.png']);
         elseif (i<100)
	 imwrite(uint8(Z),[filename,'_00',num2str(i),'.png'],'png');
	 %print('-dpng',[filename,'_00',num2str(i),'.png']);
         else
	  imwrite(uint8(Z),[filename,'_0',num2str(i),'.png'],'png');
	  % print('-dpng',[filename,'_0',num2str(i),'.png']);
         end
end
	 % Write output
	% csvwrite(['WernerMatrix/Matrix',num2str(i),'.txt'],Z)
         
%    end
end

%hold on
%	 plot((runs+1)*100+Z(200,:),'r')
	 disp('Done')
%movie2avi(M,'Dunes2D.avi');
