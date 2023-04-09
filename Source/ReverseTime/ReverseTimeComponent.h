// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FramePackage.h"
#include "ReverseTimeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class REVERSETIME_API UReverseTimeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UReverseTimeComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
	void SetReversingTime(bool InReversingTime);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Ture when reversing time, collecting data when false
	bool bReversingTime;

	// out of time data, cannot keep reversing
	bool bOutOfData;

	// Actual time since we started reversing time
	float RunningTime;

	// Running count of the Frame Package Delta Times
	float LeftRunningTime;
	float RightRunningTime;

	// The total amount of time recorded in FramePackages
	float RecordedTime;

	TDoubleLinkedList<FFramePackage> StoredFrames;

	void SetActorVariables(FVector Location, FRotator Rotation, FVector LinearVel, FVector AngularVel);
};
