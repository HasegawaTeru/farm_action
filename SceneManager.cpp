#include "SceneManager.h"
#include "GameConfig.h"
#include "Input.h"
#include "DxLib.h"

namespace
{
    const int MenuLeft = 700;
    const int MenuRight = 1220;
    const int MenuTop = 420;
    const int MenuSpacing = 110;
    const int MenuHeight = 80;
}

void SceneManager::Init()
{
    scene_ = Scene::Title;

    quit_ = false;
    showSettings_ = false;

    player_.Init();

    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        slimes_[i].Init(
            GameConfig::SlimeSpawnX[i],
            GameConfig::StageBaseLevel,
            1
        );
    }

    defeatedCount_ = 0;
    showAttackBounds_ = true;
}

void SceneManager::StartStage()
{
    player_.Init();

    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        slimes_[i].Init(
            GameConfig::SlimeSpawnX[i],
            GameConfig::StageBaseLevel,
            1
        );
    }

    defeatedCount_ = 0;
    showAttackBounds_ = true;

    scene_ = Scene::Stage;
}

void SceneManager::Update(
    const Input& input,
    float seconds)
{
    if (scene_ == Scene::Title)
    {
        if (showSettings_)
        {
            if (input.IsPressed(KEY_INPUT_ESCAPE)
                || input.IsLeftClickPressed())
            {
                showSettings_ = false;
            }

            return;
        }

        if (input.IsPressed(KEY_INPUT_RETURN))
        {
            StartStage();
            return;
        }

        if (input.IsLeftClickPressed()
            && input.MouseX() >= MenuLeft
            && input.MouseX() < MenuRight)
        {
            const int y = input.MouseY();

            if (y >= MenuTop
                && y < MenuTop + MenuHeight)
            {
                StartStage();
            }
            else if (y >= MenuTop + MenuSpacing
                && y < MenuTop + MenuSpacing + MenuHeight)
            {
                showSettings_ = true;
            }
            else if (y >= MenuTop + 2 * MenuSpacing
                && y < MenuTop + 2 * MenuSpacing + MenuHeight)
            {
                quit_ = true;
            }
        }

        return;
    }

    // 正式なゲーム内メニューは後続STEPで追加する。
    if (input.IsPressed(KEY_INPUT_ESCAPE))
    {
        scene_ = Scene::Title;
        return;
    }

    // Rは動作確認用の再挑戦。
    if (input.IsPressed(KEY_INPUT_R))
    {
        StartStage();
        return;
    }

    if (input.IsPressed(KEY_INPUT_F1))
    {
        showAttackBounds_ = !showAttackBounds_;
    }

    const int horizontal =
        (input.IsDown(KEY_INPUT_D) ? 1 : 0)
        - (input.IsDown(KEY_INPUT_A) ? 1 : 0);

    player_.Update(
        seconds,
        horizontal,
        input.IsPressed(KEY_INPUT_SPACE),
        input.IsLeftClickPressed()
    );

    for (Slime& slime : slimes_)
    {
        slime.Update(seconds, player_);

        const bool wasAlive = slime.IsAlive();

        slime.TryHit(player_);

        // 生存から撃破へ変化した瞬間に一度だけ加算する。
        if (wasAlive && !slime.IsAlive())
        {
            ++defeatedCount_;
        }

        // 倒した敵は接触ダメージを発生させない。
        if (slime.IsAlive()
            && slime.Intersects(player_.Bounds()))
        {
            const Rect bounds = slime.Bounds();

            const int direction =
                player_.X() + GameConfig::PlayerWidth * 0.5f
                < bounds.x + bounds.width * 0.5f
                ? -1
                : 1;

            player_.TakeDamage(
                slime.AttackPower(),
                direction
            );
        }
    }
}

void SceneManager::Draw() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    if (scene_ == Scene::Title)
    {
        DrawString(
            800, 260,
            "料理アクション - STEP 3",
            white
        );

        if (showSettings_)
        {
            DrawString(
                700, 420,
                "画面: 1920 x 1080 / 目標: 60 FPS",
                white
            );

            DrawString(
                700, 470,
                "設定は固定です。PCの設定は変更しません。",
                white
            );

            DrawString(
                700, 520,
                "クリックまたはESCで戻る",
                white
            );

            return;
        }

        const char* labels[] =
        {
            "はじめから（動作確認ステージ）",
            "設定",
            "ゲーム終了"
        };

        for (int i = 0; i < 3; ++i)
        {
            const int top =
                MenuTop + MenuSpacing * i;

            DrawBox(
                MenuLeft,
                top,
                MenuRight,
                top + MenuHeight,
                GetColor(55, 65, 85),
                TRUE
            );

            DrawString(
                MenuLeft + 30,
                top + 28,
                labels[i],
                white
            );
        }

        DrawString(
            700, 800,
            "クリックで選択 / Enterでも開始できます",
            white
        );

        return;
    }

    // 地面。
    DrawBox(
        0,
        static_cast<int>(GameConfig::GroundY),
        GameConfig::WindowWidth,
        GameConfig::WindowHeight,
        GetColor(125, 80, 45),
        TRUE
    );

    // プレイヤー。
    const int x = static_cast<int>(player_.X());
    const int y = static_cast<int>(player_.Y());

    if (player_.IsVisible())
    {
        DrawBox(
            x,
            y,
            x + static_cast<int>(GameConfig::PlayerWidth),
            y + static_cast<int>(GameConfig::PlayerHeight),
            GetColor(40, 120, 255),
            TRUE
        );

        // プレイヤーの向きを示す目印。
        const int eyeX =
            player_.Facing() > 0 ? x + 48 : x + 8;

        DrawBox(
            eyeX,
            y + 20,
            eyeX + 8,
            y + 28,
            white,
            TRUE
        );
    }

    // プレイヤーの攻撃判定。
    if (showAttackBounds_ && player_.IsAttacking())
    {
        const Rect hit = player_.AttackBounds();

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
    for (const Slime& slime : slimes_)
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

            // スライムの向きを示す目印。
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

        // 撃破直後も一定時間はダメージ数値を表示する。
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

    // 操作説明。
    DrawString(
        40, 30,
        "STEP 3: A・D 移動 / Space ジャンプ / 左クリック 攻撃",
        white
    );

    DrawString(
        40, 65,
        "F1 攻撃範囲表示の切替 / R テストをリセット / ESC タイトル",
        white
    );

    DrawString(
        40, 100,
        "黄: 予備動作 / 赤: 飛び込み / くすんだ緑: 着地後の隙",
        white
    );

    // HPゲージ。
    DrawBox(
        40, 145,
        340, 170,
        GetColor(60, 60, 60),
        TRUE
    );

    DrawBox(
        40, 145,
        40 + 300 * player_.Hp() / player_.MaxHp(),
        170,
        GetColor(40, 210, 70),
        TRUE
    );

    DrawFormatString(
        40, 185,
        white,
        "Lv.1  HP %d / %d  攻撃力 %d",
        player_.Hp(),
        player_.MaxHp(),
        player_.AttackPower()
    );

    if (!player_.IsAlive())
    {
        DrawString(
            700, 420,
            "HPが0になりました。Rでテストを再開できます。",
            white
        );
    }
    else if (defeatedCount_ == GameConfig::SlimeCount)
    {
        DrawString(
            700, 420,
            "全スライムを撃破しました。Rで再挑戦できます。",
            white
        );
    }

    DrawFormatString(
        40, 220,
        white,
        "討伐数 %d / %d",
        defeatedCount_,
        GameConfig::SlimeCount
    );
}