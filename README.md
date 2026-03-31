##General: 
This is a **local tank game** where the goal is to destroy various enemies (soldiers, tanks, houses, etc.). 
Each enemy has its own HP pool, and each shell deals damage.

##Features: **Positional sound** with attenuation, **different shell types** (APFSDS, HE, Smoke), 
**explosion** and **fire effects** through points,**destruction of buildings** when a shell hits or a tank crashes into them, 
tank crashes into objects and destroys them, **lighting**, a **minimap** with dynamic data (your position, enemies around, effects, etc.), 
**Arced projectile trajectory**, **ammo limitation**, the **ability to replenish ammo** at a special point, and **calling an artillery**
strike at specific coordinates with a margin of error.

##Technical nuances: 
  - Each logical system is implemented in a separate file
  - Entity-Component-System architecture for managing all enemies
  - Object-Oriented Programming for tanks, shells, sound...

##Libraries Used:
  - Renderer: gl, opengl, glfw3, glu
  - Sound: ALuint
  - GUI: ImGui
  - Other: chrono, cstdint, cstdlib, algorithm, ctime, stbimage

##Demo:
![General Visual](images/f1.png "General game view")
![Destroyed building](images/f2.png "Destroyed building example")
![Effects](images/f3.png "Explosion and fire effects")
