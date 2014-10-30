// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "DataDefinitions/Genre.h"
#include "DataDefinitions/MusicGenreAdapter.h"
#include "Caster.generated.h"

UENUM(BlueprintType)
namespace ENormalize{
	enum Type
	{
		Linear	UMETA(DisplayName = "Linear"),		//works fine
		Log		UMETA(DisplayName = "Logaritmic"),	//to be tested
		LinearReverse	UMETA(DisplayName = "Linear (Reverse)"),	//to be tested
		LogReverse		UMETA(DisplayName = "Logaritmic (Reverse)")	//to be tested
	};
}

UENUM(BlueprintType)
namespace EInputFormat{
	enum Type
	{
		Number			UMETA(DisplayName = "Number"),		//works fine
		Genres			UMETA(DisplayName = "Genre Space")	//to be tested
	};
}

/**
 * Stetefull object used to cast from an input space
 * (default numerical [0;1]) to a target space with
 * various patterns of normalization.
 *
 * Input can be:
 * - Number (float)
 * - Genre (Genre::Type)
 *
 * Output is always a float number
 * normalized on a given range (default [0;1])
 *
 * - Linear
 * - Linear (inverse)
 * - Logaritmic
 * - Logaritmic (inverse)
 *
 * @see ENormalize::Type
 * @see EInputFormat::Type
 * @see Genre::Type
 */
class AMBIF_API Caster
{
protected:
	float TargetMinBound;
	float TargetMaxBound;

	float InputMinBound;
	float InputMaxBound;
	TEnumAsByte<ENormalize::Type> NormalizationType;
	TEnumAsByte<EInputFormat::Type> InputFormat;
	float(*normalizer)(float Value, float InMin, float InMax, float TargetMin, float TargetMax);
	bool(Caster::* _Cast)(FString str, float& output);

public:
	Caster(ENormalize::Type NormalizeAs, float TargetMin = 0.0f, float TargetMax = 1.0f);

	inline void SetInputBounds(float Min, float Max);
	inline void SetTargetBounds(float TargetMin, float TargetMax);
	inline void SetNormalizationType(const ENormalize::Type newType);
	inline void SetInputFormat(EInputFormat::Type Format);

	inline bool Cast(FString str, float& output);
	inline float CastFloat(const float input);

	/** Cast FString to a float value
	*
	* @param str input string
	* @param output casted value
	* @return true if conversion successfull, false otherwise.
	*/
	template<typename t> bool CastValue(FString str, float & output)
	{
		t element;
		DebugUtils::LogString(FString("Caster: Unimplemented method CastValue called."));
		output = 0.0f;
		return false;
	}

};

extern template bool Caster::CastValue<float>(FString, float&);
extern template bool Caster::CastValue<Genre::Type>(FString, float&);
extern template bool Caster::CastValue<FString>(FString, float&);