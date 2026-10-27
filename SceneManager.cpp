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
    target_.Init();

    showAttackBounds_ = true;
}

void SceneManager::StartStage()
{
    player_.Init();
    target_.Init();

    showAttackBounds_ = true;
    scene_ = Scene::Stage;
}

void SceneManager::Update(const Input& input, float seconds)
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

    // 現段階ではESCでタイトルへ戻る。
    if (input.IsPressed(KEY_INPUT_ESCAPE))
    {
        scene_ = Scene::Title;
        return;
    }

    // 戦闘テスト用のリセット。
    // 正式なゲームオーバー処理はSTEP 6で追加する。
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

    target_.Update(seconds);
    target_.TryHit(player_);

    // 攻撃判定を先に処理し、
    // 倒した標的からは接触ダメージを受けないようにする。
    if (target_.IsAlive()
        && Overlaps(player_.Bounds(), target_.Bounds()))
    {
        const int direction =
            player_.X() + GameConfig::PlayerWidth * 0.5f
            < target_.Bounds().x + GameConfig::TargetWidth * 0.5f
            ? -1
            : 1;

        player_.TakeDamage(
            GameConfig::TargetContactDamage,
            direction
        );
    }
}

void SceneManager::Draw() const
{
    const unsigned int white = GetColor(255, 255, 255);

    if (scene_ == Scene::Title)
    {
        DrawString(
            800, 260,
            "料理アクション - STEP 2",
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
            const int top = MenuTop + MenuSpacing * i;

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

    const int x = static_cast<int>(player_.X());
    const int y = static_cast<int>(player_.Y());

    // 無敵中は表示・非表示を切り替える。
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

    // 攻撃判定のデバッグ表示。
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

    // 戦闘確認用の標的。
    const Rect target = target_.Bounds();

    if (target_.IsAlive())
    {
        DrawBox(
            static_cast<int>(target.x),
            static_cast<int>(target.y),
            static_cast<int>(target.x + target.width),
            static_cast<int>(target.y + target.height),
            GetColor(50, 190, 80),
            TRUE
        );

        DrawFormatString(
            static_cast<int>(target.x),
            static_cast<int>(target.y) - 30,
            white,
            "標的 HP %d",
            target_.Hp()
        );
    }

    if (target_.ShowsDamage())
    {
        DrawFormatString(
            static_cast<int>(target.x),
            static_cast<int>(target.y) - 60,
            GetColor(255, 230, 80),
            "-%d",
            target_.LastDamage()
        );
    }

    // 操作説明。
    DrawString(
        40, 30,
        "STEP 2: A・D 移動 / Space ジャンプ / 左クリック 攻撃",
        white
    );

    DrawString(
        40, 65,
        "F1 攻撃範囲表示の切替 / R テストをリセット / ESC タイトル",
        white
    );

    DrawString(
        40, 100,
        "緑の標的に触れると被ダメージ。敵AIは未実装です。",
        white
    );

    // HPゲージの背景。
    DrawBox(
        40, 145,
        340, 170,
        GetColor(60, 60, 60),
        TRUE
    );

    // 現在HPの割合に応じた緑色のゲージ。
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
    else if (!target_.IsAlive())
    {
        DrawString(
            700, 420,
            "標的を倒しました。Rで再配置できます。",
            white
        );
    }
}