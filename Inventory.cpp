#include "Inventory.h"

#include <algorithm>

void Inventory::Clear()
{
    counts_.fill(0);
}

int Inventory::Count(Ingredient item) const
{
    const int index = static_cast<int>(item);

    if (index < 0
        || index >= ItemSettings::IngredientCount)
    {
        return 0;
    }

    return counts_[index];
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