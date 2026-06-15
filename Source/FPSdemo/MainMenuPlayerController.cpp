#include "MainMenuPlayerController.h"
#include "MainMenuUserWidget.h"

void AMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!IsLocalController())
	{
		return;
	}

	bShowMouseCursor = true;

	FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMainMenuUserWidget>(
			this,
			MainMenuWidgetClass
		);

		if (MainMenuWidget)
		{
			MainMenuWidget->AddToViewport();
		}
	}
}

