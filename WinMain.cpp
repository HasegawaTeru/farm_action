#include <chrono>
#include <thread>

#include "GameConfig.h"
#include "Input.h"
#include "SceneManager.h"

#include "DxLib.h"

#define WINDOW_W GameConfig::WindowWidth
#define WINDOW_H GameConfig::WindowHeight

int WINAPI WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd)
{
    // 既存の初期化処理を利用する。
    SetOutApplicationLogValidFlag(FALSE);

    ChangeWindowMode(TRUE);
    SetGraphMode(WINDOW_W, WINDOW_H, 32);

    SetMainWindowText("farm action");
    SetBackgroundColor(128, 128, 128);

    if (DxLib_Init() == -1)
    {
        return -1;
    }

    // 裏画面に描画し、完成した画面をまとめて表示する。
    SetDrawScreen(DX_SCREEN_BACK);
    SetMouseDispFlag(TRUE);

    Input input;
    SceneManager scenes;

    scenes.Init();

    // CPU時間ではなく実際の経過時間を使う。
    typedef std::chrono::steady_clock Clock;

    const std::chrono::microseconds frameTime(
        1000000 / GameConfig::TargetFps
    );

    Clock::time_point previous = Clock::now();

    while (ProcessMessage() == 0)
    {
        const Clock::time_point frameStart = Clock::now();

        const float seconds =
            std::chrono::duration<float>(
                frameStart - previous
            ).count();

        previous = frameStart;

        // 入力 → 更新 → 描画の順で処理する。
        input.Update();
        scenes.Update(input, seconds);

        if (scenes.ShouldQuit())
        {
            break;
        }

        ClearDrawScreen();
        scenes.Draw();

        if (ScreenFlip() == -1)
        {
            break;
        }

        // 次のフレームまで待機する。
        std::this_thread::sleep_until(frameStart + frameTime);
    }

    DxLib_End();
    return 0;
}