#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSLobbyGameMode.generated.h"

class AFPSLobbyPlayerController;

UCLASS()
class FPSDEMO_API AFPSLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSLobbyGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	void CreateRoom(AFPSLobbyPlayerController* RequestPlayer, const FString& RoomName);

	void JoinRoom(AFPSLobbyPlayerController* RequestPlayer, const FString& RoomName);

	void StartRoom(AFPSLobbyPlayerController* RequestPlayer);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 MaxPlayersInRoom = 2;

	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	int32 MinPlayersToStart = 2;

	// 这里换成你的真实游戏地图路径
	UPROPERTY(EditDefaultsOnly, Category = "Lobby")
	FString GameMapPath = TEXT("/Game/FirstPerson/Maps/FirstPersonMap");

	UPROPERTY()
	AFPSLobbyPlayerController* HostPlayer = nullptr;

	UPROPERTY()
	TArray<AFPSLobbyPlayerController*> RoomPlayers;

	FString CurrentRoomName;

	void ClearRoom();

	void UpdateLobbyGameState();
};
