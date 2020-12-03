// Fill out your copyright notice in the Description page of Project Settings.


#include "Cambot.h"

// Sets default values
ACambot::ACambot()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    
    travelDistance = 10.0f;
    travelCollisionPillow = 5.0f;
    rotateRandomMin = 5.0f;
    rotateRandomMax = 25.0f;
    travelInterval = 1.0f;

    // Start at 1 as per instruction
    screenshotCounter = 1;

    // Generate unique folder name to place screenshots and actor capture data
    auto currentDateTime = FDateTime::Now();
    viewDataFolderPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectSavedDir() / TEXT("Data") / currentDateTime.ToString());
}

// Called when the game starts or when spawned
void ACambot::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void ACambot::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ACambot::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ACambot::CanTravel() const
{
    auto world = GetWorld();

    // Get line endpoints for line trace.
    float totalTravel = travelDistance + travelCollisionPillow;
    auto cambotLocation = GetActorLocation();
    auto cambotForward = GetActorForwardVector();
    auto travelToLocation = cambotLocation + cambotForward * totalTravel;

    // Do line trace to check if our line segment collides with any objects in the PhysicsBody channel.
    FHitResult hitResult;
    world->LineTraceSingleByChannel(hitResult, cambotLocation, travelToLocation, ECC_PhysicsBody);

    return !hitResult.bBlockingHit;
}

void ACambot::RotateRandom()
{
    // Choose a random rotation in configured rotation range.
    float randomRot = FMath::RandRange(rotateRandomMin, rotateRandomMax);
    randomRot *= FMath::RandBool() ? -1.0f : 1.0f;

    // Rotate Cambot.
    auto cambotRotator = RootComponent->GetComponentRotation();
    cambotRotator.Yaw += randomRot;
    RootComponent->SetWorldRotation(FQuat(cambotRotator));
}

void ACambot::CaptureScreenData() const
{
    auto world = GetWorld();

    // Ensure that Cambot is currently possessed by the player controller.
    auto controller = this->GetController();
    auto localPlayerController = world->GetFirstPlayerController();

    if (controller != nullptr && controller == localPlayerController)
    {
        // Get actors in the local player view.
        auto actorsInView = GetActorsInView();

        // Record actors in view to text file asynchronously through the magic of the task graph!
        FString screenDataFileName = FString::Printf(TEXT("image_%i_actors"), screenshotCounter);
        FString screenDataFilePath = viewDataFolderPath + '/' + screenDataFileName + ".txt";
        FSimpleDelegateGraphTask::CreateAndDispatchWhenReady(FSimpleDelegateGraphTask::FDelegate::CreateLambda([=]() 
        {
            // Combine all the actors into a single string.
            FString delimitedActorList;
            for (auto actorName : actorsInView)
            {
                delimitedActorList += actorName + '\n';
            }

            // Write the string to the path specified.
            FFileHelper::SaveStringToFile(delimitedActorList, *screenDataFilePath);
        }),
            TStatId(),
            nullptr,
            ENamedThreads::GameThread
        );

        // Take screenshot.
        FString screenshotFileName = FString::Printf(TEXT("image_%i"), screenshotCounter);
        FScreenshotRequest::RequestScreenshot(viewDataFolderPath + '/' + screenshotFileName, false, false);

        // Increment the counter so we can create unique file names.
        ++screenshotCounter;
    }
}

TSet<FString> ACambot::GetActorsInView() const
{
    auto world = GetWorld();

    TSet<FString> objectsInView;
    ULocalPlayer* localPlayer = Cast<ULocalPlayer>(world->GetFirstLocalPlayerFromController());

    // Get Player's view frustum and use that to collide with objects in the world. If there is a collision, store the name.
    if (localPlayer != nullptr && localPlayer->ViewportClient != nullptr && localPlayer->ViewportClient->Viewport)
    {
        // A bit of boilerplate code to get the view frustum.
        FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
            localPlayer->ViewportClient->Viewport,
            GetWorld()->Scene,
            localPlayer->ViewportClient->EngineShowFlags)
            .SetRealtimeUpdate(true));

        FVector viewLocation;
        FRotator viewRotation;
        FSceneView* sceneView = localPlayer->CalcSceneView(&viewFamily, viewLocation, viewRotation, localPlayer->ViewportClient->Viewport);
        if (sceneView != nullptr)
        {
            auto frustum = sceneView->ViewFrustum;

            // Find primitives that intersect the view frustum.
            for (TObjectIterator<UPrimitiveComponent> primitiveIter; primitiveIter; ++primitiveIter)
            {
                auto proxy = primitiveIter->SceneProxy;
                if (primitiveIter->GetWorld() == world && proxy != nullptr)
                {
                    // Perform intersection test.
                    auto proxyBounds = proxy->GetBounds();
                    if (frustum.IntersectBox(proxyBounds.Origin, proxyBounds.BoxExtent))
                    {
                        auto proxyOwnerName = proxy->GetOwnerName().ToString();

                        // Find the primitive owner as a component and then get the component's actor owner. We do this because multiple components can comprise an actor.
                        for (TObjectIterator<UActorComponent> componentIter; componentIter; ++componentIter)
                        {
                            auto componentName = componentIter->GetOuter()->GetName();
                            auto componentOwner = componentIter->GetOwner();
                            if (componentName == proxyOwnerName)
                            {
                                // Now make sure that this actor is not of a type that is on our ignore list
                                bool ownerClassIsIgnored = false;
                                for (auto ignoredActorClass : ignoredActorTypesInDataCapture)
                                {
                                    auto ownerClass = componentOwner->GetClass();
                                    if (ownerClass == ignoredActorClass)
                                    {
                                        ownerClassIsIgnored = true;
                                    }
                                }

                                if (!ownerClassIsIgnored)
                                {
                                    objectsInView.Add(componentOwner->GetName());
                                }
                            }
                        }
                            
                    }
                }
            }
        }
    }

    return objectsInView;
}