#pragma once

#include "Collision.h"

// 描画とクリック判定で同じ座標を使用する。
namespace UIConfig
{
    inline Rect Button(int row)
    {
        return {
            520.0f,
            330.0f + row * 80.0f,
            880.0f,
            64.0f
        };
    }

    inline Rect Tab(int index)
    {
        return {
            440.0f,
            360.0f + index * 90.0f,
            200.0f,
            64.0f
        };
    }

    inline Rect FoodRow(int index)
    {
        return {
            720.0f,
            360.0f + index * 110.0f,
            720.0f,
            90.0f
        };
    }

    inline Rect Minus(int index)
    {
        return {
            1260.0f,
            380.0f + index * 110.0f,
            60.0f,
            50.0f
        };
    }

    inline Rect Plus(int index)
    {
        return {
            1360.0f,
            380.0f + index * 110.0f,
            60.0f,
            50.0f
        };
    }
}