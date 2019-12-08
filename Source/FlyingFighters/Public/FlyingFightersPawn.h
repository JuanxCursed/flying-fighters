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
	void AccelerateInput(float Value);
	
	void TurnUpInput(float Value);
	
	void FireInput();

	void BombInput();

	void SpecialInput();

	void ShotTimerExpired();

	static const FName FireRightBinding;

public:	
	UPROPERTY(EditAnywhere, Category = Plane)
	float Acceleration;

	// How quicly pawn can steer (up and down movement)
	UPROPERTY(EditAnywhere, Category = Plane)
	float TurnSpeed;

	UPROPERTY(EditAnywhere, Category = Pitch)
	float MaxSpeed;
	
	UPROPERTY(EditAnywhere, Category = Pitch)
	float DefaultSpeed;	

	UPROPERTY(EditAnywhere, Category = Yaw)
	float MinSpeed;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Audio)
	class USoundBase* FireSound;
	
	// Sets default values for this pawn's properties
	AFlyingFightersPawn();

	/*** APawn interface ***/
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/*** AActor interface ***/
	virtual void Tick(float DeltaSeconds) override;
	
	/*** AActor interface ***/
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	FORCEINLINE class UStaticMeshComponent* GetMeshComponent() const { return MeshComponent; }

	FORCEINLINE class UCameraComponent* GetCameraComponent() const { return CameraComponent; }

	FORCEINLINE class USpringArmComponent* GetCameraBoomComponent() const { return CameraBoomComponent; }

};
