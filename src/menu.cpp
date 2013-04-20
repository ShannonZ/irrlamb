/*************************************************************************************
*	irrlamb - http://irrlamb.googlecode.com
*	Copyright (C) 2013  Alan Witkowski
*
*	This program is free software: you can redistribute it and/or modify
*	it under the terms of the GNU General Public License as published by
*	the Free Software Foundation, either version 3 of the License, or
*	(at your option) any later version.
*
*	This program is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************************/
#include <all.h>
#include <menu.h>
#include <engine/graphics.h>
#include <engine/input.h>
#include <engine/constants.h>
#include <engine/interface.h>
#include <engine/globals.h>
#include <engine/config.h>
#include <engine/game.h>
#include <engine/save.h>
#include <engine/campaign.h>
#include <engine/level.h>
#include <engine/save.h>
#include <viewreplay.h>
#include <play.h>
#include <null.h>
#include <engine/namespace.h>
#include <font/CGUITTFont.h>

_Menu Menu;

const int CAMPAIGN_LEVELID = 1000;
const int PLAY_CAMPAIGNID = 900;

const int TITLE_Y = 200;
const int TITLE_SPACING = 120;
const int BUTTON_SPACING = 70;
const int CAMPAIGN_SPACING = 60;
const int BACK_Y = 220;
const int SAVE_X = 60;
				
const int STATS_MIN_WIDTH = 250;
const int STATS_MIN_HEIGHT = 310;
const int STATS_PADDING = 15;
const int STATS_MOUSE_OFFSETX = 20;
const int STATS_MOUSE_OFFSETY = -105;

enum GUIElements {
	MAIN_SINGLEPLAYER, MAIN_REPLAYS, MAIN_OPTIONS, MAIN_QUIT,
	SINGLEPLAYER_BACK,
	LEVELS_GO, LEVELS_BUY, LEVELS_HIGHSCORES, LEVELS_BACK, LEVELS_SELECTEDLEVEL,
	LEVELINFO_DESCRIPTION, LEVELINFO_ATTEMPTS, LEVELINFO_WINS, LEVELINFO_LOSSES, LEVELINFO_PLAYTIME, LEVELINFO_BESTTIME,
	REPLAYS_FILES, REPLAYS_GO, REPLAYS_DELETE, REPLAYS_BACK,
	OPTIONS_VIDEO, OPTIONS_AUDIO, OPTIONS_CONTROLS, OPTIONS_BACK,
	VIDEO_SAVE, VIDEO_CANCEL, VIDEO_VIDEOMODES, VIDEO_FULLSCREEN, VIDEO_SHADOWS, VIDEO_SHADERS, VIDEO_ANISOTROPY, VIDEO_ANTIALIASING,
	AUDIO_ENABLED, AUDIO_SAVE, AUDIO_CANCEL,
	CONTROLS_SAVE, CONTROLS_CANCEL, CONTROLS_INVERTMOUSE, CONTROLS_MOVEFORWARD, CONTROLS_MOVEBACK, CONTROLS_MOVELEFT, CONTROLS_MOVERIGHT, CONTROLS_MOVERESET, CONTROLS_MOVEJUMP,
	PAUSE_RESUME, PAUSE_SAVEREPLAY, PAUSE_RESTART, PAUSE_OPTIONS, PAUSE_QUITLEVEL,
	SAVEREPLAY_NAME, SAVEREPLAY_SAVE, SAVEREPLAY_CANCEL,
	LOSE_RESTARTLEVEL, LOSE_SAVEREPLAY, LOSE_MAINMENU,
	WIN_RESTARTLEVEL, WIN_NEXTLEVEL, WIN_SAVEREPLAY, WIN_MAINMENU,
};

// Handle action inputs
bool _Menu::HandleAction(int InputType, int Action, float Value) {
	if(Input.HasJoystick())
		Input.DriveMouse(Action, Value);

	// On action press
	if(Value) {
		switch(Action) {
			case _Actions::MENU_PAUSE:
			case _Actions::MENU_BACK:
				switch(State) {
					case STATE_MAIN:
						Game.SetDone(true);
					break;
					case STATE_SINGLEPLAYER:
					case STATE_OPTIONS:
					case STATE_REPLAYS:
						if(Game.GetState() == &PlayState)
							InitPause();
						else
							InitMain();
					break;
					case STATE_LEVELS:
						InitSinglePlayer();
					break;
					case STATE_VIDEO:
					case STATE_AUDIO:
					case STATE_CONTROLS:
						InitOptions();
					break;
					case STATE_PAUSED:
						InitPlay();
					break;
					case STATE_WIN:
						NullState.State = STATE_LEVELS;
						Game.ChangeState(&NullState);
					break;
					case STATE_SAVEREPLAY:
						if(PreviousState == STATE_WIN)
							InitWin();
						else
							InitPause();
					break;
				}

				return true;
			break;
			case _Actions::RESET:
				if(!Value)
					return false;

				if(State == STATE_LOSE || State == STATE_WIN) {
					PlayState.StartReset();
					return true;
				}
			break;
		}
	}

	return false;
}

// Key presses
bool _Menu::HandleKeyPress(int Key) {

	bool Processed = true;
	switch(State) {
		case STATE_MAIN:
			switch(Key) {
				case KEY_RETURN:
					InitSinglePlayer();
				break;
				default:
					Processed = false;
				break;
			}
		break;
		case STATE_SINGLEPLAYER:
			switch(Key) {
				case KEY_RETURN:
					InitLevels();
				break;
				default:
					Processed = false;
				break;
			}
		break;
		case STATE_CONTROLS:
			if(KeyButton != NULL) {
				stringw KeyName = Input.GetKeyName(Key);

				// Assign the key
				if(Key != KEY_ESCAPE && KeyName != "") {
					
					int ActionType = KeyButton->getID() - CONTROLS_MOVEFORWARD;

					// Swap if the key already exists
					for(int i = 0; i <= _Actions::RESET; i++) {
						if(CurrentKeys[i] == Key) {
							
							// Get button
							IGUIButton *SwapButton = static_cast<IGUIButton *>(CurrentLayout->getElementFromId(CONTROLS_MOVEFORWARD + i));

							// Swap text
							SwapButton->setText(stringw(Input.GetKeyName(CurrentKeys[ActionType])).c_str());
							CurrentKeys[i] = CurrentKeys[ActionType];
							break;
						}
					}

					// Update key
					KeyButton->setText(KeyName.c_str());
					CurrentKeys[ActionType] = Key;
				}
				else
					KeyButton->setText(KeyButtonOldText.c_str());

				KeyButton = NULL;
			}
		break;
		case STATE_SAVEREPLAY:
			switch(Key) {
				case KEY_RETURN:
					Menu.SaveReplay();
				break;
				default:
					Processed = false;
				break;
			}
		break;
	}

	return Processed;
}

// Handles GUI events
void _Menu::HandleGUI(irr::gui::EGUI_EVENT_TYPE EventType, IGUIElement *Element) {

	switch(EventType) {
		case EGET_BUTTON_CLICKED:
			switch(Element->getID()) {
				case MAIN_SINGLEPLAYER:
					InitSinglePlayer();
				break;
				case MAIN_REPLAYS:
					InitReplays();
				break;
				case MAIN_OPTIONS:
					InitOptions();
				break;
				case MAIN_QUIT:
					Game.SetDone(true);
				break;
				case SINGLEPLAYER_BACK:
					InitMain();
				break;
				case LEVELS_GO:
					LaunchLevel();
				break;
				case LEVELS_BACK:
					InitSinglePlayer();
				break;
				case REPLAYS_GO:
					LaunchReplay();
				break;
				case REPLAYS_DELETE: {

					// Get list
					IGUIListBox *ReplayList = static_cast<IGUIListBox *>(CurrentLayout->getElementFromId(REPLAYS_FILES));
					int SelectedIndex = ReplayList->getSelected();
					if(SelectedIndex != -1) {

						// Remove file
						std::string FilePath = Save.GetReplayPath() + GetReplayFile();
						remove(FilePath.c_str());

						// Remove entry
						ReplayList->removeItem(SelectedIndex);
					}
				}
				break;
				case REPLAYS_BACK:
					InitMain();
				break;
				case OPTIONS_VIDEO:
					InitVideo();
				break;
				case OPTIONS_AUDIO:
					InitAudio();
				break;
				case OPTIONS_CONTROLS:
					InitControls();
				break;
				case OPTIONS_BACK:
					if(Game.GetState() == &PlayState)
						InitPause();
					else
						InitMain();
				break;
				case VIDEO_SAVE: {
					
					// Save the video mode
					IGUIComboBox *VideoModes = static_cast<IGUIComboBox *>(CurrentLayout->getElementFromId(VIDEO_VIDEOMODES));
					if(VideoModes != NULL) {
						VideoModeStruct Mode = Graphics.GetVideoModes()[VideoModes->getSelected()];
						Config.ScreenWidth = Mode.Width;
						Config.ScreenHeight = Mode.Height;
					}

					// Save full screen
					IGUICheckBox *Fullscreen = static_cast<IGUICheckBox *>(CurrentLayout->getElementFromId(VIDEO_FULLSCREEN));
					Config.Fullscreen = Fullscreen->isChecked();

					// Save shadows
					IGUICheckBox *Shadows = static_cast<IGUICheckBox *>(CurrentLayout->getElementFromId(VIDEO_SHADOWS));
					Config.Shadows = Shadows->isChecked();

					// Save the anisotropy
					IGUIComboBox *Anisotropy = static_cast<IGUIComboBox *>(CurrentLayout->getElementFromId(VIDEO_ANISOTROPY));
					if(Anisotropy != NULL) {
						if(Anisotropy->getSelected() == 0)
							Config.AnisotropicFiltering = 0;
						else
							Config.AnisotropicFiltering = 1 << (Anisotropy->getSelected() - 1);
					}

					// Save the antialiasing
					IGUIComboBox *Antialiasing = static_cast<IGUIComboBox *>(CurrentLayout->getElementFromId(VIDEO_ANTIALIASING));
					if(Antialiasing != NULL) {
						if(Antialiasing->getSelected() == 0)
							Config.AntiAliasing = 0;
						else
							Config.AntiAliasing = 1 << (Antialiasing->getSelected());
					}

					/*// Save shaders
					IGUICheckBox *Shaders = static_cast<IGUICheckBox *>(CurrentLayout->getElementFromId(VIDEO_SHADERS));
					Config.Shaders = Shaders->isChecked();
					 */

					// Write config
					Config.WriteConfig();

					InitOptions();
				}
				break;
				case VIDEO_CANCEL:
					InitOptions();
				break;
				case AUDIO_SAVE: {
					bool OldAudioEnabled = Config.AudioEnabled;

					// Get settings
					IGUICheckBox *AudioEnabled = static_cast<IGUICheckBox *>(CurrentLayout->getElementFromId(AUDIO_ENABLED));
					bool Enabled = AudioEnabled->isChecked();

					// Save
					Config.AudioEnabled = Enabled;
					Config.WriteConfig();

					// Init or disable audio system
					if(OldAudioEnabled != Enabled) {
						if(Enabled) {
							Game.EnableAudio();
						}
						else
							Game.DisableAudio();
					}

					InitOptions();
				}
				break;
				case AUDIO_CANCEL:
					InitOptions();
				break;
				case CONTROLS_SAVE: {

					// Write config
					for(int i = 0; i <= _Actions::RESET; i++)
						Config.Keys[i] = CurrentKeys[i];

					// Save invert mouse
					IGUICheckBox *InvertMouse = static_cast<IGUICheckBox *>(CurrentLayout->getElementFromId(CONTROLS_INVERTMOUSE));
					Config.InvertMouse = InvertMouse->isChecked();

					Config.WriteConfig();

					InitOptions();
				}
				break;
				case CONTROLS_CANCEL:
					InitOptions();
				break;
				case CONTROLS_MOVEFORWARD:
				case CONTROLS_MOVEBACK:
				case CONTROLS_MOVELEFT:
				case CONTROLS_MOVERIGHT:
				case CONTROLS_MOVERESET:
				case CONTROLS_MOVEJUMP:	{
					if(KeyButton)
						CancelKeyBind();

					KeyButton = static_cast<IGUIButton *>(Element);
					KeyButtonOldText = KeyButton->getText();
					KeyButton->setText(L"");
				}
				break;
				default: {

					if(Element->getID() >= CAMPAIGN_LEVELID) {
						SelectedLevel = Element->getID() - CAMPAIGN_LEVELID;
						LaunchLevel();
					}
					else if(Element->getID() >= PLAY_CAMPAIGNID) {
						CampaignIndex = Element->getID() - PLAY_CAMPAIGNID;
						InitLevels();
					}
				}
				break;
				case PAUSE_RESUME:
					InitPlay();
				break;
				case PAUSE_SAVEREPLAY:
					InitSaveReplay();
				break;
				case PAUSE_OPTIONS:
					InitOptions();
				break;
				case PAUSE_RESTART:
					PlayState.StartReset();
				break;
				case PAUSE_QUITLEVEL:
					NullState.State = STATE_LEVELS;
					Game.ChangeState(&NullState);
				break;
				case SAVEREPLAY_SAVE:
					SaveReplay();
				break;
				case SAVEREPLAY_CANCEL:
					if(PreviousState == STATE_WIN)
						InitWin();
					else
						InitPause();
				break;
				case WIN_RESTARTLEVEL:
					PlayState.StartReset();
				break;
				case WIN_NEXTLEVEL:
					if(PlayState.CampaignLevel+1 < Campaign.GetLevelCount(PlayState.CurrentCampaign))
						PlayState.CampaignLevel++;
					Game.ChangeState(&PlayState);
				break;
				case WIN_SAVEREPLAY:
					InitSaveReplay();
				break;
				case WIN_MAINMENU:
					NullState.State = STATE_LEVELS;
					Game.ChangeState(&NullState);
				break;
			}
		break;
		case EGET_LISTBOX_SELECTED_AGAIN:
			switch(Element->getID()) {
				case REPLAYS_FILES:
					LaunchReplay();
				break;
			}
		break;
		case EGET_ELEMENT_HOVERED:
			if(Element->getID() >= CAMPAIGN_LEVELID) {
				SelectedLevel = Element->getID() - CAMPAIGN_LEVELID;
			}
		break;
		case EGET_ELEMENT_LEFT:
			if(State == STATE_LEVELS)
				SelectedLevel = -1;
		break;
	}
}

// Create the main menu
void _Menu::InitMain() {
	Interface.ChangeSkin(_Interface::SKIN_MENU);
	Input.SetMouseLocked(false);
	ClearCurrentLayout();

	// Logo
	irrGUI->addImage(irrDriver->getTexture("art/logo.jpg"), position2di(Interface.GetCenterX() - 256, Interface.GetCenterY() - 270), true, CurrentLayout);
	AddMenuText(position2di(40, irrDriver->getScreenSize().Height - 20), stringw(GAME_VERSION).c_str(), _Interface::FONT_SMALL);

	// Button
	int Y = Interface.GetCenterY() - TITLE_Y + TITLE_SPACING;
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + BUTTON_SPACING * 0, 194, 52), MAIN_SINGLEPLAYER, L"Single Player", _Interface::IMAGE_BUTTON_BIG);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + BUTTON_SPACING * 1, 194, 52), MAIN_REPLAYS, L"Replays", _Interface::IMAGE_BUTTON_BIG);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + BUTTON_SPACING * 2, 194, 52), MAIN_OPTIONS, L"Options", _Interface::IMAGE_BUTTON_BIG);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + BUTTON_SPACING * 3, 194, 52), MAIN_QUIT, L"Quit", _Interface::IMAGE_BUTTON_BIG);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	PreviousState = State;
	State = STATE_MAIN;
}

// Create the single player menu
void _Menu::InitSinglePlayer() {
	ClearCurrentLayout();
	
	// Reset menu variables
	CampaignIndex = 0;
	SelectedLevel = -1;

	// Text
	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;
	AddMenuText(position2di(X, Y), L"Level Sets");

	// Campaigns
	Y += TITLE_SPACING;
	const std::vector<CampaignStruct> &Campaigns = Campaign.GetCampaigns();
	for(u32 i = 0; i < Campaigns.size(); i++) {
		irr::core::stringw Name(Campaigns[i].Name.c_str());
		IGUIButton *Button = AddMenuButton(Interface.GetCenteredRect(X, Y, 194, 52), PLAY_CAMPAIGNID + i, Name.c_str());

		Y += CAMPAIGN_SPACING;
	}

	Y = Interface.GetCenterY() + BACK_Y;
	AddMenuButton(Interface.GetCenteredRect(X, Y, 108, 44), SINGLEPLAYER_BACK, L"Back", _Interface::IMAGE_BUTTON_SMALL);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	PreviousState = State;
	State = STATE_SINGLEPLAYER;
}

// Create the levels menu
void _Menu::InitLevels() {
	ClearCurrentLayout();
	LevelStats.clear();
	SelectedLevel = -1;
	const CampaignStruct &CampaignData = Campaign.GetCampaign(CampaignIndex);

	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;

	// Text
	AddMenuText(position2di(X, Y), stringw(CampaignData.Name.c_str()).c_str());

	// Add level list
	X = Interface.GetCenterX() - 160;
	Y += TITLE_SPACING;
	int Column = 0, Row = 0;
	for(u32 i = 0; i < CampaignData.Levels.size(); i++) {
		bool Unlocked = true;
				
		// Get level stats
		const SaveLevelStruct *Stats = Save.GetLevelStats(CampaignData.Levels[i].File);
		LevelStats.push_back(Stats);

		// Set unlocked status
		if(Stats->Unlocked == 0) {
			Unlocked = false;

			// Unlock the level if it's always unlocked in the campaign
			if(CampaignData.Levels[i].Unlocked) {
				Save.UnlockLevel(CampaignData.Levels[i].File);
				Unlocked = true;
			}
		}				

		// Add button
		IGUIButton *Level = irrGUI->addButton(Interface.GetCenteredRect(X + Column * 80, Y + Row * 80, 64, 64), CurrentLayout, CAMPAIGN_LEVELID + i);

		// Set thumbnail
		if(Unlocked)
			Level->setImage(irrDriver->getTexture((CampaignData.Levels[i].DataPath + "icon.jpg").c_str()));
		else
			Level->setImage(irrDriver->getTexture("art/locked.png"));

		Column++;
		if(Column >= 5) {
			Column = 0;
			Row++;
		}
	}

	// Buttons
	X = Interface.GetCenterX();
	Y = Interface.GetCenterY() + BACK_Y;
	AddMenuButton(Interface.GetCenteredRect(X, Y, 108, 44), LEVELS_BACK, L"Back", _Interface::IMAGE_BUTTON_SMALL);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	PreviousState = State;
	State = STATE_LEVELS;
}

// Create the replay menu
void _Menu::InitReplays() {
	Interface.ChangeSkin(_Interface::SKIN_MENU);
	ClearCurrentLayout();
	char Buffer[256];

	// Text
	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;
	AddMenuText(position2di(X, Y), L"Replays");

	// Level selection
	Y = Interface.GetCenterY();
	IGUIListBox *ListReplays = irrGUI->addListBox(Interface.GetCenteredRect(X, Y, 650, 250), CurrentLayout, REPLAYS_FILES, true);

	// Change directories
	std::string OldWorkingDirectory(irrFile->getWorkingDirectory().c_str());
	irrFile->changeWorkingDirectoryTo(Save.GetReplayPath().c_str());

	// Get a list of replays
	IFileList *FileList = irrFile->createFileList();
	u32 FileCount = FileList->getFileCount();
	ReplayFiles.clear();
	for(u32 i = 0; i < FileCount; i++) {
		if(!FileList->isDirectory(i) && FileList->getFileName(i).find(".replay") != -1) {
			ReplayFiles.push_back(FileList->getFileName(i).c_str());
		}
	}
	irrFile->changeWorkingDirectoryTo(OldWorkingDirectory.c_str());

	// Add replays to menu list
	for(u32 i = 0; i < ReplayFiles.size(); i++) {
		bool Loaded = Replay.LoadReplay(ReplayFiles[i].c_str(), true);
		if(Loaded && Replay.GetVersion() == REPLAY_VERSION) {

			// Get time string
			Interface.ConvertSecondsToString(Replay.GetFinishTime(), Buffer);

			// Build replay string
			std::string ReplayInfo = ReplayFiles[i] + std::string(" - ") + Replay.GetLevelName()
									+ std::string(" - ") + Replay.GetDescription() + std::string(" - ") + Buffer;

			irr::core::stringw ReplayString(ReplayInfo.c_str());
			ListReplays->addItem(ReplayString.c_str());
		}
	}

	// Confirmations
	Y += 160;
	AddMenuButton(Interface.GetCenteredRect(X - 123, Y, 108, 44), REPLAYS_GO, L"View", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(X, Y, 108, 44), REPLAYS_DELETE, L"Delete", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(X + 123, Y, 108, 44), REPLAYS_BACK, L"Back", _Interface::IMAGE_BUTTON_SMALL);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	PreviousState = State;
	State = STATE_REPLAYS;
}

// Create the options menu
void _Menu::InitOptions() {
	Interface.ChangeSkin(_Interface::SKIN_MENU);
	ClearCurrentLayout();
			
	// Text
	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;
	AddMenuText(position2di(X, Y), L"Options");
	
	Y += TITLE_SPACING;

	// Buttons
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + 0 * BUTTON_SPACING, 194, 52), OPTIONS_VIDEO, L"Video");
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + 1 * BUTTON_SPACING, 194, 52), OPTIONS_AUDIO, L"Audio");
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Y + 2 * BUTTON_SPACING, 194, 52), OPTIONS_CONTROLS, L"Controls");
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX(), Interface.GetCenterY() + BACK_Y, 108, 44), OPTIONS_BACK, L"Back", _Interface::IMAGE_BUTTON_SMALL);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	PreviousState = State;
	State = STATE_OPTIONS;
}

// Create the video options menu
void _Menu::InitVideo() {
	ClearCurrentLayout();

	// Text
	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;
	AddMenuText(position2di(X, Y), L"Video");

	// Video modes
	Y += TITLE_SPACING;
	const std::vector<VideoModeStruct> &ModeList = Graphics.GetVideoModes();
	if(ModeList.size() > 0) {
		AddMenuText(position2di(X, Y), L"Screen Resolution", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
		IGUIComboBox *ListScreenResolution = irrGUI->addComboBox(Interface.GetCenteredRect(X + 111, Y, 200, 30), CurrentLayout, VIDEO_VIDEOMODES);

		// Populate mode list
		for(u32 i = 0; i < ModeList.size(); i++)
			ListScreenResolution->addItem(ModeList[i].String.c_str());
		ListScreenResolution->setSelected(Graphics.GetCurrentVideoModeIndex());
	}
				
	// Full Screen
	Y += 40;
	AddMenuText(position2di(X, Y), L"Fullscreen", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
	IGUICheckBox *CheckBoxFullscreen = irrGUI->addCheckBox(Config.Fullscreen, Interface.GetCenteredRect(X + 20, Y, 18, 18), CurrentLayout, VIDEO_FULLSCREEN);

	// Shadows
	Y += 40;
	AddMenuText(position2di(X, Y), L"Shadows", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
	IGUICheckBox *CheckBoxShadows = irrGUI->addCheckBox(Config.Shadows, Interface.GetCenteredRect(X + 20, Y, 18, 18), CurrentLayout, VIDEO_SHADOWS);

	// Anisotropic Filtering
	Y += 40;
	int MaxAnisotropy = irrDriver->getDriverAttributes().getAttributeAsInt("MaxAnisotropy");
	AddMenuText(position2di(X, Y), L"Anisotropic Filtering", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
	IGUIComboBox *Anisotropy = irrGUI->addComboBox(Interface.GetCenteredRect(X + 61, Y, 100, 30), CurrentLayout, VIDEO_ANISOTROPY);

	// Populate anisotropy list
	Anisotropy->addItem(stringw(0).c_str());
	for(int i = 0, Level = 1; Level <= MaxAnisotropy; i++, Level <<= 1) {
		Anisotropy->addItem(stringw(Level).c_str());
		if(Config.AnisotropicFiltering == Level)
			Anisotropy->setSelected(i+1);
	}

	// Anti-aliasing
	Y += 40;
	AddMenuText(position2di(X, Y), L"MSAA", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
	IGUIComboBox *Antialiasing = irrGUI->addComboBox(Interface.GetCenteredRect(X + 61, Y, 100, 30), CurrentLayout, VIDEO_ANTIALIASING);

	// Populate anti-aliasing list
	Antialiasing->addItem(stringw(0).c_str());
	for(int i = 0, Level = 2; Level <= 8; i++, Level <<= 1) {
		Antialiasing->addItem(stringw(Level).c_str());
		if(Config.AntiAliasing == Level)
			Antialiasing->setSelected(i+1);
	}
	
/*
	// Shaders
	Y += 30;
	IGUIStaticText *TextShaders = irrGUI->addStaticText(L"Shaders", Interface.GetCenteredRect(X - 65, Y, 110, 25), false, false);
	IGUICheckBox *CheckBoxShaders = irrGUI->addCheckBox(Config.Shaders, Interface.GetCenteredRect(X + 60, Y, 100, 25), CurrentLayout, VIDEO_SHADERS);
	if(!Graphics.GetShadersSupported())
		CheckBoxShaders->setEnabled(false);
*/

	// Save
	Y =  Interface.GetCenterY() + BACK_Y;
	AddMenuButton(Interface.GetCenteredRect(X - SAVE_X, Y, 108, 44), VIDEO_SAVE, L"Save", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(X + SAVE_X, Y, 108, 44), VIDEO_CANCEL, L"Cancel", _Interface::IMAGE_BUTTON_SMALL);

	// Warning
	Y = Interface.GetCenterY() + BACK_Y - 50;
	AddMenuText(position2di(X, Y), L"Changes are applied after restart", _Interface::FONT_SMALL, -1);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	PreviousState = State;
	State = STATE_VIDEO;
}

// Create the audio options menu
void _Menu::InitAudio() {
	ClearCurrentLayout();

	// Text
	int X = Interface.GetCenterX(), Y = Interface.GetCenterY() - TITLE_Y;
	AddMenuText(position2di(X, Y), L"Audio");

	// Sound enabled
	Y += TITLE_SPACING;
	AddMenuText(position2di(X, Y), L"Audio Enabled", _Interface::FONT_MEDIUM, -1, EGUIA_LOWERRIGHT);
	IGUICheckBox *CheckBoxAudioEnabled = irrGUI->addCheckBox(Config.AudioEnabled, Interface.GetCenteredRect(X + 20, Y, 18, 18), CurrentLayout, AUDIO_ENABLED);

	// Save
	Y += 90;
	AddMenuButton(Interface.GetCenteredRect(X - SAVE_X, Interface.GetCenterY() + BACK_Y, 108, 44), AUDIO_SAVE, L"Save", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(X + SAVE_X, Interface.GetCenterY() + BACK_Y, 108, 44), AUDIO_CANCEL, L"Cancel", _Interface::IMAGE_BUTTON_SMALL);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	PreviousState = State;
	State = STATE_AUDIO;
}

// Create the control options menu
void _Menu::InitControls() {
	ClearCurrentLayout();

	int X = Interface.GetCenterX();
	int Y = Interface.GetCenterY() - TITLE_Y;

	// Text
	AddMenuText(position2di(X, Y), L"Controls");

	// Create the key buttons
	Y += TITLE_SPACING;
	KeyButton = NULL;
	for(int i = 0; i <= _Actions::RESET; i++) {
				
		CurrentKeys[i] = Config.Keys[i];
		IGUIStaticText *Text = irrGUI->addStaticText(stringw(Actions.GetName(i).c_str()).c_str(), Interface.GetCenteredRect(X - 50, Y, 80, 20), false, false, CurrentLayout);
		Text->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT);
		AddMenuButton(Interface.GetCenteredRect(X + 50, Y, 108, 44), CONTROLS_MOVEFORWARD + i, stringw(Input.GetKeyName(CurrentKeys[i])).c_str(), _Interface::IMAGE_BUTTON_SMALL);

		Y += 35;
	}

	// Invert mouse
	Y += 5;
	IGUIStaticText *TextInvertMouse = irrGUI->addStaticText(L"Invert Mouse", Interface.GetCenteredRect(X - 65, Y, 110, 25), false, false, CurrentLayout);
	TextInvertMouse->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxInvertMouse = irrGUI->addCheckBox(Config.InvertMouse, Interface.GetCenteredRect(X + 60, Y, 100, 25), CurrentLayout, CONTROLS_INVERTMOUSE);

	// Save
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX() - SAVE_X, Interface.GetCenterY() + BACK_Y, 108, 44), CONTROLS_SAVE, L"Save", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX() + SAVE_X, Interface.GetCenterY() + BACK_Y, 108, 44), CONTROLS_CANCEL, L"Cancel", _Interface::IMAGE_BUTTON_SMALL);
	
	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	PreviousState = State;
	State = STATE_CONTROLS;
}

// Init play GUI
void _Menu::InitPlay() {
	Interface.ChangeSkin(_Interface::SKIN_GAME);
	ClearCurrentLayout();

	Graphics.SetClearColor(SColor(255, 0, 0, 0));
	Input.SetMouseLocked(true);

	PreviousState = State;
	State = STATE_NONE;
}

// Create the pause menu
void _Menu::InitPause() {
	ClearCurrentLayout();

	int X = Interface.GetCenterX();
	int Y = Interface.GetCenterY() - 125;

	AddMenuButton(Interface.GetCenteredRect(X, Y + 0 * BUTTON_SPACING, 194, 52), PAUSE_RESUME, L"Resume");
	AddMenuButton(Interface.GetCenteredRect(X, Y + 1 * BUTTON_SPACING, 194, 52), PAUSE_SAVEREPLAY, L"Save Replay");
	AddMenuButton(Interface.GetCenteredRect(X, Y + 2 * BUTTON_SPACING, 194, 52), PAUSE_RESTART, L"Restart Level");
	AddMenuButton(Interface.GetCenteredRect(X, Y + 3 * BUTTON_SPACING, 194, 52), PAUSE_OPTIONS, L"Options");
	AddMenuButton(Interface.GetCenteredRect(X, Y + 4 * BUTTON_SPACING, 194, 52), PAUSE_QUITLEVEL, L"Quit Level");

	Input.SetMouseLocked(false);

	PreviousState = State;
	State = STATE_PAUSED;
}

// Create the save replay GUI
void _Menu::InitSaveReplay() {
	Interface.ChangeSkin(_Interface::SKIN_MENU);
	ClearCurrentLayout();

	// Draw interface
	IGUIEditBox *EditName = irrGUI->addEditBox(L"", Interface.GetCenteredRect(Interface.GetCenterX(), Interface.GetCenterY() - 20, 172, 32), true, CurrentLayout, SAVEREPLAY_NAME);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX() - SAVE_X, Interface.GetCenterY() + 20, 108, 44), SAVEREPLAY_SAVE, L"Save", _Interface::IMAGE_BUTTON_SMALL);
	AddMenuButton(Interface.GetCenteredRect(Interface.GetCenterX() + SAVE_X, Interface.GetCenterY() + 20, 108, 44), SAVEREPLAY_CANCEL, L"Cancel", _Interface::IMAGE_BUTTON_SMALL);

	irrGUI->setFocus(EditName);
	EditName->setMax(32);

	PreviousState = State;
	State = STATE_SAVEREPLAY;
}

// Create the lose screen
void _Menu::InitLose() {

	PreviousState = State;
	State = STATE_LOSE;
}

// Create the win screen
void _Menu::InitWin() {
	Interface.Clear();
	
	// Skip stats if just testing a level
	bool LastLevelInCampaign = false;
	if(PlayState.TestLevel == "") {

		// Increment win count
		Save.IncrementLevelWinCount(Level.GetLevelName());

		// Add high score
		Save.AddScore(Level.GetLevelName(), PlayState.Timer);

		// Unlock next level
		int LevelCount = Campaign.GetLevelCount(PlayState.CurrentCampaign);
		if(PlayState.CampaignLevel+1 >= LevelCount) {
			LastLevelInCampaign = true;
		}
		else {
			const std::string &NextLevelFile = Campaign.GetLevel(PlayState.CurrentCampaign, PlayState.CampaignLevel+1);
			Save.UnlockLevel(NextLevelFile);
		}

		// Save stats to a file
		Save.SaveLevelStats(Level.GetLevelName());
	}
	else
		LastLevelInCampaign = true;

	// Get level stats
	WinStats = Save.GetLevelStats(Level.GetLevelName());
	
	// Clear interface
	ClearCurrentLayout();

	int X = Interface.GetCenterX();
	int Y = Interface.GetCenterY() + WIN_HEIGHT / 2 + 25;
	AddMenuButton(Interface.GetCenteredRect(X - 165, Y, 102, 34), WIN_RESTARTLEVEL, L"Retry Level", _Interface::IMAGE_BUTTON_MEDIUM);
	IGUIButton *ButtonNextLevel = AddMenuButton(Interface.GetCenteredRect(X - 55, Y, 102, 34), WIN_NEXTLEVEL, L"Next Level", _Interface::IMAGE_BUTTON_MEDIUM);
	AddMenuButton(Interface.GetCenteredRect(X + 55, Y, 102, 34), WIN_SAVEREPLAY, L"Save Replay", _Interface::IMAGE_BUTTON_MEDIUM);
	AddMenuButton(Interface.GetCenteredRect(X + 165, Y, 102, 34), WIN_MAINMENU, L"Main Menu", _Interface::IMAGE_BUTTON_MEDIUM);

	if(LastLevelInCampaign)
		ButtonNextLevel->setEnabled(false);

	Input.SetMouseLocked(false);
	vector2di Position = ButtonNextLevel->getAbsolutePosition().getCenter();
	irrDevice->getCursorControl()->setPosition(Position.X, Position.Y);

	PreviousState = State;
	State = STATE_WIN;
}

// Saves a replay
void _Menu::SaveReplay() {

	IGUIEditBox *EditName = static_cast<IGUIEditBox *>(CurrentLayout->getElementFromId(SAVEREPLAY_NAME));
	if(EditName != NULL) {
		irr::core::stringc ReplayTitle(EditName->getText());
		Replay.SaveReplay(ReplayTitle.c_str());
	}

	switch(PreviousState) {
		case STATE_WIN: {
			InitWin();
			
			IGUIButton *ButtonSaveReplay = static_cast<IGUIButton *>(CurrentLayout->getElementFromId(WIN_SAVEREPLAY));
			ButtonSaveReplay->setEnabled(false);
		}
		break;
		case STATE_LOSE: {
			InitLose();
			
			IGUIButton *ButtonSaveReplay = static_cast<IGUIButton *>(CurrentLayout->getElementFromId(LOSE_SAVEREPLAY));
			ButtonSaveReplay->setEnabled(false);
		}
		break;
		default:
			InitPause();
		break;
	}
}

// Updates the current state
void _Menu::Update(float FrameTime) {
}

// Draws the current state
void _Menu::Draw() {
	irrGUI->drawAll();

	// Draw level tooltip
	switch(State) {
		case STATE_LEVELS:
			if(SelectedLevel != -1) {
				char Buffer[256];
				const SaveLevelStruct *Stats = LevelStats[SelectedLevel];
				const std::string &NiceName = Campaign.GetLevelNiceName(CampaignIndex, SelectedLevel);
				
				// Get text dimensions
				dimension2du NiceNameSize = Interface.GetFont(_Interface::FONT_MEDIUM)->getDimension(stringw(NiceName.c_str()).c_str());
	
				// Get box position
				int Width = NiceNameSize.Width + STATS_PADDING * 2, Height = STATS_MIN_HEIGHT + STATS_PADDING, X, Y;
				int Left = (int)Input.GetMouseX() + STATS_MOUSE_OFFSETX;
				int Top = (int)Input.GetMouseY() + STATS_MOUSE_OFFSETY;
				
				if(Width < STATS_MIN_WIDTH)
					Width = STATS_MIN_WIDTH;

				// Cap limits
				if(Top < STATS_PADDING)
					Top = STATS_PADDING;
				if(Left + Width > (int)irrDriver->getScreenSize().Width - 10)
					Left -= Width + 35;

				// Draw box
				Interface.DrawTextBox(Left + Width/2, Top + Height/2, Width, Height);
				X = Left + Width/2;
				Y = Top + STATS_PADDING;

				if(Stats->Unlocked) {

					// Level nice name
					Interface.RenderText(NiceName.c_str(), X, Y, _Interface::ALIGN_CENTER, _Interface::FONT_MEDIUM, SColor(255, 255, 255, 255));
					Y += 35;

					// Play time
					Interface.RenderText("Play time", X - 10, Y, _Interface::ALIGN_RIGHT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					Interface.ConvertSecondsToString(Stats->PlayTime, Buffer);
					Interface.RenderText(Buffer, X + 10, Y, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

					// Load count
					Y += 17;
					Interface.RenderText("Plays", X - 10, Y, _Interface::ALIGN_RIGHT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					sprintf(Buffer, "%d", Stats->LoadCount);
					Interface.RenderText(Buffer, X + 10, Y, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

					// Win count
					Y += 17;
					Interface.RenderText("Wins", X - 10, Y, _Interface::ALIGN_RIGHT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					sprintf(Buffer, "%d", Stats->WinCount);
					Interface.RenderText(Buffer, X + 10, Y, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

					// Scores
					if(Stats->HighScores.size() > 0) {

						// High scores
						int HighX = Left + Width/2 - 100, HighY = Y + 28;

						// Draw header
						Interface.RenderText("#", HighX, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
						Interface.RenderText("Time", HighX + 30, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
						Interface.RenderText("Date", HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
						HighY += 19;

						for(size_t i = 0; i < Stats->HighScores.size(); i++) {
						
							// Number
							char SmallBuffer[32];
							sprintf(SmallBuffer, "%d", (int)i+1);
							Interface.RenderText(SmallBuffer, HighX, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

							// Time
							Interface.ConvertSecondsToString(Stats->HighScores[i].Time, Buffer);
							Interface.RenderText(Buffer, HighX + 30, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

							// Date
							char DateString[32];
							strftime(DateString, 32, "%m-%d-%Y", localtime(&Stats->HighScores[i].DateStamp));
							Interface.RenderText(DateString, HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));

							HighY += 18;
						}
					}
				}
				else {
				
					// Locked
					Interface.RenderText("Level Locked", X, Y, _Interface::ALIGN_CENTER, _Interface::FONT_MEDIUM, SColor(255, 255, 255, 255));
				}
			}
		break;
		case STATE_WIN:
			Menu.DrawWinScreen();
		break;
	}
}

// Draw the win screen
void _Menu::DrawWinScreen() {
	char Buffer[256];

	char TimeString[32];
	Interface.ConvertSecondsToString(PlayState.Timer, TimeString);

	// Draw header
	int X = Interface.GetCenterX();
	int Y = Interface.GetCenterY() - WIN_HEIGHT / 2 + 15;
	Interface.DrawTextBox(Interface.GetCenterX(), Interface.GetCenterY(), WIN_WIDTH, WIN_HEIGHT);
	Interface.RenderText("Level Completed!", X, Y, _Interface::ALIGN_CENTER, _Interface::FONT_LARGE);

	// Draw time
	Y += 45;
	Interface.RenderText("Your Time", X - 115, Y, _Interface::ALIGN_LEFT, _Interface::FONT_MEDIUM, SColor(200, 255, 255, 255));
	Interface.RenderText(TimeString, X + 115, Y, _Interface::ALIGN_RIGHT, _Interface::FONT_MEDIUM, SColor(200, 255, 255, 255));

	// Best time
	Y += 25;
	if(WinStats->HighScores.size() > 0) {
		Interface.RenderText("Best Time", X - 115, Y, _Interface::ALIGN_LEFT, _Interface::FONT_MEDIUM, SColor(200, 255, 255, 255));
		Interface.ConvertSecondsToString(WinStats->HighScores[0].Time, Buffer);
		Interface.RenderText(Buffer, X + 115, Y, _Interface::ALIGN_RIGHT, _Interface::FONT_MEDIUM, SColor(200, 255, 255, 255));
	}

	// High scores
	int HighX = Interface.GetCenterX() - 75, HighY = Y + 48;

	// Draw header
	Interface.RenderText("#", HighX, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
	Interface.RenderText("Time", HighX + 30, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
	Interface.RenderText("Date", HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
	HighY += 17;
	for(u32 i = 0; i < WinStats->HighScores.size(); i++) {
				
		// Number
		char SmallBuffer[32];
		sprintf(SmallBuffer, "%d", i+1);
		Interface.RenderText(SmallBuffer, HighX, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(200, 255, 255, 255));

		// Time
		Interface.ConvertSecondsToString(WinStats->HighScores[i].Time, Buffer);
		Interface.RenderText(Buffer, HighX + 30, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(200, 255, 255, 255));

		// Date
		char DateString[32];
		strftime(DateString, 32, "%m-%d-%Y", localtime(&WinStats->HighScores[i].DateStamp));
		Interface.RenderText(DateString, HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(200, 255, 255, 255));

		HighY += 17;
	}
}

// Cancels the key bind state
void _Menu::CancelKeyBind() {
	KeyButton->setText(KeyButtonOldText.c_str());
	KeyButton = NULL;
}

// Gets the replay name from a selection box
std::string _Menu::GetReplayFile() {

	// Get list
	IGUIListBox *ReplayList = static_cast<IGUIListBox *>(CurrentLayout->getElementFromId(REPLAYS_FILES));
	if(!ReplayList)
		return "";
	
	int SelectedIndex = ReplayList->getSelected();
	if(SelectedIndex != -1) {
		return ReplayFiles[SelectedIndex];
	}

	return "";
}

// Launchs a level
void _Menu::LaunchLevel() {
	
	SaveLevelStruct Stats;
	Save.GetLevelStats(Campaign.GetCampaign(CampaignIndex).Levels[SelectedLevel].File, Stats);
	if(Stats.Unlocked == 0)
		return;

	PlayState.SetTestLevel("");
	PlayState.SetCampaign(CampaignIndex);
	PlayState.SetCampaignLevel(SelectedLevel);
	Game.ChangeState(&PlayState);
}

// Launchs a replay from a list item
void _Menu::LaunchReplay() {

	// Get replay file
	std::string File = GetReplayFile();
	if(File != "") {

		// Load replay
		ViewReplayState.SetCurrentReplay(File);
		Game.ChangeState(&ViewReplayState);
	}
}

// Add a regular menu button
IGUIButton *_Menu::AddMenuButton(const irr::core::recti &Rectangle, int ID, const wchar_t *Text, _Interface::ImageType ButtonImage) {
	IGUIButton *Button = irrGUI->addButton(Rectangle, CurrentLayout, ID, Text);
	Button->setImage(Interface.GetImage(ButtonImage));
	Button->setUseAlphaChannel(true);
	Button->setDrawBorder(false);
	Button->setOverrideFont(Interface.GetFont(_Interface::FONT_BUTTON));

	return Button;
}

// Add menu text label
IGUIStaticText *_Menu::AddMenuText(const position2di &CenterPosition, const wchar_t *Text, _Interface::FontType Font, int ID, EGUI_ALIGNMENT HorizontalAlign) {
	
	// Get text dimensions
	dimension2du Size = Interface.GetFont(Font)->getDimension(Text);
	
	recti Rectangle;
	switch(HorizontalAlign) {
		case EGUIA_UPPERLEFT:
			Rectangle = Interface.GetRect(CenterPosition.X, CenterPosition.Y, Size.Width, Size.Height);
		break;
		case EGUIA_CENTER:
			Rectangle = Interface.GetCenteredRect(CenterPosition.X, CenterPosition.Y, Size.Width, Size.Height);
		break;
		case EGUIA_LOWERRIGHT:
			Rectangle = Interface.GetRightRect(CenterPosition.X, CenterPosition.Y, Size.Width, Size.Height);
		break;
	}
	
	// Add text
	IGUIStaticText *NewText = irrGUI->addStaticText(Text, Rectangle, false, false, CurrentLayout);
	NewText->setOverrideFont(Interface.GetFont(Font));
	
	return NewText;
}

// Clear out the current menu layout
void _Menu::ClearCurrentLayout() {
	if(CurrentLayout) {
		irrGUI->setFocus(0);
		CurrentLayout->remove();
	}
	
	CurrentLayout = irrGUI->addModalScreen(0);
	CurrentLayout->setVisible(false);
	CurrentLayout->setEnabled(false);
}