# Blade Dash
I designed and developed an action-RPG style first-person open-world game, using C++, realistic environmental design, and dynamic gameplay mechanics to showcase UE5 knowledge.

Tags: C++👩‍💻, Solo project 👤, Unreal ⚙️, Video Game 🎮

## Summary
In my latest project, I utilized Unreal Engine 5 (UE5) to create an immersive action-RPG game from the ground up. The game features an expansive open world filled with ultra-realistic environments made possible by Quixel Bridge megascans, detailed level design, and interactive elements like breakable objects and treasure spawns. My role involved not only scripting complex game mechanics such as combat systems and enemy AI behaviors but also implementing advanced animation techniques and optimizing game performance for a seamless player experience.

![Screenshot 2024-10-07 125548](https://github.com/user-attachments/assets/c02ebfa2-1425-4f87-b030-c14062167ef6)

# Project overview

## General

- Designed and implemented an action-RPG style game in UE5, focusing on **open world development**
- Utilized C++ to script game behavior, including enemy AI, character controls, and interactive game elements like breakable objects and treasure spawning

[Take a look at the videos here](https://github.com/GeorgiaSamaritaki/BladeDash/tree/main/Screenshots/videos)

## Enemy Patrolling and Motion Wrapping for Enemies

- Implemented and edited inverse kinematics, animation graphs with motion warping to enhance the realism of character movements and enemy interactions.
    - Blueprint Thread Safe Animations
    - Retargeted skeletal meshes, used Animation Rigs, created and used sockets 
    - Created animation montages (click on the arrow for the video)
- Used blueprints extensively but with the focus of translating concept to C++.
    - Binded ue5 delgates
- Created dynamic overlays for character and enemy attributes
    - [Attributes class](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Source/SlashWorld/Public/Components/AttributeComponent.h)
- Created pretty effects using Niagara.
- Used cutting edge features tools and systems such as **Motion Warping** for dynamic animations, **Meta Sounds** for high-quality in-game audio, Packed Level Instances for efficient level design and implemented **enhanced input control** for dynamic binding and migrating to different platforms. ****

![Niagara system for Soul collectible](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Screenshots/NiagaraSystemSouls.png)


## Technical Skills and Programming:

- Programmed **core gameplay elements** like combat mechanics with swords and other melee weapons, health and stamina management, and enemy behavior patterns.
    - Implemented enemy and character states in C++
    - Ray tracing for sword attacking
- Leveraged advanced C++ concepts within UE5, including class hierarchy manipulation, use of pointers for efficient memory management, template programming for reusable code, and using interfaces.
- Applied UE5's **animation tools** to create lifelike character movements and combat reactions, alongside particle effects

## Game Design and Development:

- Learned to **level design**. Sculpted and painted diverse landscapes, integrating an entire dungeon level, to construct a captivating open-world environment for players to explore.
- Engineered a **variety of enemies** with distinct behaviors and attributes, alongside a **detailed player character** equipped with progressively powerful weapons and abilities.
    - [base character class](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Source/SlashWorld/Public/Characters/BaseCharacter.h) (both enemy and character inherit from)
    - [enemy class](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Source/SlashWorld/Public/Enemy/Enemy.h)
    - [main character class](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Source/SlashWorld/Public/Characters/SlashCharacter.h)
    - Established a pipeline to add enemies easily based on the enemy class (see [project notes](https://www.notion.so/Blade-Dash-c3b37f1fa8cc405781d3d4dfd7db051f?pvs=21) #226)
- Developed a dynamic game world with free-roaming creatures, humanoids, and breakable objects that enrich the gameplay experience.
    - Enemies can patrol and can be triggered to attack
    - Objects can be broken for collectibles

## Combat and collectibles

**Minor**

- Applied vectors and trigonometry to gameplay programming, enhancing the realism and functionality of game mechanics.
- Adopted best coding practices, design patterns, and data structures to ensure clean, maintainable, and efficient code.
- Had a lot of fun 💃

# Resources

[BladeDash/Notes.md at main · GeorgiaSamaritaki/BladeDash](https://github.com/GeorgiaSamaritaki/BladeDash/blob/main/Notes.md)
