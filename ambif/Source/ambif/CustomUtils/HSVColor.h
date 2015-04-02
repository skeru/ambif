//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

/** Hue Saturation Value Color
 *
 * Provide a conversion from HSV values to RGB format and FLinearColor
 */
class AMBIF_API HSVColor
{
public:
	HSVColor();
	~HSVColor();
	HSVColor(unsigned int Hue, unsigned int Saturation, unsigned int Value);
	HSVColor(float Hue, float Saturation, float Value);
	HSVColor(FLinearColor rgb_color);

private:

	/** Hue
	* range [0 - 359]
	*/
	float H;

	/** Saturation
	* range [0 - 1]
	*/
	float S;

	/** Value
	* range [0 - 1]
	*/
	float V;

	/** Red
	* range [0 - 1]
	*/
	float R;

	/** Green
	* range [0 - 1]
	*/
	float G;

	/** Blue
	* range [0 - 1]
	*/
	float B;

public:
	/** Set HSV Hue [0 - 359]
	*/
	inline void SetH(unsigned int Hue);

	/** Set HSV Saturation [0 - 100]
	*/
	inline void SetS(unsigned int Saturation);

	/** Set HSV Value [0 - 100]
	*/
	inline void SetV(unsigned int Value);

	/** Set HSV Hue [0 - 360)
	*/
	inline void SetH(float Hue);

	/** Set HSV Saturation [0 - 1]
	*/
	inline void SetS(float Saturation);

	/** Set HSV Value [0 - 1]
	*/
	inline void SetV(float Value);

	/** Create FLinearColor object from this class */
	FLinearColor ToFLinearColor();

	//-------------------------------STATIC FUNCTIONS-------------------------------
public:

	static FLinearColor ToLinearColor(float H, float S, float V);

	static FLinearColor ToLinearColor(unsigned int H, unsigned int S, unsigned int V);

	/** Converts HSV values to corresponding RGB values
	*
	* @param H [ 0.0 - 360.0 ) Hue
	* @param S [ 0.0 -   1.0 ] Saturation
	* @param V [ 0.0 -   1.0 ] Value
	* @param R [ 0.0 -   1.0 ] Red (output)
	* @param G [ 0.0 -   1.0 ] Green (output)
	* @param B [ 0.0 -   1.0 ] Blue (output)
	*/
	static void HSV_to_RGB(float H, float S, float V, float&R, float&G, float&B);

	/** Converts HSV values to corresponding RGB values
	*
	* @param H [ 0 - 359 ] Hue
	* @param S [ 0 - 100 ] Saturation
	* @param V [ 0 - 100 ] Value
	* @param R [ 0 - 255 ] Red (output)
	* @param G [ 0 - 255 ] Green (output)
	* @param B [ 0 - 255 ] Blue (output)
	*/
	static void HSV_to_RGB(unsigned int H, unsigned int S, unsigned int V, unsigned int&R, unsigned int&G, unsigned int&B);

	/** Converts HSV values to corresponding RGB values
	*
	* @param R [ 0.0 -   1.0 ] Red
	* @param G [ 0.0 -   1.0 ] Green
	* @param B [ 0.0 -   1.0 ] Blue
	* @param H [ 0.0 - 360.0 ) Hue (output)
	* @param S [ 0.0 -   1.0 ] Saturation (output)
	* @param V [ 0.0 -   1.0 ] Value (output)
	*/
	static void RGB_to_HSV(float R, float G, float B, float&H, float&S, float&V);

	/** Converts HSV values to corresponding RGB values
	*
	* @param R [ 0 - 255 ] Red
	* @param G [ 0 - 255 ] Green
	* @param B [ 0 - 255 ] Blue
	* @param H [ 0 - 359 ] Hue (output)
	* @param S [ 0 - 100 ] Saturation (output)
	* @param V [ 0 - 100 ] Value (output)
	*/
	static void RGB_to_HSV(unsigned int R, unsigned int G, unsigned int B, unsigned int&H, unsigned int&S, unsigned int&V);

};
