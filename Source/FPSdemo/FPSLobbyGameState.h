#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSLobbyGameState.generated.h"

UCLASS()
class FPSDEMO_API AFPSLobbyGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UPROPERTY(ReplicatedUsing = OnRep_RoomInfo, BlueprintReadOnly, Category = "Lobby")
	bool bHasRoom = false;

	UPROPERTY(ReplicatedUsing = OnRep_RoomInfo, BlueprintReadOnly, Category = "Lobby")
	FString RoomName;

	UPROPERTY(ReplicatedUsing = OnRep_RoomInfo, BlueprintReadOnly, Category = "Lobby")
	int32 RoomPlayerCount = 0;

	UPROPERTY(ReplicatedUsing = OnRep_RoomInfo, BlueprintReadOnly, Category = "Lobby")
	int32 MaxRoomPlayers = 2;

	void SetRoomInfo(
		bool bNewHasRoom,
		const FString& NewRoomName,
		int32 NewPlayerCount,
		int32 NewMaxPlayers
	);

protected:
	UFUNCTION()
	void OnRep_RoomInfo();

	void PrintRoomInfo() const;
};
