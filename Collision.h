#pragma once

struct Rect
{
    float x;
    float y;
    float width;
    float height;
};

// 矩形同士の重なり判定。
// 辺が触れるだけの状態は重なりに含めない。
inline bool Overlaps(const Rect& a, const Rect& b)
{
    return a.x < b.x + b.width
        && a.x + a.width > b.x
        && a.y < b.y + b.height
        && a.y + a.height > b.y;
}

// 円と矩形の重なり判定。
// 円の中心から、矩形上の最近点までの距離を使用する。
inline bool OverlapsCircle(
    const Rect& rect,
    float centerX,
    float centerY,
    float radius)
{
    const float nearestX =
        centerX < rect.x
        ? rect.x
        : (
            centerX > rect.x + rect.width
            ? rect.x + rect.width
            : centerX
            );

    const float nearestY =
        centerY < rect.y
        ? rect.y
        : (
            centerY > rect.y + rect.height
            ? rect.y + rect.height
            : centerY
            );

    const float dx = centerX - nearestX;
    const float dy = centerY - nearestY;

    return dx * dx + dy * dy < radius * radius;
}   