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
% This python script is ment to be used with Blender <www.blender.org> to read output
% files created by the c-program.
% Please edit the paths to fit your system!

import bpy
import os, time, glob

#pwd = os.getcwd()
OutDir = 'Momiji/Blender/2D/Linear/'
pwd = 'Momiji/HeightMap/Linear_15/'

ob = bpy.data.objects["DesertBlok"]
mods = ob.modifiers
#ob.modifiers.items()

dis = ob.modifiers["Displace"]
#print(dis.items)
#dis.items

# Camera
cam = bpy.data.objects["Camera"]

tex = bpy.data.textures["Texture"]
tex.image

# Animation
bpy.ops.anim.keying_set_add()


# Get scene
scene = bpy.data.scenes["Scene"]
bpy.data.scenes["Scene"].camera = cam

# Allocate
c=1
date_file_list = []

# Order files
for file in glob.glob(pwd+'*png'):
    stats = os.stat(file)
    lastmod_date = time.localtime(stats[8])
    date_file_tuple = lastmod_date, file
    date_file_list.append(date_file_tuple)

date_file_list.sort()

for name in date_file_list:
    if (c>=174):
        print(name[1])
    
        # Update frame count
        bpy.data.curves["Text"].body = 'Cycle = '+str(c)
    
        # Set next frame
        scene.frame_set(c)
        bpy.data.scenes["Scene"].render.file_format = 'PNG'
        bpy.data.scenes["Scene"].render.filepath = OutDir+'render_'+str(c)

        # Texture displace setup
        IMG = bpy.data.images.load(name[1])
        tex.image = IMG
        
        bpy.ops.render.render( write_still=True )

    c=c+1

print('Done!')
