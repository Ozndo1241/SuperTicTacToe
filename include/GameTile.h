#pragma once
#include <wx/button.h>
#include <array>

#ifdef PLAYERPROPEDR_HEADER
#pragma message("Defined in GameTile.h")
#endif

class GameGrid;
class GameWindow;

class GameTile : public wxButton
{
public:
	enum Labels_ID
	{
		LABEL_FANCY_X = 1,
		LABEL_CIRCLE,
		LABEL_SQUARE,
		LABEL_TRINE,
		LABEL_HEXAGON,
		LABEL_DIAMOND,
		LABEL_MOON,
		LABEL_HEART,

		LABELS_MAX
	};

	inline static std::array<wchar_t, 8> labels
	{
		0x2718, // Fancy looking 'X'
		0x25EF, // Circle (hollow)
		0x2610, // Another hollow square.
		0x25B3, // Trine
		0x232C, // Hexagon (molecular)
		0x25C8, // WHITE DIAMOND CONTAINING OPAQUE SMALL DIAMOND
		0x263E,	// Moon
		0x2661, // Heart <3
		//0x2764, // Bold heart

		//! These below turn into emojis when the manifest file
		//! is present in the program directory (Microsoft Windows)
	 
		// 0x2744, // Snowflake
		// 0x2600, // Sun
		// 0x2618, // Better clover
		// 0x262F  // Yin Yang*/
	};
	
	GameTile(wxWindow* parent, wxWindowID id, const wxString& label, const wxPoint& tilePos, const wxPoint& posOnScreen = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
	GameTile(const GameTile& other) noexcept;

	GameTile* GetAdjacentTile(int direction) const;
	// returns the tile position in grid and not in screen coordinates
	const wxPoint& GetTilePosition() const { return tilePosition; }
	bool IsEdgeTile() const;
	virtual ~GameTile() = default;

private:
	void OnPaint(wxPaintEvent& evt);

	wxPoint tilePosition;
	friend class GameWindow;
};
