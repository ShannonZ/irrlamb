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
#ifndef CAMPAIGN_H
#define CAMPAIGN_H

// Libraries
#include "singleton.h"
#include <vector>
#include <string>

// Structures
struct LevelStruct {
	std::string File, DataPath;
	int Unlocked;
};

struct CampaignStruct {
	std::string Name;
	std::vector<LevelStruct> Levels;
};

// Classes
class CampaignClass {

	public:

		int Init();
		int Close();

		const std::vector<CampaignStruct> &GetCampaigns() const { return Campaigns; }
		const CampaignStruct &GetCampaign(int Index) const { return Campaigns[Index]; }

		const std::string &GetLevel(int Campaign, int Level) const { return Campaigns[Campaign].Levels[Level].File; }
		int GetLevelCount(int Campaign) const { return Campaigns[Campaign].Levels.size(); }

	private:

		std::vector<CampaignStruct> Campaigns;
};

// Singletons
typedef SingletonClass<CampaignClass> Campaign;

#endif
