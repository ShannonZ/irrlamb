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
#include <tinyxml/tinyxml2.h>
#include <engine/config.h>
#include <engine/globals.h>
#include <engine/save.h>
#include <engine/input.h>
#include <engine/namespace.h>

_Config Config;

using namespace tinyxml2;
 
// Initializes the config system
int _Config::Init() {

	Reset();

	return 1;
}

// Closes the config system
int _Config::Close() {
	
	return 1;
}

// Resets the configuration to the default values
void _Config::Reset() {
	
	// Video
	DriverType = EDT_OPENGL;
	ScreenWidth = 800;
	ScreenHeight = 600;
	Fullscreen = false;
	Shadows = true;
	Shaders = true;
	TrilinearFiltering = true;
	AnisotropicFiltering = 0;
	AntiAliasing = 0;

	// Audio
	AudioEnabled = 1;
	SoundVolume = 1.0;
	MusicVolume = 1.0;

	// Input
	JoystickEnabled = true;

	// Set up mapping
	AddDefaultActionMap(true);

	MouseScaleX = 1.0f;
	MouseScaleY = 1.0f;

	InvertMouse = false;
	InvertGamepadY = false; 

	// Replays
	ReplayInterval = 0.02f;

#ifdef PANDORA
	DriverType = EDT_OGLES1;
	ScreenHeight = 480;
	Fullscreen = true;
	Shaders = false;
	TrilinearFiltering = false;
	MouseScaleX = 5.0f;
	MouseScaleY = 5.0f;
#endif
}

// Add default actions
void _Config::AddDefaultActionMap(bool Force) {
	
	if(Force) {
		for(int i = 0; i < _Input::INPUT_COUNT; i++)
			Actions.ClearMappings(i);
	}
	
#ifdef PANDORA
	Actions.AddInputMap(_Input::KEYBOARD, KEY_UP, _Actions::MOVE_FORWARD);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_DOWN, _Actions::MOVE_BACK);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_LEFT, _Actions::MOVE_LEFT);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_RIGHT, _Actions::MOVE_RIGHT);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_RSHIFT, _Actions::JUMP);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_RCONTROL, _Actions::JUMP);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_NEXT, _Actions::JUMP);
#else
	Actions.AddInputMap(_Input::KEYBOARD, KEY_KEY_E, _Actions::MOVE_FORWARD);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_KEY_D, _Actions::MOVE_BACK);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_KEY_S, _Actions::MOVE_LEFT);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_KEY_F, _Actions::MOVE_RIGHT);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_SPACE, _Actions::JUMP);
#endif
	Actions.AddInputMap(_Input::KEYBOARD, KEY_KEY_X, _Actions::RESET);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_ESCAPE, _Actions::MENU_PAUSE);
	Actions.AddInputMap(_Input::KEYBOARD, KEY_ESCAPE, _Actions::MENU_BACK);
	Actions.AddInputMap(_Input::MOUSE_AXIS, 0, _Actions::CAMERA_LEFT);
	Actions.AddInputMap(_Input::MOUSE_AXIS, 1, _Actions::CAMERA_RIGHT);
	Actions.AddInputMap(_Input::MOUSE_AXIS, 2, _Actions::CAMERA_UP);
	Actions.AddInputMap(_Input::MOUSE_AXIS, 3, _Actions::CAMERA_DOWN);

	// Get joystick name in lower case
	stringc Name = "";
	if(Input.HasJoystick()) {
		Name = Input.GetJoystickInfo().Name;
		Name.make_lower();
	}

	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 0, _Actions::MOVE_LEFT);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 1, _Actions::MOVE_RIGHT);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 2, _Actions::MOVE_FORWARD);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 3, _Actions::MOVE_BACK);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 0, _Actions::CURSOR_LEFT, 400.0f);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 1, _Actions::CURSOR_RIGHT, 400.0f);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 2, _Actions::CURSOR_UP, 400.0f);
	Actions.AddInputMap(_Input::JOYSTICK_AXIS, 3, _Actions::CURSOR_DOWN, 400.0f);
	Actions.AddInputMap(_Input::JOYSTICK_BUTTON, 0, _Actions::JUMP);
	Actions.AddInputMap(_Input::JOYSTICK_BUTTON, 0, _Actions::MENU_GO);
	Actions.AddInputMap(_Input::JOYSTICK_BUTTON, 1, _Actions::MENU_BACK);
	Actions.AddInputMap(_Input::JOYSTICK_BUTTON, 6, _Actions::RESET);
	Actions.AddInputMap(_Input::JOYSTICK_BUTTON, 7, _Actions::MENU_PAUSE);

	float AxisScaleX = 130.0f;
	float AxisScaleY = 100.0f;

	#ifdef WIN32

		// Assume xbox or similar controller
		Actions.AddInputMap(_Input::JOYSTICK_AXIS, 8, _Actions::CAMERA_LEFT, AxisScaleX);
		Actions.AddInputMap(_Input::JOYSTICK_AXIS, 9, _Actions::CAMERA_RIGHT, AxisScaleX);
		Actions.AddInputMap(_Input::JOYSTICK_AXIS, 6, _Actions::CAMERA_UP, AxisScaleY);
		Actions.AddInputMap(_Input::JOYSTICK_AXIS, 7, _Actions::CAMERA_DOWN, AxisScaleY);
	#else
		if(Name.find("x-box", 0) || Name.find("xbox", 0)) {
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 6, _Actions::CAMERA_LEFT, AxisScaleX);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 7, _Actions::CAMERA_RIGHT, AxisScaleX);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 8, _Actions::CAMERA_UP, AxisScaleY);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 9, _Actions::CAMERA_DOWN, AxisScaleY);
		}
		else {
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 6, _Actions::CAMERA_LEFT, AxisScaleX);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 7, _Actions::CAMERA_RIGHT, AxisScaleX);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 8, _Actions::CAMERA_UP, AxisScaleY);
			Actions.AddInputMap(_Input::JOYSTICK_AXIS, 9, _Actions::CAMERA_DOWN, AxisScaleY);
		}

	#endif
}

// Reads the config file
int _Config::ReadConfig() {

	// Open the XML file
	XMLDocument Document;
	if(Document.LoadFile(Save.GetConfigFile().c_str()) != XML_NO_ERROR) {
		return 0;
	}

	// Check for config tag
	XMLElement *ConfigElement = Document.FirstChildElement("config");
	if(!ConfigElement)
		return 0;

	// Check for a video tag
	XMLElement *VideoElement = ConfigElement->FirstChildElement("video");
	if(VideoElement) {
		int Value = 0;
		XMLElement *Element;

		// Get driver
		//VideoElement->QueryIntAttribute("driver", (int *)(&DriverType));

		// Check for the screen tag
		Element = VideoElement->FirstChildElement("screen");
		if(Element) {
			Element->QueryIntAttribute("width", &ScreenWidth);
			Element->QueryIntAttribute("height", &ScreenHeight);
			Element->QueryBoolAttribute("fullscreen", &Fullscreen);
		}

		// Check for the filtering tag
		Element = VideoElement->FirstChildElement("filtering");
		if(Element) {
			Element->QueryBoolAttribute("trilinear", &TrilinearFiltering);
			Element->QueryIntAttribute("anisotropic", &AnisotropicFiltering);
			Element->QueryIntAttribute("antialiasing", &AntiAliasing);
		}

		// Check for the shadow tag
		Element = VideoElement->FirstChildElement("shadows");
		if(Element) {
			Element->QueryBoolAttribute("enabled", &Shadows);
		}

		// Check for the shader tag
		Element = VideoElement->FirstChildElement("shaders");
		if(Element) {
			Element->QueryBoolAttribute("enabled", &Shaders);
		}
	}

	// Check for the audio tag
	XMLElement *AudioElement = ConfigElement->FirstChildElement("audio");
	if(AudioElement) {
		AudioElement->QueryBoolAttribute("enabled", &AudioEnabled);
		
		/*
		// Get sound element
		XMLElement *SoundElement = AudioElement->FirstChildElement("sound");
		if(!SoundElement) {

			// Get sound attributes
			SoundElement->QueryFloatAttribute("volume", &SoundVolume);
		}

		// Get music element
		XMLElement *MusicElement = AudioElement->FirstChildElement("music");
		if(!MusicElement) {

			// Get music attributes
			MusicElement->QueryFloatAttribute("volume", &MusicVolume);
		}
		*/
	}

	// Get input element
	XMLElement *InputElement = ConfigElement->FirstChildElement("input");
	if(InputElement) {
		InputElement->QueryFloatAttribute("mousex", &MouseScaleX);
		InputElement->QueryFloatAttribute("mousey", &MouseScaleY);
		InputElement->QueryBoolAttribute("invert_mouse", &InvertMouse);
		InputElement->QueryBoolAttribute("invert_gamepad_y", &InvertGamepadY);
		InputElement->QueryBoolAttribute("joystick_enabled", &JoystickEnabled);
	}

	// Add action maps
	Actions.ClearMappings(_Input::KEYBOARD);
	Actions.ClearMappings(_Input::MOUSE_BUTTON);
	Actions.ClearMappings(_Input::MOUSE_AXIS);
	Actions.Unserialize(InputElement);
	
	// Read the joystick config file if it exists
	int HasJoystickConfig = ReadJoystickConfig();
	
	// Add missing mappings
	AddDefaultActionMap();

	// Replays
	XMLElement *ReplayElement = ConfigElement->FirstChildElement("replay");
	if(ReplayElement) {
		ReplayElement->QueryFloatAttribute("interval", (float *)(&ReplayInterval));
	}

	return HasJoystickConfig;
}

// Writes the config file
int _Config::WriteConfig() {

	XMLDocument Document;
	Document.InsertEndChild(Document.NewDeclaration());
	
	// Config
	XMLElement *ConfigElement = Document.NewElement("config");
	ConfigElement->SetAttribute("version", "1.0");
	Document.InsertEndChild(ConfigElement);
	
	// Create video element
	XMLElement *VideoElement = Document.NewElement("video");
	//VideoElement->SetAttribute("driver", DriverType);
	ConfigElement->LinkEndChild(VideoElement);

	// Screen settings
	XMLElement *ScreenElement = Document.NewElement("screen");
	ScreenElement->SetAttribute("width", ScreenWidth);
	ScreenElement->SetAttribute("height", ScreenHeight);
	ScreenElement->SetAttribute("fullscreen", Fullscreen);
	VideoElement->LinkEndChild(ScreenElement);

	// Filtering
	XMLElement *FilteringElement = Document.NewElement("filtering");
	FilteringElement->SetAttribute("trilinear", TrilinearFiltering);
	FilteringElement->SetAttribute("anisotropic", AnisotropicFiltering);
	FilteringElement->SetAttribute("antialiasing", AntiAliasing);
	VideoElement->LinkEndChild(FilteringElement);

	// Shadows
	XMLElement *ShadowsElement = Document.NewElement("shadows");
	ShadowsElement->SetAttribute("enabled", Shadows);
	VideoElement->LinkEndChild(ShadowsElement);

	// Shaders
	XMLElement *ShadersElement = Document.NewElement("shaders");
	ShadersElement->SetAttribute("enabled", Shaders);
	VideoElement->LinkEndChild(ShadersElement);

	// Create audio element
	XMLElement *AudioElement = Document.NewElement("audio");
	AudioElement->SetAttribute("enabled", AudioEnabled);
	ConfigElement->LinkEndChild(AudioElement);

	// Sound
	//XMLElement *SoundElement = Document.NewElement("sound");
	//SoundElement->SetDoubleAttribute("volume", SoundVolume);
	//AudioElement->LinkEndChild(SoundElement);

	// Music
	//XMLElement *MusicElement = Document.NewElement("music");
	//MusicElement->SetDoubleAttribute("volume", MusicVolume);
	//AudioElement->LinkEndChild(MusicElement);

	// Input
	XMLElement *InputElement = Document.NewElement("input");
	InputElement->SetAttribute("mousex", 1.0);
	InputElement->SetAttribute("mousey", 1.0);
	InputElement->SetAttribute("invert_mouse", InvertMouse);
	InputElement->SetAttribute("invert_gamepad_y", InvertGamepadY);
	InputElement->SetAttribute("joystick_enabled", JoystickEnabled);
	ConfigElement->LinkEndChild(InputElement);

	// Write action map
	Actions.Serialize(_Input::KEYBOARD, Document, InputElement);
	Actions.Serialize(_Input::MOUSE_BUTTON, Document, InputElement);
	Actions.Serialize(_Input::MOUSE_AXIS, Document, InputElement);

	// Replays
	XMLElement *ReplayElement = Document.NewElement("replay");
	ReplayElement->SetAttribute("interval", ReplayInterval);
	ConfigElement->LinkEndChild(ReplayElement);

	// Write file
	Document.SaveFile(Save.GetConfigFile().c_str());
	
	// Write joystick config to its own file based on name
	WriteJoystickConfig();
	
	return 1;
}

// Read the current joystick's mapping
int _Config::ReadJoystickConfig() {
	if(!Input.HasJoystick())
		return 1;

	// Get joystick name
	std::string Name = Input.GetCleanJoystickName().c_str();
	std::string Path = Save.GetSavePath() + Name + ".xml";

	// Open the XML file
	XMLDocument Document;
	if(Document.LoadFile(Path.c_str()) != XML_NO_ERROR) {
		return 0;
	}

	// Get input element
	XMLElement *InputMapElement = Document.FirstChildElement("inputmap");
	if(InputMapElement) {

		// Add action maps
		Actions.ClearMappings(_Input::JOYSTICK_BUTTON);
		Actions.ClearMappings(_Input::JOYSTICK_AXIS);
		Actions.Unserialize(InputMapElement);
	}

	return 1;
}

// Write the current joystick's mapping
int _Config::WriteJoystickConfig() {
	if(!Input.HasJoystick())
		return 1;

	XMLDocument Document;
	Document.InsertEndChild(Document.NewDeclaration());
	
	// Config
	XMLElement *InputMapElement = Document.NewElement("inputmap");
	InputMapElement->SetAttribute("name", Input.GetJoystickInfo().Name.c_str());
	Document.InsertEndChild(InputMapElement);
	
	// Write action map
	Actions.Serialize(_Input::JOYSTICK_BUTTON, Document, InputMapElement);
	Actions.Serialize(_Input::JOYSTICK_AXIS, Document, InputMapElement);

	// Write file
	std::string Name = Input.GetCleanJoystickName().c_str();
	Document.SaveFile((Save.GetSavePath() + Name + ".xml").c_str());

	return 1;
}
