#include "GameWindow.h"
#include "GameSession.h"
#include "GameGrid.h"
#include "GameTile.h"
#include "Player.h"
#include "RainbowEffect.h"

#include <wx/wx.h>
#include <vector>
#include <wx/rawbmp.h>
#include <random>

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameSession.cpp")
#endif

//class RainbowEffect;
class GameWindow;

namespace GameSession
{
	GameGrid* gameGrid;
	GameData gameData;
	std::unique_ptr<RainbowEffect> rgb;
	std::vector<Player> players{};

	void addDefaultplayers() {
		players.reserve(maxPlayerCount);
		players.push_back({GameTile::labels[0], "Player 1", *wxRED});
		players.push_back({GameTile::labels[1], "Player 2", *wxBLUE});
		gameGrid->parentWindow->AppendPlayerToPlayerList(players[0]);
		gameGrid->parentWindow->AppendPlayerToPlayerList(players[1]);
	}

// Change to your compiler's debug preprocessor define
// to enable debug buttons and code
#ifdef X_DEBUG
	GameTile* debug_currentTile = nullptr;
	wxColour* oldColor = nullptr;
	
	void HighlightAdjacentTile(wxCommandEvent& evt)
	{
		if (!debug_currentTile)
		{
			wxLogMessage("No tile selected");
			return;
		}

		oldColor = new wxColour(debug_currentTile->GetForegroundColour());
		debug_currentTile->SetForegroundColour(*oldColor);
		const wxPoint& p = debug_currentTile->GetTilePosition();

		if (!IsValidTilePosition(p, evt.GetId() - 1, gameGrid->GetGridReadonly()))
			return;
		
		auto& tiles = gameGrid->GetGridReadonly();

		/*
		* It is important to note that in our case the x and y axis are inverted.
		* This is due to the order that STL containers instantiate a 2d array.
		* This does not happen with C-style arrays, but they are much more dangerous
		* to work with and need a compile-time constant to define their size.
		* (and so does std::array)
		*/
		
		switch (evt.GetId())
		{
		case GameWindow::DEBUG_UP:
			tiles[p.x - 1][p.y]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_DOWN:
			tiles[p.x + 1][p.y]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_LEFT:
			tiles[p.x][p.y - 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_RIGHT:
			tiles[p.x][p.y + 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_DIAG_UP:
			tiles[p.x - 1][p.y + 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_DIAG_DOWN:
			tiles[p.x + 1][p.y + 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_DIAG_UP_BACKW:
			tiles[p.x - 1][p.y - 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		case GameWindow::DEBUG_DIAG_DOWN_BACKW:
			tiles[p.x + 1][p.y - 1]->SetForegroundColour(wxColor(255,255,255));
			break;
		default:
			assert(0 && "Invalid direction");
		}

		/* self note:
		* Up = left
		* Down = right
		* Right = down
		* Left = up
		* Diagonal up = inverted diagonal down.
		*/
	}
#endif

    void SortPlayersArr()
    {
		std::sort(players.begin(), players.end(),
		[](const auto& a, const auto& b) {
			return a.GetScore() < b.GetScore();
		});
    }

    bool ValidateNewPlayerData(const wchar_t &label, const wxString& name, const wxColour &clr)
    {
		wxMessageDialog msg{nullptr, ""};
		if (!name.compare("Enter player name") || name.empty()) {
			msg.SetMessage("Please enter a name.");
			msg.ShowModal();
			return false;
		}

		for (const Player& player : players)
		{
			if (!player.GetName().compare(name.c_str())) {
				msg.SetMessage("Player name already taken.");
				msg.ShowModal();
				return false;
			}
			else if (player.GetLabelColour() == clr) {
				msg.SetMessage(wxString::Format("%s (%c) already has this color.", player.GetName().c_str(),
					player.GetLabel()));
				msg.ShowModal();
				return false;
			}
			else if (clr == gameGrid->parentWindow->GetCurrentTheme()->tilesCol) {
				msg.SetMessage("Label won't be visible on tiles as they are the same colour.");
				msg.ShowModal();
				return false;
			}
			else if (player.GetLabel() == label) {
				msg.SetMessage(wxString::Format("%s already has this logo.", player.GetName().c_str()));
				msg.ShowModal();
				return false;
			}
		}
		return true;
	}

	void AddPlayer(const Player& player)
	{
		if (ValidateNewPlayerData(player.GetLabel(), player.GetName(), player.GetLabelColour())) {
			players.push_back(player);
			gameGrid->parentWindow->AppendPlayerToPlayerList(player);
		}
	}
	// out of bounds check
	bool IsValidTilePosition(const wxPoint& point, int dir, const std::vector<std::vector<GameTile*>>& grid)
	{
		switch (dir)
		{
		case GameGrid::UP:
			return point.x - 1 >= 0;
		case GameGrid::DOWN:
			return point.x + 1 < grid.back().size();
		case GameGrid::LEFT:
			return point.y - 1 >= 0;
		case GameGrid::RIGHT:
			return point.y + 1 < grid.size();
		case GameGrid::DIAGONAL_UP:
			return point.x - 1 >= 0 && point.y + 1 < grid.back().size();
		case GameGrid::DIAGONAL_DOWN:
			return point.x + 1 < grid.back().size() && point.y + 1 < grid.size();
		case GameGrid::DIAGONAL_UP_BACKWARDS:
			return point.x - 1 >= 0 && point.y - 1 >= 0;
		case GameGrid::DIAGONAL_DOWN_BACKWARDS:
			return point.x + 1 < grid.back().size() && point.y - 1 >= 0;
		default:
			assert(0 && "Invalid direction");
		}
		return false;
	}

	// Gets the opposite direction
	GameGrid::Directions operator-(GameGrid::Directions dir)
	{
		switch (dir)
		{
		case GameGrid::UP:
			return GameGrid::DOWN;
		case GameGrid::DOWN:
			return GameGrid::UP;
		case GameGrid::LEFT:
			return GameGrid::RIGHT;
		case GameGrid::RIGHT:
			return GameGrid::LEFT;
		case GameGrid::DIAGONAL_UP:
			return GameGrid::DIAGONAL_DOWN_BACKWARDS;
		case GameGrid::DIAGONAL_DOWN:
			return GameGrid::DIAGONAL_UP_BACKWARDS;
		case GameGrid::DIAGONAL_UP_BACKWARDS:
			return GameGrid::DIAGONAL_DOWN;
		case GameGrid::DIAGONAL_DOWN_BACKWARDS:
			return GameGrid::DIAGONAL_UP;
		default:
			assert("Invalid direction passed to operator-()");
		}
		return GameGrid::MAX_DIRECTIONS;
	}

	bool RecursivelyScanForWinCondition(const GameTile* tile, int dir,
		const GameTile& original, unsigned int matchingAdjacentCount = 0)
	{
		if (matchingAdjacentCount == gameGrid->GetNumOfColumns())
		{
			/*
			* This bool is basically the return value of this function.
			* If it becomes true, then that would be the final return result
			* after all the call stack pops finish.
			*/
			//gameData.winTiles.second = tile;
			gameData.winCondition = true;
			return true;
		}

		static GameTile* adjacentTile;

		if (tile)
		{
			/*
			* It is ok for this block to be reaccessed after an invalid tile
			* was encountered, since the below else-if statement would reverse
			* the search direction.
			* In the case that an invalid tile was encountered after the
			* search direction was reversed, this function returns false.
			*/

			if (tile->GetLabel() == gameData.currentLabel) {
				++matchingAdjacentCount;
				/*
				* This is needed because the second statement
				* in the if-else chain below only works
				* in cases where the search direction was reversed. i.e - 
				* a player connects the line from the middle and does not
				* complete it from an edge/corner. In the case that a line
				* is completed at the an edge/corner, and the tile label matches
				* the line pattern, then we need to save a pointer of that tile
				* to the winTiles pair, specifically, the second in the pair
				* because we couldn't reverse search direction before the top-most
				* if statement executes and starts return true.
				* the else if below would only execute if the algorithm had reversed
				* the search direction, which happens after checking the matching
				* adjacent tiles count.
				*/
				gameData.winTiles.push_back((GameTile*)tile);
			}
			else
				return false;

			adjacentTile = tile->GetAdjacentTile(dir);
			
			RecursivelyScanForWinCondition(adjacentTile, dir, original, matchingAdjacentCount);
		}
		else if (!gameData.searchedOnce)
		{
			gameData.searchedOnce = true;

			/*
			* Take note of the second parameters. We are using an overloaded
			* minus operator to retrieve the opposite direction that was
			* originally passed to this function.
			*
			* We start searching for tiles with equal "tics"
			* in opposing directions from the original tile only after we
			* reach the edge/corner of the grid and the number of matching
			* counter is less than the grid size squared.
			*
			* This else statement should be reached only once every
			* iteration from the function calling this recursion.
			*/

			// This casting mess is a workaround.
			adjacentTile = original.GetAdjacentTile(-(static_cast<GameGrid::Directions>(dir)));
			RecursivelyScanForWinCondition(adjacentTile, -(static_cast<GameGrid::Directions>(dir)), original, matchingAdjacentCount);
		}
		return gameData.winCondition;
	}

	bool CheckWinCondition(const GameTile* tile)
	{
		if (!tile)
			throw "Tile passed in CheckWinCondition() was null";

		GameTile* originalTile = new GameTile(*tile); // copy
		for (int dir = 0; dir < GameGrid::MAX_DIRECTIONS; ++dir)
		{
			if (originalTile && originalTile->GetAdjacentTile(dir)
				&& originalTile->GetAdjacentTile(dir)->GetLabel() == gameData.currentLabel) {
				if (RecursivelyScanForWinCondition(tile, static_cast<GameGrid::Directions>(dir), *originalTile)) {
					gameData.searchedOnce = false; // Has to be reset when we exit
					gameData.winCondition = false; // this loop for next use.
					gameData.winDirection = static_cast<GameGrid::Directions>(dir);
					return true;
				}
				// clear the vector from any residual tiles
				gameData.winTiles.clear();
			}
			gameData.searchedOnce = false;
			gameData.winCondition = false;
		}
		return false;
	}
	
	void OnTileClicked(wxCommandEvent& evt)
	{
		if (gameData.somebodyWon) { return; }
		
		auto currentTile = dynamic_cast<GameTile*>(evt.GetEventObject());
		auto& currentPlayer = players[gameData.turn - 1];

// Change to your compiler's debug preprocessor define
// to enable debug buttons and code
#ifdef X_DEBUG
		// debugPos1 represents the real position of the tile (after reversing the x and y coordinates)
		// debugPos2 is basically a copy of debugPos1 before it got reversed.

		debug_currentTile = currentTile;
		std::string debugPos1{ ('(' + std::to_string(currentTile->GetTilePosition().x) + ',') + (std::to_string(currentTile->GetTilePosition().y) + ')') };
		std::string debugPos2{ debugPos1 };
		std::reverse(debugPos1.begin() + 1, debugPos1.end() - 1);

		wxLogDebug((("Real position: " + debugPos1) + (" Array subscript: " + debugPos2)).c_str());
#endif
		// If a tile has already been marked.
		if (currentTile->GetLabel().size() > 0) {
			return;
		}
		else {
			currentTile->SetLabel(gameData.currentLabel);
			currentTile->SetForegroundColour(currentPlayer.GetLabelColour());
			++gameData.movesCounter;
		}
		wxLogDebug("Moves: %d", gameData.movesCounter);
		// tie
		if (gameData.movesCounter == gameGrid->GetNumOfColumns() * gameGrid->GetNumOfRows()) {
			gameGrid->parentWindow->GameOverTie();
			return;
		}
		// 2 * columns or rows count - 1
		else if (gameData.movesCounter >= 2 * gameGrid->GetNumOfColumns() - 1) {
			try
			{
				if (CheckWinCondition(currentTile)) {
					wxMessageDialog windlg(nullptr, wxString::Format("%s won!", currentPlayer.GetName()));

					currentPlayer.IncrementScore();
					// this will now allow the rgb to appear.
					gameData.somebodyWon = true;
					auto prop{gameGrid->parentWindow->GetGameMenu()->GetProperty("PLAYER_" + std::to_string(currentPlayer.GetPlayerId()))};
					wxString txt{prop->GetLabel()};
					size_t start = txt.find_first_of('[');
					size_t end = txt.find_first_of(']');

					auto currentPlayerPtr{reinterpret_cast<Player*>(prop->GetClientData())};
					if (&currentPlayer != currentPlayerPtr)
					{
						wxLogMessage("Players are not the same");
					}
				
					prop->SetLabel(txt.erase(start, end - start + 1)
					 + "[ " + std::to_string(currentPlayer.GetScore())
					 + (currentPlayer.GetScore() != 1 ? " wins ]" : " win ]"));

					gameGrid->parentWindow->SortPlayers();
					
					windlg.ShowWindowModal();
					rgb.get()->StartRainbowEffect();
				}
			}
			catch (const std::out_of_range& ex)
			{
				wxLogDebug(ex.what());
				//wxLogDebug((("Real position: " + debugPos1) + (" Array subscript: " + debugPos2)).c_str());
			}
		}

		if (gameData.turn < gameData.numOfPlayers && players.begin() + gameData.turn != players.end())
		{
			// Current label to be checked on the grid for whoever's turn it is
			Player& nextPlayer{ (*(players.begin() + gameData.turn)) };
			gameData.currentLabel = nextPlayer.GetLabel();
			gameGrid->parentWindow->SetPlayerTurnLabel(nextPlayer.GetLabel(), nextPlayer.GetLabelColour());
			++gameData.turn;
		}
		else // start back from the first player
		{
			gameData.turn = 1;
			gameData.currentLabel = (*players.begin()).GetLabel();
			gameGrid->parentWindow->SetPlayerTurnLabel((*players.begin()).GetLabel(), (*players.begin()).GetLabelColour());
		}
			
	}

	void GameData::ResetGameData(bool resetPlayerCount)
	{
		if (resetPlayerCount) 
			gameData.numOfPlayers = minPlayerCount;
		
		gameData.turn = 1;
		gameData.movesCounter = 0;
		gameData.currentLabel = *GameTile::labels.begin();
		gameData.winTiles.clear();

		gameData.somebodyWon = false;
		gameData.searchedOnce = false;
		gameData.winCondition = false;
		gameData.somebodyWon = false;
	}
}