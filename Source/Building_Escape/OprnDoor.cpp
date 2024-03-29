// Copyright Omri Braymok 2021

#include "OprnDoor.h"
#include "Components/AudioComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"

#define OUT

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

	FindPressurePlate();
	FindAudioComponent();
}

void UOprnDoor::FindAudioComponent()
{
	AudioComponent = GetOwner()->FindComponentByClass<UAudioComponent>();
	
	if (!AudioComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Missing audio component!"), *GetOwner()->GetName());
	}
}

void UOprnDoor::FindPressurePlate()
{
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component on it, but no PressurePlate set!"), *GetOwner()->GetName());
	}
}

// Called every frame
void UOprnDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (TotalMassOfActors() > MassToOpenDoor)
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

	CloseDoorSound = false;
	if(!AudioComponent) {return;}
	if (!OpenDoorSound)
	{
		AudioComponent->Play();
		OpenDoorSound = true;
	}
}


void UOprnDoor::CloseDoor(float DeltaTime)
{	
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	GetOwner()->SetActorRotation(DoorRotation);
	
	OpenDoorSound = false;
	if(!AudioComponent) {return;}
	if (!CloseDoorSound)
	{
		AudioComponent->Play();
		CloseDoorSound = true;		
	}
}

float UOprnDoor::TotalMassOfActors() const
{
	float TotalMass = 0.f;

	// Find All Overlapping Actors
	TArray<AActor*> OverlappingActors;
	if(!PressurePlate){ return TotalMass; }
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);	

	// Add up their masses to TotalMass
	for (AActor* Actor : OverlappingActors)
	{
		TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}

	return TotalMass;
}


