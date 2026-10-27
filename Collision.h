#pragma once

struct Rect
{
    float x;
    float y;
    float width;
    float height;
};

// •Ó‚ªG‚ê‚é‚¾‚¯‚Ìó‘Ô‚Íd‚È‚è‚ÉŠÜ‚ß‚È‚¢B
inline bool Overlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.width
        && a.x + a.width > b.x
        && a.y < b.y + b.height
        && a.y + a.height > b.y;
}