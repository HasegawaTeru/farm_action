#pragma once

#include "SaveData.h"

#include <string>

enum class SaveResult
{
    Success,
    NotConfigured,
    InvalidSlot,
    Missing,
    Corrupt,
    UnsupportedVersion,
    IoError,
    InvalidData,
    WrongPlace
};

const char* SaveResultText(SaveResult result);

struct SaveSlotInfo
{
    SaveResult result = SaveResult::Missing;

    bool exists = false;

    int level = 1;
    int exp = 0;

    bool stageCleared = false;
};

class SaveManager
{
public:
    static const int SlotCount = 3;

    void Configure(const std::wstring& directory)
    {
        directory_ = directory;
    }

    bool Configured() const
    {
        return !directory_.empty();
    }

    SaveResult Read(
        int slot,
        SaveData& destination
    ) const;

    SaveResult Write(
        int slot,
        const SaveData& data
    ) const;

    SaveSlotInfo Inspect(int slot) const;

private:
    std::wstring FilePath(int slot) const;

    std::wstring directory_;
};