// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Cambot.generated.h"

// Cambot moves around in the scene taking screenshots and serializing actors in the view frustum to a text file if possessed by the local player.
UCLASS()
class AIREVERIE_CHALLENGE_API ACambot : public APawn
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    ACambot();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:	
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Called to bind functionality to input
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // Checks if Cambot can move forward along its current forward vector.
    UFUNCTION(BlueprintCallable)
    bool CanTravel() const;

    // Rotates Cambot by a value between rotateRandomMin and rotateRandomMax.
    UFUNCTION(BlueprintCallable)
    void RotateRandom();

    // Takes a screenshot from Cambot's current view, captures objects in view, then writes that data to a text file asynchronously.
    UFUNCTION(BlueprintCallable)
    void CaptureScreenData() const;

protected:
    // Distance to travel forward when moving to a new location.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Travel")
    float travelDistance;

    // Distance added to travel distance to determine minimum distance that a travel is allowed.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Travel")
    float travelCollisionPillow;

    // Minimum amount to rotate when attempting to move to a new location. 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Travel")
    float rotateRandomMin;

    // Maximum amount to rotate when attempting to move to a new location. 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Travel")
    float rotateRandomMax;

    // Amount of time to wait in seconds before attempting to move to a new location.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Travel")
    float travelInterval;
    
    // Actor types that are ignored in during screen data capture.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data Capture")
    TArray<UClass*> ignoredActorTypesInDataCapture;

private:
    // The path to the screenshot and screen data folder.
    FString viewDataFolderPath;

    // Tracks the current screenshot number. Increments every time that CaptureScreenData() is called.
    mutable int screenshotCounter;

    // Helper function that gets list of actor names in player's view. Using TSet to make sure that the actors in the list of actors we compile are unique.
    TSet<FString> GetActorsInView() const;
};
