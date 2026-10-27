#pragma once
#include <array>

class Input
{
public:
    void Update();

    bool IsDown(int key) const;
    bool IsPressed(int key) const;
    bool IsLeftClickPressed() const;

    int MouseX() const { return mouseX_; }
    int MouseY() const { return mouseY_; }

private:
    std::array<char, 256> current_{};
    std::array<char, 256> previous_{};

    bool leftClick_ = false;
    bool previousLeftClick_ = false;

    int mouseX_ = 0;
    int mouseY_ = 0;
};