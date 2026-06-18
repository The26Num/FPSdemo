// Fill out your copyright notice in the Description page of Project Settings.


#include "MyUserWidget.h"
#include "Components/TextBlock.h"

void UMyUserWidget::SetHealth(float Health)
{
	if (Text_HP)
	{
		Text_HP->SetText(
			FText::FromString(
				FString::Printf(TEXT("HP: %.0f"), Health)
			)
		);
	}
}

void UMyUserWidget::SetScore(int32 Score)
{
	if (Text_Score)
	{
		Text_Score->SetText(
			FText::FromString(
				FString::Printf(TEXT("Score: %d"), Score)
			)
		);
	}
}

void UMyUserWidget::SetEnemyLeft(int32 EnemyLeft)
{
	if (Text_EnemyLeft)
	{
		Text_EnemyLeft->SetText(
			FText::FromString(
				FString::Printf(TEXT("Enemy Left: %d"), EnemyLeft)
			)
		);
	}
}

void UMyUserWidget::ShowResult(const FString& ResultText)
{
	if (Text_Result)
	{
		Text_Result->SetText(FText::FromString(ResultText));
	}
}

void UMyUserWidget::SetAmmo(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (Text_Ammo)
	{
		Text_Ammo->SetText(
			FText::FromString(
				FString::Printf(TEXT("Ammo: %d / %d"), CurrentAmmo, MaxAmmo)
			)
		);
	}
}

void UMyUserWidget::SetTime(int32 NewTime)
{
	if (Text_Time)
	{
		Text_Time->SetText(
			FText::FromString(
				FString::Printf(TEXT("Time: %d"), NewTime)
			)
		);
	}
}
