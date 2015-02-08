// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "LogicController.h"
#include "CustomUtils/CustomUtils.h"
#include "WidgetManager_active_i.generated.h"

/** Handle active GUI widgets events
 * 
 * Intercept GUI events and translate them to Logic Controller function calls.
 *
 * Most of the functions here declared are implemented in blueprint.
 */
UCLASS()
class AMBIF_API AWidgetManager_active_i : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	AWidgetManager_active_i(const FObjectInitializer& ObjectInitializer);

	//-------------------------LOGIC CONTROLLER-------------------------
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Controller")
	/** Low level request handler */
	ALogicController* LogicController;
	
	//------------------------------- API -------------------------------
public:
	//--------------------MUSIC PLAYER CONTROLS-------------------------
	UFUNCTION(BlueprintCallable, Category = "Music Player Controls")
	/** Play currently selected song */
	void Play();

	UFUNCTION(BlueprintCallable, Category = "Music Player Controls")
	/** Pause currently playing song */
	void Pause();

	UFUNCTION(BlueprintCallable, Category = "Music Player Controls")
	/** Stop currently playing song */
	void Stop();


	//-------------------------DIMENSION SELECTION-------------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Retrieve a list of available dimensions (ID list) */
	TArray<FString> GetDimensionIdList();

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Retrieve a list of plottable dimensions */
	TArray<FString> GetPlottableDimensionIdList();

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Retrieve a list of available dimensions (Description list) 
	 *
	 * Also creates a reverse mapping between description and id 
	 * so Id can later be retried by its description. 
	 * This is a little consuming. You can disable it if it's not required.
	 *
	 * @param MakeDescriptionUnique enable Reverse Mapping. Disabling it will disable Reverse Mapping.
	 */
	TArray<FString> GetDimensionDescriptionList(bool MakeDescriptionUnique = true);

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Retrieve a dimension description given its ID */
	FString GetDimensionDescription(FString DimensionID);

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Set a new DimensionID to a PlottableDimension */
	void SetDimension(FString PlottableDimensionID, FString DimensionID, bool UpdateNow = true);

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Set a new Dimension to a PlottableDimension using Dimension Description instead of Dimension ID
	 * Available only if ReverseMapping was enable by making the description unique
	 *
	 * @see GetDimensionDescriptionList(bool)
	 */
	void SetDimensionByDescription(FString PlottableDimensionID, FString DimensionDescription, bool UpdateNow = true);

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Set a new set of Dimensions as PlottableDimensions using Dimension Description
	 *
	 * @param XDimensionDescription description of the dimension to set for PlottableDimension::X
	 * @param YDimensionDescription description of the dimension to set for PlottableDimension::Y
	 * @param ZDimensionDescription description of the dimension to set for PlottableDimension::Z
	 * @param ColorHueDimensionDescription description of the dimension to set for PlottableDimension::Color_Hue
	 * @param ColorSatDimensionDescription description of the dimension to set for PlottableDimension::Color_Sat
	 *
	 * @see SetDimensionByDescription(FString, FString, bool)
	 */
	 void SetDimensionsByDescription(
			FString XDimensionDescription,
			FString YDimensionDescription,
			FString ZDimensionDescription,
			FString ColorHueDimensionDescription,
			FString ColorSatDimensionDescription);

	UFUNCTION(BlueprintCallable, Category = "Dimension Selection")
	/** Looks if is available a mapping between given ID and on screen description:
	 * if there is, returns it. Otherwise returns standard description. 
	 */
	FString GetOnScreenDimensionDescription(FString DimensionID);

	//-------------------------TESTING STUFF-------------------------
	UFUNCTION(BlueprintCallable, Category = "Testing")
	/** Trigger a data read & update than spawn objects */
	void TriggerSpawn();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	/** Trigger a manual widget interface refresh */
	void UpdateWidgets();

	UFUNCTION(BlueprintCallable, Category = "Testing")
	/** Switch between 2D / 3D mode */
	void SetEnable3DMode(bool Enable3D);

	//-------------------------PRIVATE STUFF-------------------------
private:
	/** pair <Description - counter> to avoid duplicated names */
	Utils::FHashMap<int> dim_descr_buffer;

	/** pair <shown unique description - id> for reverse conversion */
	Utils::FHashMap<FString> dim_id_descr_map;

	/** true when selectable options (descriptions) were made unique 
	 * before being set in widget. False otherwise. */
	bool ReverseMappingAvailable;
};
