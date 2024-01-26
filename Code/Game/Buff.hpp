#pragma once
#include "Skill.hpp"
#include "SkillEffect.hpp"
class Buff : public Skill
{
public:
	Buff();
	Buff(float CD, Texture* texture);
	virtual ~Buff();

	void Update(float deltaSeconds);
	void Render() const;
	void RenderToolTips() const;
	void SetNameAndDescription(std::string const& name, std::string const& description);
	void Refresh();
	virtual Hero* GetCurrentTarget() const;
	virtual void Cancel();
	virtual void SetTarget(Hero* hero);
	void virtual SkillOnStart();
	void virtual DoSkillLogic();
	void virtual SpawnEffect();
	void virtual RefreshEffect() = 0;

public:
	bool m_isDebuff = false;
	std::string m_buffName;
	std::string m_buffDescription;
	bool m_isCurserOnIcon = false;
	AABB2 m_tooltipsBox;
	bool m_markForDelete = false;
	SkillEffect* m_effect = nullptr;
};


class HoT : public Buff 
{
public:
	HoT();
	HoT(float CD, Texture* texture, Hero* target);
	~HoT();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
	Stopwatch* m_hotWatch = nullptr;
};

class sheild : public Buff
{
public:
	sheild();
	sheild(float CD, Texture* texture, Hero* target);
	~sheild();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
};

class Curse : public Buff
{
public:
	Curse();
	Curse(float CD, Texture* texture, Hero* target);
	~Curse();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
	Stopwatch* m_dotWatch = nullptr;
	int scale = 1;
};

class Taunt : public Buff
{
public:
	Taunt();
	Taunt(float CD, Texture* texture, Hero* target);
	~Taunt();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
	Hero* m_user = nullptr;
};

class Bleed : public Buff
{
public:
	Bleed();
	Bleed(float CD, Texture* texture, Hero* target);
	~Bleed();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
	Stopwatch* m_dotWatch = nullptr;
	int m_stack = 1;
};

class Kicked : public Buff
{
public:
	Kicked();
	Kicked(float CD, Texture* texture, Hero* target);
	~Kicked();

	virtual void DoSkillLogic();
	void virtual RefreshEffect();
public:
	Hero* m_target = nullptr;
	Stopwatch* m_dotWatch = nullptr;
};