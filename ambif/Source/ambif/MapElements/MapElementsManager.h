// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "MapElementActor.h"
#include "CustomUtils/HSVColor.h"
#include "CustomUtils/CustomUtils.h"
#include "Data/Caster.h"
#include "MapElementsManager.generated.h"

/** This actor translates generic invocations on a set of Element IDs
 *  to series of specific invocations on single AMapElementActor objects.
 */
UCLASS()
class AMBIF_API AMapElementsManager : public AActor, public MapElementActorListener
{
	GENERATED_BODY()
public:
	//constructor
	AMapElementsManager();

public:
	//------------------------ACTIONS ON ELEMENTS------------------------
	//-------------------------ELEMENTS MANAGING-------------------------

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Mark an element as selected */
	void SelectElement(FString ElementID);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Unmark a selected element */
	void DeselectElement(FString ElementID);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Unmark all selected elements */
	void DeselectAllElements();

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** TODO documentation */
	void AddElement(FString ElementID, AMapElementActor* Element);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	void SetVisible(FString ElementID, bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	void SetVisibleToSet(TArray<FString> ElementsID, bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	void SetVisibleToAll(bool bVisible);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Highlight a set of elements. 
	 * 
	 * @param ElementIDs Element IDs to enlight. With an empty array all elements will be set to normal.
	 */
	void Highlight(TArray<FString> ElementsIDs);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Set surface color of the specified element  */
	void SetColor(FString ElementID, FColor NewColor);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Set surface color Hue of the specified element  */
	void SetColorHue(FString ElementID, float Hue);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Set surface color Saturation of the specified element  */
	void SetColorSat(FString ElementID, float Sat);

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Move an element to specified position on the map */
	void MoveElementTo(FString ElementID, float x, float y, float z);


	//------------------------SPAWNING FUNCTIONS------------------------
	//deprecated
	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	void SpawnExactlyAt(AActor* actor, FVector2D location, float DefaultSpawnHeigth);//used only for debug purpose

	UFUNCTION(BlueprintCallable, Category = "Element Managing")
	/** Spawn an element of specified map element class in specified position
	 *
	 * @param x x position on the map
	 * @param y y position on the map
	 * @param ActorClass subclass of AMapElementActor to use for constructing element to spawn.
	 */
	AMapElementActor* Spawn(FString ElementID, float x, float y, float z, UClass* ActorClass);

	/** Spawn an element of default map element class in specified position
	 *
	 * @see Spawn(FString, float, float, UClass*)
	 */
	AMapElementActor* Spawn(FString ElementID, float x, float y, float z);

private:
	/** Actor class used when spawning */
	UClass* ActorClass;

	//-------------------------ELEMENTS BUFFER-------------------------
protected:
	/** Here are stored all actors available on the map */
	Utils::FHashMap<AMapElementActor*> ActorMap;

	TSet<FString> ThisTickSelectedElements;
	TSet<FString> ThisTickDeselectedElements;
	TSet<FString> ThisTickClickedElements;

	//------------------------------TICK------------------------------
public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnMouseBeginHover(AActor* TouchedComponent) override;

	UFUNCTION()
	virtual void OnMouseEndHover(AActor* TouchedComponent) override;

	UFUNCTION()
	virtual void OnMouseClick(AActor* TouchedComponent) override;

	//--------------------------TICK HELPER--------------------------
	//these functions must remain public because invoked by logic controller //presentation layer
public:

	/** Harvest set of ID of elements with begin mouse over in this Tick
	 * Automatically removes buffered elements */
	TSet<FString> CollectTickSelectedElements();

	/** Harvest set of ID of elements with end mouse over in this Tick
	 * Automatically removes buffered elements */
	TSet<FString> CollectTickDeselectedElements();

	/** Harvest set of ID of elements with click in this Tick
	 * Automatically removes buffered elements */
	TSet<FString> CollectTickClickedElements();
};
