#pragma once
#include "Hero.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Game/SkillGroup.hpp"
#include "Game/CastBar.hpp"


enum class warriorAnim
{
	Idle,
	Jump,
	Run,
	GetHit,
	Fall,
	Death,
	sinisterStrike,
	Eviscerate,
	Garrote,
};

class Warrior : public Hero
{
public:
	Warrior(Hero* boss);
	~Warrior();

	void virtual Update(float deltaSeconds);
	void virtual Render() const;
	void virtual RenderUI() const;
	float virtual GetMaxHP() const;
	float virtual GetPosX() const;
	Vec2 virtual GetHPBarCenter() const;
	Vec2 virtual GetMIDPlayer() const;
	virtual Hero* GetCurrentTarget() const;
	void UpdateUserInput(float deltaSeconds);
	AABB2 PlayAnimation(warriorAnim const& anim);
	virtual bool IsHeroDead() const;
	void UpdateAIInput(float deltaSeconds);
public:
	int index = 2;
	Hero* m_currentTarget = nullptr;
	Stopwatch* m_GCD = nullptr;
	float m_positionX = 500.f;
	float m_TargetPosX = 1050.f;
	AABB2 m_charBox;
	std::vector<SpriteAnimDefinition* > m_animDefs;
	warriorAnim m_currentAnim = warriorAnim::Idle;
	Stopwatch* m_animWatch = nullptr;
	SpriteSheet* m_spriteSheet[9];
	float m_maxHP = 1800.f;
	SkillGroup* m_skillGroup = nullptr;
	CastBar* m_castBar = nullptr;
	
private:
	void RenderChar() const;
	void RenderHPBars() const;
	void RenderSkillIcons() const;
	void SetAnimation(warriorAnim param1, float param2);
	bool m_isMoving = false;
};