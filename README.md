# GameX-LevelEditor
#### 3D Level Editor in C++ using OpenGL 3.3 - SDL2.0 - CEGUI 0.8.5
#### Video:[GameX Level Editor](https://youtu.be/zSfcjB-RDEo).
#### Video:[Navigation Mesh Video](https://youtu.be/aZXywPTnU34).

## WHAT CAN THIS SOFTWARE DO?
- Place any 3D Model (in any orientation or scale) or use one of the preset models like Box and Sphere to make your level and add levels to your own 3D game.
- Create terrain using your heightmaps or use my random generator to create your plane then a heightmap will be saved for you.
- Add point lights or manipulate the directional light vector.
- Load skyboxes into your scene.
- Add your custom shaders/textures.
- There are hotkeys to control the software easily.
- Water is hidden by default but its generated with terrain width and depth.
- Make Navigation Mesh and Test Shortest Path using A* Algorithm.
## WHY THIS SOFTWARE MIGHT NOT BE YOUR CHOICE ?
- This level editor is done using forward shading which is heavy on performance(in a scene with alot of lights) but easy to implement and easy for beginners to try different shaders on their levels easily.
- Moving the camera is by arrows and WASD (that will be improved on next update or so).
- Colliders are not by color ID which means i had to do ray - physical object intersection and that resulted in on rotation of objects you might not pick the object from certain directions.
- Euler rotation is used (not quaternion).
## WHAT I THINK OF DOING ON NEXT UPDATE?
1. Bloom on point light.
2. deffered shading instead of forward.
3. shadow artifacts:
   - Dir light only (dont want to kill performance more than that)
   - Shadows are shown on plane only.
   - Shadow visible for all objects in shadow ortho. Box.
   - Shadow artifacts on  farthest model rotation.
   - Shadows are not bounded to dirLight.
4. Fix Sphere indexing.
5. Perform rotation about z-axis
6. Models collider are fixed AABB size at origin of models.(fix)
7. AABB collider doesnt change on rotation.
8. rotate camera using mouse not arrows.
9. Random Generator needs some tweaks.
10. Multithreading for Assimp and Heavy Tasks like generating random terrain.
#### if you know anything that might help me or improve the project email me emaranasr@gmail.com with subject "GameXLevelEditor Fix"
