#include "RecipeData.h"

const Recipe* FindRecipe(Dish dish)
{
    // 固定レシピ。必要食材と効果をデータで管理する。
    static const Recipe recipes[] =
    {
        {
            Dish::Jelly,
            "スライムゼリー料理",
            1,
            Ingredient::SlimeJelly,
            2,
            FoodEffect::Jump,
            1.15f,
            20.0f
        },
        {
            Dish::Healing,
            "簡易回復料理",
            1,
            Ingredient::Herb,
            2,
            FoodEffect::Heal,
            5.0f,
            0.0f
        }
    };

    for (const Recipe& recipe : recipes)
    {
        if (recipe.dish == dish)
        {
            return &recipe;
        }
    }

    return nullptr;
}