Aeolian-Modeling
================

Implementations of the Werner's and Momiji's Algorithm used to simulate the creation and migration of sand dunes.

The following code has not been cleaned and may contain paths that will not exist on all systems. It is therefore published to show how one can integrate Matlab, C and Blender to create scientific animations. To run the code please read and understand the functions and modify them to fit your system.

Code in action
---------------
The software has been used (among other things) to create the following animation: https://www.youtube.com/watch?v=tsNGcjPP0Zg

How to run computations
-----------------------
The code consists of a C-source file and a Matlab helper scripts. The C-file needs to be compiled to Matlab Mex code. The RunModels.m file will then call the MEX code and produce output files in a subdirectory. The files are simple png image files where the 2D height matrix is shown as a gray scale image.

How to create animations
----------------------------
The blender file links to the Displace_script.py file. The general idéer is to point the script to a folder containing the output files and it will then loop over all the files using the gray scale image as a displacement modifier on the "sand surface" in blender. It will generate a jpg output file for each frame that can later be stiched together into a movie.

Note
---------

All the published files are released under a GPL v3 license. Use them, change them, share them with everyone but please keep the free and open!

The software needs the nrutil.c and nrutil.h source that are part of the Numerical Recipes book series (http://www.nr.com/). They are avaible from there website.
The blender file also requires a texture for the sand and stone. I have used the following,

* Sand: http://www.kaneva.com/asset/assetDetailsLandingPage.aspx?assetId=7133933&communityId=0
* Stone: http://www.art.eonworks.com/free/textures/ground_texture_01-512x512.png

For more infomation see
________________________

- http://www.emin.geog.ucl.ac.uk/~hmomiji/
- http://web.ncf.ca/jim/sand/dunefieldMorphology/index.html
