#pragma once
#include "Skill.hpp"

class SinisterStrike : public Skill
{
public:
	SinisterStrike(float CD, Texture* texture);
	~SinisterStrike();
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

class Eviscerate : public Skill
{
public:
	Eviscerate(float CD, Texture* texture);
	~Eviscerate();
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


class Garrote : public Skill
{
public:
	Garrote(float CD, Texture* texture);
	~Garrote();
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

class Kick : public Skill
{
public:
	Kick(float CD, Texture* texture);
	~Kick();
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