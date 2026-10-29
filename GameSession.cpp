#include "GameSession.h"
#include "CombatMotion.h"

#include <algorithm>

void GameSession::Message(const std::string& text)
{
    message_ = text;
    messageRemaining_ = 4.0f;
}

bool GameSession::NewGame(int slot)
{
    if (slot < 0 || slot >= SaveManager::SlotCount)
    {
        return false;
    }

    activeSlot_ = slot;
    stageCleared_ = false;

    progress_.Init();

    ingredients_.Clear();
    foods_.Clear();
    carried_.Clear();
    beforeStageFoods_.Clear();

    stageItems_.Discard();

    ingredients_.Add(
        Ingredient::SlimeJelly,
        CookingSettings::StarterIngredients
    );

    ingredients_.Add(
        Ingredient::Herb,
        CookingSettings::StarterIngredients
    );

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init();

    scene_ = Scene::Town;
    paused_ = false;

    defeatedCount_ = 0;
    pendingExp_ = 0;
    arenaLocked_ = false;

    result_ = {};

    Message(
        "最初の料理用に食材を各2個用意しました。調理場へ行ってみましょう。"
    );

    return true;
}

CookResult GameSession::Cook(Dish dish)
{
    const bool atKitchen =
        !InStage()
        && !InResult()
        && town_.NearbyFacility() == Town::Facility::Kitchen;

    const CookResult result = CookingManager::Craft(
        dish,
        atKitchen,
        ingredients_,
        foods_,
        progress_
    );

    switch (result)
    {
    case CookResult::Success:
        player_.Init(
            progress_.MaxHp(),
            progress_.AttackPower()
        );

        Message(
            std::string(FindRecipe(dish)->name)
            + " が完成！ EXP +"
            + std::to_string(FindRecipe(dish)->rarity * 2)
        );
        break;

    case CookResult::MissingIngredients:
        Message("食材が足りません。");
        break;

    case CookResult::Full:
        Message("この料理は99個までです。");
        break;

    default:
        Message("料理は町の調理場で作れます。");
        break;
    }

    return result;
}

bool GameSession::Eat(Dish dish)
{
    if (!InStage()
        || carried_.Count(dish) <= 0
        || foods_.Count(dish) <= 0)
    {
        return false;
    }

    if (!CookingManager::Apply(dish, player_))
    {
        Message(
            "今はこの料理を食べられません。回復料理はHPが減っているときに使えます。"
        );

        return false;
    }

    carried_.Remove(dish, 1);
    foods_.Remove(dish, 1);

    Message(
        std::string(FindRecipe(dish)->name)
        + " を食べました。"
    );

    return true;
}

void GameSession::Rest()
{
    if (InStage()
        || InResult()
        || town_.NearbyFacility() != Town::Facility::House)
    {
        return;
    }

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    Message("自宅で休みました。");
}

void GameSession::Update(
    float seconds,
    int horizontal,
    bool jump,
    bool attack,
    bool gather)
{
    if (paused_ || InResult() || seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    messageRemaining_ = AdvanceTimer(
        messageRemaining_,
        seconds
    );

    if (!InStage())
    {
        town_.Update(seconds, horizontal);
        return;
    }

    UpdateStage(
        seconds,
        horizontal,
        jump,
        attack,
        gather
    );
}