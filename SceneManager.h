#pragma once
#include "Player.h"

class Input;

// STEP 1ではタイトルと動作確認ステージのみを用意する。
class SceneManager
{
public:
    void Init();
    void Update(const Input& input, float seconds);
    void Draw() const;

    bool ShouldQuit() const { return quit_; }

private:
    enum class Scene
    {
        Title,
        Stage
    };

    void StartStage();

    Scene scene_ = Scene::Title;
    Player player_;

    bool quit_ = false;
    bool showSettings_ = false;
};