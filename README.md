# First OpenGL Project

<img src="/sample_scenes/scene1.png" alt="scene1" title="scene1" width="500"/>

This was my Final Project for UCSD Math 155A (Geometric Computer Graphics) where I had to design a scene in OpenGL from scratch.


# Goals:

* Implement keyboard controls to move, zoom, and manipulate light.
* Design a scene using triangle meshes and parametric surfaces with modifiable mesh resolution.
* Texture mapping the triangle meshes and parametric surfaces by calculating the correct normals and texture coordinates.
* Add different kinds of lights, and demonstrate the impacts of ambient, diffuse, and specular lights.


# Scene Overview:

Inspired by my passion for Rubik's cubes, I created a room featuring a black table, a brass-textured lamp, and my treasured "Glow in the Dark Cube". In 2009, I took part in the “Rubik’s Cube Competition Seoul 2009” in South Korea. To commemorate that, I displayed the competition's official poster on the wall and placed a Speedcube timer on the table, showcasing my official record of 32.16 seconds. Both the lamp and table legs, designed as parametric surfaces with triangle meshes, offer adjustable mesh resolutions via the 'M/m' keys.

# Features:

* **Navigation**: Use arrow keys.
* **Zooming**: 'HOME' (closer) and 'END' (farther).
* **Mesh Resolution**: Adjust using 'M' (increase) or 'm' (decrease).
* **Light Control**s:

  '1': Room light  <br />
  '2': Lamp light  <br />
  '3': Lamp light in “rainbow mode”
  
* **Viewing Modes**:

  <img src="/sample_scenes/wiremode.png" alt="wireframe" title="wireframe" width="300"/>  <br />

  'w': **Wireframe mode** (I suggest adjusting the mesh resolution in this mode; the changes become very evident!)  <br />
  'e': Emissive light (in this case, the cube).  <br />
  'a': Ambient light.  <br />
  'd': Diffuse light.  <br />
  's': Specular light (noticeable on the lamp head).  <br />

# Launching the Program

* Clone the repository.
```
git clone https://github.com/jayHuggie/First_OpenGL_Project.git
```
* Navigate to the "Release" directory.
* Open “FinalProject.exe” to run the program!

# More sample images!

<p float="left">
  <img src="/sample_scenes/scene2.png" width="300" />
  <img src="/sample_scenes/scene3.png" width="300" /> 
</p>
