#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AScoreManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChanged, int32, NewScore);

UCLASS()
class HUNTERLIKE_API AAScoreManager : public AActor
{
    GENERATED_BODY()

public:
    AAScoreManager();

    // Add to score
    UFUNCTION(BlueprintCallable, Category = "Score")
    void AddScore(int32 Amount);

    // Get current score
    UFUNCTION(BlueprintPure, Category = "Score")
    int32 GetScore() const { return Score; }

    // Event that broadcasts when score changes
    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnScoreChanged OnScoreChanged;

private:
    UPROPERTY(VisibleAnywhere, Category = "Score")
    int32 Score;
};
