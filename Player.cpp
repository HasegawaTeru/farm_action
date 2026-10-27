#include "Player.h"
#include "GameConfig.h"
#include <algorithm>

void Player::Init()
{
    x_ = 200.0f;
    y_ = GameConfig::GroundY - GameConfig::PlayerHeight;

    velocityY_ = 0.0f;
    grounded_ = true;
}

void Player::Update(float seconds, int horizontal, bool jumpPressed)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    // 長時間停止後の極端な移動を防ぐ。
    seconds = std::min(seconds, GameConfig::MaxFrameSeconds);
    horizontal = std::max(-1, std::min(1, horizontal));

    // 左右移動。空中でも移動できる。
    x_ += horizontal * GameConfig::MoveSpeed * seconds;

    // 画面の左右からはみ出さないようにする。
    x_ = std::max(
        0.0f,
        std::min(
            x_,
            GameConfig::WindowWidth - GameConfig::PlayerWidth
        )
    );

    // 接地中にSpaceを押した瞬間だけジャンプする。
    if (jumpPressed && grounded_)
    {
        velocityY_ = -GameConfig::JumpSpeed;
        grounded_ = false;
    }

    if (!grounded_)
    {
        // 等加速度運動によってフレーム間隔の影響を抑える。
        y_ += velocityY_ * seconds
            + 0.5f * GameConfig::Gravity * seconds * seconds;

        velocityY_ += GameConfig::Gravity * seconds;

        // 落下中に足元が地面へ到達したら着地する。
        if (velocityY_ >= 0.0f
            && y_ + GameConfig::PlayerHeight >= GameConfig::GroundY)
        {
            y_ = GameConfig::GroundY - GameConfig::PlayerHeight;
            velocityY_ = 0.0f;
            grounded_ = true;
        }
    }
}