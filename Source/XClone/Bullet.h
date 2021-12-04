// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class XCLONE_API ABullet : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Mesh)
	class UStaticMeshComponent* Bullet_Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FX")
	class UNiagaraComponent* Bullet_Trail;
	FTimerHandle Move_Handle;
	FVector Target;
	UFUNCTION()
	void MoveTo();
public:	
	// Sets default values for this actor's properties
	ABullet();
	UFUNCTION(BlueprintCallable)
	void GoTo(FVector Destination);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
