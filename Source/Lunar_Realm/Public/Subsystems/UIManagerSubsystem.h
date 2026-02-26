// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UI/Core/LRBaseWidget.h"
#include "Subsystems/Settings/UIManagerSettings.h"
#include "UIManagerSubsystem.generated.h"

/**
 * UI 생명주기 및 표시 레이어를 관리하는 중앙 집중식 서브시스템
 * 
 * 두 가지 UI 레이어 지원:
 * - PERSISTENT: 게임 플레이 중 지속적으로 표시되는 UI (예: HUD, Minimap)
 * - POPUP: 필요시 표시되는 모달 UI (예: 인벤토리, 설정)
 * 
 * 주요 기능:
 * - 위젯 캐싱으로 반복 생성/소멸 비용 절약
 * - Popup UI 스택 관리로 여러 팝업의 포커스 제어
 * - 입력 모드 자동 전환 (Popup 열림 시 UI Only, 닫힘 시 Game Only)
 */
//=============================================================================
// (260123) KHS 제작. 제반 사항 구현.
// (260223) PJB 수정. Persistent UI와 Popup UI 로직 개선, 입력 모드 변경 로직 추가.
// (260224) PJB 수정. UI 레이어 확장. 전체적인 코드 수정 및 최적화.
// =============================================================================

UCLASS()
class LUNAR_REALM_API UUIManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
    /** UI 레이어에 따라 적절한 ZOrder 계산 (Persistent: 낮음, Popup: 높음) */
    int32 CalculateZOrder(ULRBaseWidget* Widget) const;
    
    /** 현재 Popup 스택 상태에 따라 입력 모드 변경 */
    void NotifyInputModeChange();
    
    /** 
     * 실제 UI 닫기 작업을 수행하는 내부 헬퍼 함수
     * - 템플릿 CloseUI와 오버로드 CloseUI가 모두 사용
     * - 중복 로직 방지 및 유지보수성 향상
     */
    void CloseUIInternal(ULRBaseWidget* Widget);

	/** 팝업 UI들의 ZOrder를 현재 스택 상태에 맞게 갱신 */
	void UpdatePopupZOrders();

	/** 입력 제한 최상단 팝업 찾기
	* - Popup UI 의 존재 여부를 통해 입력 모드 결정 시 사용
	*/
	ULRBaseWidget* FindTopModalPopup();

	/** 각 위젯을 삭제하기 위한 헬퍼 함수 */
	void ResetUIState(ULRBaseWidget* Widget);

public:
    /**
     * UI를 열고 뷰포트에 추가
     * - 이미 열려있으면 기존 인스턴스 반환
     * - Persistent UI는 맵에 저장
     * - Popup UI는 스택에 추가하고 이전 Top에 FocusLost 알림
     * 
     * @param TargetClass 열고자 하는 UI 위젯 클래스
     * @return 열린 UI 위젯 인스턴스 (실패 시 nullptr)
     */
    template<typename T>
    T* OpenUI(TSubclassOf<T> TargetClassFactory);
    
    /**
     * 위젯을 캐시에서 가져오거나 없으면 새로 생성
     * - 성능 최적화: 자주 열리는 UI의 생성 비용 절감
     * - 위젯 상태 유지: 닫았다 다시 열어도 이전 상태 보존
     * 
     * @param WidgetClass 가져올/생성할 위젯 클래스
     * @return 캐싱된 또는 새로 생성된 위젯 (실패 시 nullptr)
     */
    template<typename T>
    T* GetOrCreateWidget(TSubclassOf<T> WidgetClassFactory);
    

    /**
     * 특정 타입의 UI를 닫고 뷰포트에서 제거 (타입 기반)
     * - Persistent: 맵에서 제거
     * - Popup: 스택에서 제거하고 새로운 Top에 FocusGained 알림
     * - 주의: 위젯은 캐시에 유지되므로 완전히 파괴되지 않음
     */
    template<typename T>
    void CloseUI(TSubclassOf<T> targetClassFactory);
	
    /**
     * 위젯 인스턴스를 직접 받아서 닫기 (인스턴스 기반)
     * - 주로 위젯의 닫기 요청 델리게이트 핸들러에서 사용
     * @param Widget 닫고자 하는 위젯 인스턴스
     */
	UFUNCTION()
    void CloseUI(ULRBaseWidget* Widget);
    

    /** 
     * 스택 최상위 Popup만 닫기
     * - ESC 키 바인딩 등에 사용
     * - 스택에서 Pop하고 그 아래 팝업에 포커스 전달
     */
    void CloseTopPopupUI();
    
    /** 
     * 열려있는 모든 Popup UI 닫기
     * - 게임 오버, 씬 전환 등에 사용
     */
    void CloseAllPopupUI();
    

    /** 
     * Popup 스택의 새로운 Top에 포커스 전달
     * - 중간 팝업이 닫혔을 때 호출됨
     */
    void RefreshTopPopupUI();
    

    /** 
     * 레벨 전환시 모든 UI상태 리셋 함수(Controller)
     * - 캐싱된 UI들을 모두 초기화하여 레벨 전환시 재생성 보장.
     */
	void ResetAllUIStates();

	/**
	* 페이지 UI 를 전환할 때 사용
	* - Popup UI는 스택에서 제거하고 새로운 페이지 UI는 열어서 뷰포트에 추가
	*/
	template<typename T>
	ULRBaseWidget* SwitchPageUI(TSubclassOf<T> TargetClassFactory);


	ULRBaseWidget* OpenUIByID(EUIID UIID);
	ULRBaseWidget* SwitchPageUIByID(EUIID PageID);

    /** Popup UI가 하나라도 열려있는지 확인 */
    FORCEINLINE bool HasOpenPopupUI() const { return PopupUIStack.Num() > 0; }
	FORCEINLINE int GetPopupStackSize() const { return PopupUIStack.Num(); }
    
private:
    /** 현재 열려있는 Persistent UI들의 맵 (클래스 -> 인스턴스) */
    UPROPERTY()
    TMap<TSubclassOf<ULRBaseWidget>, ULRBaseWidget*> PersistentUIMap;
    
    /** 
     * 열려있는 Popup UI들의 스택 (Last = 최상위/포커스 중)
     * 나중에 추가된 것이 위에 표시되고 먼저 입력 받음
     */
    UPROPERTY()
    TArray<ULRBaseWidget*> PopupUIStack;
    
    /** 
     * 생성된 모든 위젯의 캐시 (클래스 -> 인스턴스)
     * 성능: 매번 CreateWidget 호출 방지
     * 상태 유지: UI를 닫아도 데이터 보존
     */
    UPROPERTY()
    TMap<TSubclassOf<ULRBaseWidget>, ULRBaseWidget*> CachedWidgets;

	UPROPERTY()
	ULRBaseWidget* CurrentPageWidget = nullptr;
};


//=============================================================================
// (260123) 김하신. 탬플릿 함수 구현부.
// =============================================================================

template<typename T>
T* UUIManagerSubsystem::GetOrCreateWidget(TSubclassOf<T> WidgetClassFactory)
{
    TSubclassOf<ULRBaseWidget> TargetClassFactory = WidgetClassFactory;
    
    // 캐싱된 UI가 있으면 반환(클래스 설계도로 실제 인스턴스 키 있는지 검사)
    if (CachedWidgets.Contains(TargetClassFactory))
    {
        return Cast<T>(CachedWidgets[TargetClassFactory]);
    }
    
    // 없으면 생성 후 캐시에 저장
    APlayerController* PC = GetWorld()->GetFirstPlayerController();
    if (!PC)
    {
        return nullptr;
    }
    
    T* NewWidget = CreateWidget<T>(PC, WidgetClassFactory);
    if (NewWidget)
    {
        CachedWidgets.Add(TargetClassFactory, NewWidget);
    }
    return NewWidget;
}

template<typename T>
T* UUIManagerSubsystem::OpenUI(TSubclassOf<T> TargetClassFactory)
{
    if (!TargetClassFactory)
    {
        return nullptr;
    }
    
    //캐싱된 인스턴스가 있다면 반환, 없으면 생성 후 반환
    T* Widget = GetOrCreateWidget<T>(TargetClassFactory);
    if (!Widget)
    {
        return nullptr;
    }
	

	ULRBaseWidget* BaseWidget = Widget;

	// UI 레이어가 NONE이면 열 수 없음 (Child Widget으로만 사용 가능)
	if (BaseWidget->UILayer == EUILayer::NONE)
	{
		LR_WARN(TEXT("Invalid UILayer for %s. Please set a valid UILayer in the widget blueprint."), *BaseWidget->GetName());
		return nullptr;
	}

    // 이미 열려있으면 갱신 후 조기 종료
    if (Widget->IsOpen())
    {
		Widget->RefreshUI(); 
        return Widget;
    }
    
	BaseWidget->InitializeUI();

	switch (BaseWidget->UILayer)
	{
		case EUILayer::BACKGROUND:
		case EUILayer::PERSISTENT:
		{
			TSubclassOf<ULRBaseWidget> BaseClassFactory = TargetClassFactory;
			if (!PersistentUIMap.Contains(BaseClassFactory))
			{
				PersistentUIMap.Add(BaseClassFactory, BaseWidget);
			}

			BaseWidget->OpenUI();

			if (!BaseWidget->IsInViewport())
			{
				int32 ZOrder = CalculateZOrder(BaseWidget);
				BaseWidget->AddToViewport(ZOrder);
			}
			break;
		}
		case EUILayer::PAGE:
		case EUILayer::POPUP:
		case EUILayer::SYSTEM:
		{
			if (PopupUIStack.Num() > 0)
			{
				PopupUIStack.Last()->OnFocusLost();
			}

			PopupUIStack.Add(BaseWidget);
			BaseWidget->OpenUI();
			if (!BaseWidget->IsInViewport())
			{
				int32 ZOrder = CalculateZOrder(BaseWidget);
				BaseWidget->AddToViewport(ZOrder);
			}

			UpdatePopupZOrders();
			
			NotifyInputModeChange();
			break;
		}
		case EUILayer::NONE:
		case EUILayer::TOOLTIP:
		default:
		{
			break;
		}
	}
    return Widget;
}

template<typename T>
void UUIManagerSubsystem::CloseUI(TSubclassOf<T> targetClassFactory)
{
	if (!targetClassFactory)
	{
		return;
	}
	
	// 캐싱중인 UI라면 리턴
	TSubclassOf<ULRBaseWidget> baseClassFactory = targetClassFactory;
	if (!CachedWidgets.Contains(baseClassFactory))
	{
		return;
	}
	
	//실제 닫기 로직은 내부 헬퍼함수 호출
	ULRBaseWidget* widget = CachedWidgets[baseClassFactory];
	CloseUIInternal(widget);
}

template<typename T>
ULRBaseWidget* UUIManagerSubsystem::SwitchPageUI(TSubclassOf<T> TargetClassFactory)
{
	if (!TargetClassFactory)
	{
		LR_WARN(TEXT("SwitchPageUI failed: TargetClassFactory is null."));
		return nullptr;
	}

	if (CurrentPageWidget)
	{
		if (CurrentPageWidget->GetClass() == TargetClassFactory)
		{
			return OpenUI<T>(TargetClassFactory);
		}
		LR_INFO(TEXT("Switching Page UI: Closing current page %s before opening new page."), *CurrentPageWidget->GetName());
		CloseUIInternal(CurrentPageWidget);
		CurrentPageWidget = nullptr;
	}

	T* NewPageWidget = OpenUI<T>(TargetClassFactory);
	if (NewPageWidget)
	{
		ULRBaseWidget* BaseWidget = Cast<ULRBaseWidget>(NewPageWidget);
		if (BaseWidget)
		{
			if (BaseWidget->UILayer == EUILayer::PAGE)
			{
				CurrentPageWidget = BaseWidget;
				LR_INFO(TEXT("Switched to new Page UI: %s"), *BaseWidget->GetName());
			}
			else
			{
				LR_WARN(TEXT("Invalid UILayer for %s. Please set UILayer to PAGE in the widget blueprint."), *BaseWidget->GetName());
			}
		}
	}
	return NewPageWidget;
}
