//// Fill out your copyright notice in the Description page of Project Settings.
//
//
//#include "System/TextureUtility.h"
//
//#if WITH_EDITOR
//
//#include "Engine/Texture2D.h"
//#include "Engine/CanvasRenderTarget2D.h"
//#include "Engine/Canvas.h"
//#include "Kismet/KismetRenderingLibrary.h"
//#include "AssetRegistry/AssetRegistryModule.h"
//#include "Editor.h"
//
//
//UTextureUtility::UTextureUtility()
//{
//	SupportedClasses.Add(UTexture2D::StaticClass());
//}
//
//void UTextureUtility::Crop(UTexture2D* SourceTex, int32 StartX, int32 StartY, int32 CropWidth, int32 CropHeight)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 OriginalW = SourceTex->GetSizeX();
//	int32 OriginalH = SourceTex->GetSizeY();
//
//	// Clamp Crop Parameters
//	StartX = FMath::Clamp(StartX, 0, OriginalW - 1);
//	StartY = FMath::Clamp(StartY, 0, OriginalH - 1);
//	CropWidth = FMath::Clamp(CropWidth, 1, OriginalW - StartX);
//	CropHeight = FMath::Clamp(CropHeight, 1, OriginalH - StartY);
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(CropWidth, CropHeight);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		float UV_X = (float)StartX / OriginalW;
//		float UV_Y = (float)StartY / OriginalH;
//		float UV_Width = (float)CropWidth / OriginalW;
//		float UV_Height = (float)CropHeight / OriginalH;
//
//		Canvas->K2_DrawTexture(
//			SourceTex,
//			FVector2D(0, 0), FVector2D(CropWidth, CropHeight),
//			FVector2D(UV_X, UV_Y), FVector2D(UV_Width, UV_Height),
//			FLinearColor::White, EBlendMode::BLEND_Opaque
//		);
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	FString Suffix = FString::Printf(TEXT("_Cropped_%dx%d"), CropWidth, CropHeight);
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, Suffix);
//}
//
//void UTextureUtility::FlipHorizontal(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 W = SourceTex->GetSizeX();
//	int32 H = SourceTex->GetSizeY();
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(W, H);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		FVector2D DrawSize(W, H);
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, 0), DrawSize, FVector2D(1, 0), FVector2D(-1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // RU (Flip L-R)
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_HorizontalFlip"));
//}
//
//void UTextureUtility::FlipVertical(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 W = SourceTex->GetSizeX();
//	int32 H = SourceTex->GetSizeY();
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(W, H);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		FVector2D DrawSize(W, H);
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, 0), DrawSize, FVector2D(0, 1), FVector2D(1, -1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LD (Flip U-D)
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_VerticalFlip"));
//}
//
//void UTextureUtility::RotateLeft(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 OriginalW = SourceTex->GetSizeX();
//	int32 OriginalH = SourceTex->GetSizeY();
//
//	int32 NewW = OriginalH;
//	int32 NewH = OriginalW;
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(NewW, NewH);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		float PosX = (NewW / 2.0f) - (OriginalW / 2.0f);
//		float PosY = (NewH / 2.0f) - (OriginalH / 2.0f);
//
//		Canvas->K2_DrawTexture(
//			SourceTex,
//			FVector2D(PosX, PosY),
//			FVector2D(OriginalW, OriginalH),
//			FVector2D(0, 0),             // Coordinate Position
//			FVector2D(1, 1),             // Coordinate Size
//			FLinearColor::White,         // RenderColor
//			EBlendMode::BLEND_Translucent, // BlendMode
//			-90.0f,                       // Rotation Angle
//			FVector2D(0.5f, 0.5f)        // PivotPoint 
//		);
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_90Left"));
//}
//
//void UTextureUtility::RotateRight(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 OriginalW = SourceTex->GetSizeX();
//	int32 OriginalH = SourceTex->GetSizeY();
//
//	int32 NewW = OriginalH;
//	int32 NewH = OriginalW;
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(NewW, NewH);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		float PosX = (NewW / 2.0f) - (OriginalW / 2.0f);
//		float PosY = (NewH / 2.0f) - (OriginalH / 2.0f);
//
//		Canvas->K2_DrawTexture(
//			SourceTex,
//			FVector2D(PosX, PosY),
//			FVector2D(OriginalW, OriginalH),
//			FVector2D(0, 0),             // Coordinate Position
//			FVector2D(1, 1),             // Coordinate Size
//			FLinearColor::White,         // RenderColor
//			EBlendMode::BLEND_Translucent, // BlendMode
//			90.0f,                       // Rotation Angle
//			FVector2D(0.5f, 0.5f)        // PivotPoint 
//		);
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_90Right"));
//}
//
//
//void UTextureUtility::MirroredQuad(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 W = SourceTex->GetSizeX();
//	int32 H = SourceTex->GetSizeY();
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(W * 2, H * 2);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		FVector2D DrawSize(W, H);
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, 0), DrawSize, FVector2D(0, 0), FVector2D(1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LU (Origin)
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(W, 0), DrawSize, FVector2D(1, 0), FVector2D(-1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // RU (Flip L-R)
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, H), DrawSize, FVector2D(0, 1), FVector2D(1, -1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LD (Flip U-D)
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(W, H), DrawSize, FVector2D(1, 1), FVector2D(-1, -1), FLinearColor::White, EBlendMode::BLEND_Opaque); // RD (Rotate 180)
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_QuadMirrored"));
//}
//
//void UTextureUtility::MirroredHorizontal(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 W = SourceTex->GetSizeX();
//	int32 H = SourceTex->GetSizeY();
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(W * 2, H);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		FVector2D DrawSize(W, H);
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, 0), DrawSize, FVector2D(0, 0), FVector2D(1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LU (Origin)
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(W, 0), DrawSize, FVector2D(1, 0), FVector2D(-1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // RU (Flip L-R)
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_HorizontalMirrored"));
//}
//
//void UTextureUtility::MirroredVertical(UTexture2D* SourceTex)
//{
//	if (!SourceTex) return;
//
//	// Calculate Size
//	int32 W = SourceTex->GetSizeX();
//	int32 H = SourceTex->GetSizeY();
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(W, H * 2);
//	if (!CanvasRT) return;
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		FVector2D DrawSize(W, H);
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, 0), DrawSize, FVector2D(0, 0), FVector2D(1, 1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LU (Origin)
//		Canvas->K2_DrawTexture(SourceTex, FVector2D(0, H), DrawSize, FVector2D(0, 1), FVector2D(1, -1), FLinearColor::White, EBlendMode::BLEND_Opaque); // LD (Flip U-D)
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SourceTex, CanvasRT, TEXT("_VerticalMirrored"));
//}
//
//
//void UTextureUtility::MergeHorizontally(const TArray<UTexture2D*>& SelectedTextures)
//{
//	if (SelectedTextures.Num() == 0) return;
//
//	// Calculate Size
//	int32 TotalWidth = 0;
//	int32 MaxHeight = 0;
//	
//	for (UTexture2D* Tex : SelectedTextures)
//	{
//		if (Tex)
//		{
//			TotalWidth += Tex->GetSizeX();
//			MaxHeight = FMath::Max(MaxHeight, Tex->GetSizeY());
//		}
//	}
//
//	if (TotalWidth == 0 || MaxHeight == 0) return;
//
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(TotalWidth, MaxHeight);
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		float CurrentPosX = 0.0f;
//
//		for (UTexture2D* Tex : SelectedTextures)
//		{
//			if (Tex)
//			{
//				float TexW = Tex->GetSizeX();
//				float TexH = Tex->GetSizeY();
//
//				Canvas->K2_DrawTexture(
//					Tex,
//					FVector2D(CurrentPosX, 0), FVector2D(TexW, TexH),
//					FVector2D(0, 0), FVector2D(1, 1),
//					FLinearColor::White, EBlendMode::BLEND_Translucent
//				);
//				CurrentPosX += TexW;
//			}
//		}
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SelectedTextures[0], CanvasRT, TEXT("_HorizontalMerge"));
//}
//
//void UTextureUtility::MergeVertically(const TArray<class UTexture2D*>& SelectedTextures)
//{
//	if (SelectedTextures.Num() == 0) return;
//
//	// Calculate Size
//	int32 MaxWidth = 0;
//	int32 TotalHeight = 0;
//
//	for (UTexture2D* Tex : SelectedTextures)
//	{
//		if (Tex)
//		{
//			MaxWidth = FMath::Max(MaxWidth, Tex->GetSizeX());
//			TotalHeight += Tex->GetSizeY();
//		}
//	}
//
//	if (MaxWidth == 0 || TotalHeight == 0) return;
//
//
//	// Create Render Target
//	UCanvasRenderTarget2D* CanvasRT = CreateRenderTarget(MaxWidth, TotalHeight);
//
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvas* Canvas;
//	FVector2D Size;
//	FDrawToRenderTargetContext Context;
//
//	// Draw Texture
//	UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(EditorWorld, CanvasRT, Canvas, Size, Context);
//	if (Canvas)
//	{
//		float CurrentPosY = 0.0f;
//
//		for (UTexture2D* Tex : SelectedTextures)
//		{
//			if (Tex)
//			{
//				float TexW = Tex->GetSizeX();
//				float TexH = Tex->GetSizeY();
//
//				Canvas->K2_DrawTexture(
//					Tex,
//					FVector2D(0, CurrentPosY), FVector2D(TexW, TexH),
//					FVector2D(0, 0), FVector2D(1, 1),
//					FLinearColor::White, EBlendMode::BLEND_Translucent
//				);
//				CurrentPosY += TexH;
//			}
//		}
//	}
//	UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(EditorWorld, Context);
//
//	// Save Result
//	SaveRenderTargetToTexture(SelectedTextures[0], CanvasRT, TEXT("_VerticalMerge"));
//}
//
//UCanvasRenderTarget2D* UTextureUtility::CreateRenderTarget(int32 Width, int32 Height)
//{
//	UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();
//	UCanvasRenderTarget2D* CanvasRT = UCanvasRenderTarget2D::CreateCanvasRenderTarget2D(
//		EditorWorld, UCanvasRenderTarget2D::StaticClass(), Width, Height);
//
//	if (CanvasRT)
//	{
//		CanvasRT->ClearColor = FLinearColor(0.f, 0.f, 0.f, 0.f);
//		CanvasRT->UpdateResourceImmediate(true);
//	}
//	return CanvasRT;
//}
//
//void UTextureUtility::SaveRenderTargetToTexture(UTexture2D* BaseTexture, UCanvasRenderTarget2D* CanvasRT, const FString& Suffix)
//{
//	if (!BaseTexture || !CanvasRT) return;
//
//	FlushRenderingCommands();
//
//	FString NewTexName = BaseTexture->GetName() + Suffix;
//	FString PackageName = FPaths::GetPath(BaseTexture->GetOutermost()->GetName()) + TEXT("/") + NewTexName;
//	
//	UPackage* Package = CreatePackage(*PackageName);
//	UTexture2D* NewTexture = CanvasRT->ConstructTexture2D(Package, NewTexName, RF_Public | RF_Standalone, CTF_Default, nullptr);
//
//	if (NewTexture)
//	{
//		NewTexture->CompressionSettings = BaseTexture->CompressionSettings;
//		NewTexture->LODGroup = BaseTexture->LODGroup;
//		NewTexture->SRGB = BaseTexture->SRGB;
//		NewTexture->UpdateResource(); 
//
//		NewTexture->PostEditChange();
//		FAssetRegistryModule::AssetCreated(NewTexture);
//		Package->MarkPackageDirty();
//
//		LR_INFO(TEXT("[TextureUtility] 텍스처 에셋 생성 완료: %s"), *NewTexName);
//	}
//}
//
//#endif