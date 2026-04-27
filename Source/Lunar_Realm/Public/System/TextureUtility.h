//// Fill out your copyright notice in the Description page of Project Settings.
//
//#pragma once
//#include "CoreMinimal.h"
//#if WITH_EDITOR
//#include "AssetActionUtility.h"
//#endif
//#include "TextureUtility.generated.h"
//// =============================================================================
///**
// * 에디터에서 UI 텍스처 편집을 위한 유틸리티 클래스
// */
// //=============================================================================
// // (260304) PJB 제작.
// //=============================================================================
//
//#if WITH_EDITOR
//UCLASS(Blueprintable)
//class LUNAR_REALM_API UTextureUtility : public UAssetActionUtility
//{
//	GENERATED_BODY()
//	
//public:
//	UTextureUtility();
//
//	// TODO: 여러 텍스쳐를 선택하여 4분면으로 병합하는 기능 추가
//
//	/** 텍스쳐를 자르기 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Crop")
//	void Crop(class UTexture2D* SourceTex, int32 StartX, int32 StartY, int32 CropWidth, int32 CropHeight);
//
//	/** 텍스쳐를 좌우 반전 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Flip")
//	void FlipHorizontal(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 상하 반전 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Flip")
//	void FlipVertical(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 반시계방향으로 90도 회전 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Rotate")
//	void RotateLeft(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 시계방향으로 90도 회전 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Rotate")
//	void RotateRight(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 4분면 대칭하여 병합 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Mirror")
//	void MirroredQuad(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 좌우 대칭하여 병합 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Mirror")
//	void MirroredHorizontal(class UTexture2D* SourceTex);
//
//	/** 텍스쳐를 상하 대칭하여 병합 */
//	UFUNCTION(CallInEditor, Category = "LR|Texture Mirror")
//	void MirroredVertical(class UTexture2D* SourceTex);
//
//	/** 텍스쳐 배열을 가로로 병합*/
//	UFUNCTION(CallInEditor, Category = "LR|Texture Merge")
//	void MergeHorizontally(const TArray<class UTexture2D*>& SelectedTextures);
//
//	/** 텍스쳐 배열을 세로로 병합*/
//	UFUNCTION(CallInEditor, Category = "LR|Texture Merge")
//	void MergeVertically(const TArray<class UTexture2D*>& SelectedTextures);
//
//private:
//	/** 렌더링 타겟 생성 */
//	class UCanvasRenderTarget2D* CreateRenderTarget(int32 Width, int32 Height);
//	/** 새로운 텍스쳐 생성 */
//	void SaveRenderTargetToTexture(class UTexture2D* BaseTexture, class UCanvasRenderTarget2D* CanvasRT, const FString& Suffix);
//};
//#endif
