#pragma once

// ˆÚ“®ˆ—‚ğ“ü—ÍAPIE•`‰æAPI‚©‚ç“Æ—§‚³‚¹‚éB
class Player
{
public:
    void Init();
    void Update(float seconds, int horizontal, bool jumpPressed);

    float X() const { return x_; }
    float Y() const { return y_; }
    bool IsGrounded() const { return grounded_; }

private:
    float x_ = 0.0f;
    float y_ = 0.0f;

    float velocityY_ = 0.0f;
    bool grounded_ = false;
};