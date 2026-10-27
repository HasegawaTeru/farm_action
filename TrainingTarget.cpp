#include "TrainingTarget.h"
#include "Player.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>

void TrainingTarget::Init()
{
    x_ = GameConfig::TargetStartX;
    hp_ = GameConfig::TargetMaxHp;

    knockbackVelocity_ = 0.0f;

    lastAttackId_ = 0;

    damageTextRemaining_ = 0.0f;
    lastDamage_ = 0;
}

void TrainingTarget::Update(float seconds)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    damageTextRemaining_ = AdvanceTimer(
        damageTextRemaining_,
        seconds
    );

    x_ += AdvanceKnockback(knockbackVelocity_, seconds);

    x_ = std::max(
        0.0f,
        std::min(
            x_,
            GameConfig::WindowWidth - GameConfig::TargetWidth
        )
    );
}

bool TrainingTarget::TryHit(const Player& player)
{
    if (!IsAlive()
        || !player.IsAlive()
        || !player.IsAttacking()
        || lastAttackId_ == player.AttackId()
        || !Overlaps(Bounds(), player.AttackBounds()))
    {
        return false;
    }

    // 同じ攻撃が複数フレーム重なっても、ダメージは一度だけ。
    lastAttackId_ = player.AttackId();

    lastDamage_ = std::min(hp_, player.AttackPower());
    hp_ -= lastDamage_;

    knockbackVelocity_ =
        player.AttackDirection() * GameConfig::KnockbackSpeed;

    damageTextRemaining_ = GameConfig::DamageTextDuration;

    return true;
}

Rect TrainingTarget::Bounds() const
{
    return {
        x_,
        GameConfig::GroundY - GameConfig::TargetHeight,
        GameConfig::TargetWidth,
        GameConfig::TargetHeight
    };
}