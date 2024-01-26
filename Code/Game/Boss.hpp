#pragma once
#include "Hero.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "SkillGroup.hpp"
#include "CastBar.hpp"

class Level;

enum class bossAnim
{
	Idle,
	shadowArrow,
	deathCurse,
	GetHit,
	Fall,
	Death,
	eradication,
	breakingStars,
};

class Boss : public Hero
{
public:
	Boss();
	Boss(Level* lvl);
	~Boss();

	void virtual Update(float deltaSeconds);
	void virtual Render() const;
	void virtual RenderUI() const;
	float virtual GetMaxHP() const;
	float virtual GetPosX() const;
	Vec2 virtual GetHPBarCenter() const;
	virtual Hero* GetCurrentTarget() const;
	virtual void SetCurrentTarget(Hero* target);
	Vec2 virtual GetMIDPlayer() const;
	virtual CastBar* GetCastBar() const;
	void UpdateAIInput(float deltaSeconds);
	AABB2 PlayAnimation(bossAnim const& anim);
	virtual bool IsHeroDead() const;
public:
	Hero* m_currentTarget = nullptr;
	Level* m_level = nullptr;
	Stopwatch* m_GCD = nullptr;
	float m_positionX = 1200.f;
	AABB2 m_charBox;
	std::vector<SpriteAnimDefinition* > m_animDefs;
	bossAnim m_currentAnim = bossAnim::Idle;
	Stopwatch* m_animWatch = nullptr;
	SpriteSheet* m_spriteSheet[8];
	SkillGroup* m_skillGroup = nullptr;
	CastBar* m_castBar = nullptr;
	float m_maxHP = 50000.f;
private:
	void RenderChar() const;
	void RenderHPBars() const;
	void RenderSkillIcons() const;
	void SetAnimation(bossAnim anim, float duration);
	void SetRandomTarget();
};