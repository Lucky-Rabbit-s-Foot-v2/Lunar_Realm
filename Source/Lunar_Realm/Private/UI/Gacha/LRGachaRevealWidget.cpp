// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaRevealWidget.h"
#include "Subsystems/Gacha/LRGachaSubsystem.h"
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
	BP_PlayRevealSequence(CachedResults);
}

// 지급 확정
void ULRGachaRevealWidget::ConfirmCommit()
{
	ULRGachaSubsystem* GachaSys = GetGameInstance()->GetSubsystem<ULRGachaSubsystem>();
	if (GachaSys && CachedTxnId.IsValid())
	{
		GachaSys->CommitTransaction(CachedTxnId);
	}
	CloseSelfPopup();
}

// 취소/환불(테스트용 혹은 뒤로가기)
void ULRGachaRevealWidget::CancelAndRefund()
{
	ULRGachaSubsystem* GachaSys = GetGameInstance()->GetSubsystem<ULRGachaSubsystem>();
	if (GachaSys && CachedTxnId.IsValid())
	{
		GachaSys->CancelTransaction(CachedTxnId);
	}
	CloseSelfPopup();
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
	SkipState = FMath::Clamp(SkipState + 1, 0, 2);
	BP_OnSkipStateChanged(SkipState);
	return FReply::Handled();
}
