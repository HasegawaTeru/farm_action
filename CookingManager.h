#pragma once

#include "Inventory.h"
#include "Progression.h"

class Player;

enum class CookResult
{
    Success,
    WrongPlace,
    InvalidRecipe,
    MissingIngredients,
    Full
};

class CookingManager
{
public:
    static CookResult Craft(
        Dish dish,
        bool atKitchen,
        Inventory& ingredients,
        FoodInventory& foods,
        Progression& progress
    );

    static bool Apply(Dish dish, Player& player);
};