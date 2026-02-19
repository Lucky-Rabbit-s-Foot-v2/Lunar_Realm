// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/InGame/LRSummonSlotWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/Pawn.h"
#include "Engine/Texture2D.h"

#include "Units/Player/Component/LRSummonComponent.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Engine/GameInstance.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "Kismet/GameplayStatics.h"

void ULRSummonSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Btn_Summon)
	{
		//LR_INFO(TEXT("버튼 바인딩 성공"));
		Btn_Summon->OnClicked.AddDynamic(this, &ULRSummonSlotWidget::OnSummonButtonClicked);
	}
	else
	{
		LR_ERROR(TEXT("Btn_Summon이 NULL"));
	}

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (OwningPawn)
	{
		SummonComp = OwningPawn->GetComponentByClass<ULRSummonComponent>();
		if (SummonComp)
		{
			SummonComp->OnUnitSummoned.AddDynamic(this, &ULRSummonSlotWidget::OnSummonedEvent);
		}
	}

}

void ULRSummonSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// 쿨타임 UI 갱신
	UpdateCooldownState(InDeltaTime);

	// 버튼 활성/비활성(비용 체크)
	UpdateButtonState();
}

void ULRSummonSlotWidget::InitSlot(int32 InSlotIndex, FName InUnitID)
{
	SlotIndex = InSlotIndex;
	UnitID = InUnitID;

	if (SummonComp == nullptr)
	{
		APawn* OwningPawn = GetOwningPlayerPawn();
		if (OwningPawn)
		{
			SummonComp = OwningPawn->GetComponentByClass<ULRSummonComponent>();
		}
	}

	if (SummonComp)
	{
		if (!SummonComp->OnUnitSummoned.IsAlreadyBound(this, &ULRSummonSlotWidget::OnSummonedEvent))
		{
			SummonComp->OnUnitSummoned.AddDynamic(this, &ULRSummonSlotWidget::OnSummonedEvent);
			//LR_INFO(TEXT("[UI] Slot[%d] 델리게이트 지연 연결 성공"), SlotIndex);
		}
	}

	//LR_INFO(TEXT("[UI] Slot[%d] 초기화 UnitID: %s, Comp: %s"), SlotIndex, *UnitID.ToString(), SummonComp ? TEXT("Found") : TEXT("Missing"));

	// 데이터 조회
	UGameInstance* GI = GetGameInstance();
	if (UGameDataSubsystem* DataSys = GI ? GI->GetSubsystem<UGameDataSubsystem>() : nullptr)
	{
		const FCharacterStaticData& Data = DataSys->GetCharacterStaticData(UnitID);
		SetSlotVisuals(&Data);
	}
}

// ============================================================================
// 내부 로직
// ============================================================================

void ULRSummonSlotWidget::OnSummonedEvent(int32 InSlotIndex, float InCooldownTime)
{
	if (SlotIndex != InSlotIndex) return;

	CurrentCooldown = InCooldownTime;
	TotalCooldown = InCooldownTime;

	if (PB_Cooldown)
	{
		PB_Cooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void ULRSummonSlotWidget::OnSummonButtonClicked()
{
	LR_INFO(TEXT("[UI] 버튼 클릭 SlotIndex: %d, SummonComp: %s"),
		SlotIndex,
		SummonComp ? TEXT("Valid") : TEXT("NULL"));
	if (SummonComp && SlotIndex >= 0)
	{
		SummonComp->TrySummonUnit(SlotIndex);
	}
	else
	{
		LR_ERROR(TEXT("[UI] 클릭 무시됨 초기화가 안 됐거나 컴포넌트가 없음."));
	}
}

// ============================================================================
// 핼퍼함수
// ============================================================================

void ULRSummonSlotWidget::UpdateCooldownState(float InDeltaTime)
{
	if (CurrentCooldown <= 0.0f)
	{
		if (PB_Cooldown && PB_Cooldown->GetVisibility() != ESlateVisibility::Hidden)
		{
			PB_Cooldown->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	CurrentCooldown -= InDeltaTime;

	if (PB_Cooldown)
	{
		// 비율 계산 (0.0 ~ 1.0)
		float Percent = FMath::Clamp(CurrentCooldown / TotalCooldown, 0.0f, 1.0f);
		PB_Cooldown->SetPercent(Percent);
	}
}

void ULRSummonSlotWidget::UpdateButtonState()
{
	if (!SummonComp || !Btn_Summon) return;

	if (CurrentCooldown > 0.0f)
	{
		Btn_Summon->SetIsEnabled(false);
		return;
	}

	bool bCanAfford = false;
	bCanAfford = true;

	Btn_Summon->SetIsEnabled(bCanAfford);
}

void ULRSummonSlotWidget::SetSlotVisuals(const FCharacterStaticData* Data)
{
	if (!Data) return;

	SummonCost = Data->SummonCost;
	TotalCooldown = Data->SummonCooldown; 

	if (Img_Icon && !Data->PortraitIcon.IsNull())
	{
		Img_Icon->SetBrushFromTexture(Data->PortraitIcon.LoadSynchronous());
	}

	if (Txt_Cost)
	{
		Txt_Cost->SetText(FText::AsNumber((int32)SummonCost));
	}
}
