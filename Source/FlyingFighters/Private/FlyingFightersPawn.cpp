// Copyright 2010-2019 Married Games. All Rights Reserved.

#include "FlyingFighters/Public/FlyingFightersPawn.h"
//#include "FlyingFightersPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AFlyingFightersPawn::AFlyingFightersPawn()
{
 	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->RelativeScale3D = FVector(0.37f, 0.37f, 0.37f);

	// Create a camera boom attached to the root (capsule)
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->bAbsoluteRotation = true; // Rotation of the character should not affect the rotation of boom
	CameraBoomComponent->bDoCollisionTest = false;
	CameraBoomComponent->TargetArmLength = 1000.f;
	CameraBoomComponent->SocketOffset = FVector(0.f, 0.f, 95.f);
	CameraBoomComponent->RelativeRotation = FRotator(0.f, -90.f, 0.f);

	// Create a camera component and attach to boom component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraBoomComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false; // Prevent to rotate camera when player rotate

	/***
	 * Note: The skeletal mesh and anim blueprints referentes on the MeshComponent (inherited from Character)
	 * are set in the derived blueprint asset named "BP_Player", not here!!!
	 ***/
	
	// Set handling parameters
	Acceleration = 700.f;
	TurnSpeed = 50.f;
	MaxSpeed = 5000.f;
	DefaultSpeed = 600.f;
	MinSpeed = 300.f;
	CurrentForwardSpeed = 500.f;	
}

/***
 * Input Setup
 ***/
void AFlyingFightersPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	check(PlayerInputComponent); // check if is not null

	PlayerInputComponent->BindAxis("TurnUp", this, &AFlyingFightersPawn::TurnUpInput);
	PlayerInputComponent->BindAxis("TurnDown", this, &AFlyingFightersPawn::TurnDownInput);
	PlayerInputComponent->BindAxis("Accelerate", this, &AFlyingFightersPawn::AccelerateInput);
	PlayerInputComponent->BindAxis("Deccelerate", this, &AFlyingFightersPawn::DeccelerateInput);

}

void AFlyingFightersPawn::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	// this keep ship moving
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);
	AddActorLocalOffset(LocalMove, true); 
}

void AFlyingFightersPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit){
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// TODO: set game over here when ship is crashed
}

void AFlyingFightersPawn::AccelerateInput(float Val){
	bool bHasInput = !FMath::IsNearlyEqual(Val, 0.f);

	float CurrentAcceleration = bHasInput ? (Val * Acceleration) : (-0.5f * Acceleration);

	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcceleration);
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, DefaultSpeed, MaxSpeed);	
}

void AFlyingFightersPawn::TurnUpInput(float Val){
	float TargetHeight = (Val * TurnSpeed);
	TargetHeight += (FMath::Abs(CurrentHeight) * -0.2f);	
	FVector CurrentLocation = GetActorLocation();
	CurrentHeight = CurrentLocation.Z;
	CurrentLocation.Z += FMath::FInterpTo(CurrentHeight, TargetHeight, GetWorld()->GetDeltaSeconds(), 2.f);			
	SetActorLocation(CurrentLocation);
	//TODO: make mesh component rotate on X axis by 16 degrees while is turning
}

void AFlyingFightersPawn::FireInput(){}
void AFlyingFightersPawn::BombInput(){}
void AFlyingFightersPawn::SpecialInput(){}