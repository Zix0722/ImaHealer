#pragma once
#include "Hero.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "SkillGroup.hpp"
#include "CastBar.hpp"


class Level;

enum class healAnim
{
	Idle,
	smallHeal,
	hot,
	clean,
	bigHeal,
	protect,
	walk,
	die,
};

class Healer : public Hero
{
public:
	Healer();
	Healer(Level* lvl);
	~Healer();

	void virtual Update(float deltaSeconds);
	void virtual Render() const;
	void virtual RenderUI() const;
	float virtual GetMaxHP() const;
	float virtual GetPosX() const;
	Vec2 virtual GetHPBarCenter() const;
	Vec2 virtual GetMIDPlayer() const;
	virtual Hero* GetCurrentTarget() const;
	void UpdateUserInput(float deltaSeconds);
	AABB2 PlayAnimation(healAnim const& anim);
	void SetAnimation(healAnim const& anim, float duration);
	virtual bool IsHeroDead() const;


public:
	int index = 0;
	int nexIndex = 1;
	Level* m_level = nullptr;
	Hero* m_currentTarget = nullptr;
	Stopwatch* m_GCD = nullptr;
	float m_positionX = 100.f;
	std::vector<SpriteAnimDefinition* > m_animDefs;
	SpriteAnimDefinition* m_indicatorAnimDef = nullptr;
	healAnim m_currentAnim = healAnim::Idle;
	Stopwatch* m_animWatch = nullptr;
	SpriteSheet* m_spriteSheet = nullptr;
	SpriteSheet* m_indicatorSpritesheet = nullptr;
	float m_maxHP = 1000.f;
	Texture* m_indicatorOnChar = nullptr;
	CastBar* m_castBar = nullptr;
	SkillGroup* m_skillGroup = nullptr;

private:
	void RenderHPBars() const;
	void RenderChar() const;
	void RenderSkillIcons() const;
};