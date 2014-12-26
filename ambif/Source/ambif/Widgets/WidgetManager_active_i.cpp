// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
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

void AWidgetManager_active_i::SetDimension(FString PlottableDimensionID, FString DimensionID, bool UpdateNow)
{
	TEnumAsByte<PlottableDimension::Type> tmp_plot_dim;
	if (APresentationLayer::ToPlottableDimension(PlottableDimensionID, tmp_plot_dim))
	{
		LogicController->SetDimension(tmp_plot_dim, DimensionID, UpdateNow);
	}
	else
	{//conversion failed
		DebugUtils::LogString("WidgetManager: Error converting string to PlottableDimension " + PlottableDimensionID);
		//TODO implement
	}
}

void AWidgetManager_active_i::SetDimensionByDescription(FString PlottableDimensionID, FString DimensionDescription, bool UpdateNow)
{
	//TODO implement check on ReverseMapping Availability
	if (Utils::FHMContains(dim_id_descr_map, DimensionDescription))
	{
		SetDimension(PlottableDimensionID, dim_id_descr_map.at(DimensionDescription), UpdateNow);
	}
	else	//ReverseMapping disabled
	{
		SetDimension(PlottableDimensionID, DimensionDescription, UpdateNow);
	}
}

void AWidgetManager_active_i::SetDimensionsByDescription(FString XDimensionDescription, FString YDimensionDescription, FString ZDimensionDescription, FString ColorHueDimensionDescription, FString ColorSatDimensionDescription)
{
	LogicController->SetDimension(PlottableDimension::X, (Utils::FHMContains(dim_id_descr_map, XDimensionDescription)) ? dim_id_descr_map.at(XDimensionDescription) : XDimensionDescription, false);
	LogicController->SetDimension(PlottableDimension::Y, (Utils::FHMContains(dim_id_descr_map, YDimensionDescription)) ? dim_id_descr_map.at(YDimensionDescription) : YDimensionDescription, false);
	LogicController->SetDimension(PlottableDimension::Z, (Utils::FHMContains(dim_id_descr_map, ZDimensionDescription)) ? dim_id_descr_map.at(ZDimensionDescription) : ZDimensionDescription, false);
	LogicController->SetDimension(PlottableDimension::Color_Hue, (Utils::FHMContains(dim_id_descr_map, ColorHueDimensionDescription)) ? dim_id_descr_map.at(ColorHueDimensionDescription) : ColorHueDimensionDescription, false);
	LogicController->SetDimension(PlottableDimension::Color_Sat, (Utils::FHMContains(dim_id_descr_map, ColorSatDimensionDescription)) ? dim_id_descr_map.at(ColorSatDimensionDescription) : ColorSatDimensionDescription, false);
	LogicController->UpdateDimensionsOnMap();
}

//-------------------------TESTING STUFF-------------------------
void AWidgetManager_active_i::TriggerSpawn()
{
	LogicController->ReadData();
	LogicController->RefreshViewList();
	LogicController->Spawn();
}

void AWidgetManager_active_i::UpdateWidgets()
{
	LogicController->UpdateWidgetInterface();
}