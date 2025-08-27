#pragma once

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameWindow.h")
#endif

#include "GameSession.h"
#include "PlayerPGEditor.h"
#include "GameGrid.h"
#include <functional>

#include <memory>
#include <string>
#include <wx/wx.h>
#include <wx/gbsizer.h>
#include <wx/propgrid/propgrid.h>
#include <wx/graphics.h>
#include <wx/dc.h>

class GameTile;
class GameGrid;
class PlayersInfoProperty;
class PlayerPropertyDialog;
class PlayerPGproperty;

class GameWindow : public wxFrame
{
public:
	GameWindow();
	std::unique_ptr<GameGrid> mainGrid;
	void GameOverTie();
	bool ValidatePlayerCount(int num, size_t gs);
	void AppendPlayerToPlayerList(const Player& player);
	void SetPlayerTurnLabel(const wchar_t& label, const wxColour& clr);
	// This is used only for already existing players who
	// might want to change their label colour or to prevent
	// users from selecting a theme which has the same tile
	// colors as any of the players
	inline bool VerifyNewSetColour(const wxColour& clr, bool verifyTheme = false);
	inline bool VerifyNewSetName(const wxString& name);

	struct ThemeColorData {
		wxString themeName;

		wxColour bgCol{};
		wxColour tilesCol{};
		wxColour pgMarginCol{};
		wxColour pgLineSepCol{};
		wxColour pgCellsBgCol{};
		wxColour pgCellsTextCol{};
		wxColour pgSelectedCellCol{};
		wxColour pgSelectedCellTextCol{};
		wxColour pgCatPropTextCol{};

		bool isBuiltIn{};
	};

	inline void SortPlayers();
	inline const ThemeColorData* GetCurrentTheme() const { return &currentTheme; }
	inline std::vector<ThemeColorData>& GetThemesList() const { return themes; }
	inline wxArrayString* GetPGThemesChoices() const { return themeChoices; }
	inline wxPropertyGrid* GetGameMenu() const { return gameConfigMenu; }
	inline void SetTheme(const ThemeColorData& theme);

private:
	friend class GameTile;
	friend class GameGrid;
	friend class PlayersInfoProperty;
	friend class PlayerPropertyDialog;
	friend class PlayerPGEditor;
	friend bool GameSession::CheckWinCondition(const GameTile* tile);

	void GameConfigEventHandler(wxPropertyGridEvent& evt);
	void OnMenuButtonClicked(wxCommandEvent& evt);
	void OnResetButtonClicked(wxCommandEvent& evt);

	enum ControlsId
	{
		DBGSIZER,
		GAMEGRID,
		GAMECONF
	};

	inline bool ValidateThemeFileData(std::ifstream& file);
	inline void LoadThemesFromDir();
public:
	inline wxBitmap CreateValImageForPlayer(const wxColour& clr);
private:

	static constexpr const size_t colorDataCharacterLength{7};
	static constexpr const size_t maxNumberOfColours{9};
	// It's static inline because of the
	// static variable initialization order fiasco
	static inline std::vector<ThemeColorData> themes
	{{
		{"Default", "#404040", "#c0c0c0", "#b0b0b0", "#000040", "#ffffff", "#404040", "#202020", "#f0f0f0", "#606060", true},
		{"Aqua", "#243642", "#629584", "#387478", "#007171", "#77B0AA", "#35374B", "#88a2a8", "#2c5858", "#e3fef7", true},
		{"Purple-peach" ,"#49243e", "#bb8493", "#704264", "#400000", "#dbafa0", "#a65353", "#804040", "#400040", "#400040", true},
		{"Deep night", "#070f2b", "#1b1a55", "#535c91", "#424769", "#9290c3", "#2d3250", "#3535ff", "#d7d7ff", "#030637", true},
		{"Froggzy", "#c1cfa1", "#a5b68d", "#b17f59", "#b9b28a", "#ede8dc", "#bdb395", "#d5c7a3", "#504b38", "#723838", true},
		{"Pastel-pearl", "#adb2d4", "#c7d9dd", "#d5e5d5", "#c6e7ff", "#eef1da", "#a6aebf", "#c6e7ff", "#789dbc", "#89a8b2", true}
	}};

	const std::vector<std::pair<wxGBPosition, wxGBSpan>> itemPosAndSizeData
	{
		{{0,0}, {1,1}}, // DBG SIZER
		{{0,1}, {1,1}}, // GAMEGRID
		{{0,2}, {4,4}}  // GAMECONFIG
	};

	//GC_test* gamePanel;
	wxPanel* gamePanel;
public:
	wxFont* boldToken;
private:
	wxFont* buttonsFont;
	wxBoxSizer* masterSizer;
	wxGridSizer* gameGridSizer;
	wxBoxSizer* gameSizer;
	wxBoxSizer* buttonsSizer;

	wxArrayString* themeChoices;
	ThemeColorData currentTheme;
	
	wxButton* menuButton;
	wxButton* turnIndicator;
	wxButton* resetButton;
	wxBoxSizer* gameControlsSizer;

	PlayerPropertyDialog* addPlayerDialog;
	wxMessageDialog* playerWinDialog;
	wxMessageDialog* playerCountError;
	wxMessageDialog* badGridSizeError;
	wxMessageDialog* confirmResetDialog;
	wxMessageDialog* gameTieDialog;

	wxPropertyGrid* gameConfigMenu;
	wxPropertyCategory* playersPropertyCategory;

// Change to your compiler's debug preprocessor define
// to enable debug buttons and code
#ifdef X_DEBUG
public:
	enum DebugButtonId
	{
		DEBUG_UP = 1,
		DEBUG_DOWN,
		DEBUG_LEFT,
		DEBUG_RIGHT,
		DEBUG_DIAG_UP,
		DEBUG_DIAG_DOWN,
		DEBUG_DIAG_UP_BACKW,
		DEBUG_DIAG_DOWN_BACKW,
		DEBUG_MAX_DIR,

		DEBUG_BUTTON_DISPLAY_COORDS,
		DEBUG_BUTTON_DISPLAY_SUBSCR,
		
	};
private:
	void DisplayCoordinatesOnGrid(wxCommandEvent& evt);
	//wxPanel* debugPanel;
	wxBoxSizer* debugSizer;
	
	wxButton* displayCoordinates;
	wxButton* displaySubscriptCoords;
	wxButton* highlightAbove;
	wxButton* highlightBelow;
	wxButton* highlightLeft;
	wxButton* highlightRight;
	wxButton* highlightDiagUp;
	wxButton* highlightDiagDown;
	wxButton* highlightDiagUpBackw;
	wxButton* highlightDiagDownBackw;
#endif

};