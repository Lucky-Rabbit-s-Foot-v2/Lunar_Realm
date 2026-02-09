
#include "Subsystems/CurrencySubsystem.h"
#include "Subsystems/SaveGameSubsystem.h"
#include "SaveGame/LRSaveGame.h"
#include "Engine/GameInstance.h"

void UCurrencySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (UGameInstance* GI = GetGameInstance())
	{
		SaveSS = GI->GetSubsystem<USaveGameSubsystem>();
	}
}

ULRSaveGame* UCurrencySubsystem::SG() const
{
	return SaveSS ? SaveSS->GetCurrentSaveGame() : nullptr;
}

int32 UCurrencySubsystem::GetCurrency(ELRCurrencyType Type) const
{
	ULRSaveGame* S = SG();
	if (!S) return 0;

	switch (Type)
	{
	case ELRCurrencyType::Gold:           return S->Gold;
	case ELRCurrencyType::CrescentTicket: return S->CrescentTicket;
	case ELRCurrencyType::FullMoonTicket: return S->FullMoonTicket;
	default: return 0;
	}
}

void UCurrencySubsystem::AddCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (Amount == 0) return;

	ULRSaveGame* S = SG();
	if (!S) return;

	switch (Type)
	{
	case ELRCurrencyType::Gold:
		S->Gold = FMath::Max(0, S->Gold + Amount);
		break;
	case ELRCurrencyType::CrescentTicket:
		S->CrescentTicket = FMath::Max(0, S->CrescentTicket + Amount);
		break;
	case ELRCurrencyType::FullMoonTicket:
		S->FullMoonTicket = FMath::Max(0, S->FullMoonTicket + Amount);
		break;
	default:
		break;
	}

	if (SaveSS)
	{
		SaveSS->TouchAndSave();
	}
}

bool UCurrencySubsystem::SpendCurrency(ELRCurrencyType Type, int32 Amount)
{
	if (Amount <= 0) return true;
	if (GetCurrency(Type) < Amount) return false;

	AddCurrency(Type, -Amount);
	return true;
}
