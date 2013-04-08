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
#include "graphics.h"
#include "save.h"
#include "globals.h"
#include "interface.h"
#include "input.h"
#include "log.h"
#include "fader.h"
#include "config.h"
#include "../irrb/CIrrBMeshFileLoader.h"
#include "namespace.h"
#include <sstream>
#include <ctime>
#include <irrlicht.h>

_Graphics Graphics;

// Initializes the graphics system
int _Graphics::Init(int Width, int Height, bool Fullscreen, E_DRIVER_TYPE DriverType, IEventReceiver *EventReceiver) {
	ShowCursor = true;
	ShadersSupported = false;
	CustomMaterial = -1;

	// irrlicht parameters
	SIrrlichtCreationParameters Parameters;
	Parameters.DriverType = DriverType;
	Parameters.Fullscreen = Fullscreen;
	Parameters.Bits = 32;
	Parameters.Vsync = 0;
	Parameters.Stencilbuffer = Config.Shadows;
	Parameters.AntiAlias = Config.AntiAliasing;
	Parameters.WindowSize.set(Width, Height);

	// Create the irrlicht device
	irrDevice = createDeviceEx(Parameters);
	if(irrDevice == NULL)
		return 0;

	irrDevice->setWindowCaption(L"irrlamb");
	irrDevice->setEventReceiver(EventReceiver);
	irrDevice->getCursorControl()->setVisible(false);
	irrDevice->getLogger()->setLogLevel(ELL_ERROR);
	
	// Save off global pointers
	irrDriver = irrDevice->getVideoDriver();
	irrScene = irrDevice->getSceneManager();
	irrGUI = irrDevice->getGUIEnvironment();
	irrFile = irrDevice->getFileSystem();
	irrTimer = irrDevice->getTimer();

	VideoModes.clear();

	// Generate a list of video modes
	IVideoModeList *VideoModeList = irrDevice->getVideoModeList();
	VideoModeStruct VideoMode;
	for(int i = 0; i < VideoModeList->getVideoModeCount(); i++) {
		VideoMode.Width = VideoModeList->getVideoModeResolution(i).Width;
		VideoMode.Height = VideoModeList->getVideoModeResolution(i).Height;
		VideoMode.BPP = VideoModeList->getVideoModeDepth(i);
		
		// Add the video mode
		if((VideoMode.BPP == 32 || VideoMode.BPP == 24) && VideoMode.Width >= 640) {
			std::wstringstream Stream;
			Stream << VideoMode.Width << " x " << VideoMode.Height;
			VideoMode.String = Stream.str();
			VideoModes.push_back(VideoMode);
		}
	}

	//irrScene->setShadowColor(SColor(150, 0, 0, 0));
	DrawScene = true;
	ScreenshotRequested = false;

	// Load custom loader
	CIrrBMeshFileLoader *Loader = new CIrrBMeshFileLoader(irrScene, irrFile);
	irrScene->addExternalMeshLoader(Loader);
	Loader->drop();

	// Check for shader support
	if(irrDriver->queryFeature(EVDF_PIXEL_SHADER_1_1) 
	&& irrDriver->queryFeature(EVDF_ARB_FRAGMENT_PROGRAM_1) 
	&& irrDriver->queryFeature(EVDF_VERTEX_SHADER_1_1) 
	&& irrDriver->queryFeature(EVDF_ARB_VERTEX_PROGRAM_1)) {
		ShadersSupported = true;

		// Create shader materials
		if(Config.Shaders) {
			ShaderCallback *Shader = new ShaderCallback();
			CustomMaterial = irrDriver->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles("shaders/lighting.vert", "vertexMain", EVST_VS_1_1, "shaders/lighting.frag", "pixelMain", EPST_PS_1_1, Shader, EMT_SOLID);
			Shader->drop();
		}
	}
	else {
		Log.Write("Shaders not supported.");
		Config.Shaders = false;
	}

	return 1;
}

// Closes the graphics system
int _Graphics::Close() {
	
	// Close irrlicht
	irrDevice->drop();

	return 1;
}

// Erases the buffer and sets irrlicht up for the next frame
void _Graphics::BeginFrame() {
	irrDriver->beginScene(true, true, ClearColor);
	
	if(DrawScene)
		irrScene->drawAll();
}

// Draws the buffer to the screen
void _Graphics::EndFrame() {
	
	// Draw cursor
	if(ShowCursor)
		Interface.DrawImage(_Interface::IMAGE_MOUSECURSOR, Input.GetMouseX(), Input.GetMouseY(), 16, 16);

	Fader.Draw();
	irrDriver->endScene();
	
	// Handle screenshots
	if(ScreenshotRequested)
		CreateScreenshot();
}

// Returns the index of the current video mode
std::size_t _Graphics::GetCurrentVideoModeIndex() {

	// Find the video mode
	for(std::size_t i = 0; i < VideoModes.size(); i++) {
		if(Config.ScreenWidth == VideoModes[i].Width && Config.ScreenHeight == VideoModes[i].Height)
			return i;
	}

	return 0;
}

// Request screenshot
void _Graphics::SaveScreenshot() {
	ScreenshotRequested = 1;
}

// Create the screenshot
void _Graphics::CreateScreenshot() {

	// Get time
	time_t Now;
	time(&Now);

	// Get filename
	char Filename[32];
	strftime(Filename, 32, "%Y%m%d-%H%M%S.jpg", localtime(&Now)); 

	// Create image
	IImage *Image = irrDriver->createScreenShot();
	std::string FilePath = Save.GetScreenshotsPath() + Filename;
	irrDriver->writeImageToFile(Image, FilePath.c_str()); 
	Image->drop();

	// Drop request
	ScreenshotRequested = 0;
}
