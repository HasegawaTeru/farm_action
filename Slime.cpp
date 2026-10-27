#include "Slime.h"
#include "Player.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>

void Slime::Init(
    float x,
    int stageLevel,
    int playerLevel)
{
    InitEnemy(
        x,
        GameConfig::SlimeRadius * 2.0f,
        GameConfig::SlimeRadius * 2.0f,
        GameConfig::SlimeBaseHp,
        GameConfig::SlimeBaseAttack,
        stageLevel,
        playerLevel
    );

    spawnX_ = x_;

    velocityY_ = 0.0f;
    facing_ = -1;

    walking_ = false;

    state_ = State::Idle;
    stateRemaining_ = GameConfig::SlimeIdleDuration;
}

bool Slime::Detects(const Player& player) const
{
    if (!player.IsAlive())
    {
        return false;
    }

    const Rect bounds = player.Bounds();

    const float dx =
        bounds.x + bounds.width * 0.5f
        - (x_ + width_ * 0.5f);

    const float dy =
        bounds.y + bounds.height * 0.5f
        - (y_ + height_ * 0.5f);

    return dx * dx + dy * dy
        <= GameConfig::SlimeDetectionRange
        * GameConfig::SlimeDetectionRange;
}

void Slime::BeginWindup()
{
    state_ = State::Windup;

    stateRemaining_ =
        GameConfig::SlimeWindupDuration;

    walking_ = false;
}

void Slime::Update(
    float seconds,
    const Player& player)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    UpdateCommon(seconds);

    // 撃破後は再初期化されるまで行動・復活しない。
    if (!IsAlive())
    {
        return;
    }

    stateRemaining_ = AdvanceTimer(
        stateRemaining_,
        seconds
    );

    switch (state_)
    {
    case State::Idle:
    {
        if (!player.IsAlive())
        {
            return;
        }

        if (Detects(player))
        {
            BeginWindup();
            return;
        }

        // 待機と低速の巡回を交互に行う。
        if (stateRemaining_ == 0.0f)
        {
            walking_ = !walking_;

            stateRemaining_ =
                walking_
                ? GameConfig::SlimeWalkDuration
                : GameConfig::SlimeIdleDuration;
        }

        if (walking_)
        {
            if (x_ <= spawnX_ - GameConfig::SlimePatrolRange
                || x_ <= 0.0f)
            {
                facing_ = 1;
            }

            if (x_ >= spawnX_ + GameConfig::SlimePatrolRange
                || x_ >= GameConfig::WindowWidth - width_)
            {
                facing_ = -1;
            }

            MoveX(
                facing_
                * GameConfig::SlimeWalkSpeed
                * seconds
            );
        }

        break;
    }

    case State::Windup:
    {
        if (!player.IsAlive())
        {
            state_ = State::Idle;

            stateRemaining_ =
                GameConfig::SlimeIdleDuration;

            return;
        }

        if (stateRemaining_ == 0.0f)
        {
            // 離陸する瞬間のプレイヤー方向へ飛び込む。
            // 空中では方向転換しない。
            const Rect bounds = player.Bounds();

            facing_ =
                bounds.x + bounds.width * 0.5f
                < x_ + width_ * 0.5f
                ? -1
                : 1;

            velocityY_ =
                -GameConfig::SlimeJumpSpeed;

            state_ = State::Jumping;
        }

        break;
    }

    case State::Jumping:
    {
        MoveX(
            facing_
            * GameConfig::SlimeLeapSpeed
            * seconds
        );

        y_ += velocityY_ * seconds
            + 0.5f
            * GameConfig::Gravity
            * seconds
            * seconds;

        velocityY_ +=
            GameConfig::Gravity * seconds;

        if (velocityY_ >= 0.0f
            && y_ + height_ >= GameConfig::GroundY)
        {
            y_ = GameConfig::GroundY - height_;
            velocityY_ = 0.0f;

            state_ = State::Recovery;

            stateRemaining_ =
                GameConfig::SlimeRecoveryDuration;
        }

        break;
    }

    case State::Recovery:
    {
        // 着地後の隙が終わるまで次の行動を開始しない。
        if (stateRemaining_ == 0.0f)
        {
            state_ = State::Idle;

            stateRemaining_ =
                GameConfig::SlimeIdleDuration;
        }

        break;
    }

    case State::Dead:
        break;
    }
}

bool Slime::Intersects(const Rect& rect) const
{
    // 見た目に合わせて円と矩形で判定する。
    return OverlapsCircle(
        rect,
        x_ + width_ * 0.5f,
        y_ + height_ * 0.5f,
        GameConfig::SlimeRadius
    );
}