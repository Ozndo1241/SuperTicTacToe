#pragma once
#include <wx/wx.h>
#include <wx/collpane.h>
#include <wx/clrpicker.h>
#include "GameWindow.h"

class PlayerPropertyDialog : public wxDialog
{
public:

	PlayerPropertyDialog() {}
	virtual ~PlayerPropertyDialog() {}
	PlayerPropertyDialog(wxWindow* parent,
		wxWindowID id = -1,
		const wxString& title = "",
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& sz = wxDefaultSize,
		long style = wxCAPTION | wxCLOSE_BOX | wxWANTS_CHARS);
	void SetTheme(const GameWindow::ThemeColorData& theme);
private:
	void OnAddPlayerClicked(wxCommandEvent& evt);
	
	static constexpr const int rows{1};
	static constexpr const int cols{8};
	static constexpr const int vgap{5};
	static constexpr const int hgap{5};
	
	wxBoxSizer* masterSizer;
	wxBoxSizer* ctrlsSizer;
	wxBoxSizer* ctrlsSizer2;
	wxGridSizer* buttonsSizer;

	wxTextCtrl* playerNameTxtCtrl;
	wxButton* addPlayerButton;
	wxStaticText* labelsText;
	wxColourPickerCtrl* clrCtrl;

	wxButton* evtButton;
	wxButton* prevButton;
};