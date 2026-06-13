#include "LobbyUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "FPSLobbyPlayerController.h"

void ULobbyUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_CreateRoom)
	{
		Button_CreateRoom->OnClicked.AddDynamic(
			this,
			&ULobbyUserWidget::OnCreateRoomClicked
		);
	}

	if (Button_JoinRoom)
	{
		Button_JoinRoom->OnClicked.AddDynamic(
			this,
			&ULobbyUserWidget::OnJoinRoomClicked
		);
	}

	if (Button_StartRoom)
	{
		Button_StartRoom->OnClicked.AddDynamic(
			this,
			&ULobbyUserWidget::OnStartRoomClicked
		);
	}
}

void ULobbyUserWidget::SetMessage(const FString& Message)
{
	if (Text_Message)
	{
		Text_Message->SetText(FText::FromString(Message));
	}
}

FString ULobbyUserWidget::GetInputRoomName() const
{
	if (!EditableTextBox_RoomName)
	{
		return TEXT("");
	}

	return EditableTextBox_RoomName->GetText().ToString().TrimStartAndEnd();
}

void ULobbyUserWidget::OnCreateRoomClicked()
{
	AFPSLobbyPlayerController* PC = GetOwningPlayer<AFPSLobbyPlayerController>();

	if (PC)
	{
		PC->CreateRoom(GetInputRoomName());
	}
}

void ULobbyUserWidget::OnJoinRoomClicked()
{
	AFPSLobbyPlayerController* PC = GetOwningPlayer<AFPSLobbyPlayerController>();

	if (PC)
	{
		PC->JoinRoom(GetInputRoomName());
	}
}

void ULobbyUserWidget::OnStartRoomClicked()
{
	AFPSLobbyPlayerController* PC = GetOwningPlayer<AFPSLobbyPlayerController>();

	if (PC)
	{
		PC->StartRoom();
	}
}