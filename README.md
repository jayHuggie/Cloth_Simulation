# Cloth Simulation

<img src="/scenes/cloth_video.gif" alt="scene_overview" title="scene_overview" width="500"/>

This was my Project for UCSD CSE 169 (Computer Animation) where I had to design a scene in OpenGL from scratch.


# Goals:

* Implement keyboard controls to move, zoom, and manipulate light.
* Design a scene using triangle meshes and parametric surfaces with modifiable mesh resolution.
* Texture mapping the triangle meshes and parametric surfaces by calculating the correct normals and texture coordinates.
* Add different kinds of lights, and demonstrate the impacts of ambient, diffuse, and specular lights.


# Scene Overview:

Inspired by my passion for Rubik's cubes, I created a room featuring a black table, a brass-textured lamp, and my treasured "Glow in the Dark Cube". In 2009, I took part in the “Rubik’s Cube Competition Seoul 2009” in South Korea. To commemorate that, I displayed the competition's official poster on the wall and placed a Speedcube timer on the table, showcasing my official record of 32.16 seconds. Both the lamp and table legs, designed as parametric surfaces with triangle meshes, offer adjustable mesh resolutions via the 'M/m' keys.

# Features:


* Particle-Based Cloth Simulation

    Implements a cloth model using a system of particles, spring-dampers, and triangular surfaces.
    Simulates realistic cloth behavior with forces such as uniform gravity, spring elasticity, damping, and aerodynamic drag.

* Spring-Damper System

    Uses a spring-damper model to maintain the structural integrity of the cloth.
    Adjustable spring constants to control the stiffness of the cloth.
    Damping factor to regulate the oscillations and achieve a stable rest state.

* Aerodynamic Interactions

    Includes aerodynamic drag that affects the cloth’s movement in response to wind and other environmental factors.
    Provides controls to adjust ambient wind speed, allowing for dynamic simulations of cloth under different wind conditions.

* Ground Plane Collision Handling

    Simulates collisions between the cloth and a simple ground plane, ensuring the cloth reacts realistically upon impact.
    Implements restitution and friction to model realistic ground interactions.

* Fixed Particles for Cloth Manipulation

    Allows certain particles in the cloth to be fixed in place to simulate various cloth-hanging scenarios (e.g., corners fixed to simulate a hanging flag).
    User controls to manipulate the position of fixed particles, enabling real-time interaction and adjustment of the cloth’s configuration.

* Dynamic Normal Calculation and Shading

    Calculates triangle normals dynamically during each update for accurate physics simulation.
    Implements dynamic smooth shading by averaging normals at vertices, enhancing the visual realism of the cloth.

* Configurable Cloth Initialization

    Supports multiple initialization functions to configure different cloth sizes, material properties, and configurations (e.g., ropes, sheets).
    Easily adjustable parameters for experimenting with various simulation scenarios and material stiffness properties.

* Real-Time User Interaction

    Provides keyboard controls to adjust the position of fixed particles and influence the cloth's behavior in real time.
    Enables interactive simulation, allowing users to experiment with different configurations and observe their effects immediately.


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
