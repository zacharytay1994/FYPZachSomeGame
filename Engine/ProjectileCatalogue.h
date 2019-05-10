#pragma once

class ProjectileCatalogue {
public:
	static constexpr float GetSpeed(const int& id) {
		return ProjectileSpeedArray[id];
	}
	static constexpr int GetType(const int& id) {
		return ProjectileTypeArray[id];
	}
private:
	static constexpr float ProjectileSpeedArray[3] = { 8.0f, 8.0f, 8.0f };
	static constexpr int ProjectileTypeArray[3] = { 1, 0, 1 }; // 0 for low, 1 for lob
};