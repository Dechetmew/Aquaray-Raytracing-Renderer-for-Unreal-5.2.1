# __AquaRay Raytracing Renderer for Unreal Engine 5.2.1 (WIP)__
A Custom Realtime Raytracing Renderer for Unreal Engine 5.2.1 Built for my Upcoming Project(s)<br/>
![alt text](yay/logo.png)

## Update 12
- Many Lighting Fixes and Improvements<br/>
- Some bugs fixed and Structural Changes<br/>
- Translucent Materials now can cast Colored Shadows<br/>
- Removed Global Water Fog Effect<br/>
- Now an Exclusion zone can be set around Point Lights using the "Source Radius" Value<br/>
- World Light tracing now turns off when set to 0,0,0<br/>
- Precision setting Removed<br/>

Check changelog.txt for previous versions<br/>

## Current Features
- Whitted Style Raytracing<br/>
- Direct Lighting With Point Light Support<br/>
- Simple One-Raypath Shadows<br/>
- Reflections & Translucency up to 10 steps<br/>
- Reflected Direct Lighting<br/>
- Simple Fog<br/>
- Water<br/>

__Performance Target:__ 60fps at 1440x1080 Resolution on Nvidia RTX 3050 (at Native Resolution with No Temporal Accumulation, Upscaling or FrameGen)<br/>
__Current Performance Result:__ 75-80fps in Medium Complexity scenes. Shaders are currently compiled without optimization so the Final result may run faster<br/>

__GPU Requirements:<br/>__
- DirectX 16 Support<br/>
- Hardware Raytracing Support<br/>

## Going Forward
__Phase 1:__<br/>
- This is phase one of this project, here Im experimenting with Raytracing, Rendering tricks and Identifying the needs for a full pipeline<br/>
- One-Shader Implementation without Indirect Lighting<br/>

__Phase 2:__<br/>
- The following phase will be a full push towards a production ready Raytracing Renderer<br/>
- New Unreal Engine fork repo with Renderer already embeded<br/>
- Low Resolution Indirect Lighting pass<br/>
- Translucency Reflection Juniction<br/>
- Roughness Support<br/>
- DoF and Custom Post Processing pass with Tonemapping and Antialiasing<br/>
- Optional Checkerboard Rendering mode for GPUs below target spec<br/>

QA:<br/>
__Why is this not a Plugin?:__ Overriding Vannila Unreal Rendering is Impossible using a Plugin. Any Renderer of this scale must be done via Modifying Unreal Source<br/>
__Can you Update the Renderer to a Higher Unreal Version?:__ I dont have a reason yet, but I think this code can be ported easily with (maybe) minimal changes up to (as of writing) 5.8.0.<br/>

## __How to Install__
Follow the wiki for Instalation<br/>
[Install: Unreal Engine Source](https://github.com/Dechetmew/Aquaray-Raytracing-Renderer-for-Unreal-5.2.1/wiki/2:-Install:-Unreal-Engine-Source)<br>
[Install: Fixing Existing Engine Issues](https://github.com/Dechetmew/Aquaray-Raytracing-Renderer-for-Unreal-5.2.1/wiki/3:-Install:-Fixing-Existing-Engine-Issues)<br>
[Install: Installing AquaRay](https://github.com/Dechetmew/Aquaray-Raytracing-Renderer-for-Unreal-5.2.1/wiki/4:-Install:-Installing-AquaRay)<br>

## __Credits__
The Sample project uses:<br/>
-Skyboxes from [PolyHaven](https://polyhaven.com/)<br/>
-Some Models by the [Stanford University](https://graphics.stanford.edu/data/3Dscanrep/)<br/>

## Gallery
![alt text](yay/reflections.png)

![alt text](yay/glass.gif)

![alt text](yay/models.png)

![alt text](yay/lights.png)
