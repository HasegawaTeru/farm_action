#pragma once

#include "ItemData.h"

enum class Dish
{
    Jelly,
    Healing,
    Count
};

enum class FoodEffect
{
    Jump,
    Heal
};

namespace CookingSettings
{
    const int DishCount = static_cast<int>(Dish::Count);
    const int DishLimit = 99;
    const int CarryLimit = 2;
    const int StarterIngredients = 2;
}

struct Recipe
{
    Dish dish;
    const char* name;
    int rarity;

    Ingredient ingredient;
    int cost;

    FoodEffect effect;
    float strength;
    float seconds;
};

const Recipe* FindRecipe(Dish dish);