// Stefano Cherubin stefano1.cherubin@mail.polimi.it - except where otherwise stated
//This code is based on the wiki tutorial found at the following link
//https://wiki.unrealengine.com/HUD,_Canvas,_Code_Sample_of_800%2B_Lines,_Create_Buttons_%26_Draw_Materials
//most of the code was adapted to my intent.

#include "ambif.h"
#include "CustomUtils/DebugUtils.h"
#include "SimplePlayerHUD.h"


#define BUTTON_ID_PLAY 0
#define BUTTON_ID_PAUSE 1
#define BUTTON_ID_STOP 2

#define BUTTONTYPE_MAIN_RESTART 	5
#define BUTTONTYPE_MAIN_EXIT 		6

#define BUTTONTYPE_CONFIRM_YES 	3
#define BUTTONTYPE_CONFIRM_NO 	4

#define BUTTON_ID_SPAWN 11

#define MAX_BUTTONS_PER_ROW 10

#define CANVAS_WHITE if(Canvas) Canvas->SetDrawColor(FColor_White);

#define TESTING_SPAWN

//Static Consts
//

const FString ASimplePlayerHUD::S_Title_Main = FString("Joy!");
const FString ASimplePlayerHUD::S_Title_Confirm = FString("Exit Game?");

const FString ASimplePlayerHUD::S_Button_Restart = FString("Restart");
const FString ASimplePlayerHUD::S_Button_Exit = FString("Exit");

// Colors 
const FColor ASimplePlayerHUD::FColorBlack = FColor(0, 0, 0, 255);
const FColor ASimplePlayerHUD::FColorRed = FColor(255, 0, 0, 255);
const FColor ASimplePlayerHUD::FColorYellow = FColor(255, 255, 0, 255);
const FColor ASimplePlayerHUD::FColorBlue = FColor(0, 0, 255, 255);
const FColor ASimplePlayerHUD::FColor_White = FColor(255, 255, 255, 255);
// Backgrounds 
const FLinearColor ASimplePlayerHUD::LC_Black = FLinearColor(0, 0, 0, 1);
const FLinearColor ASimplePlayerHUD::LC_Pink = FLinearColor(1, 0, 1, 1);
const FLinearColor ASimplePlayerHUD::LC_Red = FLinearColor(1, 0, 0, 1);
const FLinearColor ASimplePlayerHUD::LC_Yellow = FLinearColor(1, 1, 0, 1);


//--layout stuff
LayoutSimple::LayoutSimple(int sizex = 100, int sizey = 50, int padx = 10, int pady = 10, int max_item_per_row = MAX_BUTTONS_PER_ROW) {
	size_x = sizex;
	size_y = sizey;
	padding_x = padx;
	padding_y = pady;
	item_per_row = max_item_per_row;
}

t_item_location LayoutSimple::getLocation(int id){
	t_item_location location;
	int row = item_per_row != 0 ? id / item_per_row : 0;
	int col = item_per_row != 0 ? id - (item_per_row * row) : id;
	location.minX = (padding_x * (col + 1)) + (size_x * col);
	location.maxX = (padding_x + size_x) * (col + 1);
	location.minY = (padding_y * (row + 1)) + (size_y * row);
	location.maxY = (padding_y + size_y) * (row + 1);
	return location;
}
//--end of layout stuff

ASimplePlayerHUD::ASimplePlayerHUD(const FObjectInitializer & ObjectInitializer) 
	:Super(ObjectInitializer)
{

	buttonLayout = LayoutSimple();

	//Draw HUD?
	DontDrawHUD = false;

	//Mouse
	CenterMouseTexture = false;
	MouseOffset = FVector2D(0, 0);
	CursorSize = FVector2D(80, 80);

	//States
	ConfirmDialogOpen = false;
	InMainMenu = true;

	//Scale
	GlobalHUDMult = 1;
	DefaultFontScale = 0.7;   //scaling down a size 36 font

	//	 I recommend creating fonts at a high resolution / size like 36
	//			then you can scale down the font as needed to any size of your choice

	// this avoids needing to make multiple fonts for different sizes, but have a high
	// resolution when you use larger font sizes
}

void ASimplePlayerHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//Establish the PC
	ThePC = GetOwningPlayerController();

	//How to get a ref to your custom PC
	//AYourPlayerController* YourChar = Cast<AYourPlayerController>(ThePC);

	//How to Get The Character
	//AYourCharacterClass* YourChar = Cast<AYourCharacterClass>(GetOwningPawn());

}


//===============
// Draw Dialogs
//===============
void ASimplePlayerHUD::DrawHUD_DrawDialogs()
{
	DrawMainMenu();
	if (ConfirmDialogOpen) DrawConfirm();
}
//Menus
void ASimplePlayerHUD::DrawMainMenu()
{
	//Draw buttons
	DrawMainMenuButtons();
}
void ASimplePlayerHUD::DrawConfirm()
{
	//Blue rect with alpha 50%
	DrawRect(Canvas->SizeX / 2 - 100, Canvas->SizeY / 2 - 50, 200, 100, FLinearColor(0, 0, 1, 0.2333));

	//Confirm Title

	//Draw buttons
	DrawConfirmButtons();
}


//Buttons
void ASimplePlayerHUD::DrawMainMenuButtons()
{
	//Start Point
	t_item_location button_loc;

	//insert button code
#define INSERT_BUTTON(ID, TEXTURE,TEXTURE_HOVER, TOOLTIP) {\
	button_loc = buttonLayout.getLocation((ID));\
	VDrawTile(((ID) != ActiveButton_Type) ? (TEXTURE) : (TEXTURE_HOVER), button_loc.minX, button_loc.minY, button_loc.maxX - button_loc.minX, button_loc.maxY - button_loc.minY, FColor(255, 255, 255, 255)); \
	if (ButtonsMain.Num() <= (ID))\
		{\
		FMusicButtonStruct newButton = FMusicButtonStruct();\
		newButton.type = (ID);\
		newButton.toolTip = (TOOLTIP);\
		newButton << button_loc;\
		ButtonsMain.Add(newButton);\
		}\
		}

	INSERT_BUTTON(BUTTON_ID_PLAY, ButtonTexturePlay, ButtonTexturePlayHover, "Play");
	INSERT_BUTTON(BUTTON_ID_PAUSE, ButtonTexturePause, ButtonTexturePauseHover, "Pause");
	INSERT_BUTTON(BUTTON_ID_STOP, ButtonTextureStop, ButtonTextureStopHover, "Stop");
#ifdef TESTING_SPAWN
	INSERT_BUTTON(BUTTON_ID_SPAWN, ButtonTextureSpawn, ButtonTextureSpawnHover, "Spawn");
#endif

#undef INSERT_BUTTON

}

void ASimplePlayerHUD::DrawConfirmButtons()
{
	float xStart = Canvas->SizeX / 2 - 100;
	float yStart = Canvas->SizeY / 2 - 40;

	//Highlighted?
	if (ActiveButton_Type == BUTTONTYPE_CONFIRM_YES) ColorPtr = &LC_Pink;
	else ColorPtr = &LC_Yellow;

	//Text
	DrawHUDText(
		VerdanaFont, "Yes", xStart + 30, yStart + 20,
		*ColorPtr, DefaultFontScale,
		true
		);

	if (ButtonsConfirm.Num() < 1)
	{
		FMusicButtonStruct newButton = FMusicButtonStruct();
		newButton.type = BUTTONTYPE_CONFIRM_YES;
		newButton.toolTip = "";
		newButton.minX = xStart;
		newButton.maxX = xStart + 75;
		newButton.minY = yStart + 20;
		newButton.maxY = yStart + 60;

		//could use GetTextSize to streamline this

		//Add to correct array
		ButtonsConfirm.Add(newButton);
	}

	xStart = Canvas->SizeX / 2 + 20;
	yStart = Canvas->SizeY / 2 - 40;

	//Highlighted?
	if (ActiveButton_Type == BUTTONTYPE_CONFIRM_NO) ColorPtr = &LC_Black;
	else ColorPtr = &LC_Yellow;

	//Text
	DrawHUDText(
		VerdanaFont, "No", xStart + 30, yStart + 20,
		*ColorPtr, DefaultFontScale,
		true
		);

	if (ButtonsConfirm.Num() < 2)
	{
		FMusicButtonStruct newButton = FMusicButtonStruct();
		newButton.type = BUTTONTYPE_CONFIRM_NO;
		newButton.toolTip = "";
		newButton.minX = xStart;
		newButton.maxX = xStart + 75;
		newButton.minY = yStart + 20;
		newButton.maxY = yStart + 60;

		//could use GetTextSize to streamline this

		//Add to correct array
		ButtonsConfirm.Add(newButton);
	}
}

//===============
// Cursor In Buttons
//===============
int32 ASimplePlayerHUD::CheckCursorInButton(const TArray<FMusicButtonStruct>& ButtonArray)
{
	for (int32 b = 0; b < ButtonArray.Num(); b++)
	{
		CurCheckButton = &ButtonArray[b];

		//check cursor in bounds
		if (CurCheckButton->minX <= MouseLocation.X && MouseLocation.X <= CurCheckButton->maxX &&
			CurCheckButton->minY <= MouseLocation.Y && MouseLocation.Y <= CurCheckButton->maxY)
		{

			//Active Button Type
			ActiveButton_Type = CurCheckButton->type;

			//Tool Tip
			ActiveButton_Tip = CurCheckButton->toolTip;

			//Change Cursor
			CursorHoveringInButton = true;

			//Mouse Clicked?
			if (ThePC->WasInputKeyJustPressed(EKeys::LeftMouseButton))
			{
				return ActiveButton_Type;
				//no need to check rest of buttons
			}
		}
	}

	//No Click Occurred This Tick
	return -1;
}

//Check Confirm
void ASimplePlayerHUD::CheckCursorInButtonsConfirm()
{
	//Check Confirm Buttons
	ClickedButtonType = CheckCursorInButton(ButtonsConfirm); //fills global ActiveButton_Type

	if (ClickedButtonType == BUTTONTYPE_CONFIRM_YES)
	{
		ThePC->ConsoleCommand("Exit");
		return;
	}
	if (ClickedButtonType == BUTTONTYPE_CONFIRM_NO)
	{
		ConfirmDialogOpen = false;
		ButtonsConfirm.Empty(); //Buttons not needed anymore
		return;
	}
}

//Check Buttons
void ASimplePlayerHUD::CheckCursorInButtonsMain()
{
	//Check Confirm Buttons
	ClickedButtonType = CheckCursorInButton(ButtonsMain);

	switch (ClickedButtonType)
	{
	case BUTTON_ID_PLAY:
		Manager->MusicPlay();
		break;
	case BUTTON_ID_PAUSE:
		Manager->MusicPause();
		break;
	case BUTTON_ID_STOP:
		Manager->MusicStop();
		break;
#ifdef TESTING_SPAWN
	case BUTTON_ID_SPAWN:
		Manager->ReadData();
		Manager->RefreshViewList();

		Manager->Spawn();
		//TODO implement ???
		Manager->UpdateWidgetInterface();

		break;
#endif
	default:
		break;
	}

	if (ClickedButtonType == BUTTONTYPE_MAIN_EXIT)
	{
		ConfirmDialogOpen = true;
		return;
	}
}

void ASimplePlayerHUD::DrawHUD_CheckCursorInButtons()
{
	if (ConfirmDialogOpen)
	{
		CheckCursorInButtonsConfirm();

		//Take Focus Away From All Other buttons
		return;
	}

	//Main
	CheckCursorInButtonsMain();
}

void ASimplePlayerHUD::DrawToolTip()
{
	//if mouse is too far to right, draw from left instead
	float xStart = MouseLocation.X + 150;
	float yStart = MouseLocation.Y + 5;

	//out vars
	float RV_xLength;
	float RV_yLength;
	//Text Size
	GetTextSize(
		ActiveButton_Tip,
		RV_xLength,
		RV_yLength,
		VerdanaFont,
		DefaultFontScale * 2
		);

	// Decide Draw to Left or to the Right 

	//Draw to the Left
	if (xStart + RV_xLength >= Canvas->SizeX - 40)
	{
		xStart -= 150 + 140 + 64 + RV_xLength;

		//If Text is too long, bring it closer to the cursor
		if (xStart < 33) xStart = 33;
	}

	//Background
	DrawRect(
		xStart, yStart,
		RV_xLength + 70,
		80,
		FLinearColor(0, 0, 1, 0.7) //alpha 0.7
		);

	//Tool Tip
	DrawHUDText(
		VerdanaFont,
		ActiveButton_Tip,
		xStart + 32, yStart + 20,
		LC_Pink,
		DefaultFontScale * 2,
		false		//scale position of message with HUD scale
		);
}

void ASimplePlayerHUD::DrawHUD_DrawCursor()
{
	//Tool Tip
	if (ActiveButton_Tip != "") DrawToolTip();

	//Cursor Hovering in a Button?
	if (CursorHoveringInButton)
	{
		//pointer tex found?
		if (!CursorHoveringButton) return;
		if (CenterMouseTexture)
		{
			VDrawTile(CursorHoveringButton, MouseLocation.X - MouseOffset.X - (CursorSize.X / 2), MouseLocation.Y - MouseOffset.Y - (CursorSize.Y / 2), CursorSize.X, CursorSize.Y, FColor_White);
		}
		else
		{
			VDrawTile(CursorHoveringButton, MouseLocation.X - MouseOffset.X, MouseLocation.Y - MouseOffset.Y, CursorSize.X, CursorSize.Y, FColor_White);
		}
	}

	else
	{
		//cursor tex found?
		if (!CursorMain) return;
		if (CenterMouseTexture)
		{
			VDrawTile(CursorMain, MouseLocation.X - MouseOffset.X - (CursorSize.X / 2), MouseLocation.Y - MouseOffset.Y - (CursorSize.Y / 2), CursorSize.X, CursorSize.Y, FColor_White);
		}
		else
		{
			VDrawTile(CursorMain, MouseLocation.X - MouseOffset.X, MouseLocation.Y - MouseOffset.Y, CursorSize.X, CursorSize.Y, FColor_White);
		}
	}
}

void ASimplePlayerHUD::PlayerInputChecks()
{
	//check out this tutorial of mine for a list of all EKeys::
	//http://forums.epicgames.com/threads/972861-Tutorials-C-for-UE4-Code-Samples-gt-gt-New-Video-Freeze-Render-When-Tabbed-Out?p=31660286&viewfull=1#post31660286

	if (ThePC->WasInputKeyJustPressed(EKeys::Escape))
	{
		SetCursorMoveOnly(false);
		return;
	}
	if (ThePC->WasInputKeyJustPressed(EKeys::F))
	{
		SetCursorMoveOnly(!ThePC->IsLookInputIgnored());
		return;
	}
	if (ThePC->WasInputKeyJustPressed(EKeys::H))
	{
		DontDrawHUD = !DontDrawHUD;
		return;
	}

	//Confirm
	if (ConfirmDialogOpen)
	{
		if (ThePC->WasInputKeyJustPressed(EKeys::Y))
		{
			ThePC->ConsoleCommand("Exit");
			//could replace with function based on confirm context

			return;
		}
		if (ThePC->WasInputKeyJustPressed(EKeys::N))
		{
			ConfirmDialogOpen = false;
			ButtonsConfirm.Empty(); //Buttons not needed anymore
			//Cancel Confirm

			return;
		}
	}
}

void ASimplePlayerHUD::DrawHUD_Reset()
{
	ActiveButton_Type = -1;
	ActiveButton_Tip = "";
	CursorHoveringInButton = false;
}

void ASimplePlayerHUD::DrawHUD()
{
	//==============================
	//==============================
	//==============================
	//Have PC for Input Checks and Mouse Cursor?
	if (!ThePC)
	{
		//Attempt to Reacquire PC
		ThePC = GetOwningPlayerController();

		//Could Not Obtain PC
		if (!ThePC) return;
	}

	//Multiplayer Safety Check
	if (!ThePC->PlayerInput) return; //not valid for first seconds of a multiplayer client

	//Player Input
	PlayerInputChecks();

	//Draw HUD?
	if (DontDrawHUD) return;

	//Super
	Super::DrawHUD();

	//No Canvas?
	if (!Canvas) return;
	//

	//Reset States
	DrawHUD_Reset();

	//================
	//Get New Mouse Position
	//================
	ThePC->GetMousePosition(MouseLocation.X, MouseLocation.Y);

	//Cursor In Buttons
	DrawHUD_CheckCursorInButtons();

	//Draw Dialogs
	DrawHUD_DrawDialogs();

	//### Do Last ###
	//Draw Cursor
	DrawHUD_DrawCursor();

	//Debugging Info
	//ThePC->ClientMessage("HUD Loop Completed!");
}

