#include "RendererPrivate.h"
#include "GlobalShader.h"
#include "DeferredShadingRenderer.h"
#include "SceneTextureParameters.h"

TAutoConsoleVariable<int> CVarAquaRay( //Temporary Console Variable to Enable the Renderer, will be Replaced by a Show Flag similiar to Path Tracing.
	TEXT("r.RayTracing.AquaRay"),
	1,
	TEXT("Enables AquaRay Renderer"),
	ECVF_RenderThreadSafe
);

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

class FAquaRayRGS : public FGlobalShader
{
	DECLARE_GLOBAL_SHADER(FAquaRayRGS)
	SHADER_USE_ROOT_PARAMETER_STRUCT(FAquaRayRGS, FGlobalShader)
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

	END_SHADER_PARAMETER_STRUCT()

	// Translation: Returns If it should even Compile the Shaders present here YES OF COURSE IT SHOULD
	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return ShouldCompileRayTracingShadersForProject(Parameters.Platform);
	}

	static ERayTracingPayloadType GetRayTracingPayloadType(const int32 PermutationId)
	{
		return ERayTracingPayloadType::RayTracingMaterial;
	}
};

IMPLEMENT_GLOBAL_SHADER(FAquaRayRGS, "/Engine/Private/RayTracing/AquaRay/AquaRay.usf", "AquaRayRGS", SF_RayGen);

// If I understand, this adds the RayGen Shaders into the Rendering Pipeline. 
void FDeferredShadingSceneRenderer::AquaRayPrepareRayTracing(const FViewInfo& View, TArray<FRHIRayTracingShader*>& OutRayGenShaders)
{
	if (!ShouldRenderRayTracingEffect(ERayTracingPipelineCompatibilityFlags::FullPipeline))
	{
		return;
	}

	auto RayGenShader = View.ShaderMap->GetShader<FAquaRayRGS>();
	OutRayGenShaders.Add(RayGenShader.GetRayTracingShader());
}


void FDeferredShadingSceneRenderer::AquaRayRender(
	FRDGBuilder& GraphBuilder,
	const FViewInfo& View,
	FRDGTextureRef* OutputTexture)
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

	float ResolutionFraction = 1;
	int32 UpscaleFactor = int32(1.0f / ResolutionFraction);
	ensure(ResolutionFraction == 1.0 / UpscaleFactor);
	ensureMsgf(FComputeShaderUtils::kGolden2DGroupSize % UpscaleFactor == 0, TEXT("Ray tracing will have uv misalignement."));
	FIntPoint RayTracingResolution = FIntPoint::DivideAndRoundUp(View.ViewRect.Size(), UpscaleFactor);

	FRDGTextureDesc Desc = SceneTextures.Color.Target->Desc;
	Desc.Format = PF_FloatRGBA;
	Desc.Flags &= ~(TexCreate_FastVRAM);
	Desc.Flags |= TexCreate_UAV;
	Desc.Extent /= UpscaleFactor;
	if (*OutputTexture == nullptr)
	{
		*OutputTexture = GraphBuilder.CreateTexture(Desc, TEXT("SceneColorTexture"));
	}
	
	FAquaRayRGS::FParameters* PassParameters = GraphBuilder.AllocParameters<FAquaRayRGS::FParameters>();

	PassParameters->ColorInput1 = ColorInputValues[0];
	PassParameters->ColorInput2 = ColorInputValues[1];
	PassParameters->ColorInput3 = ColorInputValues[2];
	PassParameters->ColorInput4 = ColorInputValues[3];
	PassParameters->ColorInput5 = ColorInputValues[4];
	PassParameters->ColorInput6 = ColorInputValues[5];
	PassParameters->ColorInput7 = ColorInputValues[6];
	PassParameters->ColorInput8 = ColorInputValues[7];
	PassParameters->ColorInput9 = FLinearColor(View.ViewMatrices.GetViewOrigin());

	PassParameters->FloatInput1 = FloatInputValues[0];
	PassParameters->FloatInput2 = FloatInputValues[1];
	PassParameters->FloatInput3 = FloatInputValues[2];
	PassParameters->FloatInput4 = FloatInputValues[3];
	PassParameters->FloatInput5 = FloatInputValues[4];
	PassParameters->FloatInput6 = FloatInputValues[5];
	PassParameters->FloatInput7 = FloatInputValues[6];
	PassParameters->FloatInput8 = FloatInputValues[7];
	PassParameters->FloatInput9 = FloatInputValues[8];

	PassParameters->IntInput1 = IntInputValues[0];
	PassParameters->IntInput2 = IntInputValues[1];
	PassParameters->IntInput3 = IntInputValues[2];
	PassParameters->IntInput4 = IntInputValues[3];
	PassParameters->IntInput5 = IntInputValues[4];
	PassParameters->IntInput6 = IntInputValues[5];
	PassParameters->IntInput7 = IntInputValues[6];
	PassParameters->IntInput8 = IntInputValues[7];
	PassParameters->IntInput9 = IntInputValues[8];

	PassParameters->TLAS = View.GetRayTracingSceneLayerViewChecked(ERayTracingSceneLayer::Base);

	PassParameters->ViewUniformBuffer = View.ViewUniformBuffer;
	PassParameters->LightDataPacked = View.RayTracingLightDataUniformBuffer;

	PassParameters->SceneTextures = SceneTextureParameters;
	PassParameters->SceneColorTexture = SceneTextures.Color.Resolve;

	PassParameters->ReflectionStruct = CreateReflectionUniformBuffer(GraphBuilder, View);
	PassParameters->FogUniformParameters = CreateFogUniformBuffer(GraphBuilder, View);

	PassParameters->UpscaleFactor = UpscaleFactor;

	PassParameters->ColorOutput = GraphBuilder.CreateUAV(*OutputTexture);

	auto RayGenShader = View.ShaderMap->GetShader<FAquaRayRGS>();

	ClearUnusedGraphResources(RayGenShader, PassParameters);

	GraphBuilder.AddPass(
		RDG_EVENT_NAME("AquaRayRGS %dx%d", RayTracingResolution.X, RayTracingResolution.Y),
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