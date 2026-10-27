#pragma once

// 座標の単位はピクセル、時間の単位は秒。
namespace GameConfig
{
    const int WindowWidth = 1920;
    const int WindowHeight = 1080;
    const int TargetFps = 60;

    const float GroundY = 900.0f;

    const float PlayerWidth = 64.0f;
    const float PlayerHeight = 96.0f;

    const float MoveSpeed = 480.0f;
    const float JumpSpeed = 900.0f;
    const float Gravity = 2400.0f;

    // 長時間停止した直後に大きく移動することを防ぐ。
    const float MaxFrameSeconds = 0.1f;
}