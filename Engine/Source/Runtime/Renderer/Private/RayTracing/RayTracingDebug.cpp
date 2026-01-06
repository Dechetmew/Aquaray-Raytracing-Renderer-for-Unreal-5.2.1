// I cant include the whole .cpp file, so search for "if (DebugVisualizationMode == RAY_TRACING_DEBUG_VIZ_PRIMARY_RAYS)" in the Official UE code and replace it with this
// At least in UE 5.2.1 it should be on line 876

	if (DebugVisualizationMode == RAY_TRACING_DEBUG_VIZ_PRIMARY_RAYS)
	{
		FRDGTextureRef OutputColor = nullptr;
		FRDGTextureRef HitDistanceTexture = nullptr;

		RenderRayTracingPrimaryRaysView(
			GraphBuilder,
			View,
			&OutputColor,
			&HitDistanceTexture,
			1, 1, 1,
			ERayTracingPrimaryRaysFlag::PrimaryView
		);

		AddDrawTexturePass(GraphBuilder, View, OutputColor, SceneColorTexture, View.ViewRect.Min, View.ViewRect.Min, View.ViewRect.Size());
		return;
	}

