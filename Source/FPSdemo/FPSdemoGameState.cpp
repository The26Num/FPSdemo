#include "FPSdemoGameState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "FPSdemoCharacter.h"
#include "MyUserWidget.h"

AFPSdemoGameState::AFPSdemoGameState()
{
	Score = 0;
	RemainingEnemies = 0;
	ResultMessage = TEXT("");
}

void AFPSdemoGameState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSdemoGameState, Score);
	DOREPLIFETIME(AFPSdemoGameState, RemainingEnemies);
	DOREPLIFETIME(AFPSdemoGameState, ResultMessage);
}

void AFPSdemoGameState::AddScore(int32 ScoreValue)
{
	if (!HasAuthority())
	{
		return;
	}

	Score += ScoreValue;

	PrintGameState();

	// Server 自己不会触发 OnRep，所以 Server 也要手动刷新 HUD
	UpdateLocalHUD();
}

void AFPSdemoGameState::SetRemainingEnemies(int32 NewEnemyCount)
{
	if (!HasAuthority())
	{
		return;
	}

	RemainingEnemies = FMath::Max(0, NewEnemyCount);

	PrintGameState();

	// Server 自己刷新 HUD
	UpdateLocalHUD();
}

void AFPSdemoGameState::SetResultMessage(const FString& NewMessage)
{
	if (!HasAuthority())
	{
		return;
	}

	ResultMessage = NewMessage;

	UpdateLocalHUD();
}

void AFPSdemoGameState::OnRep_Score()
{
	UpdateLocalHUD();
	PrintGameState();
}

void AFPSdemoGameState::OnRep_RemainingEnemies()
{
	UpdateLocalHUD();
	PrintGameState();
}

void AFPSdemoGameState::OnRep_ResultMessage()
{
	UpdateLocalHUD();
}

void AFPSdemoGameState::UpdateLocalHUD()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (!PC)
	{
		return;
	}

	AFPSdemoCharacter* Player = Cast<AFPSdemoCharacter>(PC->GetPawn());

	if (!Player)
	{
		return;
	}

	UMyUserWidget* HUD = Player->GetHUDWidget();

	if (!HUD)
	{
		return;
	}

	HUD->SetScore(Score);
	HUD->SetEnemyLeft(RemainingEnemies);
	HUD->ShowResult(ResultMessage);
}

void AFPSdemoGameState::PrintGameState()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Cyan,
			FString::Printf(
				TEXT("Score: %d | Enemy Left: %d"),
				Score,
				RemainingEnemies
			)
		);
	}
}