#pragma once

// 画面・物理・戦闘設定。
// 座標の単位はピクセル、時間は秒。
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

    const float MaxFrameSeconds = 0.1f;

    const int PlayerMaxHp = 10;
    const int PlayerAttackPower = 2;

    const float AttackInterval = 0.5f;
    const float AttackDuration = 0.15f;
    const float AttackReach = 100.0f;
    const float AttackHeight = 80.0f;

    const float InvincibleDuration = 1.0f;
    const float BlinkInterval = 0.1f;

    const float KnockbackSpeed = 360.0f;
    const float KnockbackDeceleration = 1800.0f;

    // STEP 2のテスト用として残す設定。
    const float TargetStartX = 760.0f;
    const float TargetWidth = 80.0f;
    const float TargetHeight = 80.0f;
    const int TargetMaxHp = 8;
    const int TargetContactDamage = 2;

    const float DamageTextDuration = 0.6f;

    // 敵共通設定。
    const int EnemyMaxLevel = 30;
    const int StageBaseLevel = 1;

    // スライムの配置。
    const int SlimeCount = 3;

    const float SlimeSpawnX[SlimeCount] =
    {
        760.0f,
        1200.0f,
        1620.0f
    };

    // スライムの基本ステータス。
    const float SlimeRadius = 40.0f;
    const int SlimeBaseHp = 8;
    const int SlimeBaseAttack = 2;

    // 索敵・巡回。
    const float SlimeDetectionRange = 360.0f;
    const float SlimePatrolRange = 120.0f;
    const float SlimeWalkSpeed = 50.0f;

    const float SlimeIdleDuration = 1.0f;
    const float SlimeWalkDuration = 0.8f;

    // ジャンプ攻撃。
    const float SlimeWindupDuration = 0.6f;
    const float SlimeLeapSpeed = 320.0f;
    const float SlimeJumpSpeed = 650.0f;
    const float SlimeRecoveryDuration = 1.0f;
}