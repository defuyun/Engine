to do

- write code for testing [instruction should be updated later and decomposed into several parts, need to plan for testing]

- create design doc !!! crutial, please do this next time

- test mainEngineControl class [seems to be working fine with the defautl move and default look]


issues

- for addtexture it is not bounded to an vao so it might not work, a work around may be adding a new TEX type in sim and in the use function if it is a TEX we will call the glBindTexture function [seems the be working okay]

- currently what i want is that when I share the mesh for baseobject I can still use different seperator and locs to interperate the mesh in different way, but the mesh is attached to the vb so I thought I might need to do a vertexattribpointer everytime I want to draw, I think I may overcome this issue by assigning different va to the baseobject [from current test I think this is no more a issue]

- the starting direction of the camera doesn't seem to be looking at z axis but rather x axis, this can be observed when I use the cube test, I don't start looking at the cube but rather sidways. need to look into this
