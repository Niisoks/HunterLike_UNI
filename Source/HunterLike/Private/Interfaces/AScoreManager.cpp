// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AScoreManager.h"

AAScoreManager::AAScoreManager()
{
    PrimaryActorTick.bCanEverTick = false;
    Score = 0;
}

void AAScoreManager::AddScore(int32 Amount)
{
    Score += Amount;
    OnScoreChanged.Broadcast(Score);
}

