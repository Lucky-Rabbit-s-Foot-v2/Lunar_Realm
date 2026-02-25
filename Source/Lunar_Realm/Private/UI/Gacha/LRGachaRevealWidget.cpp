// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaRevealWidget.h"
#include "UI/Gacha/LRGachaResultSlotWidget.h"
#include "UI/Gacha/LRGachaShopWidget.h"

#include "Actors/Gacha/LRGachaOrbSceneActor.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "EngineUtils.h"

void ULRGachaRevealWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// 스킵 버튼 바인딩
	if (ButtonSkip)
	{
		ButtonSkip->OnClicked.AddDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
	}
}

void ULRGachaRevealWidget::NativeDestruct()
{
	// OrbSceneActor 델리게이트 해제
	if (OrbSceneActor)
	{
		OrbSceneActor->OnOrbClicked.RemoveDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
	}

	// 카메라 ViewTarget 원래대로 복원 (위젯만 닫히는 경우에도 안전)
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (PreviousViewTarget)
		{
			PC->SetViewTargetWithBlend(PreviousViewTarget, 0.5f);
		}
	}

	Super::NativeDestruct();
}

void ULRGachaRevealWidget::StartRevealWithTransaction(
	FName InBannerID,
	FGuid InTxnId,
	const TArray<FLRGachaResult>& InResults)
{
	// 트랜잭션 ID는 나중에 서버 검증/로그용으로 활용 가능
	CachedTxnId = InTxnId;

	// 실제 연출 플로우는 기존 StartReveal 재사용
	StartReveal(InBannerID, InResults);

	// UI/입력 모드 꼬임 방지를 위해 다음 틱에서 Game+UI 모드 세팅
	ForceUIInputNextTick();
}

// ─────────────────────────────────────────────────────────────────────────────
//  외부 진입점 (상점 → 바로 리빌 위젯)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::StartReveal(FName InBannerID, const TArray<FLRGachaResult>& InResults)
{
	CachedBannerID = InBannerID;
	CachedResults = InResults;

	bAllRevealed = false;
	bResultOverlayShown = false;

	// 3D 씬 액터 찾거나 스폰
	FindOrSpawnOrbSceneActor();

	// OrbSceneActor에 결과 주입 → 달 연출 + 구슬 배치 시작
	if (OrbSceneActor)
	{
		OrbSceneActor->InitializeWithResults(CachedResults);

		// 카메라를 OrbSceneActor 쪽으로 전환 (기존 ViewTarget 저장)
		if (APlayerController* PC = GetOwningPlayer())
		{
			PreviousViewTarget = PC->GetViewTarget();
			PC->SetViewTargetWithBlend(OrbSceneActor, 0.5f);
		}
	}

	// BP에 뽑기 수 알림(1회/10회 UI 분기용)
	BP_OnRevealStarted(InResults.Num());
}

void ULRGachaRevealWidget::FinishAndClose()
{
	// 1) UIManager를 통해 이 위젯을 닫는다.
	CloseSelf();

	// 2) 리빌 전용 맵에서 로비(또는 상점맵)으로 복귀
	UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
}

// ─────────────────────────────────────────────────────────────────────────────
//  결과 슬롯 생성
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

	// 5개씩 두 줄(최대 10개)을 깔기 위한 열 개수
	const int32 NumColumns = 5;

	// UniformGridPanel로 배치하면 Row/Col로 깔끔하게 정렬 가능
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

			const int32 Row = Index / NumColumns; // 0,0,0,0,0 / 1,1,1,1,1
			const int32 Col = Index % NumColumns; // 0~4 반복

			UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(SlotWidget, Row, Col);
			if (GridSlot)
			{
				// 필요하면 여기서 각 슬롯 패딩 조정 가능
				// GridSlot->SetPadding(FMargin(8.f));
			}
		}
	}
	else
	{
		// 혹시 다른 PanelWidget으로 교체되어도 최소한 보이게 하는 폴백
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

// ─────────────────────────────────────────────────────────────────────────────
//  스킵 버튼
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::OnClickSkip()
{
	if (!OrbSceneActor)
	{
		return;
	}

	// 1) 아직 전부 안 열렸으면 즉시 모두 리빌
	if (!bAllRevealed)
	{
		OrbSceneActor->SkipAllReveal();
		// SkipAllReveal → OnAllOrbsRevealed → HandleAllOrbsRevealed 에서
		// bAllRevealed = true 로 세팅됨.
	}

	// 2) 결과 오버레이가 아직 안 떠 있으면: 이번 클릭에서 결과 화면으로 전환
	if (!bResultOverlayShown)
	{
		BuildResultSlots();
		BP_OnAllRevealed(CachedResults);
		bResultOverlayShown = true;
		return;
	}

	// 3) 이미 결과 화면이 떠 있는 상태에서 스킵 버튼 → 로비/상점 복귀
	FinishAndClose();
}

// ─────────────────────────────────────────────────────────────────────────────
//  탭/스와이프 입력 처리 (마우스/터치)
// ─────────────────────────────────────────────────────────────────────────────

FReply ULRGachaRevealWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("[Reveal] MouseDown"));

	bIsPointerDown = true;
	PointerDownPosition = InMouseEvent.GetScreenSpacePosition();

	return FReply::Handled();
}

FReply ULRGachaRevealWidget::NativeOnMouseButtonUp(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("[Reveal] MouseUp"));

	if (!bIsPointerDown)
	{
		return FReply::Unhandled();
	}
	bIsPointerDown = false;

	const FVector2D UpPos = InMouseEvent.GetScreenSpacePosition();
	const FVector2D Delta = UpPos - PointerDownPosition;

	// ── [1] 이미 모든 구슬이 열린 상태 ─────────────────────────────
	if (bAllRevealed)
	{
		// 아직 결과 오버레이를 한 번도 안 띄웠으면 → 이번 탭에서 결과 화면 전환
		if (!bResultOverlayShown)
		{
			BuildResultSlots();
			BP_OnAllRevealed(CachedResults);
			bResultOverlayShown = true;
		}
		else
		{
			// 이미 결과 화면이 떠 있는 상태에서 탭 → 상점/로비 복귀
			FinishAndClose();
		}

		return FReply::Handled();
	}

	// ── [2] 아직 구슬이 다 안 열렸을 때만 스와이프/탭 처리 ────────
	if (OrbSceneActor)
	{
		const float AbsX = FMath::Abs(Delta.X);
		const float AbsY = FMath::Abs(Delta.Y);

		// 가로로 많이 움직였으면 “캐러셀 스와이프”
		if (AbsX > SwipeMinDistance && AbsX > AbsY)
		{
			// Delta.X 부호에 따라 좌/우 이동
			OrbSceneActor->OnSwipeInput(-Delta.X);
		}
		else
		{
			// 거의 안 움직였으면 “탭” → 가운데 구슬 리빌
			OrbSceneActor->OnTapCenterOrb();
		}
	}

	return FReply::Handled();
}

// ─────────────────────────────────────────────────────────────────────────────
//  OrbSceneActor 델리게이트 핸들러
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::HandleOrbClicked(int32 OrbIndex)
{
	// 필요 시: UI에 현재 인덱스 표시, 토스트 등 확장 가능.
}

void ULRGachaRevealWidget::HandleAllOrbsRevealed()
{
	// “모든 구슬 리빌 완료” 상태 플래그만 세팅
	bAllRevealed = true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  OrbSceneActor 찾기 / 스폰
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::FindOrSpawnOrbSceneActor()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 1) 레벨에 이미 배치된 OrbSceneActor가 있으면 그걸 사용
	for (TActorIterator<ALRGachaOrbSceneActor> It(World); It; ++It)
	{
		OrbSceneActor = *It;
		break; // 첫 번째 것만 사용
	}

	// 2) 없으면 C++에서 스폰
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

	// 3) 델리게이트 구독
	if (OrbSceneActor)
	{
		OrbSceneActor->OnOrbClicked.AddDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.AddDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
	}
}

// ─────────────────────────────────────────────────────────────────────────────
//  입력 모드 유틸 / UI 닫기
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
		// UIManager가 없으면 안전하게 RemoveFromParent
		RemoveFromParent();
	}
}