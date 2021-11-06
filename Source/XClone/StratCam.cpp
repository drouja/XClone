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
#include "Net/UnrealNetwork.h"

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
	ismoving = false;
}

// Called when the game starts or when spawned
void AStratCam::BeginPlay()
{
	select->SetVisibility(false);
	Super::BeginPlay();

	if (HasAuthority())
		playerteam = Red;
	else
		playerteam = Blue;
	
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
	

	// Get list of friendly actors
	TArray<AActor* > foundpawns;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), Axpawn::StaticClass(), foundpawns);
	for (AActor* Actor : foundpawns)
	{
		Axpawn* foundpawn = Cast<Axpawn>(Actor);
		if (foundpawn != nullptr && foundpawn->team == playerteam)
			friendlypawns.Add(foundpawn);
	}
	focusedpawn = friendlypawns[0];
}

// Called every frame
void AStratCam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (movetodesiredloc) SetActorLocation(FMath::VInterpTo(GetActorLocation(), desiredloc, DeltaTime, 10.0f));
	if (HasAuthority())
	{
		//ismoving = true;
		//UE_LOG(LogTemp, Warning, TEXT("Turn: %s"), ( (battlemanager->turn == 1) ? TEXT("Client") : TEXT("Server") ));
	}
}

// Called to bind functionality to input
void AStratCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AStratCam::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStratCam::MoveRight);
	PlayerInputComponent->BindAxis("Scroll", this, &AStratCam::Zoom);
	PlayerInputComponent->BindAxis("Shift", this, &AStratCam::RotateCam);
	PlayerInputComponent->BindAxis("MouseMove", this, &AStratCam::MouseMovement);
	PlayerInputComponent->BindAction("Tab", IE_Pressed, this, &AStratCam::ChangeFocus);
	PlayerInputComponent->BindAction("LeftClick", IE_Pressed, this, &AStratCam::RequestMove);
	PlayerInputComponent->BindAction("Enter", IE_Pressed, this, &AStratCam::EndTurn);

}

void AStratCam::server_requestmove_Implementation(FVector loc, FRotator rot, Axpawn* focusedpawn1)
{
	focusedpawn1->SetActorLocationAndRotation(loc, rot);
}

bool AStratCam::server_requestmove_Validate(FVector loc, FRotator rot, Axpawn* focusedpawn1)
{
	return true;
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

void AStratCam::MouseMovement(float Value)
{
	HighlightTile();
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
	//If pawn currently moving, dont pathfind, if not your turn dont pathfind
	if (battlemanager->ismoving || !ismyturn()) return;

	//Get highlighted tile
	FHitResult outhit{};
	GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility,false,outhit);
	Atile* tile = Cast<Atile>(outhit.GetActor());

	//If the thing we're trying to highlight is a tile and it's not the tile we have been highlighting
	if (tile != nullptr && oldtile != tile)
	{
		if (!battlemanager->Pathfind(tile, patharray, focusedpawn)) return;

		oldtile = tile;
		select->SetWorldLocation(tile->GetActorLocation());
		select->SetVisibility(true);

		path->ClearSplinePoints(true);

		path->SetSplinePoints(patharray, ESplineCoordinateSpace::World,true);
		
		clearsplinemesh();

		//Draw path between target and pawn using spline components
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
	if (!ismyturn()) return;
	startmovepawn();
	for (int i{ 0 }; i < pathmesh.Num(); i++)
	{
		if (pathmesh[i] != nullptr)
			pathmesh[i]->DestroyComponent();
	}
}

void AStratCam::EndTurn()
{
	if(!HasAuthority() && battlemanager->turn == 1)
	{
		clearsplinemesh();
		server_endturn();
	}
	else if (HasAuthority() && battlemanager->turn == 0)
	{
		clearsplinemesh();
		battlemanager->turn = 1;
		if (HasAuthority())
		UE_LOG(LogTemp, Warning, TEXT("???????"));
	}
}

void AStratCam::clearsplinemesh()
{
	//Destroy old spline components / stop drawing path between old target and pawn
	for (int i{ 0 }; i < pathmesh.Num(); i++)
	{
		if (pathmesh[i] != nullptr)
		{
			pathmesh[i]->DestroyComponent();
		}
	}
	pathmesh.Empty();
}

void AStratCam::startmovepawn()
{
	if (ismoving || focusedpawn->FindTile()->GetActorLocation() == patharray.Last()) return;
	movedist = 0;
	battlemanager->ismoving = true;
	clearsplinemesh();
	GetWorldTimerManager().SetTimer(movehandle, this, &AStratCam::movepawn, 0.01, true, 0.0f);
}

void AStratCam::movepawn()
{
	movedist += 3;
	if (movedist >= path->GetSplineLength())
	{
		GetWorldTimerManager().ClearTimer(movehandle);
		battlemanager->ismoving = false;
		return;
	}
	FVector loc = path->GetLocationAtDistanceAlongSpline(movedist, ESplineCoordinateSpace::World);
	loc.Z = focusedpawn->GetActorLocation().Z; //Temporary so that pawn doesnt go down into floor
	FRotator rot = path->GetRotationAtDistanceAlongSpline(movedist, ESplineCoordinateSpace::World);
	server_requestmove(loc, rot, focusedpawn);
}

bool AStratCam::ismyturn()
{
	if (HasAuthority() && battlemanager->turn == 0) return true;
	if (!HasAuthority() && battlemanager->turn == 1) return true;
	return false;
}

void AStratCam::server_endturn_Implementation()
{
	battlemanager->turn = 0;
}

bool AStratCam::server_endturn_Validate()
{
	return true;
}
