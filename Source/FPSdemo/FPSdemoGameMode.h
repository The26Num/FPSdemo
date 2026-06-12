// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSdemoGameMode.generated.h"

class AEnemyCharacter;

UCLASS(minimalapi)
class AFPSdemoGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFPSdemoGameMode();

	//统计分数
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 Score = 0;

	//统计剩余敌人数量
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 RemainingEnemies = 0;

	//注册敌人
	UFUNCTION(BlueprintCallable, Category = "Game")
	void RegisterEnemy(AEnemyCharacter* Enemy);

	//敌人被击杀时调用
	UFUNCTION(BlueprintCallable, Category = "Game")
	void OnEnemyKilled(AEnemyCharacter* Enemy, int32 ScoreValue);
};



