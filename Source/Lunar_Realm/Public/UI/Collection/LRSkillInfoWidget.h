// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Core/LRBaseWidget.h"
#include "LRSkillInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRSkillInfoWidget : public ULRBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void RefreshUI() override;

	void SetSkillID(const FName& InID);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UImage> Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<class UTextBlock> Description;

	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "LR|Skill Info")
	class UTexture2D* DefaultIcon;

	UPROPERTY(EditANywhere, BlueprintReadWrite, Category = "LR|Skill Info")
	FText DefaultDescription;

private:
	FName ID = NAME_None;
};
