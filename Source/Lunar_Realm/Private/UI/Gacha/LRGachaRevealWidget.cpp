// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaRevealWidget.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "UI/Gacha/LRGachaShopWidget.h"

void ULRGachaRevealWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

// 트랜잭션 포함 시작
void ULRGachaRevealWidget::StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults)
{
	CachedBannerId = InBannerID;
	CachedTxnId = InTxnId;
	CachedResults = InResults;

	SkipState = 0;
	bSkipResultsShown = false;

	// UI 입력/커서 강제(리빌에서 커서 안 보이는 문제 방지)
	ForceUIInputNextTick();
	BP_PlayRevealSequence(CachedResults);
}

void ULRGachaRevealWidget::ForceUIInputNextTick()
{
	if (!GetWorld()) return;

	// 다음 틱에 실행 (UIManager가 만진 뒤 다시 덮어쓰기)
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
		{
			APlayerController* PC = GetOwningPlayer();
			if (!PC) return;

			// 커서 켜기
			PC->SetShowMouseCursor(true);

			// UI 조작 가능하게 입력 모드
			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);
			PC->SetInputMode(Mode);
		});
}

void ULRGachaRevealWidget::RestoreGachaInputAfterCloseNextTick()
{
	if (!GetWorld()) return;

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	// 다음 틱에 실행 (UIManager가 Close하면서 InputMode/커서 만진 뒤 다시 덮어쓰기)
	GetWorld()->GetTimerManager().SetTimerForNextTick([PC]()
		{
			// 커서 켜기
			PC->SetShowMouseCursor(true);
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			// UI 조작 가능하게 입력 모드
			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);

			PC->SetInputMode(Mode);
		});
}


// 리빌 종료(결과 확인까지 끝났을 때 호출)
void ULRGachaRevealWidget::FinishRevealAndClose()
{
	// 닫기 전에, 다음 틱에 커서/입력 복구 예약
	RestoreGachaInputAfterCloseNextTick();

	// 여기서 추가로 결과창이 따로 있다면 결과창을 닫는 흐름으로 바꿔도 됨
	CloseSelfPopup();
}

// UIManager로 닫기
void ULRGachaRevealWidget::CloseSelfPopup()
{
	UUIManagerSubsystem* UISys = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>();
	if (UISys)
	{
		UISys->CloseUI(this);
	}
	else
	{
		RemoveFromParent();
	}
}

FReply ULRGachaRevealWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// 이미 스킵 결과를 보여준 상태면, 다음 클릭은 닫기
	if (SkipState >= 2 && bSkipResultsShown)
	{
		FinishRevealAndClose();   // 또는 CloseSelfPopup();
		return FReply::Handled();
	}

	// 0 -> 1 -> 2 로 진행 (2는 스킵)
	SkipState = FMath::Clamp(SkipState + 1, 0, 2);
	BP_OnSkipStateChanged(SkipState);

	// 스킵(2)로 들어간 순간: "결과를 보여주는 단계"에 진입했음을 표시
	if (SkipState >= 2)
	{
		bSkipResultsShown = true;
	}

	return FReply::Handled();
}
