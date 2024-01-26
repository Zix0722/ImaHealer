#pragma once
#include "Skill.hpp"

class ShadowArrow : public Skill
{
public:
	ShadowArrow(float CD, Texture* texture);
	~ShadowArrow();
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	void virtual SpawnEffect();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	Vec2  m_startAt;
	float m_num = 0.f;
	//Effects
};

class DeathCurse : public Skill
{
public:
	DeathCurse(float CD, Texture* texture);
	~DeathCurse();
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	void virtual SpawnEffect();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects

};

class Eradication : public Skill
{
public:
	Eradication(float CD, Texture* texture);
	~Eradication();
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	void virtual SpawnEffect();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	Hero* m_targetTwo = nullptr;
	float m_num = 0.f;
	//Effects

};

class BreakingStars : public Skill
{
public:
	BreakingStars(float CD, Texture* texture);
	~BreakingStars();
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	void virtual SpawnEffect();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects

};