# Roller Coaster Simulator

#### Authors: Liam Walker, James Stewart

## Overview

This project demonstrates a 3D roller coaster simulator game. The primary goals of the project were to experiment with animation in OpenGL, and implement a basic physics model.

## Structure

All of the code is contained in src/ except for the shaders which are located in data/. Similarly, the assets are located in data/ which include textures, maps and scene files. The entry point is defined in main.cpp while most of the 'core' code is executed in scene.cpp. Several different entities make up the scene including the terrain, coaster and tracks. 

## Techniques

The roller coaster tracks are formed in a dynamic manner based on the position of various posts which can be placed by the player. Using the position of these posts, an interpolation is calculated to connect each track segment. Different change of basis matrices can be used for different tracks. The terrian is drawn using a height map texture. A corresponding texture is overlayed to give the appearance of a mountain range. An alternate terrain consisting of an animated water surface was also developed. 

## Output 
The follwing images show the output of the program with the water surface and the mountain region.
![coaster](https://user-images.githubusercontent.com/37026953/119866501-e0519600-beea-11eb-9698-545d65e1f286.PNG)

![coaster2](https://user-images.githubusercontent.com/37026953/119865865-23f7d000-beea-11eb-8e94-1f1cb2b97008.PNG)

## Status (As of 2021-05-27)

This project is concluded for the time being, however several improvements can be made in the future. An environment map (skybox) would greatly improve the visual appearance of the scenes. Some refactoring should be done in the terrain.cpp files to make it easier to transistion between the water surface and mountain terrain, and add in new terrains that may not conform to the current constraints. I am currently developing a Vulkan Engine (github.com/lwalkk/VulkanEngine) which will allow for such features to be integrated more easily. 




