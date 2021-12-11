// Fill out your copyright notice in the Description page of Project Settings.


#include "xpawn.h"
#include "tile.h"
#include "Bullet.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
Axpawn::Axpawn()
{
	bReplicates = true;
	
	
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);
	pawnmesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pawnmesh->SetupAttachment(Arootcomponent);
	gunmesh = CreateDefaultSubobject<UStaticMeshComponent>("GunMesh");
	gunmesh->SetupAttachment(Arootcomponent);
	collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	collider->SetupAttachment(Arootcomponent);
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(Arootcomponent);
	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Cam->SetupAttachment(SpringArmComp);
	HeadLoc = CreateDefaultSubobject<USceneComponent>("HeadLoc");
	HeadLoc->SetupAttachment(Arootcomponent);
	BodyLoc = CreateDefaultSubobject<USceneComponent>("BodyLoc");
	BodyLoc->SetupAttachment(Arootcomponent);
	LegLoc = CreateDefaultSubobject<USceneComponent>("LegLoc");
	LegLoc->SetupAttachment(Arootcomponent);
	GunLoc = CreateDefaultSubobject<USceneComponent>("GunLoc");
	GunLoc->SetupAttachment(Arootcomponent);
	MuzzleFlash = CreateDefaultSubobject<UNiagaraComponent>("MuzzleFlash");
	MuzzleFlash->SetupAttachment(gunmesh);
	MuzzleFlash->Deactivate();

	ActionsLeft = 2;

	movedist = 5;

	Health = 5;
	MaxHealth = Health;

	MaxDamage = 3;

}

// Called when the game starts or when spawned
void Axpawn::BeginPlay()
{
	Super::BeginPlay();
	team_mat = UMaterialInstanceDynamic::Create(pawnmesh->GetMaterial(0), this);
	pawnmesh->SetMaterial(0,team_mat);
	if (team == Red)
	{
		team_colour = FLinearColor{1,0,0};
	}
	else if (team == Blue)
	{
		team_colour = FLinearColor{0,0,1};
	}
	team_mat->SetVectorParameterValue(TEXT("team_colour"),team_colour);
}

void Axpawn::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(Axpawn, Health);
}

void Axpawn::Attack(float Acc_Modifier, FVector Target, Axpawn * TargetPawn)
{
	ShootFx(Target);
	// Attempt to damage pawn
	TargetPawn->TakeDamage(Acc_Modifier,MaxDamage);
}

void Axpawn::TakeDamage(float Acc_Modifier, int MaxDamage_)
{
	if (UKismetMathLibrary::RandomBoolWithWeight(Acc_Modifier))
	{
		UE_LOG(LogTemp, Warning, TEXT("Some warning message") );
		if (Acc_Modifier>0.7) Health-=MaxDamage_;
		else if (Acc_Modifier>0.3)  Health-=UKismetMathLibrary::RandomIntegerInRange(1,MaxDamage_);
		else Health-=1;
	}
}

void Axpawn::ShootFx(FVector Target)
{
	FActorSpawnParameters SpawnParams;
 
	//Actual Spawn. The following function returns a reference to the spawned actor
	ABullet* ActorRef = GetWorld()->SpawnActor<ABullet>(ABulletBP, GunLoc->GetComponentTransform(), SpawnParams);
	MuzzleFlash->Activate(true);
	ActorRef->GoTo(Target);
}

// Called every frame
void Axpawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void Axpawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


//Gets tile actor is on, returns nullptr if none found
Atile* Axpawn::FindTile()
{
	FHitResult Outhit{};
	TArray<AActor*> actorsToIgnore;
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation(), GetActorLocation() + FVector{ 0,0,-50 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		return Cast<Atile>(Outhit.Actor);
	}
	return nullptr;
}

