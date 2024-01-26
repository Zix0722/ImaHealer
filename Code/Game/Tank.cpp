#include "Tank.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/HPBar.hpp"
#include "TankSkillSet.hpp"

Tank::Tank(Hero* boss)
	:m_currentTarget(boss)
{
	m_hpBar = new HPBar(this);
	m_hpBar->SetPosOnSlot(slot::two);
	m_hpBar->SetButtonTextrue(g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_2.png"));
	m_currentHP = m_maxHP;
	m_bounds = AABB2(Vec2::ZERO, Vec2(400.f,400.f));
	m_GCD = new Stopwatch(1.5f);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Idle.png");
	m_spriteSheet[(int)tankAnim::Idle] = new SpriteSheet(*texture, IntVec2(10, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Jump.png");
	m_spriteSheet[(int)tankAnim::Jump] = new SpriteSheet(*texture, IntVec2(2, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Run.png");
	m_spriteSheet[(int)tankAnim::Run] = new SpriteSheet(*texture, IntVec2(6, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Get Hit.png");
	m_spriteSheet[(int)tankAnim::GetHit] = new SpriteSheet(*texture, IntVec2(3, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Fall.png");
	m_spriteSheet[(int)tankAnim::Fall] = new SpriteSheet(*texture, IntVec2(2, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Death.png");
	m_spriteSheet[(int)tankAnim::Death] = new SpriteSheet(*texture, IntVec2(9, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack3.png");
	m_spriteSheet[(int)tankAnim::Execute] = new SpriteSheet(*texture, IntVec2(5, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack2.png");
	m_spriteSheet[(int)tankAnim::Enrage] = new SpriteSheet(*texture, IntVec2(4, 1));

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack1.png");
	m_spriteSheet[(int)tankAnim::slam] = new SpriteSheet(*texture, IntVec2(4, 1));

	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Idle], 0, 9, 15.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Jump], 0, 1, 1.5f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Run], 0, 5, 8.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::GetHit], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Fall], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Death], 0, 8, 8.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::slam], 0, 3, 6.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Enrage], 0, 3, 6.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)tankAnim::Execute], 0, 4, 6.f, SpriteAnimPlaybackType::PINGPONG));

	m_castBar = new CastBar(this);
	m_skillGroup = new SkillGroup(this);

	Slam* slam = new Slam(3.f, nullptr);
	m_skillGroup->AddSkillInto(slam);
	slam->m_CDWatch->Start();

	Enrage* enrage = new Enrage(5.f, nullptr);
	m_skillGroup->AddSkillInto(enrage);
	enrage->m_CDWatch->Start();

	Execute* execute = new Execute(10.f, nullptr);
	m_skillGroup->AddSkillInto(execute);
	execute->m_CDWatch->Start();

	Shout* shout = new Shout(10.f, nullptr);
	m_skillGroup->AddSkillInto(shout);
	shout->m_CDWatch->Start();
}

Tank::~Tank()
{

}

void Tank::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (m_currentHP == 0)
	{
		if (m_currentAnim != tankAnim::Death)
		{
			SetAnimation(tankAnim::Death, 99999999999999999999999.f);
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
		m_currentAnim = tankAnim::Run;
		m_isMoving = true;
	}
	else
	{
		if(m_isMoving)
		{ 
			m_currentAnim = tankAnim::Idle;
			m_isMoving = false;
		}
	}

	Vec2 pos = Vec2(m_positionX, FLOOR_HEIGHT + 50.f);
	m_bounds.SetCenter(pos);
	UpdateAIInput(deltaSeconds);
	if (m_animWatch)
	{
		if (m_animWatch->HasDurationElapsed())
		{
			delete m_animWatch;
			m_currentAnim = tankAnim::Idle;
			m_animWatch = nullptr;
		}
	}
	m_uvs = PlayAnimation(m_currentAnim);
}

void Tank::Render() const
{
	RenderChar();
	RenderSkillIcons();
}

void Tank::RenderUI() const
{
	RenderHPBars();
}

float Tank::GetMaxHP() const
{
	return m_maxHP;
}

float Tank::GetPosX() const
{
	return m_positionX;
}

Vec2 Tank::GetHPBarCenter() const
{
	return m_hpBar->m_bounds.GetCenter();
}

Hero* Tank::GetCurrentTarget() const
{
	return m_currentTarget;
}

Vec2 Tank::GetMIDPlayer() const
{
	return m_bounds.GetCenter() + Vec2(0.f, 0.f);
}

void Tank::UpdateAIInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (!m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[0];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(skill, 0.6f);
			skill->m_CDWatch->Start();
			SetAnimation(tankAnim::slam, 0.6f);
		}
 		else if (m_skillGroup->m_skills[3]->IsSkillReadyToUse())
 		{
 			m_castBar->m_isDoLogicAfterHit = false;
 			m_castBar->OnStart(m_skillGroup->m_skills[3], 0.3f);
 			m_skillGroup->m_skills[3]->m_CDWatch->Start();
 			SetAnimation(tankAnim::Jump, 0.3f);
		}
		else if (m_skillGroup->m_skills[2]->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = false;
			m_castBar->OnStart(m_skillGroup->m_skills[2], 1.1f);
			m_skillGroup->m_skills[2]->m_CDWatch->Start();
			SetAnimation(tankAnim::Execute, 1.1f);
		}
 		else if (m_skillGroup->m_skills[1]->IsSkillReadyToUse())
 		{
 			m_castBar->m_isDoLogicAfterHit = false;
 			m_castBar->OnStart(m_skillGroup->m_skills[1], 0.7f);
 			m_skillGroup->m_skills[1]->m_CDWatch->Start();
 			SetAnimation(tankAnim::Enrage, 0.7f);
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


AABB2 Tank::PlayAnimation(tankAnim const& anim)
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
	case tankAnim::Idle:
		result = m_animDefs[(int)tankAnim::Idle]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case tankAnim::slam:
		result = m_animDefs[(int)tankAnim::slam]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case tankAnim::Enrage:
		result = m_animDefs[(int)tankAnim::Enrage]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case tankAnim::Death:
		result = m_animDefs[(int)tankAnim::Death]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case tankAnim::Execute:
		result = m_animDefs[(int)tankAnim::Execute]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case tankAnim::Run:
		result = m_animDefs[(int)tankAnim::Run]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	default:
		break;
	}
	return result;
}

bool Tank::IsHeroDead() const
{
	if (m_currentHP == 0.f)
	{
		return true;
	}
	return false;
}

void Tank::RenderChar() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, m_uvs.m_mins, m_uvs.m_maxs);
	g_theRenderer->BindTexture(&m_spriteSheet[(int)m_currentAnim]->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void Tank::RenderHPBars() const
{
	m_hpBar->Render();
}

void Tank::RenderSkillIcons() const
{

}

void Tank::SetAnimation(tankAnim param1, float param2)
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

