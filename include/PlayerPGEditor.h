#pragma once
#include <wx/propgrid/editors.h>
#include <wx/colordlg.h>
#include <wx/propgrid/property.h>
#include "GameWindow.h"
#include "Player.h"

class PlayerPGEditor : public wxPGTextCtrlEditor
{
public:
	enum PlayerPropIDs {
		ID_PLAYER_CHANGE_COLOUR = 1,
		ID_PLAYER_REMOVE,
		ID_PLAYER_RENAME
	};
	PlayerPGEditor() {}
	PlayerPGEditor(GameWindow* parent, wxPGProperty* prop) : parentwnd{ parent },
		parentProp{ prop },
		player{reinterpret_cast<Player*>(prop->GetClientData())} {}

	virtual ~PlayerPGEditor() { parentProp->SetClientData(nullptr); delete (Player*)parentProp->GetClientData(); }
	// non-const member function
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz) const override;
	virtual wxString GetName() const override { return "playerPGEditor"; }
private:

	Player* player;
	wxPGProperty* parentProp;
	GameWindow* parentwnd;
};

