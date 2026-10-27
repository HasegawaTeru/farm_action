#pragma once

#include "ItemData.h"
#include "RecipeData.h"

#include <array>

// 食材の所持数。
// 正式な持ち物とステージの一時取得分の両方で使用する。
class Inventory
{
public:
    void Clear();

    int Count(Ingredient item) const;
    int Add(Ingredient item, int quantity);

    bool Remove(Ingredient item, int quantity);

private:
    std::array<int, ItemSettings::IngredientCount> counts_{};
};

// 料理の所持数。
// 食材とは別のカウンターで管理する。
class FoodInventory
{
public:
    void Clear();

    int Count(Dish dish) const;
    int Total() const;
    int Add(Dish dish, int quantity);

    bool Remove(Dish dish, int quantity);

private:
    std::array<int, CookingSettings::DishCount> counts_{};
};