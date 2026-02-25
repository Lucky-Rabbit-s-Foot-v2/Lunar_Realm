// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Units/LRControllerBase.h"
#include "LRPlayerController.generated.h"

/**
 * 
 */

//=============================================================================
// (260203) BJM 제작. 플레이어 컨트롤러
// (260210) BJM 카메라매니저 연동
// (260223) PJB Stage UI 연동
// (260225) BJM UseSkil 1,2 연동
//=============================================================================

class UTouchInterface;

UCLASS()
class LUNAR_REALM_API ALRPlayerController : public ALRControllerBase
{
	GENERATED_BODY()
	
public:
	ALRPlayerController();

	UFUNCTION()
	void ToggleAutoMode();

	UFUNCTION()
	void UsePotion();
	UFUNCTION()
	void UseSkill1();
	UFUNCTION()
	void UseSkill2();


	class UAbilitySystemComponent* GetAbilitySystemComponent();

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Mobile")
	TObjectPtr<UTouchInterface> MobileTouchInterface;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class ULRPersistentWidget> PersistentWidgetClass;
};
