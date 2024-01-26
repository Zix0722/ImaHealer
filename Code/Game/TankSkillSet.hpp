#pragma once
#include "Skill.hpp"

class Slam : public Skill
{
public:
	Slam(float CD, Texture* texture);
	~Slam();
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

class Execute : public Skill
{
public:
	Execute(float CD, Texture* texture);
	~Execute();
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


class Enrage : public Skill
{
public:
	Enrage(float CD, Texture* texture);
	~Enrage();
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

class Shout : public Skill
{
public:
	Shout(float CD, Texture* texture);
	~Shout();
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