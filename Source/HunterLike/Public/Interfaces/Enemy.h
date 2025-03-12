// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Enemy.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemy : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class HUNTERLIKE_API IEnemy
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSelect();

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeselect();
};
