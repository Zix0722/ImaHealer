#include "Boss.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/HPBar.hpp"
#include "Game/Level.hpp"
#include "BossSkillsSet.hpp"

Boss::Boss()
{
	
}

Boss::Boss(Level* lvl)
	:m_level(lvl)
{
	m_hpBar = new HPBar(this);
	m_hpBar->SetPosOnSlot(slot::boss);
	m_hpBar->m_isBoss = true;
	m_currentHP = m_maxHP;
	m_bounds = AABB2(Vec2(1200.f, 0.f), Vec2(0.f, 1200.f));
	m_GCD = new Stopwatch(1.5f);


	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Idle.png");
	m_spriteSheet[(int)bossAnim::Idle] = new SpriteSheet(*texture, IntVec2(8, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Jump.png");
	m_spriteSheet[(int)bossAnim::shadowArrow] = new SpriteSheet(*texture, IntVec2(2, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Run.png");
	m_spriteSheet[(int)bossAnim::deathCurse] = new SpriteSheet(*texture, IntVec2(8, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Take hit.png");
	m_spriteSheet[(int)bossAnim::GetHit] = new SpriteSheet(*texture, IntVec2(3, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Fall.png");
	m_spriteSheet[(int)bossAnim::Fall] = new SpriteSheet(*texture, IntVec2(2, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Death.png");
	m_spriteSheet[(int)bossAnim::Death] = new SpriteSheet(*texture, IntVec2(7, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Attack2.png");
	m_spriteSheet[(int)bossAnim::breakingStars] = new SpriteSheet(*texture, IntVec2(8, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Attack1.png");
	m_spriteSheet[(int)bossAnim::eradication] = new SpriteSheet(*texture, IntVec2(8, 1));


	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::Idle], 0, 7, 15.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::shadowArrow], 0, 1, 10.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::deathCurse], 0, 7, 8.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::GetHit], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::Fall], 0, 1, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::Death], 0, 6, 5.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::eradication], 0, 7, 7.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)bossAnim::breakingStars], 0, 7, 7.f, SpriteAnimPlaybackType::PINGPONG));

	m_castBar = new CastBar(this);
	m_skillGroup = new SkillGroup(this);

	ShadowArrow* skill = new ShadowArrow(5.f, nullptr);
	m_skillGroup->AddSkillInto(skill);
	skill->m_CDWatch->Start();
	skill->m_startAt = GetMIDPlayer() + Vec2(400.f, 0.f);
	DeathCurse* dot = new DeathCurse(6.f, nullptr);
	m_skillGroup->AddSkillInto(dot);
	dot->m_CDWatch->Start();
	Eradication* eradication = new Eradication(8.f, nullptr);
	m_skillGroup->AddSkillInto(eradication);
	eradication->m_CDWatch->Start();

	BreakingStars* star = new BreakingStars(4.f, nullptr);
	m_skillGroup->AddSkillInto(star);
	star->m_CDWatch->Start();
}

Boss::~Boss()
{

}

void Boss::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (m_currentHP == 0)
	{
		if (m_currentAnim != bossAnim::Death)
		{
			SetAnimation(bossAnim::Death, 99999999999999999999999.f);


			for (int buffIndex = 0; buffIndex < m_hpBar->m_buffs.size(); ++buffIndex)
			{
				Buff* current = m_hpBar->m_buffs[buffIndex];
				if (current)
				{
					if (current->m_isDebuff)
					{
						if (current->m_effect)
						{
							current->m_effect->m_needToDestory = true;
						}
						current->m_markForDelete = true;
					}
				}
			}
		}
		m_uvs = PlayAnimation(m_currentAnim);
		return;
	}
	m_castBar->Update(deltaSeconds);
	m_skillGroup->Update(deltaSeconds);
	Vec2 pos = Vec2(m_positionX, FLOOR_HEIGHT + 200.f);
	m_bounds.SetCenter(pos);
	UpdateAIInput(deltaSeconds);
	m_hpBar->Update();

	if (m_hpBar->m_buffs.size() > 0)
	{
		for (int index = 0; index < m_hpBar->m_buffs.size(); ++index)
		{
			Buff* current = m_hpBar->m_buffs[index];
			if (!current)
			{
				continue;
			}
			if (current->m_buffName == "Kicked")
			{
				delete m_animWatch;
				m_currentAnim = bossAnim::Idle;
				m_animWatch = nullptr;
			}
		}
	}


	if (m_animWatch)
	{
		if (m_animWatch->HasDurationElapsed())
		{
			delete m_animWatch;
			m_currentAnim = bossAnim::Idle;
			m_animWatch = nullptr;
		}
	}
	m_uvs = PlayAnimation(m_currentAnim);

}

void Boss::Render() const
{
	RenderChar();
	RenderSkillIcons();
}

void Boss::RenderUI() const
{
	RenderHPBars();
}

float Boss::GetMaxHP() const
{
	return m_maxHP;
}

float Boss::GetPosX() const
{
	return m_positionX;
}

Vec2 Boss::GetHPBarCenter() const
{
	return m_hpBar->m_bounds.GetCenter();
}

Hero* Boss::GetCurrentTarget() const
{
	return m_currentTarget;
}

void Boss::SetCurrentTarget(Hero* target)
{
	m_currentTarget = target;
}

Vec2 Boss::GetMIDPlayer() const
{
	return m_bounds.GetCenter() + Vec2(0.f, 0.f);
}

CastBar* Boss::GetCastBar() const
{
	return m_castBar;
}

void Boss::UpdateAIInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (!m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[0];
		if (skill->IsSkillReadyToUse())
		{
			SetRandomTarget();
			m_castBar->m_isDoLogicAfterHit = true;
			m_castBar->OnStart(skill, 2.f);
			skill->m_CDWatch->Start();
			SetAnimation(bossAnim::shadowArrow, 2.f);
		}
		else if (m_skillGroup->m_skills[1]->IsSkillReadyToUse())
		{
			SetRandomTarget();
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[1], 1.f);
			m_skillGroup->m_skills[1]->m_CDWatch->Start();
			SetAnimation(bossAnim::deathCurse, 1.f);
		}
		else if (m_skillGroup->m_skills[2]->IsSkillReadyToUse())
		{
			SetRandomTarget();
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[2], 1.5f);
			m_skillGroup->m_skills[2]->m_CDWatch->Start();
			SetAnimation(bossAnim::eradication, 1.5f);
		}
		else if (m_skillGroup->m_skills[3]->IsSkillReadyToUse())
		{
			SetRandomTarget();
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[3], 1.f);
			m_skillGroup->m_skills[3]->m_CDWatch->Start();
			SetAnimation(bossAnim::breakingStars, 1.f);
		}

		if (g_theInput->WasKeyJustPressed('H'))
		{
			SetRandomTarget();
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[3], 1.f);
			m_skillGroup->m_skills[3]->m_CDWatch->Start();
			SetAnimation(bossAnim::breakingStars, 1.f);
		}
	}
	else
	{
		
	}

}

AABB2 Boss::PlayAnimation(bossAnim const& anim)
{
	AABB2 result;
	float startTime = 0.f;
	if (m_animWatch)
	{
		startTime = m_animWatch->m_startTime;
	}
	float sec = Clock::GetSystemClock().GetTotalSeconds();
	switch (anim)
	{
	case bossAnim::Idle:
		result = m_animDefs[(int)bossAnim::Idle]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case bossAnim::shadowArrow:
		result = m_animDefs[(int)bossAnim::shadowArrow]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case bossAnim::eradication:
		result = m_animDefs[(int)bossAnim::eradication]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case bossAnim::breakingStars:
		result = m_animDefs[(int)bossAnim::breakingStars]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case bossAnim::deathCurse:
		result = m_animDefs[(int)bossAnim::deathCurse]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case bossAnim::Death:
		result = m_animDefs[(int)bossAnim::Death]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	default:
		break;
	}
	return result;
}

bool Boss::IsHeroDead() const
{
	if (m_currentHP == 0.f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Boss::RenderChar() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, m_uvs.m_mins, m_uvs.m_maxs);
	g_theRenderer->BindTexture(&m_spriteSheet[(int)m_currentAnim]->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void Boss::RenderHPBars() const
{
	m_hpBar->Render();
}

void Boss::RenderSkillIcons() const
{

}

void Boss::SetAnimation(bossAnim anim, float duration)
{
	m_currentAnim = anim;
	if (m_animWatch)
	{
		delete m_animWatch;
		m_animWatch = nullptr;
	}
	m_animWatch = new Stopwatch(duration);
	m_animWatch->Start();
}

void Boss::SetRandomTarget()
{
	
	bool anyoneLive = false;
	for (int i = 0; i < m_level->m_party.size(); i++)
	{
		if (!m_level->m_party[i]->IsHeroDead())
		{
			anyoneLive = true;
		}
	}
	if (!anyoneLive)
	{
		return;
	}

	if (m_hpBar->HasTheBuff("Be provoked") && !m_level->m_party[1]->IsHeroDead())
	{
		m_currentTarget = m_level->m_party[1];
		return;
	}

	int targetIndex = g_theRNG->RollRandomIntInRange(0, 5);
	if (targetIndex == 0 || targetIndex == 1 || targetIndex == 5)
	{
		if (!m_level->m_party[1]->IsHeroDead())
		{
			m_currentTarget = m_level->m_party[1];
		}
		else
		{
			SetRandomTarget();
		}
	}
	else if (targetIndex == 2)
	{
		if (!m_level->m_party[0]->IsHeroDead())
		{
			m_currentTarget = m_level->m_party[0];
		}
		else
		{
			SetRandomTarget();
		}
	}
	else if (targetIndex == 3)
	{
		if (!m_level->m_party[2]->IsHeroDead())
		{
			m_currentTarget = m_level->m_party[2];
		}
		else
		{
			SetRandomTarget();
		}
	}
	else if (targetIndex == 4)
	{
		if (!m_level->m_party[3]->IsHeroDead())
		{
			m_currentTarget = m_level->m_party[3];
		}
		else
		{
			SetRandomTarget();
		}
	}
}

