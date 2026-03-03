// LRGachaRevealWidget.cpp
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

// 테스트 시연용
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

	// 스킵 버튼 바인딩
	// - PIE 재실행/위젯 재사용 시 중복 바인딩이 생기면 클릭이 여러 번 실행될 수 있으니 제거 후 AddUnique
	if (ButtonSkip)
	{
		ButtonSkip->OnClicked.RemoveDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
		ButtonSkip->OnClicked.AddUniqueDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
	}
}

void ULRGachaRevealWidget::NativeDestruct()
{
	// 버튼 델리게이트 해제(스테일 포인터/중복 호출 방지)
	if (ButtonSkip)
	{
		ButtonSkip->OnClicked.RemoveDynamic(this, &ULRGachaRevealWidget::OnClickSkip);
	}

	// OrbSceneActor 델리게이트 해제
	if (OrbSceneActor)
	{
		OrbSceneActor->OnOrbClicked.RemoveDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
	}

	// 카메라 ViewTarget 복원
	// - 리빌 위젯이 닫힐 때(레벨 이동 없이 닫힐 수도 있음) 카메라가 OrbSceneActor에 고정되면 안 됨
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
	// 트랜잭션 ID는 “서버 검증/로그/재처리” 확장용으로 캐시
	CachedTxnId = InTxnId;

	// 실제 연출 로직은 StartReveal에서 공통 처리
	StartReveal(InBannerID, InResults);
}

void ULRGachaRevealWidget::StartReveal(FName InBannerID, const TArray<FLRGachaResult>& InResults)
{
	// ── 0) 이번 리빌의 입력/상태 초기화 ───────────────────────────

	CachedBannerID = InBannerID;
	CachedResults = InResults;

	bAllRevealed = false;
	bResultOverlayShown = false;

	// 두 번째 리빌에서 MouseUp 무반응 같은 문제 방지(포인터 상태 리셋)
	bIsPointerDown = false;
	PointerDownPosition = FVector2D::ZeroVector;

	// ── 1) 결과 UI 초기화(이전 결과 잔상 제거) ─────────────────────

	if (ResultSlotContainer)
	{
		ResultSlotContainer->ClearChildren();
	}

	if (ResultOverlay)
	{
		// 시작 시 결과 오버레이는 숨김(최종 단계에서만 표시)
		ResultOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (HintText)
	{
		// “탭해서 열기” 등의 힌트는 시작 시 표시
		HintText->SetVisibility(ESlateVisibility::Visible);
	}

	// 테스트 시연용
	if (Text_DebugOrbIndex)
	{
		Text_DebugOrbIndex->SetVisibility(ESlateVisibility::Collapsed);
		Text_DebugOrbIndex->SetText(FText::GetEmpty());
	}

	// ── 2) 입력 모드 강제(중요) ───────────────────────────────────
	// - 두 번째 리빌부터 클릭이 안 먹는 대표 원인: InputMode가 GameOnly로 남아있음
	ForceUIInputNextTick();

	// ── 3) 3D 연출 씬 액터 연결 ───────────────────────────────────

	FindOrSpawnOrbSceneActor();

	if (OrbSceneActor)
	{
		// OrbSceneActor에게 결과 주입 → 달 연출 + 구슬 배치 시작
		OrbSceneActor->InitializeWithResults(CachedResults);

		// 카메라를 OrbSceneActor로 전환(원래 ViewTarget은 저장해두고 복원)
		if (APlayerController* PC = GetOwningPlayer())
		{
			PreviousViewTarget = PC->GetViewTarget();
			PC->SetViewTargetWithBlend(OrbSceneActor, 0.5f);
		}
	}

	// ── 4) BP에 “뽑기 수” 알림(1회/10회 UI 분기) ───────────────────
	BP_OnRevealStarted(InResults.Num());
}

// ─────────────────────────────────────────────────────────────────────────────
//  리빌 종료(로비 복귀)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::FinishAndClose()
{
	// 로비로 돌아가면 “샵을 자동으로 다시 열기” 요청
	// - 리빌 맵은 연출 전용이라, 로비로 돌아오면 UX상 샵으로 이어지는 흐름이 자연스러움
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>())
		{
			GachaSys->RequestOpenShopOnLobbyReturn(CachedBannerID);
		}
	}

	// 1) 리빌 위젯 닫기(레벨 이동 전에 UIManager 캐시 정리)
	CloseSelf();

	// 2) 로비 레벨로 이동
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

	// 10회 뽑기 기준: 5열 x 2행
	const int32 NumColumns = 5;

	// UniformGridPanel이면 Row/Col로 안정적으로 배치 가능
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

			const int32 Row = Index / NumColumns; // 0..4 -> Row 0, 5..9 -> Row 1
			const int32 Col = Index % NumColumns; // 0..4 반복

			UUniformGridSlot* GridSlot = Grid->AddChildToUniformGrid(SlotWidget, Row, Col);
			if (GridSlot)
			{
				// 필요 시 슬롯 패딩/정렬을 여기서 조정
				// GridSlot->SetPadding(FMargin(8.f));
			}
		}
	}
	else
	{
		// 폴백: 컨테이너가 다른 PanelWidget이라도 최소한 출력되게
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
//  스킵 버튼(단일 버튼 3단 동작)
// ─────────────────────────────────────────────────────────────────────────────

void ULRGachaRevealWidget::OnClickSkip()
{
	if (!OrbSceneActor)
	{
		return;
	}

	// 1) 아직 전부 안 열렸으면: 즉시 전체 리빌
	if (!bAllRevealed)
	{
		OrbSceneActor->SkipAllReveal();
		// SkipAllReveal -> OrbSceneActor가 OnAllOrbsRevealed 브로드캐스트
		// -> HandleAllOrbsRevealed()에서 bAllRevealed=true 세팅됨
	}

	// 2) 결과 오버레이가 아직 안 떴으면: 결과 슬롯 생성 + BP 이벤트 호출
	if (!bResultOverlayShown)
	{
		BuildResultSlots();
		BP_OnAllRevealed(CachedResults);
		bResultOverlayShown = true;
		return;
	}

	// 3) 이미 결과 화면이면: 로비 복귀
	FinishAndClose();
}

// ─────────────────────────────────────────────────────────────────────────────
//  마우스/터치 입력 처리(탭/스와이프)
// ─────────────────────────────────────────────────────────────────────────────

FReply ULRGachaRevealWidget::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// Down에서 위치 저장 → Up에서 Delta로 스와이프/탭 판정
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

	// ── [1] 이미 모든 구슬이 열린 상태: 탭으로 결과/복귀 처리 ───────
	if (bAllRevealed)
	{
		if (!bResultOverlayShown)
		{
			BuildResultSlots();
			BP_OnAllRevealed(CachedResults);
			bResultOverlayShown = true;
		}
		else
		{
			FinishAndClose();
		}
		return FReply::Handled();
	}

	// ── [2] 아직 리빌 중: 스와이프/탭을 OrbSceneActor로 전달 ───────
	if (OrbSceneActor)
	{
		const float AbsX = FMath::Abs(Delta.X);
		const float AbsY = FMath::Abs(Delta.Y);

		// 가로 이동이 크면 스와이프(캐러셀 이동)
		if (AbsX > SwipeMinDistance && AbsX > AbsY)
		{
			OrbSceneActor->OnSwipeInput(-Delta.X);
		}
		else
		{
			// 거의 움직이지 않으면 탭(가운데 구슬 리빌)
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
	// UI 텍스트가 없으면 무시
	if (!Text_DebugOrbIndex)
	{
		return;
	}

	// 안전장치: 인덱스가 이상하면 숨김 처리
	if (!CachedResults.IsValidIndex(OrbIndex))
	{
		Text_DebugOrbIndex->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	// OrbIndex -> 결과 배열에서 ItemID 가져오기
	const FLRGachaResult& Result = CachedResults[OrbIndex];
	const FName ItemID = Result.ItemID;

	// 텍스트 표시
	Text_DebugOrbIndex->SetText(FText::FromName(ItemID));
	Text_DebugOrbIndex->SetVisibility(ESlateVisibility::Visible);

	// 등급별 텍스트 색 적용 (게임에서도 그대로 사용)
	FLinearColor TextColor = FLinearColor::White;
	switch (Result.Rarity)
	{
	case ELRGachaRarity::Common:    TextColor = FLinearColor(0.8f, 0.8f, 0.8f, 1.f); break;
	case ELRGachaRarity::Elite:     TextColor = FLinearColor(0.1f, 0.4f, 1.0f, 1.f); break;
	case ELRGachaRarity::Unique:    TextColor = FLinearColor(0.9f, 0.1f, 0.1f, 1.f); break;
	case ELRGachaRarity::Epic:      TextColor = FLinearColor(0.5f, 0.1f, 0.9f, 1.f); break;
	case ELRGachaRarity::Legendary: TextColor = FLinearColor(1.0f, 0.75f, 0.0f, 1.f); break;
	default: break;
	}
	Text_DebugOrbIndex->SetColorAndOpacity(FSlateColor(TextColor));

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(Timer_DebugOrbIndex);

		GetWorld()->GetTimerManager().SetTimer(
			Timer_DebugOrbIndex,
			[this]()
			{
				if (Text_DebugOrbIndex)
				{
					Text_DebugOrbIndex->SetVisibility(ESlateVisibility::Collapsed);
				}
			},
			DebugOrbIndexDisplayTime,
			false
		);
	}
}

void ULRGachaRevealWidget::HandleAllOrbsRevealed()
{
	// 모든 구슬 리빌 완료 플래그만 세팅
	// - 결과 오버레이 표시/복귀는 OnClickSkip 또는 탭에서 처리
	bAllRevealed = true;
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

	// 이전 참조가 남아있으면 델리게이트 해제 후 포인터 정리
	if (OrbSceneActor)
	{
		OrbSceneActor->OnOrbClicked.RemoveDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
		OrbSceneActor = nullptr;
	}

	// 1) 레벨에 이미 배치된 OrbSceneActor가 있으면 재사용
	for (TActorIterator<ALRGachaOrbSceneActor> It(World); It; ++It)
	{
		OrbSceneActor = *It;
		break;
	}

	// 2) 없으면 클래스가 지정된 경우 스폰
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

	// 3) 델리게이트 구독(중복 방지 위해 Remove 후 AddUnique)
	if (OrbSceneActor)
	{
		OrbSceneActor->OnOrbClicked.RemoveDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.RemoveDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);

		OrbSceneActor->OnOrbClicked.AddUniqueDynamic(this, &ULRGachaRevealWidget::HandleOrbClicked);
		OrbSceneActor->OnAllOrbsRevealed.AddUniqueDynamic(this, &ULRGachaRevealWidget::HandleAllOrbsRevealed);
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

	// 다음 틱에 SetInputMode를 적용
	// - 위젯 생성 직후/레벨 로드 직후에는 타이밍 이슈로 즉시 적용이 씹히는 경우가 있음
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
	// UIManagerSubsystem이 있으면 그쪽을 통해 닫아야 캐시/스택이 정상 유지됨
	if (UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UISys->CloseUI(this);
	}
	else
	{
		// 폴백
		RemoveFromParent();
	}
}