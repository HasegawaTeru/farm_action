#include "Input.h"
#include "DxLib.h"

void Input::Update()
{
    previous_ = current_;
    previousLeftClick_ = leftClick_;

    GetHitKeyStateAll(current_.data());

    leftClick_ = (GetMouseInput() & MOUSE_INPUT_LEFT) != 0;
    GetMousePoint(&mouseX_, &mouseY_);
}

bool Input::IsDown(int key) const
{
    return key >= 0
        && key < static_cast<int>(current_.size())
        && current_[key] != 0;
}

bool Input::IsPressed(int key) const
{
    // 前フレームでは押しておらず、今フレームで押した場合。
    return IsDown(key) && previous_[key] == 0;
}

bool Input::IsLeftClickPressed() const
{
    return leftClick_ && !previousLeftClick_;
}