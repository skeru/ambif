// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "CustomUtils/CameraMacros.h"
#include "BrowserCharacter.h"

#define CHARACTER_MOVEMENT_SPEED_BASE 0.02f
#define CHARACTER_MOVEMENT_SPEED(cameraZoom_current) (CAMERA_ZOOM_PERCENT((cameraZoom_current))*CAMERA_ZOOM_PERCENT((cameraZoom_current)) + CHARACTER_MOVEMENT_SPEED_BASE)

ABrowserCharacter::ABrowserCharacter(const class FPostConstructInitializeProperties& PCIP)
	: Super(PCIP)
{
	CapsuleComponent->SetEnableGravity(false);
	CapsuleComponent->SetCapsuleSize(150.0f, 0.0f);

	//oh yeah, it's high. but it's still limited by the character movement max speed - max acceleration
	//update: oh, no more :p
	SpeedX = 0.5f;
	//oh yeah, it's high. but it's still limited by the character movement max speed - max acceleration
	//update: oh, no more
	SpeedY = 0.5f;

	SpeedZ = 0.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = PCIP.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
	CameraBoom->AttachTo(RootComponent);
	cameraZoom_current = CAMERA_ARM_LENGTH;
	float angle = CAMERA_ANGLE(cameraZoom_current);
	// Camera arm rotation, based on its length 
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetWorldRotation(FRotator(angle, 180, 0));
	CameraBoom->TargetArmLength = cameraZoom_current; // The camera follows at this distance behind the character

	// Create a follow camera
	FollowCamera = PCIP.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
	FollowCamera->AttachTo(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	// Don't rotate when the controller rotates. Let that just affect the camera.
	FollowCamera->bUsePawnControlRotation = false;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	OnActorBeginOverlap.AddDynamic(this, &ABrowserCharacter::OnBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ABrowserCharacter::OnEndOverlap);
}

void ABrowserCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	InputComponent->BindAxis("MoveForward", this, &ABrowserCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABrowserCharacter::MoveRight);
	InputComponent->BindAxis("MoveUp", this, &ABrowserCharacter::MoveUp);

	InputComponent->BindAction("ZoomIn", IE_Pressed, this, &ABrowserCharacter::CameraZoomIn);
	InputComponent->BindAction("ZoomOut", IE_Pressed, this, &ABrowserCharacter::CameraZoomOut);

}

//------------------------------ CAMERA ZOOM ------------------------------

void ABrowserCharacter::CameraZoomIn(){
	cameraZoom_current = FMath::Clamp(cameraZoom_current - CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
	float angle = (CAMERA_ANGLE(cameraZoom_current));
	CameraBoom->SetWorldRotation(FRotator(angle, 180, 0));
	DEBUG("angle should be " + FString::SanitizeFloat(angle));
}

void ABrowserCharacter::CameraZoomOut(){
	cameraZoom_current = FMath::Clamp(cameraZoom_current + CAMERA_ZOOM_STEP, CAMERA_ZOOM_MIN, CAMERA_ZOOM_MAX);
	CameraBoom->TargetArmLength = cameraZoom_current;
	float angle = (CAMERA_ANGLE(cameraZoom_current));
	CameraBoom->SetWorldRotation(FRotator(angle, 180, 0));
	DEBUG("angle should be " + FString::SanitizeFloat(angle));
}

//------------------------- CHARACTER MOVEMEMENT ------------------------------
void ABrowserCharacter::MoveForward(float Val)
{
	if ((Val != 0.f) && (Controller != NULL))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(0, Rotation.Yaw, 0));

		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::X);

		// transform to world space and add it
		AddMovementInput(WorldSpaceAccel, Val * SpeedX * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

void ABrowserCharacter::MoveRight(float Val)
{
	if ((Val != 0.f) && (Controller != NULL))
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(0, Rotation.Yaw, 0));

		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::Y);

		// transform to world space and add it
		AddMovementInput(WorldSpaceAccel, Val * SpeedY * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

void ABrowserCharacter::MoveUp(float Val)
{
	if ((Val != 0.f) && (Controller != NULL))
	{
		/*
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotationMatrix R = FRotationMatrix(FRotator(Rotation.Pitch, 0, Rotation.Roll));
		const FVector WorldSpaceAccel = R.GetScaledAxis(EAxis::Z);
		*/

		const FVector WorldSpaceAccel = FVector(0.0f, 0.0f, 1.0f);

		// transform to world space and add it
		AddMovementInput(WorldSpaceAccel, Val * SpeedZ * (CHARACTER_MOVEMENT_SPEED(cameraZoom_current)));
	}
}

//------------------------- OVERLAP EVENTS ------------------------------
void ABrowserCharacter::OnBeginOverlap(AActor* Other)
{
	//overlap begin
	/* equivalent of C++ line
	* AMapElementActor* element = dynamic_cast<AMapElementActor*>(Other);
	* but works without turning on RTTI compiler settings.
	* Pay attention to pointers. */
	AMapElementActor* element = Cast<AMapElementActor>(Other);
	if (element == NULL) // element is not a MapElementActor
	{
		return;
	}

	if (Manager)
	{
		Manager->SelectElement(element->GetElementID());
	}

	DEBUG("overlap begin")
}

void ABrowserCharacter::OnEndOverlap(AActor* Other)
{
	//overlap end
	/* equivalent of C++ line
	* AMapElementActor* element = dynamic_cast<AMapElementActor*>(Other);
	* but works without turning on RTTI compiler settings.
	* Pay attention to pointers. */
	AMapElementActor* element = Cast<AMapElementActor>(Other);
	if (element == NULL) // element is not a MapElementActor
	{
		return;
	}
	if (Manager)
	{
		Manager->DeselectElement(element->GetElementID());
	}
	DEBUG("overlap end")
}