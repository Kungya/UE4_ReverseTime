#pragma once
#include "Math/Vector.h"
#include "Math/Rotator.h"
#include "Math/Vector2D.h"

struct FFramePackage
{ // 
	FORCEINLINE FFramePackage();
	FORCEINLINE FFramePackage(FVector InLocation, FRotator InRotation, FVector InLinearVelocity, FVector InAngularVelocity, float DeltaTime);
	// 
	FVector Location;
	FRotator Rotation;
	FVector LinearVelocity;
	FVector AngularVelocity;

	bool bIsPlayerPawn;
	FVector2D MouseXY;

	float DeltaTime;
};

FFramePackage::FFramePackage()
{

}

FFramePackage::FFramePackage(FVector InLocation, FRotator InRotation, FVector InLinearVelocity, FVector InAngularVelocity, float InDeltaTime) :
	Location(InLocation), Rotation(InRotation), LinearVelocity(InLinearVelocity), AngularVelocity(InAngularVelocity), DeltaTime(InDeltaTime)
{
	
}