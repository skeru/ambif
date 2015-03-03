// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "AxesLines.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API AAxesLines : public AActor
{
	GENERATED_BODY()
	
public:	
	AAxesLines(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "Style")
	void SetLenght(float lenght);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FVector origin;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float l;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	float t;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FColor x_color;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FColor y_color;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Style")
	FColor z_color;

private:
	FVector x_end, y_end, z_end;

public:
	void AAxesLines::Tick(float DeltaTime) override;
};
