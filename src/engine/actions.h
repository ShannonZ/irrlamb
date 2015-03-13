/*************************************************************************************
*	irrlamb - https://github.com/jazztickets/irrlamb
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
#pragma once

// Libraries
#include <engine/input.h>

// Constants
const int ACTIONS_MAXINPUTS = 256;

// Forward declarations
namespace tinyxml2 {
	class XMLDocument;
	class XMLElement;
}

struct _ActionMap {
	_ActionMap(int Action, float Scale) : Action(Action), Scale(Scale) { }

	int Action;
	float Scale;
};

// Handles actions
class _Actions {

	public:
	
		enum Types {
			MOVE_LEFT,
			MOVE_RIGHT,
			MOVE_FORWARD,
			MOVE_BACK,
			JUMP,
			RESET,
			CAMERA_LEFT,
			CAMERA_RIGHT,
			CAMERA_UP,
			CAMERA_DOWN,
			MENU_LEFT,
			MENU_RIGHT,
			MENU_UP,
			MENU_DOWN,
			MENU_GO,
			MENU_BACK,
			MENU_PAUSE,
			CURSOR_LEFT,
			CURSOR_RIGHT,
			CURSOR_UP,
			CURSOR_DOWN,
			COUNT,
		};

		_Actions();

		void ResetState();
		void ClearMappings(int InputType);
		void ClearMappingsForAction(int InputType, int Action);

		// Actions
		float GetState(int Action);
		const std::string &GetName(int Action) { return Names[Action]; }

		// Maps
		void AddInputMap(int InputType, int Input, int Action, float Scale=1.0f, bool IfNone=true);
		int GetInputForAction(int InputType, int Action);

		// Handlers
		void InputEvent(int InputType, int Input, float Value);

		// Config
		void Serialize(int InputType, tinyxml2::XMLDocument &Document, tinyxml2::XMLElement *InputElement);
		void Unserialize(tinyxml2::XMLElement *InputElement);

	private:

		std::list<_ActionMap> InputMap[_Input::INPUT_COUNT][ACTIONS_MAXINPUTS];

		float State[COUNT];
		std::string Names[COUNT];
};

extern _Actions Actions;
