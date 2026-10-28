#include "BossSlime.h"
#include "StageRules.h"
#include "GameConfig.h"
#include "CombatMotion.h"
#include "Player.h"

#include <algorithm>

void BossSlime::Init(
    int stageLevel,
    int playerLevel)
{
    InitEnemy(
        StageRules::BossX,
        StageRules::BossRadius * 2,
        StageRules::BossRadius * 2,
        StageRules::BossBaseHp,
        StageRules::BossBaseAttack,
        stageLevel,
        playerLevel
    );

    state_ = State::Windup;
    remaining_ = StageRules::Windup;

    velocityY_ = 0;
    leapSpeed_ = 0;
    direction_ = -1;
}

void BossSlime::Update(
    float seconds,
    const Player& player)
{
    if (seconds <= 0)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    UpdateCommon(seconds);

    if (!IsAlive())
    {
        return;
    }

    // ノックバックでもボスエリア外へ出さない。
    x_ = std::max(
        StageRules::ArenaLeft,
        std::min(
            x_,
            StageRules::ArenaRight - width_
        )
    );

    if (!player.IsAlive())
    {
        return;
    }

    remaining_ = AdvanceTimer(
        remaining_,
        seconds
    );

    switch (state_)
    {
    case State::Windup:
    {
        if (remaining_ == 0)
        {
            const Rect p = player.Bounds();

            direction_ =
                p.x + p.width * 0.5f
                < x_ + width_ * 0.5f
                ? -1
                : 1;

            velocityY_ = -(
                Enraged()
                ? StageRules::AngryJumpSpeed
                : StageRules::JumpSpeed
                );

            leapSpeed_ =
                Enraged()
                ? StageRules::AngryLeapSpeed
                : StageRules::LeapSpeed;

            state_ = State::Jumping;
        }

        break;
    }

    case State::Jumping:
    {
        MoveX(
            direction_ * leapSpeed_ * seconds
        );

        x_ = std::max(
            StageRules::ArenaLeft,
            std::min(
                x_,
                StageRules::ArenaRight - width_
            )
        );

        y_ += velocityY_ * seconds
            + 0.5f * GameConfig::Gravity * seconds * seconds;

        velocityY_ +=
            GameConfig::Gravity * seconds;

        if (velocityY_ >= 0
            && y_ + height_ >= GameConfig::GroundY)
        {
            y_ = GameConfig::GroundY - height_;
            velocityY_ = 0;

            state_ = State::Recovery;

            remaining_ =
                Enraged()
                ? StageRules::AngryRecovery
                : StageRules::Recovery;
        }

        break;
    }

    case State::Recovery:
    {
        if (remaining_ == 0)
        {
            state_ = State::Windup;

            remaining_ =
                Enraged()
                ? StageRules::AngryWindup
                : StageRules::Windup;
        }

        break;
    }

    default:
        break;
    }
}

bool BossSlime::Intersects(const Rect& rect) const
{
    return OverlapsCircle(
        rect,
        x_ + width_ * 0.5f,
        y_ + height_ * 0.5f,
        StageRules::BossRadius
    );
}