// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "Runtime/Landscape/Classes/Landscape.h"
#include "Engine/BlockingVolume.h"
#include "GameFramework/PhysicsVolume.h"
#include "CustomUtils/CustomUtils.h"
#include "Data/Caster.h"
#include "Data/DataManager.h"
#include "MapElements/MapElementsManager.h"
#include "PresentationLayer.generated.h"

UENUM(BlueprintType)
namespace PlottableDimension{
	enum Type
	{
		X			UMETA(DisplayName = "X"),
		Y			UMETA(DisplayName = "Y"),
		Z			UMETA(DisplayName = "Z"),
		Color_Hue	UMETA(DisplayName = "Element Color"),
		Color_Sat	UMETA(DisplayName = "Element Color Saturation"),
		Size		UMETA(DisplayName = "Element Size")
		//Something	UMETA(DisplayName = "Something")	
	};
}


struct _DimensionOnMapInfo
{
	FString Id;
	Caster* Caster;
};


/** This actor is meant as a layer between data and map elements.
 * Its purpose is to define how data should be representated on the map.
 *
 * Upon request it triggers an update of the map.
 */
UCLASS()
class AMBIF_API APresentationLayer : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	APresentationLayer(const FObjectInitializer& ObjectInitializer);

public:
	//-----------------OTHER ACTOR REFERENCES-----------------
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor References")
	/** DataManager agent in the scene.
	 *
	 * Need to be manually set in the scene. */
	ADataManager* DataAgent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor References")
	/** DataManager agent in the scene.
	 *
	 * Need to be manually set in the scene. */
	AMapElementsManager* MapElementsManagerAgent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor References")
	/** Landscape actor in the scene.
	 *
	 * Need to be manually set in the scene. */
	ALandscape* Landscape;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Actor References")
	/** Actually used volume. Used to optimize rendering.
	 *
	 * Need to be manually set in the scene. */
	APhysicsVolume* UsedVolume;

	//------------------------INVISIBLE WALLS------------------------------

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invisible Walls")
	/** Top invisible Wall. Automatically moved up/down according to Z dimensions
	 *
	 * Need to be manually set in the scene. */
	ABlockingVolume* TopBlocker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invisible Walls")
	/** Invisible Wall. Automatically scaled according to Z dimensions
	 *
	 * Need to be manually set in the scene.
	 * InvisibleWall should be set as Movable */
	ABlockingVolume* EastBlocker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invisible Walls")
	/** Invisible Wall. Automatically scaled according to Z dimensions
	 *
	 * Need to be manually set in the scene.
	 * InvisibleWall should be set as Movable */
	ABlockingVolume* WestBlocker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invisible Walls")
	/** Invisible Wall. Automatically scaled according to Z dimensions
	 *
	 * Need to be manually set in the scene.
	 * InvisibleWall should be set as Movable */
	ABlockingVolume* NorthBlocker;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Invisible Walls")
	/** Invisible Wall. Automatically scaled according to Z dimensions
	 *
	 * Need to be manually set in the scene.
	 * InvisibleWall should be set as Movable */
	ABlockingVolume* SouthBlocker;

	//-------------------------PRIVATE VARIABLES-------------------------
private:

	/** Usable range of the map */
	float MapBound_x, MapBound_y, MapBound_X, MapBound_Y;

	/** Maps what Data Dimension is rendered as PlottalbeDimension */
	Utils::hashmap<PlottableDimension::Type, _DimensionOnMapInfo> DimensionOnMap;

	//-------------------------ELEMENTS BUFFERS-------------------------
private:
	/** Contains IDs of Selected Elements on the map (mouse hover) */
	//deprecated
	TSet<FString> CurrentlySelectedElements;

	/** Contains IDs of Active Elements on the map (mouse click) */
	//deprecated
	TSet<FString> CurrentlyClickedElements;

	//------------------------SPAWNER SETTINGS-------------------------

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	/** Height where Actor will be placed if no Z dimension is set.
	 * It is best pratice to choose a value right above max map height (when landscape).
	 * Should also correspond to default z value set in csv file (when z dimension is set).
	 */
	float DefaultSpawnHeigth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	/** Actual usage of the landscape actor. (%).
	 * Should be positive.
	 * Only in this portion element will be spawned */
	float MapUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	/** Volume Z
	 *
	 * Used Volume lower bound */
	float MinZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Map Settings")
	/** Volume Z
	 *
	 * Used Volume upper bound. Should be positive. */
	float MaxZ;

	UFUNCTION(BlueprintCallable, Category = "Map Settings")
	/** Updates references to Landscape Bounds, RayCaster, etc... */
	void UpdateVolumeCasterBounds();

	UFUNCTION(BlueprintCallable, Category = "Map Settings")
	void SetSpawnTargetBounds(float MinX, float MaxX, float MinY, float MaxY);


	//------------------------PLOTTABLE DIMENSION------------------------
	UFUNCTION(BlueprintCallable, Category = "Conversions")
	/** Provides conversion from PlottableDimension to FString using a unique human readable representation */
	static FString ToString(PlottableDimension::Type var);

	UFUNCTION(BlueprintCallable, Category = "Conversions")
	/** Provides conversion from human readable FString to PlottableDimension.
	 * 
	 * @param var FString representation to convert
	 * @output PlottableDimension output
	 * @return true if conversion successfull, false otherwise.
	 */
	static bool ToPlottableDimension(FString var, TEnumAsByte<PlottableDimension::Type>& output);

	UFUNCTION(BlueprintCallable, Category = "Conversions")
	/** Provides a list of all enabled (and tested) plottable dimensions */
	static TArray<TEnumAsByte<PlottableDimension::Type>> GetAvailablePlottableDimensions();

	UFUNCTION(BlueprintCallable, Category = "Conversions")
	/** Convert a FSongProperty element to its FString humar readable representation */
	static FString ElementToString(FSongDetails element);

	//---------------------------ACTIONS----------------------------

public:
	UFUNCTION(BlueprintCallable, Category = "Actions (Map)")
	/** Loads data and plot specified View */
	void Initialize(FString ViewID);

	UFUNCTION(BlueprintCallable, Category = "Actions (Map)")
	/** Add / Overwrite a dimension on the map.
	 *
	 * @param DimensionSpace HOW data dimension should be plotted on the map
	 * @param DimensionID WHAT dimension should be plotted on the map (identifier)
	 * @param UpdateNow if true triggers a map update
	 *
	 * This function may trigger a map update. */
	void SetDimension(PlottableDimension::Type DimensionSpace, FString DimensionID, bool UpdateNow = true);//WIP

	UFUNCTION(BlueprintCallable, Category = "Actions (Map)")
	/** For every dimension plotted, update dimension.
	 * 
	 * @param CurrentTime plot dimensions using value for currentTime.
	 * @param ElementID which element should be updated. Default = "" means all elements.
	 */
	void GlobalUpdateMap(float CurrentTime = 0.0f, FString ElementID = "");

	UFUNCTION(BlueprintCallable, Category = "Actions (Map)")
	/** For X, Y, Z dimension plotted, update dimension
	 *
	 * @param CurrentTime plot dimensions using value for currentTime.
	 * @param ElementID which element should be updated. Default = "" means all elements.
	 */
	void UpdateMapXYZ(float CurrentTime = 0.0f, FString ElementID = "");

	UFUNCTION(BlueprintCallable, Category = "Actions (Map)")
	/** Loads details and render on the map a PlottableDimension 
	 * 
	 * @param Dimension Which one of the rendered dimension should be updated.
	 *		If no dimension is associated to this PlottableDimension, a default NO_DIMENSION is used.
	 * @param CurrentTime plot dimensions using value for currentTime.
	 * @param ElementID which element should be updated. Default = "" means all elements.
	 */
	void UpdateSingleMapDimension(PlottableDimension::Type Dimension, float CurrentTime = 0.0f, FString ElementID = "");

	void ShowElements(TArray<FString> ElementIDs);

	void ApplySelectionToElement(FString ElementID);
	void RemoveSelectionFromElement(FString ElementID);

	//------------------------PRIVATE STUFF------------------------
private:	//EHI, Go away! I'm naked.

	bool TakeValue(FSongDetails elem, DimensionDetails dim, Caster* c, float& OutputValue, float time = 0.0f);

	/** Update Single Plottable Dimension: Color Hue. 
	 * 
	 * @param dim Dimension to plot in this PlottableDimension
	 * @param CurrentTime use value for this time if available. Default use summary value.
	 * @param ElementID update only ElementID. Default update all elements.
	 */
	inline void UpdateColorHue(const DimensionDetails dim, float CurrentTime = 0.0f, FString ElementID = "");

	/** Update Single Plottable Dimension: Color Sat.
	*
	* @param dim Dimension to plot in this PlottableDimension
	* @param CurrentTime use value for this time if available. Default use summary value.
	* @param ElementID update only ElementID. Default update all elements.
	*/
	inline void UpdateColorSat(const DimensionDetails dim, float CurrentTime = 0.0f, FString ElementID = "");
};
