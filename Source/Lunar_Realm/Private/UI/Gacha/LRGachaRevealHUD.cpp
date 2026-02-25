// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Gacha/LRGachaRevealHUD.h"
#include "UI/Gacha/LRGachaRevealWidget.h"

#include "Subsystems/Gacha/LRGachaSubsystem.h"
#include "Subsystems/UIManagerSubsystem.h"

#include "Engine/GameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h" // 로그용

// 간단한 로그 매크로 (별도 로깅 시스템 없을 때)
#define LR_LOG(Verbosity, Format, ...) \
	UE_LOG(LogTemp, Verbosity, TEXT("[GachaRevealHUD] " Format), ##__VA_ARGS__)

void ALRGachaRevealHUD::BeginPlay()
{
	Super::BeginPlay();

	// 1) GameInstance 가져오기
	UGameInstance* GI = GetWorld() ? GetWorld()->GetGameInstance() : nullptr;
	if (!GI)
	{
		LR_LOG(Warning, TEXT("GameInstance is null. Opening Lobby level."));
		UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
		return;
	}

	// 2) 서브시스템 참조
	ULRGachaSubsystem* GachaSys = GI->GetSubsystem<ULRGachaSubsystem>();
	UUIManagerSubsystem* UISys = GI->GetSubsystem<UUIManagerSubsystem>();

	if (!GachaSys || !UISys)
	{
		LR_LOG(
			Warning,
			TEXT("Subsystem missing. GachaSys: %s, UISys: %s"),
			GachaSys ? TEXT("Valid") : TEXT("Null"),
			UISys ? TEXT("Valid") : TEXT("Null")
		);

		// 잘못된 상태면 로비로 복귀
		UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
		return;
	}

	// 3) PendingReveal 소비
	FName BannerID;
	FGuid TxnId;
	TArray<FLRGachaResult> Results;

	// 추천 시그니처:
	//  bool ConsumePendingReveal(FName& OutBannerId, FGuid& OutTxnId, TArray<FLRGachaResult>& OutResults);
	if (!GachaSys->ConsumePendingReveal(BannerID, TxnId, Results) || Results.Num() == 0)
	{
		LR_LOG(Warning, TEXT("No pending gacha reveal. Back to Lobby."));
		UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
		return;
	}

	// 4) RevealWidgetClass 설정 체크
	if (!RevealWidgetClass)
	{
		LR_LOG(
			Error,
			TEXT("RevealWidgetClass is not set on %s. Check BP_GachaRevealHUD settings."),
			*GetName()
		);

		UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
		return;
	}

	// 5) UIManagerSubsystem을 통해 리빌 위젯 열기
	ULRGachaRevealWidget* RevealWidget = UISys->OpenUI<ULRGachaRevealWidget>(RevealWidgetClass);
	if (!RevealWidget)
	{
		LR_LOG(Error, TEXT("Failed to open RevealWidget from UIManagerSubsystem."));
		UGameplayStatics::OpenLevel(this, FName(TEXT("Map_Lobby")));
		return;
	}

	// 6) 리빌 시작 (트랜잭션 ID 포함)
	RevealWidget->StartRevealWithTransaction(BannerID, TxnId, Results);

	LR_LOG(
		Display,
		TEXT("Gacha reveal started. BannerID: %s, ResultCount: %d"),
		*BannerID.ToString(),
		Results.Num()
	);
}