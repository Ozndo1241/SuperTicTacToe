#pragma once

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameSession.h")
#endif

#include <wx/wx.h>
#include <algorithm>
#include <string>
#include <utility>
#include <memory>
#include "Player.h"
#include "GameTile.h"
#include "GameGrid.h"
#include "RainbowEffect.h"

class GameGrid;

namespace GameSession
{
	extern GameGrid* gameGrid;
	extern std::unique_ptr<RainbowEffect> rgb;
	constexpr const size_t minPlayerCount{2};
	constexpr const size_t maxPlayerCount{8};
	constexpr const int test{1002};
	struct GameData
	{
		size_t numOfPlayers = 0;
		wchar_t currentLabel = *GameTile::labels.begin();
		wchar_t newPlayerLabel = '\0';
		unsigned int movesCounter = 0;
		size_t turn = 1;

		//! Internal data
		bool searchedOnce = false;
		bool winCondition = false;
		bool somebodyWon = false;
		bool gridWasReset = false;
		std::vector<GameTile*> winTiles{};
		GameGrid::Directions winDirection{};

		// If a too little player count was input,
		// then numOfPlayers defaults to 2, and if the
		// player count exceeds 8 players, it defaults
		// to 8 players.
		void SetNumOfPlayers(size_t num) { numOfPlayers = (num > minPlayerCount
			? (num <= maxPlayerCount ? num : maxPlayerCount) : minPlayerCount); }

		// This does not reset the number of players by default
		void ResetGameData(bool resetPlayerCount = false);
	};

	extern GameData gameData;
	extern std::vector<Player> players;
	void addDefaultplayers();
	
// Change to your compiler's debug preprocessor define
// to enable debug buttons and code
#ifdef X_DEBUG
	void HighlightAdjacentTile(wxCommandEvent& evt);
#endif
	extern void SortPlayersArr();
	extern bool ValidateNewPlayerData(const wchar_t& label, const wxString& name, const wxColour& clr);
	extern void AddPlayer(const Player& player);
	extern bool IsValidTilePosition(const wxPoint& point, int dir, const std::vector<std::vector<GameTile*>>& grid);

	// Note! dir is supposed to be of type GameWindow::Directions,
	// but because of the header file mess, it's an int as a temporary workaround.
	static bool RecursivelyScanForWinCondition(const GameTile* tile, int dir, const GameTile& original, unsigned int matchingAdjacentCount);
	static bool CheckWinCondition(const GameTile* tile);
	extern void OnTileClicked(wxCommandEvent& evt);
}

class TicTacToeGame : public wxApp
{
private:
	virtual bool OnInit();
};