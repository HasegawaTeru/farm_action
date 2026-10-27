#pragma once

namespace TownSettings
{
    const float Width = 3200.0f;
    const float SpawnX = 200.0f;

    // ©‘îE’²—êEŒf¦”ÂB
    const float FacilityX[3] =
    {
        240.0f,
        1500.0f,
        2600.0f
    };

    const float InteractionRange = 120.0f;
}

class Town
{
public:
    enum class Facility
    {
        None = -1,
        House,
        Kitchen,
        Board
    };

    void Init(bool atHouse = true);
    void Update(float seconds, int horizontal);

    Facility NearbyFacility() const;

    float X() const { return x_; }
    float CameraX() const;

    int Facing() const { return facing_; }

private:
    float x_ = TownSettings::SpawnX;
    int facing_ = 1;
};