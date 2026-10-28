#include "SceneManager.h"
#include "UIConfig.h"
#include "DxLib.h"

namespace
{
    void Box(
        const Rect& rect,
        unsigned int color,
        bool fill = true)
    {
        DrawBox(
            static_cast<int>(rect.x),
            static_cast<int>(rect.y),
            static_cast<int>(rect.x + rect.width),
            static_cast<int>(rect.y + rect.height),
            color,
            fill ? TRUE : FALSE
        );
    }

    void Button(
        const Rect& rect,
        const char* text,
        bool selected = false)
    {
        Box(rect, GetColor(55, 65, 85));

        if (selected)
        {
            Box(
                rect,
                GetColor(255, 220, 60),
                false
            );
        }

        DrawString(
            static_cast<int>(rect.x) + 15,
            static_cast<int>(rect.y) + 18,
            text,
            GetColor(255, 255, 255)
        );
    }
}

void SceneManager::DrawTitle() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    DrawString(
        800, 240,
        "料理アクション",
        white
    );

    Button(UIConfig::Button(0), "はじめから");
    Button(UIConfig::Button(1), "設定");
    Button(UIConfig::Button(2), "ゲーム終了");

    DrawString(
        520, 640,
        "クリックで選択 / Enterでも開始できます",
        white
    );
}

void SceneManager::DrawTown() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    const Town& town = session_.TownState();
    const float camera = town.CameraX();

    // 地面。
    Box(
        {
            0,
            GameConfig::GroundY,
            static_cast<float>(GameConfig::WindowWidth),
            GameConfig::WindowHeight - GameConfig::GroundY
        },
        GetColor(125, 80, 45)
    );

    const char* names[] =
    {
        "主人公の家",
        "調理場",
        "ステージ掲示板"
    };

    for (int i = 0; i < 3; ++i)
    {
        const int x = static_cast<int>(
            TownSettings::FacilityX[i] - camera
            );

        if (i == 0)
        {
            // 主人公の家。
            Box(
                {
                    static_cast<float>(x - 90),
                    GameConfig::GroundY - 180,
                    180,
                    180
                },
                GetColor(170, 130, 90)
            );

            DrawLine(x - 110, 720, x, 640, white);
            DrawLine(x, 640, x + 110, 720, white);
        }
        else if (i == 1)
        {
            // 鍋を模した調理場。
            DrawCircle(
                x,
                830,
                50,
                GetColor(75, 85, 100),
                TRUE
            );

            Box(
                {
                    static_cast<float>(x - 65),
                    790,
                    130,
                    12
                },
                white
            );

            DrawLine(x - 40, 870, x - 60, 900, white);
            DrawLine(x + 40, 870, x + 60, 900, white);
        }
        else
        {
            // ステージ選択の掲示板。
            Box(
                {
                    static_cast<float>(x - 90),
                    700,
                    180,
                    120
                },
                GetColor(150, 105, 50)
            );

            DrawLine(x - 50, 820, x - 50, 900, white);
            DrawLine(x + 50, 820, x + 50, 900, white);
        }

        DrawString(
            x - 70,
            610,
            names[i],
            white
        );

        if (town.NearbyFacility()
            == static_cast<Town::Facility>(i))
        {
            DrawString(
                x - 50,
                580,
                "Eで調べる",
                white
            );
        }
    }

    // 町にいるプレイヤー。
    const int x = static_cast<int>(
        town.X() - camera
        );

    Box(
        {
            static_cast<float>(x),
            GameConfig::GroundY - GameConfig::PlayerHeight,
            GameConfig::PlayerWidth,
            GameConfig::PlayerHeight
        },
        GetColor(40, 120, 255)
    );

    const int eye =
        town.Facing() > 0 ? x + 48 : x + 8;

    DrawBox(
        eye,
        824,
        eye + 8,
        832,
        white,
        TRUE
    );

    DrawString(
        40, 220,
        "町: A・D 移動 / 施設の近くでE / ESC メニュー",
        white
    );

    DrawString(
        40, 250,
        "家 → 調理場 → 掲示板の順に右へ進めます。",
        white
    );
}

void SceneManager::DrawStage() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    // 地面。
    DrawBox(
        0,
        static_cast<int>(GameConfig::GroundY),
        GameConfig::WindowWidth,
        GameConfig::WindowHeight,
        GetColor(125, 80, 45),
        TRUE
    );

    // 落ちている食材。
    for (const IngredientDrop& drop
        : session_.Items().Drops())
    {
        const Rect& bounds = drop.bounds;

        DrawBox(
            static_cast<int>(bounds.x),
            static_cast<int>(bounds.y),
            static_cast<int>(bounds.x + bounds.width),
            static_cast<int>(bounds.y + bounds.height),
            GetColor(255, 220, 40),
            TRUE
        );
    }

    const int nearby =
        session_.Actor().IsAlive()
        ? session_.Items().NearbyPoint(
            session_.Actor().Bounds()
        )
        : -1;

    // 採取ポイント。
    for (int i = 0; i < ItemSettings::GatherPointCount; ++i)
    {
        const GatherPoint& point =
            session_.Items().Points()[i];

        const Rect& bounds = point.bounds;

        const int centerX = static_cast<int>(
            bounds.x + bounds.width * 0.5f
            );

        DrawBox(
            centerX - 4,
            static_cast<int>(bounds.y),
            centerX + 4,
            static_cast<int>(bounds.y + bounds.height),
            point.depleted
            ? GetColor(100, 100, 100)
            : GetColor(30, 170, 60),
            TRUE
        );

        if (!point.depleted)
        {
            DrawCircle(
                centerX,
                static_cast<int>(bounds.y) + 10,
                18,
                GetColor(60, 220, 90),
                TRUE
            );
        }

        DrawString(
            centerX - 40,
            static_cast<int>(bounds.y) - 30,
            point.depleted ? "採取済み" : "香草",
            white
        );

        if (nearby == i)
        {
            DrawString(
                centerX - 40,
                static_cast<int>(bounds.y) - 60,
                "Eで採取",
                white
            );
        }
    }

    // プレイヤー。
    const int x =
        static_cast<int>(session_.Actor().X());

    const int y =
        static_cast<int>(session_.Actor().Y());

    if (session_.Actor().IsVisible())
    {
        DrawBox(
            x,
            y,
            x + static_cast<int>(GameConfig::PlayerWidth),
            y + static_cast<int>(GameConfig::PlayerHeight),
            GetColor(40, 120, 255),
            TRUE
        );

        // 向きを示す目印。
        const int eyeX =
            session_.Actor().Facing() > 0
            ? x + 48
            : x + 8;

        DrawBox(
            eyeX,
            y + 20,
            eyeX + 8,
            y + 28,
            white,
            TRUE
        );
    }

    // 攻撃判定のデバッグ表示。
    if (showAttackBounds_
        && session_.Actor().IsAttacking())
    {
        const Rect hit =
            session_.Actor().AttackBounds();

        DrawBox(
            static_cast<int>(hit.x),
            static_cast<int>(hit.y),
            static_cast<int>(hit.x + hit.width),
            static_cast<int>(hit.y + hit.height),
            GetColor(255, 220, 50),
            FALSE
        );
    }

    // スライム。
    for (const Slime& slime : session_.Slimes())
    {
        const Rect bounds = slime.Bounds();

        const int centerX = static_cast<int>(
            bounds.x + bounds.width * 0.5f
            );

        const int centerY = static_cast<int>(
            bounds.y + bounds.height * 0.5f
            );

        if (slime.IsAlive())
        {
            const char* stateText = "待機・巡回";

            unsigned int color =
                GetColor(50, 190, 80);

            switch (slime.CurrentState())
            {
            case Slime::State::Windup:
                stateText = "予備動作";
                color = GetColor(240, 190, 40);
                break;

            case Slime::State::Jumping:
                stateText = "飛び込み";
                color = GetColor(240, 80, 50);
                break;

            case Slime::State::Recovery:
                stateText = "着地後の隙";
                color = GetColor(100, 160, 100);
                break;

            default:
                break;
            }

            DrawCircle(
                centerX,
                centerY,
                static_cast<int>(GameConfig::SlimeRadius),
                color,
                TRUE
            );

            DrawCircle(
                centerX + slime.Facing() * 14,
                centerY - 10,
                5,
                white,
                TRUE
            );

            DrawFormatString(
                static_cast<int>(bounds.x) - 20,
                static_cast<int>(bounds.y) - 50,
                white,
                "Lv.%d HP %d/%d",
                slime.Level(),
                slime.Hp(),
                slime.MaxHp()
            );

            DrawString(
                static_cast<int>(bounds.x) - 20,
                static_cast<int>(bounds.y) - 25,
                stateText,
                white
            );
        }

        if (slime.ShowsDamage())
        {
            DrawFormatString(
                centerX,
                static_cast<int>(bounds.y) - 80,
                GetColor(255, 230, 80),
                "-%d",
                slime.LastDamage()
            );
        }
    }

    DrawString(
        40, 220,
        "A・D 移動 / Space ジャンプ / 左クリック 攻撃 / E 採取 / ESC メニュー",
        white
    );

    // 正式な所持数と、今回の一時取得分。
    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        const Ingredient item =
            static_cast<Ingredient>(i);

        DrawFormatString(
            40,
            250 + i * 30,
            white,
            "%s 所持 %d / 今回 +%d",
            IngredientName(item),
            session_.Ingredients().Count(item),
            session_.Items().Pending().Count(item)
        );
    }

    DrawBoss();

    DrawFormatString(
        40, 350,
        white,
        "討伐 %d / %d   今回EXP +%d",
        session_.DefeatedCount(),
        GameConfig::SlimeCount + 1,
        session_.PendingExp()
    );

    if (session_.Items().HasNotice())
    {
        DrawFormatString(
            700, 100,
            GetColor(255, 230, 80),
            "%s %s%d",
            IngredientName(session_.Items().NoticeItem()),
            session_.Items().NoticeQuantity() > 0
            ? "取得 +"
            : "所持上限 ",
            session_.Items().NoticeQuantity()
        );
    }

    if (session_.DefeatedCount() == GameConfig::SlimeCount
        && !session_.ArenaLocked())
    {
        DrawString(
            700, 420,
            "通常の敵を撃破！ 右へ進んで大スライムを倒しましょう。",
            white
        );
    }
}

void SceneManager::DrawHud() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    const Player& player = session_.Actor();

    DrawFormatString(
        40, 30,
        white,
        "Lv.%d HP %d/%d 攻撃力 %d",
        session_.Progress().Level(),
        player.Hp(),
        player.MaxHp(),
        player.AttackPower()
    );

    // HPゲージの背景。
    Box(
        { 40, 65, 300, 25 },
        GetColor(60, 60, 60)
    );

    // 現在HP。
    Box(
        {
            40,
            65,
            300.0f * player.Hp() / player.MaxHp(),
            25
        },
        GetColor(40, 210, 70)
    );

    DrawFormatString(
        40, 110,
        white,
        "EXP %d / %d",
        session_.Progress().Exp(),
        session_.Progress().RequiredExp()
    );

    DrawFormatString(
        40, 140,
        white,
        "料理所持 %d / 持ち込み残り %d",
        session_.Foods().Total(),
        session_.Carried().Total()
    );

    if (player.BuffRemaining() > 0.0f)
    {
        DrawFormatString(
            40, 170,
            white,
            "ジャンプ x%.2f 残り %.1f 秒",
            player.JumpMultiplier(),
            player.BuffRemaining()
        );
    }
}

void SceneManager::DrawMenu() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    // メニュー背景。
    Box(
        { 400, 200, 1120, 720 },
        GetColor(25, 30, 45)
    );

    DrawString(
        440, 240,
        "クリックで選択 / ESCで戻る",
        white
    );

    Button(UIConfig::Button(5), "戻る");

    switch (menu_)
    {
    case Menu::Pause:
    {
        DrawString(
            440, 285,
            "メニュー（時間停止中）",
            white
        );

        Button(UIConfig::Button(0), "設定");
        Button(UIConfig::Button(1), "持ち物");

        Button(
            UIConfig::Button(2),
            session_.InStage()
            ? "ステージから出る（一時食材は破棄）"
            : "タイトルへ戻る"
        );

        break;
    }

    case Menu::Settings:
    {
        DrawString(
            520, 360,
            "1920 x 1080 / 目標60FPS / 設定値は固定です。",
            white
        );

        DrawString(
            520, 410,
            "A・D: 移動  Space: ジャンプ  左クリック: 攻撃",
            white
        );

        DrawString(
            520, 460,
            "E: 調べる・採取  ESC: メニュー  F: 選択した料理を食べる",
            white
        );

        break;
    }

    case Menu::House:
    {
        DrawString(
            440, 285,
            "主人公の家",
            white
        );

        Button(UIConfig::Button(0), "休む");

        break;
    }

    case Menu::Kitchen:
    {
        DrawString(
            440, 285,
            "調理場: レシピをクリックすると1個作ります。",
            white
        );

        for (int i = 0; i < CookingSettings::DishCount; ++i)
        {
            const Recipe& recipe =
                *FindRecipe(static_cast<Dish>(i));

            const Rect row = UIConfig::Button(i);

            Button(row, recipe.name);

            DrawFormatString(
                static_cast<int>(row.x) + 380,
                static_cast<int>(row.y) + 18,
                white,
                "★%d %s %d/%d 料理 %d/99",
                recipe.rarity,
                IngredientName(recipe.ingredient),
                session_.Ingredients().Count(recipe.ingredient),
                recipe.cost,
                session_.Foods().Count(recipe.dish)
            );
        }

        DrawString(
            520, 550,
            "スライムゼリー料理: ジャンプ力1.15倍・20秒",
            white
        );

        DrawString(
            520, 590,
            "簡易回復料理: HPを5回復 / ★1の料理作成EXP: 2",
            white
        );

        break;
    }

    case Menu::Board:
    {
        DrawString(
            440, 285,
            "草原ステージ: 持ち込み料理を合計2個まで選択",
            white
        );

        for (int i = 0; i < CookingSettings::DishCount; ++i)
        {
            const Dish dish =
                static_cast<Dish>(i);

            const Rect row =
                UIConfig::FoodRow(i);

            Button(
                row,
                FindRecipe(dish)->name
            );

            DrawFormatString(
                740,
                static_cast<int>(row.y) + 55,
                white,
                "所持 %d / 選択 %d",
                session_.Foods().Count(dish),
                selection_.Count(dish)
            );

            Button(UIConfig::Minus(i), "-");
            Button(UIConfig::Plus(i), "+");
        }

        DrawFormatString(
            520, 590,
            white,
            "選択合計 %d / 2 （0個でも出発できます）",
            selection_.Total()
        );

        Button(
            UIConfig::Button(4),
            "草原へ出発"
        );

        break;
    }

    case Menu::Inventory:
    {
        DrawString(
            440, 285,
            "持ち物（時間停止中）",
            white
        );

        Button(
            UIConfig::Tab(0),
            "食材",
            !foodCategory_
        );

        Button(
            UIConfig::Tab(1),
            "料理",
            foodCategory_
        );

        if (!foodCategory_)
        {
            for (int i = 0; i < ItemSettings::IngredientCount; ++i)
            {
                const Ingredient item =
                    static_cast<Ingredient>(i);

                const Rect row =
                    UIConfig::FoodRow(i);

                Button(
                    row,
                    IngredientName(item)
                );

                DrawFormatString(
                    1000,
                    static_cast<int>(row.y) + 60,
                    white,
                    "所持 %d / 今回 +%d",
                    session_.Ingredients().Count(item),
                    session_.Items().Pending().Count(item)
                );
            }
        }
        else
        {
            for (int i = 0; i < CookingSettings::DishCount; ++i)
            {
                const Dish dish =
                    static_cast<Dish>(i);

                const Rect row =
                    UIConfig::FoodRow(i);

                Button(
                    row,
                    FindRecipe(dish)->name,
                    selectedDish_ == dish
                );

                DrawFormatString(
                    1000,
                    static_cast<int>(row.y) + 60,
                    white,
                    "所持 %d / 持込 %d",
                    session_.Foods().Count(dish),
                    session_.Carried().Count(dish)
                );
            }

            DrawString(
                720, 610,
                session_.InStage()
                ? "料理を選択してFで食べる（成功すると再開）"
                : "料理はステージで持ち物からFで使えます。",
                white
            );
        }

        break;
    }

    default:
        break;
    }

    if (!title_)
    {
        DrawString(
            440, 840,
            session_.Message(),
            GetColor(255, 230, 80)
        );
    }
}