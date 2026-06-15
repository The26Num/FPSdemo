#include "FPSLobbyPlayerController.h"
#include "FPSLobbyGameMode.h"
#include "Engine/Engine.h"
#include "LobbyUserWidget.h"

void AFPSLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	if (LobbyWidgetClass)
	{
		LobbyWidget = CreateWidget<ULobbyUserWidget>(
			this,
			LobbyWidgetClass
		);

		if (LobbyWidget)
		{
			LobbyWidget->AddToViewport();
			LobbyWidget->SetMessage(TEXT("Connected to Lobby."));
		}
	}
}

void AFPSLobbyPlayerController::CreateRoom(const FString& RoomName)
{
	ServerCreateRoom(RoomName);
}

void AFPSLobbyPlayerController::JoinRoom(const FString& RoomName)
{
	ServerJoinRoom(RoomName);
}

void AFPSLobbyPlayerController::StartRoom()
{
	ServerStartRoom();
}

void AFPSLobbyPlayerController::ServerCreateRoom_Implementation(const FString& RoomName)
{
	AFPSLobbyGameMode* LobbyGameMode = GetWorld()
		? GetWorld()->GetAuthGameMode<AFPSLobbyGameMode>()
		: nullptr;

	if (LobbyGameMode)
	{
		LobbyGameMode->CreateRoom(this, RoomName);
	}
}

void AFPSLobbyPlayerController::ServerJoinRoom_Implementation(const FString& RoomName)
{
	AFPSLobbyGameMode* LobbyGameMode = GetWorld()
		? GetWorld()->GetAuthGameMode<AFPSLobbyGameMode>()
		: nullptr;

	if (LobbyGameMode)
	{
		LobbyGameMode->JoinRoom(this, RoomName);
	}
}

void AFPSLobbyPlayerController::ServerStartRoom_Implementation()
{
	AFPSLobbyGameMode* LobbyGameMode = GetWorld()
		? GetWorld()->GetAuthGameMode<AFPSLobbyGameMode>()
		: nullptr;

	if (LobbyGameMode)
	{
		LobbyGameMode->StartRoom(this);
	}
}

void AFPSLobbyPlayerController::ClientLobbyMessage_Implementation(const FString& Message)
{
	if (LobbyWidget)
	{
		LobbyWidget->SetMessage(Message);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			4.0f,
			FColor::Yellow,
			Message
		);
	}
}

void AFPSLobbyPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (LobbyWidget)
	{
		LobbyWidget->RemoveFromParent();
		LobbyWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}