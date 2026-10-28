#pragma once

#include "Collision.h"

enum class Ingredient
{
    SlimeJelly,
    Herb,
    BossJelly,
    Count
};

inline const char* IngredientName(Ingredient item)
{
    switch (item)
    {
    case Ingredient::SlimeJelly:
        return "スライムゼリー";

    case Ingredient::Herb:
        return "香草";

    case Ingredient::BossJelly:
        return "大スライムゼリー";

    default:
        return "不明な食材";
    }
}

// 食材に関する調整用設定。
namespace ItemSettings
{
    const int IngredientCount =
        static_cast<int>(Ingredient::Count);

    const int IngredientLimit = 999;

    const int MaxSlimeDrops = 2;

    const float DropSize = 20.0f;
    const float DropSpacing = 28.0f;

    const int GatherPointCount = 2;

    const float GatherX[GatherPointCount] =
    {
        500.0f,
        1400.0f
    };

    const float GatherWidth = 40.0f;
    const float GatherHeight = 50.0f;
    const float GatherRange = 100.0f;

    const int HerbsPerGather = 1;

    const float NoticeDuration = 2.0f;
}

// 地面に落ちている食材。
struct IngredientDrop
{
    Rect bounds;
    Ingredient item;
    int quantity;
};

// 香草の採取ポイント。
struct GatherPoint
{
    Rect bounds;
    bool depleted;
};