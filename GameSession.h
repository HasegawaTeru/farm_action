#pragma once

#include "Player.h"
#include "Slime.h"
#include "StageItems.h"
#include "CookingManager.h"
#include "Town.h"
#include "GameConfig.h"

#include <array>
#include <string>

// 持ち物・進行状態・プレイ中の状態を管理する。
// 入力APIと描画APIには依存しない。
class GameSession
{
public:
    void NewGame();

    void SetPaused(bool paused)
    {
        paused_ = paused;
    }

    void Update(
        float seconds,
        int horizontal,
        bool jump,
        bool attack,
        bool gather
    );

    CookResult Cook(Dish dish);

    bool StartStage(const FoodInventory& selection);
    void LeaveStage(bool defeated);

    bool Eat(Dish dish);
    void Rest();

    bool InStage() const
    {
        return inStage_;
    }

    const Player& Actor() const
    {
        return player_;
    }

    const Town& TownState() const
    {
        return town_;
    }

    const Progression& Progress() const
    {
        return progress_;
    }

    const Inventory& Ingredients() const
    {
        return ingredients_;
    }

    const FoodInventory& Foods() const
    {
        return foods_;
    }

    const FoodInventory& Carried() const
    {
        return carried_;
    }

    const StageItems& Items() const
    {
        return stageItems_;
    }

    const std::array<
        Slime,
        GameConfig::SlimeCount
    >& Slimes() const
    {
        return slimes_;
    }

    int DefeatedCount() const
    {
        return defeatedCount_;
    }

    const char* Message() const
    {
        return messageRemaining_ > 0.0f
            ? message_.c_str()
            : "";
    }

private:
    void Message(const std::string& text);

    Player player_;
    Town town_;
    Progression progress_;

    Inventory ingredients_;

    FoodInventory foods_;
    FoodInventory carried_;
    FoodInventory beforeStageFoods_;

    StageItems stageItems_;

    std::array<
        Slime,
        GameConfig::SlimeCount
    > slimes_;

    int defeatedCount_ = 0;

    bool inStage_ = false;
    bool paused_ = false;

    std::string message_;
    float messageRemaining_ = 0.0f;
};