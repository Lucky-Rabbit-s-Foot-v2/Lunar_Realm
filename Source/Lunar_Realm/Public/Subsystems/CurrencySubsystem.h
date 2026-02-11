#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Data/LREnumType.h"
#include "CurrencySubsystem.generated.h"

class USaveGameSubsystem;

UCLASS()
class LUNAR_REALM_API UCurrencySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	int32 GetCurrency(ELRCurrencyType Type) const;

	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	void AddCurrency(ELRCurrencyType Type, int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "LR|Currency")
	bool SpendCurrency(ELRCurrencyType Type, int32 Amount);

private:
	USaveGameSubsystem* GetSaveGameSubsystem() const;
};
