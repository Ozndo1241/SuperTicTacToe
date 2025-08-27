#pragma once
#include "GameWindow.h"
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/editors.h>

void SaveThemeToFile(wxCommandEvent& evt);
void RemoveThemeFromDir(wxCommandEvent& evt);

class ThemeSelectionProperty : public wxPGChoiceEditor
{
	wxDECLARE_DYNAMIC_CLASS(PlayersInfoProperty);
public:
	ThemeSelectionProperty() {}
	ThemeSelectionProperty(GameWindow* parent) : parentWnd{ parent ? parent : nullptr } {}
	virtual ~ThemeSelectionProperty() {}

	virtual wxString GetName() const wxOVERRIDE { return "ThemeSelectionProperty"; }
	virtual wxPGWindowList CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz) const wxOVERRIDE;
	virtual bool OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const wxOVERRIDE;
private:
	
	GameWindow* parentWnd{};
};

