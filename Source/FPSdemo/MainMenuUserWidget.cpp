#include "MainMenuUserWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UMainMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (Button_HostLAN)
	{
		Button_HostLAN->OnClicked.AddDynamic(
			this,
			&UMainMenuUserWidget::OnHostLANClicked
		);
	}

	if (Button_JoinLAN)
	{
		Button_JoinLAN->OnClicked.AddDynamic(
			this,
			&UMainMenuUserWidget::OnJoinLANClicked
		);
	}

	SetMessage(TEXT("Input host IP or start a LAN server."));
}

void UMainMenuUserWidget::OnHostLANClicked()
{
	SetMessage(TEXT("Starting LAN Host..."));

	UGameplayStatics::OpenLevel(
		this,
		FName("/Game/FirstPerson/Maps/LobbyMap"),
		true,
		TEXT("listen")
	);
}

void UMainMenuUserWidget::OnJoinLANClicked()
{
	if (!EditableTextBox_ServerIP)
	{
		return;
	}

	FString ServerIP = EditableTextBox_ServerIP->GetText().ToString().TrimStartAndEnd();

	if (ServerIP.IsEmpty())
	{
		SetMessage(TEXT("Please input server IP."));
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (!PC)
	{
		SetMessage(TEXT("No PlayerController found."));
		return;
	}

	SetMessage(FString::Printf(TEXT("Connecting to %s ..."), *ServerIP));

	PC->ClientTravel(ServerIP, TRAVEL_Absolute);
}

void UMainMenuUserWidget::SetMessage(const FString& Message)
{
	if (Text_Message)
	{
		Text_Message->SetText(FText::FromString(Message));
	}
}
