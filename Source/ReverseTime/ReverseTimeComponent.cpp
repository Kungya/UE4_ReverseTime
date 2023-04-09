// Fill out your copyright notice in the Description page of Project Settings.


#include "ReverseTimeComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ReverseTimeCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "FramePackage.h"


// Sets default values for this component's properties
UReverseTimeComponent::UReverseTimeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...

	// 
	bReversingTime = false;
}


// Called when the game starts
void UReverseTimeComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...



	AReverseTimeCharacter* RevTimeCharacter = Cast<AReverseTimeCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	RevTimeCharacter->ReverseTimeDelegate.AddDynamic(this, &UReverseTimeComponent::SetReversingTime);
}


// Called every frame
void UReverseTimeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (!bReversingTime)
	{ // 1) storing data
		RunningTime = 0.f;
		LeftRunningTime = 0.f;
		RightRunningTime = 0.f;

		AActor* Owner = GetOwner();
		TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());

		if (Components.Num() > 0)
		{
			UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Components[0]);
			if (SMC)
			{
				FFramePackage Package(Owner->GetActorLocation(),
					Owner->GetActorRotation(),
					SMC->GetPhysicsLinearVelocity(),
					SMC->GetPhysicsAngularVelocity(),
					DeltaTime);
				// Head로 갈수록 옛날 데이터, Tail로 갈수록 최근 데이터
				if (RecordedTime < 15.f)
				{
					StoredFrames.AddTail(Package);
					RecordedTime += Package.DeltaTime;
					bOutOfData = false;
				}
				else
				{
					while (RecordedTime >= 15.f)
					{ // Remove Node until RunningTime < 15.f
						auto Head = StoredFrames.GetHead();
						float HeadDT = Head->GetValue().DeltaTime;
						StoredFrames.RemoveNode(Head);
						RecordedTime -= HeadDT;
					}
					StoredFrames.AddTail(Package);
					RecordedTime += Package.DeltaTime;
					bOutOfData = false;
				}
			}
		}
	}
	else if (!bOutOfData)
	{ // Reversing Time
		//TODO : Refactor and...
		RunningTime += DeltaTime * 1.f;
		auto Right = StoredFrames.GetTail();
		auto Left = Right->GetPrevNode();
		LeftRunningTime = RightRunningTime + Right->GetValue().DeltaTime;

		while (RunningTime > LeftRunningTime)
		{ // 현재 Tick의 DeltaTime (RunningTime)이 이전 Tick (ReverseingTime의 첫번째 Tick(Tail))의 DletaTime
			// 보다 클 경우 계속 반복 => 현재 DeltaTime이 이전 보다 크다는 현재 프레임이 상대적으로 감소했다는 뜻이다
			RightRunningTime += Right->GetValue().DeltaTime;
			Right = Left;
			LeftRunningTime += Left->GetValue().DeltaTime;
			Left = Left->GetPrevNode();

			auto Tail = StoredFrames.GetTail();
			RecordedTime -= Tail->GetValue().DeltaTime;
			StoredFrames.RemoveNode(Tail);

			if (Left == StoredFrames.GetHead())
			{
				// TODO : 
				bOutOfData = true;
			}
		}
		// now Left and Right surround RunningTime
		if (RunningTime <= LeftRunningTime && RunningTime >= RightRunningTime)
		{ // can interpolate
			float DT = RunningTime - RightRunningTime;
			float Interval = LeftRunningTime - RightRunningTime;
			float Fraction = DT / Interval;

			FVector InterpLoc = FMath::VInterpTo(Right->GetValue().Location, Left->GetValue().Location, Fraction, 1.f);
			FRotator InterpRot = FMath::RInterpTo(Right->GetValue().Rotation, Left->GetValue().Rotation, Fraction, 1.f);
			FVector InterpLinVel = FMath::VInterpTo(Right->GetValue().LinearVelocity, Left->GetValue().LinearVelocity, Fraction, 1.f);
			FVector InterpAngVel = FMath::VInterpTo(Right->GetValue().AngularVelocity, Left->GetValue().AngularVelocity, Fraction, 1.f);

			SetActorVariables(InterpLoc, InterpRot, InterpLinVel, InterpAngVel);
		}

		/*auto Tail = StoredFrames.GetTail();
		if (Tail)
		{
			// TODO :
			SetActorVariables(Tail->GetValue().Location, Tail->);

			auto Head = StoredFrames.GetHead();
			if (Head == Tail)
			{ // List With Only One Node
				RecordedTime = 0.f;
				bOutOfData = true;
			}
			else
			{
				RecordedTime -= Tail->GetValue().DeltaTime;
			}
			// 만약 OutOfData일 경우 여기로 분기하지도 않았을 것이기 때문에 RemoveNode 가능
			StoredFrames.RemoveNode(Tail);
		}*/
	}
}

void UReverseTimeComponent::SetReversingTime(bool InReversingTime)
{
	bReversingTime = InReversingTime;

	if (InReversingTime)
	{
		UE_LOG(LogTemp, Warning, TEXT("Reversing Time!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Stop Reversing Time!"));
	}
}

void UReverseTimeComponent::SetActorVariables(FVector Location, FRotator Rotation, FVector LinearVel, FVector AngularVel)
{
	AActor* Owner = GetOwner();

	Owner->SetActorLocation(Location);
	Owner->SetActorRotation(Rotation);

	TArray<UActorComponent*> Components = Owner->GetComponentsByClass(UStaticMeshComponent::StaticClass());

	if (Components.Num() > 0)
	{
		UStaticMeshComponent* SMC = Cast<UStaticMeshComponent>(Components[0]);

		if (SMC)
		{
			SMC->SetPhysicsLinearVelocity(LinearVel);
			SMC->SetPhysicsAngularVelocity(AngularVel);

		}
	}
}