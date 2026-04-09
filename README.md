# __AquaRay Raytracing Renderer for Unreal Engine 5.2.1 (WIP)__
## Version 0.10.0
A Custom Realtime Raytracing Renderer for Unreal Engine 5.2.1 Built for my Upcoming Project(s)<br/>
SAMPLE UPROJECT: [Download (MediaFire)](https://www.mediafire.com/file/4nv9wtncbtahgmb/Aquaray_Demo.zip/file)<br/>
__Note: The Sample Project does not contain any Raytracing code. Its only a Test Scene with Example Materials and Meshes, And a Controller Actor for Interacting with the Shader Settings. Please Refer to the Installation Section for Installing the Raytracer<br/>__
Also read the [Wiki](https://github.com/Dechetmew/Aquaray-Raytracer-for-Unreal-5.2.1/wiki) when you finish Installation<br/>
![alt text](yay/logo.png)

## Scope of Project
- Whitted Style Raytracing<br/>
- Direct Lighting With Point Light Support<br/>
- Simple One-Raypath Shadows<br/>
- Reflections & Translucency up to 10 steps<br/>
- Reflected Direct Lighting<br/>
- Simple Fog<br/>
- Water<br/>
- Performance Target: 60fps at 1440x1080 Resolution on Nvidia RTX 3050 (at Native Resolution with No Temporal Accumulation, Upscaling or FrameGen)<br/>
(also some silly lighting hacks, not too important)<br/>

This is a Simple Whitted Style Raytracing Renderer built into Unreal Engine, slowly developing into a Functional Tool to fuel my passion towards Raytraced Visuals and Art. I have decided to split this project into two Stages:<br/>
- Aquaray 1 - Single Shader Whitted style Raytracing, Resemblant of Bryce 3D visuals<br/>
- Aquaray 2 - Will include more Standard features like Proper Indirect Lighting, and other features needed for Making games or Advanced Renders<br/>

GPU Requirements:<br/>
- DirectX 16 Support<br/>
- Hardware Raytracing Support<br/>

The sample project runs around 60-120 fps on my RTX 3050, The Framerate is mostly based on how much is on the screen. (Also avoid using that AO Sampler thing I made in the past)<br/>
Right now the Shader is running as a Debug Layer, I will move the code Directly into the Renderer somewhat soon<br/>

## Update V0.10.0
- Recentralized Light Sampling code to fix Lighting Issues<br/>
- Also Other Lighting Improvements<br/>
- All Color units now are in Kilolux (1.0f = 100 Kilolux)<br/>
- Spot Lights now have Unlimited angle to almost 180 degrees<br/>

Check changelog.txt for more info<br/>

## __How to Install__
1. Install Unreal Engine 5.2.1 Source  if you havent already<br/>
Should be 110GB in size, make sure to install on a fast drive, or you will be waiting DAYS for it to compile
https://www.unrealengine.com/en-US/ue-on-github<br/>
https://dev.epicgames.com/documentation/en-us/unreal-engine/building-unreal-engine-from-source<br/>
2. Compile an Umodified Build of 5.2.1 and test if its all working<br/>
https://github.com/EpicGames/UnrealEngine/tree/5.2.1-release<br/>
3. Replace/add files from this repository to your Engine Source<br/>
NOTE: For RayTracingDebug.cpp you must find the if statement for enabling the PrimaryRays viewmode in your source, and Replace it with the version on this Repository. Similiar for DeferredShadingRenderer.h<br/>
Other files should be only Replace/Add<br/>
4. Recompile the Engine Again<br/>
5. Download the AquaRay sample Project from this Repository and add it to your Unreal Projects folder<br/>
6. If all stars Align, you should (hopefully) load into the project without any Explosions or Crashes<br/>
Load into "emptytestmap" and Click the AquaRayController Actor in the Outliner<br/>
In the Details Panel, Click "Apply Settings", and "Enable Aquaray"<br/>

After Clicking the button of Doom, you will be either Greeted by a "D3D Crashed" window, or a Fantastic Raytraced Scene in your Viewport<br/>

## __Credits__
The Sample project uses:<br/>
-Skyboxes from [PolyHaven](https://polyhaven.com/)<br/>
-Some Models by the [Stanford University](https://graphics.stanford.edu/data/3Dscanrep/)<br/>

## Gallery
![alt text](yay/reflections.png)

![alt text](yay/glass.gif)

![alt text](yay/models.png)

![alt text](yay/lights.png)
