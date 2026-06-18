// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSdemoGameMode.h"
#include "FPSdemoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyCharacter.h"
#include "Engine/Engine.h"
#include "MyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FPSdemoGameState.h"
#include "Engine/TargetPoint.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "FPSdemoPlayerState.h"

AFPSdemoGameMode::AFPSdemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	/*Score = 0;
	RemainingEnemies = 0;*/

	GameStateClass = AFPSdemoGameState::StaticClass();
	PlayerStateClass = AFPSdemoPlayerState::StaticClass();
}

void AFPSdemoGameMode::BeginPlay()
{
	Super::BeginPlay();

	FindEnemySpawnPoints();

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (FPSGameState)
	{
		FPSGameState->SetRemainingEnemies(0);
		FPSGameState->SetResultMessage(TEXT(""));
	}

	// 延迟 1 秒刷第一批，避免玩家和 HUD 还没初始化好
	GetWorldTimerManager().SetTimer(
		InitialSpawnTimerHandle,
		this,
		&AFPSdemoGameMode::SpawnInitialEnemies,
		1.0f,
		false
	);

	StartMatchTimer();
}

void AFPSdemoGameMode::RegisterEnemy(AEnemyCharacter* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	// GameMode 只在 Server 存在
	// 所以这里一定是 Server 逻辑
	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (FPSGameState)
	{
		FPSGameState->SetRemainingEnemies(
			FPSGameState->RemainingEnemies + 1
		);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Cyan,
			FString::Printf(
				TEXT("Server Register Enemy. Remaining: %d"),
				FPSGameState ? FPSGameState->RemainingEnemies : -1
			)
		);
	}
}

void AFPSdemoGameMode::OnEnemyKilled(AEnemyCharacter* Enemy, int32 ScoreValue, AFPSdemoCharacter* KillerCharacter)
{
	if (bMatchEnded)
	{
		return;
	}

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (!FPSGameState)
	{
		return;
	}
	// 只更新场上敌人数
	FPSGameState->SetRemainingEnemies(FPSGameState->RemainingEnemies - 1);

	// 给击杀者加个人分数
	if (KillerCharacter)
	{
		AFPSdemoPlayerState* KillerPlayerState =
			KillerCharacter->GetPlayerState<AFPSdemoPlayerState>();

		if (KillerPlayerState)
		{
			KillerPlayerState->AddPersonalScore(ScoreValue);

			KillerCharacter->ClientUpdatePersonalScoreHUD(
				KillerPlayerState->PersonalScore
			);
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			TEXT("Enemy killed. Personal score updated.")
		);
	}

	// 死一个，3 秒后随机补一个
	ScheduleEnemyRespawn();
}

void AFPSdemoGameMode::FindEnemySpawnPoints()
{
	EnemySpawnPoints.Empty();

	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsWithTag(
		GetWorld(),
		FName(TEXT("EnemySpawn")),
		FoundActors
	);

	for (AActor* Actor : FoundActors)
	{
		ATargetPoint* SpawnPoint = Cast<ATargetPoint>(Actor);

		if (SpawnPoint)
		{
			EnemySpawnPoints.Add(SpawnPoint);
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			4.0f,
			FColor::Yellow,
			FString::Printf(
				TEXT("Found Enemy Spawn Points: %d"),
				EnemySpawnPoints.Num()
			)
		);
	}
}

void AFPSdemoGameMode::SpawnInitialEnemies()
{
	if (!HasAuthority())
	{
		return;
	}

	if (EnemySpawnPoints.Num() <= 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				4.0f,
				FColor::Red,
				TEXT("No EnemySpawn points found!")
			);
		}

		return;
	}

	const int32 SpawnCount = FMath::Min(MaxAliveEnemies, EnemySpawnPoints.Num());

	for (int32 i = 0; i < SpawnCount; i++)
	{
		SpawnEnemyAtPoint(EnemySpawnPoints[i]);
	}
}

void AFPSdemoGameMode::SpawnOneEnemy()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bMatchEnded)
	{
		return;
	}

	if (EnemySpawnPoints.Num() <= 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				4.0f,
				FColor::Red,
				TEXT("No EnemySpawn points found!")
			);
		}

		return;
	}

	const int32 RandomIndex = FMath::RandRange(0, EnemySpawnPoints.Num() - 1);

	ATargetPoint* SpawnPoint = EnemySpawnPoints[RandomIndex];

	SpawnEnemyAtPoint(SpawnPoint);
}

void AFPSdemoGameMode::ScheduleEnemyRespawn()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bMatchEnded)
	{
		return;
	}

	FTimerHandle RespawnTimerHandle;

	GetWorldTimerManager().SetTimer(
		RespawnTimerHandle,
		this,
		&AFPSdemoGameMode::SpawnOneEnemy,
		RespawnDelay,
		false
	);
}

void AFPSdemoGameMode::SpawnEnemyAtPoint(ATargetPoint* SpawnPoint)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!EnemyClass)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				4.0f,
				FColor::Red,
				TEXT("EnemyClass is not set in GameMode!")
			);
		}

		return;
	}

	if (!SpawnPoint)
	{
		return;
	}

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (FPSGameState && FPSGameState->RemainingEnemies >= MaxAliveEnemies)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	AEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AEnemyCharacter>(
		EnemyClass,
		SpawnPoint->GetActorLocation(),
		SpawnPoint->GetActorRotation(),
		SpawnParams
	);

	if (SpawnedEnemy)
	{
		// 关键：让运行时生成的 Enemy 也生成 AIController
		SpawnedEnemy->SpawnDefaultController();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.0f,
				FColor::Green,
				TEXT("Spawned one enemy.")
			);
		}
	}
}

void AFPSdemoGameMode::StartMatchTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	bMatchEnded = false;

	CurrentRemainingTime = MatchDuration;

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (FPSGameState)
	{
		FPSGameState->SetRemainingTime(CurrentRemainingTime);
	}

	GetWorldTimerManager().SetTimer(
		MatchTimerHandle,
		this,
		&AFPSdemoGameMode::TickMatchTimer,
		1.0f,
		true
	);
}

void AFPSdemoGameMode::TickMatchTimer()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bMatchEnded)
	{
		return;
	}

	CurrentRemainingTime--;

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (FPSGameState)
	{
		FPSGameState->SetRemainingTime(CurrentRemainingTime);
	}

	if (CurrentRemainingTime <= 0)
	{
		FinishMatch();
	}
}

void AFPSdemoGameMode::FinishMatch()
{
	if (!HasAuthority())
	{
		return;
	}

	if (bMatchEnded)
	{
		return;
	}

	bMatchEnded = true;

	GetWorldTimerManager().ClearTimer(MatchTimerHandle);

	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (!FPSGameState)
	{
		return;
	}

	AFPSdemoPlayerState* BestPlayerState = nullptr;
	int32 BestScore = -1;
	bool bTie = false;

	for (APlayerState* PlayerState : FPSGameState->PlayerArray)
	{
		AFPSdemoPlayerState* FPSPlayerState = Cast<AFPSdemoPlayerState>(PlayerState);

		if (!FPSPlayerState)
		{
			continue;
		}

		if (FPSPlayerState->PersonalScore > BestScore)
		{
			BestScore = FPSPlayerState->PersonalScore;
			BestPlayerState = FPSPlayerState;
			bTie = false;
		}
		else if (FPSPlayerState->PersonalScore == BestScore)
		{
			bTie = true;
		}
	}

	if (bTie)
	{
		FPSGameState->SetResultMessage(TEXT("TIME UP! DRAW!"));
	}
	else if (BestPlayerState)
	{
		FPSGameState->SetResultMessage(
			FString::Printf(
				TEXT("TIME UP! WINNER: %s | SCORE: %d"),
				*BestPlayerState->GetPlayerName(),
				BestScore
			)
		);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::Yellow,
			FPSGameState->ResultMessage
		);
	}
}