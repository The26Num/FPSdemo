// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSdemoGameMode.h"
#include "FPSdemoCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "EnemyCharacter.h"
#include "Engine/Engine.h"
#include "MyUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FPSdemoGameState.h"

AFPSdemoGameMode::AFPSdemoGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	/*Score = 0;
	RemainingEnemies = 0;*/

	GameStateClass = AFPSdemoGameState::StaticClass();
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

void AFPSdemoGameMode::OnEnemyKilled(AEnemyCharacter* Enemy, int32 ScoreValue)
{
	AFPSdemoGameState* FPSGameState = GetGameState<AFPSdemoGameState>();

	if (!FPSGameState)
	{
		return;
	}

	// Server 增加分数
	FPSGameState->AddScore(ScoreValue);

	// Server 减少剩余敌人数
	FPSGameState->SetRemainingEnemies(
		FPSGameState->RemainingEnemies - 1
	);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			3.0f,
			FColor::Green,
			FString::Printf(
				TEXT("Server Score: %d, Enemy Left: %d"),
				FPSGameState->Score,
				FPSGameState->RemainingEnemies
			)
		);
	}

	// 胜利判断仍然由 Server 做
	if (FPSGameState->RemainingEnemies <= 0)
	{
		FPSGameState->SetResultMessage(TEXT("VICTORY"));

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				10.0f,
				FColor::Yellow,
				TEXT("VICTORY! All enemies defeated!")
			);
		}
		

		// 这里先不要直接更新某一个 Player 的 HUD
		// 多人情况下 UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
		// 只会拿到 Server 视角下的第 0 个玩家，不代表所有客户端
		//
		// 胜利 UI 后面建议也放进 GameState 复制变量里同步
	}
}