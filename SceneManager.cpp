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
}

void SceneManager::StartStage()
{
    player_.Init();
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

        // マウスでタイトルの項目を選択する。
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

    // STEP 1ではESCでタイトルへ戻る。
    if (input.IsPressed(KEY_INPUT_ESCAPE))
    {
        scene_ = Scene::Title;
        return;
    }

    const int horizontal =
        (input.IsDown(KEY_INPUT_D) ? 1 : 0)
        - (input.IsDown(KEY_INPUT_A) ? 1 : 0);

    player_.Update(
        seconds,
        horizontal,
        input.IsPressed(KEY_INPUT_SPACE)
    );
}

void SceneManager::Draw() const
{
    const unsigned int white = GetColor(255, 255, 255);

    if (scene_ == Scene::Title)
    {
        DrawString(
            800, 260,
            "料理アクション - STEP 1",
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
                "STEP 1では設定は固定です。PCの設定は変更しません。",
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

    // 地面を茶色の長方形で描画する。
    DrawBox(
        0,
        static_cast<int>(GameConfig::GroundY),
        GameConfig::WindowWidth,
        GameConfig::WindowHeight,
        GetColor(125, 80, 45),
        TRUE
    );

    // プレイヤーを青色の四角形で描画する。
    const int x = static_cast<int>(player_.X());
    const int y = static_cast<int>(player_.Y());

    DrawBox(
        x,
        y,
        x + static_cast<int>(GameConfig::PlayerWidth),
        y + static_cast<int>(GameConfig::PlayerHeight),
        GetColor(40, 120, 255),
        TRUE
    );

    DrawString(
        40, 40,
        "STEP 1: A・Dで移動 / Spaceでジャンプ / ESCでタイトル",
        white
    );

    DrawString(
        40, 80,
        player_.IsGrounded() ? "接地中" : "空中",
        white
    );
}