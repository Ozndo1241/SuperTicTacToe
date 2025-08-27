#include <wx/wx.h>
#include <utility>
#include "Player.h"

Player::Player(const wchar_t label, const wxString& name, const wxColour& labelclr)
	: m_name{name}, m_label{label}, m_labelColour{labelclr}, m_id{++m_count}, dontDeleteMovedTo{false}
{
}

Player::Player(Player&& other)
{
	this->m_id = other.m_id;
	this->m_label = other.m_label;
	this->m_name = std::move(other.m_name);
	this->m_labelColour = std::move(other.m_labelColour);
	this->m_score = other.m_score;
	this->dontDeleteMovedTo = false;
	
	other.dontDeleteMovedTo = true;
	other.m_id = 0;
	other.m_label = '\0';
	other.m_name = wxString{};
	other.m_labelColour = wxColour{};
	other.m_score = 0;
}

Player &Player::operator=(Player&& other)
{
    if (&other == this)
		return *this;

	this->m_id = other.m_id;
	this->m_label = other.m_label;
	this->m_name = std::move(other.m_name);
	this->m_labelColour = std::move(other.m_labelColour);
	this->m_score = other.m_score;
	this->dontDeleteMovedTo = false;
	
	other.dontDeleteMovedTo = true;
	other.m_id = 0;
	other.m_label = '\0';
	other.m_name = wxString{};
	other.m_labelColour = wxColour{};
	other.m_score = 0;

	return *this;
}

Player::Player(const Player& copy)
{
	this->dontDeleteMovedTo = copy.dontDeleteMovedTo;
	this->m_id = copy.m_id;
	this->m_label = copy.m_label;
	this->m_score = copy.m_score;
	this->m_name = copy.m_name;
	this->m_labelColour = copy.m_labelColour;
}

Player &Player::operator=(const Player& copy)
{
    if (&copy == this)
		return *this;

	this->m_id = copy.m_id;
	this->m_label = copy.m_label;
	this->m_name = copy.m_name;
	this->m_labelColour = copy.m_labelColour;
	this->m_score = copy.m_score;
	this->dontDeleteMovedTo = copy.dontDeleteMovedTo;

	return *this;
}

size_t Player::m_count = 0;
bool Player::shouldDecPlayerCount = false;
