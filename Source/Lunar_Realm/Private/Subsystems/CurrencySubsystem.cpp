
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "Engine/GameInstance.h"

void UCurrencySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

USaveGameSubsystem* UCurrencySubsystem::GetSaveGameSubsystem() const
{
	if (UGameInstance* GI = GetGameInstance())
	{
		return GI->GetSubsystem<USaveGameSubsystem>();
	}
	return nullptr;
}

int32 UCurrencySubsystem::GetCurrency(ELRCurrencyType Type) const
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		return SaveGameSubsystem->GetCurrency(Type);
	}
	return 0;
}

void UCurrencySubsystem::AddCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		SaveGameSubsystem->AddCurrency(Type, Amount);
	}
}


bool UCurrencySubsystem::SpendCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (USaveGameSubsystem* SaveGameSubsystem = GetSaveGameSubsystem())
	{
		return SaveGameSubsystem->TrySpendCurrency(Type, Amount);
	}
	return false;
}
