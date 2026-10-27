#pragma once

#include "Player.h"
#include "Slime.h"
#include "GameConfig.h"
#include "StageItems.h"

#include <array>

class Input;

// 現段階はタイトルと動作確認ステージのみ。
// 町はSTEP 5で追加する。
class SceneManager
{
public:
    void Init();

    void Update(
        const Input& input,
        float seconds
    );

    void Draw() const;

    bool ShouldQuit() const
    {
        return quit_;
    }

private:
    enum class Scene
    {
        Title,
        Stage
    };

    void StartStage();

    Scene scene_ = Scene::Title;

    Player player_;

    std::array<
        Slime,
        GameConfig::SlimeCount
    > slimes_;

    int defeatedCount_ = 0;

    // 正式な持ち物。
    Inventory inventory_;

    // 今回のステージの食材。
    StageItems stageItems_;

    bool showAttackBounds_ = true;
    bool quit_ = false;
    bool showSettings_ = false;
};