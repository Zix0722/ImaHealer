#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Skill.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class CastBar
{
public:
	CastBar(Hero* owner);
	~CastBar();

	void Update(float deltaseconds);
	void Render() const;

	void SetPos(Vec2 pos);
	void OnStart(Skill* skill, float duration);
	bool IsChannelling() const;
	void Interupt();
	void OnCast(Skill* skill);
	void OnCastEffect(Skill* skill);

public:
	Hero* m_owner = nullptr;
	AABB2 m_bounds;
	Vec2 m_pos;
	Stopwatch* m_channelingWatch = nullptr;
	Stopwatch* m_mode2Watch = nullptr;
	Texture* m_texture = nullptr;
	Skill* m_currentChanelling = nullptr;
	bool m_isDoLogicAfterHit = false;
	bool m_isChannelingMode = false;
	SoundID castingSound;
	SoundPlaybackID castingSound_playback;
};