// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberCharacter.h"
#include "Animation/AnimInstance.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AIController.h" 
#include "System/LoggingSystem.h"
#include "Components/SkeletalMeshComponent.h"


ALRMemberCharacter::ALRMemberCharacter()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);	//cpu 낭비 줄이기

	MemberAttributeSet = CreateDefaultSubobject<ULRPlayerAttributeSet>(TEXT("AttributeSet"));

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

UAbilitySystemComponent* ALRMemberCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRMemberCharacter::BeginPlay()
{

	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("[Member] BeginPlay Called!"));

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		if (MemberAttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				ULRPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRMemberCharacter::OnHealthChangedNative);
		}
	}
}

void ALRMemberCharacter::OnHealthChangedNative(const FOnAttributeChangeData& Data)
{
	OnHealthChanged(Data.NewValue);
}

void ALRMemberCharacter::OnPoolActivate_Implementation()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);

	ResetAttributes();
	ResetAIController();

	LR_INFO(TEXT("Member 풀에서 소환 및 초기화 완료 : %s"), *GetName());
}

void ALRMemberCharacter::OnPoolDeactivate_Implementation()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (AController* AICon = GetController())
	{
		AICon->StopMovement();
	}
}

void ALRMemberCharacter::InitCharacterData(FName InCharacterID)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return;

	const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(InCharacterID);

	if (CharData.RowName == NAME_None)
	{
		LR_ERROR(TEXT("옷 갈아입기 실패: 데이터 테이블에서 ID [%s]를 찾을 수 없습니다! (행 이름 확인)"), *InCharacterID.ToString());
		return;
	}

	if (!CharData.CharacterMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = CharData.CharacterMesh.LoadSynchronous();
		if (LoadedMesh && GetMesh())
		{
			GetMesh()->SetSkeletalMesh(LoadedMesh);
			LR_INFO(TEXT("[%s] 메시 적용 성공!"), *InCharacterID.ToString());
		}
		else
		{
			LR_WARN(TEXT("[%s] 메시 로드 실패! 에셋 경로를 확인하세요."), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] 데이터 테이블에 CharacterMesh가 비어있습니다!"), *InCharacterID.ToString());
	}

	if (!CharData.AnimBlueprintClass.IsNull())
	{
		TSubclassOf<UAnimInstance> LoadedAnim = CharData.AnimBlueprintClass.LoadSynchronous();
		if (LoadedAnim && GetMesh())
		{
			GetMesh()->SetAnimInstanceClass(LoadedAnim);
			LR_INFO(TEXT("[%s] 애님 블루프린트 적용 성공!"), *InCharacterID.ToString());
		}
		else
		{
			LR_WARN(TEXT("[%s] 애님 블루프린트 로드 실패!"), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] 데이터 테이블에 AnimBlueprintClass가 비어있습니다!"), *InCharacterID.ToString());
	}

	// (참고: 필요하면 여기서 CharData의 스탯을 이용해 체력/공격력 세팅을 추가할 수도 있음)
	LR_INFO(TEXT("[%s] 캐릭터 데이터 세팅 완료!"), *InCharacterID.ToString());
}

void ALRMemberCharacter::ResetAttributes()
{
	if (MemberAttributeSet)
	{
		float MaxHP = MemberAttributeSet->GetMaxHealth();
		if (MaxHP <= 0.0f)
		{
			MaxHP = 50.0f;
		}

		MemberAttributeSet->InitHealth(MaxHP);
		MemberAttributeSet->InitMaxHealth(MaxHP);
		MemberAttributeSet->InitAttackPower(5.0f);
		
	}
}
void ALRMemberCharacter::ResetAIController()
{
	if (Controller == nullptr && AIControllerClass)
	{
		SpawnDefaultController();
	}
}
