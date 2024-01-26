#include "Effect.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/Stopwatch.hpp"

Effect::Effect(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond /*= 20.f*/, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/)
{
	m_spritesheet = sheet;
	m_animDef = new SpriteAnimDefinition(*sheet, startSpriteIndex, endSpriteIndex, framesPerSecond, playbackType);
	m_lifeWatch = new Stopwatch(999999999.f);
	m_type = playbackType;
	m_lifeWatch->Start();
}

void Effect::SetSize(float width, float height)
{
	m_bounds = AABB2(Vec2(0.f, 0.f), Vec2(width, height));
}

void Effect::SetUpTime(float duration)
{
	if (m_lifeWatch)
	{
		delete m_lifeWatch;
		m_lifeWatch = nullptr;
	}
	m_lifeWatch = new Stopwatch(duration);
	m_lifeWatch->Start();
}

void Effect::SetUpMovements(Vec2 const& velocity, Vec2 const& dirction)
{
	m_dirction = dirction;
	m_velocity = velocity;

}

void Effect::Update(float deltaSeconds)
{
	if (m_lifeWatch)
	{
		if (m_lifeWatch->HasDurationElapsed())
		{
			m_needToDestory = true;
			return;
		}
	}
	m_bounds.SetCenter(m_position);
	m_position += m_dirction.GetNormalized() * m_velocity * deltaSeconds;
	
}

void Effect::Render() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	float timeForOnce = Clock::GetSystemClock().GetTotalSeconds() - m_lifeWatch->m_startTime;
	AABB2 currentUV = m_animDef->GetSpriteDefAtTime(timeForOnce + m_animDelay).GetUVs();
	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, currentUV.m_mins, currentUV.m_maxs);
	g_theRenderer->BindTexture(&m_spritesheet->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

