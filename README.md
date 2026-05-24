# __AquaRay Raytracing Renderer for Unreal Engine 5.2.1 (WIP)__
A Custom Realtime Raytracing Renderer for Unreal Engine 5.2.1 Built for my Upcoming Project(s)<br/>
![alt text](yay/logo.png)

## Update 11.1
- New versioning<br/>
- Improved sky fallback, Removing Composite Opacity<br/>
- Fixed the Previously Fully messed up Fresnel Approach, Improving Lighting on Dielectric Mirror, Metallic Mirror, Water and Glass Shading Models<br/>
- Water Absorbtion Removed From Shading Model, Water will be redone soon (Hopefully)<br/>
- Volumetric Translucent absorption is now driven only by Base Color<br/>
- Translucent Materials now casts colored Shadows<br/>
- Split Volumetric Translucent and Thin Translucent<br/>
- Reverted the Raypth/Composite structure due to large compilation times<br/>
- Fixed glass sky reflection not respecting sky brightness<br/>
- Fixed Thin Translucent Shadows not Working<br/>
- Normal in Thin Translucent seems to be working now<br/>

Check changelog.txt for more info<br/>

## Current Features
- Whitted Style Raytracing<br/>
- Direct Lighting With Point Light Support<br/>
- Simple One-Raypath Shadows<br/>
- Reflections & Translucency up to 10 steps<br/>
- Reflected Direct Lighting<br/>
- Simple Fog<br/>
- Water<br/>

__Performance Target:__ 60fps at 1440x1080 Resolution on Nvidia RTX 3050 (at Native Resolution with No Temporal Accumulation, Upscaling or FrameGen)<br/>
__Current Performance Result:__ 70fps in Medium Complexity scenes. Shaders are currently compiled without optimization so the Final result may run faster<br/>

__GPU Requirements:<br/>__
- DirectX 16 Support<br/>
- Hardware Raytracing Support<br/>

## Going Forward
__Phase 1:__<br/>
- This is phase one of this project, here Im experimenting with Raytracing, Rendering tricks and Identifying the needs for a full pipeline<br/>
- One-Shader Implementation without Indirect Lighting<br/>
__Phase 2:__<br/>
- The following phase will be full push towards a full production ready Raytracing Renderer<br/>
- New Unreal Engine fork repo with Renderer already embeded<br/>
- Low Resolution Indirect Lighting pass<br/>
- Translucency Reflection Juniction<br/>
- Roughness Support<br/>
- DoF and Custom Post Processing pass with Tonemapping and Antialiasing<br/>
- Optional Checkerboard Rendering mode for devices below target spec<br/>

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

Also read the [Wiki](https://github.com/Dechetmew/Aquaray-Raytracer-for-Unreal-5.2.1/wiki) when you finish Installation<br/>

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
