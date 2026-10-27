#include "GameSession.h"
#include "CombatMotion.h"

#include <algorithm>

void GameSession::Message(const std::string& text)
{
    message_ = text;
    messageRemaining_ = 4.0f;
}

void GameSession::NewGame()
{
    progress_.Init();

    ingredients_.Clear();
    foods_.Clear();
    carried_.Clear();
    beforeStageFoods_.Clear();

    stageItems_.Discard();

    // 正式なクリア実装前でも、最初の料理を試せるようにする。
    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        ingredients_.Add(
            static_cast<Ingredient>(i),
            CookingSettings::StarterIngredients
        );
    }

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init();

    inStage_ = false;
    paused_ = false;
    defeatedCount_ = 0;

    Message(
        "最初の料理用に食材を各2個用意しました。調理場へ行ってみましょう。"
    );
}

CookResult GameSession::Cook(Dish dish)
{
    const bool atKitchen =
        !inStage_
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
            + " が完成！ EXP +2"
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

bool GameSession::StartStage(
    const FoodInventory& selection)
{
    if (inStage_
        || town_.NearbyFacility() != Town::Facility::Board
        || selection.Total() > CookingSettings::CarryLimit)
    {
        return false;
    }

    for (int i = 0; i < CookingSettings::DishCount; ++i)
    {
        const Dish dish = static_cast<Dish>(i);

        if (selection.Count(dish) > foods_.Count(dish))
        {
            return false;
        }
    }

    carried_ = selection;
    beforeStageFoods_ = foods_;

    stageItems_.Begin(ingredients_);

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        slimes_[i].Init(
            GameConfig::SlimeSpawnX[i],
            GameConfig::StageBaseLevel,
            progress_.Level()
        );
    }

    defeatedCount_ = 0;

    inStage_ = true;
    paused_ = false;

    Message("草原へ出発しました。");

    return true;
}

void GameSession::LeaveStage(bool defeated)
{
    if (!inStage_)
    {
        return;
    }

    stageItems_.Discard();

    // 死亡では料理を復元する。
    // リタイアでは食べた分の消費を維持する。
    if (defeated)
    {
        foods_ = beforeStageFoods_;
    }

    carried_.Clear();

    inStage_ = false;
    paused_ = false;

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init(defeated);

    Message(
        defeated
        ? "悪い夢を見た……。自宅へ戻りました。"
        : "町へ帰りました。今回の食材は失われました。"
    );
}

bool GameSession::Eat(Dish dish)
{
    if (!inStage_
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
    if (inStage_
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
    // メニュー中は全ゲーム内時間を止める。
    if (paused_ || seconds <= 0.0f)
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

    if (!inStage_)
    {
        town_.Update(seconds, horizontal);
        return;
    }

    player_.Update(
        seconds,
        horizontal,
        jump,
        attack
    );

    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        Slime& slime = slimes_[i];

        slime.Update(seconds, player_);

        const bool alive = slime.IsAlive();

        slime.TryHit(player_);

        if (alive && !slime.IsAlive())
        {
            ++defeatedCount_;

            stageItems_.DropSlimeLoot(
                static_cast<unsigned int>(i),
                slime.Bounds()
            );
        }

        if (slime.IsAlive()
            && slime.Intersects(player_.Bounds()))
        {
            const Rect bounds = slime.Bounds();

            const int direction =
                player_.X() + GameConfig::PlayerWidth * 0.5f
                < bounds.x + bounds.width * 0.5f
                ? -1
                : 1;

            player_.TakeDamage(
                slime.AttackPower(),
                direction
            );
        }
    }

    if (!player_.IsAlive())
    {
        LeaveStage(true);
        return;
    }

    stageItems_.Update(
        seconds,
        player_.Bounds(),
        true,
        gather
    );
}