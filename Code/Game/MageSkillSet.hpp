#pragma once
#include "Skill.hpp"

class FireBall : public Skill
{
public:
	FireBall(float CD, Texture* texture);
	~FireBall();
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

class Blizzard : public Skill
{
public:
	Blizzard(float CD, Texture* texture);
	~Blizzard();
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

