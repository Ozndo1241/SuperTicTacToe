#pragma once
#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/editors.h>

class GameWindow;

class PlayersInfoProperty : public wxPGTextCtrlEditor
{
	wxDECLARE_DYNAMIC_CLASS(PlayersInfoProperty);
public:
	PlayersInfoProperty() {}
	PlayersInfoProperty(GameWindow* parent) : parentWindow{ (parent ? parent : nullptr) } {}
	virtual ~PlayersInfoProperty() {}

	virtual wxString GetName() const wxOVERRIDE { return "PlayerPropertyEditor"; }
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz) const wxOVERRIDE;
	virtual bool OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const wxOVERRIDE;
private:
	GameWindow* parentWindow;
};