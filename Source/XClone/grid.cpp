// Fill out your copyright notice in the Description page of Project Settings.


#include "grid.h"
#include "Components/BoxComponent.h"
#include "Math/Vector2D.h"
#include "Math/Vector.h"
#include "Engine/World.h"


void Agrid::UpdateGrid()
{

	//Update Debug Box to show new grid size
	box->SetBoxExtent(FVector{ numnodes[0]*nodeDiameter,numnodes[1] * nodeDiameter,5 });

	//Calculate the location of the bottom right of the grid
	FVector brloc = FVector{ this->GetActorLocation().X - (numnodes[0] * nodeDiameter / 2), this->GetActorLocation().Y - (numnodes[1] * nodeDiameter / 2), this->GetActorLocation().Z };

	//Populate 2D Tarray with Nodes
	grid.SetNum(numnodes[0]);
	for (int i{ 0 }; i < numnodes[0]; i++)
	{
		grid[i].ary.SetNum(numnodes[1]);
		for (int j{ 0 }; j < numnodes[1]; j++)
		{
			FActorSpawnParameters SpawnInfo;
			SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			grid[i].ary[j] = GetWorld()->SpawnActor<Atile>(FVector{ brloc.X + nodeDiameter * i, brloc.Y + nodeDiameter * j, brloc.Z }, FRotator{ 0.0,0.0,0.0 }, SpawnInfo);
			//grid[i].ary[j]->SetActorLocation();
		}
	}
}



// Sets default values
Agrid::Agrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize Box and set it as root
	box = CreateDefaultSubobject<UBoxComponent>("DebugBox");
	this->SetRootComponent(box);
	
	nodeDiameter = 50;

	//Create Dynamic Material
	UpdateGrid();
}

#if WITH_EDITOR
void Agrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (!PropertyChangedEvent.Property)
		return;
	UpdateGrid();
}
#endif

// Called when the game starts or when spawned
void Agrid::BeginPlay()
{
	Super::BeginPlay();
	
	

}

// Called every frame
void Agrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

