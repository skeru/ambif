//* Copyright 2015 Stefano Cherubin
//* stefano1{dot}cherubin{at}mail{dot}polimi{dot}it
//* ISPG - Image and Sound Processing Group
//* Politecnico di Milano

#pragma once

#include "GameFramework/HUD.h"
#include "InspectionHUD.generated.h"

/**
 * 
 */
UCLASS()
class AMBIF_API AInspectionHUD : public AHUD
{
	GENERATED_BODY()

public:
	AInspectionHUD(const FObjectInitializer & ObjectInitializer);
	//------------------------------CONST DEF------------------------------
public:
	/** Beackground Color */
	static const FColor BgColor;
	
	//-----------------------------HUD TEXTURE----------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Style")
	/** BgTexture */
	UTexture2D* BackgroundTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Style")
	/** Background padding for tooltip text */
	FVector2D ToolTipPadding;

	//----------------------------HUD SETTINGS----------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Style")
	/** Default text font */
	UFont* DefaultTextFont;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD Style")
	/** Font scaling used to render text */
	float DefaultFontScale;

	//------------------------------HUD REFS------------------------------
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "References")
	/** Player Controller */
	APlayerController* PC;

private:
	/** text to show */
	FString Text;

	/** current mouse location */
	FVector2D MouseLocation;

	/** How far from mouse start drawing tooltip */
	FVector2D ToolTipMouseOffset;

	//------------------------------HUD UTILS------------------------------
private:
	/** Creates a TextItem ready to be drown in canvas
	 *
	 * @param TheFont font used to draw text.
	 * @param start starting point of rendered texture.
	 * @param TheColor text color.
	 * @param TheScale font scale.
	 * @param DrawOutline enable text outline. default false.
	 * @param OutlineColor text outline color. default white.
	 * @return an FCanvasTextItem object ready to be drawed in Canvas.
	 */
	inline FCanvasTextItem MakeTextItem(
		UFont*	TheFont,
		const FString& TheStr,
		const FVector2D& start,
		const FLinearColor& TheColor,
		const float& TheScale,
		bool DrawOutline = false,
		const FLinearColor OutlineColor = FLinearColor(0, 0, 0, 1)
		);

	/** Draw a 2D texture
	 * 
	 * @param tex Texture to draw
	 * @param x Screen starting point x
	 * @param y Screen starting point y
	 * @param screenX rendered width
	 * @param screenY rendered height
	 */
	inline void DrawTexture(UTexture2D* tex, FVector2D start, FVector2D size);

public:
	void SetText(FString NewText);
		
	//----------------------------HUD FUNCTION----------------------------
protected:
	/** Main HUD function */
	virtual void DrawHUD() override;
};
