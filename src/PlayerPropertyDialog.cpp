#include <string>
#include <wx/wx.h>
#include <wx/clrpicker.h>
#include "PlayerPropertyDialog.h"
#include "GameTile.h"
#include "GameSession.h"
#include "GameWindow.h"

PlayerPropertyDialog::PlayerPropertyDialog(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& sz,
	long style) : wxDialog(parent, id, title, pos, sz, style), evtButton{ nullptr }
{
	this->SetDoubleBuffered(true);
	this->SetBackgroundColour("#D3D3D3");
	
	masterSizer = new wxBoxSizer(wxVERTICAL);
	ctrlsSizer = new wxBoxSizer(wxHORIZONTAL);
	ctrlsSizer2 = new wxBoxSizer(wxHORIZONTAL);
	buttonsSizer = new wxGridSizer(rows, cols, vgap, hgap);

	playerNameTxtCtrl = new wxTextCtrl(this, wxID_ANY, "Enter player name", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
	addPlayerButton = new wxButton(this, wxID_ANY, "Add player", wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	playerNameTxtCtrl->Bind(wxEVT_TEXT_ENTER, &PlayerPropertyDialog::OnAddPlayerClicked, this);
	addPlayerButton->Bind(wxEVT_BUTTON, &PlayerPropertyDialog::OnAddPlayerClicked, this);

	labelsText = new wxStaticText(this, wxID_ANY, "Select color", wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	labelsText->SetFont(wxFont(wxFontInfo(FromDIP(wxSize(0, 15))).Bold()));
	labelsText->SetForegroundColour(wxColor(150, 150, 150));
	clrCtrl = new wxColourPickerCtrl(this, wxID_ANY, *wxBLACK, wxDefaultPosition, wxDefaultSize);

	int iterateEnum{ 1 };
	prevButton = nullptr;

	for (const wchar_t& label : GameTile::labels) {
		buttonsSizer->Add(new wxButton(this, static_cast<GameTile::Labels_ID>(iterateEnum), label,
			wxDefaultPosition, this->FromDIP(wxSize(75, 75)), wxBORDER_NONE), 0)->GetWindow()
			// Highlight the label clicked in the dialog
			->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) mutable
			{
				evtButton = static_cast<wxButton*>(evt.GetEventObject());
				if (prevButton) { 
					prevButton->SetForegroundColour(*wxBLACK);
					prevButton->SetFont(wxFont(wxFontInfo().Bold(false)));
				}
				prevButton = evtButton;
				evtButton->SetForegroundColour(clrCtrl->GetColour());
				evtButton->SetFont(wxFont(wxFontInfo(this->FromDIP(wxSize(0, 30))).Bold()));
				GameSession::gameData.newPlayerLabel = evtButton->GetLabel()[0];
			});

		++iterateEnum;
	}

	//buttonsSizer->AddSpacer(5);

	clrCtrl->Bind(wxEVT_COLOURPICKER_CHANGED, [this](wxColourPickerEvent& evt) {
		if (evtButton)
			evtButton->SetForegroundColour(clrCtrl->GetColour());
		});

	this->Bind(wxEVT_CLOSE_WINDOW, [](wxCloseEvent& evt) {
			GameSession::gameData.newPlayerLabel = '\0';
			evt.Skip();
		});

	ctrlsSizer->Add(playerNameTxtCtrl, 1, wxALL, FromDIP(5));
	ctrlsSizer->Add(addPlayerButton, 0, wxALL, FromDIP(5));

	ctrlsSizer2->Add(labelsText, 0, wxALL, FromDIP(5));
	ctrlsSizer2->Add(clrCtrl, 1, wxALL, FromDIP(5));

	masterSizer->Add(ctrlsSizer2, 0, wxEXPAND);
	masterSizer->Add(buttonsSizer, 0, wxLEFT | wxRIGHT, 5);
	masterSizer->Add(ctrlsSizer, 0, wxEXPAND);

	this->SetSizerAndFit(masterSizer);
	masterSizer->SetSizeHints(this);
	this->SetTheme(*static_cast<GameWindow*>(this->GetParent())->GetCurrentTheme());
}

void PlayerPropertyDialog::SetTheme(const GameWindow::ThemeColorData& theme)
{
	auto& buttons = buttonsSizer->GetChildren();
	addPlayerButton->SetBackgroundColour(theme.pgCellsBgCol);
	playerNameTxtCtrl->SetBackgroundColour(theme.pgMarginCol);
	labelsText->SetForegroundColour(theme.pgSelectedCellTextCol);
	this->SetBackgroundColour(theme.bgCol);
	for (auto szItem : buttons) {
		auto button = dynamic_cast<wxButton*>(szItem->GetWindow());
		button->SetBackgroundColour(theme.tilesCol);
	}
}

void PlayerPropertyDialog::OnAddPlayerClicked(wxCommandEvent& evt)
{
	if (GameSession::gameData.movesCounter > 0)
		if (static_cast<GameWindow*>(this->GetParent())->confirmResetDialog->ShowModal() == wxID_NO) { return; }
	
	const wchar_t label{ (evtButton ? evtButton->GetLabel()[0] : static_cast<wchar_t>('\0')) };
	const wxString& name{ playerNameTxtCtrl->GetValue() };
	const wxColour color{ clrCtrl->GetColour() };

	Player::shouldDecPlayerCount = false;
	GameSession::AddPlayer({ label, name, color });
	Player::shouldDecPlayerCount = true;
	this->Close();
	evt.Skip();
}