#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyUserWidget.generated.h"

class UButton;
class UEditableTextBox;
class UTextBlock;

UCLASS()
class FPSDEMO_API ULobbyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

	void SetMessage(const FString& Message);

protected:
	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* EditableTextBox_RoomName;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_CreateRoom;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_JoinRoom;

	UPROPERTY(meta = (BindWidget))
	UButton* Button_StartRoom;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Message;

	UFUNCTION()
	void OnCreateRoomClicked();

	UFUNCTION()
	void OnJoinRoomClicked();

	UFUNCTION()
	void OnStartRoomClicked();

	FString GetInputRoomName() const;
};