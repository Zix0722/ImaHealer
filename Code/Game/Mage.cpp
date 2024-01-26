#include "Mage.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/HPBar.hpp"
#include "MageSkillSet.hpp"

Mage::Mage(Hero* boss)
	:m_currentTarget(boss)
{
	m_hpBar = new HPBar(this);
	m_currentHP = m_maxHP;
	m_hpBar->SetPosOnSlot(slot::four);
	m_hpBar->SetButtonTextrue(g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_4.png"));
	m_bounds = AABB2(Vec2::ZERO, Vec2(400.f,400.f));
	m_GCD = new Stopwatch(1.5f);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Idle.png");
	m_spriteSheet[(int)mageAnim::Idle] = new SpriteSheet(*texture, IntVec2(8, 1), 1.2f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Run.png");
	m_spriteSheet[(int)mageAnim::Run] = new SpriteSheet(*texture, IntVec2(6, 1), 1.2f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Get Hit.png");
	m_spriteSheet[(int)mageAnim::GetHit] = new SpriteSheet(*texture, IntVec2(3, 1), 1.2f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Death.png");
	m_spriteSheet[(int)mageAnim::Death] = new SpriteSheet(*texture, IntVec2(5, 1), 1.2f);

	texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Attack.png");
	m_spriteSheet[(int)mageAnim::Attack] = new SpriteSheet(*texture, IntVec2(8, 1), 1.2f);

	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::Idle], 0, 7, 10.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::Run], 0, 5, 8.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::GetHit], 0, 2, 3.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::Death], 0, 4, 4.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::Attack], 0, 7, 6.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet[(int)mageAnim::Blizzard], 0, 1, 6.f, SpriteAnimPlaybackType::PINGPONG));

	m_castBar = new CastBar(this);
	m_skillGroup = new SkillGroup(this);

	FireBall* fire = new FireBall(3.0f, nullptr);
	m_skillGroup->AddSkillInto(fire);
	fire->m_CDWatch->Start();
	fire->m_startAt = GetMIDPlayer() + Vec2(120.f,-0.f);

	Blizzard* ice = new Blizzard(3.0f, nullptr);
	m_skillGroup->AddSkillInto(ice);
	ice->m_CDWatch->Start();
	ice->m_startAt = GetMIDPlayer() + Vec2(120.f, -0.f);
}

Mage::~Mage()
{

}

void Mage::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);

	if (m_currentHP == 0)
	{
		if (m_currentAnim != mageAnim::Death)
		{
			SetAnimation(mageAnim::Death, 99999999999999999999999.f);


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
	Vec2 pos = Vec2(m_positionX, FLOOR_HEIGHT + 20.f);
	m_bounds.SetCenter(pos);
	UpdateAIInput(deltaSeconds);
	if (m_animWatch)
	{
		if (m_animWatch->HasDurationElapsed())
		{
			delete m_animWatch;
			m_currentAnim = mageAnim::Idle;
			m_animWatch = nullptr;
		}
	}
	m_uvs = PlayAnimation(m_currentAnim);
}

void Mage::Render() const
{
	RenderChar();
	RenderSkillIcons();
}

void Mage::RenderUI() const
{
	RenderHPBars();
}

float Mage::GetMaxHP() const
{
	return m_maxHP;
}

float Mage::GetPosX() const
{
	return m_positionX - 15.f;
}

Vec2 Mage::GetHPBarCenter() const
{
	return m_hpBar->m_bounds.GetCenter();
}

Vec2 Mage::GetMIDPlayer() const
{
	return m_bounds.GetCenter() + Vec2(0.f, 35.f);
}

Hero* Mage::GetCurrentTarget() const
{
	return m_currentTarget;
}

void Mage::UpdateUserInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

AABB2 Mage::PlayAnimation(mageAnim const& anim)
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
	case mageAnim::Idle:
		result = m_animDefs[(int)mageAnim::Idle]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case mageAnim::Attack:
		result = m_animDefs[(int)mageAnim::Attack]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case mageAnim::Blizzard:
		result = m_animDefs[(int)mageAnim::Blizzard]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case mageAnim::Run:
		result = m_animDefs[(int)mageAnim::Run]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case mageAnim::Death:
		result = m_animDefs[(int)mageAnim::Death]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	default:
		break;
	}
	return result;
}

bool Mage::IsHeroDead() const
{
	if (m_currentHP == 0.f)
	{
		return true;
	}
	return false;
}

void Mage::UpdateAIInput(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (!m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[0];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = true;
			m_castBar->m_isChannelingMode = false;
			m_castBar->OnStart(skill, 2.0f);
			skill->m_CDWatch->Start();
			SetAnimation(mageAnim::Attack, 2.0f);
		}
		else if (m_skillGroup->m_skills[1]->IsSkillReadyToUse())
		{
			m_castBar->m_isDoLogicAfterHit = true;
			m_castBar->m_isChannelingMode = true;
			m_castBar->OnStart(m_skillGroup->m_skills[1], 3.0f);
			m_skillGroup->m_skills[1]->m_CDWatch->Start();
			SetAnimation(mageAnim::Attack, 3.0f);
		}
	}
}

void Mage::RenderChar() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, m_uvs.m_mins, m_uvs.m_maxs);
	g_theRenderer->BindTexture(&m_spriteSheet[(int)m_currentAnim]->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void Mage::RenderHPBars() const
{
	m_hpBar->Render();
}

void Mage::RenderSkillIcons() const
{

}

void Mage::SetAnimation(mageAnim param1, float param2)
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

