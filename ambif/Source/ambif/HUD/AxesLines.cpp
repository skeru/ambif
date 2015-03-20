// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "AxesLines.h"

AAxesLines::AAxesLines() {
	origin = FVector::ZeroVector;
	SetLenght(500.0f);
	t = 50.0f;
	x_color = FColor::Red;
	y_color = FColor::Green;
	z_color = FColor::Blue;

	PrimaryActorTick.bCanEverTick = true;
}

void AAxesLines::SetLenght(float lenght)
{
	l = lenght;
	/*x_end = origin + FVector(l, 0.0f, 0.0f);
	y_end = origin + FVector(0.0f, l, 0.0f);
	z_end = origin + FVector(0.0f, 0.0f, l);*/
}

void AAxesLines::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	x_end = origin + FVector(l, 0.0f, 0.0f);
	y_end = origin + FVector(0.0f, l, 0.0f);
	z_end = origin + FVector(0.0f, 0.0f, l);
	DrawDebugLine(GetWorld(), origin, x_end, x_color, false, -1, 0, t);
	DrawDebugLine(GetWorld(), origin, y_end, y_color, false, -1, 0, t);
	DrawDebugLine(GetWorld(), origin, z_end, z_color, false, -1, 0, t);
} 
