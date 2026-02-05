// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Gacha/LRGachaRevealWidget.h"

#include "Subsystems/UIManagerSubsystem.h"

#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "TimerManager.h"

void ULRGachaRevealWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

// 트랜잭션 포함 시작
void ULRGachaRevealWidget::StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults)
{
	// 결과 캐시(스킵/다음 클릭 처리에 필요)
	CachedBannerId = InBannerID;
	CachedTxnId = InTxnId;
	CachedResults = InResults;

	SkipState = 0;
	bSkipResultsShown = false;

	// 리빌에서 커서/입력모드가 꺼지는 문제 방지
	// UIManager가 뭘 건드려도 다음 틱에 다시 덮어씀
	ForceUIInputNextTick();

	// 실제 연출은 BP에서 구현
	BP_PlayRevealSequence(CachedResults);
}

void ULRGachaRevealWidget::ForceUIInputNextTick()
{
	if (!GetWorld()) return;

	// 위젯(this)을 캡처하면 닫힌 뒤에 크래시 위험이 있으니 PC만 캡처
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	GetWorld()->GetTimerManager().SetTimerForNextTick([PC]()
		{
			// 커서/클릭/오버 이벤트 활성화
			PC->SetShowMouseCursor(true);
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;

			// UI 조작 가능한 입력 모드
			FInputModeGameAndUI Mode;
			Mode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			Mode.SetHideCursorDuringCapture(false);

			PC->SetInputMode(Mode);
		});
}

// 리빌 종료(결과 확인까지 끝났을 때 호출)
void ULRGachaRevealWidget::FinishRevealAndClose()
{
	// 닫는 순간 UIManager가 커서를 꺼버릴 수 있으므로,
	// 닫기 직전에 다음 틱 덮어쓰기 예약을 한 번 더 걸어준다.
	ForceUIInputNextTick();

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
		FinishRevealAndClose();   
		return FReply::Handled();
	}

	// 0 -> 1 -> 2 로 진행 (2는 스킵)
	SkipState = FMath::Clamp(SkipState + 1, 0, 2);
	BP_OnSkipStateChanged(SkipState);

	// SkipState=2 진입 순간: 결과를 다 보여준 상태로 판단
	if (SkipState >= 2)
	{
		bSkipResultsShown = true;
	}

	return FReply::Handled();
}
