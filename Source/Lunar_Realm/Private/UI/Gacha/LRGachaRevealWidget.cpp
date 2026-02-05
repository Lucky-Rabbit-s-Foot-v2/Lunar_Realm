// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaRevealWidget.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "Engine/GameInstance.h"
#include "TimerManager.h"
#include "GameFramework/PlayerController.h"

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

	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	GetWorld()->GetTimerManager().SetTimerForNextTick([PC]()
		{
			PC->SetShowMouseCursor(true);
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);

			PC->SetInputMode(Mode);
		});
}

// 리빌 종료(결과 확인까지 끝났을 때 호출)
void ULRGachaRevealWidget::FinishRevealAndClose()
{
	// 닫기 전에 다음 틱에 UI 입력/커서 덮어쓰기 예약
	ForceUIInputNextTick();

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
