// Copyright 2010-2019 Married Games. All Rights Reserved.

#include "FlyingFightersPawn.h"
#include "FlyingFightersDefaultProjectile.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/CollisionProfile.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundBase.h"

const FName AFlyingFightersPawn::FireRightBinding("FireRight");

// Sets default values
AFlyingFightersPawn::AFlyingFightersPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set this pawn to be controlled by the lowest-numbered player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> ShipMesh(TEXT("/Game/Meshes/SM_StarSparrow09.SM_StarSparrow09"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
	MeshComponent->RelativeScale3D = FVector(0.37f, 0.37f, 0.37f);
	MeshComponent->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	MeshComponent->SetStaticMesh(ShipMesh.Object);

	static ConstructorHelpers::FObjectFinder<USoundBase> FireAudio(TEXT("/Game/Audio/PrimaryFire.PrimaryFire"));
	FireSound = FireAudio.Object;

	// Create a camera boom attached to the root (capsule)
	CameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoomComponent"));
	CameraBoomComponent->SetupAttachment(RootComponent);
	CameraBoomComponent->bAbsoluteRotation = true; // Rotation of the character should not affect the rotation of boom
	CameraBoomComponent->bDoCollisionTest = false;
	CameraBoomComponent->TargetArmLength = 1700.f;
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
	
	// Map
	HeightMaxLimit = 450.f;
	HeightMinLimit = 40.f;

	// Plane 
	Acceleration = 700.f;
	RollLimit = 23.f;
	DefaultTurnSpeed = 0.5f;		
	MaxTurnSpeed = 3.5f;
	IncreaseTurnSpeed = 1.12f;
	CurrentTurnSpeed = DefaultTurnSpeed;
	MaxSpeed = 5000.f;
	DefaultSpeed = 600.f;
	MinSpeed = 300.f;
	CurrentForwardSpeed = 500.f;	

	// Weapon
	GunOffset = FVector(90.f, 0.f, 0.f);
	FireRate = 0.1f;
	bCanFire = true;
}

/***
 * Input Setup
 ***/
void AFlyingFightersPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent){
	check(PlayerInputComponent); // check if is not null

	PlayerInputComponent->BindAxis("TurnUp", this, &AFlyingFightersPawn::TurnUpInput);
	PlayerInputComponent->BindAxis("Accelerate", this, &AFlyingFightersPawn::AccelerateInput);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, &AFlyingFightersPawn::FireInput);
	//PlayerInputComponent->BindAction("Bomb", IE_Pressed, &AFlyingFightersPawn::BombInput);
	//PlayerInputComponent->BindAction("Special", IE_Pressed, &AFlyingFightersPawn::SpecialInput);
}

void AFlyingFightersPawn::Tick(float DeltaSeconds){
	Super::Tick(DeltaSeconds);

	// this keep ship moving
	const FVector LocalMove = FVector(CurrentForwardSpeed * DeltaSeconds, 0.f, 0.f);
	//AddActorLocalOffset(LocalMove, true); 
}

void AFlyingFightersPawn::NotifyHit(class UPrimitiveComponent* MyComp, class AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit){
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	// TODO: set game over here when ship is crashed
}

void AFlyingFightersPawn::AccelerateInput(float Value){
	float CurrentAcceleration = !FMath::IsNearlyEqual(Value, 0.f) ? 
		(Value * Acceleration) : 
		(-0.5f * Acceleration);

	float NewForwardSpeed = CurrentForwardSpeed + (GetWorld()->GetDeltaSeconds() * CurrentAcceleration);
	CurrentForwardSpeed = FMath::Clamp(NewForwardSpeed, DefaultSpeed, MaxSpeed);	
	AddMovementInput(GetActorRightVector() * CurrentForwardSpeed);
}

void AFlyingFightersPawn::TurnUpInput(float Value){
	if (Value != 0.f){		
		FVector CurrentLocation = GetActorLocation();		
		
		bool bGoingUpOrDown = (Value > 0.f)? true : false;

		if (CurrentLocation.Z >= HeightMinLimit && !bGoingUpOrDown || 
			CurrentLocation.Z <= HeightMaxLimit && bGoingUpOrDown){

			CurrentLocation.Z = CurrentLocation.Z + (CurrentTurnSpeed * Value);
			SetActorLocation(CurrentLocation);
			
			FRotator CurrentRotation = GetActorRotation();
			float NewRollValue = CurrentRotation.Roll + (CurrentTurnSpeed * 0.3f * Value * -1.f); // *-1 is needed to make more sense between rotation and direction
			
			if (NewRollValue < 0.f){
					NewRollValue += 360.f;
			}
			if (NewRollValue <=  RollLimit || NewRollValue >= 360.0f - RollLimit){								
				SetActorRotation(FRotator(0.f, 0.f, NewRollValue));
			}
			
			float NewTurnSpeed = CurrentTurnSpeed * IncreaseTurnSpeed;
			if (NewTurnSpeed >= DefaultTurnSpeed && NewTurnSpeed <= MaxTurnSpeed){
				CurrentTurnSpeed = NewTurnSpeed;
			}
		}
	} else{
		CurrentTurnSpeed = DefaultTurnSpeed;
		FRotator CurrentRollPosition = GetActorRotation();
		if (CurrentRollPosition.Roll != 0.f){
			CurrentRollPosition.Roll *= 0.65f;
			if (FMath::IsNearlyZero(CurrentRollPosition.Roll)){
				CurrentRollPosition.Roll = 0.f;
			}				
			SetActorRotation(CurrentRollPosition);
		}
		
	}		
}

void AFlyingFightersPawn::FireInput(){
	if (bCanFire == true){
		UWorld* const World = GetWorld();
		if (World != nullptr){
			World->SpawnActor<AFlyingFightersDefaultProjectile>(GetActorLocation(), FRotator(0.f, 0.f, 0.f));
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

void AFlyingFightersPawn::ShotTimerExpired()
{
	bCanFire = true;
}
