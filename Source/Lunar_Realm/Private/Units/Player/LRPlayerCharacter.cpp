// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"

#include "Units/Player/LRPlayerState.h"
#include "Units/Player/LRPlayerController.h"
#include "UI/InGame/LRPlayerWidget.h"
#include "AbilitySystemComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Input/LRInputComponent.h"
#include "Input/LRInputConfig.h"   
#include "GAS/Tags/LRGameplayTags.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"
#include "GAS/Attributes/LRAttributeSet.h"
#include "Actors/Equipment/LREquipmentBase.h"

#include "GameplayTagsManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Structures/Core/LRPlayerCore.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

#include "AIController.h" 
#include "Subsystems/SaveGameSubsystem.h"




ALRPlayerCharacter::ALRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f); 

	AutoPossessPlayer = EAutoReceiveInput::Player0; // 플레이어 자동 빙의 (싱글 플레이 테스트용)

	SummonComponent = CreateDefaultSubobject<ULRSummonComponent>(TEXT("SummonComponent"));
	CombatComponent = CreateDefaultSubobject<ULRCombatComponent>(TEXT("CombatComponent"));

	TargetIndicator = CreateDefaultSubobject<UDecalComponent>(TEXT("TargetIndicator"));
	TargetIndicator->SetupAttachment(RootComponent);
	TargetIndicator->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
	TargetIndicator->DecalSize = FVector(400.0f, 120.0f, 120.0f);
	TargetIndicator->SetVisibility(false);
}

void ALRPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// TODO_BJM: 테스트용으로 임시 배치.
	//TestSummonSlot();
}

void ALRPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ALRPlayerState* PS = GetPlayerState<ALRPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();	
		AttributeSet = PS->GetAttributeSet();

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);
		AbilitySystemComponent->AddLooseGameplayTag(UnitTag);
		PS->InitializePlayerData();

		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				ULRAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRPlayerCharacter::OnHealthChangedNative);
		}

		// TODO_BJM: 테스트용으로 임시 배치. 추후 덱 데이터 로드 방식 확정되면 제거 예정
		if (SummonComponent)
		{
			if (SummonComponent)
			{
				TArray<FName> RealDeck;

				UGameInstance* GI = GetGameInstance();
				USaveGameSubsystem* SaveSys = GI ? GI->GetSubsystem<USaveGameSubsystem>() : nullptr;

				if (SaveSys)
				{
					// 저장된 전체 파티 목록 (최대 5명)
					TArray<FName> AllSlots = SaveSys->GetAllPartyCharactersIDs();

					// 리더(0번)를 제외하고 1번부터 덱에 추가
					for (int32 i = 1; i < AllSlots.Num(); ++i)
					{
						RealDeck.Add(AllSlots[i]);
					}
				}

				// 안전장치: 에디터에서 바로 켜서 세이브 데이터가 비어있을 경우
				if (RealDeck.IsEmpty())
				{
					RealDeck = { FName("Anubis"), FName("Maid"), FName("Merry"), FName("Army") };
				}

				SummonComponent->LoadDeckData(RealDeck);
			}
		}

		if (ALRPlayerController* PC = Cast<ALRPlayerController>(NewController))
		{
			if (ULRPlayerWidget* MyWidget = PC->GetPlayerWidget())
			{
				MyWidget->TestSummonPanelRefresh();
			}
		}

		UE_LOG(LogTemp, Log, TEXT("GAS Initialized completely in %s"), *GetName());
	}
}

void ALRPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AActor* Target = nullptr;
	if (CombatComponent)
	{
		Target = CombatComponent->GetCurrentTarget();
	}

	if (Target)
	{
		TargetIndicator->SetVisibility(true);

		FVector TargetLoc = Target->GetActorLocation(); 

		ACharacter* TargetChar = Cast<ACharacter>(Target);
		if (TargetChar)
		{
			float HalfHeight = TargetChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			TargetLoc.Z -= HalfHeight; 

			TargetLoc.Z += 1.0f;
		}
		else
		{
			TargetLoc.Z -= 90.0f;
		}

		TargetIndicator->SetWorldLocation(TargetLoc);
	}
	else
	{
		TargetIndicator->SetVisibility(false);
	}
}

void ALRPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	ULRInputComponent* LRInputComp = Cast<ULRInputComponent>(PlayerInputComponent);

	if (!LRInputComp)
	{
		UE_LOG(LogTemp, Error, TEXT("SetupInput Failed: PlayerInputComponent is NOT ULRInputComponent!"));
		return;
	}

	if (MoveAction)
	{
		LRInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALRPlayerCharacter::Move);
	}

	if (InputConfig)
	{
		LRInputComp->BindAbilityActions(InputConfig, this, &ALRPlayerCharacter::Input_Summon, /*ReleasedFunc=*/ nullptr);
		
		if (InputConfig->ChargeAction)
		{
			LRInputComp->BindAction(InputConfig->ChargeAction, ETriggerEvent::Started, this, &ALRPlayerCharacter::Input_Charge);
		}
	}
}

UAbilitySystemComponent* ALRPlayerCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALRPlayerCharacter::ToggleAutoMode()
{
	if (CombatComponent)
	{
		CombatComponent->ToggleAutoMode();
	}
}

void ALRPlayerCharacter::UsePotion()
{
	AbilitySystemComponent->TryActivateAbilitiesByTag(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(FName("Ability.Skill.Heal"))));
}

void ALRPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CombatComponent && CombatComponent->IsAutoMode())
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

	if (bIsDead)
	{
		CameraOffsetY += MovementVector.X * DeadCameraSpeed * GetWorld()->GetDeltaSeconds();

		float CurrentY = GetActorLocation().Y;

		// TODO_BJM : 추후 맵 확정될때 CameraManager의 MinY, MaxY랑 값 맞춰줘야함
		float MapMinY = -2000.0f;
		float MapMaxY = 2000.0f;

		float LimitMin = MapMinY - CurrentY;
		float LimitMax = MapMaxY - CurrentY;

		CameraOffsetY = FMath::Clamp(CameraOffsetY, LimitMin, LimitMax);

		return;
	}

	if (Controller != nullptr)
	{
		const FVector ForwardDirection = FVector::ForwardVector;
		AddMovementInput(ForwardDirection, MovementVector.Y);

		const FVector RightDirection = FVector::RightVector;
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ALRPlayerCharacter::Input_Summon(FGameplayTag InputTag)
{
	if (!SummonComponent) return;

	int32 SlotIndex = -1;

	if (InputTag.MatchesTag(LRTags::Input_Summon_1))
	{
		SlotIndex = 0;
	}
	else if (InputTag.MatchesTag(LRTags::Input_Summon_2))
	{
		SlotIndex = 1;
	}
	else if (InputTag.MatchesTag(LRTags::Input_Summon_3))
	{
		SlotIndex = 2;
	}
	else if (InputTag.MatchesTag(LRTags::Input_Summon_4))
	{
		SlotIndex = 3;
	}

	if (SlotIndex >= 0)
	{
		SummonComponent->TrySummonUnit(SlotIndex);
	}
}

void ALRPlayerCharacter::GrantTestAbility(TSubclassOf<class UGameplayAbility> AbilityClass)
{
	if (AbilitySystemComponent && AbilityClass)
	{
		FGameplayAbilitySpec Spec(AbilityClass, 1, INDEX_NONE, this);
		AbilitySystemComponent->GiveAbility(Spec);

		UE_LOG(LogTemp, Warning, TEXT("[Test] 스킬 부여됨: %s"), *AbilityClass->GetName());
	}
}

void ALRPlayerCharacter::Input_Charge(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("키보드 Q 입력 감지됨!"));

	if (GetAbilitySystemComponent())
	{
		FGameplayTagContainer TagContainer;
		TagContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Aether.Charge")));
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(TagContainer);
	}
}

void ALRPlayerCharacter::OnHealthChangedNative(const FOnAttributeChangeData& Data)
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
			// 피격 모션 재생
			PlayAnimMontage(LoadedHitMontage);

			//  맞았으니 지금 쓰던 평타(기본 공격) 스킬 강제 취소!
			if (AbilitySystemComponent)
			{
				FGameplayTagContainer CancelTags;
				CancelTags.AddTag(LRTags::Ability_Combat_BasicShoot);

				// 해당 태그를 가진 진행 중인 스킬을 즉시 EndAbility 시킴
				AbilitySystemComponent->CancelAbilities(&CancelTags);
			}
		}
	}

}

void ALRPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	LR_INFO(TEXT("플레이어 사망"));

	if (AAIController* AICon = Cast<AAIController>(GetController()))
	{
		AICon->ClearFocus(EAIFocusPriority::Gameplay);
		AICon->ClearFocus(EAIFocusPriority::Default);
	}

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (ALRPlayerController* PC = Cast<ALRPlayerController>(GetController()))
	{
		PC->OnPlayerDied(RespawnTime);
	}


	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		AnimInst->Montage_Stop(0.1f);
	}

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AddLooseGameplayTag(LRTags::State_Dead);
		AbilitySystemComponent->CancelAllAbilities();
	}
	if (CombatComponent)
	{
		CombatComponent->SetActive(false);
	}
	if (LoadedDeathMontage)
	{
		PlayAnimMontage(LoadedDeathMontage);
		LR_INFO(TEXT("사망 몽타주 재생 성공"));
	}
	else
	{
		LR_WARN(TEXT("사망 몽타주가 비어있어서 재생할 수 없음"));
	}

	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &ALRPlayerCharacter::RespawnPlayer, RespawnTime, false);

}

void ALRPlayerCharacter::RespawnPlayer()
{
	bIsDead = false;

	AActor* FoundCore = UGameplayStatics::GetActorOfClass(GetWorld(), ALRPlayerCore::StaticClass());
	if (ALRPlayerCore* PlayerCore = Cast<ALRPlayerCore>(FoundCore))
	{
		FVector SpawnLoc = PlayerCore->GetRandomSpawnLocation();
		SetActorLocation(SpawnLoc);
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			AnimInst->StopAllMontages(0.0f);
		}
	}


	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveLooseGameplayTag(LRTags::State_Dead);

		float MaxHP = ASC->GetNumericAttribute(ULRAttributeSet::GetMaxHealthAttribute());
		ASC->SetNumericAttributeBase(ULRAttributeSet::GetHealthAttribute(), MaxHP);
	}

	if (ALRPlayerController* PC = Cast<ALRPlayerController>(GetController()))
	{
		PC->OnPlayerRespawned(); 
	}


	LR_INFO(TEXT("플레이어 코어에서 부활 완료"));

	CameraOffsetY = 0.0f;

	bIsInvincible = true;

	GetWorld()->GetTimerManager().SetTimer(BlinkTimerHandle, this, &ALRPlayerCharacter::OnBlinkTimer, 0.15f, true);
	GetWorld()->GetTimerManager().SetTimer(InvincibilityTimerHandle, this, &ALRPlayerCharacter::EndInvincibility, 2.0f, false);

	GetCharacterMovement()->bOrientRotationToMovement = true;

	// TODO: 무적 & 깜빡임 효과
}

void ALRPlayerCharacter::OnBlinkTimer()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetHiddenInGame(!MeshComp->bHiddenInGame);
	}
}

void ALRPlayerCharacter::EndInvincibility()
{
	bIsInvincible = false;

	GetWorld()->GetTimerManager().ClearTimer(BlinkTimerHandle);

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetHiddenInGame(false);
	}

	LR_INFO(TEXT("무적 및 깜빡임 종료"));
}

void ALRPlayerCharacter::UpdateWeaponMesh(FName InWeaponID)
{
	if (!WeaponClass)
	{
		LR_WARN(TEXT("WeaponClass가 지정되지 않았습니다. 캐릭터 블루프린트를 확인하세요!"));
		return;
	}

	if (!CurrentWeaponActor)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;

		CurrentWeaponActor = GetWorld()->SpawnActor<ALREquipmentBase>(WeaponClass, GetActorLocation(), GetActorRotation(), SpawnParams);

		if (CurrentWeaponActor)
		{
			CurrentWeaponActor->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
		}
	}

	if (CurrentWeaponActor)
	{
		bool bSuccess = CurrentWeaponActor->InitEquipment(InWeaponID);
		if (bSuccess)
		{
			LR_INFO(TEXT("무기 외형 업데이트 완료: %s"), *InWeaponID.ToString());
		}
		else
		{
			LR_WARN(TEXT("무기 외형 업데이트 실패 (DT에 메시가 없거나 ID가 잘못됨): %s"), *InWeaponID.ToString());
		}
	}
}
