// Copyright Epic Games, Inc. All Rights Reserved.

#include "FPSdemoCharacter.h"
#include "FPSdemoProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "FPSdemoGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Net/UnrealNetwork.h"
#include "FPSdemoGameState.h"


DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFPSdemoCharacter

AFPSdemoCharacter::AFPSdemoCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetCapsuleComponent());
	BodyMesh->SetRelativeLocation(FVector(0.f, 0.f, -40.f));
	BodyMesh->SetRelativeScale3D(FVector(0.5f, 0.5f, 1.5f));
	BodyMesh->SetOnlyOwnerSee(false);
	BodyMesh->SetOwnerNoSee(true);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFPSdemoCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CurrentHealth = MaxHealth;
	}

	//控制UI
	if (IsLocallyControlled() && HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UMyUserWidget>(
			GetWorld(),
			HUDWidgetClass
		);

		if (HUDWidget)
		{
			HUDWidget->AddToViewport();

			HUDWidget->SetHealth(CurrentHealth);
			HUDWidget->SetScore(0);
			HUDWidget->SetEnemyLeft(0);
			HUDWidget->ShowResult(TEXT(""));

			AFPSdemoGameState* FPSGameState = GetWorld()
				? GetWorld()->GetGameState<AFPSdemoGameState>()
				: nullptr;

			if (FPSGameState)
			{
				FPSGameState->UpdateLocalHUD();
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AFPSdemoCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFPSdemoCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSdemoCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AFPSdemoCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AFPSdemoCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFPSdemoCharacter::ReceiveDamage(float DamageAmount)
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
	if (HUDWidget)
	{
		HUDWidget->SetHealth(CurrentHealth);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Red,
			FString::Printf(TEXT("Player HP: %.1f"), CurrentHealth)
		);
	}

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void AFPSdemoCharacter::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			10.0f,
			FColor::Red,
			TEXT("GAME OVER! Player Dead!")
		);
	}

	// 停止角色移动
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->DisableMovement();
	}

	// 禁止玩家继续移动和转向
	if (AController* MyController = GetController())
	{
		MyController->SetIgnoreMoveInput(true);
		MyController->SetIgnoreLookInput(true);
	}

	if (HUDWidget)
	{
		HUDWidget->ShowResult(TEXT("GAME OVER"));
	}
}

void AFPSdemoCharacter::ServerFireProjectile_Implementation(
	TSubclassOf<AFPSdemoProjectile> InProjectileClass,
	FVector InMuzzleOffset)
{
	FireProjectileOnServer(InProjectileClass, InMuzzleOffset);
}

void AFPSdemoCharacter::FireProjectileOnServer(
	TSubclassOf<AFPSdemoProjectile> InProjectileClass,
	FVector InMuzzleOffset)
{
	if (!HasAuthority()) //用来判断当前代码是不是在服务器权威端运行，生成子弹就应该在服务端运行
	{
		return;
	}

	if (!InProjectileClass)
	{
		return;
	}

	UWorld* World = GetWorld();//先拿世界对象
	if (!World)
	{
		return;
	}

	FRotator SpawnRotation = GetControlRotation();

	FVector SpawnLocation =
		GetActorLocation()
		+ FVector(0.0f, 0.0f, 60.0f)
		+ SpawnRotation.RotateVector(InMuzzleOffset);

	FActorSpawnParameters SpawnParams; //设置生成参数
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	World->SpawnActor<AFPSdemoProjectile>( //生成子弹
		InProjectileClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
}

void AFPSdemoCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFPSdemoCharacter, CurrentHealth);
	DOREPLIFETIME(AFPSdemoCharacter, bIsDead);
}

void AFPSdemoCharacter::OnRep_CurrentHealth()
{
	if (HUDWidget)
	{
		HUDWidget->SetHealth(CurrentHealth);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			2.0f,
			FColor::Yellow,
			FString::Printf(TEXT("CLIENT Player HP Updated: %.1f"), CurrentHealth)
		);
	}
}