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
#include <engine/namespace.h>

_Menu Menu;

const int CAMPAIGN_LEVELID = 1000;
const int PLAY_CAMPAIGNID = 900;

const int WIN_WIDTH = 430;
const int WIN_HEIGHT = 350;

// List of action names
const wchar_t *_Menu::ActionNames[_Actions::COUNT] = {
	L"Move Left",
	L"Move Right",
	L"Move Forward",
	L"Move Back",
	L"Jump",
	L"Restart Level",
};

enum GUIElements {
	MAIN_SINGLEPLAYER,
	MAIN_REPLAYS,
	MAIN_OPTIONS,
	MAIN_QUIT,
	SINGLEPLAYER_BACK,
	LEVELS_GO,
	LEVELS_BUY,
	LEVELS_HIGHSCORES,
	LEVELS_BACK,
	LEVELS_SELECTEDLEVEL,
	LEVELINFO_DESCRIPTION,
	LEVELINFO_ATTEMPTS,
	LEVELINFO_WINS,
	LEVELINFO_LOSSES,
	LEVELINFO_PLAYTIME,
	LEVELINFO_BESTTIME,
	REPLAYS_FILES,
	REPLAYS_GO,
	REPLAYS_DELETE,
	REPLAYS_BACK,
	OPTIONS_VIDEO,
	OPTIONS_AUDIO,
	OPTIONS_CONTROLS,
	OPTIONS_BACK,
	VIDEO_SAVE,
	VIDEO_CANCEL,
	VIDEO_VIDEOMODES,
	VIDEO_FULLSCREEN,
	VIDEO_SHADOWS,
	VIDEO_SHADERS,
	AUDIO_ENABLED,
	AUDIO_SAVE,
	AUDIO_CANCEL,
	CONTROLS_SAVE,
	CONTROLS_CANCEL,
	CONTROLS_INVERTMOUSE,
	CONTROLS_MOVEFORWARD,
	CONTROLS_MOVEBACK,
	CONTROLS_MOVELEFT,
	CONTROLS_MOVERIGHT,
	CONTROLS_MOVERESET,
	CONTROLS_MOVEJUMP,
	PAUSE_RESUME,
	PAUSE_SAVEREPLAY,
	PAUSE_RESTART,
	PAUSE_MAINMENU,
	SAVEREPLAY_NAME,
	SAVEREPLAY_SAVE,
	SAVEREPLAY_CANCEL,
	LOSE_RESTARTLEVEL,
	LOSE_SAVEREPLAY,
	LOSE_MAINMENU,
	WIN_RESTARTLEVEL,
	WIN_NEXTLEVEL,
	WIN_SAVEREPLAY,
	WIN_MAINMENU,
};

// Handle action inputs
void _Menu::HandleAction(int Action, float Value) {
	if(Input.HasJoystick())
		Input.DriveMouse(Action, Value);

	// On action press
	if(Value) {
		switch(Action) {
			case _Actions::MENU_BACK:
				switch(State) {
					case STATE_MAIN:
						Game.SetDone(true);
					break;
					case STATE_SINGLEPLAYER:
					case STATE_OPTIONS:
					case STATE_REPLAYS:
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
				}
			break;
			case STATE_SAVEREPLAY:
			if(Input.HasJoystick())
				Input.DriveMouse(Action, Value);
		break;
		case STATE_LOSE:
		case STATE_WIN:
			if(Input.HasJoystick())
				Input.DriveMouse(Action, Value);

			switch(Action) {
				case _Actions::RESET:
					if(Value)
						PlayState.StartReset();
				break;
			}
		break;
		case STATE_PAUSED:
			if(Input.HasJoystick())
				Input.DriveMouse(Action, Value);
			
			if(!Value)
				return;

			switch(Action) {
				case _Actions::MENU_PAUSE:
					Menu.InitPlay();
				break;
			}
		break;
		}
	}
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
							IGUIButton *SwapButton = static_cast<IGUIButton *>(irrGUI->getRootGUIElement()->getElementFromId(CONTROLS_MOVEFORWARD + i));

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
			else {
				switch(Key) {
					case KEY_ESCAPE:
						InitOptions();
					break;
					default:
						Processed = false;
					break;
				}
			}
		break;
		case STATE_SAVEREPLAY:
			switch(Key) {
				case KEY_ESCAPE:
					Menu.InitPause();
				break;
				case KEY_RETURN:
					Menu.SaveReplay();
				break;
				default:
					Processed = false;
				break;
			}
		break;
		case STATE_LOSE:
			switch(Key) {
				case KEY_ESCAPE:
					//Game.ChangeState(&Menu);
				break;
				default:
					Processed = false;
				break;
			}
		break;
		case STATE_WIN:
			switch(Key) {
				case KEY_ESCAPE:
					//Game.ChangeState(&Menu);
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
					IGUIListBox *ReplayList = static_cast<IGUIListBox *>(irrGUI->getRootGUIElement()->getElementFromId(REPLAYS_FILES));
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
					InitMain();
				break;
				case VIDEO_SAVE: {
					
					// Save the video mode
					IGUIComboBox *VideoModes = static_cast<IGUIComboBox *>(irrGUI->getRootGUIElement()->getElementFromId(VIDEO_VIDEOMODES));
					if(VideoModes != NULL) {
						VideoModeStruct Mode = Graphics.GetVideoModes()[VideoModes->getSelected()];
						Config.ScreenWidth = Mode.Width;
						Config.ScreenHeight = Mode.Height;
					}

					// Save full screen
					IGUICheckBox *Fullscreen = static_cast<IGUICheckBox *>(irrGUI->getRootGUIElement()->getElementFromId(VIDEO_FULLSCREEN));
					Config.Fullscreen = Fullscreen->isChecked();

					// Save shadows
					IGUICheckBox *Shadows = static_cast<IGUICheckBox *>(irrGUI->getRootGUIElement()->getElementFromId(VIDEO_SHADOWS));
					Config.Shadows = Shadows->isChecked();

					/*// Save shaders
					IGUICheckBox *Shaders = static_cast<IGUICheckBox *>(irrGUI->getRootGUIElement()->getElementFromId(VIDEO_SHADERS));
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
					IGUICheckBox *AudioEnabled = static_cast<IGUICheckBox *>(irrGUI->getRootGUIElement()->getElementFromId(AUDIO_ENABLED));
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
					IGUICheckBox *InvertMouse = static_cast<IGUICheckBox *>(irrGUI->getRootGUIElement()->getElementFromId(CONTROLS_INVERTMOUSE));
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
				case PAUSE_RESTART:
					PlayState.StartReset();
				break;
				case PAUSE_MAINMENU:
					if(PlayState.TestLevel == "")
						Menu.InitLevels();
					//Game.ChangeState(&Menu);
				break;
				case SAVEREPLAY_SAVE:
					SaveReplay();
				break;
				case SAVEREPLAY_CANCEL:
					/*
					if(PlayState.TargetState == STATE_WIN)
						InitWin();
					else if(PlayState.TargetState == STATE_LOSE)
						InitLose();
					else
						InitPause();
						*/
				break;
				case LOSE_RESTARTLEVEL:
					PlayState.StartReset();
				break;
				case LOSE_SAVEREPLAY:
					InitSaveReplay();
				break;
				case LOSE_MAINMENU:
					if(PlayState.TestLevel == "")
						Menu.InitLevels();
					//Game.ChangeState(&Menu);
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
					if(PlayState.TestLevel == "")
						Menu.InitLevels();
					//Game.ChangeState(&Menu);
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

void _Menu::InitMain() {
	Interface.ChangeSkin(_Interface::SKIN_MENU);
	Input.SetMouseLocked(false);

	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2;
	Interface.Clear();

	// Logo
	irrGUI->addImage(irrDriver->getTexture("art/logo.jpg"), position2di(CenterX - 256, CenterY - 215));
	IGUIStaticText *TextVersion = irrGUI->addStaticText(stringw(GAME_VERSION).c_str(), Interface.GetCenteredRect(40, irrDriver->getScreenSize().Height - 20, 50, 15), false, false);

	// Button
	int Y = CenterY - 50;
	IGUIButton *ButtonSinglePlayer = irrGUI->addButton(Interface.GetCenteredRect(CenterX, Y, 130, 34), 0, MAIN_SINGLEPLAYER, L"Single Player");
	IGUIButton *ButtonReplays = irrGUI->addButton(Interface.GetCenteredRect(CenterX, Y + 50, 130, 34), 0, MAIN_REPLAYS, L"Replays");
	IGUIButton *ButtonOptions = irrGUI->addButton(Interface.GetCenteredRect(CenterX, Y + 100, 130, 34), 0, MAIN_OPTIONS, L"Options");
	IGUIButton *ButtonQuit = irrGUI->addButton(Interface.GetCenteredRect(CenterX, Y + 150, 130, 34), 0, MAIN_QUIT, L"Quit");
	ButtonSinglePlayer->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonSinglePlayer->setUseAlphaChannel(true);
	ButtonSinglePlayer->setDrawBorder(false);
	ButtonReplays->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonReplays->setUseAlphaChannel(true);
	ButtonReplays->setDrawBorder(false);
	ButtonOptions->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonOptions->setUseAlphaChannel(true);
	ButtonOptions->setDrawBorder(false);
	ButtonQuit->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonQuit->setUseAlphaChannel(true);
	ButtonQuit->setDrawBorder(false);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	State = STATE_MAIN;
}

void _Menu::InitSinglePlayer() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();

	// Reset menu variables
	CampaignIndex = 0;
	SelectedLevel = -1;

	// Text
	X = CenterX, Y = CenterY - 150;
	IGUIStaticText *Text = irrGUI->addStaticText(L"Level Sets", Interface.GetCenteredRect(X, Y, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Campaigns
	Y += 50;
	const std::vector<CampaignStruct> &Campaigns = Campaign.GetCampaigns();
	for(u32 i = 0; i < Campaigns.size(); i++) {
		irr::core::stringw Name(Campaigns[i].Name.c_str());
		IGUIButton *Button = irrGUI->addButton(Interface.GetCenteredRect(X, Y, 130, 34), 0, PLAY_CAMPAIGNID + i, Name.c_str());
		Button->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
		Button->setUseAlphaChannel(true);
		Button->setDrawBorder(false);

		Y += 40;
	}

	Y += 50;
	IGUIButton *BackButton = irrGUI->addButton(Interface.GetCenteredRect(X, Y, 130, 34), 0, SINGLEPLAYER_BACK, L"Back");
	BackButton->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	BackButton->setUseAlphaChannel(true);
	BackButton->setDrawBorder(false);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	State = STATE_SINGLEPLAYER;
}

void _Menu::InitLevels() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();
	LevelStats.clear();
	SelectedLevel = -1;
	X = CenterX, Y = CenterY - 190;

	// Text
	IGUIStaticText *Text = irrGUI->addStaticText(L"Levels", Interface.GetCenteredRect(X, Y, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Add level list
	X = CenterX - 160;
	Y += 60;
	int Column = 0, Row = 0;
	const CampaignStruct &CampaignData = Campaign.GetCampaign(CampaignIndex);
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
		IGUIButton *Level = irrGUI->addButton(Interface.GetCenteredRect(X + Column * 80, Y + Row * 80, 64, 64), 0, CAMPAIGN_LEVELID + i);

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
	X = CenterX;
	Y = CenterY + 180;
	IGUIButton *ButtonBack = irrGUI->addButton(Interface.GetCenteredRect(X, Y, 82, 34), 0, LEVELS_BACK, L"Back");
	ButtonBack->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonBack->setUseAlphaChannel(true);
	ButtonBack->setDrawBorder(false);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	State = STATE_LEVELS;
}

void _Menu::InitReplays() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();
	char Buffer[256];

	// Text
	X = CenterX, Y = CenterY - 180;
	IGUIStaticText *Text = irrGUI->addStaticText(L"Replays", Interface.GetCenteredRect(X, Y, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Level selection
	Y += 160;
	IGUIListBox *ListReplays = irrGUI->addListBox(Interface.GetCenteredRect(X, Y, 450, 250), 0, REPLAYS_FILES, true);

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
	IGUIButton *ButtonGo = irrGUI->addButton(Interface.GetCenteredRect(X - 123, Y, 102, 34), 0, REPLAYS_GO, L"View");
	IGUIButton *ButtonDelete = irrGUI->addButton(Interface.GetCenteredRect(X, Y, 102, 34), 0, REPLAYS_DELETE, L"Delete");
	IGUIButton *ButtonBack = irrGUI->addButton(Interface.GetCenteredRect(X + 123, Y, 102, 34), 0, REPLAYS_BACK, L"Back");
	ButtonGo->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonGo->setUseAlphaChannel(true);
	ButtonGo->setDrawBorder(false);
	ButtonDelete->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonDelete->setUseAlphaChannel(true);
	ButtonDelete->setDrawBorder(false);
	ButtonBack->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonBack->setUseAlphaChannel(true);
	ButtonBack->setDrawBorder(false);

	// Play sound
	if(!FirstStateLoad)
		Interface.PlaySound(_Interface::SOUND_CONFIRM);
	FirstStateLoad = false;

	State = STATE_REPLAYS;
}

void _Menu::InitOptions() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2;
	Interface.Clear();
			
	// Text
	IGUIStaticText *Text = irrGUI->addStaticText(L"Options", Interface.GetCenteredRect(CenterX, CenterY - 120, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Buttons
	IGUIButton *ButtonVideo = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY - 50, 130, 34), 0, OPTIONS_VIDEO, L"Video");
	IGUIButton *ButtonAudio = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY, 130, 34), 0, OPTIONS_AUDIO, L"Audio");
	IGUIButton *ButtonControls = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY + 50, 130, 34), 0, OPTIONS_CONTROLS, L"Controls");
	IGUIButton *ButtonBack = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY + 100, 130, 34), 0, OPTIONS_BACK, L"Back");
	ButtonVideo->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonVideo->setUseAlphaChannel(true);
	ButtonVideo->setDrawBorder(false);
	ButtonAudio->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonAudio->setUseAlphaChannel(true);
	ButtonAudio->setDrawBorder(false);
	ButtonControls->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonControls->setUseAlphaChannel(true);
	ButtonControls->setDrawBorder(false);
	ButtonBack->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonBack->setUseAlphaChannel(true);
	ButtonBack->setDrawBorder(false);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	State = STATE_OPTIONS;
}

void _Menu::InitVideo() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();

	// Text
	X = CenterX, Y = CenterY - 150;
	IGUIStaticText *Text = irrGUI->addStaticText(L"Video", Interface.GetCenteredRect(X, Y, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Video modes
	Y += 40;
	const std::vector<VideoModeStruct> &ModeList = Graphics.GetVideoModes();
	if(ModeList.size() > 0) {
		IGUIStaticText *TextScreenResolution = irrGUI->addStaticText(L"Screen Resolution", Interface.GetCenteredRect(X - 65, Y, 110, 25));
		TextScreenResolution->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
		IGUIComboBox *ListScreenResolution = irrGUI->addComboBox(Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, VIDEO_VIDEOMODES);

		// Populate mode list
		for(u32 i = 0; i < ModeList.size(); i++)
			ListScreenResolution->addItem(ModeList[i].String.c_str());
		ListScreenResolution->setSelected(Graphics.GetCurrentVideoModeIndex());
	}
				
	// Full Screen
	Y += 30;
	IGUIStaticText *TextFullscreen = irrGUI->addStaticText(L"Fullscreen", Interface.GetCenteredRect(X - 65, Y, 110, 25));
	TextFullscreen->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxFullscreen = irrGUI->addCheckBox(Config.Fullscreen, Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, VIDEO_FULLSCREEN);

	// Shadows
	Y += 30;
	IGUIStaticText *TextShadows = irrGUI->addStaticText(L"Shadows", Interface.GetCenteredRect(X - 65, Y, 110, 25), false, false);
	TextShadows->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxShadows = irrGUI->addCheckBox(Config.Shadows, Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, VIDEO_SHADOWS);
/*
	// Shaders
	Y += 30;
	IGUIStaticText *TextShaders = irrGUI->addStaticText(L"Shaders", Interface.GetCenteredRect(X - 65, Y, 110, 25), false, false);
	TextShaders->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxShaders = irrGUI->addCheckBox(Config.Shaders, Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, VIDEO_SHADERS);
	if(!Graphics.GetShadersSupported())
		CheckBoxShaders->setEnabled(false);
*/
	// Save
	Y += 60;
	IGUIButton *ButtonSave = irrGUI->addButton(Interface.GetCenteredRect(X - 50, Y, 82, 34), 0, VIDEO_SAVE, L"Save");
	IGUIButton *ButtonCancel = irrGUI->addButton(Interface.GetCenteredRect(X + 50, Y, 82, 34), 0, VIDEO_CANCEL, L"Cancel");
	ButtonSave->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonSave->setUseAlphaChannel(true);
	ButtonSave->setDrawBorder(false);
	ButtonCancel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonCancel->setUseAlphaChannel(true);
	ButtonCancel->setDrawBorder(false);

	// Warning
	Y += 40;
	IGUIStaticText *TextWarning = irrGUI->addStaticText(L"Some changes are applied after restart", Interface.GetCenteredRect(X, Y, 250, 25), false, false, 0, -1, true);
	TextWarning->setTextAlignment(EGUIA_CENTER, EGUIA_CENTER);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	State = STATE_VIDEO;
}

void _Menu::InitAudio() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();

	// Text
	X = CenterX, Y = CenterY - 150;
	IGUIStaticText *Text = irrGUI->addStaticText(L"Audio", Interface.GetCenteredRect(X, Y, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Sound enabled
	Y += 60;
	IGUIStaticText *TextAudioEnabled = irrGUI->addStaticText(L"Audio Enabled", Interface.GetCenteredRect(X - 65, Y, 110, 25));
	TextAudioEnabled->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxAudioEnabled = irrGUI->addCheckBox(Config.AudioEnabled, Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, AUDIO_ENABLED);

	// Save
	Y += 90;
	IGUIButton *ButtonSave = irrGUI->addButton(Interface.GetCenteredRect(X - 50, Y, 82, 34), 0, AUDIO_SAVE, L"Save");
	IGUIButton *ButtonCancel = irrGUI->addButton(Interface.GetCenteredRect(X + 50, Y, 82, 34), 0, AUDIO_CANCEL, L"Cancel");
	ButtonSave->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonSave->setUseAlphaChannel(true);
	ButtonSave->setDrawBorder(false);
	ButtonCancel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonCancel->setUseAlphaChannel(true);
	ButtonCancel->setDrawBorder(false);

	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	State = STATE_AUDIO;
}

void _Menu::InitControls() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	Interface.Clear();
	KeyButton = NULL;

	// Text
	IGUIStaticText *Text = irrGUI->addStaticText(L"Controls", Interface.GetCenteredRect(CenterX, CenterY - 160, 150, 40), false, false);
	Text->setOverrideFont(Interface.GetFont(_Interface::FONT_LARGE));
	Text->setTextAlignment(EGUIA_CENTER, EGUIA_UPPERLEFT);

	// Create the key buttons
	X = CenterX;
	Y = CenterY - 110;
	for(int i = 0; i <= _Actions::RESET; i++) {
				
		CurrentKeys[i] = Config.Keys[i];
		IGUIStaticText *Text = irrGUI->addStaticText(ActionNames[i], Interface.GetCenteredRect(X - 50, Y, 80, 20), false, false);
		Text->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT);
		IGUIButton *Button = irrGUI->addButton(Interface.GetCenteredRect(X + 50, Y, 82, 34), 0, CONTROLS_MOVEFORWARD + i, stringw(Input.GetKeyName(CurrentKeys[i])).c_str());
		Button->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
		Button->setUseAlphaChannel(true);
		Button->setDrawBorder(false);

		Y += 35;
	}

	// Invert mouse
	Y += 5;
	IGUIStaticText *TextInvertMouse = irrGUI->addStaticText(L"Invert Mouse", Interface.GetCenteredRect(X - 65, Y, 110, 25));
	TextInvertMouse->setTextAlignment(EGUIA_LOWERRIGHT, EGUIA_CENTER);
	IGUICheckBox *CheckBoxInvertMouse = irrGUI->addCheckBox(Config.InvertMouse, Interface.GetCenteredRect(X + 60, Y, 100, 25), 0, CONTROLS_INVERTMOUSE);

	// Save
	IGUIButton *ButtonSave = irrGUI->addButton(Interface.GetCenteredRect(CenterX - 50, CenterY + 150, 82, 34), 0, CONTROLS_SAVE, L"Save");
	IGUIButton *ButtonCancel = irrGUI->addButton(Interface.GetCenteredRect(CenterX + 50, CenterY + 150, 82, 34), 0, CONTROLS_CANCEL, L"Cancel");
	ButtonSave->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonSave->setUseAlphaChannel(true);
	ButtonSave->setDrawBorder(false);
	ButtonCancel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonCancel->setUseAlphaChannel(true);
	ButtonCancel->setDrawBorder(false);
	
	// Play sound
	Interface.PlaySound(_Interface::SOUND_CONFIRM);

	State = STATE_CONTROLS;
}

// Init play GUI
void _Menu::InitPlay() {
	irrGUI->clear();

	Graphics.SetClearColor(SColor(255, 0, 0, 0));
	Input.SetMouseLocked(true);

	State = STATE_NONE;
}

// Draws the pause menu
void _Menu::InitPause() {
	irrGUI->clear();
	
	// Draw interface
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2;
	IGUIButton *ButtonResume = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY - 75, 130, 34), 0, PAUSE_RESUME, L"Resume");
	IGUIButton *ButtonSaveReplay = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY - 25, 130, 34), 0, PAUSE_SAVEREPLAY, L"Save Replay");
	IGUIButton *ButtonRestart = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY + 25, 130, 34), 0, PAUSE_RESTART, L"Restart Level");
	IGUIButton *ButtonMainMenu = irrGUI->addButton(Interface.GetCenteredRect(CenterX, CenterY + 75, 130, 34), 0, PAUSE_MAINMENU, L"Main Menu");
	ButtonResume->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonResume->setUseAlphaChannel(true);
	ButtonResume->setDrawBorder(false);
	ButtonSaveReplay->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonSaveReplay->setUseAlphaChannel(true);
	ButtonSaveReplay->setDrawBorder(false);
	ButtonRestart->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonRestart->setUseAlphaChannel(true);
	ButtonRestart->setDrawBorder(false);
	ButtonMainMenu->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON128));
	ButtonMainMenu->setUseAlphaChannel(true);
	ButtonMainMenu->setDrawBorder(false);
	
	Input.SetMouseLocked(false);

	State = STATE_PAUSED;
}

// Draws the save replay GUI
void _Menu::InitSaveReplay() {
	irrGUI->clear();

	// Draw interface
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2;
	IGUIEditBox *EditName = irrGUI->addEditBox(L"", Interface.GetCenteredRect(CenterX, CenterY - 20, 172, 32), true, 0, SAVEREPLAY_NAME);
	IGUIButton *ButtonSave = irrGUI->addButton(Interface.GetCenteredRect(CenterX - 45, CenterY + 20, 82, 34), 0, SAVEREPLAY_SAVE, L"Save");
	IGUIButton *ButtonCancel = irrGUI->addButton(Interface.GetCenteredRect(CenterX + 45, CenterY + 20, 82, 34), 0, SAVEREPLAY_CANCEL, L"Cancel");
	ButtonSave->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonSave->setUseAlphaChannel(true);
	ButtonSave->setDrawBorder(false);
	ButtonCancel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON80));
	ButtonCancel->setUseAlphaChannel(true);
	ButtonCancel->setDrawBorder(false);

	irrGUI->setFocus(EditName);
	EditName->setMax(32);

	State = STATE_SAVEREPLAY;
}

// Draws the lose screen
void _Menu::InitLose() {
/*	
	// Update stats
	if(TestLevel == "") {
		Save.IncrementLevelLoseCount(Level.GetLevelFile());
		Save.SaveLevelStats();
	}

	// Draw interface
	irrGUI->clear();
	IGUIStaticText *TextWin = irrGUI->addStaticText(L"You Lose", Interface.GetCenteredTextRect(L"You Lose", 0.15f, 0.20f), false, false, 0, -1, false);
	IGUIButton *ButtonRestartLevel = irrGUI->addButton(Interface.GetAbsoluteRectWH(0.4f, 0.3f, 0.2f, 0.05f), 0, LOSE_RESTARTLEVEL, L"Restart Level");
	IGUIButton *ButtonSaveReplay = irrGUI->addButton(Interface.GetAbsoluteRectWH(0.4f, 0.4f, 0.2f, 0.05f), 0, LOSE_SAVEREPLAY, L"Save Replay");
	IGUIButton *ButtonMainMenu = irrGUI->addButton(Interface.GetAbsoluteRectWH(0.4f, 0.5f, 0.2f, 0.05f), 0, LOSE_MAINMENU, L"Main Menu");

	Input.SetMouseLocked(false);
*/
	State = STATE_LOSE;
}

// Draws the win screen
void _Menu::InitWin() {
	
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
	Interface.Clear();

	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2, X, Y;
	X = CenterX;
	Y = CenterY + WIN_HEIGHT / 2 + 25;
	IGUIButton *ButtonRestartLevel = irrGUI->addButton(Interface.GetCenteredRect(X - 165, Y, 102, 34), 0, WIN_RESTARTLEVEL, L"Retry Level");
	IGUIButton *ButtonNextLevel = irrGUI->addButton(Interface.GetCenteredRect(X - 55, Y, 102, 34), 0, WIN_NEXTLEVEL, L"Next Level");
	IGUIButton *ButtonSaveReplay = irrGUI->addButton(Interface.GetCenteredRect(X + 55, Y, 102, 34), 0, WIN_SAVEREPLAY, L"Save Replay");
	IGUIButton *ButtonMainMenu = irrGUI->addButton(Interface.GetCenteredRect(X + 165, Y, 102, 34), 0, WIN_MAINMENU, L"Main Menu");
	ButtonRestartLevel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonRestartLevel->setUseAlphaChannel(true);
	ButtonRestartLevel->setDrawBorder(false);
	ButtonNextLevel->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonNextLevel->setUseAlphaChannel(true);
	ButtonNextLevel->setDrawBorder(false);
	ButtonSaveReplay->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonSaveReplay->setUseAlphaChannel(true);
	ButtonSaveReplay->setDrawBorder(false);
	ButtonMainMenu->setImage(Interface.GetImage(_Interface::IMAGE_BUTTON100));
	ButtonMainMenu->setUseAlphaChannel(true);
	ButtonMainMenu->setDrawBorder(false);
	
	if(LastLevelInCampaign)
		ButtonNextLevel->setEnabled(false);

	Input.SetMouseLocked(false);

	State = STATE_WIN;
}

// Saves a replay
void _Menu::SaveReplay() {

	IGUIEditBox *EditName = static_cast<IGUIEditBox *>(irrGUI->getRootGUIElement()->getElementFromId(SAVEREPLAY_NAME));
	if(EditName != NULL) {
		irr::core::stringc ReplayTitle(EditName->getText());
		Replay.SaveReplay(ReplayTitle.c_str());
	}

/*	switch(PlayState.TargetState) {
		case STATE_WIN: {
			InitWin();
			
			IGUIButton *ButtonSaveReplay = static_cast<IGUIButton *>(irrGUI->getRootGUIElement()->getElementFromId(WIN_SAVEREPLAY));
			ButtonSaveReplay->setEnabled(false);
		}
		break;
		case STATE_LOSE: {
			InitLose();
			
			IGUIButton *ButtonSaveReplay = static_cast<IGUIButton *>(irrGUI->getRootGUIElement()->getElementFromId(LOSE_SAVEREPLAY));
			ButtonSaveReplay->setEnabled(false);
		}
		break;
		default:
			InitPause();
		break;
	}*/
}

// Updates the current state
void _Menu::Update(float FrameTime) {
}

// Draws the current state
void _Menu::Draw() {
	irrGUI->drawAll();

	// Draw level tooltip
	if(State == STATE_LEVELS) {

		if(SelectedLevel != -1) {
			char Buffer[256];
			const SaveLevelStruct *Stats = LevelStats[SelectedLevel];
			const std::string &NiceName = Campaign.GetLevelNiceName(CampaignIndex, SelectedLevel);

			// Get box position
			int Width = 250, Height = 305, X, Y;
			int Left = (int)Input.GetMouseX() + 20;
			int Top = (int)Input.GetMouseY() - 105;

			// Cap limits
			if(Top < 10)
				Top = 10;
			if(Left + Width > (int)irrDriver->getScreenSize().Width - 10)
				Left -= Width + 35;

			// Draw box
			Interface.DrawTextBox(Left + Width/2, Top + Height/2, Width, Height);
			X = Left + Width/2;
			Y = Top + 10;

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
					int HighX = Left + Width/2 - 80, HighY = Y + 28;

					// Draw header
					Interface.RenderText("#", HighX, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					Interface.RenderText("Time", HighX + 30, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					Interface.RenderText("Date", HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(255, 255, 255, 255));
					HighY += 17;

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
						Interface.RenderText(DateString, HighX + 110, HighY, _Interface::ALIGN_LEFT, _Interface::FONT_SMALL, SColor(200, 255, 255, 255));

						HighY += 17;
					}
				}
			}
			else {
				
				// Locked
				Interface.RenderText("Level Locked", X, Y, _Interface::ALIGN_CENTER, _Interface::FONT_MEDIUM, SColor(255, 255, 255, 255));
			}
		}
	}

	switch(State) {
		case STATE_LOSE:
			Interface.FadeScreen(0.8f);
		break;
		case STATE_WIN: {
			Interface.FadeScreen(0.8f);

			Menu.DrawWinScreen();
		} break;
	}
}


void _Menu::DrawWinScreen() {
	int CenterX = irrDriver->getScreenSize().Width / 2, CenterY = irrDriver->getScreenSize().Height / 2;
	char TimeString[32];
	char Buffer[256];

	// Draw header
	int X = CenterX;
	int Y = CenterY - WIN_HEIGHT / 2 + 15;
	Interface.DrawTextBox(CenterX, CenterY, WIN_WIDTH, WIN_HEIGHT);
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
	int HighX = CenterX - 75, HighY = Y + 48;

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
	IGUIListBox *ReplayList = static_cast<IGUIListBox *>(irrGUI->getRootGUIElement()->getElementFromId(REPLAYS_FILES));
	
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
