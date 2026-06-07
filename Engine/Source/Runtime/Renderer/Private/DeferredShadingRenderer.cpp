//Refer to the wiki for Installation

//Modify If-Else block at line 1807
auto* Var = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.RayTracing.AquaRay"));
if (Var->GetValueOnRenderThread())
{
	if (!IsForwardShadingEnabled(ShaderPlatform))
  {
  	for (const FViewInfo& View : Views)
    {
  				AquaRayPrepareRayTracing(View, RayGenShaders);
  	}
  }
}
else if (bIsPathTracing)
{...

//Modify If-Else block inside if (IsRayTracingEnabled())
auto* Var = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.RayTracing.AquaRay"));
if (Var->GetValueOnRenderThread()) //ADM8 AquaRay Call
{
	for (const FViewInfo& View : Views)
	{
		FRDGTextureRef OutputTexture = nullptr;
		AquaRayRender(GraphBuilder, View, &OutputTexture);
		AddDrawTexturePass(GraphBuilder, View, OutputTexture, SceneTextures.Color.Target, View.ViewRect.Min, View.ViewRect.Min, View.ViewRect.Size());
	}
}
else if (ViewFamily.EngineShowFlags.PathTracing && FDataDrivenShaderPlatformInfo::GetSupportsPathTracing(Scene->GetShaderPlatform()))
{...

//Replace AnyRayTracingPassEnabled() Function with this:
bool AnyRayTracingPassEnabled(const FScene* Scene, const FViewInfo& View)
{
	if (!IsRayTracingEnabled() || Scene == nullptr)
	{
		return false;
	}

	auto* Var = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.RayTracing.AquaRay"));

	return ShouldRenderRayTracingAmbientOcclusion(View)
		|| ShouldRenderRayTracingReflections(View)
		|| ShouldRenderRayTracingGlobalIllumination(View)
		|| ShouldRenderRayTracingTranslucency(View)
		|| ShouldRenderRayTracingSkyLight(Scene->SkyLight)
		|| ShouldRenderRayTracingShadows()
		|| Scene->bHasRayTracedLights
		|| ShouldRenderPluginRayTracingGlobalIllumination(View)
        || Lumen::AnyLumenHardwareRayTracingPassEnabled(Scene, View)
		|| ShouldRenderRayTracingReflectionsWater(View)
		|| HasRayTracedOverlay(*View.Family)
		|| Var->GetValueOnRenderThread();
}

//Replace HasRayTracedOverlay() Function with this:
bool HasRayTracedOverlay(const FSceneViewFamily& ViewFamily)
{
	// Return true if a full screen ray tracing pass will be displayed on top of the raster pass
	// This can be used to skip certain calculations
	auto* Var = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("r.RayTracing.AquaRay"));

	return
		ViewFamily.EngineShowFlags.PathTracing ||
		ViewFamily.EngineShowFlags.RayTracingDebug ||
		Var->GetValueOnRenderThread();
}
