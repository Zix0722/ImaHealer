#pragma once
#include "Hero.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "SkillGroup.hpp"
#include "CastBar.hpp"


enum class mageAnim
{
	Idle,
	Run,
	GetHit,
	Death,
	Attack,
	Blizzard,
};

class Mage : public Hero
{
public:
	Mage(Hero* boss);
	~Mage();

	void virtual Update(float deltaSeconds);
	void virtual Render() const;
	void virtual RenderUI() const;
	float virtual GetMaxHP() const;
	float virtual GetPosX() const;
	Vec2 virtual GetHPBarCenter() const;
	Vec2 virtual GetMIDPlayer() const;
	virtual Hero* GetCurrentTarget() const;
	void UpdateUserInput(float deltaSeconds);
	AABB2 PlayAnimation(mageAnim const& anim);
	virtual bool IsHeroDead() const;
	SkillGroup* m_skillGroup = nullptr;
	CastBar* m_castBar = nullptr;
	void UpdateAIInput(float deltaSeconds);
public:
	int index = 3;
	int nextIndex = 0;
	Hero* m_currentTarget = nullptr;
	Stopwatch* m_GCD = nullptr;
	float m_positionX = 300.f;
	AABB2 m_charBox;
	std::vector<SpriteAnimDefinition* > m_animDefs;
	mageAnim m_currentAnim = mageAnim::Idle;
	Stopwatch* m_animWatch = nullptr;
	Stopwatch* m_randBehaveWatch = nullptr;
	SpriteSheet* m_spriteSheet[6];
	float m_maxHP = 1300.f;
private:
	void RenderChar() const;
	void RenderHPBars() const;
	void RenderSkillIcons() const;
	void SetAnimation(mageAnim param1, float param2);
};