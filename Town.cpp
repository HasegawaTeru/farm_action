#include "Town.h"
#include "GameConfig.h"

#include <algorithm>
#include <cmath>

void Town::Init(bool atHouse)
{
    x_ = atHouse
        ? TownSettings::SpawnX
        : TownSettings::FacilityX[2]
        - GameConfig::PlayerWidth * 0.5f;

    facing_ = 1;
}

void Town::Update(float seconds, int horizontal)
{
    if (seconds <= 0.0f)
    {
        return;
    }

    seconds = std::min(
        seconds,
        GameConfig::MaxFrameSeconds
    );

    horizontal = std::max(-1, std::min(1, horizontal));

    if (horizontal != 0)
    {
        facing_ = horizontal;
    }

    x_ = std::max(
        0.0f,
        std::min(
            TownSettings::Width - GameConfig::PlayerWidth,
            x_ + horizontal * GameConfig::MoveSpeed * seconds
        )
    );
}

float Town::CameraX() const
{
    return std::max(
        0.0f,
        std::min(
            TownSettings::Width - GameConfig::WindowWidth,

            x_ + GameConfig::PlayerWidth * 0.5f
            - GameConfig::WindowWidth * 0.5f
        )
    );
}

Town::Facility Town::NearbyFacility() const
{
    for (int i = 0; i < 3; ++i)
    {
        const float distance = std::fabs(
            x_ + GameConfig::PlayerWidth * 0.5f
            - TownSettings::FacilityX[i]
        );

        if (distance <= TownSettings::InteractionRange)
        {
            return static_cast<Facility>(i);
        }
    }

    return Facility::None;
}