#include "CookingManager.h"
#include "Player.h"

CookResult CookingManager::Craft(
    Dish dish,
    bool atKitchen,
    Inventory& ingredients,
    FoodInventory& foods,
    Progression& progress)
{
    if (!atKitchen)
    {
        return CookResult::WrongPlace;
    }

    const Recipe* recipe = FindRecipe(dish);

    if (!recipe)
    {
        return CookResult::InvalidRecipe;
    }

    if (foods.Count(dish) >= CookingSettings::DishLimit)
    {
        return CookResult::Full;
    }

    if (ingredients.Count(recipe->ingredient) < recipe->cost)
    {
        return CookResult::MissingIngredients;
    }

    // 条件をすべて確認してから、食材・料理・EXPを更新する。
    ingredients.Remove(
        recipe->ingredient,
        recipe->cost
    );

    foods.Add(dish, 1);

    progress.AddExp(recipe->rarity * 2);

    return CookResult::Success;
}

bool CookingManager::Apply(Dish dish, Player& player)
{
    const Recipe* recipe = FindRecipe(dish);

    if (!recipe || !player.IsAlive())
    {
        return false;
    }

    switch (recipe->effect)
    {
    case FoodEffect::Heal:
        return player.Heal(
            static_cast<int>(recipe->strength)
        ) > 0;

    case FoodEffect::Jump:
        return player.ApplyJumpBuff(
            recipe->strength,
            recipe->seconds
        );
    }

    return false;
}