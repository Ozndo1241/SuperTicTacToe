#include "ThemeSelectionProperty.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <wx/textdlg.h>

wxIMPLEMENT_DYNAMIC_CLASS(ThemeSelectionProperty, wxPGChoiceEditor);

wxPGWindowList ThemeSelectionProperty::CreateControls(wxPropertyGrid* propgrid,
	wxPGProperty* property,
	const wxPoint& pos,
	const wxSize& sz) const
{
	wxPGMultiButton* buttons = new wxPGMultiButton(propgrid, sz);
	buttons->Add("Save");
	buttons->Add("Remove");

	auto& abc = buttons->GetChildren();
	auto saveButton = dynamic_cast<wxButton*>(*abc.begin());
	auto removeButton = dynamic_cast<wxButton*>(*(++abc.begin()));

	if (saveButton && removeButton) {
		saveButton->Bind(wxEVT_BUTTON, &SaveThemeToFile);
		saveButton->SetClientData(parentWnd);
		removeButton->Bind(wxEVT_BUTTON, &RemoveThemeFromDir);
		removeButton->SetClientData(parentWnd);
	}

	wxPGWindowList wndlist = wxPGChoiceEditor::CreateControls(propgrid, property, pos, buttons->GetPrimarySize());
	buttons->Finalize(propgrid, pos);
	wndlist.SetSecondary(buttons);
	return wndlist;
}

bool ThemeSelectionProperty::OnEvent(wxPropertyGrid* propgrid, wxPGProperty* property, wxWindow* ctrl, wxEvent& event) const
{
	return wxPGChoiceEditor::OnEvent(propgrid, property, ctrl, event);
}

// sort themes alphabetically
static inline void SortThemesArr(std::vector<GameWindow::ThemeColorData>& vec)
{
	std::sort(vec.begin(), vec.end(),
		[](const GameWindow::ThemeColorData& a, const GameWindow::ThemeColorData& b) {
			return a.themeName < b.themeName;
		});
}

void SaveThemeToFile(wxCommandEvent& evt)
{
	auto gameWnd = reinterpret_cast<GameWindow*>(dynamic_cast<wxButton*>(evt.GetEventObject())->GetClientData());
	GameWindow::ThemeColorData themeCopy{ *gameWnd->GetCurrentTheme() };

	wxTextEntryDialog dlg(nullptr, "Enter theme name", "Save theme");

	if (dlg.ShowModal() == wxID_CANCEL)
		return;

	std::string newThemeName = dlg.GetValue().ToStdString();

	for (const auto& themeName : *gameWnd->GetPGThemesChoices()) {
		if (!themeName.compare(newThemeName)) {
			wxMessageBox("A theme with this name already exists");
			return;
		}
	}

	// Saved themes originate from built-in themes,
	// so they are made non built-in for deletion
	// later if users wish.
	themeCopy.isBuiltIn = false;

	std::string fileName{ newThemeName + ".txt" };

	if (!std::filesystem::exists(std::filesystem::current_path() / "Themes")) {
		std::filesystem::create_directory(std::filesystem::current_path() / "Themes");
	}

	std::string filepath{ std::filesystem::current_path() / "Themes" / fileName };
	std::ofstream file{filepath};

	if (!file) {
		wxMessageBox("Error loading file");
		return;
	}

	file << "<Theme>\n";
	file << "Name: " << '\"' << newThemeName << "\"\n";
	file << "Colour data {\n";
	file << themeCopy.bgCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.tilesCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgMarginCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgLineSepCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgCellsBgCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgCellsTextCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgSelectedCellCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgSelectedCellTextCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << themeCopy.pgCatPropTextCol.GetAsString(wxC2S_HTML_SYNTAX) << '\n';
	file << "}\n";

	auto& themesArr{ gameWnd->GetThemesList() };
	auto themeChoices{ gameWnd->GetPGThemesChoices() };

	// required for proper sorting of themes
	themeCopy.themeName = newThemeName;

	themesArr.push_back(themeCopy);
	SortThemesArr(themesArr);
	themeChoices->Add(newThemeName);
	themeChoices->Sort();

	auto prop{ gameWnd->GetGameMenu()->GetProperty("THEME") };
	// refresh theme choices
	if (prop) {
		prop->SetChoices(wxPGChoices(*themeChoices));
	}
	else {
		assert(0 && "THEME was null.");
	}
}

void RemoveThemeFromDir(wxCommandEvent& evt)
{
	auto gameWnd = reinterpret_cast<GameWindow*>(dynamic_cast<wxButton*>(evt.GetEventObject())->GetClientData());
	auto theme = gameWnd->GetCurrentTheme();

	if (theme->isBuiltIn) { return; }

	auto& themesArr{ gameWnd->GetThemesList() };
	auto themeChoices{ gameWnd->GetPGThemesChoices() };

	std::string filepath{ "/tictactoe_ui/Themes/" };
	filepath += (theme->themeName + ".txt");
	std::ifstream file{ filepath };

	try {
		if (0 && !theme->themeName.empty())
			std::filesystem::remove(filepath);
	}
	catch (const std::exception& ex) {
		wxLogMessage(ex.what());
	}

	themeChoices->Remove(theme->themeName);
	themeChoices->Sort();

	// get the index of the theme that needs to be deleted
	size_t offset{ 0 };
	for ( ; offset < themesArr.size(); ++offset) {
		if (!themesArr[offset].themeName.compare(theme->themeName)) {
			break;
		}
	}
	
	// swap that theme with the theme at the back of the array,
	// then pop_back in order remove it.
	auto it = themesArr.begin() + offset;
	std::swap(*it, themesArr.back());
	themesArr.pop_back();
	SortThemesArr(themesArr);

	auto prop{ gameWnd->GetGameMenu()->GetProperty("THEME") };
	// refresh the theme choices
	if (prop) {
		prop->SetChoices(wxPGChoices(*themeChoices));
		prop->SetChoiceSelection(themesArr.size() - 1);
		gameWnd->SetTheme(themesArr.back());
	}
}
