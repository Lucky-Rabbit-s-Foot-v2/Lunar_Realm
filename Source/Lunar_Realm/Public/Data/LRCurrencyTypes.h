#pragma once
#include "CoreMinimal.h"
#include "LRCurrencyTypes.generated.h"

UENUM(BlueprintType)
enum class ELRCurrencyType : uint8
{
	Gold,
	CrescentTicket,
	FullMoonTicket
};
