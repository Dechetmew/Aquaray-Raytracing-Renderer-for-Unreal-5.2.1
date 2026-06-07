//Refer to the wiki for Installation

// Insert into Line 1092
void AquaRayRender(FRDGBuilder& GraphBuilder, const FViewInfo& View, FRDGTextureRef* OutputTexture);

// Insert near line 1154
static void AquaRayPrepareRayTracing(const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders);
//static void PrepareRayTracingReflections(...
