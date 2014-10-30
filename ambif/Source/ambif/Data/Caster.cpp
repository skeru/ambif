// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "stdexcept"
#include "Caster.h"

#define EPSILON 0.0000001f

//-------------------------NORMALIZER FUNCTIONS-------------------------

#define RATE FMath::Clamp((Value - InMin) / (InMax - InMin), 0.0f, 1.0f)
#define TARGETRANGE (TargetMax - TargetMin) 

float LinearNormalizer(float Value, float InMin, float InMax, float TargetMin, float TargetMax)
{
	return TargetMin + (RATE * TARGETRANGE);
}

float LogNormalizer(float Value, float InMin, float InMax, float TargetMin, float TargetMax)
{
	float rate = RATE + 1.0f; //range [1;2]
	return TargetMin + (FMath::LogX(2.0f, rate) * TARGETRANGE);
}

float LinearReverseNormalizer(float Value, float InMin, float InMax, float TargetMin, float TargetMax)
{
	return TargetMax - (RATE * TARGETRANGE);
}

float LogReverseNormalizer(float Value, float InMin, float InMax, float TargetMin, float TargetMax)
{
	float rate = RATE + 1.0f; //range [1;2]
	return TargetMax - (FMath::LogX(2.0f, rate) * TARGETRANGE);
}

#undef RATE
#undef TARGETRANGE

//-------------------------VARIOUS SETTERS-------------------------

void Caster::SetInputBounds(float Min = 0.0f, float Max = 1.0f)
{
	InputMinBound = Min;
	InputMaxBound = Max;
	if (Max == Min)
	{
		Max += EPSILON;
	}
}

void Caster::SetNormalizationType(const ENormalize::Type newType)
{
	NormalizationType = newType;
	switch (newType){
	case ENormalize::Linear:
		Caster::normalizer = LinearNormalizer;
		break;
	case ENormalize::Log:
		Caster::normalizer = LogNormalizer;
		break;
	case ENormalize::LinearReverse:
		Caster::normalizer = LinearReverseNormalizer;
		break;
	case ENormalize::LogReverse:
		Caster::normalizer = LogReverseNormalizer;
		break;
	default:
		Caster::normalizer = LinearNormalizer;
		break;
	}
}

void Caster::SetTargetBounds(float TargetMin, float TargetMax)
{
	TargetMinBound = TargetMin;
	TargetMaxBound = TargetMax;
}

//------------------------ CONSTRUCTOR ------------------------

Caster::Caster(ENormalize::Type NormalizeAs, float TargetMin, float TargetMax)
{
	SetTargetBounds(TargetMin, TargetMax);
	SetInputBounds();
	SetNormalizationType(NormalizeAs);
	SetInputFormat(EInputFormat::Number);
}

//---------------------- CASTING METHODS ----------------------

inline float Caster::CastFloat(const float input)
{
	return normalizer(input, InputMinBound, InputMaxBound, TargetMinBound, TargetMaxBound);
}

inline bool Caster::Cast(FString str, float& output)
{
	return (this->*_Cast)(str, output);	//chosen basing on InputFormat
}

template<>
bool Caster::CastValue<float>(FString str, float & output)
{
	float fval;
	bool conv_ok = Utils::FStrToF(str, fval);
	if (conv_ok)
	{
		output = CastFloat(fval);
	}
	return conv_ok;
}

template <>
bool Caster::CastValue<Genre::Type>(FString str, float & output)
{
	float val;
	try{
		val = MusicGenreAdapter::GetInstance()->StringToFloat(str);
		val = LinearNormalizer(val, GenreNodes::MIN_VALUE, GenreNodes::MAX_VALUE, Caster::InputMinBound, Caster::InputMaxBound);
		val = CastFloat(val); //normalize on declared output
	}
	catch (std::out_of_range shit_happens){
		return false;
	}
	output = val;
	return true;
}

template <>
bool Caster::CastValue<FString>(FString str, float & output)
{
	switch (InputFormat)
	{
	case EInputFormat::Genres:
		return CastValue<Genre::Type>(str, output);
		break;
	}
	DebugUtils::LogString(FString("Caster: Unimplemented method CastValue<FString> called. This is a quite bad thing, it shouldn't happen!"));
	return false;
}


//-----this should stay as last 'cause uses specialization templates
void Caster::SetInputFormat(EInputFormat::Type Format)
{
	InputFormat = Format;

	switch (Format)
	{
	case EInputFormat::Number:
		_Cast = &Caster::CastValue < float >;
		break;
	case EInputFormat::Genres:
		_Cast = &Caster::CastValue < Genre::Type >;
		break;
	default:
		_Cast = &Caster::CastValue < FString >;		//uninplemented
		break;
	}
}