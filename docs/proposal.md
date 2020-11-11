Project Doomed: Team members Ehrlinspiel, Björn and Oesterle, Max
Game Idea: 3D-Maze Escape game using portals. You spawn in a maze and have to find the exit (or a safe place) in a given time. From a safe place you can then continue your search for the exit. The maze's minimap is uncovered as you proceed to explorer more parts of the maze.
Portals can lead you into different parts of the maze. They shall support light transport and shadows, they are supposed to look like normal windows into an "impossible room" (maybe with some effect on it). We try to create a tense, realistic atmosphere of a maze using raytracing, many lights and some kind of relief mapping.

M = Max Oesterle
B = Björn Ehrlinspiel

Milestone 1: Basics
- setup and open window (M)
- load and render simple objects with texture, phong (M)
- wireframe rendering (B)
- imgui menu (B)
- generate/create simple world (later a node in graph)

Milestone 2: World
- camera movement using WASD and Mouse (B)
- generate/create simple graph based worlds, hierarchical data structures for nodes, e.g. BVH (B, M for designing boundaries between nodes which are similar to portals)
- multithreading, game loop (M)
- raytracing (M)
- envmap (M)


Milestone 3: Game
- portals supporting light transport/shadows (M)
- normal, relief mapping (respectively steep parallax occlusion mapping?) (M)
- game state (timer, visible map), entities (B)
- game logic and story (B)

Milestone 4: Fancyness
- many lights, lighting grid hierarchy (M)
- day/night cycle, lights in maze (B)
- add game logic, story, objects, light sources, HDR and bloom, new scenes as time allows (B, M)


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

Papers
- http://www.cemyuksel.com/research/lgh/real-time_rendering_with_lgh_i3d2019.pdf

Resources
- open source object models and textures