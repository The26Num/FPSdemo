#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FPSdemoPlayerState.generated.h"

UCLASS()
class FPSDEMO_API AFPSdemoPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AFPSdemoPlayerState();

	virtual void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UPROPERTY(ReplicatedUsing = OnRep_PersonalScore, VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 PersonalScore = 0;

	void AddPersonalScore(int32 ScoreValue);

protected:
	UFUNCTION()
	void OnRep_PersonalScore();
};
