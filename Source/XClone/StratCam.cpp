// Fill out your copyright notice in the Description page of Project Settings.

#include "StratCam.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/World.h"


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

	speed = 12;
	scrollspeed = 5000;
}

// Called when the game starts or when spawned
void AStratCam::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStratCam::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AStratCam::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AStratCam::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AStratCam::MoveRight);
	PlayerInputComponent->BindAxis("Scroll", this, &AStratCam::Zoom);
	PlayerInputComponent->BindAxis("Shift", this, &AStratCam::RotateCam);

}

void AStratCam::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FVector fv = GetActorForwardVector() * Value;
		fv.Z = 0;
		SetActorLocation(GetActorLocation() + fv * speed);
	}
}

void AStratCam::MoveRight(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		FVector rv = GetActorRightVector() * Value;
		rv.Z = 0;
		SetActorLocation(GetActorLocation() + rv * speed);
	}
}

void AStratCam::Zoom(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		SpringArmComp->TargetArmLength = FMath::FInterpTo(SpringArmComp->TargetArmLength, FMath::Clamp(SpringArmComp->TargetArmLength + Value * scrollspeed, 100.0f, 1200.0f), GetWorld()->GetDeltaSeconds(), 7.0f);
	}
}

void AStratCam::RotateCam(float Value)
{
	if (Controller != nullptr && Value!= 0.0f)
	{
		SetActorRotation(FMath::RInterpTo(GetActorRotation(), GetActorRotation() + FRotator{ 0,Value*20,0 }, GetWorld()->DeltaTimeSeconds, 10.0f));
	}
}
