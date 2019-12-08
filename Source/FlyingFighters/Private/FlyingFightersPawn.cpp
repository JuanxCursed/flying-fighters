// Copyright 2010-2019 Married Games. All Rights Reserved.

#include "FlyingFighters/Public/FlyingFightersPawn.h"
#include "FlyingFighters/Public/FlyingFightersDefaultProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/CollisionProfile.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

const FName AFlyingFightersPawn::FireRightBinding("FireRight");

// Sets default values
AFlyingFightersPawn::AFlyingFightersPawn()
{
 	PrimaryActorTick.bCanEverTick = true;

//	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/TwinStick/Meshes/TwinStickUFO.TwinStickUFO"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->RelativeScale3D = FVector(0.37f, 0.37f, 0.37f);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	//ShipMeshComponent->SetStaticMesh(ShipMesh.Object);

	//static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/TwinStick/Audio/TwinStickFire.TwinStickFire"));
	//FireSound = FireAudio.Object;

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
	PlayerInputComponent->BindAxis("Accelerate", this, &AFlyingFightersPawn::AccelerateInput);
	//PlayerInputComponent->BindAction("Fire", this, &AFlyingFightersPawn::FireInput);
	//PlayerInputComponent->BindAction("Bomb", this, &AFlyingFightersPawn::BombInput);
	//PlayerInputComponent->BindAction("Special", this, &AFlyingFightersPawn::SpecialInput);
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

void AFlyingFightersPawn::AccelerateInput(float Value){
	bool bHasInput = !FMath::IsNearlyEqual(Value, 0.f);

	float CurrentAcceleration = bHasInput ? (Value * Acceleration) : (-0.5f * Acceleration);

	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcceleration);
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, DefaultSpeed, MaxSpeed);	
	AddMovementInput(GetActorRightVector() * CurrentForwardSpeed);
}

void AFlyingFightersPawn::TurnUpInput(float Value){
	float TargetHeight = (Value * TurnSpeed);
	TargetHeight += (FMath::Abs(CurrentHeight) * -0.2f);	
	FVector CurrentLocation = GetActorLocation();
	CurrentHeight = CurrentLocation.Z;
	CurrentLocation.Z += FMath::FInterpTo(CurrentHeight, TargetHeight, GetWorld()->GetDeltaSeconds(), 2.f);			
	SetActorLocation(CurrentLocation);
	//TODO: make mesh component rotate on X axis by 16 degrees while is turning

}

void AFlyingFightersPawn::FireInput(){
	if (bCanFire == true){
		UWorld* const World = GetWorld();
		if (World != nullptr){
			World->SpawnActor<AFlyingFightersDefaultProjectile>(GetActorLocation(), FVector::ZeroVector);
		}

		bCanFire = false;
		World->GetTimerManager().SetTimer(TimerHandle_ShotTimerExpired, this, &AFlyingFightersPawn::ShotTimerExpired, FireRate);

		if(FireSound != nullptr){
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
		}

		bCanFire = false;
	}
}
void AFlyingFightersPawn::BombInput(){}
void AFlyingFightersPawn::SpecialInput(){}