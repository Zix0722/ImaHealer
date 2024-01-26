#pragma once
#include "Hero.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "CastBar.hpp"
#include "SkillGroup.hpp"


enum class tankAnim
{
	Idle,
	Jump,
	Run,
	GetHit,
	Fall,
	Death,
	slam,
	Enrage,
	Execute,
};

class Tank : public Hero
{
public:
	Tank(Hero* boss);
	~Tank();

	void virtual Update(float deltaSeconds);
	void virtual Render() const;
	void virtual RenderUI() const;
	float virtual GetMaxHP() const;
	float virtual GetPosX() const;
	Vec2 virtual GetHPBarCenter() const;
	virtual Hero* GetCurrentTarget() const;
	Vec2 virtual GetMIDPlayer() const;
	void UpdateAIInput(float deltaSeconds);
	AABB2 PlayAnimation(tankAnim const& anim);
	virtual bool IsHeroDead() const;
public:
	int index = 1;
	Hero* m_currentTarget = nullptr;
	Stopwatch* m_GCD = nullptr;
	float m_positionX = 700.f;
	AABB2 m_charBox;
	std::vector<SpriteAnimDefinition* > m_animDefs;
	tankAnim m_currentAnim = tankAnim::Idle;
	Stopwatch* m_animWatch = nullptr;
	SpriteSheet* m_spriteSheet[9];
	float m_maxHP = 2000.f;
	SkillGroup* m_skillGroup = nullptr;
	CastBar* m_castBar = nullptr;
private:
	void RenderChar() const;
	void RenderHPBars() const;
	void RenderSkillIcons() const;
	void SetAnimation(tankAnim param1, float param2);
	bool m_isMoving = false;
	float m_TargetPosX = 1100.f;
};