#include "UIHelpers.h"
#include "Input.h"
#include "DxLib.h"

bool UI::Clicked(
    const Input& input,
    const Rect& rect)
{
    return input.IsLeftClickPressed()
        && input.MouseX() >= rect.x
        && input.MouseX() < rect.x + rect.width
        && input.MouseY() >= rect.y
        && input.MouseY() < rect.y + rect.height;
}

void UI::Box(
    const Rect& rect,
    unsigned int color,
    bool fill)
{
    DrawBox(
        static_cast<int>(rect.x),
        static_cast<int>(rect.y),
        static_cast<int>(rect.x + rect.width),
        static_cast<int>(rect.y + rect.height),
        color,
        fill ? TRUE : FALSE
    );
}

void UI::Button(
    const Rect& rect,
    const char* text,
    bool selected)
{
    Box(rect, GetColor(55, 65, 85));

    if (selected)
    {
        Box(
            rect,
            GetColor(255, 220, 60),
            false
        );
    }

    DrawString(
        static_cast<int>(rect.x) + 15,
        static_cast<int>(rect.y) + 18,
        text,
        GetColor(255, 255, 255)
    );
}