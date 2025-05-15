// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/AScoreManager.h"

AAScoreManager::AAScoreManager()
{
    PrimaryActorTick.bCanEverTick = false;
    Score = 0;
}

void AAScoreManager::AddScore(int32 Amount)
{
    if (Amount > 0) { Multiplier += Amount / 10; }
    else { Multiplier = 1; }
    Score += Amount * Multiplier;
    OnScoreChanged.Broadcast(Score);
}

int AAScoreManager::ShowScore()
{
    OnShowScore.Broadcast(Score);
    return Score;
}
