#include "GameSession.h"
#include "StageRules.h"

// ステージ開始。
bool GameSession::StartStage(
    const FoodInventory& selection)
{
    if (InStage()
        || InResult()
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

    scene_ = Scene::Stage;
    paused_ = false;

    Message("草原へ出発しました。");

    return true;
}

// 終了理由に応じて確定・破棄・復元を行う。
bool GameSession::FinishStage(StageEndReason reason)
{
    if (!InStage())
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

        if (!stageItems_.CommitTo(ingredients_))
        {
            return false;
        }

        progress_.AddExp(pendingExp_);

        completed.newLevel = progress_.Level();
        completed.newExp = progress_.Exp();

        result_ = completed;
        scene_ = Scene::Result;
    }
    else
    {
        scene_ = Scene::Town;

        stageItems_.Discard();

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
    paused_ = false;

    return true;
}

// リザルトを閉じて町へ戻る。
void GameSession::ConfirmResult()
{
    if (!InResult())
    {
        return;
    }

    scene_ = Scene::Town;

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init(false);

    Message(
        "ステージクリア！ 食材とEXPを持ち帰りました。"
    );
}

// 通常敵・ボスに共通の命中と接触ダメージ。
bool GameSession::ResolveCombat(Enemy& enemy)
{
    const bool alive = enemy.IsAlive();

    enemy.TryHit(player_);

    const bool defeated =
        alive && !enemy.IsAlive();

    if (enemy.IsAlive()
        && enemy.Intersects(player_.Bounds()))
    {
        const Rect bounds = enemy.Bounds();

        const int direction =
            player_.X() + GameConfig::PlayerWidth * 0.5f
            < bounds.x + bounds.width * 0.5f
            ? -1
            : 1;

        player_.TakeDamage(
            enemy.AttackPower(),
            direction
        );
    }

    return defeated;
}

// ステージ中の更新。
void GameSession::UpdateStage(
    float seconds,
    int horizontal,
    bool jump,
    bool attack,
    bool gather)
{
    player_.Update(
        seconds,
        horizontal,
        jump,
        attack
    );

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

        if (ResolveCombat(slime))
        {
            ++defeatedCount_;

            pendingExp_ +=
                2 + (slime.Level() - 1) / 2;

            stageItems_.DropSlimeLoot(
                static_cast<unsigned int>(i),
                slime.Bounds()
            );
        }
    }

    // ボス。
    if (arenaLocked_)
    {
        boss_.Update(seconds, player_);

        if (ResolveCombat(boss_))
        {
            ++defeatedCount_;

            pendingExp_ += StageRules::BossExp;

            stageItems_.GrantReward(
                Ingredient::BossJelly,
                StageRules::BossIngredientCount
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