#pragma once
#include "Hero.hpp"
#include "Skill.hpp"

class SkillGroup
{
public:

	SkillGroup(Hero* owner);
	~SkillGroup();

	void Update(float deltaseconds);
	void Render() const;

	void AddSkillInto(Skill* skill);


public:
	Hero* m_owner = nullptr;
	std::vector<Skill*> m_skills;
	Vec2 m_pos;
};