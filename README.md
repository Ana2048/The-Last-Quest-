# The Last Quest

**The Last Quest** is a modern OpenGL project that builds a hybrid 3D world inspired by **The Witcher 3: Wild Hunt** and **Cyberpunk 2077**.  
The player takes control of **Geralt of Rivia** and travels through two distinct environments connected by a magical portal: a forest world and a futuristic cyberpunk city.

---

## Overview

This project was developed for a **Computer Graphics** course and focuses on combining core real-time rendering techniques with a game-inspired interactive experience.

The application includes:

- two distinct 3D scenes
- first-person camera movement
- advanced lighting
- shadow mapping
- procedural animation
- a 2D dialogue overlay system
- a cinematic presentation mode

The main goal was not only to render a visually interesting world, but also to implement the underlying graphics concepts in a clean and modular way.

---

## World Design

### Forest Scene
The first scene is a natural environment with:
- directional lighting
- local torch lights
- atmospheric fog
- an interaction point with **Yennefer**
- a portal used to travel to the second scene

### Night City Scene
The second scene is a futuristic urban environment with:
- multiple colored point lights
- cyberpunk-inspired atmosphere
- an urban skybox
- a procedurally animated flying car

The contrast between these two scenes is one of the central visual ideas of the project.

---

## Main Graphics Features

### Matrix-based camera orientation
Instead of relying only on a classical yaw-pitch approach with manual vector reconstruction, the camera orientation is updated through **incremental matrix transformations** using GLM.

This makes the implementation:
- cleaner
- more modular
- easier to extend
- numerically stable after re-orthonormalization

### Blinn-Phong lighting
The project uses **Blinn-Phong shading** for the specular component instead of the classic Phong reflection model.

This provides:
- smoother highlights
- more stable specular reflections
- better results at wider viewing angles

### Point light attenuation
Torch lights and city neon lights use **quadratic attenuation**, which improves realism by making the light intensity decrease naturally with distance.

Different attenuation coefficients are used depending on the scene:
- shorter range in the forest
- wider colored light influence in Night City

### Shadow mapping
Dynamic shadows are implemented through a **two-pass shadow mapping pipeline**:

1. render scene depth from the light's perspective
2. render the final scene using the depth map to determine shadowed fragments

The implementation includes:
- scene-dependent orthographic light projection
- light-space matrix computation
- bias correction to reduce shadow acne artifacts

### 2D dialogue rendered over a 3D scene
A dedicated **dialogue system** renders a fullscreen textured quad directly in **Normalized Device Coordinates (NDC)**.

Important details:
- the UI is rendered independently of the 3D camera
- depth testing is disabled during dialogue rendering
- alpha blending is enabled for PNG transparency
- texture flipping is handled carefully with `stb_image`

### Procedural animation
Several scene elements are animated procedurally using trigonometric functions and elapsed time.

#### Flying car
The flying car in Night City uses:
- sinusoidal motion on the Z axis
- hovering motion on the Y axis
- automatic yaw adjustment based on movement direction

This avoids the need for keyframe animation and creates smooth continuous motion.

#### Portal
The portal animation combines:
- slow pulsing
- fast wobble
- time-based scaling

This creates an unstable magical effect and makes the portal feel more alive.

### Cinematic mode
The application includes a cinematic camera mode that automatically moves the camera on a circular path around the scene.

Features:
- automatic orbit movement
- pause functionality
- useful for scene presentation and screenshots

---

## Controls

| Key | Action |
|------|--------|
| **W, A, S, D** | Move through the scene |
| **Mouse** | Control camera direction |
| **E** | Interact with Yennefer / use the portal |
| **N** | Switch render mode |
| **F** | Toggle fog |
| **V** | Toggle cinematic mode |
| **P** | Pause cinematic mode |

### Render Modes
The application supports multiple rendering modes:
- **Solid**
- **Wireframe**
- **Points**

These are useful for debugging and for visualizing scene geometry.

---

## Project Structure

The project follows a modular architecture, with separate classes for the major engine components:

- **Window** – manages window creation and OpenGL context
- **Camera** – handles movement, orientation, and view matrix logic
- **Shader** – loads, compiles, and links GLSL shaders
- **Model3D** – loads `.obj` geometry and materials
- **SkyBox** – handles cubemap-based background rendering
- **Dialog** – renders the 2D dialogue system above the 3D world

---

## Technical Highlights

Some of the most important implementation ideas in this project are:

- modular OpenGL application structure
- scene switching through interaction logic
- matrix-based camera rotation
- Blinn-Phong specular lighting
- point light attenuation
- shadow mapping with adapted light projection
- procedural object animation
- overlay UI rendering in NDC space
- cinematic state-based camera control

---

## Assets

This project uses external assets such as:
- 3D models
- textures
- skybox textures

Some large source files were excluded from the repository due to GitHub size limitations.

---

---

## Credits

This project uses external assets for educational purposes.  
All rights belong to their respective creators:

### 3D Models
- 🎮 **Yennefer of Vengerberg**  
  https://sketchfab.com/3d-models/yennefer-wch-9d844f4b270d4a2eb96bd03262642aa0

- 🚗 **Cyberpunk Hovercar**  
  https://sketchfab.com/3d-models/free-cyberpunk-hovercar-3205b1075bb44ffc826bce0c2a04d74c

- 🏙️ **City Buildings Pack**  
  https://sketchfab.com/3d-models/full-gameready-city-buildings-0545317292c44490af5408cb58633121

### Textures
- 🌌 **Skybox (Humus)**  
  https://www.humus.name/index.php?page=Textures

---

**Note:**  
All assets are used strictly for academic purposes as part of a Computer Graphics project.

## Future Improvements

Possible future extensions include:

- collision detection using AABB
- animated characters
- more advanced interaction mechanics
- story progression
- improved asset optimization
- post-processing effects
- better environment detail and polish

---

## Academic Context

This project was created as part of a **Computer Graphics** course and demonstrates the practical use of:

- geometric transformations
- shader programming
- real-time lighting
- shadow mapping
- camera systems
- procedural animation
- 2D interface rendering over 3D content

---



## Author

**Ana Pastin**  
Faculty of Automation and Computer Science  
Technical University of Cluj-Napoca
