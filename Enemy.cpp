#include "Enemy.h"
#include "Player.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>
#include <cmath>

void Enemy::InitEnemy(
    float x,
    float width,
    float height,
    int baseHp,
    int baseAttack,
    int stageLevel,
    int playerLevel)
{
    width_ = width;
    height_ = height;

    x_ = x;

    // 初期位置も画面内へ収める。
    MoveX(0.0f);

    y_ = GameConfig::GroundY - height_;

    // 敵Lv =
    // min(30, max(ステージ基準Lv, ceil(主人公Lv * 0.5)))
    level_ = std::min(
        GameConfig::EnemyMaxLevel,
        std::max(
            1,
            std::max(
                stageLevel,
                static_cast<int>(
                    std::ceil(playerLevel * 0.5)
                    )
            )
        )
    );

    maxHp_ = static_cast<int>(
        std::ceil(
            baseHp * (1.0 + 0.10 * (level_ - 1))
        )
        );

    attackPower_ = static_cast<int>(
        std::ceil(
            baseAttack * (1.0 + 0.05 * (level_ - 1))
        )
        );

    hp_ = maxHp_;

    knockbackVelocity_ = 0.0f;
    lastAttackId_ = 0;

    lastDamage_ = 0;
    damageTextRemaining_ = 0.0f;
}

void Enemy::MoveX(float distance)
{
    x_ = std::max(
        0.0f,
        std::min(
            x_ + distance,
            GameConfig::WindowWidth - width_
        )
    );
}

void Enemy::UpdateCommon(float seconds)
{
    damageTextRemaining_ = AdvanceTimer(
        damageTextRemaining_,
        seconds
    );

    if (IsAlive())
    {
        MoveX(
            AdvanceKnockback(
                knockbackVelocity_,
                seconds
            )
        );
    }
}

bool Enemy::TryHit(const Player& player)
{
    if (!IsAlive()
        || !player.IsAlive()
        || !player.IsAttacking()
        || lastAttackId_ == player.AttackId()
        || !Intersects(player.AttackBounds()))
    {
        return false;
    }

    // 敵ごとに攻撃IDを記録する。
    // 同じ攻撃で同じ敵へ何度もダメージを与えない。
    lastAttackId_ = player.AttackId();

    lastDamage_ = std::min(
        hp_,
        player.AttackPower()
    );

    hp_ -= lastDamage_;

    knockbackVelocity_ =
        player.AttackDirection()
        * GameConfig::KnockbackSpeed;

    damageTextRemaining_ =
        GameConfig::DamageTextDuration;

    return true;
}

Rect Enemy::Bounds() const
{
    return {
        x_,
        y_,
        width_,
        height_
    };
}

bool Enemy::Intersects(const Rect& rect) const
{
    return Overlaps(Bounds(), rect);
}