// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#pragma once

#include "DataDefinitions/Genre.h"
#include "DataDefinitions/MusicGenreAdapter.h"
#include "Caster.generated.h"

UENUM(BlueprintType)
/**
 * Available normalization strategies
 */
namespace ENormalize{
	enum Type
	{
		Linear	UMETA(DisplayName = "Linear"),		//works fine
		Log		UMETA(DisplayName = "Logaritmic"),	//works fine
		LinearReverse	UMETA(DisplayName = "Linear (Reverse)"),	//to be tested
		LogReverse		UMETA(DisplayName = "Logaritmic (Reverse)")	//to be tested
	};
}

UENUM(BlueprintType)
/**
 * Supported input formats
 */
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
	/**
	 * Create a new Caster object
	 *
	 * @param NormalizeAs Normalization strategy
	 * @param TargetMin Lower bound of target space [default = 0.0f]
	 * @param TargetMax Upper bound of target space [default = 1.0f]
	 * 
	 * @see ENormalize::Type
	 */
	Caster(ENormalize::Type NormalizeAs, float TargetMin = 0.0f, float TargetMax = 1.0f);

	/**
	 * Set expected input bounds.
	 * Every input value lower than Min will be clamped to Min.
	 * Every input value higher than Max will be clamped to Max.
	 * 
	 * @param Min Minimun input space expected value.
	 * @param Max Maximum input space expected value.
	 */
	inline void SetInputBounds(float Min, float Max);
	
	/**
	 * Set expected target bounds.
	 *
	 * @param Min Minimun target space expected value.
	 * @param Max Maximum target space expected value.
	 */
	inline void SetTargetBounds(float TargetMin, float TargetMax);
	
	/**
	 * Set normalization strategy.
	 *
	 * @param newType new normalization strategy
	 *
	 * @see ENormalize::Type
	 */
	inline void SetNormalizationType(const ENormalize::Type newType);
	
	/**
	 * Set expected input format.
	 * Basing on this format, every input string will be evalueted in a different way.
	 *
	 * @param Format Expected input string format.
	 *
	 * @see EInputFormat::Type
	 */
	inline void SetInputFormat(EInputFormat::Type Format);

	/**
	 * Perform a cast from input space to target space.
	 * Automatically calls the correct internal function to perform this operation.
	 *
	 * @param str input string
	 * @param output casted output value
	 * 
	 * @return true if cast was successfully performed, false otherwise.
	 */
	inline bool Cast(FString str, float& output);

	/**
	 * Quick cast from float to float.
	 * 
	 * @deprecated use #Cast(FString, float&) instead.
	 */
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

//------------------------KNOWN TEMPLATE SPECIFICATIONS------------------------

extern template bool Caster::CastValue<float>(FString, float&);
extern template bool Caster::CastValue<Genre::Type>(FString, float&);
extern template bool Caster::CastValue<FString>(FString, float&);