// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "Music/MusicPlayerActor.h"
#include "Data/DataManager.h"
#include "PresentationLayer.h"
#include "LogicController.generated.h"

/**
 * This class holds actor references and the logic of the program.
 *
 * I know this should be the goal of GameMode and PlayerController
 * but they have too weird constraints to be understood easily.
 * so i built up this custom actor to manage all of this in one
 * easy-to-access actor.
 *
 * This actor should be referenced by the HUD class
 */
UCLASS()
class AMBIF_API ALogicController : public AActor
{
	GENERATED_UCLASS_BODY()

		//------------------ACTOR REFERENCES------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MusicPlayer)
	/** Music Player Actor */
	AMusicPlayerActor* MusicPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
	/** Data layer Manager */
	ADataManager* DataAgent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TestingStuff)
	/** Presentation Layer */
	APresentationLayer* PresentationLayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TestingStuff)
	/** Map Elements Manager (notifications) */
	AMapElementsManager* MapElementsAgent;

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

	//---------------MUSIC PLAYER FUNCTIONS---------------
public:
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

	//-------------------MAP ACTIONS--------------------
public:
	UFUNCTION(BlueprintCallable, Category = "Map Actions")
	/** Select element from map */
	void SelectElement(FString ElementID);

	/** Deselect element from map */
	UFUNCTION(BlueprintCallable, Category = "Map Actions")
	void DeselectElement(FString ElementID);

	//----------------------TICK-----------------------
public:
	UFUNCTION()
	void Tick(float DeltaTime) override;

	//private tick stuff
private:
	void DetectSelectedActors();

	void DetectClickedActors();
};