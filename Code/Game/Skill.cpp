#include "Skill.hpp"
#include <vector>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Game/App.hpp"


Skill::Skill()
{

}

Skill::Skill(float CD, Texture* texture)
{
	m_CDWatch = new Stopwatch(CD);
	m_texture = texture;
	m_bounds.SetDimensions(Vec2(50.f, 50.f));
	m_tooltipsBox.SetDimensions(Vec2(200.f, 200.f));
}

Skill::~Skill()
{

}

void Skill::Update(float deltaseconds)
{
	UNUSED(deltaseconds);
	Vec2 cursorUV = g_theWindow->GetCursorNormalizeUVs();
	Vec2 cursorPos = App::m_screenCamera->GetPosAtUV(cursorUV);

	if (m_bounds.IsPointInside(cursorPos))
	{
		m_isCurserOnIcon = true;
		m_tooltipsBox.SetPosAsBottomLeft(cursorPos);
	}
	else
	{
		m_isCurserOnIcon = false;
	}
}

void Skill::Render() const
{
	std::vector<Vertex_PCU> Verts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	std::vector<Vertex_PCU> txtVerts;
	std::vector<Vertex_PCU> tintVerts;
	std::string str;
	float remainCD = m_CDWatch->m_duration - (Clock::GetSystemClock().GetTotalSeconds() - m_CDWatch->m_startTime);
	str = Stringf("%.1f",remainCD);
	if (m_CDWatch->IsStopped())
	{
		AddVertsForAABB2(Verts, m_bounds, Rgba8::WHITE);
	}
	else if (!m_CDWatch->HasDurationElapsed())
	{
		m_gameFont->AddVertsForTextInBox2D(txtVerts, m_bounds, 15.f, str, Rgba8(255, 50, 50), 0.7f);
		AddVertsForAABB2(tintVerts, m_bounds, Rgba8(0,0,0,100));
		AddVertsForAABB2(Verts, m_bounds, Rgba8::WHITE);
	}
	else
	{
		AddVertsForAABB2(Verts, m_bounds, Rgba8::WHITE);
	}

	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)Verts.size(), Verts.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)tintVerts.size(), tintVerts.data());
	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());

	//button icon
	if (m_buttonTexture)
	{
		std::vector<Vertex_PCU> buttonVerts;
		AABB2 iconBox = AABB2(Vec2(0.f, 0.f), Vec2(20.f, 20.f));
		iconBox.SetCenter(m_bounds.GetCenter() + Vec2(-0.f, -30.f));
		AddVertsForAABB2(buttonVerts, iconBox, Rgba8::WHITE);
		g_theRenderer->BindTexture(m_buttonTexture);
		g_theRenderer->DrawVertexArray((int)buttonVerts.size(), buttonVerts.data());
	}
}

void Skill::RenderToolTip() const
{
	std::vector<Vertex_PCU> bgVerts;
	std::vector<Vertex_PCU> iconVerts;
	std::vector<Vertex_PCU> descriptionBgVerts;
	std::vector<Vertex_PCU> NameTxtVerts;
	std::vector<Vertex_PCU> descriptionTxtVerts;
	std::vector<Vertex_PCU> outlineVerts;

	float width = m_tooltipsBox.GetDimensions().x;
	float length = m_tooltipsBox.GetDimensions().y;
	AABB2 nameLineBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.5f, m_tooltipsBox.m_mins.y + length * 0.6f),
		Vec2(m_tooltipsBox.m_mins.x + width * 0.9f, m_tooltipsBox.m_mins.y + length * 0.9f));
	AABB2 descriptionBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.1f, m_tooltipsBox.m_mins.y + length * 0.1f),
		Vec2(m_tooltipsBox.m_mins.x + width * 0.9f, m_tooltipsBox.m_mins.y + length * 0.5f));
	AABB2 iconBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.1f, m_tooltipsBox.m_mins.y + length * 0.6f),
		Vec2(m_tooltipsBox.m_mins.x + width * 0.4f, m_tooltipsBox.m_mins.y + length * 0.9f));
	AddVertsForAABB2(bgVerts, m_tooltipsBox, Rgba8(4, 45, 56, 230));
	AddVertsForAABB2(descriptionBgVerts, descriptionBox, Rgba8(250, 250, 0, 255));
	AddVertsForAABB2(iconVerts, iconBox, Rgba8(255, 255, 255, 180));

	m_gameFont->AddVertsForTextInBox2D(NameTxtVerts, nameLineBox, 10.f, m_skillName, Rgba8(250, 250, 100), 0.7f);
	m_gameFont->AddVertsForTextInBox2D(descriptionTxtVerts, descriptionBox, 10.f, m_skillDescription, Rgba8(4, 45, 56, 255), 0.7f, Vec2(.5f, .5f));

	Rgba8 outlineColor = Rgba8(250, 250, 100);

	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.05f)), m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.95f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.05f)), m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.05f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.95f)), m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.95f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.95f)), m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.05f)), 1.1f, outlineColor);

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
	g_theRenderer->DrawVertexArray((int)descriptionBgVerts.size(), descriptionBgVerts.data());


	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)iconVerts.size(), iconVerts.data());

	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)NameTxtVerts.size(), NameTxtVerts.data());
	g_theRenderer->DrawVertexArray((int)descriptionTxtVerts.size(), descriptionTxtVerts.data());

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)outlineVerts.size(), outlineVerts.data());
}

void Skill::SpawnEffect()
{

}

void Skill::SkillOnRelease()
{
	m_CDWatch->Start();
}

bool Skill::IsSkillReadyToUse() const
{
	if (m_CDWatch->IsStopped())
	{
		return true;
	}
	if (m_CDWatch->HasDurationElapsed()) 
	{
		return true;
	}
	else
	{
		return false;
	}
}

