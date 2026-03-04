// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetActionUtility.h"
#include "TextureUtility.generated.h"

// =============================================================================
/**
 * 에디터에서 UI 텍스처 편집을 위한 유틸리티 클래스
 */
 //=============================================================================
 // (260304) PJB 제작.
 //=============================================================================

UCLASS(Blueprintable)
class LUNAR_REALM_API UTextureUtility : public UAssetActionUtility
{
	GENERATED_BODY()
	
public:
	UTextureUtility();

	/** 텍스쳐를 4분면 대칭하여 병합 */
	UFUNCTION(CallInEditor, Category = "LR|Texture Tools")
	void Create4MirroredTexture(class UTexture2D* SourceTex);
	
	/** 텍스쳐를 90도 회전 */
	UFUNCTION(CallInEditor, Category = "LR|Texture Tools")
	void RotateTexture90(class UTexture2D* SourceTex);
	
	/** 텍스쳐 배열을 가로로 병합*/
	UFUNCTION(CallInEditor, Category = "LR|Texture Tools")
	void MergeTexturesHorizontally(const TArray<class UTexture2D*>& SelectedTextures);

private:
	/** 렌더링 타겟 생성 */
	class UCanvasRenderTarget2D* CreateRenderTarget(int32 Width, int32 Height);
	/** 새로운 텍스쳐 생성 */
	void SaveRenderTargetToTexture(class UTexture2D* BaseTexture, class UCanvasRenderTarget2D* CanvasRT, const FString& Suffix);
};
