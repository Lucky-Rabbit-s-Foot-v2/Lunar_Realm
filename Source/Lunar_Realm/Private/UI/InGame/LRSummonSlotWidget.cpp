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
#include "Materials/MaterialInstanceDynamic.h"

void ULRSummonSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Img_Cooldown)
	{
		CooldownMID = Img_Cooldown->GetDynamicMaterial();
	}

	if (APawn* OwningPawn = GetOwningPlayerPawn())
	{
		SummonComp = OwningPawn->GetComponentByClass<ULRSummonComponent>();
		if (SummonComp)
		{
			SummonComp->OnUnitSummoned.AddUniqueDynamic(this, &ULRSummonSlotWidget::OnSummonedEvent);
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

void ULRSummonSlotWidget::BindProperties()
{
	Super::BindProperties();

	if (Btn_Summon) Btn_Summon->OnClicked.AddDynamic(this, &ULRSummonSlotWidget::OnSummonButtonClicked);
}

void ULRSummonSlotWidget::UnbindProperties()
{
	Btn_Summon->OnClicked.Clear();

	Super::UnbindProperties();
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

	//if (PB_Cooldown)
	//{
	//	PB_Cooldown->SetVisibility(ESlateVisibility::HitTestInvisible);
	//}
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
		if (Img_Cooldown && Img_Cooldown->GetVisibility() != ESlateVisibility::Hidden)
		{
			Img_Cooldown->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	CurrentCooldown -= InDeltaTime;

	// 쿨타임 UI 켜기
	if (Img_Cooldown && Img_Cooldown->GetVisibility() != ESlateVisibility::SelfHitTestInvisible)
	{
		Img_Cooldown->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}

	// 머티리얼의 Percent 파라미터에 남은 비율(0.0 ~ 1.0) 전달하기
	if (CooldownMID)
	{
		float Percent = FMath::Clamp(CurrentCooldown / TotalCooldown, 0.0f, 1.0f);
		CooldownMID->SetScalarParameterValue(FName("Percent"), Percent);
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
	if (!Data)
	{
		Img_Icon->SetBrushFromTexture(DefaultIcon);
		return;
	}

	SummonCost = Data->SummonCost;
	TotalCooldown = Data->SummonCooldown; 

	// 캐릭터 아이콘
	if (Img_Icon && !Data->CharacterTexture.IsNull())
	{
		Img_Icon->SetBrushFromTexture(Data->CharacterTexture.LoadSynchronous());
	}
	else
	{
		Img_Icon->SetBrushFromTexture(DefaultIcon);
	}

	// 코스트 텍스트 적용
	if (Txt_Cost)
	{
		Txt_Cost->SetText(FText::AsNumber((int32)SummonCost));
	}

	// 등급별 테두리
	if (Img_Border)
	{
		ELRGrade CharacterRarity = Data->Grade;

		if (UTexture2D** FoundTexture = RarityBorderMap.Find(CharacterRarity))
		{
			if (*FoundTexture)
			{
				Img_Border->SetBrushFromTexture(*FoundTexture);
				Img_Border->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
		}
		else
		{
			Img_Border->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}
