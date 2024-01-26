#pragma once
#include "Effect.hpp"
#include "Hero.hpp"
#include "Skill.hpp"
#include "Engine/Math/OBB2.hpp"

class SkillEffect : public Effect 
{
public:
	SkillEffect() = default;
	~SkillEffect();

	SkillEffect(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond = 20.f,
		SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP, Hero* target = nullptr,  float numDmg = 0.f);
	void SetDuration(float duration);
	void SetSkill(Skill* skill);
	void GetReadyPlayTowardTarget(float speed);
	void GetReadyPlayOnTarget();
	void Update(float deltaseconds);
	void Render() const;

	void EndFrame();
public:
	Skill* m_skill = nullptr;
	bool m_isusingOBB = false;
	bool m_isUsingRotate90Degrees = false;
	OBB2 m_oBounds;
	Vec2 m_offset;
	float m_speed = 1.f;
	Hero* m_target = nullptr;
	float m_numDmg = 0.f;
	bool m_isStatic = false;
private:
	bool m_doOnce = false;
};