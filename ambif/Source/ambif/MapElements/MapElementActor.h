// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "MapElementActorListener.h"
#include "MapElementActor.generated.h"

#define _ParamName_color "CustomColor"
#define _ElementAsset_Ball "StaticMesh'/Game/StaticMesh/SM_CustomSphere.SM_CustomSphere'"
#define _ElementAsset_BallMaterial "Material'/Game/Material/M_CustomBall.M_CustomBall'"

/**
 * Base class for element to be shown in the map
 */
UCLASS()
class AMBIF_API AMapElementActor : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	AMapElementActor();
	
	//------------------------ ELEMENT BODY ------------------------
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Look")
	USceneComponent* RootScene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Element Look")
	UStaticMeshComponent* Mesh;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkelMeshComponents)
	//UStaticMesh* SubComponent;

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
	FLinearColor _color_backup;
	uint32 _activeColorIndex;
	const float _color_alpha_normal = 1.0;
	const float _color_alpha_ghost = 0.25;
	const float _scale_normal = 15.0;
	const float _scale_ghost = 4.0;

protected:
	uint32 AvailableColorsNum;

	//------------------------ AUXILIARITIES ------------------------
private:
	UMaterialInstanceDynamic* _material;

	//---------------------------- MOVE ----------------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Element Movement")
	/** Move actor to absolute position */
	virtual void MoveTo(float x, float y, float z);

	//------------------- ELEMENT STATE ACCESSORS ------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Call this only if you know what are you doing */
	void SetElementID(FString NewElementID);

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Extract represented element's identifier */
	FString GetElementID();

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Make this element a ghost (hide / reduce alpha channel) */
	virtual void Ghost(bool IsGhost = true);

	//---colors---


	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** read element current color */
	virtual FLinearColor GetColor();

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Force a new element color */
	virtual void SetColor(FColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** set secondary color. Might not be visible. */
	virtual void SetBgColor(FColor NewColor);
	
private:
	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Actually set color in material object */
	virtual void _applyColor(FColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** switch between main color and secondary color */
	virtual void ToggleColor();

public:
	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Number of different colors available for this object */
	uint32 GetAvailableColorNum();

	UFUNCTION(BlueprintCallable, Category = "Element State")
	/** Apply a new color to the object 
	 * 
	 * @param ColorIndex should be a number between 0 and AvailableColorNum.
	 *     - 0 use default (primary) color
	 *     - 1 use background (secondary) color
	 */
	virtual void ApplyColor(uint32 ColorIndex);

	//-------------------- EVENT REGISTRATION --------------------
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
