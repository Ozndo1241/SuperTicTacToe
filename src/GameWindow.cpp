#include "GameWindow.h"
#include "GameSession.h"
#include "GameGrid.h"
#include "GameTile.h"
#include "Player.h"
#include "PlayerPropertyDialog.h"
#include "PlayersInfoEditor.h"
#include "ThemeSelectionProperty.h"
#include "PlayerPGEditor.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <memory>
#include <string>
#include <wx/wx.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/gbsizer.h>
#include <wx/graphics.h>
#include <wx/rawbmp.h>

class GameGrid;

bool TicTacToeGame::OnInit()
{
	GameWindow* gameWindow = new GameWindow();
	GameSession::gameGrid = gameWindow->mainGrid.get();
	GameSession::addDefaultplayers();
	GameSession::rgb = std::make_unique<RainbowEffect>(GameSession::gameData.winTiles);
	return gameWindow->Show(true);
}

wxIMPLEMENT_APP(TicTacToeGame);

//?TODOLIST

//!TODO: --> Fix header file mess (again -_-)
//!TODO: --> Implement player class
//!TODO: --> add remove player button in leaderboard (or in player count property).
//!TODO: --> implement player count algorithm
//!TODO: --> outline the line of consecutive labels when there is a winner
//!TODO: --> Add selectable themes for program
//!TODO: --> Add filesystem for custom themes
//!TODO: --> Add dialog for entering theme name upon save.
//!TODO: --> Fix out-of-range error upon remove theme.
//!TODO: --> Add turn indicator button.
//!TODO: --> Add diverse selectable labels for each player (Should also be color customizable)
//!TODO:-medium-priority: --> Add scoring counter for all available players
//!TODO: Fix wxColourData thing in player change colour.
//!TODO: --> fix property grid error when adding players after removing previous ones
//!TODO: --> implement player renaming functionality.

inline bool GameWindow::ValidateThemeFileData(std::ifstream& file)
{
	std::string colData;
	// 7 hex characters. One for the # and the rest for the actual data. 
	for (int dataCount{ 0 }; std::getline(file, colData); ++dataCount) {
		if (colData.find('#') != std::string::npos && colData.length() == colorDataCharacterLength) {
			/*
			* The string is transformed to all uppercase because even though
			* the wxWidgets framework returns color data in all uppercase,
			* any external modification to the theme file could cause errors.
			* For example, users may change the value of a hex colour code
			* and include lower-case letters in it. This could cause the
			* below algorithm to render it incorrect and return an error.
			*/
			std::transform(colData.begin(), colData.end(), colData.begin(), ::toupper);
			bool hashtagFound{};
			for (const char token : colData) {
				if (!hashtagFound && token == '#') {
					hashtagFound = true;
					continue;
				}
				// If color data contains anything else other than
				// 0123456789ABCDEF characters...
				if (token > 'F' || (token < 'A' && token > '9') || token < '0') {
					return false;
				}
			}
		}
		else if (dataCount == maxNumberOfColours) {
			return true;
		}
		else { // Each color data must have a value for each R, G, and B
			return false;
		}
	}
	return false;
}

inline void GameWindow::LoadThemesFromDir()
{
	if (!std::filesystem::exists(std::filesystem::current_path() / "Themes")) {
		std::filesystem::create_directory(std::filesystem::current_path() / "Themes");
		// return;
	}
	
	std::string filepath{ std::filesystem::current_path() / "Themes" };
	currentTheme = themes[0];

	try {
		// iterate over every theme file in the directory
		for (const auto& entry : std::filesystem::directory_iterator(filepath)) {
			std::ifstream file{ entry.path(), std::ios::in };

			if (file) {
				std::string themeName{};
				wxColour clrDataBuff[maxNumberOfColours] = {};
				size_t idx{ 0 };
				std::string ln;
				std::getline(file, ln);

				// If no theme data was found in current file, go check the next file
				if (ln.find_first_of("<Theme>") == std::string::npos) {
					continue;
				}

				ssize_t pos{}; // previous file position
				while (std::getline(file, ln)) {
					if (ln.find("Name") != std::string::npos) {
						size_t begin{ ln.find_first_of('\"') };
						size_t end{ ln.find_last_of('\"') };
						themeName = ln.substr(begin + 1, (end - begin) - 1);
					}
					else if (ln.find('}') != std::string::npos) {
						break;
					} // Data validation 
					else if (ln.find("Colour data {") != std::string::npos) {
						pos = file.tellg(); // store previous position of file
						if (ValidateThemeFileData(file)) {
							// not sure about why ifstream might fail here.
							if (file.fail()) {
								file.close();
								file.open(entry.path(), std::ios::in);
							}
							file.seekg(pos); // Resume where we stopped off before data
											 // validation if it returned true.
							std::getline(file, ln);
						}
						else {
							throw "Error loading theme: colour data seems to be missing or corrupted.";
						}
					} // save data to colour buffer from file
					if (ln.find('#') != std::string::npos) {
						clrDataBuff[idx] = wxColour{ ln.c_str() };
						++idx;
					}
				}

				auto [a, b, c, d, e, f, g, h, i] = clrDataBuff;
				themes.push_back({ themeName ,a,b,c,d,e,f,g,h,i });
			} else {
				wxMessageBox("Failed to load theme.", "Error");
			}	
		}
	}
	catch (const std::exception& ex) {
		wxLogMessage(ex.what());
	}
}
// prototype
static int PGSortPlayers(wxPropertyGrid* propGrid, wxPGProperty* p1, wxPGProperty* p2);

GameWindow::GameWindow() : wxFrame(nullptr, wxID_ANY, "Tic Tac Toe V4", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER | wxMAXIMIZE_BOX))
{
#pragma region sizers and panels
	
	SetIcon(wxIcon("ico.png"));
	gamePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	//gamePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	masterSizer = new wxBoxSizer(wxHORIZONTAL);

	gameSizer = new wxBoxSizer(wxVERTICAL);
	buttonsSizer = new wxBoxSizer(wxHORIZONTAL);
	gameControlsSizer = new wxBoxSizer(wxHORIZONTAL);

	// This prevent tiles from flickering when user clicks/hovers on/around them.
	gamePanel->SetDoubleBuffered(true);

	boldToken = new wxFont(wxFontInfo(FromDIP(wxSize(0, 50))).Bold());
	buttonsFont = new wxFont(wxFontInfo(FromDIP(wxSize(0, 15))).Bold().Italic());

	gameGridSizer = new wxGridSizer(4,4, FromDIP(2), FromDIP(2));
#pragma endregion

	mainGrid = std::make_unique<GameGrid>(this, 4, 4);

#pragma region controls
	menuButton = new wxButton(gamePanel, wxID_ANY, "Menu", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	turnIndicator = new wxButton(gamePanel, wxID_ANY, "-", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	resetButton = new wxButton(gamePanel, wxID_ANY, "Clear", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);

	resetButton->Bind(wxEVT_BUTTON, &GameWindow::OnResetButtonClicked, this);
	resetButton->SetFont(*buttonsFont);
	turnIndicator->SetFont(wxFont(wxFontInfo(resetButton->GetFont().GetPixelSize())).Bold());
	menuButton->Bind(wxEVT_BUTTON, &GameWindow::OnMenuButtonClicked, this);
	menuButton->SetFont(*buttonsFont);

	playerWinDialog = new wxMessageDialog(gamePanel, "", "", wxYES_NO | wxCANCEL | wxICON_EXCLAMATION);
	
	playerCountError = new wxMessageDialog(gamePanel, "", "Config error", wxOK | wxICON_ERROR);
	badGridSizeError = new wxMessageDialog(gamePanel, "Grid size is too large or too small", "Config error", wxOK | wxICON_ERROR);
	confirmResetDialog = new wxMessageDialog(gamePanel, "Are you sure you want to reset/change the grid?", "Confirm?", wxYES_NO | wxICON_QUESTION);
	gameTieDialog = new wxMessageDialog(gamePanel, "Would you like to start a new game?", "Game Tie!", wxYES_NO | wxICON_ASTERISK);

	gameConfigMenu = new wxPropertyGrid(gamePanel, wxID_ANY, wxDefaultPosition, wxDefaultSize);
// Theme stuff
	wxPGProperty* themesProperty = new wxStringProperty("Theme", "THEME", "Select theme");
	LoadThemesFromDir();
	themeChoices = new wxArrayString();
	themeChoices->reserve(themes.size());
	for (const auto& theme : themes)
		themeChoices->push_back(theme.themeName);

	wxPGChoices themes(*themeChoices);

	themesProperty->SetChoices(themes);
	themesProperty->SetChoiceSelection(0);

	gameConfigMenu->Append(new wxPropertyCategory("Colors and Themes", "CLR_THEME"));
	gameConfigMenu->Append(new wxColourProperty("Background color", "BGCOLOR"));
	gameConfigMenu->Append(new wxColourProperty("Tiles color", "TILECOLOR"));
	gameConfigMenu->Append(new wxColourProperty("Menu margins color", "MNU_MARGIN_CLR"));
	gameConfigMenu->Append(new wxColourProperty("Line seperator colors", "MNU_LINE_SEP_CLR"));
	gameConfigMenu->Append(new wxColourProperty("Cells background color", "CELLBGCLR"));
	gameConfigMenu->Append(new wxColourProperty("Cells text color", "CELL_TXT_CLR"));
	gameConfigMenu->Append(new wxColourProperty("Cell selected color", "CELL_SELECTED"));
	gameConfigMenu->Append(new wxColourProperty("Cell selected text color", "CELL_SELECTED_TXT_CLR"));
	gameConfigMenu->Append(new wxColourProperty("Categories text color", "CAT_PROP_TEXT_CLR"));

	gameConfigMenu->SetWindowStyle(wxBORDER_NONE);
	resetButton->SetBackgroundColour(*wxRED);
	menuButton->SetBackgroundColour(*wxBLUE);
	gameConfigMenu->SetEmptySpaceColour(wxColor(64, 64, 64));

	gameConfigMenu->Append(themesProperty);
	themesProperty->SetEditor(new ThemeSelectionProperty(this));
//
// Game configuration category
	auto gameConfigCategory = new wxPropertyCategory("Game Configuration", "GAMECFG");
	gameConfigMenu->Append(gameConfigCategory);
	
	wxArrayString gridSizeChoices{
		"3 (9 tiles)",
		"4 (16 tiles)",
		"5 (25 tiles)",
		"6 (36 tiles)",
		"7 (49 tiles)",
		"8 (64 tiles)",
		"9 (81 tiles)",
		"10 (100 tiles)"};
	auto gridSizeProp{ new wxEnumProperty{"Grid size", "GRIDSIZE", gridSizeChoices}};
	gameConfigMenu->Append(gridSizeProp);
	gridSizeProp->SetChoiceSelection(1);
	// 2 == default player count.
	wxPGProperty* playerProperties = new wxIntProperty("Player count", "PLAYERCOUNT", 2);
//
// Editor stuff
	wxPGEditor* playersInfoProp = wxPropertyGrid::RegisterEditorClass(new PlayersInfoProperty(this));
//
	playersPropertyCategory = new wxPropertyCategory("Players", "PLAYERS");

	addPlayerDialog = new PlayerPropertyDialog(this, wxID_ANY, "Add player");

	gameConfigMenu->Append(playerProperties);
	gameConfigMenu->AppendIn(gameConfigCategory, playersPropertyCategory);
	gameConfigMenu->Append(new wxPropertyCategory("", "EMPTY"));

	playerProperties->SetEditor(playersInfoProp);

	// for reference
	// int todip = ToDIP(500); // division by scaling factor
	// int fromdip = FromDIP(500); // multiply by scaling factor

	gameConfigMenu->Bind(wxEVT_PG_CHANGED, &GameWindow::GameConfigEventHandler, this);
	// gameConfigMenu->Bind(wxEVT_PG_SELECTED, [this](wxPropertyGridEvent& evt) {gameConfigMenu->SetupColours(); });
	gameTieDialog->SetYesNoLabels("Yes", "Quit");

	gameConfigMenu->SetFont(wxFont(wxFontInfo(wxSize(0, FromDIP(20))).Bold().Italic()));
	// Arbitrary numbers
	gameConfigMenu->SetMinSize(wxSize(FromDIP(625), FromDIP(375)));
	gameConfigMenu->SetSortFunction(&PGSortPlayers);
#pragma endregion

// enable debug buttons and code
#ifdef X_DEBUG
	//Everything here is for debug controls only.
	
	debugSizer = new wxBoxSizer(wxVERTICAL);
	//gameSizer->Add(debugSizer, wxGBPosition(0,0), wxGBSpan(1,1), wxALL, 5);
	
	displayCoordinates = new wxButton(gamePanel, DEBUG_BUTTON_DISPLAY_COORDS, "Display coordinates", wxDefaultPosition, wxDefaultSize);
	displaySubscriptCoords = new wxButton(gamePanel, DEBUG_BUTTON_DISPLAY_SUBSCR, "Display array subscript coord", wxDefaultPosition, wxDefaultSize);

	displayCoordinates->Bind(wxEVT_BUTTON, &GameWindow::DisplayCoordinatesOnGrid, this);
	displaySubscriptCoords->Bind(wxEVT_BUTTON, &GameWindow::DisplayCoordinatesOnGrid, this);

	highlightAbove = new wxButton(gamePanel, DEBUG_UP, "Up", wxDefaultPosition, wxDefaultSize);
	highlightBelow = new wxButton(gamePanel, DEBUG_DOWN, "Down", wxDefaultPosition, wxDefaultSize);
	highlightLeft = new wxButton(gamePanel, DEBUG_LEFT, "Left", wxDefaultPosition, wxDefaultSize);
	highlightRight = new wxButton(gamePanel, DEBUG_RIGHT, "Right", wxDefaultPosition, wxDefaultSize);
	highlightDiagUp = new wxButton(gamePanel, DEBUG_DIAG_UP, "Diagonal up", wxDefaultPosition, wxDefaultSize);
	highlightDiagDown = new wxButton(gamePanel, DEBUG_DIAG_DOWN, "Diagonal down", wxDefaultPosition, wxDefaultSize);
	highlightDiagUpBackw = new wxButton(gamePanel, DEBUG_DIAG_UP_BACKW, "Backwards diagonal up", wxDefaultPosition, wxDefaultSize);
	highlightDiagDownBackw = new wxButton(gamePanel, DEBUG_DIAG_DOWN_BACKW, "Backwards diagonal down", wxDefaultPosition, wxDefaultSize);

	highlightAbove->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightBelow->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightLeft->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightRight->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightDiagUp->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightDiagDown->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightDiagUpBackw->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);
	highlightDiagDownBackw->Bind(wxEVT_BUTTON, &GameSession::HighlightAdjacentTile);

	debugSizer->Add(displayCoordinates, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(displaySubscriptCoords, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightAbove, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightBelow, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightLeft, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightRight, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightDiagUp, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightDiagDown, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightDiagUpBackw, 0, wxEXPAND | wxALL, ToDIP(5));
	debugSizer->Add(highlightDiagDownBackw, 0, wxEXPAND | wxALL, ToDIP(5));

	gameSizer->Add(debugSizer, itemPosAndSizeData[DBGSIZER].first, itemPosAndSizeData[DBGSIZER].second, wxEXPAND | wxTOP, ToDIP(5));
#endif
// sizer stuff
	gameSizer->Add(gameGridSizer, 0, wxTOP, ToDIP(5));
	
	buttonsSizer->Add(resetButton, 1, wxBOTTOM | wxRIGHT, ToDIP(5));
	buttonsSizer->Add(turnIndicator, 1, wxBOTTOM, ToDIP(5));
	buttonsSizer->Add(menuButton, 1, wxBOTTOM | wxLEFT, ToDIP(5));

	gameSizer->Add(buttonsSizer, 1, wxTOP | wxEXPAND, ToDIP(5));
	gameGridSizer->SetSizeHints(this);
	
	masterSizer->Add(gameSizer, 0, wxLEFT | wxRIGHT, ToDIP(5));
	masterSizer->Add(gameConfigMenu, 0, wxEXPAND | wxTOP, ToDIP(5));

	// arbitrary resolution. Overridden by sizer's hints
	SetSize(FromDIP(wxSize(1920, 1080)));

	gamePanel->SetFont(*boldToken);
	gamePanel->SetBackgroundColour(wxColor(64,64,64));
	gamePanel->SetSizerAndFit(masterSizer);

	masterSizer->SetSizeHints(this);

	gameConfigMenu->CenterSplitter();
	this->SetTheme(currentTheme);
}

void GameWindow::OnResetButtonClicked(wxCommandEvent& evt)
{
	switch (confirmResetDialog->ShowModal())
	{
	case wxID_NO:
		return;
	case wxID_YES:
		GameSession::gameData.gridWasReset = true;
		mainGrid->ResetGrid();
		const Player& firstPlayer{ *GameSession::players.begin() };
		GameSession::rgb.get()->StopRainbowEffect();
		this->SetPlayerTurnLabel(firstPlayer.GetLabel(), firstPlayer.GetLabelColour());
		GameSession::gameData.winTiles.clear();
	}
}

void GameWindow::SetTheme(const ThemeColorData& theme)
{
	// freeze and thaw window to prevent flickering and artifacts
	this->Freeze();
	gamePanel->SetBackgroundColour(theme.bgCol);
	gameConfigMenu->SetEmptySpaceColour(theme.bgCol);
	mainGrid->SetTileColors(theme.tilesCol);
	gameConfigMenu->SetMarginColour(theme.pgMarginCol);
	gameConfigMenu->SetCaptionBackgroundColour(theme.pgMarginCol);
	gameConfigMenu->SetLineColour(theme.pgLineSepCol);
	gameConfigMenu->SetCellBackgroundColour(theme.pgCellsBgCol);
	gameConfigMenu->SetCellTextColour(theme.pgCellsTextCol);
	gameConfigMenu->SetSelectionBackgroundColour(theme.pgSelectedCellCol);
	gameConfigMenu->SetSelectionTextColour(theme.pgSelectedCellTextCol);
	gameConfigMenu->SetCaptionTextColour(theme.pgCatPropTextCol);

	resetButton->SetBackgroundColour(theme.pgMarginCol);
	resetButton->SetForegroundColour(theme.pgCellsTextCol);
	menuButton->SetBackgroundColour(theme.pgCatPropTextCol);
	menuButton->SetForegroundColour(theme.pgCellsTextCol);

	gameConfigMenu->GetProperty("BGCOLOR")->SetValueFromString(theme.bgCol.GetAsString());
	gameConfigMenu->GetProperty("TILECOLOR")->SetValueFromString(theme.tilesCol.GetAsString());
	gameConfigMenu->GetProperty("MNU_MARGIN_CLR")->SetValueFromString(theme.pgMarginCol.GetAsString());
	gameConfigMenu->GetProperty("MNU_LINE_SEP_CLR")->SetValueFromString(theme.pgLineSepCol.GetAsString());
	gameConfigMenu->GetProperty("CELLBGCLR")->SetValueFromString(theme.pgCellsBgCol.GetAsString());
	gameConfigMenu->GetProperty("CELL_TXT_CLR")->SetValueFromString(theme.pgCellsTextCol.GetAsString());
	gameConfigMenu->GetProperty("CELL_SELECTED")->SetValueFromString(theme.pgSelectedCellCol.GetAsString());
	gameConfigMenu->GetProperty("CELL_SELECTED_TXT_CLR")->SetValueFromString(theme.pgSelectedCellTextCol.GetAsString());
	gameConfigMenu->GetProperty("CAT_PROP_TEXT_CLR")->SetValueFromString(theme.pgCatPropTextCol.GetAsString());

	currentTheme = theme;
	this->Thaw();
}

bool GameWindow::ValidatePlayerCount(int num, size_t gridSizeSqrt)
{
	/*
	* 3*3 -> 2p
	* 4*4 -> 2p
	* 5*5 -> 3p
	* 6*6 -> 4p
	* 7*7 -> 5p
	* 8*8 -> 6p
	* 9*9 -> 7p
	* 10*10 -> 8p
	*/
	
	if (num < GameSession::minPlayerCount || num > GameTile::labels.size())
	{
		if (num < GameSession::minPlayerCount)
			playerCountError->SetMessage("Player count must be atleast two");
		else if (num > GameTile::labels.size())
			playerCountError->SetMessage("Number of players cannot exceed 8 players");

		playerCountError->ShowModal();
		return false;
	}

	if (gridSizeSqrt == std::sqrt(GameGrid::minTilesInGrid) && num > GameSession::minPlayerCount) {
		wxMessageBox("Grid is too small for this playercount");
		return false;
	}
	else if (gridSizeSqrt >= 4 && gridSizeSqrt - num < GameSession::minPlayerCount) {
		wxMessageBox("Grid is too small for this playercount");
		return false;
	}
	return true;
}

static void* GetPlayerPtr(const wxString& name)
{
	const auto it{std::find_if(GameSession::players.begin(), GameSession::players.end(),
		[&name](const auto& a){
			return static_cast<bool>(!a.GetName().compare(name));
		})};
	
	return &(*it);
}

void GameWindow::AppendPlayerToPlayerList(const Player& player)
{
	wxPropertyGridEvent* evt = new wxPropertyGridEvent(wxEVT_PG_CHANGED, wxID_ANY);
	evt->m_property = gameConfigMenu->GetProperty("PLAYERCOUNT");
	evt->m_propertyName = evt->m_property->GetName();
	evt->m_pg = gameConfigMenu;
	evt->SetClientData(GetPlayerPtr(player.GetName()));
	wxQueueEvent(gameConfigMenu, evt);
}

void GameWindow::SetPlayerTurnLabel(const wchar_t& label, const wxColour& clr)
{
	turnIndicator->SetLabel(label);
	turnIndicator->SetForegroundColour(clr);
}

inline bool GameWindow::VerifyNewSetName(const wxString& name)
{
	for (const auto& player : GameSession::players) {
		if (!player.GetName().compare(name)) {
			wxMessageBox("This name is already taken.");
			return false;
		}
	}
	return true;
}

bool GameWindow::VerifyNewSetColour(const wxColour& clr, bool verifyTheme)
{
	for (const auto& player : GameSession::players) {
		if (player.GetLabelColour() == clr) {
			wxMessageBox(verifyTheme ? "Current theme selection contains tile colours similar to one of the players. Please change label colour or select another theme" : "This colour is already taken by another player.");
			return false;
		}
		else if (clr == currentTheme.tilesCol && !verifyTheme) {
			wxMessageBox("Label won't be visible on tiles as they are the same colour.");
			return false;
		}
	}
	return true;
}

inline wxBitmap GameWindow::CreateValImageForPlayer(const wxColour& clr)
{
	// Bitmap width, height, and color depth
	constexpr const int bmpw{16}, bmph{27}, bps{24};
	wxBitmap bmp(FromDIP(bmpw), FromDIP(bmph), bps);
	wxNativePixelData data{ bmp };
	wxNativePixelData::Iterator mainit{ data };
	const int width{ data.GetWidth() };
	const int height{ data.GetHeight() };

	for (int y{ 0 }; y < height; ++y) {
		auto row = mainit;
		for (int x{ 0 }; x < width; ++x, ++mainit) {
			if (!x || !y || x == width - 1 || y == height - 1) {
				mainit.Red() = 0;
				mainit.Green() = 0;
				mainit.Blue() = 0;
				continue;
			}
			mainit.Red() = clr.Red();
			mainit.Green() = clr.Green();
			mainit.Blue() = clr.Blue();
		}
		mainit = row;
		mainit.OffsetY(data, 1);
	}

	return bmp;
}

void GameWindow::SortPlayers()
{
	gameConfigMenu->Freeze();
	gameConfigMenu->SortChildren(gameConfigMenu->GetProperty("PLAYERS"));
	gameConfigMenu->Thaw();
}

static int PGSortPlayers(wxPropertyGrid* propGrid, wxPGProperty* p1, wxPGProperty* p2)
{
	auto pl1{reinterpret_cast<Player*>(p1->GetClientData())};
	auto pl2{reinterpret_cast<Player*>(p2->GetClientData())};

	// should return a positive value if p1 should go after p2,
	// negative if p1 should go before p2 and 0 if they are equivalent

	if (pl1->GetScore() > pl2->GetScore()) { return -1; }
	if (pl1->GetScore() < pl2->GetScore()) { return 1; }
	return 0;
}

void GameWindow::GameConfigEventHandler(wxPropertyGridEvent& evt)
{
	wxPGProperty* prop = evt.GetProperty();
	wxAny val = prop->GetValue();

	if (val.IsNull())
		return;

	// all individual theme related events
	if (val.CheckType<wxColour>())
	{
		const auto clr = val.As<wxColour>();
		if (!evt.GetPropertyName().compare("BGCOLOR")) {
			gamePanel->SetBackgroundColour(clr);
			gameConfigMenu->SetEmptySpaceColour(clr);
			currentTheme.bgCol = clr;
		}
		else if (!evt.GetPropertyName().compare("TILECOLOR")) {
			mainGrid->SetTileColors(clr);
			currentTheme.tilesCol = clr;
		}
		else if (!evt.GetPropertyName().compare("MNU_MARGIN_CLR")) {
			gameConfigMenu->SetMarginColour(clr);
			gameConfigMenu->SetCaptionBackgroundColour(clr);
			currentTheme.pgMarginCol = clr;
		}
		else if (!evt.GetPropertyName().compare("MNU_LINE_SEP_CLR")) {
			gameConfigMenu->SetLineColour(clr);
			currentTheme.pgLineSepCol = clr;
		}
		else if (!evt.GetPropertyName().compare("CELLBGCLR")) {
			gameConfigMenu->SetCellBackgroundColour(clr);
			currentTheme.pgCellsBgCol = clr;
		}
		else if (!evt.GetPropertyName().compare("CELL_TXT_CLR")) {
			gameConfigMenu->SetCellTextColour(clr);
			currentTheme.pgCellsTextCol = clr;
		}
		else if (!evt.GetPropertyName().compare("CAT_PROP_TEXT_CLR")) {
			gameConfigMenu->SetCaptionTextColour(clr);
			currentTheme.pgCatPropTextCol = clr;
		}
		else if (!evt.GetPropertyName().compare("CELL_SELECTED")) {
			gameConfigMenu->SetSelectionBackgroundColour(clr);
			currentTheme.pgSelectedCellCol = clr;
		}
		else if (!evt.GetPropertyName().compare("CELL_SELECTED_TXT_CLR")) {
			gameConfigMenu->SetSelectionTextColour(clr);
			currentTheme.pgSelectedCellTextCol = clr;
		}
	}
	// Remove player or change player label colour
	if (!prop->GetParent()->GetName().compare("PLAYERS")) {
		Player& ply{ *reinterpret_cast<Player*>(prop->GetClientData()) };

		if (evt.GetId() == PlayerPGEditor::ID_PLAYER_REMOVE
			&& ValidatePlayerCount(GameSession::players.size() - 1, mainGrid->GetNumOfRows())) {
			
			if (confirmResetDialog->ShowModal() == wxID_NO) { return; }

			prop->SetName("PLAYER_DEL");
			gameConfigMenu->DeleteProperty(prop);
			mainGrid->ResetGrid();

			size_t offset{ 0 };
			for ( ; offset < GameSession::players.size(); ++offset) {
				if (!GameSession::players[offset].GetName().compare(ply.GetName())) {
					break;
				}
			}

			auto it = (GameSession::players.begin() + offset);
			std::swap(*it, GameSession::players.back());
			GameSession::players.pop_back();
			this->SortPlayers();

			int newPlayersCount = GameSession::players.size();
			gameConfigMenu->GetProperty("PLAYERCOUNT")->SetValueFromInt(newPlayersCount);
			GameSession::gameData.numOfPlayers = newPlayersCount;
			
			return;
		} else if (ply.dontDeleteMovedTo) {
			ply.dontDeleteMovedTo = false;
		}
		else if (evt.GetId() == PlayerPGEditor::ID_PLAYER_CHANGE_COLOUR) {
			wxColour clr{ *reinterpret_cast<wxColour*>(evt.GetClientData()) };
			if (VerifyNewSetColour(clr)) {
				prop->SetValueImage(CreateValImageForPlayer(clr));
				prop->SetValue(clr.GetAsString());
				mainGrid->ChangePlayerLabelColour(ply, clr);
			}
		} else if (evt.GetId() == PlayerPGEditor::ID_PLAYER_RENAME) {
			wxString labelName{ prop->GetLabel() };
			size_t start = labelName.find_first_of('[');
			size_t end = labelName.find_first_of(']');
			wxTextEntryDialog newNameDlg{ this, "Enter new name", "New name" };
			labelName.erase(0, start);
			if (newNameDlg.ShowModal() == wxID_OK) {
				const wxString& newName{ newNameDlg.GetValue() };
				if (VerifyNewSetName(newName)) {
					prop->SetLabel(newName
					+ (" (")
					+ (ply.GetLabel())
					+ (")\t")
					+ (labelName));
					ply.SetName(newName);
				}
			}
		}
	} // resize grid
	else if (!evt.GetPropertyName().compare("GRIDSIZE"))
	{
		int newSize = std::stoi(val.As<wxString>().ToStdString());
		if (mainGrid->ResizeGrid(newSize + 3, newSize + 3))
			;
			//prop->SetValue((std::to_string(newSize) +
			//" (" + std::to_string(newSize * newSize) + " tiles)").c_str());
	} // Add new player
	else if (!evt.GetPropertyName().compare("PLAYERCOUNT"))
	{
		int newPlayersCount = GameSession::players.size();
		prop->SetValueFromInt(newPlayersCount);
		GameSession::gameData.numOfPlayers = newPlayersCount;
			
		const auto player = reinterpret_cast<Player*>(evt.GetClientData());
		const auto& color = player->GetLabelColour();

		auto tempStr{((player->GetName() + wxString{" ("} + player->GetLabel() + wxString{")"}).c_str()).AsString()};

		auto newProp{ new wxStringProperty(tempStr + "\t[ 0 wins ]",
			(std::string{ "PLAYER_" } + std::to_string(player->GetPlayerId())).c_str()) };
		newProp->SetClientData(evt.GetClientData());

		newProp->SetEditor(new PlayerPGEditor(this, newProp));

		gameConfigMenu->AppendIn(playersPropertyCategory, newProp);
		newProp->SetValueImage(CreateValImageForPlayer(color));
		newProp->SetValue(color.GetAsString());

		//gameConfigMenu->SortChildren(prop);
		this->SortPlayers();
	} // Apply a theme
	else if (!evt.GetPropertyName().compare("THEME")) {
		int idx = val.As<int>();
		auto theme{ themes[idx] };
		if (!VerifyNewSetColour(theme.tilesCol, true)) {
			prop->SetValue(currentTheme.themeName);
			return;
		}
		prop->SetValue(prop->GetChoices()[idx].GetText());
		SetTheme(theme);
		addPlayerDialog->SetTheme(theme);
		currentTheme = themes[idx];
	}

	gamePanel->Refresh();
}

static bool hide{ true };
void GameWindow::OnMenuButtonClicked(wxCommandEvent& evt)
{
	this->Freeze();
	if (hide) {
		this->masterSizer->Hide(1);
		//this->gameSizer->SetSizeHints(this); // buggy
		//this->masterSizer->SetSizeHints(this);
		hide = false;
	}
	else {
		this->masterSizer->Show(1, true);
		//this->masterSizer->SetSizeHints(this);
		hide = true;
	}
	this->masterSizer->SetSizeHints(this);
	this->masterSizer->Layout();
	this->Thaw();
}

// enable debug buttons and code
#ifdef X_DEBUG
void GameWindow::DisplayCoordinatesOnGrid(wxCommandEvent& evt)
{
	size_t outer = 0;
	for (auto& row : GameGrid::tiles)
	{
		for (size_t i = 0; i < row.size(); ++i)
		{
			if (evt.GetId() == DEBUG_BUTTON_DISPLAY_COORDS)
				row[i]->SetLabel((std::to_string(i) + ',' + std::to_string(outer)).c_str());
			else
				row[i]->SetLabel((std::to_string(outer) + ',' + std::to_string(i)).c_str());
		}
		++outer;
	}
}
#endif

void GameWindow::GameOverTie()
{
	switch (gameTieDialog->ShowModal())
	{
	case wxID_YES:
		mainGrid->ResetGrid();
		break;
	case wxID_NO:
		Close();
	}
}
