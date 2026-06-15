#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MainMenuPlayerController.generated.h"

class UMainMenuUserWidget;

UCLASS()
class FPSDEMO_API AMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Main Menu")
	TSubclassOf<UMainMenuUserWidget> MainMenuWidgetClass;

	UPROPERTY()
	UMainMenuUserWidget* MainMenuWidget;
};