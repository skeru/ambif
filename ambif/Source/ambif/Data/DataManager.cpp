// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "stdexcept"
#include "CustomUtils/DebugUtils.h"
#include "DataManager.h"

//----------------------HARDCODED FILE NAMES----------------------
#define SongFileName "D:/_ambif_files_/SongMetadata.csv"
#define DimensionFileName "D:/_ambif_files_/DimensionList.csv"
#define ViewFileName "D:/_ambif_files_/ViewList.csv"
#define PropertiesFileName "D:/_ambif_files_/Properties.csv"

//#define DATA_MANAGER_VERBOSE

ADataManager::ADataManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SongMap = Utils::FHashMap<SongDetails>();
	DimensionMap = Utils::FHashMap<DimensionDetails>();
	ViewMap = Utils::FHashMap<ViewDetails>();
	CasterSet = Utils::FHashMap<Caster*>();
}

void ADataManager::LoadOldWayCSVData()
{
	int n_song, n_dim, n_view, n_prop;
	SongMap = Utils::FHashMap<SongDetails>();
	DimensionMap = Utils::FHashMap<DimensionDetails>();
	ViewMap = Utils::FHashMap<ViewDetails>();
	n_song = CSVParser::ReadPlainTextFile<SongDetails>(SongFileName, SongMap);
	n_dim = CSVParser::ReadPlainTextFile<DimensionDetails>(DimensionFileName, DimensionMap);
	n_view = CSVParser::ReadPlainTextFile<ViewDetails>(ViewFileName, ViewMap);
	n_prop = CSVParser::ReadPropertyDataCSV(PropertiesFileName, SongMap);
#ifdef DATA_MANAGER_VERBOSE
	DEBUG("Found " + FString::FromInt(n_song) + " songs in CSV");
	DEBUG("Found " + FString::FromInt(n_dim) + " dimensions in CSV");
	DEBUG("Found " + FString::FromInt(n_view) + " views in CSV");
	DEBUG("Found " + FString::FromInt(n_prop) + " properties in CSV");
#endif
}

void ADataManager::LogData()
{
	for (auto v : ViewMap)
	{
		DebugUtils::LogString(FString("------------------------------------"));
		DebugUtils::LogString(FString("View ID ") + v.first + FString(" :"));
		DebugUtils::LogString(v.second.XDimension + FString(" - ") + v.second.YDimension + FString(" - ") + v.second.ZDimension + FString(" - ") + v.second.HueDimension + FString(" - ") + v.second.SatDimension);
	}

	for (auto d : DimensionMap)
	{
		DebugUtils::LogString(FString("------------------------------------"));
		DebugUtils::LogString(FString("Dimension ID ") + d.first + FString(":"));
		DebugUtils::LogString(FString("Dimension Full Name: ") + d.second.FullName);
		DebugUtils::LogString(FString("Dimension Description: ") + d.second.Description);
		DebugUtils::LogString(FString("Dimension Range ") + FString::SanitizeFloat(d.second.LowerBound) + FString(" - ") + FString::SanitizeFloat(d.second.UpperBound));
		FString tmp;
		tmp = d.second.OrderAs + FString(" - ");
		if (d.second.ReplaceMissingValues)
		{
			tmp = tmp + FString("Replace Missing Values: YES - default value: ") + FString::SanitizeFloat(d.second.DefaultValue);
		}
		else
		{
			tmp = tmp + FString("Replace Missing Values: NO");
		}
		DebugUtils::LogString(tmp);
	}

	for (auto s : SongMap)
	{
		DebugUtils::LogString(FString("------------------------------------"));
		DebugUtils::LogString(FString("Element ID: ") + s.first + FString(":"));
		DebugUtils::LogString(FString("Element Name: ") + s.second.Name);
		DebugUtils::LogString(FString("Element file path: ") + s.second.Path);
		for (auto p : s.second.Properties)
		{
			DebugUtils::LogString(FString("Property ") + p.first + FString(" of value ") + p.second.SummaryValue);
		}
	}
}

//------------------------GETTERS------------------------
Utils::FHashMap<SongDetails>* ADataManager::GetElementMap()
{
	return &SongMap;
}

Utils::FHashMap<ViewDetails>* ADataManager::GetViewMap()
{
	return &ViewMap;
}

TArray<FString> ADataManager::GetDimensionIdList()
{
	return Utils::GetIdList(DimensionMap);
}

inline Caster* ADataManager::getCaster(const DimensionDetails& Dimension)
{
	Caster* ret;
	try
	{
		ret = CasterSet.at(Dimension.Id);
	}
	catch (const std::out_of_range do_not_worry)
	{
		ret = new Caster(ENormalize::Linear);
		ret->SetInputBounds(Dimension.LowerBound, Dimension.UpperBound);
		//TODO set input format
#ifdef DATA_MANAGER_VERBOSE
		DebugUtils::LogString(FString("Data Manager:: New caster instanciated. Bounds ") + FString::SanitizeFloat(Dimension.LowerBound) + FString(" - ") + FString::SanitizeFloat(Dimension.UpperBound));
#endif
		CasterSet[Dimension.Id] = ret;
	}
	return ret;
}

inline bool ADataManager::getDimensionDetails(FString DimensionID, DimensionDetails& output)
{
	try
	{
		output = DimensionMap.at(DimensionID);
		return true;
	}
	catch (const std::out_of_range do_not_care)
	{
		output.Id = DimensionID;
		DEBUG("Failed to load Dimension Details. ID not found " + DimensionID);
		return false;
	}
}

inline bool ADataManager::getViewDetails(FString ViewID, ViewDetails& output)
{
	try
	{
		output = ViewMap.at(ViewID);
		return true;
	}
	catch (const std::out_of_range do_not_care)
	{
		DEBUG("Failed to load View Details. ID not found " + ViewID);
		return false;
	}
}

inline bool ADataManager::getSongDetails(FString SongID, SongDetails& output)
{
	try
	{
		output = SongMap.at(SongID);
		return true;
	}
	catch (const std::out_of_range do_not_care)
	{
		DEBUG("Failed to load Song Details. ID not found " + SongID);
		return false;
	}
}