#include "SkillEffect.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"

SkillEffect::~SkillEffect()
{

}

SkillEffect::SkillEffect(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond /*= 20.f*/, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/, Hero* target /*= nullptr*/, float numDmg /*= 0.f*/)
	:Effect(sheet, startSpriteIndex, endSpriteIndex, framesPerSecond, playbackType)
{
	m_target = target;
	m_numDmg = numDmg;
}

void SkillEffect::SetDuration(float duration)
{
	if (m_lifeWatch)
	{
		delete m_lifeWatch;
		m_lifeWatch = nullptr;
	}
	m_lifeWatch = new Stopwatch(duration);
	m_lifeWatch->Start();
}

void SkillEffect::SetSkill(Skill* skill)
{
	m_skill = skill;
}

void SkillEffect::GetReadyPlayTowardTarget(float speed)
{
	Vec2 disp = (m_target->m_bounds.GetCenter() + m_offset) - m_position;
	m_dirction = disp.GetNormalized();
	m_speed = speed;
}

void SkillEffect::GetReadyPlayOnTarget()
{

}

void SkillEffect::Update(float deltaseconds)
{
	UNUSED(deltaseconds);
	m_bounds.SetCenter(m_position);
	if (m_lifeWatch)
	{
		if (m_lifeWatch->HasDurationElapsed() && !m_needToDestory)
		{
			m_needToDestory = true;
			if (m_skill)
			{
				m_skill->DoSkillLogic();
			}
			return;
		}
	}
	if (!m_target || !m_lifeWatch)
	{
		
		return;
	}
	if (m_isStatic)
	{
		m_bounds.SetCenter(Vec2(m_target->GetMIDPlayer().x, m_position.y));
		
	}
	else
	{
		if (m_isusingOBB)
		{
			Vec2 heroBox = m_target->GetMIDPlayer() + m_offset;
			float x = m_position.x;
			float y = m_position.y;
			float targetX = heroBox.x;
			float targetY = heroBox.y;
			float fraction = m_lifeWatch->GetElapsedFration();
			x = Interpolate(x, targetX, fraction);
			y = Interpolate(y, targetY, fraction);
			Vec2 ibasis = (heroBox - m_position).GetNormalized();
			m_oBounds.m_center = Vec2(x, y);
			if (m_isUsingRotate90Degrees)
			{
				ibasis.Rotate90Degrees();
				m_oBounds.m_iBasisNormal = ibasis;
			}
			else
			{
				m_oBounds.m_iBasisNormal = ibasis;
			}

		}
		else
		{
			Vec2 heroBox = m_target->GetMIDPlayer() + m_offset;
			float x = m_position.x;
			float y = m_position.y;
			float targetX = heroBox.x;
			float targetY = heroBox.y;
			float fraction = m_lifeWatch->GetElapsedFration();
			x = Interpolate(x, targetX, fraction);
			y = Interpolate(y, targetY, fraction);
			m_bounds.SetCenter(Vec2(x, y));
		}
		
	}


	
}

void SkillEffect::Render() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	float timeForOnce = Clock::GetSystemClock().GetTotalSeconds() - m_lifeWatch->m_startTime;
	AABB2 currentUV = m_animDef->GetSpriteDefAtTime(timeForOnce + m_animDelay).GetUVs();
	if (m_isusingOBB)
	{
		AddVertsForOBB2D(mainVerts, m_oBounds, Rgba8::WHITE, currentUV);
	}
	else
	{
		AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, currentUV.m_mins, currentUV.m_maxs);
	}
	g_theRenderer->BindTexture(&m_spritesheet->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void SkillEffect::EndFrame()
{

}

