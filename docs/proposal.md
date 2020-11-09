Project Earthbender: Team members Ehrlinspiel, Björn and Oesterle, Max. Rendering using non-euclidean geometry. Truely hyperbolic or spheric geometry rendering is targeted with fallback to normal rendering using tricks like virtual cameras to give the illusion of non-euclidean spaces. Generation of worlds using tile mapping.

Milestone 1:
- open window
- continuous build setup
- render simple objects
- flat world generation (tiles)

Milestone 2:
- simple camera movement using WASD and Mouse
- tile generation in euclidean space
- understand the mathematics of gyrovectors and develop idea for coordinates and transformations/projections (Vectors don't work in non-euclidean spaces. There are two different points (1, 1) and six different points (1, 1, 1))

Milestone 3:
- Try to implement the rendering of a non-euclidean flat world. If it turns out, that truely non-euclidean rendering is not feasible for this practical, implement portal style illusions of non-euclidean geometry, but avoid teleportations. Use virtual cameras instead.

Milestone 4:
- If truely non-euclidean rendering works
    - Simple objects creation and loading. Standard object models will probably not work because we don't render in a vector space.
- If we used portal-style rendering
    - tandard object loading and rendering 
    - create multiple scenes with different applications of portals
    - implement recursive rendering of portals which are visible within a portal

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

(Resources
- 