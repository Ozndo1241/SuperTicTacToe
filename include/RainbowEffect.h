#pragma once
#include <wx/wx.h>
#include <wx/timer.h>
#include <random>
#include <vector>
#include "GameTile.h"

class RainbowEffect {
public:
    RainbowEffect();
    RainbowEffect(std::vector<GameTile*>& winTiles);
    ~RainbowEffect();
    void StartRainbowEffect();
    void StopRainbowEffect();
    void RGBTimerEvent(wxTimerEvent& tevt);

private:
    std::random_device rd;
    std::mt19937 mt;
    // Randomly choose next colour
    std::uniform_int_distribution<int> dist;
    // Randomly choose to either decrement or increment if nextClr was 1
    std::uniform_int_distribution<int> dist2;
    std::vector<GameTile*>& m_winTiles;
    wxTimer m_timer;
    enum {
        RED,
        GREEN,
        BLUE
    };
    // clr = which color. 0 = red, 1 = green, 2 = blue
    int clr{};
    bool r_r{}, r_g{}, r_b{};
    int r{}, g{}, b{};

};