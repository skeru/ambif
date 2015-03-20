// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "InspectionHUD.h"

#define HUD_BGTEXTURE_MANDATORY

AInspectionHUD::AInspectionHUD(const FObjectInitializer &ObjectInitializer) 
	:Super(ObjectInitializer) 
{
	DefaultFontScale = 0.05f;
	ToolTipPadding = FVector2D(10.0, 10.0);
	Text = "";
	MouseLocation = FVector2D(0.0f, 0.0f);
	ToolTipMouseOffset = FVector2D(20.0, 10.0);
}

//------------------------------CONST DEF------------------------------

const FColor AInspectionHUD::BgColor = FColor::White;

//------------------------------HUD UTILS------------------------------

FCanvasTextItem AInspectionHUD::MakeTextItem(
	UFont*	TheFont,
	const FString& TheStr,
	const FVector2D& start,
	const FLinearColor& TheColor,
	const float& TheScale,
	bool DrawOutline,
	const FLinearColor OutlineColor
	) 
{
	//Text and Font
	FCanvasTextItem NewText(
		start,
		FText::FromString(TheStr),
		TheFont,
		TheColor
		);

	//Text Scale
	NewText.Scale.Set(TheScale, TheScale);

	//NewText.bCentreX = true;

	//Outline gets its alpha from the main color
	NewText.bOutlined = DrawOutline;
	NewText.OutlineColor = OutlineColor;
	NewText.OutlineColor.A = TheColor.A * 2;

	return NewText;
}

void AInspectionHUD::DrawTexture(UTexture2D* tex, FVector2D start, FVector2D size)
{
	if (!Canvas) return;
	if (!tex) return;

	Canvas->SetDrawColor(BgColor);

	//Draw
	Canvas->DrawTile(
		tex, start.X, start.Y,
		size.X, //screen width
		size.Y,  //screen height
		0, //texture start width
		0, //texture start height
		tex->GetSurfaceWidth(), //texture width from start
		tex->GetSurfaceHeight(), //texture height from start
		BLEND_Translucent
		);
}

void AInspectionHUD::SetText(FString NewText)
{
	Text = NewText;
}

//----------------------------HUD FUNCTION----------------------------
void AInspectionHUD::DrawHUD()
{
	if (Text == "")
		return;
	if (!PC) {
		DebugUtils::LogString("InspectionHUD: PlayerController not set.");
		return;
	}
#ifdef HUD_BGTEXTURE_MANDATORY
	if (!BackgroundTexture) {
		DebugUtils::LogString("InspectionHUD: Background Texture not set.");
		return;
	}
#endif
	if (!Canvas) {
		DebugUtils::LogString("InspectionHUD: Canvas not found.");
		return;
	}
	PC->GetMousePosition(MouseLocation.X, MouseLocation.Y);
	
	FCanvasTextItem item = MakeTextItem(DefaultTextFont, Text, MouseLocation + ToolTipMouseOffset, FColor::Black, DefaultFontScale);
	Canvas->DrawItem(item);	//draw once to estimate size
	DrawTexture(BackgroundTexture, item.Position - ToolTipPadding, item.DrawnSize + (ToolTipPadding * 2));	//draw background
	Canvas->DrawItem(item);	//draw second time over background
}
