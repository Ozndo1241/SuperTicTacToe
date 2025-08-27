#pragma once

#include <vector>
#include <wx/wx.h>
#include <wx/bmpbuttn.h>
//#include "GameWindow.h"
#include "Player.h"

class GameTile;
class GameWindow;

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameGrid.h")
#endif

class GameGrid
{
public:
	GameGrid(GameWindow* parent, const size_t cols, const size_t rows);

	enum Directions
	{
		UP,
		DOWN,
		LEFT,
		RIGHT,
		DIAGONAL_UP,
		DIAGONAL_DOWN,
		DIAGONAL_UP_BACKWARDS,
		DIAGONAL_DOWN_BACKWARDS,
		MAX_DIRECTIONS
	};

	static constexpr const int maxTilesInGrid{100};
	static constexpr const int minTilesInGrid{9};

	const size_t GetNumOfColumns() const { return colsToReserve; } 
	const size_t GetNumOfRows() const { return rowsToCreate; }
	void ResetGrid();
	bool ResizeGrid(const size_t x, const size_t y);
	void SetTileColors(const wxColour& clr);
	void ChangePlayerLabelColour(Player& player, const wxColour& col);
	const std::vector<std::vector<GameTile*>>& GetGridReadonly() const { return tiles; }
	GameWindow* parentWindow;

	friend class GameWindow;
	friend class GameTile;
private:
	void CreateGrid();
	size_t colsToReserve;
	size_t rowsToCreate; 
	static std::vector<std::vector<GameTile*>> tiles;
};