// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "Music/MusicPlayerActor.h"
#include "Data/DataManager.h"
#include "PresentationLayer.h"
#include "Widgets/WidgetManager_i.h"
#include "HUD/InspectionHUD.h"
#include "LogicController.generated.h"

/**
 * This class holds actor references and the logic of the program.
 *
 * I know this should be the goal of GameMode and PlayerController
 * but they have too weird constraints to be understood easily.
 * so i built up this custom actor to manage all of this in one
 * easy-to-access actor.
 *
 * This actor should be referenced by user interface classes
 * (user interface classes could be HUD or widgets or something else)
 */
UCLASS()
class AMBIF_API ALogicController : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	ALogicController(const FObjectInitializer& ObjectInitializer);

	//-------------------ACTOR REFERENCES-------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MusicPlayer)
	/** Music Player Actor */
	AMusicPlayerActor* MusicPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	/** Data layer Manager */
	ADataManager* DataAgent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SubManagers)
	/** Presentation Layer */
	APresentationLayer* PresentationLayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = SubManagers)
	/** Map Elements Manager (notifications) */
	AMapElementsManager* MapElementsAgent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Graphics)
	/** Interface to update widget user interface */
	AWidgetManager_i* WidgetManager;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Graphics)
	/** HUD used for tooltips */
	AInspectionHUD* TooltipHUD;
	
	//-----------------------BUFFER------------------------
private:
	Utils::FHashMap<ViewDetails> ViewList_cache;
	Utils::FHashMap<DimensionDetails> DimensionList_cache;

	//-------------------ELEMENTS BUFFERS-------------------
private:
	/** Contains IDs of Selected Elements on the map (mouse hover) */
	TSet<FString> CurrentlySelectedElements;

	/** Contains IDs of Active Elements on the map (mouse click) */
	TSet<FString> CurrentlyClickedElements;

	FString LastClickedElementID;

	//---------------MUSIC PLAYER FUNCTIONS---------------
public:
	UFUNCTION(BlueprintCallable, Category = "Music Player")
	/** Load Element in buffer */
	void LoadElement(FString ElementID);

	UFUNCTION(BlueprintCallable, Category = "Music Player")
	/** Play Song */
	void MusicPlay();

	UFUNCTION(BlueprintCallable, Category = "Music Player")
	/** Pause song */
	void MusicPause();

	UFUNCTION(BlueprintCallable, Category = "Music Player")
	/** Stop song */
	void MusicStop();

	//------------------SPAWN FUNCTIONS------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Spawner")
	/** Spawn an element in a random position */
	void Spawn();

	//------------------DATA FUNCTIONS-------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Read imported CSV tables */
	void ReadData();

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Refresh view cache (does not open CSV data) */
	void RefreshViewList();

#define NO_DIMENSION_FOUND ""

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Get a list of all available dimension's identifiers */
	TArray<FString> GetDimensionList();

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Retrieve description from ID 
	 *
	 * Empty FString is returned when no description can be found */
	FString GetDimensionDescription(FString DimensionId);

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Get a list of all available plottable dimensions (human readable format) */
	static TArray<FString> GetPlottableDimensionList();

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Proxy for PresentationLayer::SetDimension */
	void SetDimension(PlottableDimension::Type DimensionSpace, FString DimensionID, bool UpdateNow = true);//testing

	UFUNCTION(BlueprintCallable, Category = "Data functions")
	/** Triggers a global map elements update.
	 * 
	 * @see PresentationLayer::GlobalUpdateMap() */
	void UpdateDimensionsOnMap();

	//-------------------MAP ACTIONS--------------------
public:
	/** Select element from map */
	UFUNCTION(BlueprintCallable, Category = "Map Actions")
	void SelectElement(FString ElementID);

	UFUNCTION(BlueprintCallable, Category = "Map Actions")
	/** Deselect element from map */
	void DeselectElement(FString ElementID);

	//------------------------GUI-------------------------
	UFUNCTION(BlueprintCallable, Category = "GUI")
	/** Triggers an update of interface widgets.
	 *
	 * Actually updates:
	 *  - Dimension Selection Widget
	 */
	void UpdateWidgetInterface();

	/** Force zoom to NewZoomPercentage.
	 * Updates widget slider and triggers a zoom update. 
	 * 
	 * Proxy for WidgetManager_i::ForceUpdateZoomWidget
	 *
	 * @see AWidgetManager_i::ForceUpdateZoomWidget(float)
	 */
	void ForceUpdateZoomWidget(float NewZoomPercentage);

	//----------------------TICK-----------------------
public:
	UFUNCTION()
	void Tick(float DeltaTime) override;

	//private tick stuff
private:

	void DetectSelectedActors();

	void DetectClickedActors();

	inline void HandleElement();

	//*time from last element update
	double DeltaTimeElementUpdate;

	//*update element every ElmentUpdateGranularity seconds.
	const double ElmentUpdateGranularity = 1.0f;

	//--------------------PRIVATE STUFF--------------------
private:
	/** Retrieve track artist and title given its id */
	FString inline ReadMetadata(FString elementID);
	
	void ShowToolTip(FString Message);
};
