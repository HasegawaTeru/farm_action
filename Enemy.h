#pragma once

#include "Collision.h"

class Player;

// 敵共通のHP・レベル・被ダメージ・ノックバックを管理する。
// 描画処理はシーン側へ分離する。
class Enemy
{
public:
    virtual ~Enemy() = default;

    virtual void Update(
        float seconds,
        const Player& player
    ) = 0;

    virtual bool Intersects(const Rect& rect) const;

    bool TryHit(const Player& player);

    Rect Bounds() const;

    bool IsAlive() const
    {
        return hp_ > 0;
    }

    int Hp() const
    {
        return hp_;
    }

    int MaxHp() const
    {
        return maxHp_;
    }

    int Level() const
    {
        return level_;
    }

    int AttackPower() const
    {
        return attackPower_;
    }

    bool ShowsDamage() const
    {
        return damageTextRemaining_ > 0.0f;
    }

    int LastDamage() const
    {
        return lastDamage_;
    }

protected:
    void InitEnemy(
        float x,
        float width,
        float height,
        int baseHp,
        int baseAttack,
        int stageLevel,
        int playerLevel
    );

    void UpdateCommon(float seconds);
    void MoveX(float distance);

    float x_ = 0.0f;
    float y_ = 0.0f;

    float width_ = 0.0f;
    float height_ = 0.0f;

private:
    int hp_ = 0;
    int maxHp_ = 0;

    int level_ = 1;
    int attackPower_ = 0;

    float knockbackVelocity_ = 0.0f;

    unsigned int lastAttackId_ = 0;

    int lastDamage_ = 0;
    float damageTextRemaining_ = 0.0f;
};