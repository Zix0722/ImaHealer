#include "Warrior.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/HPBar.hpp"
#include "CastBar.hpp"
#include "SkillGroup.hpp"
#include "WarriorSkillSet.hpp"

Warrior::Warrior(Hero* boss)
	:m_currentTarget(boss)
{
	m_hpBar = new HPBar(this);
	m_hpBar->SetPosOnSlot(slot::third);
	m_hpBar->SetButtonTextrue(g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_3.png"));
	m_currentHP = m_maxHP;
	m_bounds = AABB2(Vec2::ZERO, Vec2(400.f,400.f));
	m_GCD = new Stopwatch(1.5f);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Idle.png");
	m_spriteSheet[(int)warriorAnim::Idle] = new SpriteSheet(*texture, IntVec2(8, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Jump.png");
	m_spriteSheet[(int)warriorAnim::Jump] = new SpriteSheet(*texture, IntVec2(2, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Run.png");
	m_spriteSheet[(int)warriorAnim::Run] = new SpriteSheet(*texture, IntVec2(8, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Get Hit.png");
	m_spriteSheet[(int)warriorAnim::GetHit] = new SpriteSheet(*texture, IntVec2(3, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Fall.png");
	m_spriteSheet[(int)warriorAnim::Fall] = new SpriteSheet(*texture, IntVec2(2, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Death.png");
	m_spriteSheet[(int)warriorAnim::Death] = new SpriteSheet(*texture, IntVec2(6, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack3.png");
	m_spriteSheet[(int)warriorAnim::Garrote] = new SpriteSheet(*texture, IntVec2(4, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack2.png");
	m_spriteSheet[(int)warriorAnim::Eviscerate] = new SpriteSheet(*texture, IntVec2(4, 1), 1.7f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack1.png");
	m_spriteSheet[(int)warriorAnim::sinisterStrike] = new SpriteSheet(*texture, IntVec2(4, 1), 1.7f);

	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Idle], 0, 7, 10.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Jump], 0, 1, 1.5f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Run], 0, 7, 8.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::GetHit], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Fall], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Death], 0, 5, 5.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::sinisterStrike], 0, 3, 5.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Eviscerate], 0, 3, 5.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)warriorAnim::Garrote], 0, 3, 5.f, SpriteAnimPlaybackType::ONCE));

	m_castBar = new CastBar(this);
	m_skillGroup = new SkillGroup(this);

	SinisterStrike* sini = new SinisterStrike(1.5f, nullptr);
	m_skillGroup->AddSkillInto(sini);
	sini->m_CDWatch->Start();

	Eviscerate* evis = new Eviscerate(8.f, nullptr);
	m_skillGroup->AddSkillInto(evis);
	evis->m_CDWatch->Start();

	Garrote* garrote = new Garrote(12.f, nullptr);
	m_skillGroup->AddSkillInto(garrote);
	garrote->m_CDWatch->Start();

	Kick* kick = new Kick(20.f, nullptr);
	m_skillGroup->AddSkillInto(kick);
	kick->m_CDWatch->Start();
}

Warrior::~Warrior()
{

}

void Warrior::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_currentHP == 0)
	{
		if (m_currentAnim != warriorAnim::Death)
		{
			SetAnimation(warriorAnim::Death, 99999999999999999999999.f);


			for (int buffIndex = 0; buffIndex < m_hpBar->m_buffs.size(); ++buffIndex)
			{
				Buff* current = m_hpBar->m_buffs[buffIndex];
				if (current)
				{
					if (current->m_isDebuff)
					{
						current->m_effect->m_needToDestory = true;
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
	m_hpBar->Update();
	if (m_positionX <= m_TargetPosX)
	{
		m_positionX += deltaSeconds * 150.f;
		m_currentAnim = warriorAnim::Run;
		m_isMoving = true;
	}
	else
	{
		if (m_isMoving)
		{
			m_currentAnim = warriorAnim::Idle;
			m_isMoving = false;
		}
	}

	Vec2 pos = Vec2(m_positionX, FLOOR_HEIGHT + 20.f);
	m_bounds.SetCenter(pos);
	UpdateAIInput(deltaSeconds);
	if (m_animWatch)
	{
		if (m_animWatch->HasDurationElapsed())
		{
			delete m_animWatch;
			m_currentAnim = warriorAnim::Idle;
			m_animWatch = nullptr;
		}
	}
	m_uvs = PlayAnimation(m_currentAnim);	
}


void Warrior::UpdateAIInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (!m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[2];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(skill, 0.6f);
			skill->m_CDWatch->Start();
			SetAnimation(warriorAnim::Garrote, 0.6f);
		}
		else if (m_skillGroup->m_skills[3]->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[3], 0.3f);
			m_skillGroup->m_skills[3]->m_CDWatch->Start();
			SetAnimation(warriorAnim::Jump, 0.3f);
		}
		else if (m_skillGroup->m_skills[1]->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[1], 1.1f);
			m_skillGroup->m_skills[1]->m_CDWatch->Start();
			SetAnimation(warriorAnim::Eviscerate, 1.1f);
		}
		else if (m_skillGroup->m_skills[0]->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[0], 0.7f);
			m_skillGroup->m_skills[0]->m_CDWatch->Start();
			SetAnimation(warriorAnim::sinisterStrike, 0.7f);
		}
		// 
		// 		if (g_theInput->WasKeyJustPressed('H'))
		// 		{
		// 			SetRandomTarget();
		// 			m_castBar->m_isDoLogicAfterHit = false;
		// 			m_castBar->OnStart(m_skillGroup->m_skills[3], 1.f);
		// 			m_skillGroup->m_skills[3]->m_CDWatch->Start();
		// 			SetAnimation(bossAnim::breakingStars, 1.f);
		// 		}
	}
}

void Warrior::Render() const
{
	RenderChar();
	RenderSkillIcons();
}

void Warrior::RenderUI() const
{
	RenderHPBars();
}

float Warrior::GetMaxHP() const
{
	return m_maxHP;
}

float Warrior::GetPosX() const
{
	return m_positionX;
}

Vec2 Warrior::GetHPBarCenter() const
{
	return m_hpBar->m_bounds.GetCenter();
}

Vec2 Warrior::GetMIDPlayer() const
{
	return m_bounds.GetCenter() + Vec2(15.f,30.f);
}

Hero* Warrior::GetCurrentTarget() const
{
	return m_currentTarget;
}

void Warrior::UpdateUserInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

AABB2 Warrior::PlayAnimation(warriorAnim const& anim)
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
	case warriorAnim::Idle:
		result = m_animDefs[(int)warriorAnim::Idle]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case warriorAnim::sinisterStrike:
		result = m_animDefs[(int)warriorAnim::sinisterStrike]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case warriorAnim::Eviscerate:
		result = m_animDefs[(int)warriorAnim::Eviscerate]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case warriorAnim::Garrote:
		result = m_animDefs[(int)warriorAnim::Garrote]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case warriorAnim::Run:
		result = m_animDefs[(int)warriorAnim::Run]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case warriorAnim::Death:
		result = m_animDefs[(int)warriorAnim::Death]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	default:
		break;
	}
	return result;
}

bool Warrior::IsHeroDead() const
{
	if (m_currentHP == 0.f)
	{
		return true;
	}
	return false;
}

void Warrior::RenderChar() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, m_uvs.m_mins, m_uvs.m_maxs);
	g_theRenderer->BindTexture(&m_spriteSheet[(int)m_currentAnim]->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void Warrior::RenderHPBars() const
{
	m_hpBar->Render();
}

void Warrior::RenderSkillIcons() const
{

}

void Warrior::SetAnimation(warriorAnim param1, float param2)
{
	m_currentAnim = param1;
	if (m_animWatch)
	{
		delete m_animWatch;
		m_animWatch = nullptr;
	}
	m_animWatch = new Stopwatch(param2);
	m_animWatch->Start();
}

