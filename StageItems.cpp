#include "StageItems.h"
#include "GameConfig.h"
#include "CombatMotion.h"

#include <algorithm>

StageItems::StageItems(unsigned int seed)
    : random_(seed)
{
}

void StageItems::Begin(const Inventory& owned)
{
    Discard();

    carried_ = owned;
    active_ = true;

    for (int i = 0; i < ItemSettings::GatherPointCount; ++i)
    {
        points_[i] =
        {
            {
                ItemSettings::GatherX[i],
                GameConfig::GroundY - ItemSettings::GatherHeight,
                ItemSettings::GatherWidth,
                ItemSettings::GatherHeight
            },
            false
        };
    }
}

void StageItems::Discard()
{
    pending_.Clear();
    carried_.Clear();

    drops_.clear();
    droppedEnemies_.clear();

    active_ = false;

    noticeRemaining_ = 0.0f;
    noticeQuantity_ = 0;
}

bool StageItems::CommitTo(Inventory& owned)
{
    if (!active_)
    {
        return false;
    }

    // 先に全種類の空き容量を確認する。
    // 途中の種類だけ確定されることを防ぐ。
    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        const Ingredient item =
            static_cast<Ingredient>(i);

        const int room =
            ItemSettings::IngredientLimit - owned.Count(item);

        if (pending_.Count(item) > room)
        {
            return false;
        }
    }

    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        const Ingredient item =
            static_cast<Ingredient>(i);

        owned.Add(
            item,
            pending_.Count(item)
        );
    }

    // 一時データを消し、二重確定を防ぐ。
    Discard();

    return true;
}

int StageItems::DropSlimeLoot(
    unsigned int enemyId,
    const Rect& enemyBounds)
{
    // 0個だった敵も記録する。
    // 同じ敵を繰り返し抽選することを防ぐ。
    if (!active_
        || !droppedEnemies_.insert(enemyId).second)
    {
        return 0;
    }

    std::uniform_int_distribution<int> distribution(
        0,
        ItemSettings::MaxSlimeDrops
    );

    const int count = distribution(random_);

    for (int i = 0; i < count; ++i)
    {
        const float x =
            enemyBounds.x
            + enemyBounds.width * 0.5f
            - ItemSettings::DropSize * 0.5f
            + (i - (count - 1) * 0.5f)
            * ItemSettings::DropSpacing;

        // 空中で倒した場合も、その下の地面へ配置する。
        const Rect bounds =
        {
            std::max(
                0.0f,
                std::min(
                    x,
                    GameConfig::WindowWidth
                        - ItemSettings::DropSize
                )
            ),
            GameConfig::GroundY - ItemSettings::DropSize,
            ItemSettings::DropSize,
            ItemSettings::DropSize
        };

        drops_.push_back(
            {
                bounds,
                Ingredient::SlimeJelly,
                1
            }
        );
    }

    return count;
}

int StageItems::NearbyPoint(const Rect& player) const
{
    if (!active_)
    {
        return -1;
    }

    int nearest = -1;

    float best =
        ItemSettings::GatherRange
        * ItemSettings::GatherRange;

    for (int i = 0; i < ItemSettings::GatherPointCount; ++i)
    {
        if (points_[i].depleted)
        {
            continue;
        }

        const Rect& point = points_[i].bounds;

        const float dx =
            player.x + player.width * 0.5f
            - (point.x + point.width * 0.5f);

        const float dy =
            player.y + player.height * 0.5f
            - (point.y + point.height * 0.5f);

        const float distance =
            dx * dx + dy * dy;

        if (distance <= best)
        {
            best = distance;
            nearest = i;
        }
    }

    return nearest;
}

int StageItems::Collect(
    Ingredient item,
    int quantity)
{
    // 持ち込み分と今回取得分の合計で999個まで。
    const int room =
        ItemSettings::IngredientLimit
        - carried_.Count(item)
        - pending_.Count(item);

    return pending_.Add(
        item,
        std::min(quantity, room)
    );
}

void StageItems::SetNotice(
    Ingredient item,
    int quantity)
{
    noticeItem_ = item;
    noticeQuantity_ = quantity;

    noticeRemaining_ =
        ItemSettings::NoticeDuration;
}

void StageItems::Update(
    float seconds,
    const Rect& player,
    bool alive,
    bool gatherPressed)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    noticeRemaining_ = AdvanceTimer(
        noticeRemaining_,
        seconds
    );

    if (!active_ || !alive)
    {
        return;
    }

    std::array<
        int,
        ItemSettings::IngredientCount
    > collected{};

    // 落ちている食材に触れたら取得する。
    for (auto it = drops_.begin(); it != drops_.end();)
    {
        if (Overlaps(player, it->bounds))
        {
            const int added = Collect(
                it->item,
                it->quantity
            );

            it->quantity -= added;

            collected[
                static_cast<int>(it->item)
            ] += added;

            if (added == 0)
            {
                SetNotice(it->item, 0);
            }
        }

        // 上限で取れなかった食材は地面に残す。
        if (it->quantity == 0)
        {
            it = drops_.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // 同じフレームに拾った個数をまとめて通知する。
    for (int i = 0; i < ItemSettings::IngredientCount; ++i)
    {
        if (collected[i] > 0)
        {
            SetNotice(
                static_cast<Ingredient>(i),
                collected[i]
            );
        }
    }

    // 近くの採取ポイントでEを押した場合。
    const int point = NearbyPoint(player);

    if (gatherPressed && point >= 0)
    {
        const int added = Collect(
            Ingredient::Herb,
            ItemSettings::HerbsPerGather
        );

        // 上限で取れなかった場合はポイントを消費しない。
        if (added > 0)
        {
            points_[point].depleted = true;
        }

        SetNotice(
            Ingredient::Herb,
            added
        );
    }
}