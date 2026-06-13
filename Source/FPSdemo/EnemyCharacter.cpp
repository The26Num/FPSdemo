// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "Engine/Engine.h"
#include "FPSdemoCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "FPSdemoGameMode.h"


// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
 

    DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(RootComponent);//将DamageSphere附加到角色的根组件上
    DamageSphere->SetSphereRadius(100.0f);

    DamageSphere->SetGenerateOverlapEvents(true);

	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);//设置碰撞为查询模式
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);//忽略所有碰撞通道
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);//对Pawn通道设置为重叠响应
	//绑定重叠事件
    DamageSphere->OnComponentBeginOverlap.AddDynamic(
        this,
        &AEnemyCharacter::OnDamageSphereOverlap
    );
	//绑定结束重叠事件
    DamageSphere->OnComponentEndOverlap.AddDynamic(
        this,
        &AEnemyCharacter::OnDamageSphereEndOverlap
    );
    //
    GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

    PrimaryActorTick.bCanEverTick = true;

    bReplicates = true;
    SetReplicateMovement(true);

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth; //设置生命值

    AFPSdemoGameMode* GameMode = Cast<AFPSdemoGameMode>(
        UGameplayStatics::GetGameMode(GetWorld())
	);//创建别的类的实例，获取游戏模式实例

    if (GameMode)
    {
        GameMode->RegisterEnemy(this);
    }

    PlayerTarget = Cast<AFPSdemoCharacter>(
        UGameplayStatics::GetPlayerCharacter(GetWorld(), 0)
    );

    if (GEngine)
    {
        if (PlayerTarget)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Green,
                TEXT("Enemy found player")
            );
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                3.0f,
                FColor::Red,
                TEXT("Enemy cannot find player")
            );
        }
    }
}

// Called every frame
void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    if (!PlayerTarget)
    {
        return;
    }

    FVector EnemyLocation = GetActorLocation(); //敌人位置
	FVector PlayerLocation = PlayerTarget->GetActorLocation();//玩家位置

    // 只在水平面追踪，不要往上/往下飞
    FVector Direction = PlayerLocation - EnemyLocation;
    Direction.Z = 0.0f;

    float Distance = Direction.Size();

    if (Distance > StopDistance)
    {
		Direction.Normalize(); //归一化方向向量

        AddMovementInput(Direction, 1.0f, true);

		FRotator LookRotation = Direction.Rotation(); //计算朝向玩家的旋转
        SetActorRotation(FRotator(0.0f, LookRotation.Yaw, 0.0f));
    }
}

// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::ReceiveDamage(float DamageAmount)
{
    if (!HasAuthority())
    {
        return;
    }

    if (bIsDead)
    {
        return;
    }
    CurrentHealth -= DamageAmount;

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(
            -1,
            3.0f,
            FColor::Green,
            FString::Printf(
                TEXT("Enemy HP: %.1f"),
                CurrentHealth)
        );
    }

    if (CurrentHealth <= 0.0f)
    {
        bIsDead = true;

        AFPSdemoGameMode* GameMode = Cast<AFPSdemoGameMode>(
            UGameplayStatics::GetGameMode(GetWorld())
        );

        if (GameMode)
        {
            GameMode->OnEnemyKilled(this, ScoreValue);
        }

        Destroy();
    }
}

void AEnemyCharacter::OnDamageSphereOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult)
{
    AFPSdemoCharacter* Player = Cast<AFPSdemoCharacter>(OtherActor);

    if (Player)
    {
        CurrentTargetPlayer = Player;//选定对象

        ApplyDamageToPlayer();//

        GetWorldTimerManager().SetTimer(
            DamageTimerHandle,
            this,
            &AEnemyCharacter::ApplyDamageToPlayer,
            DamageInterval,
            true
        );
    }
}

void AEnemyCharacter::OnDamageSphereEndOverlap(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (OtherActor == CurrentTargetPlayer)
    {
        CurrentTargetPlayer = nullptr;

        GetWorldTimerManager().ClearTimer(DamageTimerHandle);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                2.0f,
                FColor::Green,
                TEXT("Player left enemy range")
            );
        }
    }
}

void AEnemyCharacter::ApplyDamageToPlayer()
{
    if (!HasAuthority())
    {
        return;
    }

    AFPSdemoCharacter* Player = Cast<AFPSdemoCharacter>(CurrentTargetPlayer);

    if (Player)
    {
        Player->ReceiveDamage(TouchDamage);

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(
                -1,
                1.0f,
                FColor::Yellow,
                TEXT("Enemy damage player")
            );
        }
    }
    else
    {
        GetWorldTimerManager().ClearTimer(DamageTimerHandle);
    }
}
