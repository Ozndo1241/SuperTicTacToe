#include "GameGrid.h"
#include "GameSession.h"
#include "GameTile.h"
#include "GameWindow.h"
#include "Player.h"

#include <vector>
#include <wx/display.h>

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameGrid.cpp")
#endif

std::vector<std::vector<GameTile*>> GameGrid::tiles;
GameGrid::GameGrid(GameWindow* parent, const size_t cols = 3, const size_t rows = 3)
	: parentWindow{ parent }, colsToReserve{ cols }, rowsToCreate{ rows }
{
	// We won't be putting this function's code in the constructor
	// Because we will be calling it again when players want to resize
	// the grid to avoid redundancy.
	CreateGrid();
}

void GameGrid::CreateGrid()
{
	tiles.resize(colsToReserve);
	for (auto& col : tiles) { col.reserve(rowsToCreate); }
	size_t outer = 0;
	const wxColour tilesCol = parentWindow->GetCurrentTheme()->tilesCol;

	for (auto& row : tiles)
	{
		for (size_t i = 0; i < rowsToCreate; ++i)
		{
			row.push_back(new GameTile(parentWindow->gamePanel, wxID_ANY, "", wxPoint(outer, i),
			wxDefaultPosition, parentWindow->FromDIP(wxSize(75, 75))));

			parentWindow->gameGridSizer->Add(row[i], wxSizerFlags().Center().Proportion(0));
			row[i]->Bind(wxEVT_BUTTON, &GameSession::OnTileClicked);
			row[i]->SetFont(*parentWindow->boldToken);
			row[i]->SetBackgroundColour(tilesCol);
		}
		++outer;
	}
}

// Clear all the tiles' labels
void GameGrid::ResetGrid()
{
	GameSession::gameData.ResetGameData();
	for (auto& row : tiles) {
		for (auto& b : row) {
			b->SetLabel("");
		}
	}
}

bool GameGrid::ResizeGrid(const size_t x, const size_t y)
{
	GameSession::gameData.ResetGameData();
	if (x * y > maxTilesInGrid || x * y < minTilesInGrid) {
		parentWindow->badGridSizeError->ShowModal();
		return false;
	}

	if (parentWindow->confirmResetDialog->ShowModal() == wxID_NO) { return false; }

	rowsToCreate = x;
	colsToReserve = y;
	
	for (const auto& row : tiles)
	{
		for (const auto& tile : row)
		{
			tile->Unbind(wxEVT_BUTTON, &GameSession::OnTileClicked);
			tile->Hide();
		}
	}

	tiles.clear();

	parentWindow->gameGridSizer->Clear();
	parentWindow->gameGridSizer->SetRows(x);
	parentWindow->gameGridSizer->SetCols(y);

	// No turn indicator needed for two players
	if (x == minTilesInGrid || y == minTilesInGrid) {
		parentWindow->buttonsSizer->Hide(1);
	}
	else {
		parentWindow->buttonsSizer->Show(1, true);
	}

	CreateGrid();

	GameSession::gameData.currentLabel = *GameTile::labels.begin();
	parentWindow->masterSizer->SetSizeHints(parentWindow);
	parentWindow->masterSizer->Layout();

	return true;
}

void GameGrid::SetTileColors(const wxColour& clr)
{
	for (const auto& a : tiles)
	{
		for (const auto& b : a)
		{
			b->SetBackgroundColour(clr);
			b->Refresh();
		}
	}
}

void GameGrid::ChangePlayerLabelColour(Player& player, const wxColour& col)
{
	player.SetLabelColour(col);
	for (const auto& row : tiles) {
		for (const auto& tile : row) {
			if (tile->GetLabel().empty()) { continue; }
			if (tile->GetLabel()[0] == player.GetLabel()) {
				tile->SetForegroundColour(col);
			}
		}
	}
}
