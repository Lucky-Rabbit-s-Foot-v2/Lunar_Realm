#include "UI/InGame/LRReadyStartWidget.h"

#include "Animation/WidgetAnimation.h"
#include "Engine/GameInstance.h"
#include "Subsystems/UIManagerSubsystem.h"
#include "System/LoggingSystem.h"

void ULRReadyStartWidget::OpenUI()
{
	Super::OpenUI();

	// Ready/Start 표시 중 입력 차단위해 Visible로 덮어씀
	SetVisibility(ESlateVisibility::Visible);

	if (AnimReadyStart)
	{
		// 애니메이션 종료 콜백 바인딩
		FWidgetAnimationDynamicEvent AnimFinishedEvent;
		AnimFinishedEvent.BindDynamic(this, &ULRReadyStartWidget::OnSequenceAnimFinished);
		BindToAnimationEvent(AnimReadyStart, AnimFinishedEvent, EWidgetAnimationEvent::Finished);

		PlayAnimation(AnimReadyStart);

		// TEST
		LR_INFO(TEXT("[ReadyStartWidget] Animation started. Duration: %.2f sec"), AnimReadyStart->GetEndTime());
	}
	else
	{
		LR_WARN(TEXT("[ReadyStartWidget] AnimReadyStart is null — skipping animation"));
		OnSequenceAnimFinished();
	}
}

void ULRReadyStartWidget::NativeDestruct()
{

	OnReadySequenceFinished.Clear();
	Super::NativeDestruct();
}

void ULRReadyStartWidget::OnSequenceAnimFinished()
{
	// TEST
	LR_INFO(TEXT("[ReadyStartWidget] Animation finished. Broadcasting OnReadySequenceFinished."));

	OnReadySequenceFinished.Broadcast();

	CleanupWidget();
}

void ULRReadyStartWidget::CleanupWidget()
{
	// 입력 차단 해제
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	OnReadySequenceFinished.Clear();

	// UIManager를 통해 닫기
	if (UUIManagerSubsystem* UIManager = GetGameInstance()->GetSubsystem<UUIManagerSubsystem>())
	{
		UIManager->CloseUI(this);
	}
}