// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseFloatText.generated.h"

/**
 * 
 */
UCLASS()
class XCLONE_API UBaseFloatText : public UUserWidget
{
	GENERATED_BODY()
	public:
		UFUNCTION(BlueprintNativeEvent)
		void PlayFloatText(int Damage);
};
