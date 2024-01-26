#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"

enum class FloatingNumType
{
	Damage,
	Heal,
	Cnt,
};

class FloatingNumber
{
public:
	FloatingNumber(Vec2 start, float num, FloatingNumType type);
	~FloatingNumber();

	void Update(float deltaseconds);
	void Render() const;
	static FloatingNumber* SpawnNumber(float num, FloatingNumType type, Vec2 pos);



public:
	FloatingNumType m_type = FloatingNumType::Damage;
	float m_number = 0.f;
	Vec2 m_startPos = Vec2::ZERO;
	Stopwatch* m_stopWatch = nullptr;
	AABB2 m_bounds;
	float m_movingDegrees = 0.f;
	bool m_isEnd = false;
};
