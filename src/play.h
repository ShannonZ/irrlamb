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
#include <stdafx.h>
#pragma once

// Libraries
#include "engine/state.h"

// Forward Declarations
class _Object;
class _Player;
class _Camera;
struct SaveLevelStruct;

// Classes
class _PlayState : public _State {

	public:

		enum StateType {
			STATE_PLAY,
			STATE_PAUSED,
			STATE_SAVEREPLAY,
			STATE_LOSE,
			STATE_WIN
		};

		enum GUIElements {
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

		int Init();
		int Close();

		bool HandleKeyPress(int Key);
		void HandleMouseMotion(float UpdateX, float UpdateY);
		bool HandleMousePress(int Button, int MouseX, int MouseY);
		void HandleMouseLift(int Button, int MouseX, int MouseY);
		void HandleMouseWheel(float Direction);
		void HandleGUI(int EventType, irr::gui::IGUIElement *Element);

		void Update(float FrameTime);
		void UpdateRender(float TimeStepRemainder);
		void Draw();
		
		void StartReset();
		void ResetLevel();
		void InitLose();
		void InitWin();

		void SetTestLevel(const std::string &Level) { TestLevel = Level; }
		void SetCampaign(int Value) { CurrentCampaign = Value; }
		void SetCampaignLevel(int Value) { CampaignLevel = Value; }

		_Camera *GetCamera() { return Camera; }
		float GetTimer() { return Timer; }

	private:

		void InitPlay();
		void InitPause();
		void InitSaveReplay();
		void SaveReplay();

		// States
		StateType State, TargetState;
		std::string TestLevel;
		float Timer;
		bool Resetting;

		// HUD
		bool ShowHUD;

		// Campaign
		int CurrentCampaign, CampaignLevel;

		// High scores
		const SaveLevelStruct *WinStats;

		// Objects
		_Player *Player;
		_Camera *Camera;
};

extern _PlayState PlayState;
