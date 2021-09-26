// Fill out your copyright notice in the Description page of Project Settings.


#include "xpawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"

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

