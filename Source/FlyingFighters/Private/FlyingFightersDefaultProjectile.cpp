// Copyright 2010-2019 Married Games. All Rights Reserved.


#include "FlyingFighters/Public/FlyingFightersDefaultProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

// Sets default values
AFlyingFightersDefaultProjectile::AFlyingFightersDefaultProjectile()
{
	// Static reference to the mesh to use for the projectile 
	// TODO: make suitible to blueprint
	static ConstructorHelpers::FObjectFinder<UStaticMesh> ProjectileMeshAsset(TEXT("/Game/Meshes/SM_PrimaryFire.SM_PrimaryFire"));

	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	ProjectileMesh->SetStaticMesh(ProjectileMeshAsset.Object);
	//ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->BodyInstance.SetCollisionProfileName("Projectile");
	ProjectileMesh->OnComponentHit.AddDynamic(this, &AFlyingFightersDefaultProjectile::OnHit);
	RootComponent = ProjectileMesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = ProjectileMesh;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.f;

	InitialLifeSpan = 3.0f; // this variable comes from Actor Class
}	

void AFlyingFightersDefaultProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit){
	if(OtherActor != nullptr && OtherActor != this && OtherComp->IsSimulatingPhysics()){
		//TODO: register damage to actor
	}
	// TODO: maybe add a particle effect before destroy? or on otheracter to "demonstrate damage"
	Destroy();
}
