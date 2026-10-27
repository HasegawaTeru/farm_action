#pragma once

#include "Collision.h"

// 入力・描画APIから独立した、プレイヤーの移動と戦闘状態。
class Player
{
public:
    void Init();

    void Update(
        float seconds,
        int horizontal,
        bool jumpPressed,
        bool attackPressed = false
    );

    bool TakeDamage(int damage, int knockbackDirection);

    float X() const { return x_; }
    float Y() const { return y_; }

    bool IsGrounded() const { return grounded_; }

    int Hp() const { return hp_; }
    int MaxHp() const;
    int AttackPower() const;

    int Facing() const { return facing_; }
    int AttackDirection() const { return attackDirection_; }

    unsigned int AttackId() const { return attackId_; }

    bool IsAttacking() const
    {
        return attackRemaining_ > 0.0f;
    }

    bool IsInvincible() const
    {
        return invincibleRemaining_ > 0.0f;
    }

    bool IsAlive() const
    {
        return hp_ > 0;
    }

    bool IsVisible() const;

    Rect Bounds() const;
    Rect AttackBounds() const;

private:
    float x_ = 0.0f;
    float y_ = 0.0f;

    float velocityY_ = 0.0f;
    float knockbackVelocity_ = 0.0f;

    bool grounded_ = false;

    int hp_ = 0;

    // 右向きは1、左向きは-1。
    int facing_ = 1;
    int attackDirection_ = 1;

    unsigned int attackId_ = 0;

    float attackRemaining_ = 0.0f;
    float attackCooldown_ = 0.0f;
    float invincibleRemaining_ = 0.0f;
};