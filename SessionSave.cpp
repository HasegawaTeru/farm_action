#include "GameSession.h"

SaveData GameSession::CaptureSave() const
{
    SaveData data;

    data.level = progress_.Level();
    data.exp = progress_.Exp();
    data.stageCleared = stageCleared_;

    for (int i = 0;
        i < ItemSettings::IngredientCount;
        ++i)
    {
        data.ingredients[i] = ingredients_.Count(
            static_cast<Ingredient>(i)
        );
    }

    for (int i = 0;
        i < CookingSettings::DishCount;
        ++i)
    {
        data.foods[i] = foods_.Count(
            static_cast<Dish>(i)
        );
    }

    return data;
}

SaveResult GameSession::SaveAtHome(int slot)
{
    if (CurrentScene() != Scene::Town
        || town_.NearbyFacility()
        != Town::Facility::House)
    {
        lastSave_ = SaveResult::WrongPlace;
    }
    else
    {
        lastSave_ = saves_.Write(
            slot,
            CaptureSave()
        );
    }

    if (lastSave_ == SaveResult::Success)
    {
        activeSlot_ = slot;

        Message(
            "スロット"
            + std::to_string(slot + 1)
            + "にセーブしました。"
        );
    }
    else
    {
        Message(SaveResultText(lastSave_));
    }

    return lastSave_;
}

void GameSession::AutoSave()
{
    if (!saves_.Configured())
    {
        return;
    }

    lastSave_ = saves_.Write(
        activeSlot_,
        CaptureSave()
    );

    if (lastSave_ == SaveResult::Success)
    {
        Message(
            message_ + "（オートセーブ済み）"
        );
    }
    else
    {
        Message(
            std::string("オートセーブに失敗しました。")
            + SaveResultText(lastSave_)
        );
    }
}

bool GameSession::LoadSlot(int slot)
{
    if (CurrentScene() != Scene::Town)
    {
        lastSave_ = SaveResult::WrongPlace;
        return false;
    }

    SaveData data;

    lastSave_ = saves_.Read(slot, data);

    if (lastSave_ != SaveResult::Success)
    {
        return false;
    }

    // 全項目が検証された後で正式データへ反映する。
    progress_.Restore(data.level, data.exp);

    ingredients_.Clear();
    foods_.Clear();
    carried_.Clear();
    beforeStageFoods_.Clear();

    for (int i = 0;
        i < ItemSettings::IngredientCount;
        ++i)
    {
        ingredients_.Add(
            static_cast<Ingredient>(i),
            data.ingredients[i]
        );
    }

    for (int i = 0;
        i < CookingSettings::DishCount;
        ++i)
    {
        foods_.Add(
            static_cast<Dish>(i),
            data.foods[i]
        );
    }

    stageCleared_ = data.stageCleared;
    activeSlot_ = slot;

    stageItems_.Discard();

    result_ = {};

    defeatedCount_ = 0;
    pendingExp_ = 0;
    arenaLocked_ = false;
    paused_ = false;

    scene_ = Scene::Town;

    player_.Init(
        progress_.MaxHp(),
        progress_.AttackPower()
    );

    town_.Init();

    Message(
        "スロット"
        + std::to_string(slot + 1)
        + "をロードしました。"
    );

    return true;
}