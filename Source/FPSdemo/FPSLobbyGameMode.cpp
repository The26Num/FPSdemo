#include "FPSLobbyGameMode.h"
#include "FPSLobbyGameState.h"
#include "FPSLobbyPlayerController.h"
#include "Engine/World.h"

AFPSLobbyGameMode::AFPSLobbyGameMode()
{
	GameStateClass = AFPSLobbyGameState::StaticClass();
	PlayerControllerClass = AFPSLobbyPlayerController::StaticClass();

	// Lobby 里暂时不生成玩家角色
	DefaultPawnClass = nullptr;
}

void AFPSLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFPSLobbyPlayerController* LobbyPC = Cast<AFPSLobbyPlayerController>(NewPlayer);

	if (LobbyPC)
	{
		LobbyPC->ClientLobbyMessage(TEXT("Connected to Lobby Server."));
	}

	UpdateLobbyGameState();
}

void AFPSLobbyGameMode::Logout(AController* Exiting)
{
	AFPSLobbyPlayerController* LeavingPC = Cast<AFPSLobbyPlayerController>(Exiting);

	if (LeavingPC)
	{
		RoomPlayers.Remove(LeavingPC);

		// 房主退出，直接清空房间
		if (LeavingPC == HostPlayer)
		{
			ClearRoom();
		}
	}

	Super::Logout(Exiting);

	UpdateLobbyGameState();
}

void AFPSLobbyGameMode::CreateRoom(
	AFPSLobbyPlayerController* RequestPlayer,
	const FString& RoomName
)
{
	if (!RequestPlayer)
	{
		return;
	}

	FString CleanRoomName = RoomName.TrimStartAndEnd();

	if (CleanRoomName.IsEmpty())
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Create failed: room name is empty."));
		return;
	}

	if (!CurrentRoomName.IsEmpty())
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Create failed: a room already exists."));
		return;
	}

	CurrentRoomName = CleanRoomName;
	HostPlayer = RequestPlayer;

	RoomPlayers.Empty();
	RoomPlayers.Add(RequestPlayer);

	RequestPlayer->ClientLobbyMessage(
		FString::Printf(TEXT("Create room success: %s"), *CurrentRoomName)
	);

	UpdateLobbyGameState();
}

void AFPSLobbyGameMode::JoinRoom(
	AFPSLobbyPlayerController* RequestPlayer,
	const FString& RoomName
)
{
	if (!RequestPlayer)
	{
		return;
	}

	FString CleanRoomName = RoomName.TrimStartAndEnd();

	if (CurrentRoomName.IsEmpty())
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Join failed: room does not exist."));
		return;
	}

	if (CleanRoomName != CurrentRoomName)
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Join failed: room does not exist."));
		return;
	}

	if (RoomPlayers.Contains(RequestPlayer))
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Join failed: you are already in this room."));
		return;
	}

	if (RoomPlayers.Num() >= MaxPlayersInRoom)
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Join failed: room is full."));
		return;
	}

	RoomPlayers.Add(RequestPlayer);

	RequestPlayer->ClientLobbyMessage(
		FString::Printf(TEXT("Join room success: %s"), *CurrentRoomName)
	);

	UpdateLobbyGameState();
}

void AFPSLobbyGameMode::StartRoom(AFPSLobbyPlayerController* RequestPlayer)
{
	if (!RequestPlayer)
	{
		return;
	}

	if (CurrentRoomName.IsEmpty())
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Start failed: no room exists."));
		return;
	}

	if (RequestPlayer != HostPlayer)
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Start failed: only host can start the game."));
		return;
	}

	if (RoomPlayers.Num() < MinPlayersToStart)
	{
		RequestPlayer->ClientLobbyMessage(TEXT("Start failed: not enough players."));
		return;
	}

	FString TravelURL = GameMapPath + TEXT("?listen");

	GetWorld()->ServerTravel(TravelURL);
}

void AFPSLobbyGameMode::ClearRoom()
{
	CurrentRoomName.Empty();
	HostPlayer = nullptr;
	RoomPlayers.Empty();
}

void AFPSLobbyGameMode::UpdateLobbyGameState()
{
	AFPSLobbyGameState* LobbyGameState = GetGameState<AFPSLobbyGameState>();

	if (!LobbyGameState)
	{
		return;
	}

	const bool bHasRoom = !CurrentRoomName.IsEmpty();

	LobbyGameState->SetRoomInfo(
		bHasRoom,
		CurrentRoomName,
		RoomPlayers.Num(),
		MaxPlayersInRoom
	);
}
