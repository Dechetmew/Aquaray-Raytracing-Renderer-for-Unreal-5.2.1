//find where RenderRayTracingPrimaryRaysView() is in DeferredShadingRenderer.h, and replace it with this (should be around line 1090) 
	
	void RenderRayTracingPrimaryRaysView(
		FRDGBuilder& GraphBuilder,
		const FViewInfo& View,
		FRDGTextureRef* OutFinalColorTexture,
		FRDGTextureRef* OutRayHitDistanceTexture,
		int32 SamplePerPixel,
		int32 HeightFog,
		float ResolutionFraction,
		ERayTracingPrimaryRaysFlag Flags);
