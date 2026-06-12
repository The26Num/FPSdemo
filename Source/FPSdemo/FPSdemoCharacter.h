// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
//引用
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FPSdemoCharacter.generated.h"

//前向声明
class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UMyUserWidget;
class UStaticMeshComponent;
class AFPSdemoProjectile;


DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFPSdemoCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P; //骨骼网络组件

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BodyMesh; //静态网格组件

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
public:
	AFPSdemoCharacter();

protected:
	virtual void BeginPlay();

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	//人物的最大生命值
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
	float MaxHealth = 100.0f;
	//人物的当前生命值
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player")
	float CurrentHealth;
	//受到伤害函数
	UFUNCTION(BlueprintCallable, Category = "Player")
	void ReceiveDamage(float DamageAmount);
	//人物是否死亡
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	//人物死亡函数
	void Die();

	//
	UMyUserWidget* GetHUDWidget() const { return HUDWidget; }

	UFUNCTION(Server, Reliable)  //ue的RPC声明，意思是这个函数是客户端调用，但是在服务器上执行  Reliable代表是可靠发送，这个太多会导致网络拥塞
	void ServerFireProjectile( //客户端调用这个函数，ue会把这个调用发给服务器，服务器收到后会执行ServerFireProjectile_Implementation
		TSubclassOf<AFPSdemoProjectile> InProjectileClass,//要生成哪一种子弹类
		FVector InMuzzleOffset//枪口相对于角色位置的偏移量
	);       

	void FireProjectileOnServer( //真正执行生成子弹的逻辑
		TSubclassOf<AFPSdemoProjectile> InProjectileClass,
		FVector InMuzzleOffset
	);



protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UMyUserWidget> HUDWidgetClass;

	UPROPERTY()
	UMyUserWidget* HUDWidget;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface
	

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

