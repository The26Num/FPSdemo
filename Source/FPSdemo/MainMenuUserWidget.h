#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuUserWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class FPSDEMO_API UMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* Button_HostLAN;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_JoinLAN;

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* EditableTextBox_ServerIP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Message;

	UFUNCTION()
	void OnHostLANClicked();

	UFUNCTION()
	void OnJoinLANClicked();

	void SetMessage(const FString& Message);
};