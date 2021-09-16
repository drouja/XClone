// Fill out your copyright notice in the Description page of Project Settings.


#include "grid.h"
#include "node.h"
#include "Components/BoxComponent.h"
#include "Math/Vector2D.h"
#include "Math/Vector.h"
#include "DrawDebugHelpers.h"

// Sets default values
Agrid::Agrid()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Initialize Box and set it as root
	box = CreateDefaultSubobject<UBoxComponent>("DebugBox");
	this->SetRootComponent(box);
}

// Called when the game starts or when spawned
void Agrid::BeginPlay()
{
	Super::BeginPlay();
	
	//Set grid size to debug box size
	size = FVector2D{ 2*box->GetUnscaledBoxExtent().X,2*box->GetUnscaledBoxExtent().Y };

	numnodes[0] = size.X / nodeDiameter;
	numnodes[1] = size.Y / nodeDiameter;

	FVector brloc = FVector{this->GetActorLocation().X - (size.X /2), this->GetActorLocation().Y - (size.Y / 2), this->GetActorLocation().Z };

	TArray<TArray<Node>> grid;
	grid.SetNum(numnodes[0]);

	for (int i{ 0 }; i < numnodes[0]; i++)
	{
		grid[i].SetNum(numnodes[1]);
		for (int j{ 0 }; j < numnodes[1]; j++)
		{
			grid[i][j].loc = FVector{ brloc.X + nodeDiameter * i, brloc.Y + nodeDiameter * j, brloc.Z };
			
		}
	}

}

// Called every frame
void Agrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

