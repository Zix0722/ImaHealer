#pragma once
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Hero.hpp"

class Skill
{
public:
	Skill();
	Skill(float CD, Texture* texture);
	virtual ~Skill();

	void Update(float deltaseconds);
	void Render() const;
	void RenderToolTip() const;
	void virtual SkillOnStart() = 0;
	void virtual DoSkillLogic() = 0;
	void virtual SpawnEffect();
	virtual Hero* GetCurrentTarget() const = 0;
	virtual void Cancel() = 0;
	virtual void SetTarget(Hero* hero) = 0;
	void SkillOnRelease();
	bool IsSkillReadyToUse() const;

public:
	Stopwatch* m_CDWatch = nullptr;
	Texture* m_texture = nullptr;
	Texture* m_buttonTexture = nullptr;
	AABB2 m_bounds;
	Vec2 m_pos;
	bool m_isCurserOnIcon = false;
	AABB2 m_tooltipsBox;
	std::string m_skillName;
	std::string m_skillDescription;
};