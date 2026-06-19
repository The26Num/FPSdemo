// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSdemoGameMode.generated.h"

class AEnemyCharacter;
class ATargetPoint;
class AFPSdemoCharacter;

UCLASS(minimalapi)
class AFPSdemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSdemoGameMode();

	////统计分数
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	//int32 Score = 0;

	////统计剩余敌人数量
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	//int32 RemainingEnemies = 0;

	//注册敌人
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemy(AEnemyCharacter* Enemy);

	//敌人被击杀时调用
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnEnemyKilled(AEnemyCharacter* Enemy, int32 ScoreValue, AFPSdemoCharacter* KillerCharacter);

	UPROPERTY(EditDefaultsOnly, Category = "Match")
	int32 MatchDuration = 20;

	int32 CurrentRemainingTime = 180;

	bool bMatchEnded = false;

	FTimerHandle MatchTimerHandle;

	void StartMatchTimer();

	void TickMatchTimer();

	void FinishMatch();

	void OnPlayerDied(AFPSdemoCharacter* DeadCharacter);

protected:
	virtual void BeginPlay() override;

	// 要生成的敌人蓝图类
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Spawn")
	TSubclassOf<AEnemyCharacter> EnemyClass;

	// 场上最多同时存在几个敌人
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Spawn")
	int32 MaxAliveEnemies = 4;

	// 敌人死亡后几秒补一个
	UPROPERTY(EditDefaultsOnly, Category = "Enemy Spawn")
	float RespawnDelay = 3.0f;

	// 所有刷怪点
	UPROPERTY()
	TArray<ATargetPoint*> EnemySpawnPoints;

	FTimerHandle InitialSpawnTimerHandle;

	void FindEnemySpawnPoints();

	void SpawnInitialEnemies();

	void SpawnOneEnemy();

	void SpawnEnemyAtPoint(ATargetPoint* SpawnPoint);

	void ScheduleEnemyRespawn();
};



