// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "FPSdemoCharacter.h"
#include "FPSdemoProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "MyUserWidget.h"
#include "Engine/Engine.h"
#include "TimerManager.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (!Character)
	{
		return;
	}

	// Server 玩家：直接执行服务器开火逻辑
	if (Character->HasAuthority())
	{
		TryFireOnServer();
	}
	else
	{
		// Client 玩家：请求 Server 使用当前装备武器开火
		Character->ServerFireEquippedWeapon();
	}
}

bool UTP_WeaponComponent::AttachWeapon(AFPSdemoCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	// 检查角色是否有效，以及角色是否已经有武器
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// 挂载武器到第一人称手臂的 GripPoint
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// 把武器组件加入角色实例组件
	Character->AddInstanceComponent(this);

	// 记录当前装备武器
	Character->SetEquippedWeapon(this);

	// 只有 Server 初始化权威 Ammo
	if (Character->HasAuthority())
	{
		CurrentAmmo = MaxAmmo;
		bIsReloading = false;
	}

	// 刷新拥有者 HUD
	UpdateAmmoToOwner();

	// 设置输入绑定
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent =
			Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// 开火
			EnhancedInputComponent->BindAction(
				FireAction,
				ETriggerEvent::Triggered,
				this,
				&UTP_WeaponComponent::Fire
			);

			// 换弹
			if (ReloadAction)
			{
				EnhancedInputComponent->BindAction(
					ReloadAction,
					ETriggerEvent::Started,
					this,
					&UTP_WeaponComponent::Reload
				);
			}
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UTP_WeaponComponent::Reload()
{
	if (!Character)
	{
		return;
	}

	// Server 玩家：直接执行服务器换弹逻辑
	if (Character->HasAuthority())
	{
		TryReloadOnServer();
	}
	else
	{
		// Client 玩家：请求 Server 换弹
		Character->ServerReloadEquippedWeapon();
	}
}

void UTP_WeaponComponent::FinishReload()
{
	if (!Character)
	{
		return;
	}

	// 只有 Server 可以完成换弹
	if (!Character->HasAuthority())
	{
		return;
	}

	CurrentAmmo = MaxAmmo;
	bIsReloading = false;

	UpdateAmmoToOwner();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Green,
			TEXT("Reload Complete")
		);
	}
}

void UTP_WeaponComponent::TryFireOnServer()
{
	if (!Character)
	{
		return;
	}

	// 只有 Server 可以真正判断 Ammo 和生成子弹
	if (!Character->HasAuthority())
	{
		return;
	}

	if (bIsReloading)
	{
		UpdateAmmoToOwner();
		return;
	}

	if (CurrentAmmo <= 0)
	{
		UpdateAmmoToOwner();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				1.5f,
				FColor::Red,
				TEXT("Out of Ammo! Press R to Reload")
			);
		}

		return;
	}

	// Server 扣 Ammo
	CurrentAmmo--;

	// 通知拥有者刷新 HUD
	UpdateAmmoToOwner();

	// Server 生成子弹
	if (ProjectileClass)
	{
		Character->FireProjectileOnServer(ProjectileClass, MuzzleOffset);
	}


	// 播放开火效果
	if (Character->IsLocallyControlled())
	{
		// Listen Server 玩家自己开火
		PlayFireEffects();
	}
	else
	{
		// Client 玩家开火：通知这个 Client 播放自己的声音和动画
		Character->ClientPlayFireEffects();
	}
}

void UTP_WeaponComponent::TryReloadOnServer()
{
	if (!Character)
	{
		return;
	}

	// 只有 Server 可以真正开始换弹
	if (!Character->HasAuthority())
	{
		return;
	}

	if (bIsReloading)
	{
		return;
	}

	if (CurrentAmmo >= MaxAmmo)
	{
		UpdateAmmoToOwner();
		return;
	}

	bIsReloading = true;

	UpdateAmmoToOwner();

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			1.5f,
			FColor::Yellow,
			TEXT("Reloading...")
		);
	}

	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UTP_WeaponComponent::FinishReload,
		ReloadTime,
		false
	);
}

void UTP_WeaponComponent::UpdateAmmoToOwner()
{
	if (!Character)
	{
		return;
	}

	// Listen Server 自己控制的角色，直接刷新本地 HUD
	if (Character->IsLocallyControlled() && Character->GetHUDWidget())
	{
		Character->GetHUDWidget()->SetAmmo(CurrentAmmo, MaxAmmo);
	}

	// Server 通知拥有者 Client 刷新 HUD
	if (Character->HasAuthority())
	{
		Character->ClientUpdateAmmoHUD(
			CurrentAmmo,
			MaxAmmo,
			bIsReloading
		);
	}
}