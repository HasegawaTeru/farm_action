#pragma once

#include "ItemData.h"

#include <array>

// 正式な持ち物と、一時取得分の両方で使用する食材カウンター。
class Inventory
{
public:
    void Clear();

    int Count(Ingredient item) const;

    // 実際に追加できた個数を返す。
    int Add(Ingredient item, int quantity);

private:
    std::array<int, ItemSettings::IngredientCount> counts_{};
};