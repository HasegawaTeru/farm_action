#pragma once

#include "Inventory.h"

enum class StageEndReason
{
    Clear,
    Retire,
    Defeat
};

struct StageResult
{
    Inventory ingredients;

    int exp = 0;
    int defeated = 0;

    int oldLevel = 1;
    int newLevel = 1;

    int oldExp = 0;
    int newExp = 0;
};