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
#include "filestream.h"
#include <cstring>

using namespace std;

// Open for writing
int FileClass::OpenForWrite(const char *Filename) {

	File.open(Filename, ios::out | ios::binary);
	if(!File.is_open())
		return 0;

	return 1;
}

// Open for reading
int FileClass::OpenForRead(const char *Filename) {

	File.open(Filename, ios::in | ios::binary);
	if(!File.is_open())
		return 0;

	return 1;
}

// Writes a string
void FileClass::WriteString(const char *Data) {

	File.write(Data, strlen(Data));
	File.put(0);
}

// Reads an integer
int FileClass::ReadInt() {

	int Data;
	File.read(reinterpret_cast<char *>(&Data), sizeof(Data));

	return Data;
}

// Reads an integer
short int FileClass::ReadShortInt() {

	short int Data;
	File.read(reinterpret_cast<char *>(&Data), sizeof(Data));

	return Data;
}

// Reads a float
float FileClass::ReadFloat() {

	float Data;
	File.read(reinterpret_cast<char *>(&Data), sizeof(Data));

	return Data;
}

// Reads a string
void FileClass::ReadString(char *Data) {

	// Check for null strings
	if(File.peek() == 0) {
		Data[0] = File.get();
		return;
	}

	// Get string
	File.get(Data, 2147483647, 0);
	File.get();
}
