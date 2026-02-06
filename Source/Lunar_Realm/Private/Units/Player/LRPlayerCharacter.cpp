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
#include "GameplayTagsManager.h"
#include "GAS/Tags/LRGameplayTags.h"




ALRPlayerCharacter::ALRPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);

	SpringArmComponent->TargetArmLength = 800.0f;
	SpringArmComponent->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;

	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false;
	SpringArmComponent->bInheritRoll = false;


	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f); 

	AutoPossessPlayer = EAutoReceiveInput::Player0; // 플레이어 자동 빙의 (싱글 플레이 테스트용)

	SummonComponent = CreateDefaultSubobject<ULRSummonComponent>(TEXT("SummonComponent"));
	CombatComponent = CreateDefaultSubobject<ULRCombatComponent>(TEXT("CombatComponent"));
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

	// PlayerState 가져오기
	ALRPlayerState* PS = GetPlayerState<ALRPlayerState>();
	if (PS)
	{
		AbilitySystemComponent = PS->GetAbilitySystemComponent();	
		AttributeSet = PS->GetAttributeSet();

		AbilitySystemComponent->InitAbilityActorInfo(PS, this);

		PS->InitializePlayerData();

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
	}


	//if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	//{
	//	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALRPlayerCharacter::Move);
	//	if (SummonAction_1)
	//		{
	//			EnhancedInputComponent->BindAction(SummonAction_1, ETriggerEvent::Started, this, &ALRPlayerCharacter::SummonSlot1);
	//		}
	//	if (SummonAction_2)
	//		{
	//			EnhancedInputComponent->BindAction(SummonAction_2, ETriggerEvent::Started, this, &ALRPlayerCharacter::SummonSlot2);
	//		}
	//	if (SummonAction_3)
	//		{
	//			EnhancedInputComponent->BindAction(SummonAction_3, ETriggerEvent::Started, this, &ALRPlayerCharacter::SummonSlot3);
	//		}
	//	if (SummonAction_4)
	//		{
	//			EnhancedInputComponent->BindAction(SummonAction_4, ETriggerEvent::Started, this, &ALRPlayerCharacter::SummonSlot4);
	//		}
	//}
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
		AddMovementInput(ForwardDirection, MovementVector.X);

		const FVector RightDirection = FVector::RightVector;
		AddMovementInput(RightDirection, MovementVector.Y);
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

//void ALRPlayerCharacter::SummonSlot1()
//{
//	if (SummonComponent)
//	{
//		SummonComponent->TrySummonUnit(0);
//	}
//}
//
//void ALRPlayerCharacter::SummonSlot2()
//{
//}
//
//void ALRPlayerCharacter::SummonSlot3()
//{
//}
//
//void ALRPlayerCharacter::SummonSlot4()
//{
//}
