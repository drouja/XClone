// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Niagara/Public/NiagaraComponent.h"
#include "Niagara/Public/NiagaraFunctionLibrary.h"

void ABullet::MoveTo()
{
	SetActorLocation(UKismetMathLibrary::VInterpTo(GetActorLocation(),
		Target,GetWorld()->DeltaTimeSeconds,1.0f));
	if(GetActorLocation().Equals(Target,1))
	{
		GetWorldTimerManager().ClearTimer(Move_Handle);
		Destroy();
	}
}

// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	Bullet_Mesh = CreateDefaultSubobject<UStaticMeshComponent>("BulletMesh");
	this->SetRootComponent(Bullet_Mesh);
	Bullet_Trail = CreateDefaultSubobject<UNiagaraComponent>("BulletTrail");
	Bullet_Trail->SetupAttachment(Bullet_Mesh);

}

void ABullet::GoTo(FVector Destination)
{
	Target = Destination;
	FRotator Target_Rot = UKismetMathLibrary::MakeRotFromXY(Destination - GetActorLocation(),FVector{0,0,1});
	SetActorRotation(Target_Rot);
	GetWorldTimerManager().ClearTimer(Move_Handle);
	GetWorldTimerManager().SetTimer(Move_Handle, this, &ABullet::MoveTo, 0.001, true, 0.0f);
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

