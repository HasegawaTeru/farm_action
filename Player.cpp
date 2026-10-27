#include "Player.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>

void Player::Init()
{
    x_ = 200.0f;
    y_ = GameConfig::GroundY - GameConfig::PlayerHeight;

    velocityY_ = 0.0f;
    knockbackVelocity_ = 0.0f;

    grounded_ = true;

    hp_ = GameConfig::PlayerMaxHp;

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

    // 長時間停止後の極端な移動を防ぐ。
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

    if (!IsAlive())
    {
        return;
    }

    horizontal = std::max(-1, std::min(1, horizontal));

    // 攻撃中も移動できるが、向きは攻撃開始時のまま固定する。
    if (!IsAttacking() && horizontal != 0)
    {
        facing_ = horizontal;
    }

    // 攻撃間隔を過ぎていれば、新しい攻撃を開始する。
    if (attackPressed && attackCooldown_ == 0.0f)
    {
        attackRemaining_ = GameConfig::AttackDuration;
        attackCooldown_ = GameConfig::AttackInterval;

        attackDirection_ = facing_;
        ++attackId_;
    }

    // 通常移動にノックバックによる移動を加える。
    x_ += horizontal * GameConfig::MoveSpeed * seconds
        + AdvanceKnockback(knockbackVelocity_, seconds);

    x_ = std::max(
        0.0f,
        std::min(
            x_,
            GameConfig::WindowWidth - GameConfig::PlayerWidth
        )
    );

    // 接地中に押した瞬間だけジャンプする。
    if (jumpPressed && grounded_)
    {
        velocityY_ = -GameConfig::JumpSpeed;
        grounded_ = false;
    }

    if (!grounded_)
    {
        // 等加速度運動を使い、フレーム間隔による軌道の差を抑える。
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
    return GameConfig::PlayerMaxHp;
}

int Player::AttackPower() const
{
    return GameConfig::PlayerAttackPower;
}

bool Player::TakeDamage(int damage, int knockbackDirection)
{
    // 無敵中・HP 0・無効なダメージは受け付けない。
    if (!IsAlive() || IsInvincible() || damage <= 0)
    {
        return false;
    }

    hp_ = damage >= hp_ ? 0 : hp_ - damage;

    invincibleRemaining_ = GameConfig::InvincibleDuration;

    knockbackVelocity_ =
        (knockbackDirection < 0 ? -1.0f : 1.0f)
        * GameConfig::KnockbackSpeed;

    // HP 0になった場合は攻撃を終了する。
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

    // 無敵中は一定間隔で表示・非表示を切り替える。
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