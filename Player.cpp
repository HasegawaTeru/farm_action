#include "Player.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>

void Player::Init()
{
    Init(
        GameConfig::PlayerMaxHp,
        GameConfig::PlayerAttackPower
    );
}

void Player::Init(int maxHp, int attackPower)
{
    x_ = 200.0f;
    y_ = GameConfig::GroundY - GameConfig::PlayerHeight;

    velocityY_ = 0.0f;
    knockbackVelocity_ = 0.0f;

    grounded_ = true;

    maxHp_ = std::max(1, maxHp);
    attackPower_ = std::max(1, attackPower);
    hp_ = maxHp_;

    buffRemaining_ = 0.0f;
    jumpMultiplier_ = 1.0f;

    facing_ = 1;
    attackDirection_ = 1;
    attackId_ = 0;

    attackRemaining_ = 0.0f;
    attackCooldown_ = 0.0f;
    invincibleRemaining_ = 0.0f;
}

void Player::Update(
    float seconds,
    int horizontal,
    bool jumpPressed,
    bool attackPressed)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    attackRemaining_ = AdvanceTimer(
        attackRemaining_,
        seconds
    );

    attackCooldown_ = AdvanceTimer(
        attackCooldown_,
        seconds
    );

    invincibleRemaining_ = AdvanceTimer(
        invincibleRemaining_,
        seconds
    );

    buffRemaining_ = AdvanceTimer(
        buffRemaining_,
        seconds
    );

    if (buffRemaining_ == 0.0f)
    {
        jumpMultiplier_ = 1.0f;
    }

    if (!IsAlive())
    {
        return;
    }

    horizontal = std::max(-1, std::min(1, horizontal));

    // 攻撃中は向きを固定する。移動は可能。
    if (!IsAttacking() && horizontal != 0)
    {
        facing_ = horizontal;
    }

    if (attackPressed && attackCooldown_ == 0.0f)
    {
        attackRemaining_ = GameConfig::AttackDuration;
        attackCooldown_ = GameConfig::AttackInterval;

        attackDirection_ = facing_;
        ++attackId_;
    }

    x_ += horizontal * GameConfig::MoveSpeed * seconds
        + AdvanceKnockback(knockbackVelocity_, seconds);

    x_ = std::max(
        0.0f,
        std::min(
            x_,
            GameConfig::WindowWidth - GameConfig::PlayerWidth
        )
    );

    if (jumpPressed && grounded_)
    {
        velocityY_ =
            -GameConfig::JumpSpeed * jumpMultiplier_;

        grounded_ = false;
    }

    if (!grounded_)
    {
        y_ += velocityY_ * seconds
            + 0.5f * GameConfig::Gravity * seconds * seconds;

        velocityY_ += GameConfig::Gravity * seconds;

        if (velocityY_ >= 0.0f
            && y_ + GameConfig::PlayerHeight >= GameConfig::GroundY)
        {
            y_ = GameConfig::GroundY - GameConfig::PlayerHeight;
            velocityY_ = 0.0f;
            grounded_ = true;
        }
    }
}

int Player::MaxHp() const
{
    return maxHp_;
}

int Player::AttackPower() const
{
    return attackPower_;
}

bool Player::TakeDamage(int damage, int knockbackDirection)
{
    if (!IsAlive() || IsInvincible() || damage <= 0)
    {
        return false;
    }

    hp_ = damage >= hp_ ? 0 : hp_ - damage;

    invincibleRemaining_ =
        GameConfig::InvincibleDuration;

    knockbackVelocity_ =
        (knockbackDirection < 0 ? -1.0f : 1.0f)
        * GameConfig::KnockbackSpeed;

    if (!IsAlive())
    {
        attackRemaining_ = 0.0f;
    }

    return true;
}

bool Player::IsVisible() const
{
    if (!IsInvincible())
    {
        return true;
    }

    const float elapsed =
        GameConfig::InvincibleDuration - invincibleRemaining_;

    return static_cast<int>(
        elapsed / GameConfig::BlinkInterval
        ) % 2 == 0;
}

Rect Player::Bounds() const
{
    return {
        x_,
        y_,
        GameConfig::PlayerWidth,
        GameConfig::PlayerHeight
    };
}

Rect Player::AttackBounds() const
{
    return {
        attackDirection_ > 0
            ? x_ + GameConfig::PlayerWidth
            : x_ - GameConfig::AttackReach,

        y_ + (
            GameConfig::PlayerHeight - GameConfig::AttackHeight
        ) * 0.5f,

        GameConfig::AttackReach,
        GameConfig::AttackHeight
    };
}

int Player::Heal(int amount)
{
    if (!IsAlive() || amount <= 0)
    {
        return 0;
    }

    const int restored =
        std::min(amount, maxHp_ - hp_);

    hp_ += restored;

    return restored;
}

bool Player::ApplyJumpBuff(float multiplier, float seconds)
{
    if (!IsAlive()
        || !std::isfinite(multiplier)
        || !std::isfinite(seconds)
        || multiplier < 1.0f
        || seconds <= 0.0f)
    {
        return false;
    }

    // 新しい強化は倍率と残り時間を上書きする。
    jumpMultiplier_ = multiplier;
    buffRemaining_ = seconds;

    return true;
}