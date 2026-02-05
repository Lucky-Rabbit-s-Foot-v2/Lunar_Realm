// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BaseWidget.h"
#include "Data/Gacha/LRGachaTypes.h"
#include "LRGachaRevealWidget.generated.h"

/**
 * 
 */
UCLASS()
class LUNAR_REALM_API ULRGachaRevealWidget : public UBaseWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

	// 트랜잭션 포함 시작
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void StartRevealWithTransaction(FName InBannerID, FGuid InTxnId, const TArray<FLRGachaResult>& InResults);

	UFUNCTION(BlueprintCallable)
	void ForceUIInputNextTick();

	// 리빌 종료(결과 확인까지 끝났을 때 호출)
	UFUNCTION(BlueprintCallable, Category = "LR|Gacha|Reveal")
	void FinishRevealAndClose();

protected:
	// 블루프린트에서 구현할 연출 진입점
	// 등급별 카드색/이팩트/사운드/실루엣->리빌 시퀸스를 여기서 처리
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_PlayRevealSequence(const TArray<FLRGachaResult>& InResults);

	// 블루프린트에 "가속/스킵 상태 변경" 알려주기(애니메이션 속도 조절용)
	UFUNCTION(BlueprintImplementableEvent, Category = "LR|Gacha|Reveal")
	void BP_OnSkipStateChanged(int32 NewSkipState);

	// 스킵/가속 입력 처리
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	// 현재 결과 캐시(스킵/가속시 필요)
	UPROPERTY()
	TArray<FLRGachaResult> CachedResults;

	FGuid CachedTxnId;	//	저장
	FName CachedBannerId;

	// 0=정상, 1=가속, 2=완전 스킵(끝까지)
	int32 SkipState = 0;

	void CloseSelfPopup();	//	UIManager로 닫기

	// SkipState=2(스킵)으로 결과를 보여준 뒤, 다음 클릭에 닫기용
	bool bSkipResultsShown = false;

	// 닫은 후 커서/입력 복구
	void RestoreGachaInputAfterCloseNextTick();
};
