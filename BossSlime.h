#pragma once

#include "Enemy.h"

class BossSlime : public Enemy
{
public:
    enum class State
    {
        Windup,
        Jumping,
        Recovery,
        Dead
    };

    void Init(int stageLevel, int playerLevel);

    void Update(
        float seconds,
        const Player& player
    ) override;

    bool Intersects(const Rect& rect) const override;

    bool Enraged() const
    {
        return IsAlive() && Hp() * 2 <= MaxHp();
    }

    State CurrentState() const
    {
        return IsAlive() ? state_ : State::Dead;
    }

    const char* StateName() const;

private:
    State state_ = State::Windup;

    float remaining_ = 0.0f;
    float velocityY_ = 0.0f;
    float leapSpeed_ = 0.0f;

    int direction_ = -1;
};