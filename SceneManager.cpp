#include "SceneManager.h"
#include "Input.h"
#include "UIConfig.h"
#include "UIHelpers.h"
#include "DxLib.h"

//Å@èâä˙ê›íË
void SceneManager::Init(
    const std::wstring& saveDirectory)
{
    session_.ConfigureSaveDirectory(saveDirectory);
    saveMenuMessage_.clear();

    debug_.Init();

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
    debug_.Update(
        seconds,
        input.IsPressed(KEY_INPUT_F2)
    );

    if (title_)
    {
        if (menu_ != Menu::None)
        {
            UpdateMenu(input);
            return;
        }

        if (input.IsPressed(KEY_INPUT_RETURN)
            || UI::Clicked(input, UIConfig::Button(0)))
        {
            OpenSlots(SlotAction::NewGame);
        }
        else if (UI::Clicked(input, UIConfig::Button(1)))
        {
            OpenSlots(SlotAction::Load);
        }
        else if (UI::Clicked(input, UIConfig::Button(2)))
        {
            menu_ = Menu::Settings;
        }
        else if (UI::Clicked(input, UIConfig::Button(3)))
        {
            quit_ = true;
        }

        return;
    }

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
            || UI::Clicked(input, UIConfig::Button(5)))
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

    if (session_.InResult())
    {
        resultTime_ = 0;
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

    debug_.Draw(
        session_,
        menu_ != Menu::None,
        title_
    );
}