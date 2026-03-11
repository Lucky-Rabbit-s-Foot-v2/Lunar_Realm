// Fill out your copyright notice in the Description page of Project Settings.


#include "Units/Member/LRMemberCharacter.h"
#include "Animation/AnimInstance.h"
#include "Engine/GameInstance.h"
#include "Subsystems/GameDataSubsystem.h"
#include "Subsystems/PoolingSubsystem.h"
#include "AbilitySystemComponent.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "AIController.h" 
#include "System/LoggingSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "BrainComponent.h"
#include "BehaviorTree/BehaviorTree.h"  
#include "BehaviorTree/BehaviorTreeComponent.h"  
#include "BehaviorTree/BlackboardComponent.h"

#include "Units/LRAIController.h"      
#include "Units/Member/LRMemberAIController.h"
#include "GAS/Ability/LRGameplayAbilityBase.h"

#include "Actors/Equipment/LREquipmentBase.h"

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
				ULRAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRMemberCharacter::OnHealthChangedNative);
		}
	}
}

void ALRMemberCharacter::OnHealthChangedNative(const FOnAttributeChangeData& Data)
{
	float NewHealth = Data.NewValue;
	float OldHealth = Data.OldValue;

	if (bIsDead)
	{
		return;
	}
	if (NewHealth <= 0.0f)
	{
		Die();
		return;
	}
	if (NewHealth < OldHealth)
	{
		if (LoadedHitMontage)
		{
			PlayAnimMontage(LoadedHitMontage);
		}
	}

	OnHealthChanged(Data.NewValue);
}

void ALRMemberCharacter::OnPoolActivate_Implementation()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			AnimInst->StopAllMontages(0.0f);
		}

		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight()));
		MeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->StopMovementImmediately();
		MoveComp->Velocity = FVector::ZeroVector;
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->SetActive(true);
		MoveComp->bWantsToCrouch = false;
	}

	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	SetActorTickEnabled(true);
	bIsDead = false; 

	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->SetActorHiddenInGame(false);
		CurrentWeaponActor->SetActorTickEnabled(true);
	}

	if (UCapsuleComponent* Capsule = GetCapsuleComponent())
	{
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionProfileName(TEXT("Pawn"));
	}

	//ResetAttributes();
	ResetAIController();

	GetCharacterMovement()->bOrientRotationToMovement = true;

	LR_INFO(TEXT("Member 풀에서 소환 및 초기화 완료 : %s"), *GetName());
}

void ALRMemberCharacter::OnPoolDeactivate_Implementation()
{

	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	SetActorTickEnabled(false);

	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->SetActorHiddenInGame(true);
		CurrentWeaponActor->SetActorTickEnabled(false);
	}

	if (ALRAIController* AICon = Cast<ALRAIController>(GetController()))
	{
		AICon->StopMovement();
		//AICon->UnPossess();
		if (UBehaviorTreeComponent* MemberBT = Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()))
		{
			MemberBT->StopTree();
		}

	}
}

void ALRMemberCharacter::InitCharacterData(FName InCharacterID)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	UGameDataSubsystem* DataSys = GI->GetSubsystem<UGameDataSubsystem>();
	if (!DataSys) return;

	const FCharacterStaticData& CharData = DataSys->GetCharacterStaticData(InCharacterID);

	if (CharData.DataID == NAME_None)
	{
		LR_ERROR(TEXT("DT에서 ID [%s]를 찾을 수 없음"), *InCharacterID.ToString());
		return;
	}

	// 캐릭터 매쉬 로드 및 캐싱
	if (!CharData.CharacterMesh.IsNull())
	{
		USkeletalMesh* LoadedMesh = CharData.CharacterMesh.LoadSynchronous();
		if (LoadedMesh && GetMesh())
		{
			GetMesh()->SetSkeletalMeshAsset(LoadedMesh);
			LR_INFO(TEXT("[%s] 메시 적용 성공!"), *InCharacterID.ToString());
		}
		else
		{
			LR_WARN(TEXT("[%s] 메시 로드 실패."), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] 데이터 테이블에 CharacterMesh가 빔!"), *InCharacterID.ToString());
	}

	// 애님 블루프린트 로드 및 캐싱
	if (!CharData.AnimBlueprintClass.IsNull())
	{
		TSubclassOf<UAnimInstance> LoadedAnim = CharData.AnimBlueprintClass.LoadSynchronous();
		if (LoadedAnim && GetMesh())
		{
			GetMesh()->SetAnimInstanceClass(LoadedAnim);
			LR_INFO(TEXT("[%s] ABP 적용 성공"), *InCharacterID.ToString());
		}
		else
		{ 
			LR_WARN(TEXT("[%s] ABP 로드 실패"), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] DT에 AnimBlueprintClass가 비어있음"), *InCharacterID.ToString());
	}

	// 사망 몽타주 로드 및 캐싱
	if (!CharData.DeathMontage.IsNull())
	{
		LoadedDeathMontage = CharData.DeathMontage.LoadSynchronous();
		if (LoadedDeathMontage)
		{
			LR_INFO(TEXT("[%s] 사망 몽타주 로드 성공"), *InCharacterID.ToString());
		}
	}
	else
	{
		LoadedDeathMontage = nullptr;
		LR_WARN(TEXT("[%s] DT에 사망 몽타주가 비어있습니다. (기본값 사용 주의)"), *InCharacterID.ToString());
	}

	// TODO : 공격 몽타주 추가
	// 공격 몽타주 로드 및 캐싱
	if (!CharData.NormalAttackMontage.IsNull())
	{
		LoadedAttackMontage = CharData.NormalAttackMontage.LoadSynchronous();
		LR_INFO(TEXT("[%s] 공격 몽타주 로드 성공"), *InCharacterID.ToString());
	}
	else
	{
		LoadedAttackMontage = nullptr;
	}

	// Behavior Tree 실행 로직
	if (CharData.BehaviorTree)
	{
		// 컨트롤러가 없으면 강제로 만듦
		if (GetController() == nullptr)
		{
			SpawnDefaultController();
		}

		// ALRAIController로 캐스팅해서 BT 실행
		if (ALRMemberAIController* AIC = Cast<ALRMemberAIController>(GetController()))
		{
			AIC->InitializeBehaviorTree(CharData.BehaviorTree);

			AIC->RestartAI(); 

			LR_INFO(TEXT("[%s] BT 실행 성공: %s"), *InCharacterID.ToString(), *CharData.BehaviorTree->GetName());
		}
		else
		{
			LR_WARN(TEXT("[%s] Controller가 ALRAIController가 아님"), *InCharacterID.ToString());
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] DT에 BehaviorTree가 없음."), *InCharacterID.ToString());
	}

	// GA 로직
	if (AbilitySystemComponent && HasAuthority())
	{
		AbilitySystemComponent->ClearAllAbilities();

		for (TSubclassOf<ULRGameplayAbilityBase> AbilityClass : CharData.MemberAbilities)
		{
			if (AbilityClass)
			{
				AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, INDEX_NONE, this));
				LR_INFO(TEXT("[%s] 스킬 부여 완료: %s"), *InCharacterID.ToString(), *AbilityClass->GetName());
			}
		}
	}
	else
	{
		LR_WARN(TEXT("[%s] ASC가 없어서 스킬을 부여할 수 없음"), *InCharacterID.ToString());
	}

	// 맴버 매쉬 스케일 적용
	GetMesh()->SetRelativeScale3D(CharData.MemberScale);
	
	// 무기 장착
	if (CharData.MemberWeaponID != NAME_None)
	{
	    UpdateWeaponMesh(CharData.MemberWeaponID);
	}
	else
	{
		if (CurrentWeaponActor)
		{
			CurrentWeaponActor->SetActorHiddenInGame(true);
			CurrentWeaponActor->SetActorTickEnabled(false);
		}
	}

	// 히트 몽타주 로드 및 캐싱
	if (!CharData.HitMontage.IsNull())
	{
		LoadedHitMontage = CharData.HitMontage.LoadSynchronous();
	}
	else
	{
		LoadedHitMontage = nullptr;
	}

	// 스탯 가져와서 적용하기
	if (DataSys && MemberAttributeSet)
	{
		//TODO_BJM : 레벨 시스템이 구현되면 MemberLevel을 캐릭터의 실제 레벨로 바꿔야 함
		int32 MemberLevel = 1;

		float CharHP = DataSys->GetCharacterFinalStat(InCharacterID, ELRStatusType::HP, MemberLevel);
		float CharAtk = DataSys->GetCharacterFinalStat(InCharacterID, ELRStatusType::ATK, MemberLevel);
		float CharDef = DataSys->GetCharacterFinalStat(InCharacterID, ELRStatusType::DEF, MemberLevel);

		float EquipHP = 0.0f; float EquipAtk = 0.0f; float EquipDef = 0.0f;
		float SetHP_Mul = 1.0f; float SetAtk_Mul = 1.0f; float SetDef_Mul = 1.0f;

		if (CharData.MemberWeaponID != NAME_None)
		{
			TArray<FName> ItemIDs;
			ItemIDs.Add(CharData.MemberWeaponID);
			TArray<int32> ItemLevels;
			ItemLevels.Add(1);

			EquipHP = DataSys->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::HP);
			EquipAtk = DataSys->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::ATK);
			EquipDef = DataSys->GetTotalEquipmentBonus(ItemIDs, ItemLevels, ELRStatusType::DEF);

			DataSys->GetSetEffectStatBonus(ItemIDs, SetHP_Mul, SetAtk_Mul, SetDef_Mul);
		}

		// 최종 스탯 계산
		float Balance = 0.75f;
		float FinalHP = (CharHP + EquipHP) * SetHP_Mul * Balance;
		float FinalAtk = (CharAtk + EquipAtk) * SetAtk_Mul * Balance;
		float FinalDef = (CharDef + EquipDef) * SetDef_Mul * Balance;

		// 어트리뷰트셋 적용
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->SetNumericAttributeBase(ULRAttributeSet::GetMaxHealthAttribute(), FinalHP);
			AbilitySystemComponent->SetNumericAttributeBase(ULRAttributeSet::GetHealthAttribute(), FinalHP);
			AbilitySystemComponent->SetNumericAttributeBase(ULRAttributeSet::GetAttackPowerAttribute(), FinalAtk);
			AbilitySystemComponent->SetNumericAttributeBase(ULRAttributeSet::GetDefenseAttribute(), FinalDef);
		}
		//LR_INFO(TEXT("[%s] Member Final Stats - HP: %.1f, ATK: %.1f, DEF: %.1f"), *InCharacterID.ToString(), FinalHP, FinalAtk, FinalDef);
	}


	// (참고: 필요하면 여기서 CharData의 스탯을 이용해 체력/공격력 세팅을 추가할 수도 있음)
	LR_INFO(TEXT("[%s] 캐릭터 데이터 세팅 완료"), *InCharacterID.ToString());
}

void ALRMemberCharacter::ResetAttributes()
{
	//if (MemberAttributeSet)
	//{
	//	float MaxHP = MemberAttributeSet->GetMaxHealth();
	//	if (MaxHP <= 0.0f)
	//	{
	//		MaxHP = 50.0f;
	//	}

	//	MemberAttributeSet->InitHealth(MaxHP);
	//	MemberAttributeSet->InitMaxHealth(MaxHP);
	//	MemberAttributeSet->InitAttackPower(5.0f);
	//	
	//}

	if (MemberAttributeSet)
	{
		float MaxHP = MemberAttributeSet->GetMaxHealth();
		MemberAttributeSet->SetHealth(MaxHP);
	}
}
void ALRMemberCharacter::ResetAIController()
{
	if (Controller == nullptr && AIControllerClass)
	{
		SpawnDefaultController();
	}
	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		if (UBlackboardComponent* BBComp = AICon->GetBlackboardComponent())
		{
			BBComp->ClearValue(TEXT("TargetActor"));
			BBComp->ClearValue(TEXT("HasNearbyHostile"));
		}
	}
}

void ALRMemberCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}
	bIsDead = true;

	LR_INFO(TEXT("맴버 사망 : %s"), *GetName());

	if (Controller)
	{
		Controller->StopMovement();
		//Controller->UnPossess();
	}

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
		AICon->ClearFocus(EAIFocusPriority::Default);

		if (UBehaviorTreeComponent* MemberBT = Cast<UBehaviorTreeComponent>(AICon->GetBrainComponent()))
		{
			MemberBT->StopTree();
		}
	}

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->Montage_Stop(0.1f);
	}

	float ReturnDelay = 3.0f;
	if (LoadedDeathMontage)
	{
		float Duration = PlayAnimMontage(LoadedDeathMontage);

		if (Duration > 0.0f)
		{
			ReturnDelay = Duration + 0.5f;
		}
	}
	else
	{
		LR_WARN(TEXT("재생할 사망 몽타주가 없음 DT를 확인 필요."));
	}

	GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, this, &ALRMemberCharacter::ReturnSelf, ReturnDelay, false);
}


void ALRMemberCharacter::ReturnSelf()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	UPoolingSubsystem* PoolSys = World->GetSubsystem<UPoolingSubsystem>();
	if (PoolSys)
	{
		PoolSys->ReturnToPool(this);
		LR_INFO(TEXT("%s 풀 반환 완료"), *GetName());
	}
	else
	{
		Destroy();
		LR_INFO(TEXT("%s 풀 반환 실패"), *GetName());
	}
}

void ALRMemberCharacter::UpdateWeaponMesh(FName InWeaponID)
{
	if (!WeaponClass)
	{
		LR_WARN(TEXT("[%s] WeaponClass가 지정되지 않았습니다. 멤버 블루프린트를 확인하세요!"), *GetName());
		return;
	}

	// 무기가 없으면 일단 스폰
	if (!CurrentWeaponActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		CurrentWeaponActor = GetWorld()->SpawnActor<ALREquipmentBase>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);
		
		if (CurrentWeaponActor)
		{
			CurrentWeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocketName);
		}
		
	}

	// 무기가 있으면 소켓에 다시 붙이고 세팅
	if (CurrentWeaponActor)
	{
		CurrentWeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);

		CurrentWeaponActor->SetActorScale3D(FVector(0.7f, 0.7f, 0.7f));

		CurrentWeaponActor->SetActorHiddenInGame(false);
		CurrentWeaponActor->SetActorTickEnabled(true);

		bool bSuccess = CurrentWeaponActor->InitEquipment(InWeaponID);
		if (bSuccess)
		{
			LR_INFO(TEXT("[%s] 멤버 무기 업데이트 완료: %s"), *GetName(), *InWeaponID.ToString());
		}
		else
		{
			LR_WARN(TEXT("[%s] 멤버 무기 업데이트 실패 (DT에 메시가 없거나 ID 오류): %s"), *GetName(), *InWeaponID.ToString());
		}
	}
}