#include <string>
#include "PlayerPGEditor.h"
#include "Player.h"

wxPGWindowList PlayerPGEditor::CreateControls(wxPropertyGrid* propgrid, wxPGProperty* property, const wxPoint& pos, const wxSize& sz) const
{
	wxPGMultiButton* buttons = new wxPGMultiButton{ propgrid, sz };
	
	buttons->Add("Change colour", ID_PLAYER_CHANGE_COLOUR);
	buttons->Add("Remove", ID_PLAYER_REMOVE);
	buttons->Add("Rename", ID_PLAYER_RENAME);

	auto& buttonslist{ buttons->GetChildren() };
	for (size_t i{ 0 }; i < buttons->GetCount(); ++i) {
		static_cast<wxButton*>(buttonslist[i])->Bind(wxEVT_BUTTON,
			[this](wxCommandEvent& evt) {
				auto pgEvent{ new wxPropertyGridEvent(wxEVT_PG_CHANGED, evt.GetId()) };
				//clrdata.SetColour(*wxBLACK);
				if (evt.GetId() == ID_PLAYER_CHANGE_COLOUR) {
					static wxColourData clrdata{};
					static wxColour val{};
					wxColourDialog dlg{ nullptr, &clrdata };
					if (dlg.ShowModal() == wxID_OK) {
						val = dlg.GetColourData().GetColour();
						pgEvent->SetClientData(&val);
					}
					else
						return;
				}
				pgEvent->SetProperty(parentProp);
				wxPostEvent(parentwnd->GetGameMenu(), *pgEvent);
			});
	}

	wxPGWindowList wndlist{ wxPGTextCtrlEditor::CreateControls(propgrid, property, pos, buttons->GetPrimarySize()) };
	buttons->Finalize(propgrid, pos);
	wndlist.SetSecondary(buttons);
	static_cast<wxTextCtrl*>(wndlist.m_primary)->SetEditable(false);
	return wndlist;
}
