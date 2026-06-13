#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSdemoGameState.generated.h"

UCLASS()
class FPSDEMO_API AFPSdemoGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AFPSdemoGameState();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	// 当前总分
	UPROPERTY(ReplicatedUsing = OnRep_Score, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 Score = 0;

	// 剩余敌人数量
	UPROPERTY(ReplicatedUsing = OnRep_RemainingEnemies, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	int32 RemainingEnemies = 0;

	// 胜利 / 失败提示
	UPROPERTY(ReplicatedUsing = OnRep_ResultMessage, VisibleAnywhere, BlueprintReadOnly, Category = "Game")
	FString ResultMessage;

	// 只能由 Server 调用
	void AddScore(int32 ScoreValue);

	// 只能由 Server 调用
	void SetRemainingEnemies(int32 NewEnemyCount);

	// Server 调用：设置结果提示，比如 VICTORY
	void SetResultMessage(const FString& NewMessage);

	// 更新当前本地玩家的 HUD
	void UpdateLocalHUD();


protected:
	UFUNCTION()
	void OnRep_Score();

	UFUNCTION()
	void OnRep_RemainingEnemies();

	UFUNCTION()
	void OnRep_ResultMessage();


	void PrintGameState();
};