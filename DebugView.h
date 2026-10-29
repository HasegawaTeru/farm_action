#pragma once

class GameSession;

// 観察専用。ゲームデータを変更しない。
class DebugView
{
public:
    void Init();

    void Update(
        float realSeconds,
        bool toggle
    );

    void Draw(
        const GameSession& game,
        bool menuOpen,
        bool title
    ) const;

    bool Enabled() const { return enabled_; }
    float Fps() const { return fps_; }
    float FrameMs() const { return frameMs_; }

private:
    bool enabled_ = false;

    float fps_ = 0.0f;
    float frameMs_ = 0.0f;
    float elapsed_ = 0.0f;

    int frames_ = 0;
};