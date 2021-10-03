// Fill out your copyright notice in the Description page of Project Settings.


#include "xpawn.h"
#include "tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
Axpawn::Axpawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);
	pawnmesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	pawnmesh->SetupAttachment(Arootcomponent);
	collider = CreateDefaultSubobject<UCapsuleComponent>("Collider");
	collider->SetupAttachment(Arootcomponent);

}

// Called when the game starts or when spawned
void Axpawn::BeginPlay()
{
	Super::BeginPlay();
	
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
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Some debug message!"));
		return Cast<Atile>(Outhit.Actor);
	}
	return nullptr;
}

