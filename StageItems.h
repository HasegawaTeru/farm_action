#pragma once

#include "Inventory.h"

#include <array>
#include <random>
#include <set>
#include <vector>

// 挑戦中の食材・採取ポイントを管理する。
// 正式な持ち物は、確定処理を呼ぶまで変更しない。
class StageItems
{
public:
    explicit StageItems(
        unsigned int seed = std::random_device{}()
    );

    void Begin(const Inventory& owned);

    void Discard();

    // 将来のステージクリア処理から呼ぶ確定処理。
    bool CommitTo(Inventory& owned);

    int DropSlimeLoot(
        unsigned int enemyId,
        const Rect& enemyBounds
    );

    void Update(
        float seconds,
        const Rect& player,
        bool alive,
        bool gatherPressed
    );

    int NearbyPoint(const Rect& player) const;

    const Inventory& Pending() const
    {
        return pending_;
    }

    const std::vector<IngredientDrop>& Drops() const
    {
        return drops_;
    }

    const std::array<
        GatherPoint,
        ItemSettings::GatherPointCount
    >& Points() const
    {
        return points_;
    }

    bool HasNotice() const
    {
        return noticeRemaining_ > 0.0f;
    }

    Ingredient NoticeItem() const
    {
        return noticeItem_;
    }

    int NoticeQuantity() const
    {
        return noticeQuantity_;
    }

    int GrantReward(Ingredient item, int quantity);

private:
    int Collect(Ingredient item, int quantity);

    void SetNotice(Ingredient item, int quantity);

    // ステージ開始時点の正式な持ち物。
    Inventory carried_;

    // 今回のステージで取得した一時データ。
    Inventory pending_;

    std::vector<IngredientDrop> drops_;

    std::array<
        GatherPoint,
        ItemSettings::GatherPointCount
    > points_{};

    // ドロップを抽選済みの敵ID。
    std::set<unsigned int> droppedEnemies_;

    std::mt19937 random_;

    bool active_ = false;

    float noticeRemaining_ = 0.0f;

    Ingredient noticeItem_ = Ingredient::SlimeJelly;
    int noticeQuantity_ = 0;
};