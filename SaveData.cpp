#include "SaveData.h"

bool SaveData::Valid() const
{
    Progression progress;

    if (!progress.Restore(level, exp))
    {
        return false;
    }

    for (int count : ingredients)
    {
        if (count < 0
            || count > ItemSettings::IngredientLimit)
        {
            return false;
        }
    }

    for (int count : foods)
    {
        if (count < 0
            || count > CookingSettings::DishLimit)
        {
            return false;
        }
    }

    return true;
}