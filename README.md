# HairSimulation-OpenGL
## Cloning the project
This project uses git submodules, so clone with:  
```
git clone --recurse-submodules https://github.com/dominikcondric/HairSimulation-OpenGL.git
```

## About the project
Project contains a hair simulation written in C++/GLSL with minimal dependencies as part of an undergraduate thesis in Computer Graphics. Algorithm behind the hair simulation is called Follow The Leader, and it is closely related to the idea of simulating pysics using Position Based Dynamics. All of the hair computation is done in GLSL compute shader to benefit from paralellism while working on individual strands.

## Controls
**W** - moves camera in positive **z** direction of a scene camera  
**A** - moves camera in positive **x** direction of a scene camera   
**S** - moves camera in positive **z** direction of a scene camera   
**D** - moves camera in positive **x** direction of a scene camera   
**Spacebar** - moves camera in positive **y** direction of a scene camera   
**Left shift** - moves camera in positive **y** direction of a scene camera  
