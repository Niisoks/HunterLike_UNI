#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AScoreManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowScore, int32, CurrentScore);

UCLASS()
class HUNTERLIKE_API AAScoreManager : public AActor
{
    GENERATED_BODY()

public:
    AAScoreManager();

    // Add to score
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    UFUNCTION(BlueprintCallable)
    int32 ShowScore();

    UPROPERTY(BlueprintAssignable, Category = "Score")
    FOnShowScore OnShowScore;

    // Get current score
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const { return Score; }

    // Event that broadcasts when score changes
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScoreChanged OnScoreChanged;

private:
    UPROPERTY(VisibleAnywhere, Category = "Score")
    int32 Score;

    UPROPERTY(VisibleAnywhere, Category = "Score")
    int32 Multiplier{ 1 };
};
