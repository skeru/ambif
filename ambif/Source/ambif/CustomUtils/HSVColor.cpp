//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#include "ambif.h"
#include "HSVColor.h"

HSVColor::HSVColor()
{
	H = V = S = R = G = B = 0; //black
}

HSVColor::~HSVColor()
{
}

HSVColor::HSVColor(unsigned int Hue, unsigned int Saturation, unsigned int Value)
{
	H = FRotator::ClampAxis((float)Hue);								//[0 - 360)
	S = FMath::Clamp(Saturation, (unsigned int)0, (unsigned int)100);	//[0 - 100]
	S = S / 100.0f;														//[0 - 1]
	V = FMath::Clamp(Value, (unsigned int)0, (unsigned int)100);		//[0 - 100]
	V = V / 100.0f;														//[0 - 1]
	HSV_to_RGB(H, S, V, R, G, B);
}

HSVColor::HSVColor(float Hue, float Saturation, float Value)
{
	H = FRotator::ClampAxis(Hue);										//[0 - 360)
	S = FMath::Clamp(Saturation, 0.0f, 1.0f);							//[0 - 1]
	V = FMath::Clamp(Value, 0.0f, 1.0f);								//[0 - 1]
	HSV_to_RGB(H, S, V, R, G, B);
}

HSVColor::HSVColor(FLinearColor rgb_color)
{
	R = rgb_color.R;
	G = rgb_color.G;
	B = rgb_color.B;

	RGB_to_HSV(R, G, B, H, S, V);
}

inline void HSVColor::SetH(unsigned int Hue)
{
	H = FRotator::ClampAxis(Hue);
	HSV_to_RGB(H, S, V, R, G, B);
}

inline void HSVColor::SetS(unsigned int Saturation)
{
	S = FMath::Clamp(Saturation, (unsigned int)0, (unsigned int)100);
	S /= 100.0f;
	HSV_to_RGB(H, S, V, R, G, B);
}

inline void HSVColor::SetV(unsigned int Value)
{
	V = FMath::Clamp(Value, (unsigned int)0, (unsigned int)100);
	V /= 100.0f;
	HSV_to_RGB(H, S, V, R, G, B);
}

inline void HSVColor::SetH(float Hue)
{
	H = FRotator::ClampAxis(Hue);
	HSV_to_RGB(H, S, V, R, G, B);
}

inline void HSVColor::SetS(float Saturation)
{
	S = FMath::Clamp(Saturation, 0.0f, 1.0f);
	HSV_to_RGB(H, S, V, R, G, B);
}

inline void HSVColor::SetV(float Value)
{
	V = FMath::Clamp(Value, 0.0f, 1.0f);
	HSV_to_RGB(H, S, V, R, G, B);
}

FLinearColor HSVColor::ToFLinearColor()
{
	return FLinearColor(R, G, B);
}

//-------------------------------STATIC FUNCTIONS-------------------------------

FLinearColor HSVColor::ToLinearColor(float H, float S, float V)
{
	float R, G, B;
	HSV_to_RGB(H, S, V, R, G, B);
	return FLinearColor(R, G, B);
}

FLinearColor HSVColor::ToLinearColor(unsigned int H, unsigned int S, unsigned int V)
{
	unsigned int R, G, B;
	HSV_to_RGB(H, S, V, R, G, B);
	return FLinearColor(R, G, B);
}

void HSVColor::HSV_to_RGB(float H, float S, float V, float&R, float&G, float&B)
{
	//conversion formula from http://www.rapidtables.com/convert/color/hsv-to-rgb.htm

	float const _H = FRotator::ClampAxis(H);								//[0 - 360)
	float const _S = FMath::Clamp(S, 0.0f, 1.0f);							//[0 - 1]
	float const _V = FMath::Clamp(V, 0.0f, 1.0f);							//[0 - 1]

	float const C = _V * _S;
	float const h_over_60 = _H / 60.0f;
	float const X = C * (1 - FMath::Abs(FMath::Fmod(h_over_60, 2) - 1));
	float const m = _V - C;

	R = G = B = m;

	switch ((int)h_over_60)
	{
	case 0:		//   0° <= H <  60°
		R += C;
		G += X;
		break;
	case 1:		//  60° <= H < 120°
		R += X;
		G += C;
		break;
	case 2:		// 120° <= H < 180°
		G += C;
		B += X;
		break;
	case 3:		// 180° <= H < 240°
		G += X;
		B += C;
		break;
	case 4:		// 240° <= H < 300°
		R += X;
		B += C;
		break;
	case 5:		// 300° <= H < 360°
		R += C;
		B += X;
		break;
	default:	//should never happen
		break;
	}
}

void HSVColor::HSV_to_RGB(unsigned int H, unsigned int S, unsigned int V, unsigned int&R, unsigned int&G, unsigned int&B)
{
	float _h, _s, _v, _r, _g, _b;
	_h = FRotator::ClampAxis((float)H);
	_s = FMath::Clamp(S, (unsigned int)0, (unsigned int)100) / 100.0f;
	_v = FMath::Clamp(V, (unsigned int)0, (unsigned int)100) / 100.0f;
	HSV_to_RGB(_h, _s, _v, _r, _g, _b);
	R = (unsigned int)(_r * 255.0f);
	G = (unsigned int)(_g * 255.0f);
	B = (unsigned int)(_b * 255.0f);
}

void HSVColor::RGB_to_HSV(float R, float G, float B, float&H, float&S, float&V)
{
	//conversion formula taken from http://www.rapidtables.com/convert/color/rgb-to-hsv.htm

	const float _r = FMath::Clamp(R, 0.0f, 1.0f);
	const float _g = FMath::Clamp(G, 0.0f, 1.0f);
	const float _b = FMath::Clamp(B, 0.0f, 1.0f);
	float Cmax, Cmin, Delta;
	if (_r < _b)
	{
		Cmax = (_g < _b) ? _b : _g;
		Cmin = (_g < _r) ? _g : _r;
	}
	else
	{
		Cmax = (_g < _r) ? _r : _g;
		Cmin = (_g < _b) ? _g : _b;
	}
	Delta = Cmax - Cmin;

	V = Cmax;
	S = (Cmax != 0.0f) ? Delta / Cmax : Cmax;

	if (Delta == 0.0f)
	{	//avoid divide by zero
		H = Delta;
	}
	else
	{	//using hue formula
		if (Cmax == _r)
		{
			H = 60.0f * FMath::Fmod(FMath::Abs(_g - _b) / Delta, 6.0f);
		}
		else if (Cmax == _g)
		{
			H = 60.0f * ((FMath::Abs(_b - _r) / Delta) + 2.0f);
		}
		else	//Cmax == _b
		{
			H = 60.0f * ((FMath::Abs(_r - _g) / Delta) + 4.0f);
		}
	}	//end 0 delta special case
}


void HSVColor::RGB_to_HSV(unsigned int R, unsigned int G, unsigned int B, unsigned int&H, unsigned int&S, unsigned int&V)
{
	float _h, _s, _v, _r, _g, _b;
	_r = FMath::Clamp((int)R, 0, 255);
	_g = FMath::Clamp((int)G, 0, 255);
	_b = FMath::Clamp((int)B, 0, 255);
	_r /= 255.0f;
	_g /= 255.0f;
	_b /= 255.0f;
	RGB_to_HSV(_r, _g, _b, _h, _s, _v);
	H = (unsigned int)(_h);
	S = (unsigned int)(_s * 100.0f);
	V = (unsigned int)(_v * 100.0f);
}