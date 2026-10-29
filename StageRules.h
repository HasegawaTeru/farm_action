#pragma once
#include "GameConfig.h"

namespace StageRules
{
    // 通常敵はボスエリアより手前に配置する。
    const float NormalSpawnX[3] =
    {
        600.0f,
        950.0f,
        1200.0f
    };

    static_assert(
        sizeof(NormalSpawnX) / sizeof(NormalSpawnX[0])
        == GameConfig::SlimeCount,
        "NormalSpawnX must contain one spawn for every slime."
        );

    const float ArenaLeft = 1400.0f;
    const float ArenaRight = 1920.0f;

    const float BossX = 1700.0f;
    const float BossRadius = 72.0f;

    const int BossBaseHp = 24;
    const int BossBaseAttack = 3;

    const int BossExp = 12;
    const int BossIngredientCount = 1;

    const float Windup = 0.8f;
    const float AngryWindup = 0.4f;

    const float LeapSpeed = 250.0f;
    const float AngryLeapSpeed = 400.0f;

    const float JumpSpeed = 650.0f;
    const float AngryJumpSpeed = 750.0f;

    const float Recovery = 1.1f;
    const float AngryRecovery = 0.55f;
}