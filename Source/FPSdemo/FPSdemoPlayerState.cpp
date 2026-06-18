#include "FPSdemoPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"

AFPSdemoPlayerState::AFPSdemoPlayerState()
{
	PersonalScore = 0;
}

void AFPSdemoPlayerState::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSdemoPlayerState, PersonalScore);
}

void AFPSdemoPlayerState::AddPersonalScore(int32 ScoreValue)
{
	if (!HasAuthority())
	{
		return;
	}

	PersonalScore += ScoreValue;
}

void AFPSdemoPlayerState::OnRep_PersonalScore()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Cyan,
			FString::Printf(TEXT("My replicated score changed: %d"), PersonalScore)
		);
	}
}