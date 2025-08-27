#pragma once
#include <string>
#include <utility>
#include <wx/wx.h>

class Player
{
public:
	Player() = default;
	~Player() { if (!this->dontDeleteMovedTo && this->shouldDecPlayerCount) {--m_count;} }
	Player(const wchar_t label, const wxString& name, const wxColour& labelclr);

	Player(Player&& other);
	Player& operator=(Player&& other);
	Player(const Player& copy);
	Player& operator=(const Player& copy);

	static bool shouldDecPlayerCount;
	bool dontDeleteMovedTo;

	const int GetScore() const { return m_score; }
	const size_t GetPlayerId() const { return m_id; }
	const wxString& GetName() const { return m_name; }
	const wxColour& GetLabelColour() const { return m_labelColour; }
	const wchar_t& GetLabel() const { return m_label; }
	static const size_t GetPlayersCount() { return m_count; }

	inline void IncrementScore() { ++m_score; }
	void SetLabelColour(const wxColour& clr) { m_labelColour = clr; }
	void SetName(const wxString& name) { m_name = name; }
private:
	static size_t m_count; // total player count
	int m_score{};
	size_t m_id;
	wchar_t m_label;
	wxString m_name;
	wxColour m_labelColour;
};