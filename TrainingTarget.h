#pragma once

#include "Collision.h"

class Player;

// 戦闘確認専用の静止標的。
// AI・報酬・食材は後続STEPで追加する。
class TrainingTarget
{
public:
    void Init();
    void Update(float seconds);

    bool TryHit(const Player& player);

    Rect Bounds() const;

    int Hp() const { return hp_; }

    bool IsAlive() const
    {
        return hp_ > 0;
    }

    bool ShowsDamage() const
    {
        return damageTextRemaining_ > 0.0f;
    }

    int LastDamage() const
    {
        return lastDamage_;
    }

private:
    float x_ = 0.0f;
    float knockbackVelocity_ = 0.0f;

    int hp_ = 0;

    unsigned int lastAttackId_ = 0;

    float damageTextRemaining_ = 0.0f;
    int lastDamage_ = 0;
};