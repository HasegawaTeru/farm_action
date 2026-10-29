#pragma once

class Progression
{
public:
    void Init();
    void AddExp(int amount);

    int Level() const { return level_; }
    int Exp() const { return exp_; }

    int RequiredExp() const;
    int MaxHp() const;
    int AttackPower() const;

    bool Restore(int level, int exp);
private:
    int level_ = 1;
    int exp_ = 0;
};