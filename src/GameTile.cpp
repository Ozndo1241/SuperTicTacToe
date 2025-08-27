#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameTile.cpp")
#endif

#include "GameSession.h"
#include "GameTile.h"
#include "GameGrid.h"
#include "GameWindow.h"

#include <wx/wx.h>

using namespace GameSession;
class GameWindow;

GameTile::GameTile(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& tilePos, const wxPoint& pos, const wxSize& size, long style)
	: wxButton(parent, id, label, pos, size, style), tilePosition(tilePos)
{
}

GameTile::GameTile(const GameTile& other) noexcept
{
	this->tilePosition = other.tilePosition;
}

GameTile* GameTile::GetAdjacentTile(int direction) const
{
	if (!IsValidTilePosition(tilePosition, direction, gameGrid->GetGridReadonly())) {
		wxLogDebug("No adjacent tile found");
		return nullptr;
	}
#ifdef X_DEBUG
	else {
		std::string dirStr;
		switch (direction)
		{
		case GameGrid::UP:
			dirStr = "UP";
			break;
		case GameGrid::DOWN:
			dirStr = "DOWN";
			break;
		case GameGrid::LEFT:
			dirStr = "LEFT";
			break;
		case GameGrid::RIGHT:
			dirStr = "RIGHT";
			break;
		case GameGrid::DIAGONAL_UP:
			dirStr = "DIAGONAL UP";
			break;
		case GameGrid::DIAGONAL_DOWN:
			dirStr = "DIAGONAL DOWN";
			break;
		case GameGrid::DIAGONAL_UP_BACKWARDS:
			dirStr = "DIAGONAL UP BACKWARDS";
			break;
		case GameGrid::DIAGONAL_DOWN_BACKWARDS:
			dirStr = "DIAGONAL DOWN BACKWARDS";
			break;
		default:
			dirStr = "???";
		}
		wxLogDebug(dirStr.c_str());
	}
#endif
	switch (direction)
	{
	case GameGrid::UP:
		return GameGrid::tiles[tilePosition.x - 1][tilePosition.y];
	case GameGrid::DOWN:
		return GameGrid::tiles[tilePosition.x + 1][tilePosition.y];
	case GameGrid::LEFT:
		return GameGrid::tiles[tilePosition.x][tilePosition.y - 1];
	case GameGrid::RIGHT:
		return GameGrid::tiles[tilePosition.x][tilePosition.y + 1];
	case GameGrid::DIAGONAL_UP:
		return GameGrid::tiles[tilePosition.x - 1][tilePosition.y + 1];
	case GameGrid::DIAGONAL_DOWN:
		return GameGrid::tiles[tilePosition.x + 1][tilePosition.y + 1];
	case GameGrid::DIAGONAL_UP_BACKWARDS:
		return GameGrid::tiles[tilePosition.x - 1][tilePosition.y - 1];
	case GameGrid::DIAGONAL_DOWN_BACKWARDS:
		return GameGrid::tiles[tilePosition.x + 1][tilePosition.y - 1];
	default:
		assert(0 && "Invalid direction");
	}
	return nullptr;
}

bool GameTile::IsEdgeTile() const
{
	return tilePosition.x == 0
		|| tilePosition.y == 0
		|| tilePosition.x == gameGrid->GetNumOfColumns() - 1
		|| tilePosition.y == gameGrid->GetNumOfRows() - 1;
}