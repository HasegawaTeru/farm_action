#include "DebugView.h"
#include "GameSession.h"
#include "UIHelpers.h"

#include <cmath>

#include "DxLib.h"

void DebugView::Init()
{
    enabled_ = false;

    fps_ = 0.0f;
    frameMs_ = 0.0f;
    elapsed_ = 0.0f;

    frames_ = 0;
}

void DebugView::Update(
    float realSeconds,
    bool toggle)
{
    if (toggle)
    {
        enabled_ = !enabled_;
    }

    if (!std::isfinite(realSeconds)
        || realSeconds <= 0)
    {
        return;
    }

    frameMs_ = realSeconds * 1000;

    elapsed_ += realSeconds;
    ++frames_;

    if (elapsed_ >= 0.25f)
    {
        fps_ = frames_ / elapsed_;

        frames_ = 0;
        elapsed_ = 0;
    }
}

void DebugView::Draw(
    const GameSession& game,
    bool menuOpen,
    bool title) const
{
    if (!enabled_)
    {
        return;
    }

    const unsigned int white =
        GetColor(255, 255, 255);

    const Player& player = game.Actor();

    if (!title && game.InStage())
    {
        UI::Box(
            player.Bounds(),
            GetColor(30, 240, 255),
            false
        );

        if (player.IsAttacking())
        {
            UI::Box(
                player.AttackBounds(),
                GetColor(255, 220, 50),
                false
            );
        }

        for (const Slime& slime : game.Slimes())
        {
            if (slime.IsAlive())
            {
                UI::Box(
                    slime.Bounds(),
                    GetColor(50, 255, 100),
                    false
                );
            }
        }

        if (game.Boss().IsAlive())
        {
            UI::Box(
                game.Boss().Bounds(),
                GetColor(255, 60, 100),
                false
            );
        }

        for (const IngredientDrop& drop : game.Items().Drops())
        {
            UI::Box(
                drop.bounds,
                GetColor(255, 220, 50),
                false
            );
        }

        for (const GatherPoint& point : game.Items().Points())
        {
            UI::Box(
                point.bounds,
                GetColor(220, 80, 255),
                false
            );
        }
    }
    else if (!title && !game.InResult())
    {
        UI::Box(
            {
                game.TownState().X()
                    - game.TownState().CameraX(),

                GameConfig::GroundY
                    - GameConfig::PlayerHeight,

                GameConfig::PlayerWidth,
                GameConfig::PlayerHeight
            },
            GetColor(30, 240, 255),
            false
        );
    }

    UI::Box(
        { 20, 490, 400, 550 },
        GetColor(15, 20, 30)
    );

    DrawFormatString(
        40, 510,
        white,
        "DEBUG [F2] %.1f FPS / %.2f ms",
        fps_,
        frameMs_
    );

    const char* scene =
        title
        ? "TITLE"
        : (
            game.InStage()
            ? "STAGE"
            : (
                game.InResult()
                ? "RESULT"
                : "TOWN"
                )
            );

    DrawFormatString(
        40, 540,
        white,
        "%s / %s",
        scene,
        menuOpen || game.InResult() || title
        ? "PAUSED"
        : "RUNNING"
    );

    if (title)
    {
        return;
    }

    const float x =
        game.InStage() || game.InResult()
        ? player.X()
        : game.TownState().X();

    DrawFormatString(
        40, 570,
        white,
        "X %.1f / Y %.1f",
        x,
        player.Y()
    );

    DrawFormatString(
        40, 600,
        white,
        "HP %d/%d / facing %d",
        player.Hp(),
        player.MaxHp(),
        player.Facing()
    );

    DrawFormatString(
        40, 630,
        white,
        "ground %d / attack %d / invincible %d",
        player.IsGrounded(),
        player.IsAttacking(),
        player.IsInvincible()
    );

    DrawFormatString(
        40, 660,
        white,
        "buff %.2fx / %.2fs",
        player.JumpMultiplier(),
        player.BuffRemaining()
    );

    DrawFormatString(
        40, 690,
        white,
        "Lv.%d EXP %d / pending %d",
        game.Progress().Level(),
        game.Progress().Exp(),
        game.PendingExp()
    );

    DrawFormatString(
        40, 720,
        white,
        "food %d / carried %d / drops %d",
        game.Foods().Total(),
        game.Carried().Total(),
        static_cast<int>(game.Items().Drops().size())
    );

    if (!game.InStage())
    {
        return;
    }

    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        DrawFormatString(
            40,
            760 + i * 30,
            white,
            "enemy %d HP %d %s",
            i,
            game.Slimes()[i].Hp(),
            game.Slimes()[i].StateName()
        );
    }

    DrawFormatString(
        40, 880,
        white,
        "boss HP %d/%d %s",
        game.Boss().Hp(),
        game.Boss().MaxHp(),
        game.Boss().StateName()
    );

    DrawFormatString(
        40, 910,
        white,
        "arena %d / enraged %d / kills %d",
        game.ArenaLocked(),
        game.Boss().Enraged(),
        game.DefeatedCount()
    );
}