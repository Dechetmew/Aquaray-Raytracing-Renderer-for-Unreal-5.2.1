#include "RendererPrivate.h"
#include "GlobalShader.h"
#include "DeferredShadingRenderer.h"
#include "SceneTextureParameters.h"

#if RHI_RAYTRACING

#include "ClearQuad.h"
#include "FogRendering.h"
#include "SceneRendering.h"
#include "PostProcess/SceneRenderTargets.h"
#include "RHIResources.h"
#include "PostProcess/PostProcessing.h"
#include "RayTracing/RaytracingOptions.h"
#include "RayTracing/RayTracingLighting.h"

#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollection.h"
#include "Materials/MaterialParameterCollectionInstance.h"
#include "Engine/World.h"

class FRayTracingPrimaryRaysRGS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FRayTracingPrimaryRaysRGS)
	SHADER_USE_ROOT_PARAMETER_STRUCT(FRayTracingPrimaryRaysRGS, FGlobalShader)
	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		SHADER_PARAMETER(FLinearColor, ColorInput1)
		SHADER_PARAMETER(FLinearColor, ColorInput2)
		SHADER_PARAMETER(FLinearColor, ColorInput3)
		SHADER_PARAMETER(FLinearColor, ColorInput4)
		SHADER_PARAMETER(FLinearColor, ColorInput5)
		SHADER_PARAMETER(FLinearColor, ColorInput6)
		SHADER_PARAMETER(FLinearColor, ColorInput7)
		SHADER_PARAMETER(FLinearColor, ColorInput8)
		SHADER_PARAMETER(FLinearColor, ColorInput9)
		SHADER_PARAMETER(float, FloatInput1)
		SHADER_PARAMETER(float, FloatInput2)
		SHADER_PARAMETER(float, FloatInput3)
		SHADER_PARAMETER(float, FloatInput4)
		SHADER_PARAMETER(float, FloatInput5)
		SHADER_PARAMETER(float, FloatInput6)
		SHADER_PARAMETER(float, FloatInput7)
		SHADER_PARAMETER(float, FloatInput8)
		SHADER_PARAMETER(float, FloatInput9)
		SHADER_PARAMETER(int32, IntInput1)
		SHADER_PARAMETER(int32, IntInput2)
		SHADER_PARAMETER(int32, IntInput3)
		SHADER_PARAMETER(int32, IntInput4)
		SHADER_PARAMETER(int32, IntInput5)
		SHADER_PARAMETER(int32, IntInput6)
		SHADER_PARAMETER(int32, IntInput7)
		SHADER_PARAMETER(int32, IntInput8)
		SHADER_PARAMETER(int32, IntInput9)

		SHADER_PARAMETER_SRV(RaytracingAccelerationStructure, TLAS)

		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, ViewUniformBuffer)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FRaytracingLightDataPacked, LightDataPacked)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FReflectionUniformParameters, ReflectionStruct)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FFogUniformParameters, FogUniformParameters)

		SHADER_PARAMETER(int32, UpscaleFactor)

		SHADER_PARAMETER_STRUCT_INCLUDE(FSceneTextureParameters, SceneTextures)

		SHADER_PARAMETER_RDG_TEXTURE(Texture2D, SceneColorTexture)

		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, ColorOutput)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, RayHitDistanceOutput)

	END_SHADER_PARAMETER_STRUCT()

	// this must stay here or it breaks
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return ShouldCompileRayTracingShadersForProject(Parameters.Platform);
	}

	static ERayTracingPayloadType GetRayTracingPayloadType(const int32 PermutationId)
	{
		return ERayTracingPayloadType::RayTracingMaterial;
	}
};

IMPLEMENT_GLOBAL_SHADER(FRayTracingPrimaryRaysRGS, "/Engine/Private/RayTracing/AquaRayMain.usf", "RayTracingPrimaryRaysRGS", SF_RayGen);

void FDeferredShadingSceneRenderer::PrepareRayTracingTranslucency(const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders)
{
	// Translucency and primary ray tracing requires the full ray tracing pipeline with material bindings.
	if (!ShouldRenderRayTracingEffect(ERayTracingPipelineCompatibilityFlags::FullPipeline))
	{
		return;
	}

	// Declare all RayGen shaders that require material closest hit shaders to be bound.
	// NOTE: Translucency shader may be used for primary ray debug view mode.

	if (GetRayTracingTranslucencyOptions(View).bEnabled || View.Family->EngineShowFlags.RayTracingDebug)
	{
		auto RayGenShader = View.ShaderMap->GetShader<FRayTracingPrimaryRaysRGS>();
		OutRayGenShaders.Add(RayGenShader.GetRayTracingShader());
	}
}

void FDeferredShadingSceneRenderer::RenderRayTracingPrimaryRaysView(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FRDGTextureRef* OutFinalColorTexture,
	FRDGTextureRef* OutRayHitDistanceTexture,
	int32 SamplePerPixel,
	int32 HeightFog,
	float ResolutionFraction,
	ERayTracingPrimaryRaysFlag Flags)
{
	auto IsColorValid = [](const FLinearColor& Color) -> bool
		{
			return !FMath::IsNaN(Color.R) &&
				!FMath::IsNaN(Color.G) &&
				!FMath::IsNaN(Color.B) &&
				!FMath::IsNaN(Color.A);
		};

	// Hardcoded Directory - Your MPC MUST be at that Location with said name
	UMaterialParameterCollection* Collection = LoadObject<UMaterialParameterCollection>(nullptr, TEXT("/Game/AquaRay/AquarayMPC"));
	UWorld* WorldContextObject = (View.Family && View.Family->Scene) ? View.Family->Scene->GetWorld() : nullptr;

	bool IsSafe = true;

	if (!Collection)
	{
		IsSafe = false;
		UE_LOG(LogTemp, Error, TEXT("Aquaray: MPC is Missing or Invalid"));
	}

	if (!WorldContextObject)
	{
		IsSafe = false;
		UE_LOG(LogTemp, Error, TEXT("Aquaray: WorldContext is Invalid"));
	}

	TArray<FName> ColorInputs;
	TArray<FLinearColor> ColorInputValues;

	TArray<FName> FloatInputs;
	TArray<float> FloatInputValues;

	TArray<FName> IntInputs;
	TArray<int32> IntInputValues;

	for (int32 i = 1; i <= 9; ++i)
	{
		ColorInputs.Add(FName(*FString::Printf(TEXT("ColorInput%d"), i)));
		ColorInputValues.Add(FLinearColor::Black);
	}

	for (int32 i = 1; i <= 9; ++i)
	{
		FloatInputs.Add(FName(*FString::Printf(TEXT("FloatInput%d"), i)));
		FloatInputValues.Add(0.0f);
	}

	for (int32 i = 1; i <= 9; ++i)
	{
		IntInputs.Add(FName(*FString::Printf(TEXT("IntInput%d"), i)));
		IntInputValues.Add(0);
	}

	if (IsSafe)
	{
		// Color Inputs
		for (int32 i = 0; i < ColorInputs.Num(); ++i)
		{
			FLinearColor Value = UKismetMaterialLibrary::GetVectorParameterValue(WorldContextObject, Collection, ColorInputs[i]);

			if (IsColorValid(Value))
			{
				ColorInputValues[i] = Value;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AQUARAY: ColorInput %s contains NaN, using default"), *ColorInputs[i].ToString());
			}
		}

		// Float Inputs
		for (int32 i = 0; i < FloatInputs.Num(); ++i)
		{
			float Value = UKismetMaterialLibrary::GetScalarParameterValue(WorldContextObject, Collection, FloatInputs[i]);

			if (!FMath::IsNaN(Value))
			{
				FloatInputValues[i] = Value;
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AQUARAY: FloatInput %s is NaN, using default"), *FloatInputs[i].ToString());
			}
		}

		// Int Inputs
		for (int32 i = 0; i < IntInputs.Num(); ++i)
		{
			float Value = UKismetMaterialLibrary::GetScalarParameterValue(WorldContextObject, Collection, IntInputs[i]);

			if (!FMath::IsNaN(Value))
			{
				IntInputValues[i] = FMath::RoundToInt(Value);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AQUARAY: IntInput %s is NaN, using default"), *IntInputs[i].ToString());
			}
		}
	}

	const FSceneTextures& SceneTextures = GetActiveSceneTextures();
	const FSceneTextureParameters SceneTextureParameters = GetSceneTextureParameters(GraphBuilder, SceneTextures);

	int32 UpscaleFactor = int32(1.0f / ResolutionFraction);
	ensure(ResolutionFraction == 1.0 / UpscaleFactor);
	ensureMsgf(FComputeShaderUtils::kGolden2DGroupSize % UpscaleFactor == 0, TEXT("PrimaryRays ray tracing will have uv misalignement."));
	FIntPoint RayTracingResolution = FIntPoint::DivideAndRoundUp(View.ViewRect.Size(), UpscaleFactor);

	{
		FRDGTextureDesc Desc = SceneTextures.Color.Target->Desc;
		Desc.Format = PF_FloatRGBA;
		Desc.Flags &= ~(TexCreate_FastVRAM);
		Desc.Flags |= TexCreate_UAV;
		Desc.Extent /= UpscaleFactor;

		if (*OutFinalColorTexture == nullptr)
		{
			*OutFinalColorTexture = GraphBuilder.CreateTexture(Desc, TEXT("RayTracingPrimaryRays"));

		}

		Desc.Format = PF_R16F;
		if (*OutRayHitDistanceTexture == nullptr)
		{
			*OutRayHitDistanceTexture = GraphBuilder.CreateTexture(Desc, TEXT("RayTracingPrimaryRaysHitDistance"));
		}
	}

	FRayTracingPrimaryRaysRGS::FParameters* PassParameters = GraphBuilder.AllocParameters<FRayTracingPrimaryRaysRGS::FParameters>();

	FRayTracingPrimaryRaysOptions TranslucencyOptions = GetRayTracingTranslucencyOptions(View);

	for (int32 i = 0; i < ColorInputValues.Num(); ++i)
	{
		switch (i)
		{
		case 0: PassParameters->ColorInput1 = ColorInputValues[i]; break;
		case 1: PassParameters->ColorInput2 = ColorInputValues[i]; break;
		case 2: PassParameters->ColorInput3 = ColorInputValues[i]; break;
		case 3: PassParameters->ColorInput4 = ColorInputValues[i]; break;
		case 4: PassParameters->ColorInput5 = ColorInputValues[i]; break;
		case 5: PassParameters->ColorInput6 = ColorInputValues[i]; break;
		case 6: PassParameters->ColorInput7 = ColorInputValues[i]; break;
		case 7: PassParameters->ColorInput8 = ColorInputValues[i]; break;
		case 8: PassParameters->ColorInput9 = FLinearColor(View.ViewMatrices.GetViewOrigin()); break;
		}
	}

	for (int32 i = 0; i < FloatInputValues.Num(); ++i)
	{
		switch (i)
		{
		case 0: PassParameters->FloatInput1 = FloatInputValues[i]; break;
		case 1: PassParameters->FloatInput2 = FloatInputValues[i]; break;
		case 2: PassParameters->FloatInput3 = FloatInputValues[i]; break;
		case 3: PassParameters->FloatInput4 = FloatInputValues[i]; break;
		case 4: PassParameters->FloatInput5 = FloatInputValues[i]; break;
		case 5: PassParameters->FloatInput6 = FloatInputValues[i]; break;
		case 6: PassParameters->FloatInput7 = FloatInputValues[i]; break;
		case 7: PassParameters->FloatInput8 = FloatInputValues[i]; break;
		case 8: PassParameters->FloatInput9 = FloatInputValues[i]; break;
		}
	}

	for (int32 i = 0; i < IntInputValues.Num(); ++i)
	{
		switch (i)
		{
		case 0: PassParameters->IntInput1 = IntInputValues[i]; break;
		case 1: PassParameters->IntInput2 = IntInputValues[i]; break;
		case 2: PassParameters->IntInput3 = IntInputValues[i]; break;
		case 3: PassParameters->IntInput4 = IntInputValues[i]; break;
		case 4: PassParameters->IntInput5 = IntInputValues[i]; break;
		case 5: PassParameters->IntInput6 = IntInputValues[i]; break;
		case 6: PassParameters->IntInput7 = IntInputValues[i]; break;
		case 7: PassParameters->IntInput8 = IntInputValues[i]; break;
		case 8: PassParameters->IntInput9 = IntInputValues[i]; break;
		}
	}

	PassParameters->TLAS = View.GetRayTracingSceneLayerViewChecked(ERayTracingSceneLayer::Base);

	PassParameters->ViewUniformBuffer = View.ViewUniformBuffer;
	PassParameters->LightDataPacked = View.RayTracingLightDataUniformBuffer;

	PassParameters->SceneTextures = SceneTextureParameters;
	PassParameters->SceneColorTexture = SceneTextures.Color.Resolve;

	PassParameters->ReflectionStruct = CreateReflectionUniformBuffer(GraphBuilder, View);
	PassParameters->FogUniformParameters = CreateFogUniformBuffer(GraphBuilder, View);

	PassParameters->UpscaleFactor = UpscaleFactor;

	PassParameters->ColorOutput = GraphBuilder.CreateUAV(*OutFinalColorTexture);
	PassParameters->RayHitDistanceOutput = GraphBuilder.CreateUAV(*OutRayHitDistanceTexture);

	auto RayGenShader = View.ShaderMap->GetShader<FRayTracingPrimaryRaysRGS>();

	ClearUnusedGraphResources(RayGenShader, PassParameters);

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("RayTracingPrimaryRays %dx%d", RayTracingResolution.X, RayTracingResolution.Y),
		PassParameters,
		ERDGPassFlags::Compute,
		[PassParameters, this, &View, RayGenShader, RayTracingResolution](FRHIRayTracingCommandList& RHICmdList)
		{
			FRayTracingPipelineState* Pipeline = View.RayTracingMaterialPipeline;

			FRayTracingShaderBindingsWriter GlobalResources;
			SetShaderParameters(GlobalResources, RayGenShader, *PassParameters);

			FRHIRayTracingScene* RayTracingSceneRHI = View.GetRayTracingSceneChecked();
			RHICmdList.RayTraceDispatch(Pipeline, RayGenShader.GetRayTracingShader(), RayTracingSceneRHI, GlobalResources, RayTracingResolution.X, RayTracingResolution.Y);
		});
}

#endif // RHI_RAYTRACING
