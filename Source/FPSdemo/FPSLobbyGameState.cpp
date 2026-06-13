#include "FPSLobbyGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

void AFPSLobbyGameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSLobbyGameState, bHasRoom);
	DOREPLIFETIME(AFPSLobbyGameState, RoomName);
	DOREPLIFETIME(AFPSLobbyGameState, RoomPlayerCount);
	DOREPLIFETIME(AFPSLobbyGameState, MaxRoomPlayers);
}

void AFPSLobbyGameState::SetRoomInfo(
	bool bNewHasRoom,
	const FString& NewRoomName,
	int32 NewPlayerCount,
	int32 NewMaxPlayers
)
{
	if (!HasAuthority())
	{
		return;
	}

	bHasRoom = bNewHasRoom;
	RoomName = NewRoomName;
	RoomPlayerCount = NewPlayerCount;
	MaxRoomPlayers = NewMaxPlayers;

	PrintRoomInfo();
}

void AFPSLobbyGameState::OnRep_RoomInfo()
{
	PrintRoomInfo();
}

void AFPSLobbyGameState::PrintRoomInfo() const
{
	if (!GEngine)
	{
		return;
	}

	if (!bHasRoom)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Cyan,
			TEXT("Lobby: No room currently.")
		);
		return;
	}

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.0f,
		FColor::Cyan,
		FString::Printf(
			TEXT("Room: %s | Players: %d / %d"),
			*RoomName,
			RoomPlayerCount,
			MaxRoomPlayers
		)
	);
}