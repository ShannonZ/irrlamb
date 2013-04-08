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

// Struct for OpenAL buffers
struct AudioBufferStruct {
	ALuint ID;
	ALenum Format;
};

// Class for OpenAL sources
class _AudioSource {

	public:

		_AudioSource(const AudioBufferStruct *Buffer, bool Loop=false, float MinGain=0.0f, float MaxGain=1.0f, float ReferenceDistance=1.0f, float RollOff=1.0f);
		~_AudioSource();

		void Play();
		void SetPitch(float Value);
		void SetGain(float Value);
		void SetPosition(float X, float Y, float Z);

	private:

		bool Loaded;
		ALuint ID;
};

// Classes
class _Audio {

	public:

		int Init(bool Enabled);
		int Close();

		bool IsEnabled() { return Enabled; }

		// Buffers
		bool LoadBuffer(const std::string &File);
		const AudioBufferStruct *GetBuffer(const std::string &File);
		void FreeAllBuffers();

		// 3D Audio
		void SetPosition(float X, float Y, float Z);
		void SetDirection(float X, float Y, float Z);
		void SetGain(float Value);
		
	private:

		// State
		bool Enabled;

		// Buffers
		std::map<std::string, AudioBufferStruct>::iterator BuffersIterator;
		std::map<std::string, AudioBufferStruct> Buffers;
};

// Singletons
extern _Audio Audio;
