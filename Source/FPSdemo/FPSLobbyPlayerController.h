#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSLobbyPlayerController.generated.h"

class ULobbyUserWidget;

UCLASS()
class FPSDEMO_API AFPSLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UFUNCTION(exec)
	void CreateRoom(const FString& RoomName);

	UFUNCTION(exec)
	void JoinRoom(const FString& RoomName);

	UFUNCTION(exec)
	void StartRoom();

	UFUNCTION(Server, Reliable)
	void ServerCreateRoom(const FString& RoomName);

	UFUNCTION(Server, Reliable)
	void ServerJoinRoom(const FString& RoomName);

	UFUNCTION(Server, Reliable)
	void ServerStartRoom();

	UFUNCTION(Client, Reliable)
	void ClientLobbyMessage(const FString& Message);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lobby UI")
	TSubclassOf<ULobbyUserWidget> LobbyWidgetClass;

	UPROPERTY()
	ULobbyUserWidget* LobbyWidget;
};
