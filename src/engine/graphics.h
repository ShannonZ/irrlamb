/*************************************************************************************
*	irrlamb - http://irrlamb.googlecode.com
*	Copyright (C) 2011  Alan Witkowski
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
#ifndef GRAPHICS_H
#define GRAPHICS_H

// Libraries
#include "singleton.h"
#include <irrlicht.h>
#include <vector>
#include <string>

// Structures
struct VideoModeStruct {

	int Width, Height, BPP;
	std::wstring String;
};

// Shader callback
class ShaderCallback : public irr::video::IShaderConstantSetCallBack {

	public:
		virtual void OnSetConstants(irr::video::IMaterialRendererServices *Services, irr::s32 UserData) { }
};

// Classes
class GraphicsClass {

	public:

		int Init(int Width, int Height, bool Fullscreen, irr::video::E_DRIVER_TYPE DriverType, irr::IEventReceiver *EventReceiver);
		int Close();

		void BeginFrame();
		void EndFrame();

		int GetCustomMaterial() { return CustomMaterial; }
		bool GetShadersSupported() { return ShadersSupported; }

		void SaveScreenshot();

		void SetClearColor(const irr::video::SColor &Color) { ClearColor = Color; }
		void SetDrawScene(bool Value) { DrawScene = Value; }
		void ToggleCursor(bool Value) { ShowCursor = Value; }

		const std::vector<VideoModeStruct> &GetVideoModes() { return VideoModes; }
		std::size_t GetCurrentVideoModeIndex();

	private:

		void CreateScreenshot();

		// Graphics state
		irr::video::SColor ClearColor;
		bool DrawScene, ShowCursor;

		// Shaders
		int CustomMaterial;
		bool ShadersSupported;

		// Screenshots
		bool ScreenshotRequested;

		// Modes
		std::vector<VideoModeStruct> VideoModes;
};

// Singletons
typedef SingletonClass<GraphicsClass> Graphics;

#endif
