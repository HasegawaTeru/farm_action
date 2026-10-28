#include "GameSession.h"
#include "CombatMotion.h"
#include "StageRules.h"

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

    // 初回の調理用。ボス専用食材は配布しない。
    for (int i = 0; i < 2; ++i)
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
    pendingExp_ = 0;
    arenaLocked_ = false;
    inResult_ = false;

    result_ = {};

    Message(
        "最初の料理用に食材を各2個用意しました。調理場へ行ってみましょう。"
    );
}

CookResult GameSession::Cook(Dish dish)
{
    const bool atKitchen =
        !inStage_
        && !inResult_
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
        || inResult_
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
            StageRules::NormalSpawnX[i],
            GameConfig::StageBaseLevel,
            progress_.Level()
        );
    }

    boss_.Init(
        GameConfig::StageBaseLevel,
        progress_.Level()
    );

    pendingExp_ = 0;
    arenaLocked_ = false;
    defeatedCount_ = 0;

    result_ = {};

    inStage_ = true;
    paused_ = false;

    Message("草原へ出発しました。");

    return true;
}

bool GameSession::FinishStage(StageEndReason reason)
{
    // 終了済みの挑戦から二重に報酬を取得させない。
    if (!inStage_)
    {
        return false;
    }

    if (reason == StageEndReason::Clear)
    {
        if (!arenaLocked_ || boss_.IsAlive())
        {
            return false;
        }

        StageResult completed;

        completed.ingredients = stageItems_.Pending();
        completed.exp = pendingExp_;
        completed.defeated = defeatedCount_;

        completed.oldLevel = progress_.Level();
        completed.oldExp = progress_.Exp();

        // 食材を確定できなければEXPも状態も変更しない。
        if (!stageItems_.CommitTo(ingredients_))
        {
            return false;
        }

        progress_.AddExp(pendingExp_);

        completed.newLevel = progress_.Level();
        completed.newExp = progress_.Exp();

        result_ = completed;
        inResult_ = true;
    }
    else
    {
        stageItems_.Discard();

        // 死亡時のみ、料理を開始前へ復元する。
        if (reason == StageEndReason::Defeat)
        {
            foods_ = beforeStageFoods_;
        }

        player_.Init(
            progress_.MaxHp(),
            progress_.AttackPower()
        );

        town_.Init(
            reason == StageEndReason::Defeat
        );

        Message(
            reason == StageEndReason::Defeat
            ? "悪い夢を見た……。自宅へ戻りました。"
            : "町へ帰りました。今回の食材とEXPは失われました。"
        );
    }

    carried_.Clear();

    pendingExp_ = 0;
    arenaLocked_ = false;
    inStage_ = false;
    paused_ = false;

    return true;
}

void GameSession::ConfirmResult()
{
    if (!inResult_)
    {
        return;
    }

    inResult_ = false;

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init(false);

    Message(
        "ステージクリア！ 食材とEXPを持ち帰りました。"
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
        || inResult_
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
    // メニュー中とリザルト中は通常更新を止める。
    if (paused_ || inResult_ || seconds <= 0.0f)
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

    // ボスエリアに入ったら入口を封鎖する。
    if (player_.X() >= StageRules::ArenaLeft)
    {
        arenaLocked_ = true;
    }

    if (arenaLocked_)
    {
        player_.ConstrainX(
            StageRules::ArenaLeft,
            StageRules::ArenaRight
        );
    }

    // 通常敵。
    for (int i = 0; i < GameConfig::SlimeCount; ++i)
    {
        Slime& slime = slimes_[i];

        slime.Update(seconds, player_);

        const bool alive = slime.IsAlive();

        slime.TryHit(player_);

        if (alive && !slime.IsAlive())
        {
            ++defeatedCount_;

            pendingExp_ +=
                2 + (slime.Level() - 1) / 2;

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

    // ボスはエリア封鎖後に行動を開始する。
    if (arenaLocked_)
    {
        boss_.Update(seconds, player_);

        const bool alive = boss_.IsAlive();

        boss_.TryHit(player_);

        if (alive && !boss_.IsAlive())
        {
            ++defeatedCount_;

            pendingExp_ += StageRules::BossExp;

            stageItems_.GrantReward(
                Ingredient::BossJelly,
                StageRules::BossIngredientCount
            );
        }

        if (boss_.IsAlive()
            && boss_.Intersects(player_.Bounds()))
        {
            const Rect bounds = boss_.Bounds();

            const int direction =
                player_.X() + GameConfig::PlayerWidth * 0.5f
                < bounds.x + bounds.width * 0.5f
                ? -1
                : 1;

            player_.TakeDamage(
                boss_.AttackPower(),
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

    if (arenaLocked_ && !boss_.IsAlive())
    {
        FinishStage(StageEndReason::Clear);
    }
}