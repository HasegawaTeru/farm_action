#include "SceneManager.h"
#include "StageRules.h"
#include "UIConfig.h"
#include "UIHelpers.h"
#include "DxLib.h"

void SceneManager::DrawBoss() const
{
    const BossSlime& boss = session_.Boss();

    const unsigned int white =
        GetColor(255, 255, 255);

    // ボスエリアの入口。
    if (session_.ArenaLocked())
    {
        DrawBox(
            static_cast<int>(StageRules::ArenaLeft) - 8,
            0,
            static_cast<int>(StageRules::ArenaLeft),
            static_cast<int>(GameConfig::GroundY),
            GetColor(180, 90, 50),
            TRUE
        );
    }

    if (!boss.IsAlive())
    {
        return;
    }

    const Rect bounds = boss.Bounds();

    const int x = static_cast<int>(
        bounds.x + bounds.width * 0.5f
        );

    const int y = static_cast<int>(
        bounds.y + bounds.height * 0.5f
        );

    unsigned int color =
        boss.Enraged()
        ? GetColor(220, 80, 160)
        : GetColor(40, 150, 80);

    if (boss.CurrentState() == BossSlime::State::Windup)
    {
        color = GetColor(240, 180, 40);
    }

    DrawCircle(
        x,
        y,
        static_cast<int>(StageRules::BossRadius),
        color,
        TRUE
    );

    DrawString(
        x - 50,
        static_cast<int>(bounds.y) - 30,
        "大スライム",
        white
    );

    if (boss.ShowsDamage())
    {
        DrawFormatString(
            x,
            static_cast<int>(bounds.y) - 60,
            white,
            "-%d",
            boss.LastDamage()
        );
    }

    if (!session_.ArenaLocked())
    {
        return;
    }

    // 画面上部のボスHPゲージ。
    DrawString(
        520, 25,
        boss.Enraged()
        ? "大スライム：怒り状態"
        : "大スライム",
        white
    );

    DrawBox(
        520, 60,
        1420, 85,
        GetColor(60, 60, 60),
        TRUE
    );

    DrawBox(
        520,
        60,
        520 + 900 * boss.Hp() / boss.MaxHp(),
        85,
        GetColor(210, 50, 70),
        TRUE
    );

    DrawFormatString(
        1450, 60,
        white,
        "%d / %d",
        boss.Hp(),
        boss.MaxHp()
    );
}

void SceneManager::DrawResult() const
{
    const StageResult& result = session_.Result();

    const unsigned int white =
        GetColor(255, 255, 255);

    DrawBox(
        400, 180,
        1520, 930,
        GetColor(25, 30, 45),
        TRUE
    );

    DrawString(
        760, 230,
        "ステージクリア！",
        GetColor(255, 220, 60)
    );

    DrawFormatString(
        520, 300,
        white,
        "獲得EXP: %d   討伐した敵: %d",
        result.exp,
        result.defeated
    );

    DrawString(
        520, 350,
        "獲得食材（正式取得済み）",
        white
    );

    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        const Ingredient item =
            static_cast<Ingredient>(i);

        DrawFormatString(
            560,
            400 + i * 40,
            white,
            "%s x%d",
            IngredientName(item),
            result.ingredients.Count(item)
        );
    }

    DrawFormatString(
        520, 560,
        white,
        "Lv.%d → Lv.%d / EXP %d → %d",
        result.oldLevel,
        result.newLevel,
        result.oldExp,
        result.newExp
    );

    if (result.newLevel > result.oldLevel)
    {
        const unsigned int color =
            static_cast<int>(resultTime_ * 3) % 2 == 0
            ? GetColor(255, 220, 60)
            : white;

        DrawFormatString(
            520, 610,
            color,
            "LEVEL UP! 最大HP +%d / 攻撃力 +%d",
            (result.newLevel - result.oldLevel) * 3,
            (result.newLevel - result.oldLevel) * 2
        );
    }

    UI::Button(
        UIConfig::Button(5),
        "町へ戻る（Enter / ESCでも戻れます）"
    );
}