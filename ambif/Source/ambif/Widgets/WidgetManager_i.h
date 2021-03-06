//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "GameFramework/Actor.h"
#include "DataDefinitions/TableDefinitions.h"
#include "PresentationLayer.h"
/*
//UMG handling include block
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"
*/

#include "CustomUtils/DebugUtils.h"
#include "WidgetManager_i.generated.h"

/** Control passive widgets.
 * (Logic triggers widget update)
 *
 * C++ abstract class for Blueprint implemented functions.
 */
UCLASS()
class AMBIF_API AWidgetManager_i : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	AWidgetManager_i();	//initialize private queues and enable Tick

public:

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text Metadata")
	/** Widget component to show metadata
	 * 
	 * Use an asset here of type 'UUserWidget', 'BlueprintWidget'.
	 * NOTE removed because cannot enforce BP type
	 */
//	UBlueprint* MetadataWid;

//	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dimension Selection")
	/** Widget component that controls dimension selection
	 *
	 * Use an asset here of type 'UUserWidget', 'BlueprintWidget'.
	 * NOTE removed because cannot enforce BP type
	 */
//	UBlueprint* DimensionMenuWidget;


public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Metadata")
	/** Update widget text
	 */
	void UpdateMetadataWid(const FString& metadata);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Music Player")
	/** Update Music Player widget buttons
	 */
	void UpdateMusicPlayerWidget(bool IsPlaying, bool IsPaused);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Dimension")
	/** Update widget text options
	 */
	void UpdateDimensionWidegt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Dimension")
	/** Update selectable dimensions 
	 * -directly access widget-
	 * do not perform any kind of mapping / translation.
	 *
	 * should be called by the dimenison widget update
	 *
	 * @see AWidgetManager_i::UpdateDimensionWidegt()
	 */
	void UpdateDimensionList(const TArray<FString>& OptionsList);

	UFUNCTION(BlueprintCallable, Category = "Update Dimension")
	/** Load to dimension menu options coherent with specified view.
	 * 
	 * WARNING: enforce only menu options, not dimension on map. 
	 */
	void ApplyView(FViewDetails view);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Dimension")
	/** Force Selection to Widget Menu
	 */
	void SetSelectedDimension(PlottableDimension::Type dimension, const FString& DimensionID);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Zoom")
	/** Force zoom to NewZoomPercentage.
	 * Updates widget slider and triggers a zoom update. */
	void ForceUpdateZoomWidget(float NewZoomPercentage);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Update Zoom")
	/** Updates widget buttons. */
	void ForceUpdateParabolicLinear();

	//------------------------TICK stuff------------------------
public:
	//tick needed to apply deferred updates
	UFUNCTION()
	void Tick(float DeltaTime) override;

	//-------------------deferred updates stuff-------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Deferred Updates")
	/** Schedule a ApplyView call to DeferBy number of ticks. 
	 *
	 * Queue is reset at first element applied to avoid too many requests.
	 *
	 * @see ApplyView(FViewDetails)
	 */
	void EnqueueApplyView(FViewDetails Details, int32 DeferBy = 2);

private:
	TArray<std::pair<int32, FViewDetails>> ApplyViewQueue;

};
