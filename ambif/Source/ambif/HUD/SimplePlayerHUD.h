// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated
//This code is based on the wiki tutorial found at the following link
//https://wiki.unrealengine.com/HUD,_Canvas,_Code_Sample_of_800%2B_Lines,_Create_Buttons_%26_Draw_Materials
//most of the code was adapted to my intent.

#pragma once

#include "GameFramework/HUD.h"
#include "LogicController.h"
#include "SimplePlayerHUD.generated.h"

typedef struct location{
	float minX, minY, maxX, maxY;

	location(){
		minX = minY = maxX = maxY = -1;
	}
} t_item_location;

USTRUCT()
struct FMusicButtonStruct
{
	GENERATED_USTRUCT_BODY()

	//Vars
	int32 		type;
	FString		toolTip;
	float 		minX;
	float 		maxX;
	float 		minY;
	float 		maxY;

	//default properties

	FMusicButtonStruct()
	{
		type = -1;
		toolTip = "";
		minX = 0;
		maxX = 0;
		minY = 0;
		maxY = 0;
	}

	FORCEINLINE FMusicButtonStruct& operator<<(t_item_location& location){
		minX = location.minX;
		maxX = location.maxX;
		minY = location.minY;
		maxY = location.maxY;
		return *this;
	}
};

class LayoutSimple{
public:
	LayoutSimple(int sizex, int sizey, int padx, int pady, int max_item_per_row);
private:
	int size_x, size_y;
	int padding_x, padding_y;
	int item_per_row;

public:
	t_item_location getLocation(int item_id);

};

/**
 * 
 */
UCLASS()
class AMBIF_API ASimplePlayerHUD : public AHUD
{
	GENERATED_UCLASS_BODY()

	//---------internal components reference-------------
public:
	/** Manager Actor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LogicController)
	ALogicController* Manager;

	//-------------HUD settings------------------
	/** Verdana */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyHUD_Options)
	UFont* VerdanaFont;

	/** Font Scaling Used By Your HUD Code */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyHUD_Options)
	float DefaultFontScale;

	/** HUD Scaling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyHUD_Options)
	float GlobalHUDMult;

	//-----------------------CURSOR & BUTTON TEXTURE-----------------------
	/** Cursor */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	UTexture2D* CursorMain;

	/** Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	UTexture2D* CursorHoveringButton;

	/** Button Play normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTexturePlay;

	/** Button Play hover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTexturePlayHover;

	/** Button Pause normal */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTexturePause;

	/** Button Pause hover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTexturePauseHover;

	/** Button Stop normal*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTextureStop;

	/** Button Stop hover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTextureStopHover;

	/** Button Spawn normal*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTextureSpawn;

	/** Button Spawn hover */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Buttons)
	UTexture2D* ButtonTextureSpawnHover;

	/* Draw Hud? */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MyHUD_Options)
	bool DontDrawHUD;

	//Cursor
public:
	FVector2D MouseLocation;
	/**Cursor Draw Offset
	 *		use this to position texture over the point of your cursor,
	 *			if the point is not at exactly 0,0
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	FVector2D MouseOffset;

	/** Cursor size
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	FVector2D CursorSize;

	/** Is cursor texture designed to set mouse focus in the middle of the texture?
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cursor)
	bool CenterMouseTexture;

	void DrawHUD_DrawCursor();

	//Buttons
public:
	TArray<FMusicButtonStruct> ButtonsMain;
	TArray<FMusicButtonStruct> ButtonsConfirm;

	//* custom button layout used to set button size and position
	LayoutSimple buttonLayout;

	//Cursor In buttons
	void DrawHUD_CheckCursorInButtons();
	void CheckCursorInButtonsMain();
	void CheckCursorInButtonsConfirm();

	const FMusicButtonStruct* CurCheckButton;
	int32 CheckCursorInButton(const TArray<FMusicButtonStruct>& ButtonArray);
	int32 ClickedButtonType;
	//States
	bool ConfirmDialogOpen;
	bool InMainMenu;

	int32 		ActiveButton_Type;
	FString 	ActiveButton_Tip;
	bool CursorHoveringInButton;

	//Colors
public:
	const FLinearColor * ColorPtr;

	//Colors
	static const FColor		FColorBlack;
	static const FColor		FColorRed;
	static const FColor		FColorYellow;
	static const FColor		FColorBlue;
	static const FColor		FColor_White;

	static const FLinearColor LC_Black;
	static const FLinearColor LC_Pink;
	static const FLinearColor LC_Red;
	static const FLinearColor LC_Yellow;

	//FString
public:

	//Titles
	static const FString S_Title_Main;
	static const FString S_Title_Confirm;

	//Button Text
	static const FString S_Button_Restart;
	static const FString S_Button_Exit;

	// Utility 

	//Stop Camera From Moving With Mouse
	FORCEINLINE void SetCursorMoveOnly(bool CursorOnly)
	{
		if (!ThePC) return;
		//
		ThePC->SetIgnoreLookInput(CursorOnly);

	}

	//DrawLine
	FORCEINLINE void DrawLine
		(
		const FVector2D& Start,
		const FVector2D& End,
		const FLinearColor& TheColor,
		const float& Thick
		)
	{
		if (!Canvas) return;
		//

		FCanvasLineItem NewLine(Start, End);
		NewLine.SetColor(TheColor);
		NewLine.LineThickness = Thick;
		Canvas->DrawItem(NewLine);
	}

	//~

	FORCEINLINE void DrawRect(
		float X, float Y,
		float Width, float Height,
		const FLinearColor& Color
		)
	{
		if (!Canvas) return;
		//

		FCanvasTileItem RectItem(
			FVector2D(X, Y),
			FVector2D(Width, Height),
			Color
			);

		RectItem.BlendMode = SE_BLEND_Translucent;
		Canvas->DrawItem(RectItem);
	}


	//DrawText
	FORCEINLINE void DrawHUDText(
		UFont*	TheFont,
		const FString& TheStr,
		const float& X, const float& Y,
		const FLinearColor& TheColor,
		const float& TheScale,
		bool DrawOutline = false,
		const FLinearColor OutlineColor = FLinearColor(0, 0, 0, 1)
		) {
		if (!Canvas) return;
		//

		//Text and Font
		FCanvasTextItem NewText(
			FVector2D(X, Y),
			FText::FromString(TheStr),
			TheFont,
			TheColor
			);

		//Text Scale
		NewText.Scale.Set(TheScale, TheScale);

		//Outline gets its alpha from the main color
		NewText.bOutlined = true;
		NewText.OutlineColor = OutlineColor;
		NewText.OutlineColor.A = TheColor.A * 2;

		//Draw
		Canvas->DrawItem(NewText);
	}

	//Draw Full Size Tile
	FORCEINLINE void DrawFullSizeTile(UTexture2D* tex, float x, float y, const FColor& Color)
	{
		if (!Canvas) return;
		if (!tex) return;

		Canvas->SetDrawColor(Color);

		//Draw
		Canvas->DrawTile(
			tex, x, y,
			tex->GetSurfaceWidth(), //screen width
			tex->GetSurfaceHeight(),  //screen height
			0, //texture start width
			0, //texture start height
			tex->GetSurfaceWidth(), //texture width from start
			tex->GetSurfaceHeight(), //texture height from start
			BLEND_Translucent
			);
	}


	FORCEINLINE void VDrawTile(UTexture2D* tex, float x, float y, float screenX, float screenY, const FColor& TheColor)
	{
		if (!Canvas) return;
		if (!tex) return;

		Canvas->SetDrawColor(TheColor);

		//Draw
		Canvas->DrawTile(
			tex, x, y,
			screenX, //screen width
			screenY,  //screen height
			0, //texture start width
			0, //texture start height
			tex->GetSurfaceWidth(), //texture width from start
			tex->GetSurfaceHeight(), //texture height from start
			BLEND_Translucent
			);
	}

	//Draw
public:
	void DrawHUD_DrawDialogs();

	//Menus
	void DrawMainMenu();
	void DrawConfirm();

	//Buttons
	void DrawMainMenuButtons();
	void DrawConfirmButtons();
public:
	void DrawToolTip();

	//Core
public:
	APlayerController* ThePC;
	void PlayerInputChecks();
protected:
	//Draw HUD
	void DrawHUD_Reset();
	virtual void DrawHUD() override;

	/** after all game elements are created */
	virtual void PostInitializeComponents() override;

};