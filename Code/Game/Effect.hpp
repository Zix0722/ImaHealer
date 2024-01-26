#pragma once
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

class Stopwatch;

class Effect
{
public:
	Effect() = default;
	Effect(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond = 20.f, SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP);
	~Effect() = default;

	void SetSize(float width, float height);
	void SetUpTime(float duration);
	void SetUpMovements(Vec2 const& velocity, Vec2 const& dirction);
	void Update(float deltaSeconds);
	void Render() const;

public:
	AABB2 m_bounds;
	SpriteSheet* m_spritesheet = nullptr;
	SpriteAnimDefinition* m_animDef = nullptr;
	Vec2 m_position;
	Vec2 m_velocity;
	Vec2 m_dirction;
	float m_animDelay = 0.f;
	bool m_needToDestory = false;
	Stopwatch* m_lifeWatch = nullptr;
	SpriteAnimPlaybackType m_type = SpriteAnimPlaybackType::ONCE;
};