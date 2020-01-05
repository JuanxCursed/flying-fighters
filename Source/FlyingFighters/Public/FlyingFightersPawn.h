// Copyright 2010-2019 Married Games. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FlyingFightersPawn.generated.h"

UCLASS(Blueprintable)
class FLYINGFIGHTERS_API AFlyingFightersPawn : public APawn
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoomComponent;

private:
	float CurrentForwardSpeed;
	
	float CurrentHeight;
	
	/* Flag to control firing  */
	uint32 bCanFire : 1;

	/** Handle for efficient management of ShotTimerExpired timer */
	FTimerHandle TimerHandle_ShotTimerExpired;

protected:	
	UFUNCTION()
	void AccelerateInput(float Value);
	
	void TurnUpInput(float Value);
	
	void FireInput();

	void BombInput();

	void SpecialInput();	

	static const FName FireRightBinding;

public:	
	AFlyingFightersPawn();

	float HeightMaxLimit;
	float HeightMinLimit;
		
	UPROPERTY(EditAnywhere, Category = Plane)
	float Acceleration;

	UPROPERTY(EditAnywhere, Category = Plane)
	float CurrentTurnSpeed;
	
	UPROPERTY(EditAnywhere, Category = Plane)
	float DefaultTurnSpeed;

	UPROPERTY(EditAnywhere, Category = Plane)
	float IncreaseTurnSpeed;

	UPROPERTY(EditAnywhere, Category = Plane)
	float MaxTurnSpeed;
	
	UPROPERTY(EditAnywhere, Category = Plane)
	float RollLimit;

	UPROPERTY(EditAnywhere, Category = Plane)
	float MaxSpeed;
	
	UPROPERTY(EditAnywhere, Category = Plane)
	float DefaultSpeed;	

	UPROPERTY(EditAnywhere, Category = Plane)
	float MinSpeed;

	/** Offset from the Plane location to spawn projectiles */
	UPROPERTY(Category = Plane, EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget = true) )
	FVector GunOffset;
	
	/* How fast the weapon will fire */
	UPROPERTY(EditAnywhere, Category = Plane)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Plane)
	class USoundBase* FireSound;

	/*** APawn interface ***/
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/*** AActor interface ***/
	virtual void Tick(float DeltaSeconds) override;
	
	/*** AActor interface ***/
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	/* Handler for the fire timer expiry */
	void ShotTimerExpired();

	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const { return CameraBoomComponent; }

};
