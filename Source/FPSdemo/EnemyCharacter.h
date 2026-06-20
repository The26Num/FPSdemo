// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;
class AFPSdemoCharacter; //

UCLASS()
class FPSDEMO_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

	// 最大生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float MaxHealth = 100.0f;

	// 当前生命值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	float CurrentHealth;

	// 受到伤害函数
	UFUNCTION(BlueprintCallable, Category = "Enemy")
	void ReceiveDamage(float DamageAmount, AFPSdemoCharacter* DamageCauser);

	// 伤害范围碰撞组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	USphereComponent* DamageSphere;

	// 碰撞函数
	UFUNCTION()
	void OnDamageSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	//碰撞伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float TouchDamage = 10.0f;

	//伤害间隔
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	float DamageInterval = 1.0f;

	//当前正在受到伤害的玩家
	AActor* CurrentTargetPlayer;  

	//定时器句柄
	FTimerHandle DamageTimerHandle;

	//结束碰撞函数
	UFUNCTION()
	void OnDamageSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	//应用伤害函数
	void ApplyDamageToPlayer();

	//移动速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Move")
	float MoveSpeed = 250.0f;

	//停止距离，当敌人距离玩家小于这个值时停止移动
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy|Move")
	float StopDistance = 120.0f;

	//分数值，玩家击杀敌人后获得的分数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
	int32 ScoreValue = 10;

	//状态标志，表示敌人是否已经死亡
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	bool bIsDead = false;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//当前目标玩家
	UPROPERTY()
	AFPSdemoCharacter* PlayerTarget;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
