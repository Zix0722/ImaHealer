#include "CastBar.hpp"
#include <vector>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/GameCommon.hpp"

CastBar::CastBar(Hero* owner)
	:m_owner(owner)
{
	m_bounds.SetDimensions(Vec2(200.f, 20.f));
	if (m_owner->GetMaxHP() == 1000.f)
	{
		castingSound = g_theAudio->CreateOrGetSound("Data/Audio/casting.wav");
	}
}

CastBar::~CastBar()
{

}

void CastBar::Update(float deltaseconds)
{
	UNUSED(deltaseconds);
	if (m_isDoLogicAfterHit)
	{
		if (m_channelingWatch)
		{
			if (m_isChannelingMode)
			{
				if (!m_channelingWatch->HasDurationElapsed())
				{
					if (m_mode2Watch->HasDurationElapsed())
					{
						OnCastEffect(m_currentChanelling);
						m_mode2Watch->Start();
					}

				}
				else
				{
					delete m_channelingWatch;
					m_channelingWatch = nullptr;
					m_currentChanelling = nullptr;
				}
			}
			else
			{
				if (m_channelingWatch->HasDurationElapsed())
				{
					OnCastEffect(m_currentChanelling);

				}
			}
			
		}
	}
	else
	{
		if (m_channelingWatch)
		{
			if (m_channelingWatch->HasDurationElapsed())
			{
				if (m_currentChanelling->GetCurrentTarget() == nullptr)
				{
					return;
				}
				OnCast(m_currentChanelling);
			}
		}
	}


	
}

void CastBar::Render() const
{
	if (!IsChannelling())
	{
		return;
	}
	//bott
	std::vector<Vertex_PCU> bott;
	AddVertsForAABB2(bott, m_bounds, Rgba8::GREY);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)bott.size(), bott.data());
	//chanelBox
	std::vector<Vertex_PCU> chanelVerts;
	AABB2 chanelBox = m_bounds;
	chanelBox.SetDimensions(Vec2(197.f, 17.f));
	float width = chanelBox.m_maxs.x - chanelBox.m_mins.x;
	float percentage = m_channelingWatch->GetElapsedTime() / m_channelingWatch->m_duration;
	width = width * percentage;
	chanelBox.m_maxs.x = width + chanelBox.m_mins.x;

	AddVertsForAABB2(chanelVerts, chanelBox, Rgba8(190,190,0, 200));
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)chanelVerts.size(), chanelVerts.data());
	//icon
	std::vector<Vertex_PCU> skillIconVerts;
	AABB2 IconBox = AABB2(Vec2(m_bounds.m_mins.x - 30.f, m_bounds.m_mins.y - 5.f), Vec2(m_bounds.m_mins.x, m_bounds.m_maxs.y + 5.f));
	AddVertsForAABB2(skillIconVerts, IconBox, Rgba8::WHITE);
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)skillIconVerts.size(), skillIconVerts.data());
	

	//text
	std::vector<Vertex_PCU> txtVerts;
	std::string str;
	AABB2 textBox = AABB2(m_bounds.m_maxs - Vec2(50.f,50.f), Vec2(m_bounds.m_maxs.x, m_bounds.m_maxs.y));
	str = Stringf("%.1f / %.1f ", m_channelingWatch->GetElapsedTime(), m_channelingWatch->m_duration);
	m_gameFont->AddVertsForTextInBox2D(txtVerts, textBox, 15.f, str, Rgba8::WHITE, 0.5f, Vec2(0.5f, 0.8f));
	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());
}

void CastBar::SetPos(Vec2 pos)
{
	m_pos = pos;
	m_bounds.SetCenter(pos);
}

void CastBar::OnStart(Skill* skill, float duration)
{
	if (m_owner->GetMaxHP() == 1000.f)
	{
		castingSound_playback = g_theAudio->StartSound(castingSound, false, 1.f, 0.f);
	}
	if (skill)
	{
		m_texture = skill->m_texture;
		if (!IsChannelling())
		{
			m_channelingWatch = new Stopwatch(duration);
			m_channelingWatch->Start();
			if (m_isChannelingMode)
			{
				m_mode2Watch = new Stopwatch(duration / 6.f);
				m_mode2Watch->Start();
			}
			m_currentChanelling = skill;
			skill->SetTarget(m_owner->GetCurrentTarget());
			skill->SkillOnStart();
		}
	}
	else
	{
		m_texture = nullptr;
		m_currentChanelling = nullptr;
		m_mode2Watch = nullptr;
	}

}

bool CastBar::IsChannelling() const
{
	if (m_channelingWatch)
	{
		if (!m_channelingWatch->HasDurationElapsed())
		{
			return true;
		}
	}
	return false;
}

void CastBar::Interupt()
{
	if (m_owner->GetMaxHP() == 1000.f)
	{
		g_theAudio->StopSound(castingSound_playback);
	}
	if (m_currentChanelling)
	{
		m_currentChanelling->Cancel();
	}
	if (m_channelingWatch)
	{
		delete m_channelingWatch;
		m_channelingWatch = nullptr;
	}
	m_currentChanelling = nullptr;
}

void CastBar::OnCast(Skill* skill)
{
	if (m_owner->GetMaxHP() == 1000.f)
	{
		g_theAudio->StopSound(castingSound_playback);
	}
	skill->SkillOnRelease();
	skill->DoSkillLogic();
	delete m_channelingWatch;
	m_channelingWatch = nullptr;
	m_currentChanelling = nullptr;
}

void CastBar::OnCastEffect(Skill* skill)
{
	if (m_owner->GetMaxHP() == 1000.f)
	{
		g_theAudio->StopSound(castingSound_playback);

	}
	skill->SkillOnRelease();
	skill->SpawnEffect();
	if (m_isChannelingMode)
	{

	}
	else
	{
		delete m_channelingWatch;
		m_channelingWatch = nullptr;
		m_currentChanelling = nullptr;
	}
	
}

