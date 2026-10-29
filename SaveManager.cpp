#include "SaveManager.h"

#include <cstdio>
#include <cerrno>
#include <cstdint>
#include <sstream>
#include <locale>

#ifdef _WIN32
#include <direct.h>
#include <windows.h>
#else
#include <sys/stat.h>
#include <codecvt>
#endif

namespace
{
    const char* IngredientKeys[] =
    {
        "slime_jelly",
        "herb",
        "boss_jelly"
    };

    const char* FoodKeys[] =
    {
        "jelly",
        "healing"
    };

    static_assert(
        sizeof(IngredientKeys) / sizeof(IngredientKeys[0])
        == ItemSettings::IngredientCount,
        "Update the save schema when adding ingredients."
        );

    static_assert(
        sizeof(FoodKeys) / sizeof(FoodKeys[0])
        == CookingSettings::DishCount,
        "Update the save schema when adding dishes."
        );

#ifndef _WIN32
    std::string Utf8(const std::wstring& path)
    {
        return std::wstring_convert<
            std::codecvt_utf8<wchar_t>
        >().to_bytes(path);
    }
#endif

    FILE* Open(
        const std::wstring& path,
        bool writing)
    {
#ifdef _WIN32
        FILE* file = nullptr;

        _wfopen_s(
            &file,
            path.c_str(),
            writing ? L"wb" : L"rb"
        );

        return file;
#else
        return std::fopen(
            Utf8(path).c_str(),
            writing ? "wb" : "rb"
        );
#endif
    }

    bool MakeDirectory(const std::wstring& path)
    {
#ifdef _WIN32
        return _wmkdir(path.c_str()) == 0
            || errno == EEXIST;
#else
        return mkdir(Utf8(path).c_str(), 0700) == 0
            || errno == EEXIST;
#endif
    }

    bool Replace(
        const std::wstring& from,
        const std::wstring& to)
    {
#ifdef _WIN32
        return MoveFileExW(
            from.c_str(),
            to.c_str(),
            MOVEFILE_REPLACE_EXISTING
            | MOVEFILE_WRITE_THROUGH
        ) != 0;
#else
        return std::rename(
            Utf8(from).c_str(),
            Utf8(to).c_str()
        ) == 0;
#endif
    }

    void RemoveTemporary(const std::wstring& path)
    {
#ifdef _WIN32
        _wremove(path.c_str());
#else
        std::remove(Utf8(path).c_str());
#endif
    }

    // 破損検出用。暗号化や改ざん防止ではない。
    std::uint32_t Checksum(const std::string& text)
    {
        std::uint32_t hash = 2166136261u;

        for (unsigned char c : text)
        {
            hash ^= c;
            hash *= 16777619u;
        }

        return hash;
    }

    std::string Encode(const SaveData& data)
    {
        std::ostringstream stream;
        stream.imbue(std::locale::classic());

        stream
            << "FARM_ACTION_SAVE 1\n"
            << "level " << data.level << '\n'
            << "exp " << data.exp << '\n'
            << "cleared "
            << (data.stageCleared ? 1 : 0)
            << '\n';

        for (int i = 0;
            i < ItemSettings::IngredientCount;
            ++i)
        {
            stream
                << IngredientKeys[i]
                << ' '
                << data.ingredients[i]
                << '\n';
        }

        for (int i = 0;
            i < CookingSettings::DishCount;
            ++i)
        {
            stream
                << FoodKeys[i]
                << ' '
                << data.foods[i]
                << '\n';
        }

        stream << "END\n";

        const std::string body = stream.str();

        return body
            + "CHECKSUM "
            + std::to_string(Checksum(body))
            + "\n";
    }

    SaveResult Decode(
        const std::string& text,
        SaveData& destination)
    {
        std::istringstream header(text);
        header.imbue(std::locale::classic());

        std::string magic;
        int version = 0;

        if (!(header >> magic >> version)
            || magic != "FARM_ACTION_SAVE")
        {
            return SaveResult::Corrupt;
        }

        if (version != 1)
        {
            return SaveResult::UnsupportedVersion;
        }

        const auto marker = text.rfind("CHECKSUM ");

        if (marker == std::string::npos)
        {
            return SaveResult::Corrupt;
        }

        const std::string body =
            text.substr(0, marker);

        std::istringstream check(
            text.substr(marker)
        );

        check.imbue(std::locale::classic());

        unsigned long long checksum = 0;

        if (!(check >> magic >> checksum)
            || magic != "CHECKSUM"
            || checksum != Checksum(body))
        {
            return SaveResult::Corrupt;
        }

        check >> std::ws;

        if (!check.eof())
        {
            return SaveResult::Corrupt;
        }

        std::istringstream stream(body);
        stream.imbue(std::locale::classic());

        stream >> magic >> version;

        auto integer =
            [&](const char* expected, int& value)
            {
                std::string key;

                return static_cast<bool>(
                    stream >> key >> value
                    ) && key == expected;
            };

        SaveData data;
        int cleared = 0;

        if (!integer("level", data.level)
            || !integer("exp", data.exp)
            || !integer("cleared", cleared)
            || (cleared != 0 && cleared != 1))
        {
            return SaveResult::Corrupt;
        }

        data.stageCleared = cleared != 0;

        for (int i = 0;
            i < ItemSettings::IngredientCount;
            ++i)
        {
            if (!integer(
                IngredientKeys[i],
                data.ingredients[i]))
            {
                return SaveResult::Corrupt;
            }
        }

        for (int i = 0;
            i < CookingSettings::DishCount;
            ++i)
        {
            if (!integer(
                FoodKeys[i],
                data.foods[i]))
            {
                return SaveResult::Corrupt;
            }
        }

        if (!(stream >> magic) || magic != "END")
        {
            return SaveResult::Corrupt;
        }

        stream >> std::ws;

        if (!stream.eof() || !data.Valid())
        {
            return SaveResult::Corrupt;
        }

        // 全項目を確認してから反映する。
        destination = data;

        return SaveResult::Success;
    }
}

std::wstring SaveManager::FilePath(int slot) const
{
    return directory_
        + L"/slot"
        + std::to_wstring(slot + 1)
        + L".sav";
}

SaveResult SaveManager::Read(
    int slot,
    SaveData& destination) const
{
    if (slot < 0 || slot >= SlotCount)
    {
        return SaveResult::InvalidSlot;
    }

    if (!Configured())
    {
        return SaveResult::NotConfigured;
    }

    FILE* file = Open(FilePath(slot), false);

    if (!file)
    {
        return errno == ENOENT
            ? SaveResult::Missing
            : SaveResult::IoError;
    }

    char buffer[4097];

    const size_t size = std::fread(
        buffer,
        1,
        sizeof(buffer),
        file
    );

    const bool failed =
        std::ferror(file) != 0;

    const int closed = std::fclose(file);

    if (failed || closed != 0)
    {
        return SaveResult::IoError;
    }

    if (size > 4096)
    {
        return SaveResult::Corrupt;
    }

    return Decode(
        std::string(buffer, size),
        destination
    );
}

SaveResult SaveManager::Write(
    int slot,
    const SaveData& data) const
{
    if (slot < 0 || slot >= SlotCount)
    {
        return SaveResult::InvalidSlot;
    }

    if (!Configured())
    {
        return SaveResult::NotConfigured;
    }

    if (!data.Valid())
    {
        return SaveResult::InvalidData;
    }

    if (!MakeDirectory(directory_))
    {
        return SaveResult::IoError;
    }

    const std::wstring path =
        FilePath(slot);

    const std::wstring temporary =
        path + L".tmp";

    FILE* file = Open(temporary, true);

    if (!file)
    {
        return SaveResult::IoError;
    }

    const std::string encoded = Encode(data);

    bool success =
        std::fwrite(
            encoded.data(),
            1,
            encoded.size(),
            file
        ) == encoded.size();

    if (std::fflush(file) != 0)
    {
        success = false;
    }

    if (std::fclose(file) != 0)
    {
        success = false;
    }

    // 書き終わってから既存ファイルと置き換える。
    if (!success || !Replace(temporary, path))
    {
        RemoveTemporary(temporary);
        return SaveResult::IoError;
    }

    return SaveResult::Success;
}

SaveSlotInfo SaveManager::Inspect(int slot) const
{
    SaveData data;
    SaveSlotInfo info;

    info.result = Read(slot, data);

    info.exists =
        info.result != SaveResult::Missing
        && info.result != SaveResult::NotConfigured
        && info.result != SaveResult::InvalidSlot;

    if (info.result == SaveResult::Success)
    {
        info.level = data.level;
        info.exp = data.exp;
        info.stageCleared = data.stageCleared;
    }

    return info;
}

const char* SaveResultText(SaveResult result)
{
    switch (result)
    {
    case SaveResult::Success:
        return "保存データを確認しました。";

    case SaveResult::NotConfigured:
        return "保存先が設定されていません。";

    case SaveResult::InvalidSlot:
        return "スロット番号が不正です。";

    case SaveResult::Missing:
        return "データなし";

    case SaveResult::Corrupt:
        return "データが破損しているためロードできません。";

    case SaveResult::UnsupportedVersion:
        return "対応していない形式のデータです。";

    case SaveResult::IoError:
        return "保存先を読み書きできませんでした。";

    case SaveResult::InvalidData:
        return "保存するデータが不正です。";

    case SaveResult::WrongPlace:
        return "セーブは町の自宅で行ってください。";
    }

    return "保存データのエラーです。";
}