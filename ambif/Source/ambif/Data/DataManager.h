// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "GameFramework/Actor.h"
#include "CustomUtils/CustomUtils.h"
#include "Caster.h"
#include "DataDefinitions/TableDefinitions.h"
#include "CSVParser.h"
#include "DataManager.generated.h"

/**
 * Base actor class to access data
 */
UCLASS()
class AMBIF_API ADataManager : public AActor
{
	GENERATED_BODY()
public:
	//constructor
	ADataManager(const FObjectInitializer& ObjectInitializer);

	//---------------------DATA STRUCTURES---------------------
protected:

	Utils::FHashMap<SongDetails> SongMap;
	Utils::FHashMap<DimensionDetails> DimensionMap;
	Utils::FHashMap<ViewDetails> ViewMap;

private:
	Utils::FHashMap<Caster*> CasterSet;

public:
	//-----------------DATA ACCESS FUNCTIONS-----------------

	/** Load data to memory - initialization */
	void LoadOldWayCSVData();
	/** Log data */
	void LogData();

	//------------------------GETTERS------------------------
public:
	Utils::FHashMap<SongDetails>* GetElementMap();
	Utils::FHashMap<ViewDetails>* GetViewMap();
	
	TArray<FString> GetDimensionIdList();

public:
	inline Caster* getCaster(const DimensionDetails& DimendionId);

	/** Extract structur from id.
	 *
	 * Returns true if success, false if failed.
	 */
	inline bool getDimensionDetails(FString DimensionID, DimensionDetails& output);

	/** Extract structur from id.
	 *
	 * Returns true if success, false if failed.
	 */
	inline bool getViewDetails(FString ViewID, ViewDetails& output);

	/** Extract structur from id.
	 *
	 * Returns true if success, false if failed.
	 */
	inline bool getElementDetails(FString ElementID, SongDetails& output);
};
