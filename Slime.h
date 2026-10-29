#pragma once

#include "Enemy.h"

// スライム固有の行動を管理する。
// HP・ダメージなどの共通処理はEnemyを使用する。
class Slime : public Enemy
{
public:
    enum class State
    {
        Idle,
        Windup,
        Jumping,
        Recovery,
        Dead
    };

    void Init(
        float x,
        int stageLevel = 1,
        int playerLevel = 1
    );

    void Update(
        float seconds,
        const Player& player
    ) override;

    bool Intersects(const Rect& rect) const override;

    State CurrentState() const
    {
        return IsAlive() ? state_ : State::Dead;
    }

    int Facing() const
    {
        return facing_;
    }

    const char* StateName() const;

private:
    bool Detects(const Player& player) const;
    void BeginWindup();

    State state_ = State::Idle;

    float stateRemaining_ = 0.0f;
    float spawnX_ = 0.0f;

    float velocityY_ = 0.0f;

    int facing_ = -1;
    bool walking_ = false;
};