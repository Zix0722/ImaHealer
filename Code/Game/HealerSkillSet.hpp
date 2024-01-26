#pragma once
#include "Skill.hpp"

class FlashHeal : public Skill
{
public:
	FlashHeal(float CD, Texture* texture);
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects
};

class Rejuvenation : public Skill
{
public:
	Rejuvenation(float CD, Texture* texture);
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects
};

class Dispel : public Skill
{
public:
	Dispel(float CD, Texture* texture);
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects
};

class InfusionOfLight : public Skill
{
public:
	InfusionOfLight(float CD, Texture* texture);
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects
};

class DivineShield : public Skill
{
public:
	DivineShield(float CD, Texture* texture);
	void virtual SkillOnStart();
	virtual Hero* GetCurrentTarget() const;
	void virtual DoSkillLogic();
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
public:
	Hero* m_target = nullptr;
	float m_num = 0.f;
	//Effects
};