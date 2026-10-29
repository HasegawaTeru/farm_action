#include "Progression.h"
#include "GameConfig.h"

void Progression::Init()
{
    level_ = 1;
    exp_ = 0;
}

void Progression::AddExp(int amount)
{
    if (amount <= 0 || level_ >= 30)
    {
        return;
    }

    // 大きなEXP加算でも整数のオーバーフローを防ぐ。
    long long remaining =
        static_cast<long long>(exp_) + amount;

    while (level_ < 30 && remaining >= level_ * 10)
    {
        remaining -= level_ * 10;
        ++level_;
    }

    exp_ = level_ == 30
        ? 0
        : static_cast<int>(remaining);
}

int Progression::RequiredExp() const
{
    return level_ == 30 ? 0 : level_ * 10;
}

int Progression::MaxHp() const
{
    return GameConfig::PlayerMaxHp + (level_ - 1) * 3;
}

int Progression::AttackPower() const
{
    return GameConfig::PlayerAttackPower + (level_ - 1) * 2;
}

bool Progression::Restore(int level, int exp)
{
    if (level < 1
        || level > 30
        || exp < 0
        || (level == 30
            ? exp != 0
            : exp >= level * 10))
    {
        return false;
    }

    level_ = level;
    exp_ = exp;

    return true;
}