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
#ifndef SAVE_H
#define SAVE_H

// Libraries
#include "singleton.h"
#include <irrlicht.h>
#include <map>
#include <vector>
#include <string>

// Forward Declarations
class DatabaseClass;

// Struct for one highscore
struct HighScoreStruct {
	HighScoreStruct() : Time(0), DateStamp(0) { }
	HighScoreStruct(float Time, int DateStamp) : Time(Time), DateStamp(DateStamp) { }

	bool operator<(const HighScoreStruct &Value) {
		return Time < Value.Time;
	}

	float Time;
	time_t DateStamp;
};

// Struct for one level stat
struct SaveLevelStruct {
	SaveLevelStruct() : ID(0), Unlocked(0), LoadCount(0), LoseCount(0), WinCount(0), PlayTime(0) { }

	int ID;
	int Unlocked;
	int LoadCount, LoseCount, WinCount;
	float PlayTime;
	std::vector<HighScoreStruct> HighScores;
};

// Classes
class SaveClass {

	public:

		int Init();
		int Close();

		int InitStatsDatabase();

		const std::string &GetSavePath() { return SavePath; }
		const std::string &GetReplayPath() { return ReplayPath; }
		const std::string &GetConfigFile() { return ConfigFile; }
		const std::string &GetScreenshotsPath() { return ScreenshotsPath; }
		const std::string &GetCustomLevelsPath() { return CustomLevelsPath; }

		int LoadLevelStats();
		void SaveLevelStats(const std::string &Level);
		void UpdateLevelStats(const std::string &Level, const SaveLevelStruct &Stats);
		bool GetLevelStats(const std::string &Level, SaveLevelStruct &Stats);
		const SaveLevelStruct *GetLevelStats(const std::string &Level);

		void AddScore(const std::string &Level, float Time);
		void IncrementLevelLoadCount(const std::string &Level);
		void IncrementLevelLoseCount(const std::string &Level);
		void IncrementLevelWinCount(const std::string &Level);
		void IncrementLevelPlayTime(const std::string &Level, float Time);
		void UnlockLevel(const std::string &Level);

	private:

		// Paths
		std::string SavePath, ReplayPath, ScreenshotsPath, CustomLevelsPath, ConfigFile, StatsFile;
		
		// Stats
		std::map<std::string, SaveLevelStruct> LevelStats;
		std::map<std::string, SaveLevelStruct>::iterator LevelStatsIterator;

		// Database
		DatabaseClass *Database;
};

// Singletons
typedef SingletonClass<SaveClass> Save;

#endif
