// Copyright Omri Braymok 2021


#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "OprnDoor.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOprnDoor::UOprnDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	// ...
}


// Called when the game starts
void UOprnDoor::BeginPlay()
{
	Super::BeginPlay();
	InitialYaw = GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	OpenAngle += InitialYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component on it, but no PressurePlate set!"), *GetOwner()->GetName());
	}

	ActorThatOpen = GetWorld()->GetFirstPlayerController()->GetPawn();
	
}


// Called every frame
void UOprnDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpen))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}
	else
	{
		//Checks if the time has passed for the door to stay open
		if (GetWorld()->GetTimeSeconds() - DoorLastOpened > DoorCloseDelay) 
			CloseDoor(DeltaTime);
	}

}


void UOprnDoor::OpenDoor(float DeltaTime)
{	
	CurrentYaw = FMath::Lerp(CurrentYaw, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}


void UOprnDoor::CloseDoor(float DeltaTime)
{	
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
}

