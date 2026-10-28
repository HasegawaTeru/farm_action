#include "SceneManager.h"
#include "Input.h"
#include "UIConfig.h"
#include "DxLib.h"

namespace
{
    bool Clicked(
        const Input& input,
        const Rect& rect)
    {
        return input.IsLeftClickPressed()
            && input.MouseX() >= rect.x
            && input.MouseX() < rect.x + rect.width
            && input.MouseY() >= rect.y
            && input.MouseY() < rect.y + rect.height;
    }
}

void SceneManager::Init()
{
    resultTime_ = 0.0f;

    title_ = true;
    quit_ = false;

    menu_ = Menu::None;

    selection_.Clear();

    selectedDish_ = Dish::Jelly;
    foodCategory_ = false;

    showAttackBounds_ = true;
}

void SceneManager::Update(
    const Input& input,
    float seconds)
{
    if (title_)
    {
        if (menu_ != Menu::None)
        {
            UpdateMenu(input);
            return;
        }

        if (input.IsPressed(KEY_INPUT_RETURN)
            || Clicked(input, UIConfig::Button(0)))
        {
            session_.NewGame();
            title_ = false;
        }
        else if (Clicked(input, UIConfig::Button(1)))
        {
            menu_ = Menu::Settings;
        }
        else if (Clicked(input, UIConfig::Button(2)))
        {
            quit_ = true;
        }

        return;
    }

    // リザルト中は閉じる操作と表示演出だけ更新する。
    if (session_.InResult())
    {
        const float elapsed =
            seconds < 0.0f
            ? 0.0f
            : (
                seconds > GameConfig::MaxFrameSeconds
                ? GameConfig::MaxFrameSeconds
                : seconds
                );

        resultTime_ += elapsed;

        if (input.IsPressed(KEY_INPUT_RETURN)
            || input.IsPressed(KEY_INPUT_ESCAPE)
            || Clicked(input, UIConfig::Button(5)))
        {
            session_.ConfirmResult();
        }

        return;
    }

    if (input.IsPressed(KEY_INPUT_ESCAPE)
        && menu_ == Menu::None)
    {
        menu_ = Menu::Pause;

        session_.SetPaused(true);
        return;
    }

    if (menu_ != Menu::None)
    {
        session_.SetPaused(true);
        UpdateMenu(input);
        return;
    }

    session_.SetPaused(false);

    if (input.IsPressed(KEY_INPUT_F1))
    {
        showAttackBounds_ = !showAttackBounds_;
    }

    const int horizontal =
        (input.IsDown(KEY_INPUT_D) ? 1 : 0)
        - (input.IsDown(KEY_INPUT_A) ? 1 : 0);

    session_.Update(
        seconds,
        horizontal,
        input.IsPressed(KEY_INPUT_SPACE),
        input.IsLeftClickPressed(),
        input.IsPressed(KEY_INPUT_E)
    );

    // クリアしたフレームでは施設操作などを行わない。
    if (session_.InResult())
    {
        resultTime_ = 0.0f;
        menu_ = Menu::None;
        return;
    }

    if (!session_.InStage()
        && input.IsPressed(KEY_INPUT_E))
    {
        switch (session_.TownState().NearbyFacility())
        {
        case Town::Facility::House:
            menu_ = Menu::House;
            break;

        case Town::Facility::Kitchen:
            menu_ = Menu::Kitchen;
            break;

        case Town::Facility::Board:
            selection_.Clear();
            menu_ = Menu::Board;
            break;

        default:
            break;
        }

        session_.SetPaused(menu_ != Menu::None);
    }
}

void SceneManager::UpdateMenu(const Input& input)
{
    // ESCまたは「戻る」で前の画面へ戻る。
    if (input.IsPressed(KEY_INPUT_ESCAPE)
        || Clicked(input, UIConfig::Button(5)))
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
        if (Clicked(input, UIConfig::Button(0)))
        {
            menu_ = Menu::Settings;
        }
        else if (Clicked(input, UIConfig::Button(1)))
        {
            foodCategory_ = false;
            selectedDish_ = Dish::Jelly;

            menu_ = Menu::Inventory;
        }
        else if (Clicked(input, UIConfig::Button(2)))
        {
            if (session_.InStage())
            {
                // リタイア。食べた料理の消費は維持する。
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
        if (Clicked(input, UIConfig::Tab(0)))
        {
            foodCategory_ = false;
        }

        if (Clicked(input, UIConfig::Tab(1)))
        {
            foodCategory_ = true;
        }

        if (foodCategory_)
        {
            for (int i = 0; i < CookingSettings::DishCount; ++i)
            {
                if (Clicked(input, UIConfig::FoodRow(i)))
                {
                    selectedDish_ = static_cast<Dish>(i);
                }
            }

            if (input.IsPressed(KEY_INPUT_F)
                && session_.Eat(selectedDish_))
            {
                // 使用に成功した場合だけメニューを閉じる。
                menu_ = Menu::None;
            }
        }

        break;
    }

    case Menu::Kitchen:
    {
        for (int i = 0; i < CookingSettings::DishCount; ++i)
        {
            if (Clicked(input, UIConfig::Button(i)))
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

            if (Clicked(input, UIConfig::Minus(i)))
            {
                selection_.Remove(dish, 1);
            }

            if (Clicked(input, UIConfig::Plus(i))
                && selection_.Total() < CookingSettings::CarryLimit
                && selection_.Count(dish)
                < session_.Foods().Count(dish))
            {
                selection_.Add(dish, 1);
            }
        }

        if (Clicked(input, UIConfig::Button(4))
            && session_.StartStage(selection_))
        {
            menu_ = Menu::None;
        }

        break;
    }

    case Menu::House:
    {
        if (Clicked(input, UIConfig::Button(0)))
        {
            session_.Rest();
        }

        break;
    }

    default:
        break;
    }
}

void SceneManager::Draw() const
{
    if (title_)
    {
        DrawTitle();
    }
    else if (session_.InResult())
    {
        DrawResult();
    }
    else
    {
        if (session_.InStage())
        {
            DrawStage();
        }
        else
        {
            DrawTown();
        }

        DrawHud();

        DrawString(
            40,
            380,
            session_.Message(),
            GetColor(255, 230, 80)
        );
    }

    if (menu_ != Menu::None)
    {
        DrawMenu();
    }
}