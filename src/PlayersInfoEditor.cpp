#include "PlayersInfoEditor.h"
#include "PlayerPropertyDialog.h"
#include "GameWindow.h"
#include "GameGrid.h"

wxIMPLEMENT_DYNAMIC_CLASS(PlayersInfoProperty, wxPGTextCtrlEditor);

wxPGWindowList PlayersInfoProperty::CreateControls(wxPropertyGrid* propgrid,
	wxPGProperty* property,
	const wxPoint& pos,
	const wxSize& sz) const
{
	wxPGMultiButton* buttons = new wxPGMultiButton(propgrid, sz);
	buttons->Add("Add player");

	wxPGWindowList wndlist = wxPGTextCtrlEditor::CreateControls(propgrid, property, pos, buttons->GetPrimarySize());
	buttons->Finalize(propgrid, pos);
	wndlist.SetSecondary(buttons);
	static_cast<wxTextCtrl*>(wndlist.m_primary)->SetEditable(false);
	return wndlist;
}

bool PlayersInfoProperty::OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const
{
	if (event.GetEventType() == wxEVT_BUTTON) {
		GameWindow* parent = dynamic_cast<GameWindow*>(propgrid->GetGrandParent());
		if (parent) {
			if (GameSession::gameData.numOfPlayers != GameTile::labels.size()
				&& parentWindow->ValidatePlayerCount(GameSession::players.size() + 1,
					GameSession::gameGrid->GetNumOfColumns()))
				parentWindow->addPlayerDialog->ShowModal();
			else if (GameSession::gameData.numOfPlayers == GameTile::labels.size()) {
				parent->playerCountError->SetMessage("Max playercount reached.");
				parent->playerCountError->ShowModal();
			}
		}
	}
	
	return wxPGTextCtrlEditor::OnEvent(propgrid, property, ctrl, event);
}
