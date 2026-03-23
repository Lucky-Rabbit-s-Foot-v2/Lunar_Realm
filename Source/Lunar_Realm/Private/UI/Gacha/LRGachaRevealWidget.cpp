// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaRevealWidget.h"
#include "UI/Gacha/LRGachaResultSlotWidget.h"

#include "Actors/Gacha/LRGachaOrbSceneActor.h"

#include "Subsystems/Settings/MapSettings.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/AudioComponent.h"

#include "Core/LRGameInstance.h"

#include "MediaPlayer.h"
#include "MediaTexture.h"

#include "Materials/MaterialInstanceDynamic.h"

#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EngineUtils.h"

// ─────────────────────────────────────────────────────────────────────────────
//  위젯 생명주기
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ButtonSkip)
	{
		ButtonSkip->OnClicked.RemoveDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
		ButtonSkip->OnClicked.AddUniqueDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
	}
}

void ULRGachaRevealWidget::NativeDestruct()
{
	bTransitionPlaying = false;
	PendingTransitionOrbIndex = INDEX_NONE;
	PendingTransitionResult = FLRGachaResult();

	if (TransitionOverlay)
	{
		TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerSequentialResultSlots);
		GetWorld()->GetTimerManager().ClearTimer(TimerRevealSFXDelay);
	}

	StopAllPreviousStageSounds();
	StopRevealStageBGM();

	CachedResultSlotWidgets.Empty();

	if (ButtonSkip)
	{
		ButtonSkip->OnClicked.RemoveDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
	}

	if (OrbSceneActor)
	{
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
		OrbSceneActor->OnRevealPresentationRequested.RemoveDynamic(this, &ULRGachaRevealWidget::HandleRevealPresentationRequested);
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PreviousViewTarget)
		{
			PC->SetViewTargetWithBlend(PreviousViewTarget, 0.5f);
		}
	}

	Super::NativeDestruct();
}

// ─────────────────────────────────────────────────────────────────────────────
//  외부 진입점(리빌 시작)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::StartRevealWithTransaction(
	FName InBannerID,
	FGuid InTxnId,
	const TArray<FLRGachaResult>& InResults)
{
	CachedTxnId = InTxnId;
	StartReveal(InBannerID, InResults);
}

void ULRGachaRevealWidget::StartReveal(FName InBannerID, const TArray<FLRGachaResult>& InResults)
{
	CachedBannerID = InBannerID;
	CachedResults = InResults;

	bAllRevealed = false;
	bResultOverlayShown = false;
	bPresentationVisible = false;
	bPresentationUsingVideo = false;

	bTransitionPlaying = false;
	PendingTransitionOrbIndex = INDEX_NONE;
	PendingTransitionResult = FLRGachaResult();

	bIsPointerDown = false;
	PointerDownPosition = FVector2D::ZeroVector;

	if (TransitionOverlay)
	{
		TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (ResultSlotContainer)
	{
		ResultSlotContainer->ClearChildren();
	}

	if (ResultOverlay)
	{
		ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (PresentationOverlay)
	{
		PresentationOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (HintText)
	{
		HintText->SetVisibility(ESlateVisibility::Visible);
	}

	ResetTransitionVisuals();

	StartRevealStageBGM();

	ForceUIInputNextTick();

	FindOrSpawnOrbSceneActor();

	if (OrbSceneActor)
	{
		OrbSceneActor->InitializeWithResults(CachedResults);

		if (APlayerController* PC = GetOwningPlayer())
		{
			PreviousViewTarget = PC->GetViewTarget();
			PC->SetViewTargetWithBlend(OrbSceneActor, 0.5f);
		}
	}

	BP_OnRevealStarted(InResults.Num());
}

// ─────────────────────────────────────────────────────────────────────────────
//  리빌 종료(로비 복귀)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::FinishAndClose()
{
	StopAllPreviousStageSounds();
	StopRevealStageBGM();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>())
		{
			GachaSys->RequestOpenShopOnLobbyReturn(CachedBannerID);
		}
	}

	CloseSelf();

	if (ULRGameInstance* GI = Cast<ULRGameInstance>(GetGameInstance()))
	{
		GI->OpenNextLevelImmediately(ELevelName::LOBBY);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  결과 슬롯 생성(10칸 그리드)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::BuildResultSlots()
{
	if (!ResultSlotContainer || !ResultSlotWidgetClass)
	{
		return;
	}

	ResultSlotContainer->ClearChildren();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const int32 NumColumns = 5;

	if (UUniformGridPanel* Grid = Cast<UUniformGridPanel>(ResultSlotContainer))
	{
		const int32 NumResults = CachedResults.Num();

		for (int32 Index = 0; Index < NumResults; ++Index)
		{
			const FLRGachaResult& Result = CachedResults[Index];

			ULRGachaResultSlotWidget* SlotWidget =
				CreateWidget<ULRGachaResultSlotWidget>(World, ResultSlotWidgetClass);

			if (!SlotWidget)
			{
				continue;
			}

			SlotWidget->SetupWithResult(Result);

			const int32 Row = Index / NumColumns;
			const int32 Col = Index % NumColumns;

			UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(SlotWidget, Row, Col);
			if (GridSlot)
			{
			}
		}
	}
	else
	{
		for (const FLRGachaResult& Result : CachedResults)
		{
			ULRGachaResultSlotWidget* SlotWidget =
				CreateWidget<ULRGachaResultSlotWidget>(World, ResultSlotWidgetClass);

			if (!SlotWidget)
			{
				continue;
			}

			SlotWidget->SetupWithResult(Result);
			ResultSlotContainer->AddChild(SlotWidget);
		}
	}
}

// 최종 결과 슬롯 위젯을 전부 먼저 배치해 그리드 간격을 고정한 뒤,
// 각 슬롯을 일정 간격으로 하나씩 표시하는 순차 연출 함수
void ULRGachaRevealWidget::BuildResultSlotsSequential()
{
	if (!ResultSlotContainer || !ResultSlotWidgetClass)
	{
		return;
	}

	ResultSlotContainer->ClearChildren();
	CachedResultSlotWidgets.Empty();
	NextResultSlotIndex = 0;

	UWorld* World = GetWorld();
	if (!World || CachedResults.Num() <= 0)
	{
		return;
	}

	const int32 NumColumns = 5;

	// 1) 결과 슬롯을 전부 먼저 생성해서 그리드 자리를 고정
	for (int32 Index = 0; Index < CachedResults.Num(); ++Index)
	{
		const FLRGachaResult& Result = CachedResults[Index];

		ULRGachaResultSlotWidget* SlotWidget =
			CreateWidget<ULRGachaResultSlotWidget>(World, ResultSlotWidgetClass);

		if (!SlotWidget)
		{
			continue;
		}

		SlotWidget->SetupWithResult(Result);

		// 처음에는 보이지 않게 숨겨둠
		SlotWidget->SetVisibility(ESlateVisibility::Hidden);

		CachedResultSlotWidgets.Add(SlotWidget);

		if (UUniformGridPanel* Grid = Cast<UUniformGridPanel>(ResultSlotContainer))
		{
			const int32 Row = Index / NumColumns;
			const int32 Col = Index % NumColumns;
			Grid->AddChildToUniformGrid(SlotWidget, Row, Col);
		}
		else
		{
			ResultSlotContainer->AddChild(SlotWidget);
		}
	}

	// 2) 첫 슬롯 즉시 등장
	SpawnNextResultSlot();

	// 3) 두 번째부터는 타이머로 순차 등장
	if (CachedResultSlotWidgets.Num() > 1)
	{
		World->GetTimerManager().SetTimer(
			TimerSequentialResultSlots,
			this,
			&ULRGachaRevealWidget::SpawnNextResultSlot,
			ResultSlotAppearInterval,
			true
		);
	}
}

// 순차 등장 타이머에서 반복 호출되는 단일 슬롯 표시 함수
// 이미 배치된 슬롯을 Visible로 전환하고 등장 연출을 실행
void ULRGachaRevealWidget::SpawnNextResultSlot()
{
	if (!GetWorld())
	{
		return;
	}

	if (!CachedResultSlotWidgets.IsValidIndex(NextResultSlotIndex))
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerSequentialResultSlots);
		return;
	}

	ULRGachaResultSlotWidget* SlotWidget = CachedResultSlotWidgets[NextResultSlotIndex];
	if (SlotWidget)
	{
		// 자리는 이미 잡혀 있으므로 이제 보이게만 전환
		SlotWidget->SetVisibility(ESlateVisibility::Visible);

		// 등장 사운드 + BP 연출(애니메이션/VFX/Niagara) 실행
		SlotWidget->PlayAppearEffect();
	}

	NextResultSlotIndex++;

	if (NextResultSlotIndex >= CachedResultSlotWidgets.Num())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerSequentialResultSlots);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  스킵 버튼(단일 버튼 3단 동작)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::OnClickSkip()
{
	if (!OrbSceneActor)
	{
		return;
	}

	// 전환 애니메이션 중 스킵하면 캐릭터 등장화면으로 가지 말고
	// 바로 최종 결과창으로 직행
	if (bTransitionPlaying)
	{
		bTransitionPlaying = false;

		PendingTransitionOrbIndex = INDEX_NONE;
		PendingTransitionResult = FLRGachaResult();

		if (TransitionOverlay)
		{
			TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}

		BP_OnTransitionClosed();
		ResetTransitionVisuals();

		if (!bAllRevealed)
		{
			OrbSceneActor->SkipAllReveal();
			bAllRevealed = true;
		}

		bPresentationVisible = false;

		if (PresentationOverlay)
		{
			PresentationOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (RevealMediaPlayer)
		{
			RevealMediaPlayer->Close();
		}

		if (Image_RevealVideo)
		{
			Image_RevealVideo->SetVisibility(ESlateVisibility::Collapsed);
		}

		ShowFinalResultOverlay();
		return;
	}

	// 개별 리빌 화면이 떠 있으면 닫고 결과창으로
	if (bPresentationVisible)
	{
		HidePresentation();
		ShowFinalResultOverlay();
		return;
	}

	// 아직 전체 리빌이 안 끝났으면 캐릭터/영상 리빌은 전부 생략하고 결과창으로 직행
	if (!bResultOverlayShown)
	{
		if (!bAllRevealed)
		{
			OrbSceneActor->SkipAllReveal();
			bAllRevealed = true;
		}

		bPresentationVisible = false;

		if (PresentationOverlay)
		{
			PresentationOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (RevealMediaPlayer)
		{
			RevealMediaPlayer->Close();
		}

		if (Image_RevealVideo)
		{
			Image_RevealVideo->SetVisibility(ESlateVisibility::Collapsed);
		}

		ShowFinalResultOverlay();
		return;
	}

	FinishAndClose();
}

// ─────────────────────────────────────────────────────────────────────────────
//  마우스/터치 입력 처리(탭/스와이프)
// ─────────────────────────────────────────────────────────────────────────────

FReply ULRGachaRevealWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (bTransitionPlaying)
	{
		return FReply::Handled();
	}

	bIsPointerDown = true;
	PointerDownPosition = InMouseEvent.GetScreenSpacePosition();

	return FReply::Handled();
}

FReply ULRGachaRevealWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if (bTransitionPlaying)
	{
		return FReply::Handled();
	}

	if (!bIsPointerDown)
	{
		return FReply::Unhandled();
	}
	bIsPointerDown = false;

	const FVector2D UpPos = InMouseEvent.GetScreenSpacePosition();
	const FVector2D Delta = UpPos - PointerDownPosition;

	// ── [1] 개별 리빌 화면이 떠 있으면 탭으로 닫기 ──────────────────
	if (bPresentationVisible)
	{
		HidePresentation();

		// 모든 구슬 리빌이 끝난 뒤에만 결과창 표시
		if (bAllRevealed)
		{
			ShowFinalResultOverlay();
		}

		return FReply::Handled();
	}

	// ── [2] 최종 결과창 상태 ──────────────────────────────────────
	if (bAllRevealed && bResultOverlayShown)
	{
		FinishAndClose();
		return FReply::Handled();
	}

	// ── [3] 아직 리빌 중: 스와이프/탭 처리 ─────────────────────────
	if (OrbSceneActor)
	{
		const float AbsX = FMath::Abs(Delta.X);
		const float AbsY = FMath::Abs(Delta.Y);

		if (AbsX > SwipeMinDistance && AbsX > AbsY)
		{
			OrbSceneActor->OnSwipeInput(-Delta.X);
		}
		else
		{
			OrbSceneActor->OnTapCenterOrb();
		}
	}

	return FReply::Handled();
}

// ─────────────────────────────────────────────────────────────────────────────
//  OrbSceneActor 델리게이트 핸들러
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::HandleAllOrbsRevealed()
{
	bAllRevealed = true;
}

void ULRGachaRevealWidget::HandleRevealPresentationRequested(int32 OrbIndex, const FLRGachaResult& Result)
{
	// 이미 최종 결과창이 떠 있으면 뒤늦게 도착한 개별 리빌 요청은 무시
	if (bResultOverlayShown)
	{
		return;
	}

	StartTransitionSequence(OrbIndex, Result);
}

void ULRGachaRevealWidget::StartTransitionSequence(int32 OrbIndex, const FLRGachaResult& Result)
{
	if (bTransitionPlaying)
	{
		return;
	}

	if (bPresentationVisible)
	{
		return;
	}

	bTransitionPlaying = true;
	PendingTransitionOrbIndex = OrbIndex;
	PendingTransitionResult = Result;

	if (TransitionOverlay)
	{
		TransitionOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (HintText)
	{
		HintText->SetVisibility(ESlateVisibility::Collapsed);
	}

	ApplyTransitionTexturesByRarity(Result.Rarity);

	BP_PlayTransitionIntro(Result);
}

void ULRGachaRevealWidget::NotifyTransitionFinished()
{
	bTransitionPlaying = false;

	if (TransitionOverlay)
	{
		TransitionOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	BP_OnTransitionClosed();

	if (PendingTransitionOrbIndex != INDEX_NONE && !PendingTransitionResult.ItemID.IsNone())
	{
		ShowPresentation(PendingTransitionOrbIndex, PendingTransitionResult);
	}

	PendingTransitionOrbIndex = INDEX_NONE;
	PendingTransitionResult = FLRGachaResult();

	ResetTransitionVisuals();
}

// ─────────────────────────────────────────────────────────────────────────────
//  개별 리빌 화면 표시/종료
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::ShowPresentation(int32 OrbIndex, const FLRGachaResult& Result)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI)
	{
		return;
	}

	ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>();
	if (!GachaSys)
	{
		return;
	}

	CurrentPresentationResult = Result;
	CurrentPresentationData = FLRGachaRevealPresentationData();

	// 표시 데이터 구성 실패해도 최소한 ItemID 텍스트는 보이도록 fallback 허용
	GachaSys->BuildRevealPresentationData(Result, CurrentPresentationData);

	bPresentationUsingVideo = CurrentPresentationData.bUseVideo;

	ApplyPresentationDataToWidgets(CurrentPresentationData);

	StopActiveRevealSFX();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerRevealSFXDelay);
	}

	if (CurrentPresentationData.RevealSound)
	{
		const float Delay = FMath::Max(0.0f, RevealSFXDelay);

		if (Delay <= 0.0f)
		{
			ActiveRevealSFXComponent = UGameplayStatics::SpawnSound2D(
				this,
				CurrentPresentationData.RevealSound
			);
		}
		else if (GetWorld())
		{
			TWeakObjectPtr<ULRGachaRevealWidget> WeakThis(this);
			USoundBase* RevealSound = CurrentPresentationData.RevealSound;

			GetWorld()->GetTimerManager().SetTimer(
				TimerRevealSFXDelay,
				[WeakThis, RevealSound]()
				{
					if (!WeakThis.IsValid() || !RevealSound)
					{
						return;
					}

					WeakThis->StopActiveRevealSFX();

					WeakThis->ActiveRevealSFXComponent = UGameplayStatics::SpawnSound2D(
						WeakThis.Get(),
						RevealSound
					);
				},
				Delay,
				false
			);
		}
	}

	// 영상이면 반복 재생 시작
	if (bPresentationUsingVideo)
	{
		if (RevealMediaPlayer && CurrentPresentationData.VideoSource)
		{
			RevealMediaPlayer->Close();
			RevealMediaPlayer->OpenSource(CurrentPresentationData.VideoSource);
			RevealMediaPlayer->SetLooping(true);
			RevealMediaPlayer->Play();
		}
	}
	else
	{
		if (RevealMediaPlayer)
		{
			RevealMediaPlayer->Close();
		}
	}

	bPresentationVisible = true;

	if (PresentationOverlay)
	{
		PresentationOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (HintText)
	{
		HintText->SetVisibility(ESlateVisibility::Collapsed);
	}

	BP_OnPresentationOpened(CurrentPresentationData);
}

void ULRGachaRevealWidget::HidePresentation()
{
	StopAllPreviousStageSounds();

	if (!bPresentationVisible)
	{
		return;
	}

	bPresentationVisible = false;

	if (PresentationOverlay)
	{
		PresentationOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (RevealMediaPlayer)
	{
		RevealMediaPlayer->Close();
	}

	if (Image_RevealVideo)
	{
		Image_RevealVideo->SetVisibility(ESlateVisibility::Collapsed);
	}

	bPresentationUsingVideo = false;

	if (OrbSceneActor)
	{
		OrbSceneActor->NotifyPresentationClosed();
	}

	BP_OnPresentationClosed();
}

void ULRGachaRevealWidget::ApplyPresentationDataToWidgets(const FLRGachaRevealPresentationData& InData)
{
	const bool bUseVideo = InData.bUseVideo && InData.VideoSource != nullptr;

	// 배경
	if (Image_RevealBackground)
	{
		UTexture2D* FinalBackgroundTexture = nullptr;

		if (!bUseVideo)
		{
			// 1순위: 공통 배경
			if (bUseCommonRevealBackground && !CommonRevealBackgroundTexture.IsNull())
			{
				FinalBackgroundTexture = CommonRevealBackgroundTexture.LoadSynchronous();
			}

			// 2순위: 개별 배경
			if (!FinalBackgroundTexture && InData.BackgroundTexture)
			{
				FinalBackgroundTexture = InData.BackgroundTexture;
			}
		}

		if (bUseVideo)
		{
			Image_RevealBackground->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (FinalBackgroundTexture)
		{
			Image_RevealBackground->SetBrushFromTexture(FinalBackgroundTexture);
			Image_RevealBackground->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Image_RevealBackground->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 실루엣
	if (Image_RevealSilhouette)
	{
		if (bUseVideo)
		{
			Image_RevealSilhouette->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (InData.MainTexture)
		{
			Image_RevealSilhouette->SetBrushFromTexture(InData.MainTexture);
			Image_RevealSilhouette->SetVisibility(ESlateVisibility::Visible);
			Image_RevealSilhouette->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 1.f));
		}
		else
		{
			Image_RevealSilhouette->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 컬러 메인
	if (Image_RevealMain)
	{
		if (bUseVideo)
		{
			Image_RevealMain->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (InData.MainTexture)
		{
			Image_RevealMain->SetBrushFromTexture(InData.MainTexture);
			Image_RevealMain->SetVisibility(ESlateVisibility::Visible);
			Image_RevealMain->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
		}
		else
		{
			Image_RevealMain->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 이름
	if (Text_RevealName)
	{
		if (!InData.DisplayName.IsEmpty())
		{
			Text_RevealName->SetText(InData.DisplayName);
		}
		else
		{
			Text_RevealName->SetText(FText::FromName(InData.ItemID));
		}

		Text_RevealName->SetColorAndOpacity(FSlateColor(GetNameColorByRarity(InData.Rarity)));
	}

	// 등급 이미지
	if (Image_RarityBadge)
	{
		if (UTexture2D* RarityTexture = GetRarityTextureByRarity(InData.Rarity))
		{
			Image_RarityBadge->SetBrushFromTexture(RarityTexture);
			Image_RarityBadge->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Image_RarityBadge->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// 플래시 이미지
	if (Image_RevealFlash)
	{
		if (bUseVideo)
		{
			Image_RevealFlash->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Image_RevealFlash->SetVisibility(ESlateVisibility::Visible);
			Image_RevealFlash->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
		}
	}

	// 영상 레이어
	if (Image_RevealVideo)
	{
		if (bUseVideo)
		{
			Image_RevealVideo->SetVisibility(ESlateVisibility::Visible);

			if (RevealVideoMaterial && RevealMediaTexture)
			{
				if (!RevealVideoMID)
				{
					RevealVideoMID = UMaterialInstanceDynamic::Create(RevealVideoMaterial, this);
				}

				if (RevealVideoMID)
				{
					RevealVideoMID->SetTextureParameterValue(TEXT("VideoTexture"), RevealMediaTexture);
					Image_RevealVideo->SetBrushFromMaterial(RevealVideoMID);
				}
			}
		}
		else
		{
			Image_RevealVideo->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  OrbSceneActor 찾기/스폰 + 델리게이트 연결
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::FindOrSpawnOrbSceneActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (OrbSceneActor)
	{
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
		OrbSceneActor->OnRevealPresentationRequested.RemoveDynamic(this, &ULRGachaRevealWidget::HandleRevealPresentationRequested);
		OrbSceneActor = nullptr;
	}

	for (TActorIterator<ALRGachaOrbSceneActor> It(World); It; ++It)
	{
		OrbSceneActor = *It;
		break;
	}

	if (!OrbSceneActor && OrbSceneActorClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		OrbSceneActor = World->SpawnActor<ALRGachaOrbSceneActor>(
			OrbSceneActorClass,
			FTransform::Identity,
			Params
		);
	}

	if (OrbSceneActor)
	{
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
		OrbSceneActor->OnAllOrbsRevealed.AddUniqueDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);

		OrbSceneActor->OnRevealPresentationRequested.RemoveDynamic(this, &ULRGachaRevealWidget::HandleRevealPresentationRequested);
		OrbSceneActor->OnRevealPresentationRequested.AddUniqueDynamic(this, &ULRGachaRevealWidget::HandleRevealPresentationRequested);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  입력 모드 강제 / UI 닫기
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::ForceUIInputNextTick()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC || !GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().SetTimerForNextTick(
		[PC]()
		{
			PC->SetShowMouseCursor(true);
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);

			PC->SetInputMode(Mode);
		}
	);
}

void ULRGachaRevealWidget::CloseSelf()
{
	if (UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UISys->CloseUI(this);
	}
	else
	{
		RemoveFromParent();
	}
}

UTexture2D* ULRGachaRevealWidget::GetRarityTextureByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:
		return RarityTextureN.IsNull() ? nullptr : RarityTextureN.LoadSynchronous();

	case ELRGachaRarity::R:
		return RarityTextureR.IsNull() ? nullptr : RarityTextureR.LoadSynchronous();

	case ELRGachaRarity::SR:
		return RarityTextureSR.IsNull() ? nullptr : RarityTextureSR.LoadSynchronous();

	case ELRGachaRarity::SSR:
		return RarityTextureSSR.IsNull() ? nullptr : RarityTextureSSR.LoadSynchronous();

	case ELRGachaRarity::UR:
		return RarityTextureUR.IsNull() ? nullptr : RarityTextureUR.LoadSynchronous();

	default:
		return nullptr;
	}
}

FLinearColor ULRGachaRevealWidget::GetNameColorByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:
		return FLinearColor(0.96f, 0.93f, 0.88f, 1.0f); // 아이보리 베이지

	case ELRGachaRarity::R:
		return FLinearColor(0.63f, 1.00f, 0.25f, 1.0f); // 라임 그린

	case ELRGachaRarity::SR:
		return FLinearColor(0.35f, 0.78f, 1.00f, 1.0f); // 밝은 블루

	case ELRGachaRarity::SSR:
		return FLinearColor(1.00f, 0.42f, 0.95f, 1.0f); // 핑크 마젠타

	case ELRGachaRarity::UR:
		return FLinearColor(1.00f, 0.82f, 0.18f, 1.0f); // 골드 옐로우

	default:
		return FLinearColor::White;
	}
}

UTexture2D* ULRGachaRevealWidget::GetTransitionMagicTextureByRarity(ELRGachaRarity Rarity) const
{
	switch (Rarity)
	{
	case ELRGachaRarity::N:
		return TransitionMagicTextureN.IsNull() ? nullptr : TransitionMagicTextureN.LoadSynchronous();

	case ELRGachaRarity::R:
		return TransitionMagicTextureR.IsNull() ? nullptr : TransitionMagicTextureR.LoadSynchronous();

	case ELRGachaRarity::SR:
		return TransitionMagicTextureSR.IsNull() ? nullptr : TransitionMagicTextureSR.LoadSynchronous();

	case ELRGachaRarity::SSR:
		return TransitionMagicTextureSSR.IsNull() ? nullptr : TransitionMagicTextureSSR.LoadSynchronous();

	case ELRGachaRarity::UR:
		return TransitionMagicTextureUR.IsNull() ? nullptr : TransitionMagicTextureUR.LoadSynchronous();

	default:
		return nullptr;
	}
}

void ULRGachaRevealWidget::ApplyTransitionTexturesByRarity(ELRGachaRarity Rarity)
{
	if (Image_MagicFull)
	{
		if (UTexture2D* MagicTexture = GetTransitionMagicTextureByRarity(Rarity))
		{
			Image_MagicFull->SetBrushFromTexture(MagicTexture);
			Image_MagicFull->SetVisibility(ESlateVisibility::Visible);
			Image_MagicFull->SetColorAndOpacity(FLinearColor::White);
		}
		else
		{
			Image_MagicFull->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	// Glow는 항상 그대로 사용
	if (Image_Glow)
	{
		Image_Glow->SetVisibility(ESlateVisibility::Visible);
		Image_Glow->SetColorAndOpacity(FLinearColor::White);
	}
}

void ULRGachaRevealWidget::ResetTransitionVisuals()
{
	if (Image_Glow)
	{
		Image_Glow->SetVisibility(ESlateVisibility::Visible);
		Image_Glow->SetColorAndOpacity(FLinearColor::White);
	}

	if (Image_MagicFull)
	{
		Image_MagicFull->SetColorAndOpacity(FLinearColor::White);
	}
}

void ULRGachaRevealWidget::ShowFinalResultOverlay()
{
	StopAllPreviousStageSounds();
	StopRevealStageBGM();

	if (bResultOverlayShown)
	{
		return;
	}

	BuildResultSlotsSequential();
	BP_OnAllRevealed(CachedResults);
	bResultOverlayShown = true;

	if (ResultOverlay)
	{
		ResultOverlay->SetVisibility(ESlateVisibility::Visible);
	}

	if (ResultOverlayOpenSound)
	{
		UGameplayStatics::PlaySound2D(this, ResultOverlayOpenSound);
	}
}

void ULRGachaRevealWidget::PlayCurrentColorRevealSound()
{
	StopActiveRevealSFX();

	if (CurrentPresentationData.ColorRevealSound)
	{
		ActiveRevealSFXComponent = UGameplayStatics::SpawnSound2D(
			this,
			CurrentPresentationData.ColorRevealSound
		);
	}
}

void ULRGachaRevealWidget::StopActiveRevealSFX()
{
	if (ActiveRevealSFXComponent)
	{
		ActiveRevealSFXComponent->Stop();
		ActiveRevealSFXComponent = nullptr;
	}
}

void ULRGachaRevealWidget::StopAllResultSlotSounds()
{
	for (ULRGachaResultSlotWidget* SlotWidget : CachedResultSlotWidgets)
	{
		if (SlotWidget)
		{
			SlotWidget->StopAppearSound();
		}
	}
}

void ULRGachaRevealWidget::StopAllPreviousStageSounds()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerRevealSFXDelay);
	}

	StopActiveRevealSFX();
	StopAllResultSlotSounds();

	if (OrbSceneActor)
	{
		OrbSceneActor->StopAllOrbSounds();
	}
}

void ULRGachaRevealWidget::StartRevealStageBGM()
{
	if (!RevealStageBGMSound)
	{
		return;
	}

	if (RevealStageBGMComponent && RevealStageBGMComponent->IsPlaying())
	{
		return;
	}

	RevealStageBGMComponent = UGameplayStatics::SpawnSound2D(this, RevealStageBGMSound);
	if (RevealStageBGMComponent)
	{
		RevealStageBGMComponent->SetVolumeMultiplier(RevealStageBGMVolume);
		RevealStageBGMComponent->bIsUISound = true;
		RevealStageBGMComponent->bAutoDestroy = false;
	}
}

void ULRGachaRevealWidget::StopRevealStageBGM()
{
	if (RevealStageBGMComponent)
	{
		RevealStageBGMComponent->Stop();
		RevealStageBGMComponent = nullptr;
	}
}