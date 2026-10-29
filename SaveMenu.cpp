#include "SceneManager.h"
#include "Input.h"
#include "UIConfig.h"
#include "UIHelpers.h"
#include "DxLib.h"

void SceneManager::OpenSlots(SlotAction action)
{
    slotAction_ = action;

    saveMenuMessage_.clear();

    for (int i = 0; i < SaveManager::SlotCount; ++i)
    {
        slotInfo_[i] = session_.InspectSlot(i);
    }

    menu_ = Menu::Slots;
}

void SceneManager::SelectSlot(int slot)
{
    if (slotAction_ == SlotAction::NewGame)
    {
        if (session_.NewGame(slot))
        {
            title_ = false;
            menu_ = Menu::None;
        }
    }
    else if (slotAction_ == SlotAction::Load)
    {
        if (session_.LoadSlot(slot))
        {
            title_ = false;
            menu_ = Menu::None;
        }
        else
        {
            saveMenuMessage_ = SaveResultText(
                session_.LastSaveResult()
            );
        }
    }
    else
    {
        const SaveResult result =
            session_.SaveAtHome(slot);

        if (result == SaveResult::Success)
        {
            menu_ = Menu::House;
        }
        else
        {
            saveMenuMessage_ =
                SaveResultText(result);
        }
    }
}

bool SceneManager::UpdateSaveMenu(const Input& input)
{
    if (menu_ != Menu::Slots
        && menu_ != Menu::Overwrite)
    {
        return false;
    }

    if (input.IsPressed(KEY_INPUT_ESCAPE)
        || UI::Clicked(input, UIConfig::Button(5)))
    {
        if (menu_ == Menu::Overwrite)
        {
            menu_ = Menu::Slots;
        }
        else
        {
            menu_ =
                slotAction_ == SlotAction::Save
                ? Menu::House
                : Menu::None;
        }

        return true;
    }

    if (menu_ == Menu::Overwrite)
    {
        if (UI::Clicked(input, UIConfig::Button(0)))
        {
            SelectSlot(pendingSlot_);
        }
        else if (UI::Clicked(input, UIConfig::Button(1)))
        {
            menu_ = Menu::Slots;
        }

        return true;
    }

    for (int i = 0; i < SaveManager::SlotCount; ++i)
    {
        if (!UI::Clicked(input, UIConfig::Button(i)))
        {
            continue;
        }

        if (slotAction_ == SlotAction::Load)
        {
            if (slotInfo_[i].result == SaveResult::Success)
            {
                SelectSlot(i);
            }
            else
            {
                saveMenuMessage_ =
                    SaveResultText(slotInfo_[i].result);
            }
        }
        else if (slotInfo_[i].exists)
        {
            pendingSlot_ = i;
            menu_ = Menu::Overwrite;
        }
        else
        {
            SelectSlot(i);
        }

        break;
    }

    return true;
}

void SceneManager::DrawSaveMenu() const
{
    const unsigned int white =
        GetColor(255, 255, 255);

    if (menu_ == Menu::Overwrite)
    {
        DrawFormatString(
            520, 280,
            white,
            "スロット%dの既存データを上書きしますか？",
            pendingSlot_ + 1
        );

        UI::Button(UIConfig::Button(0), "はい");
        UI::Button(UIConfig::Button(1), "いいえ");

        if (slotAction_ == SlotAction::NewGame)
        {
            DrawString(
                520, 560,
                "新しく開始し、次のセーブ・帰還時に既存データを上書きします。",
                white
            );
        }

        return;
    }

    const char* title =
        slotAction_ == SlotAction::NewGame
        ? "はじめから：スロットを選択"
        : (
            slotAction_ == SlotAction::Load
            ? "つづきから：スロットを選択"
            : "セーブ先を選択"
            );

    DrawString(440, 285, title, white);

    for (int i = 0; i < SaveManager::SlotCount; ++i)
    {
        const Rect row = UIConfig::Button(i);

        UI::Button(row, "");

        DrawFormatString(
            540,
            static_cast<int>(row.y) + 10,
            white,
            "スロット%d",
            i + 1
        );

        const SaveSlotInfo& info = slotInfo_[i];

        if (info.result == SaveResult::Success)
        {
            DrawFormatString(
                740,
                static_cast<int>(row.y) + 10,
                white,
                "Lv.%d EXP %d / 草原 %s",
                info.level,
                info.exp,
                info.stageCleared
                ? "クリア済み"
                : "未クリア"
            );
        }
        else
        {
            DrawString(
                740,
                static_cast<int>(row.y) + 10,
                SaveResultText(info.result),
                white
            );
        }
    }

    DrawString(
        520, 610,
        "ロードすると町の自宅前から再開します。",
        white
    );

    DrawString(
        520, 650,
        saveMenuMessage_.c_str(),
        GetColor(255, 200, 80)
    );
}