// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyUserWidget.generated.h"


class UTextBlock;
/**
 * 
 */
UCLASS()
class FPSDEMO_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetHealth(float Health);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetScore(int32 Score);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetEnemyLeft(int32 EnemyLeft);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ShowResult(const FString& ResultText);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetAmmo(int32 CurrentAmmo, int32 MaxAmmo);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void SetTime(int32 NewTime);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_HP;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Score;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_EnemyLeft;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Result;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Ammo;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Text_Time;
};
