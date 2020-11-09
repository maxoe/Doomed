Project Earthbender: Team members Ehrlinspiel, Björn and Oesterle, Max. Rendering using non-euclidean geometry. Truely hyperbolic or spheric geometry rendering is targeted with fallback to normal rendering using tricks like virtual cameras to give the illusion of non-euclidean spaces. Generation of worlds using tile mapping.

M = Max Oesterle
B = Björn Ehrlinspiel

Milestone 1: Basics for Flat World Rendering
- open window (M)
- render flat world, ray intersection with objects (M)
- flat euclidean world generation (tile mapping) (B)
- wireframe rendering
- basic materials lighting, implement phong model (B)
(- continuous build setup)

Milestone 2: A Working Flat World and Research for M3
- simple camera movement using WASD and Mouse (B)
- imgui menu (M)
- multithreading (game loop, ...)
- understand the mathematics of gyrovectors and develop an idea for coordinates and transformations/projections (vectors don't work in non-euclidean spaces because there are two different points (1, 1) and six different points (1, 1, 1)) (M, B)
- gyrovector math library

Milestone 3: Flat World --> Curved World
- Try to implement the rendering of a non-euclidean flat world. If it turns out, that truely non-euclidean rendering is not feasible this project, implement portal style illusions of non-euclidean geometry, but avoid teleportations. Use virtual cameras instead.

- If truely non-euclidean rendering works
    - adapt tile mapping (B)
    - fix rendering distance (M)

Milestone 4: Scenes and a More Complex World
- If truely non-euclidean rendering works
    - Simple objects creation and loading. Standard object models will probably not work because we don't render in a vector space. (B)
    - Switch world curvature in menu, transformations between worlds (M)

- If we used portal-style rendering
    - standard object loading and rendering (B)
    - create multiple scenes with different applications of portals (B)
    - implement recursive rendering of portals which are visible within a portal (M)
    - Shadows (e.g. Shadow Mapping) (M)


Libraries / APIs:
- IMGui
- OpenGL
- glad
- glm


Tutorials
- https://www.glfw.org/docs/3.0/quick.html
- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/ and following

Papers
- Ungar: Hyperbolic Trigonometry and its Application in the Poincaré Ball Model of Hyperbolic Geometry. 2000.
- Osudin, Child, Hui-He: Rendering Non-Euclidean Geometry in Real-Time Using Spherical and Hyperbolic Trigonometry. 2019.

Resources
- if rendering pseudo-non-euclidean open source object models and textures