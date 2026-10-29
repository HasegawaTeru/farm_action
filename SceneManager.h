#pragma once

#include "GameSession.h"
#include "DebugView.h"

class Input;

class SceneManager
{
public:
    void Init();

    void Update(
        const Input& input,
        float seconds
    );

    void Draw() const;

    bool DebugVisible() const
    {
        return debug_.Enabled();
    }

    bool ShouldQuit() const
    {
        return quit_;
    }

    const GameSession& State() const
    {
        return session_;
    }

    bool IsMenuOpen() const
    {
        return menu_ != Menu::None;
    }

private:
    enum class Menu
    {
        None,
        Pause,
        Settings,
        Inventory,
        Kitchen,
        Board,
        House
    };

    void UpdateMenu(const Input& input);

    void DrawTitle() const;
    void DrawResult() const;
    void DrawBoss() const;
    void DrawTown() const;
    void DrawStage() const;
    void DrawHud() const;
    void DrawMenu() const;

    GameSession session_;
    DebugView debug_;

    Menu menu_ = Menu::None;

    FoodInventory selection_;
    Dish selectedDish_ = Dish::Jelly;

    bool foodCategory_ = false;

    float resultTime_ = 0.0f;

    bool title_ = true;
    bool quit_ = false;
    bool showAttackBounds_ = true;
};