#pragma once

#include "Inventory.h"
#include "Progression.h"

#include <array>

// 正式な進行データだけを保存する。
struct SaveData
{
    int level = 1;
    int exp = 0;

    bool stageCleared = false;

    std::array<
        int,
        ItemSettings::IngredientCount
    > ingredients{};

    std::array<
        int,
        CookingSettings::DishCount
    > foods{};

    bool Valid() const;
};