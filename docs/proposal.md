Project Doomed: Team members Ehrlinspiel, Björn and Oesterle, Max
Game Idea: 3D-Maze Escape game using portals. You spawn in a maze and have to find the exit (or a safe place) in a given time. From a safe place you can then continue your search for the exit.
Portals can lead you into different parts of the maze. They shall support light transport and shadows, they are supposed to look like normal windows into an "impossible room" (maybe with some effect on it). We try to create a tense, realistic atmosphere of a maze using raytracing, many lights and some kind of relief mapping.

M = Max Oesterle
B = Björn Ehrlinspiel

Milestone 1: Basics
- setup and open window (M)
- phong shading (M)
- load objects and textures (M)
- wireframe rendering (B)
- imgui menu (B)
- generate/create simple world (later a node in graph)

Milestone 2: World
- camera movement using WASD and Mouse (B)
- generate/create simple graph based worlds, hierarchical data structures for nodes, e.g. BVH (B and M for designing boundaries between nodes which are similar to portals)
- multithreading, game loop (M)
- deferred shading approach (M)
- envmap (M)


Milestone 3: Game
- Implement portals extending the rendering approach of Milestone 2. Portals shall behave like windows (regarding shadows). (M)
- normal, relief mapping (respectively steep parallax occlusion mapping?) (M)
- game state (timer, visible map), entities (B)
- simple game logic (deadline to exit maze) and story (B)

Milestone 4: Fancyness
- AO (optional many lights, see below) (M)
- Add different lights in maze and implement a dynamic behaviour of the lights (e.g. turn on when player passes the first time, adjust intensity according game mechanics like time left) (B)
- Add multiple stages until exit is reached to game logic

Optional:
- many lights (M)
- day/night cycle (B)
- Additional game logic, story (M, B)
- Addiitonal objects to interact with (M, B)
- HDR, Bloom (M)
- New puzzle-like scenes (B)


Libraries / APIs:
- IMGui
- OpenGL
- glad
- glm
- Assimp


Tutorials
- https://www.glfw.org/docs/3.0/quick.html
- http://www.opengl-tutorial.org/beginners-tutorials/tutorial-1-opening-a-window/ and following
- https://learnopengl.com/
-https://developer-blog.net/professionelles-loggen-unter-c/

Papers
- http://www.cemyuksel.com/research/lgh/real-time_rendering_with_lgh_i3d2019.pdf

Resources
- open source object models and textures