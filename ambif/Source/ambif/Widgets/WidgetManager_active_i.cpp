// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "WidgetManager_active_i.h"


AWidgetManager_active_i::AWidgetManager_active_i(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	dim_descr_buffer = Utils::FHashMap<int>();
	dim_id_descr_map = Utils::FHashMap<FString>();
}

//--------------------MUSIC PLAYER CONTROLS-------------------------
inline void AWidgetManager_active_i::Play()
{
	LogicController->MusicPlay();
}

inline void AWidgetManager_active_i::Pause()
{
	LogicController->MusicPause();
}

inline void AWidgetManager_active_i::Stop()
{
	LogicController->MusicStop();
}

//-------------------------DIMENSION SELECTION-------------------------
TArray<FString> AWidgetManager_active_i::GetDimensionIdList()
{
	return LogicController->GetDimensionList();
}

TArray<FString> AWidgetManager_active_i::GetPlottableDimensionIdList()
{
	return LogicController->GetPlottableDimensionList();
}

TArray<FString> AWidgetManager_active_i::GetDimensionDescriptionList(bool MakeDescriptionUnique)
{
	TArray<FString> id_list = GetDimensionIdList();
	TArray<FString> d_list = TArray<FString>();
	dim_descr_buffer.clear();
	dim_id_descr_map.clear();
	FString descr_tmp, descr_base;
	ReverseMappingAvailable = MakeDescriptionUnique;
	for (auto elem : id_list)
	{
		int counter = 1;
		descr_base = GetDimensionDescription(elem);
		descr_tmp = descr_base;
		if (MakeDescriptionUnique)//skip unique mapping if not required
		{
			if (Utils::FHMContains(dim_descr_buffer, descr_tmp))
			{
				counter = dim_descr_buffer[descr_tmp] + 1;
				do	//append counter to make unique the description
				{
					descr_tmp = descr_base;
					descr_tmp.AppendInt(counter);
					counter++;
				} while (Utils::FHMContains(dim_descr_buffer, descr_tmp));
			}
			dim_descr_buffer[descr_base] = counter;
			dim_id_descr_map[descr_tmp] = elem;
		}
		d_list.Add(descr_tmp);
	}
	return d_list;
}

FString AWidgetManager_active_i::GetDimensionDescription(FString DimensionID)
{
	return LogicController->GetDimensionDescription(DimensionID);
}

void AWidgetManager_active_i::SetDimension(FString PlottableDimensionID, FString DimensionID)
{
	//TODO implement
}

void AWidgetManager_active_i::SetDimensionByDescription(FString PlottableDimensionID, FString DimensionDescription)
{
	//TODO implement check on ReverseMapping Availability
	if (Utils::FHMContains(dim_id_descr_map, DimensionDescription))
	{
		SetDimension(PlottableDimensionID, dim_id_descr_map.at(DimensionDescription));
	}
	else	//ReverseMapping disabled
	{
		SetDimension(PlottableDimensionID, DimensionDescription);
	}
}