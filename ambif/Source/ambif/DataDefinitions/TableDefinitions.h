// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "CustomUtils/CustomUtils.h"
#include "TableDefinitions.generated.h"

//default dimension for no dimension to represent
#define NO_DIMENSION FString("void")

USTRUCT(Blueprintable)
struct FSongProperty
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Property Value (Summary) */
		FString SummaryValue;

	//UPROPERTY(BlueprintReadOnly, Category = "Details")
	/** Property Segment Values <start segment time, value> */
	TMap<unsigned, FString> SegmentValues;

	inline struct FSongProperty& operator<<(const TArray<FString> serialized_data)
	{
		SegmentValues = TMap<unsigned, FString>();
		if (serialized_data.Num() > 0)
		{
			SummaryValue = serialized_data[0];
			size_t SegmentCounter;
			SegmentCounter = (serialized_data.Num() - 1) / 2;
			for (int i = 0, offset = 1; i < SegmentCounter; i++)
			{
				unsigned tmp;
				if (Utils::FStrToU(serialized_data[offset + (2 * i)], tmp))
				{
					SegmentValues[tmp] = serialized_data[offset + (2 * i) + 1];
				}
			}
		}
		else
		{
			SummaryValue = FString("");
			//			DebugUtils::LogString("Error loading value");
		}
		return *this;
	}

	inline FString operator[](unsigned int time)
	{
		if (SegmentValues.Num() > 0)
		{
			int a, b, middle, old_min;
			TArray<unsigned> keys;
			a = 0;
			b = SegmentValues.GetKeys(keys);
			keys.Sort();
			//binary search
			old_min = -1;
			while (a <= b)
			{
				middle = (a + b) / 2;
				if (keys[middle] <= time)
				{
					a = middle + 1;
					old_min = middle;
				}
				else
				{
					b = middle - 1;
				}
			}

			if (old_min != -1)
			{
				return SegmentValues[keys[old_min]];
			}
		}
		return SummaryValue;
	}
};

USTRUCT(Blueprintable)
struct FSongDetails //TODO promote to class & create interface via abstract class
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Song Identifier */
		FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Song Name, Public Identifier */
		FString Name;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Where song file is stored */
		FString Path;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Artist */
		FString Artist;

	//UPROPERTY(BlueprintReadOnly, Category = "Details")
	/** Length */
	unsigned Length;

	/** 'Array' of song available properties.
	*
	* Values can be accessed via operator[FString propertyID] */
	Utils::FMap<FSongProperty> Properties;	//check out TMap

	inline struct FSongDetails& operator<<(const TArray<FString> serialized_data)
	{
		Properties = Utils::FMap<FSongProperty>();
		//Properties = TArray<FSongProperty>();
		if (serialized_data.Num() < 5)
		{
			Id = FString("");
			Name = FString("Invalid value");
			Path = FString("");
			Artist = FString("Invalid value");
			Length = 0;
		}
		else
		{
			Id = serialized_data[0];
			Name = serialized_data[1];
			Path = serialized_data[2];
			Artist = serialized_data[3];
			if (!Utils::FStrToU(serialized_data[4], Length))
			{
				Length = 0;
			}


			//-------propery loading suspended waiting for refactoring------------

			//columns except Name, Path. Half is property Ids, Half is property values
			/*			size_t numberOfProperties = (serialized_data.Num() - 2) / 2;
			for (size_t i = 0, offset = 3; i < numberOfProperties; i++)
			{
			FSongProperty prop;
			prop.Id = serialized_data[2 * i + offset];
			prop.Value = serialized_data[2 * i + offset + 1];
			Properties[prop.Id] = prop; //Properties.Add(prop);
			}*/
		}
		return *this;
	}

	inline FString operator[](const FString key)
	{
		return Properties.at(key).SummaryValue;//out_of_range exception when key does not match
	}
};

USTRUCT(Blueprintable)
struct FDimensionDetails //TODO promote to class & create interface via abstract class
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension Identifier */
		FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Display Name */
		FString FullName;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Description */
		FString Description;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Lowest value this dimension could assume */
		float LowerBound;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Highest value this dimension could assume */
		float UpperBound;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Numbers, Nominal values, Genre classification, ... */
		FString OrderAs;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Should default value be used */
		bool ReplaceMissingValues;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Default Value */
		float DefaultValue;

	inline struct FDimensionDetails& operator<<(const TArray<FString> serialized_data)
	{
		if (serialized_data.Num() < 7)
		{
			Id = FString("");
			FullName = FString("Invalid Entry");
			Description = FString("");
			LowerBound = 0.0f;
			UpperBound = 0.0f;
			OrderAs = FString("Numbers");
			ReplaceMissingValues = false;
			DefaultValue = 0.0f;
		}
		else
		{
			Id = serialized_data[0];
			FullName = serialized_data[1];
			Description = serialized_data[2];
			LowerBound = FCString::Atof(*serialized_data[3]);
			UpperBound = FCString::Atof(*serialized_data[4]);
			OrderAs = serialized_data[5];
			ReplaceMissingValues = serialized_data[6].ToBool();;
			DefaultValue = (ReplaceMissingValues && serialized_data.Num() > 7) ? FCString::Atof(*serialized_data[7]) : 0.0f;
		}
		return *this;
	}
};

USTRUCT(Blueprintable)
struct FViewDetails //TODO promote to class & create interface via abstract class
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** View Identifier */
		FString Id;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension to show among X */
		FString XDimension;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension to show among Y */
		FString YDimension;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension to show among Z */
		FString ZDimension;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension to show as element color */
		FString HueDimension;

	UPROPERTY(BlueprintReadOnly, Category = "Details")
		/** Dimension to show as element color saturation*/
		FString SatDimension;

	inline struct FViewDetails& operator<<(const TArray<FString> serialized_data)
	{
		if (serialized_data.Num() < 3)
		{
			Id = FString("");
			XDimension = NO_DIMENSION;
			YDimension = NO_DIMENSION;
			ZDimension = NO_DIMENSION;
			HueDimension = NO_DIMENSION;
			SatDimension = NO_DIMENSION;
		}
		else
		{
			Id = serialized_data[0];
			XDimension = serialized_data[1];
			YDimension = serialized_data[2];

			if (serialized_data.Num() < 6)
			{
				ZDimension = NO_DIMENSION;
				HueDimension = NO_DIMENSION;
				SatDimension = NO_DIMENSION;
			}
			else
			{
				ZDimension = serialized_data[3];
				HueDimension = serialized_data[4];
				SatDimension = serialized_data[5];
			}
		}
		return *this;
	}

};

typedef struct FSongDetails SongDetails;
typedef struct FDimensionDetails DimensionDetails;
typedef struct FViewDetails ViewDetails;