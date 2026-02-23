// Fill out your copyright notice in the Description page of Project Settings.

#include "Units/Player/LRPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
#include "Engine/LocalPlayer.h"
#include "UI/HUD/LRStageHUD.h"

#include "Units/Player/LRPlayerState.h"
#include "AbilitySystemComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Input/LRInputComponent.h"
#include "Input/LRInputConfig.h"   
#include "GAS/Tags/LRGameplayTags.h"
#include "GAS/Attributes/LRPlayerAttributeSet.h"

#include "GameplayTagsManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Structures/Core/LRPlayerCore.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"




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
		PS->InitializePlayerData();

		if (AttributeSet)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
				ULRPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ALRPlayerCharacter::OnHealthChangedNative);
		}

		UE_LOG(LogTemp, Log, TEXT("GAS Initialized completely in %s"), *GetName());
	}
	if (APlayerController* PC = Cast<APlayerController>(NewController))
	{
		AHUD* RawHUD = PC->GetHUD();
		if (RawHUD)
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD Found: %s"), *RawHUD->GetName());
		}

		if (ALRStageHUD* LRHUD = Cast<ALRStageHUD>(RawHUD))
		{
			UE_LOG(LogTemp, Warning, TEXT("HUD Cast SUCCESS! Initializing Overlay..."));
			LRHUD->InitOverlay(PC, PS, AbilitySystemComponent, AttributeSet);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HUD Cast FAILED! Is BP_StageHUD parent class 'ALRHUD'?"));
		}
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

void ALRPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (CombatComponent && CombatComponent->IsAutoMode())
	{
		return;
	}

	FVector2D MovementVector = Value.Get<FVector2D>();

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

	if (bIsDead)
	{
		return;
	}
	if (NewHealth <= 0.0f)
	{
		Die();
	}
}

void ALRPlayerCharacter::Die()
{
	if (bIsDead) return;
	bIsDead = true;

	LR_INFO(TEXT("플레이어 사망"));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
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
	// 혹시 안 움직이면 주석 해제
	// GetCharacterMovement()->SetActive(true);

	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
		{
			AnimInst->StopAllMontages(0.0f);
		}
	}


	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		float MaxHP = ASC->GetNumericAttribute(ULRPlayerAttributeSet::GetMaxHealthAttribute());
		ASC->SetNumericAttributeBase(ULRPlayerAttributeSet::GetHealthAttribute(), MaxHP);
	}

	LR_INFO(TEXT("플레이어 코어에서 부활 완료!"));

	// TODO: 무적 & 깜빡임 효과
}
