// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaRevealWidget.h"
#include "UI/Gacha/LRGachaResultSlotWidget.h"

#include "Actors/Gacha/LRGachaOrbSceneActor.h"

#include "Subsystems/UIManagerSubsystem.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

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
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerSequentialResultSlots);
	}

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

	bIsPointerDown = false;
	PointerDownPosition = FVector2D::ZeroVector;

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
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>())
		{
			GachaSys->RequestOpenShopOnLobbyReturn(CachedBannerID);
		}
	}

	CloseSelf();
	UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
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

	// 개별 리빌 화면이 떠 있으면 먼저 닫기
	if (bPresentationVisible)
	{
		HidePresentation();

		if (bAllRevealed)
		{
			BuildResultSlotsSequential();
			BP_OnAllRevealed(CachedResults);
			bResultOverlayShown = true;

			if (ResultOverlay)
			{
				ResultOverlay->SetVisibility(ESlateVisibility::Visible);
			}
		}

		return;
	}

	if (!bAllRevealed)
	{
		OrbSceneActor->SkipAllReveal();
	}

	if (!bResultOverlayShown)
	{
		BuildResultSlotsSequential();
		BP_OnAllRevealed(CachedResults);
		bResultOverlayShown = true;

		if (ResultOverlay)
		{
			ResultOverlay->SetVisibility(ESlateVisibility::Visible);
		}
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
	bIsPointerDown = true;
	PointerDownPosition = InMouseEvent.GetScreenSpacePosition();

	return FReply::Handled();
}

FReply ULRGachaRevealWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
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

		// 마지막 구슬까지 열린 상태면 바로 결과창으로 전환
		if (bAllRevealed)
		{
			if (!bResultOverlayShown)
			{
				BuildResultSlotsSequential();
				BP_OnAllRevealed(CachedResults);
				bResultOverlayShown = true;

				if (ResultOverlay)
				{
					ResultOverlay->SetVisibility(ESlateVisibility::Visible);
				}
			}
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
	ShowPresentation(Result);
}

// ─────────────────────────────────────────────────────────────────────────────
//  개별 리빌 화면 표시/종료
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::ShowPresentation(const FLRGachaResult& Result)
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

	ApplyPresentationDataToWidgets(CurrentPresentationData);

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
	if (!bPresentationVisible)
	{
		return;
	}

	bPresentationVisible = false;

	if (PresentationOverlay)
	{
		PresentationOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (OrbSceneActor)
	{
		OrbSceneActor->NotifyPresentationClosed();
	}

	BP_OnPresentationClosed();
}

void ULRGachaRevealWidget::ApplyPresentationDataToWidgets(const FLRGachaRevealPresentationData& InData)
{
	// 배경
	if (Image_RevealBackground)
	{
		if (InData.BackgroundTexture)
		{
			Image_RevealBackground->SetBrushFromTexture(InData.BackgroundTexture);
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
		if (InData.MainTexture)
		{
			Image_RevealSilhouette->SetBrushFromTexture(InData.MainTexture);
			Image_RevealSilhouette->SetVisibility(ESlateVisibility::Visible);

			// 검은 실루엣 처리
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
		if (InData.MainTexture)
		{
			Image_RevealMain->SetBrushFromTexture(InData.MainTexture);
			Image_RevealMain->SetVisibility(ESlateVisibility::Visible);

			// 시작 시 투명 → BP 애니메이션 또는 후속 확장으로 자연스럽게 노출
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
	}

	// 플래시 이미지
	if (Image_RevealFlash)
	{
		Image_RevealFlash->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 0.f));
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