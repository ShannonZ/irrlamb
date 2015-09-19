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
#include <engine/campaign.h>
#include <engine/globals.h>
#include <engine/log.h>
#include <engine/game.h>
#include <engine/level.h>
#include <tinyxml/tinyxml2.h>

_Campaign Campaign;

using namespace tinyxml2;

// Loads the campaign data
int _Campaign::Init() {
	Campaigns.clear();

	Log.Write("_Campaign::Init - Loading file irrlamb.xml");

	// Open the XML file
	std::string LevelFile = std::string("levels/main.xml");
	XMLDocument Document;
	if(Document.LoadFile(LevelFile.c_str()) != XML_NO_ERROR) {
		Log.Write("Error loading level file with error id = %d", Document.ErrorID());
		Log.Write("Error string 1: %s", Document.GetErrorStr1());
		Log.Write("Error string 2: %s", Document.GetErrorStr2());
		Close();
		return 0;
	}

	// Check for level tag
	XMLElement *CampaignsElement = Document.FirstChildElement("campaigns");
	if(!CampaignsElement) {
		Log.Write("Could not find campaigns tag");
		return 0;
	}

	// Load campaigns
	XMLElement *CampaignElement = CampaignsElement->FirstChildElement("campaign");
	for(; CampaignElement != 0; CampaignElement = CampaignElement->NextSiblingElement("campaign")) {

		CampaignStruct Campaign;
		Campaign.Name = CampaignElement->Attribute("name");

		// Get levels
		XMLElement *LevelElement = CampaignElement->FirstChildElement("level");
		for(; LevelElement != 0; LevelElement = LevelElement->NextSiblingElement("level")) {
			LevelStruct Level;
			Level.File = LevelElement->GetText();
			Level.DataPath = Game.GetWorkingPath() + "levels/" + Level.File + "/";
			Level.Unlocked = 0;
			LevelElement->QueryIntAttribute("unlocked", &Level.Unlocked);

			::Level.Init(Level.File, true);
			Level.NiceName = ::Level.GetLevelNiceName();

			Campaign.Levels.push_back(Level);
		}

		Campaigns.push_back(Campaign);
	}

	return 1;
}

// Closes the campaign system
int _Campaign::Close() {

	Campaigns.clear();

	return 1;
}

// Get the level count for the campaign
int _Campaign::GetLevelCount(int Campaign) {
	if(Campaign < 0 || Campaign >= (int)Campaigns.size())
		return 0;

	return Campaigns[Campaign].Levels.size();
}

// Check if a level is the last in the campaign
bool _Campaign::IsLastLevel(int Campaign, int Level) {

	return Level + 1 >= GetLevelCount(Campaign);
}
