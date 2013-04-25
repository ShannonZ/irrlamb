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
#include <play.h>
#include <engine/constants.h>
#include <engine/globals.h>
#include <engine/input.h>
#include <engine/log.h>
#include <engine/graphics.h>
#include <engine/audio.h>
#include <engine/config.h>
#include <engine/physics.h>
#include <engine/scripting.h>
#include <engine/objectmanager.h>
#include <engine/replay.h>
#include <engine/interface.h>
#include <engine/camera.h>
#include <engine/game.h>
#include <engine/level.h>
#include <engine/campaign.h>
#include <engine/fader.h>
#include <engine/actions.h>
#include <engine/save.h>
#include <objects/player.h>
#include <menu.h>
#include <viewreplay.h>
#include <engine/namespace.h>

_PlayState PlayState;

// Initializes the state
int _PlayState::Init() {
	Timer = 0.0f;
	Physics.SetEnabled(true);
	Interface.ChangeSkin(_Interface::SKIN_GAME);

	// Add camera
	Camera = new _Camera();

	// Load the level
	std::string LevelFile;
	
	// Select a level to load
	if(TestLevel != "")
		LevelFile = TestLevel;
	else
		LevelFile = Campaign.GetLevel(CurrentCampaign, CampaignLevel);

	// Load level
	if(!Level.Init(LevelFile))
		return 0;

	// Reset level
	ResetLevel();

	return 1;
}

// Shuts the state down
int _PlayState::Close() {

	// Stop the replay
	Replay.StopRecording();

	// Close the system down
	delete Camera;
	Level.Close();
	ObjectManager.ClearObjects();
	Interface.Clear();
	irrScene->clear();

	// Save stats
	if(TestLevel == "") {
		Save.IncrementLevelPlayTime(Level.GetLevelName(), Timer);
		Save.SaveLevelStats(Level.GetLevelName());
	}

	return 1;
}

// Resets the level
void _PlayState::ResetLevel() {

	// Handle saves
	if(TestLevel == "") {
		Save.IncrementLevelLoadCount(Level.GetLevelName());
		Save.IncrementLevelPlayTime(Level.GetLevelName(), Timer);
		Save.SaveLevelStats(Level.GetLevelName());
	}

	// Stop recording
	Replay.StopRecording();

	// Set up GUI
	Menu.InitPlay();
	Interface.Clear();
	Timer = 0.0f;

	// Set up camera
	Camera->SetRotation(0.0f, 30.0f);
	Camera->SetDistance(5.0f);

	// Clear objects
	ObjectManager.ClearObjects();
	Physics.Reset();

	// Start replay recording
	Replay.StartRecording();

	// Load level objects
	Level.SpawnObjects();
	Level.RunScripts();

	// Get the player
	Player = static_cast<_Player *>(ObjectManager.GetObjectByType(_Object::PLAYER));
	if(Player == NULL) {
		Log.Write("_PlayState::ResetLevel - Cannot find player object");
		return;
	}
	Player->SetCamera(Camera);

	// Record camera in replay
	btVector3 Position = Player->GetPosition();
	Camera->Update(vector3df(Position[0], Position[1], Position[2]));
	Camera->RecordReplay();

	// Reset game timer
	Game.ResetTimer();
	Fader.Start(FADE_SPEED);
	Resetting = false;
}

// Handle new actions
bool _PlayState::HandleAction(int InputType, int Action, float Value) {
	if(Resetting)
		return false;
		
	//printf("%d %f\n", Action, Value);
	bool Processed = false;
	if(!IsPaused()) {
		switch(Action) {
			case _Actions::JUMP:
				if(Value)
					Player->Jump();
			break;
			case _Actions::RESET:
				if(Value)
					StartReset();
			break;
			case _Actions::MENU_PAUSE:
				if(!Value)
					return false;

				if(TestLevel != "")
					Game.SetDone(true);
				else
					Menu.InitPause();

				return true;
			break;
			case _Actions::CAMERA_LEFT:
				if(Camera) {
					if(InputType == _Input::JOYSTICK_AXIS)
						Value *= Game.GetLastFrameTime();
					Camera->HandleMouseMotion(-Value, 0);
				}
			break;
			case _Actions::CAMERA_RIGHT:
				if(Camera) {
					if(InputType == _Input::JOYSTICK_AXIS)
						Value *= Game.GetLastFrameTime();
					Camera->HandleMouseMotion(Value, 0);
				}
			break;
			case _Actions::CAMERA_UP:
				if(Camera) {
					if(InputType == _Input::JOYSTICK_AXIS)
						Value *= Game.GetLastFrameTime();
					Camera->HandleMouseMotion(0, -Value);
				}
			break;
			case _Actions::CAMERA_DOWN:
				if(Camera) {
					if(InputType == _Input::JOYSTICK_AXIS)
						Value *= Game.GetLastFrameTime();
					Camera->HandleMouseMotion(0, Value);
				}
			break;
		}
	}
	else {
		Processed = Menu.HandleAction(InputType, Action, Value);
	}

	//printf("action press %d %f\n", Action, Value);

	return Processed;
}

// Key presses
bool _PlayState::HandleKeyPress(int Key) {
	if(Resetting)
		return true;

	bool LuaProcessed = false;
	
	if(Menu.State == _Menu::STATE_NONE) {
		switch(Key) {
			case KEY_F1:
				Menu.InitPause();
			break;
			case KEY_F2:
				Config.InvertMouse = !Config.InvertMouse;
			break;
			case KEY_F3:
				Log.Write("Player: position=%.3f %.3f %.3f", Player->GetPosition()[0], Player->GetPosition()[1], Player->GetPosition()[2]);
			break;
			case KEY_F5:
				Game.ChangeState(&PlayState);
			break;
			case KEY_F11:
				Interface.DrawHUD = !Interface.DrawHUD;
			break;
			case KEY_F12:
				Graphics.SaveScreenshot();
			break;
		}

		// Send key presses to Lua
		LuaProcessed = Scripting.HandleKeyPress(Key);
	}

	bool Processed = Menu.HandleKeyPress(Key);

	return Processed || LuaProcessed;
}

// Mouse buttons
bool _PlayState::HandleMousePress(int Button, int MouseX, int MouseY) {
	if(Resetting)
		return false;

	if(!IsPaused()) {
		Scripting.HandleMousePress(Button, MouseX, MouseY); 
	}

	return false;
}

// Mouse buttons
void _PlayState::HandleMouseLift(int Button, int MouseX, int MouseY) {
	if(Resetting)
		return;
}

// Mouse wheel
void _PlayState::HandleMouseWheel(float Direction) {

}

// GUI events
void _PlayState::HandleGUI(irr::gui::EGUI_EVENT_TYPE EventType, IGUIElement *Element) {
	if(Resetting)
		return;

	Menu.HandleGUI(EventType, Element);
}

// Updates the current state
void _PlayState::Update(float FrameTime) {

	if(Resetting) {
		if(Fader.IsDoneFading()) {
			ResetLevel();
		}

		return;
	}

	// Check if paused
	if(!IsPaused()) {

		// Update time
		Timer += FrameTime;

		// Update replay
		Replay.Update(FrameTime);

		// Update game logic
		ObjectManager.BeginFrame();

		Player->HandleInput();
		Physics.Update(FrameTime);
		ObjectManager.Update(FrameTime);
		Interface.Update(FrameTime);
		Scripting.UpdateTimedCallbacks();

		ObjectManager.EndFrame();

		// Update audio
		const btVector3 &Position = Player->GetPosition();
		Audio.SetPosition(Position[0], Position[1], Position[2]);

		// Update camera for replay
		Camera->Update(vector3df(Position[0], Position[1], Position[2]));
		Camera->RecordReplay();

		Replay.ResetNextPacketTimer();
	}
}

// Interpolate object positions
void _PlayState::UpdateRender(float TimeStepRemainder) {
	if(Resetting)
		return;

	if(!IsPaused()) {
		Physics.GetWorld()->setTimeStepRemainder(TimeStepRemainder);
		Physics.GetWorld()->synchronizeMotionStates();

		// Set camera position
		btVector3 Position = Player->GetGraphicsPosition();
		Camera->Update(vector3df(Position[0], Position[1], Position[2]));
	}
}

// Draws the current state
void _PlayState::Draw() {
	
	// Draw interface elements
	Interface.Draw(Timer);

	// Darken the screen
	if(IsPaused())
		Interface.FadeScreen(FADE_AMOUNT);

	// Draw irrlicht GUI
	Menu.Draw();
}

// Returns true if the game is and paused
bool _PlayState::IsPaused() {

	return Menu.State != _Menu::STATE_NONE;
}

// Start resetting the level
void _PlayState::StartReset() {
	if(Resetting)
		return;
		
	Fader.Start(-FADE_SPEED);
	Resetting = true;
}

