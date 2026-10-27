#pragma once

#include "GameConfig.h"

#include <algorithm>
#include <cmath>

// プレイヤーと標的で共通の、自然に減速する水平ノックバック。
// 戻り値は今回の更新で移動する距離。
inline float AdvanceKnockback(float& velocity, float seconds)
{
    const float speed = std::fabs(velocity);

    const float movingSeconds = std::min(
        seconds,
        speed / GameConfig::KnockbackDeceleration
    );

    const float sign = velocity < 0.0f ? -1.0f : 1.0f;

    const float distance = sign * (
        speed * movingSeconds
        - 0.5f
        * GameConfig::KnockbackDeceleration
        * movingSeconds
        * movingSeconds
        );

    velocity = sign * std::max(
        0.0f,
        speed - GameConfig::KnockbackDeceleration * seconds
    );

    return distance;
}

// 小さな丸め誤差が残っても、時間切れと判定できるようにする。
inline float AdvanceTimer(float remaining, float seconds)
{
    const float next = remaining - seconds;
    return next > 0.000001f ? next : 0.0f;
}