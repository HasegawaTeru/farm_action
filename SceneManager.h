#pragma once

#include "GameSession.h"
#include "DebugView.h"

class Input;

class SceneManager
{
public:
    void Init(
        const std::wstring& saveDirectory = L""
    );

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
        House,
        Slots,
        Overwrite
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

    enum class SlotAction
    {
        NewGame,
        Load,
        Save
    };

    void OpenSlots(SlotAction action);
    void SelectSlot(int slot);
    bool UpdateSaveMenu(const Input& input);
    void DrawSaveMenu() const;

    SlotAction slotAction_ = SlotAction::NewGame;

    int pendingSlot_ = 0;

    std::array<
        SaveSlotInfo,
        SaveManager::SlotCount
    > slotInfo_{};

    std::string saveMenuMessage_;
};