#include "SceneManager.h"
#include "Input.h"
#include "UIConfig.h"
#include "UIHelpers.h"
#include "DxLib.h"

void SceneManager::UpdateMenu(const Input& input)
{
    if (UpdateSaveMenu(input))
    {
        return;
    }

    if (input.IsPressed(KEY_INPUT_ESCAPE)
        || UI::Clicked(input, UIConfig::Button(5)))
    {
        if (menu_ == Menu::Inventory
            || (menu_ == Menu::Settings && !title_))
        {
            menu_ = Menu::Pause;
        }
        else
        {
            menu_ = Menu::None;
        }

        return;
    }

    switch (menu_)
    {
    case Menu::Pause:
    {
        if (UI::Clicked(input, UIConfig::Button(0)))
        {
            menu_ = Menu::Settings;
        }
        else if (UI::Clicked(input, UIConfig::Button(1)))
        {
            foodCategory_ = false;
            selectedDish_ = Dish::Jelly;

            menu_ = Menu::Inventory;
        }
        else if (UI::Clicked(input, UIConfig::Button(2)))
        {
            if (session_.InStage())
            {
                session_.LeaveStage(false);
            }
            else
            {
                title_ = true;
            }

            menu_ = Menu::None;
        }

        break;
    }

    case Menu::Inventory:
    {
        if (UI::Clicked(input, UIConfig::Tab(0)))
        {
            foodCategory_ = false;
        }

        if (UI::Clicked(input, UIConfig::Tab(1)))
        {
            foodCategory_ = true;
        }

        if (foodCategory_)
        {
            for (int i = 0; i < CookingSettings::DishCount; ++i)
            {
                if (UI::Clicked(input, UIConfig::FoodRow(i)))
                {
                    selectedDish_ = static_cast<Dish>(i);
                }
            }

            if (input.IsPressed(KEY_INPUT_F)
                && session_.Eat(selectedDish_))
            {
                menu_ = Menu::None;
            }
        }

        break;
    }

    case Menu::Kitchen:
    {
        for (int i = 0; i < CookingSettings::DishCount; ++i)
        {
            if (UI::Clicked(input, UIConfig::Button(i)))
            {
                session_.Cook(static_cast<Dish>(i));
            }
        }

        break;
    }

    case Menu::Board:
    {
        for (int i = 0; i < CookingSettings::DishCount; ++i)
        {
            const Dish dish = static_cast<Dish>(i);

            if (UI::Clicked(input, UIConfig::Minus(i)))
            {
                selection_.Remove(dish, 1);
            }

            if (UI::Clicked(input, UIConfig::Plus(i))
                && selection_.Total() < CookingSettings::CarryLimit
                && selection_.Count(dish)
                < session_.Foods().Count(dish))
            {
                selection_.Add(dish, 1);
            }
        }

        if (UI::Clicked(input, UIConfig::Button(4))
            && session_.StartStage(selection_))
        {
            menu_ = Menu::None;
        }

        break;
    }

    case Menu::House:
    {
        if (UI::Clicked(input, UIConfig::Button(0)))
        {
            session_.Rest();
        }
        else if (UI::Clicked(input, UIConfig::Button(1)))
        {
            OpenSlots(SlotAction::Save);
        }

        break;
    }

    default:
        break;
    }
}