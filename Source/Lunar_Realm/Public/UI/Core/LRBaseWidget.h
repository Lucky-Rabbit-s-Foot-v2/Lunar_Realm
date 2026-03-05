// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LRBaseWidget.generated.h"


// =============================================================================
/** 
 * BaseWidget 구성 요소
 * 모든 UI 위젯의 베이스 클래스
 * - 열림/닫힘 상태 관리
 * - 포커스 이벤트 처리
 * - 닫기 요청 델리게이트
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260213) PJB 수정. 공용 델리게이트 해제 추가.
// (260213) PJB 수정. 초기화 함수 추가.
// (260224) PJB 수정. UI 레이어 확장
// =============================================================================

/**
 * 위젯이 닫기를 요청할 때 발생하는 이벤트
 * PlayerController나 다른 시스템에서 구독하여 실제 닫기 처리 수행
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCloseUIRequested, ULRBaseWidget*, RequestingWidget);

UENUM(BlueprintType)
enum class EUILayer : uint8
{
	NONE			UMETA(DisplayName = "None (Child Widget)"),
	BACKGROUND		UMETA(DisplayName = "Background"),
	PAGE			UMETA(DisplayName = "Page"),
	PERSISTENT		UMETA(DisplayName = "Persistent"),
	POPUP			UMETA(DisplayName = "Popup"),
	TOOLTIP			UMETA(DisplayName = "Tooltip"),
	SYSTEM			UMETA(DisplayName = "System"),
};

UCLASS()
class LUNAR_REALM_API ULRBaseWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//=============================================================================
	// UI 생성/제거 시점
	//=============================================================================

	ULRBaseWidget(const FObjectInitializer& ObjectInitializer);

	/** 초기화를 위해 선언 */
	virtual void NativeConstruct() override;

	/** 델리게이트 해제를 위해 선언 */
	virtual void NativeDestruct() override;

	/** 초기화를 위해 선언 */
	virtual void NativeOnInitialized() override;

	/** UI를 처음 생성할 때 초기화*/
	virtual void InitializeUI();

	/** UI 를 제거할 때 */
	virtual void DeinitializeUI();

	/** 서브 위젯 등록 */
	virtual void RegisterSubWidgets();

	/** 보유한 위젯 바인딩 */
	virtual void BindProperties();

	/** 보유한 위젯 바인딩 해제 */
	virtual void UnbindProperties();

	virtual void BindToController(class ALRControllerBase* Controller);

	//=============================================================================
	// UI 열림/닫힘, 갱신, 포커스 이벤트
	//=============================================================================

	/** UI를 활성화하고 화면에 표시 */
	virtual void OpenUI();
    
	/** UI를 비활성화하고 화면에서 숨김 */
	virtual void CloseUI();
    
	/** UI 데이터 갱신 (예: 아이템 목록, 스탯 변경 등) */
	virtual void RefreshUI();
    
	/** 이 UI가 팝업 스택의 최상위가 되었을 때 호출 */
	virtual void OnFocusGained();
    
	/** 이 UI 위에 다른 팝업이 열렸을 때 호출 */
	virtual void OnFocusLost();

	UFUNCTION(BlueprintCallable, Category = "LR|UI Events")
	virtual void OnCloseRequested();

	/** UI가 현재 열려있는지 확인 */
	FORCEINLINE bool IsOpen() const { return bIsOpen; }

	/**
	 * 위젯이 스스로 닫기를 요청할 때 발생하는 델리게이트
	 * - 블루프린트에서도 바인딩 가능
	 * - 여러 곳에서 구독 가능 (PlayerController, 통계 시스템 등)
	 */
	UPROPERTY(BlueprintAssignable, Category = "LR|UI Events")
	FOnCloseUIRequested OnCloseUIRequestedDel;
        
public:
	/** UI 레이어 타입 (Persistent: 지속형, Popup: 팝업형) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI Settings")
	EUILayer UILayer = EUILayer::NONE;
    
	/** 뷰포트 내 표시 순서 (높을수록 위에 렌더링) */
	UPROPERTY(EditDefaultsOnly, Category = "LR|UI Settings")
	int32 ZOrder = 0;

protected:
	/** UI 열림/닫힘 상태 */
	bool bIsOpen = false;

	/** 위젯을 포함하고 있을 경우 관리 */
	TArray<ULRBaseWidget*> SubWidgets;

	/** bIsFocusable : 키보드/패드 입력을 받을 수 있도록 설정 */
};
