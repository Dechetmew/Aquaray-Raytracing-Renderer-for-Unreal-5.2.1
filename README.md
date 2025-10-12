# __AquaRay Raytracer for Unreal Engine 5.2.1 (WIP)__
## V0.6.1
A Custom Hacked-In Hardware Accelerated Realtime Raytracer for Unreal Engine 5.2.1 Built for my Upcoming Game Project<br/>
SAMPLE PROJECT: https://mega.nz/file/DZgzFZKL#TDsautGjtkMpEODBVYFwjvYCzrVAfbX21R4qgzyx6nc
![alt text](yay/img1.png)

I was always fascinated by Raytracing for a long time, wanting to make a Game where the Environment is built Around Typical Raytracing Effects. After finding out that Lumen and all the Additional NVRTX stuff ~~sucks~~ is not satisfactory for my project, I decided to find a Way to Implement my own Raytracing code into the Engine that would suit the what I need<br/>

After YEARS of brute forcing a solution, I came up with this.... hack. It uses Unreal Engine's Raytracing Debug System to sneak in Custom Raytracing Passes, Running them using Console Commands<br/>

It should work in Unreal 5.2.1, but im not sure about other versions (But you can try :D)<br/>

Make sure you have a DirectX 16 SM6 Capable GPU With Hardware Acceleration support for Raytracing, the sample project runs around 60-120fps on my Geforce RTX 3050 (The Framerate is mostly Based on how much is on the Screen)<br/>

## Update V0.6.0
- Added Point and Spot Lighting<br/>
- Added Foliage Shading Model<br/>
- More Settings<br/>
- Code Improvements<br/>

### Patch V0.6.1
- Now Color is FINNALY getting Applied Back to Front, which will make Reflections and Translucency way easier to Implement<br/>
- Improved Specular<br/>
- Sky is now Considered a Shading Model in the Raytracer (which also opens up Possibility for Custom Sky Rendering Functions)<br/>
- And Many Changes, Cleanups, And Code Improvements I forgot to Document<br/>

## __How to Install__
1. Install Unreal Engine 5.2.1 Source  if you havent already

Should be 110GB in size, make sure to install on a fast drive, or you will be waiting DAYS for it to compile
https://www.unrealengine.com/en-US/ue-on-github<br/>
https://dev.epicgames.com/documentation/en-us/unreal-engine/building-unreal-engine-from-source<br/>
2. Compile a Vannila Build of 5.2.1 and test if its all working<br/>
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
