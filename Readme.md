to do

- create design for keyboard mouse interaction [COMPLETE]

- write code for testing

- create design doc

- implement default mouse, key action

- test mainEngineControl class

issues

- for addtexture it is not bounded to an vao so it might not work, a work around may be adding a new TEX type in sim and in the use function if it is a TEX we will call the glBindTexture function

- currently what i want is that when I share the mesh for baseobject I can still use different seperator and locs to interperate the mesh in different way, but the mesh is attached to the vb so I thought I might need to do a vertexattribpointer everytime I want to draw, I think I may overcome this issue by assigning different va to the baseobject | from current test I think this is no more a issue

