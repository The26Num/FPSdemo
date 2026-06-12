// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSdemoGameMode.h"
#include "FPSdemoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyCharacter.h"
#include "Engine/Engine.h"
#include "MyUserWidget.h"
#include "Kismet/GameplayStatics.h"

AFPSdemoGameMode::AFPSdemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	Score = 0;
	RemainingEnemies = 0;
}

void AFPSdemoGameMode::RegisterEnemy(AEnemyCharacter* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	RemainingEnemies++;

	AFPSdemoCharacter* Player = Cast<AFPSdemoCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);

	//更新UI显示剩余敌人数量
	if (Player && Player->GetHUDWidget())
	{
		Player->GetHUDWidget()->SetEnemyLeft(RemainingEnemies);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Cyan,
			FString::Printf(TEXT("Enemy Registered. Remaining: %d"), RemainingEnemies)
		);
	}
}

void AFPSdemoGameMode::OnEnemyKilled(AEnemyCharacter* Enemy, int32 ScoreValue)
{
	Score += ScoreValue;
	RemainingEnemies--;

	AFPSdemoCharacter* Player = Cast<AFPSdemoCharacter>(
		UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
	);

	if (Player && Player->GetHUDWidget())
	{
		Player->GetHUDWidget()->SetScore(Score);
		Player->GetHUDWidget()->SetEnemyLeft(RemainingEnemies);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			FString::Printf(TEXT("Score: %d, Enemy Left: %d"), Score, RemainingEnemies)
		);
	}

	if (RemainingEnemies <= 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.0f,
				FColor::Yellow,
				TEXT("VICTORY! All enemies defeated!")
			);
		}

		if (Player && Player->GetHUDWidget())
		{
			Player->GetHUDWidget()->ShowResult(TEXT("VICTORY"));
		}
	}
}