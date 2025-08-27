#include "RainbowEffect.h"
#include "GameSession.h"

RainbowEffect::RainbowEffect(std::vector<GameTile*>& winTiles)
 : m_winTiles{winTiles}, mt{rd()}, dist{0, 2}, dist2{1, 2}
{
    m_timer.Bind(wxEVT_TIMER, &RainbowEffect::RGBTimerEvent, this);
}

void RainbowEffect::StartRainbowEffect()
{
    m_timer.Start(5);
}

RainbowEffect::~RainbowEffect()
{
    if (m_timer.IsRunning())
        m_timer.Stop();
}

void RainbowEffect::StopRainbowEffect()
{
    m_timer.Stop();
}

void RainbowEffect::RGBTimerEvent(wxTimerEvent &tevt)
{
    switch (clr) {
        case RED:
            if (!r_r)
                ++r;
            else
                --r;
            for (auto* tile : m_winTiles) {
                tile->SetForegroundColour(wxColour(r, g ,b));
            }
            if (!r_r && r == 255) {
                r_r = true;
                break;
            } else if (r_r && r == 0) {
                r_r = false;
                break;
            }
        
        case GREEN:
            if (!r_g)
                ++g;
            else
                --g;
            for (auto* tile : m_winTiles) {
                tile->SetForegroundColour(wxColour(r, g ,b));
            }
            if (!r_g && g == 255) {
                r_g = true;
                break;
            } else if (r_g && g == 0) {
                r_g = false;
                break;
            }
        
        case BLUE:
            if (!r_b)
                ++b;
            else
                --b;
            for (auto* tile : m_winTiles) {
                tile->SetForegroundColour(wxColour(r, g ,b));
            }
            if (!r_b && b == 255) {
                r_b = true;
                break;
            } else if (r_b && b == 0) {
                r_b = false;
                break;
            }
    }
    int nextClr{dist(mt)};							
    // If randomly generated int 'nextClr' is equal to previous color...
    if (nextClr == clr) {
        // if it was red previously, increment and make it green.
        if (nextClr == RED) {
            ++nextClr;
        // if it was green previously, randomly choose red or blue.
        } else if (nextClr == 1) {
            int c{dist2(mt)};
            if (c == 1) {--nextClr;}
            else if (c == 2) {++nextClr;}
        // If it was blue previously, decrement and make it green.
        } else if (nextClr == 2) {
            --nextClr;
        }
    }

    clr = nextClr;
}
