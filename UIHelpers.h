#pragma once

#include "Collision.h"

class Input;

namespace UI
{
    bool Clicked(const Input& input, const Rect& rect);

    void Box(
        const Rect& rect,
        unsigned int color,
        bool fill = true
    );

    void Button(
        const Rect& rect,
        const char* text,
        bool selected = false
    );
}