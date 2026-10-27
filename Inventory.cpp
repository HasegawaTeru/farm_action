#include "Inventory.h"

#include <algorithm>

void Inventory::Clear()
{
    counts_.fill(0);
}

int Inventory::Count(Ingredient item) const
{
    const int index = static_cast<int>(item);

    return index >= 0
        && index < ItemSettings::IngredientCount
        ? counts_[index]
        : 0;
}

int Inventory::Add(Ingredient item, int quantity)
{
    const int index = static_cast<int>(item);

    if (index < 0
        || index >= ItemSettings::IngredientCount
        || quantity <= 0)
    {
        return 0;
    }

    const int added = std::min(
        quantity,
        ItemSettings::IngredientLimit - counts_[index]
    );

    counts_[index] += added;

    return added;
}

bool Inventory::Remove(Ingredient item, int quantity)
{
    const int index = static_cast<int>(item);

    if (index < 0
        || index >= ItemSettings::IngredientCount
        || quantity <= 0
        || counts_[index] < quantity)
    {
        return false;
    }

    counts_[index] -= quantity;

    return true;
}

void FoodInventory::Clear()
{
    counts_.fill(0);
}

int FoodInventory::Count(Dish dish) const
{
    const int index = static_cast<int>(dish);

    return index >= 0
        && index < CookingSettings::DishCount
        ? counts_[index]
        : 0;
}

int FoodInventory::Total() const
{
    int total = 0;

    for (int count : counts_)
    {
        total += count;
    }

    return total;
}

int FoodInventory::Add(Dish dish, int quantity)
{
    const int index = static_cast<int>(dish);

    if (index < 0
        || index >= CookingSettings::DishCount
        || quantity <= 0)
    {
        return 0;
    }

    const int added = std::min(
        quantity,
        CookingSettings::DishLimit - counts_[index]
    );

    counts_[index] += added;

    return added;
}

bool FoodInventory::Remove(Dish dish, int quantity)
{
    const int index = static_cast<int>(dish);

    if (index < 0
        || index >= CookingSettings::DishCount
        || quantity <= 0
        || counts_[index] < quantity)
    {
        return false;
    }

    counts_[index] -= quantity;

    return true;
}