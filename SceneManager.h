#pragma once

#include "Player.h"
#include "TrainingTarget.h"

class Input;

// 現段階ではタイトルと動作確認ステージを管理する。
class SceneManager
{
public:
    void Init();
    void Update(const Input& input, float seconds);
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
    TrainingTarget target_;

    bool showAttackBounds_ = true;

    bool quit_ = false;
    bool showSettings_ = false;
};