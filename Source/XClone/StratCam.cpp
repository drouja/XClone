// Fill out your copyright notice in the Description page of Project Settings.

#include "StratCam.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"
#include "BattleManager.h"
#include "Kismet/GameplayStatics.h"
#include "xpawn.h"
#include "tile.h"
#include "Engine/EngineTypes.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"

// Sets default values
AStratCam::AStratCam()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Arootcomponent = CreateDefaultSubobject<USceneComponent>("Root");
	this->SetRootComponent(Arootcomponent);
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(Arootcomponent);
	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Cam->SetupAttachment(SpringArmComp);
	select = CreateDefaultSubobject<UStaticMeshComponent>("SelectMesh");
	path = CreateDefaultSubobject<USplineComponent>("PathMesh");

	speed = 12;
	scrollspeed = 5000;

	battlemanager = nullptr;

	oldtile = nullptr;

	focusindex = 0;
}

// Called when the game starts or when spawned
void AStratCam::BeginPlay()
{
	select->SetVisibility(false);
	Super::BeginPlay();
	TArray<AActor* >foundactor;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattleManager::StaticClass(), foundactor);
	for (AActor* Actor : foundactor)
	{
		battlemanager = Cast<ABattleManager>(Actor);
	}
	
	// Get controller and enable mouse cursor
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
	}

	// Set timers
	GetWorldTimerManager().SetTimer(findtile, this, &AStratCam::HighlightTile, 0.05f, true, 0.0f);

	// Get list of friendly actors
	TArray<AActor* > foundpawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Axpawn::StaticClass(), foundpawns);
	for (AActor* Actor : foundpawns)
	{
		friendlypawns.Add(Cast<Axpawn>(Actor));
	}
	focusedpawn = friendlypawns[0];
}

// Called every frame
void AStratCam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (movetodesiredloc) SetActorLocation(FMath::VInterpTo(GetActorLocation(), desiredloc, DeltaTime, 10.0f));
}

// Called to bind functionality to input
void AStratCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AStratCam::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStratCam::MoveRight);
	PlayerInputComponent->BindAxis("Scroll", this, &AStratCam::Zoom);
	PlayerInputComponent->BindAxis("Shift", this, &AStratCam::RotateCam);
	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AStratCam::ChangeFocus);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AStratCam::RequestMove);

}

void AStratCam::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		movetodesiredloc = false; //Stop moving to some location
		FVector fv = GetActorForwardVector() * Value;
		fv.Z = 0;
		SetActorLocation(GetActorLocation() + fv * speed);
	}
}

void AStratCam::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		movetodesiredloc = false; //Stop moving to some location
		FVector rv = GetActorRightVector() * Value;
		rv.Z = 0;
		SetActorLocation(GetActorLocation() + rv * speed);
	}
}

void AStratCam::Zoom(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, FMath::Clamp(SpringArmComp->TargetArmLength + Value * scrollspeed, 200.0f, 1200.0f), GetWorld()->GetDeltaSeconds(), 7.0f);
	}
}

void AStratCam::RotateCam(float Value)
{
	if (Controller != nullptr && Value!= 0.0f)
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), GetActorRotation() + FRotator{ 0,Value*20,0 }, GetWorld()->DeltaTimeSeconds, 10.0f));
	}
}

void AStratCam::ChangeFocus()
{
	focusindex++;
	if (focusindex >= friendlypawns.Num()) focusindex = 0;
	focusedpawn = friendlypawns[focusindex];
	FVector loc = focusedpawn->GetActorLocation();
	loc.Z = GetActorLocation().Z;
	MoveTo(loc);
}

void AStratCam::MoveTo(FVector loc)
{
	desiredloc = loc;
	movetodesiredloc = true;
}

void AStratCam::HighlightTile()
{

	FHitResult outhit{};
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility,false,outhit);
	Atile* tile = Cast<Atile>(outhit.GetActor());
	
	if (tile != nullptr && oldtile != tile)
	{
		if (!battlemanager->Pathfind(tile, patharray, focusedpawn)) return;

		oldtile = tile;
		select->SetWorldLocation(tile->GetActorLocation());
		select->SetVisibility(true);

		path->ClearSplinePoints(true);

		path->SetSplinePoints(patharray, ESplineCoordinateSpace::World,true);
		
		for (int i{ 0 }; i < pathmesh.Num(); i++) //For some reason pathmesh.empty doesnt call destructors so we have to do it here :(
		{
			if (pathmesh[i] != nullptr)
			{
				pathmesh[i]->DestroyComponent();
			}
		}

		pathmesh.Empty();

		for (int i{ 0 }; i < patharray.Num() - 1; i++)
		{
			pathmesh.Add(NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass()));
			pathmesh[i]->SetMobility(EComponentMobility::Movable);
			pathmesh[i]->SetupAttachment(path);
			pathmesh[i]->SetStaticMesh(meshref);
			pathmesh[i]->RegisterComponent();
			pathmesh[i]->SetStartAndEnd(path->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local),path->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local), path->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::Local), path->GetTangentAtSplinePoint(i+1, ESplineCoordinateSpace::Local));
		}
	}
}

void AStratCam::RequestMove()
{
	battlemanager->startmovepawn(oldtile, path,focusedpawn);
	for (int i{ 0 }; i < pathmesh.Num(); i++)
	{
		pathmesh[i]->DestroyComponent();
	}
}
