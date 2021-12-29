// Fill out your copyright notice in the Description page of Project Settings.


#include "tile.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
Atile::Atile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	tilemesh = CreateDefaultSubobject<UStaticMeshComponent>("TileMesh");
	this->SetRootComponent(tilemesh);
	tilemesh->SetMobility(EComponentMobility::Static);
	box = CreateDefaultSubobject<UBoxComponent>("DebugBox");
	box->SetupAttachment(tilemesh);
	box->SetBoxExtent(FVector{ 40,40,40 });
	box->SetRelativeLocation(FVector{0,0,50});
	box->SetMobility(EComponentMobility::Static);
	box->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	box->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECR_Block);

	parent = nullptr;
	gcost = 0;
	hcost = 0;
	fcost = 0;
}

// Called when the game starts or when spawned
void Atile::BeginPlay()
{
	Super::BeginPlay();

	tilemesh->SetMobility(EComponentMobility::Static);
	FHitResult Outhit{};
	TArray<AActor*> actorsToIgnore;
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ 70,0,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		neighbours[0] = Cast<Atile>(Outhit.Actor);
		actorsToIgnore.Add(neighbours[0]);
	}
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ 0,70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		neighbours[1] = Cast<Atile>(Outhit.Actor);
		actorsToIgnore.Add(neighbours[2]);
	}
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ -70,0,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		neighbours[2] = Cast<Atile>(Outhit.Actor);
		actorsToIgnore.Add(neighbours[3]);
	}
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ 0,-70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
	{
		neighbours[3] = Cast<Atile>(Outhit.Actor);
		actorsToIgnore.Add(neighbours[5]);
	}
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ 70,70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		neighbours[4] = Cast<Atile>(Outhit.Actor);
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ -70,-70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		neighbours[5] = Cast<Atile>(Outhit.Actor);
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ 70,-70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		neighbours[6] = Cast<Atile>(Outhit.Actor);
	if (UKismetSystemLibrary::LineTraceSingle(this, GetActorLocation() + FVector{ 0,0,10 }, GetActorLocation() + FVector{ -70,70,10 }, UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_GameTraceChannel2), false, actorsToIgnore, EDrawDebugTrace::None, Outhit, true, FLinearColor::Red, FLinearColor::Green, 0.0f))
		neighbours[7] = Cast<Atile>(Outhit.Actor);

}

// Called every frame


void Atile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

