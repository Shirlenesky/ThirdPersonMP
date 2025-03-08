// Fill out your copyright notice in the Description page of Project Settings.


#include "ThirdPersonMPProjectile.h"

#include "Components/SphereComponent.h"
#include "Particles/ParticleSystem.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AThirdPersonMPProjectile::AThirdPersonMPProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	//SphereComp
	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("RootComp"));
	SphereComp->InitSphereRadius(38.0f);
	SphereComp->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	RootComponent = SphereComp;
	if (GetLocalRole() == ROLE_Authority)
	{
		SphereComp->OnComponentHit.AddDynamic(this, &AThirdPersonMPProjectile::AThirdPersonMPProjectile::OnProjectileImpact);
	}

	
	//StaticMesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh>DefaultMesh(TEXT("/Game/StarterContent/Shapes/Shape_Sphere.Shape_Sphere"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	if (DefaultMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(DefaultMesh.Object);
		StaticMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -38.f));
		StaticMesh->SetRelativeScale3D(FVector(0.75f, 0.75f, 0.75f));
	}
	//explosion effect 
	static ConstructorHelpers::FObjectFinder<UParticleSystem>DefaultExplosionEffect(TEXT("/Game/StarterContent/Particles/P_Explosion.P_Explosion"));
	if (DefaultExplosionEffect.Succeeded())
	{
		ExplosionEffect = DefaultExplosionEffect.Object;
	}
	
	//projectile movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(SphereComp);
	ProjectileMovement->InitialSpeed = 1500.0f;
	ProjectileMovement->MaxSpeed = 1500.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	DamageType = UDamageType::StaticClass();
	Damage = 10;
}

// Called when the game starts or when spawned
void AThirdPersonMPProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AThirdPersonMPProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AThirdPersonMPProjectile::Destroyed()
{
	FVector Location = GetActorLocation();
	UGameplayStatics::SpawnEmitterAtLocation(this, ExplosionEffect, Location);
}

void AThirdPersonMPProjectile::OnProjectileImpact(UPrimitiveComponent* hitComp, AActor* otherActor, UPrimitiveComponent* otherComp, FVector normalImpulse, const FHitResult& hit)
{
	if (otherActor)
	{
		UGameplayStatics::ApplyPointDamage(otherActor, Damage, normalImpulse, hit, GetInstigator()->Controller, this, DamageType);
	}
	Destroyed();
}


