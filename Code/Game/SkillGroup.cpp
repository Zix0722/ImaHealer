#include "SkillGroup.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/GameCommon.hpp"

SkillGroup::SkillGroup(Hero* owner)
	:m_owner(owner)
{

}

SkillGroup::~SkillGroup()
{

}

void SkillGroup::Update(float deltaseconds)
{
	for (int i = 0; i < m_skills.size(); ++i)
	{
		if (m_skills[i])
		{
			m_skills[i]->Update(deltaseconds);
		}
	}
}

void SkillGroup::Render() const
{
	Vec2 rightDragonPos = Vec2(m_pos.x + (m_skills.size() + 1) * 52.f, m_pos.y);
	AABB2 leftDragon = AABB2(Vec2(70.f, 0.f), Vec2(0.f,70.f));
	AABB2 rightDragon = AABB2(Vec2::ZERO, Vec2(70.f,70.f));
	leftDragon.SetCenter(m_pos);
	rightDragon.SetCenter(rightDragonPos);

	std::vector<Vertex_PCU> Verts;

	for (int i = 0; i < m_skills.size(); ++i)
	{
		if (m_skills[i])
		{
			m_skills[i]->Render();
		}
	}
	Texture* dragon = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/actionBarDragon.png");
	AddVertsForAABB2(Verts, leftDragon, Rgba8::WHITE);
	AddVertsForAABB2(Verts, rightDragon, Rgba8::WHITE);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	g_theRenderer->BindTexture(dragon);
	g_theRenderer->DrawVertexArray((int)Verts.size(), Verts.data());
}

void SkillGroup::AddSkillInto(Skill* skill)
{
	m_skills.push_back(skill);
	skill->m_pos = Vec2(m_pos.x + m_skills.size() * 52.f, m_pos.y);
	skill->m_bounds.SetCenter(skill->m_pos);
}

