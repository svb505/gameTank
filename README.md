## General: 
This is a **local tank game** where the goal is to destroy various enemies (tanks, houses). 
Each enemy has its own HP pool, and each shell deals damage.

## Features: **Positional sound** with attenuation, **kill chat**, **AI**, **Json configs** ,**modifications system**,**different shell types** (APFSDS, HE, Smoke, ATGM),
**APS**, **fire/smoke effects** through points, **destruction of buildings and tanks** when a shell hits or a tank crashes into them.

Small **AI** for enemy tanks that **turns the turret towards the player** if he is within range and **shoots at him**.
Tank crashes into objects and destroys them, **lighting**, a **minimap** with dynamic data (your position, enemies around, effects, etc.), 
**Arced projectile trajectory**, **ammo limitation**, the **ability to replenish ammo and repair tank** at a special point, **calling an artillery**
strike at specific coordinates with a margin of error, **data base** for kills and death. **APS**(Active Protection System) who destroy enemyes shells if they have 
a specific speed, and they are located in a specific position around the tank. **Key bind** thanks to which player can bind any key for any action.

## Technical nuances: 
  - Each logical system is implemented in a separate file
  - Entity-Component-System architecture for managing all enemyes. It works faster with the processor because the data is stored in lists,
    and such a system is also much easier to scale.
  - Object-Oriented Programming for tanks, shells, sound...
  - Json config to change values ​​without recompiling the code

## Libraries Used:
  - Renderer: gl, opengl, glfw3, glu
  - Sound: ALuint
  - GUI: ImGui
  - Data Base: SQLite3
  - Config: Json(nlohmann)
  - Other: chrono, cstdint, cstdlib, algorithm, ctime, stbimage

## Required:
  - For launch this game you need all .dll files in folder "DLLs"

## Demo:
![General Visual](images/f1.png "General game view")
![Destroyed building](images/f2.png "Destroyed building example")
![Effects](images/f3.png "Explosion and fire effects")
