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
#pragma once

// Classes
class _State {

	public:

		// Setup functions
		virtual int Init() { return 1; }
		virtual int Close() { return 1; }

		virtual ~_State() { }

		// Events
		virtual bool HandleKeyPress(int Key) { return false; }
		virtual bool HandleKeyLift(int Key) { return false; }
		virtual void HandleMouseMotion(float UpdateX, float UpdateY) { }
		virtual bool HandleMousePress(int Button, int MouseX, int MouseY) { return false; }
		virtual void HandleMouseLift(int Button, int MouseX, int MouseY) { }
		virtual void HandleMouseWheel(float Direction) { }
		virtual void HandleGUI(int EventType, irr::gui::IGUIElement *Element) { }
		
		// Update
		virtual void Update(float FrameTime) { }
		virtual void UpdateRender(float TimeStepRemainder) { }
		virtual void Draw() { }

	private:

};
