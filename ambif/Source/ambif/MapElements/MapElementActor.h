// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "MapElementActorListener.h"
#include "MapElementActor.generated.h"

#define _ParamName_color "CustomColor"
#define _ElementAsset_Ball "StaticMesh'/Game/Props/SM_CustomSphere.SM_CustomSphere'"
#define _ElementAsset_BallMaterial "Material'/Game/Props/Materials/M_CustomBall.M_CustomBall'"

/**
 * Base class for element to be shown in the map
 */
UCLASS()
class AMBIF_API AMapElementActor : public AActor
{
	GENERATED_UCLASS_BODY()
		//------------------------ ELEMENT BODY ------------------------
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Look")
	TSubobjectPtr<UStaticMeshComponent> Mesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkelMeshComponents)
	//TSubobjectPtr<UStaticMesh> SubComponent;

	//------------------------ELEMENT STATE ------------------------
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Element Content")
	FString ElementId;

	/** Collection of listeners interested in this actor's mouse events.
	 * Should be Actor manager
	 *
	 * @see MapElementActorListeners
	 * @see MapElementsManager
	 */
	TArray<MapElementActorListener*> Managers;

private:
	FLinearColor _color;

	//------------------------ AUXILIARITIES ------------------------
private:
	UMaterialInstanceDynamic* _material;

	//------------------- ELEMENT STATE ACCESSORS ------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Call this only if you know what are you doing */
	void SetElementID(FString NewElementID);

	UFUNCTION(BlueprintCallable, Category = "Element State")
	FString GetElementID();

	UFUNCTION(BlueprintCallable, Category = "Element State")
	virtual void SetColor(FColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Element State")
	virtual FLinearColor GetColor();

	//-------------------- EVENTE REGISTRATION --------------------
public:
	/** Set Manager Actor for callback notifications */
	void RegisterElementListener(MapElementActorListener* Manager);

	void UnregisterElementListener(MapElementActorListener* Manager);

	//-------------------- EVENT NOTIFICATION --------------------
private:
	UFUNCTION()
	/** Alerts registered managers of mouse event over begin */
	void NotifyMouseBeginOver();

	UFUNCTION()
	/** Alerts registered managers of mouse event over end */
	void NotifyMouseEndOver();

	UFUNCTION()
	/** Alerts registered managers of mouse event click */
	void NotifyMouseClick();
	
	/* if applied to Mesh component instead of actor need to change signature
	void NotifyMouseClick(UPrimitiveComponent* TouchedComponent);*/
};
