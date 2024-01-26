#include "Healer.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/HPBar.hpp"
#include "Level.hpp"
#include "HealerSkillSet.hpp"

Healer::Healer()
{
	m_GCD = new Stopwatch(1.5f);
	m_hpBar = new HPBar(this);
	m_currentHP = m_maxHP;
	m_hpBar->SetPosOnSlot(slot::one);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/healer.png");
	m_spriteSheet = new SpriteSheet(*texture, IntVec2(10, 10));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 0, 9, 15.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 10, 19, 8.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 20, 29, 15.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 30, 39, 8.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 40, 49, 15.f, SpriteAnimPlaybackType::ONCE));


}

Healer::Healer(Level* lvl)
	:m_level(lvl)
{
	m_GCD = new Stopwatch(1.5f);
	m_hpBar = new HPBar(this);
	m_hpBar->SetButtonTextrue(g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_1.png"));
	m_currentHP = m_maxHP;
	m_hpBar->SetPosOnSlot(slot::one);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/healer.png");
	m_spriteSheet = new SpriteSheet(*texture, IntVec2(10, 10));

	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 0, 9, 15.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 10, 19, 6.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 10, 19, 20.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 30, 39, 20.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 30, 39, 7.f, SpriteAnimPlaybackType::PINGPONG));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 30, 39, 9.f, SpriteAnimPlaybackType::ONCE));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 20, 29, 15.f, SpriteAnimPlaybackType::LOOP));
	m_animDefs.emplace_back(new SpriteAnimDefinition(*m_spriteSheet, 40, 49, 9.f, SpriteAnimPlaybackType::ONCE));

	m_indicatorOnChar = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/arrow.png");
	m_indicatorSpritesheet = new SpriteSheet(*m_indicatorOnChar, IntVec2(5, 1));
	m_indicatorAnimDef = new SpriteAnimDefinition(*m_indicatorSpritesheet, 0, 4, 6.f, SpriteAnimPlaybackType::LOOP);

	m_skillGroup = new SkillGroup(this);
	m_skillGroup->m_pos = Vec2(650.f, 70.f);
	Texture* skillTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/smallHeal.png");
	Skill* newSkill = new FlashHeal(0.2f, skillTexture);
	newSkill->m_skillName = "Flash Heal";
	newSkill->m_skillDescription = "Heals the target in \nsmall increments with \nHoly Light";
	m_skillGroup->AddSkillInto(newSkill);

	skillTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/hot.png");
	newSkill = new Rejuvenation(5.f, skillTexture);
	newSkill->m_skillName = "Rejuvenation";
	newSkill->m_skillDescription = "Adds a heal over\n time that lasts 8 \nseconds";
	m_skillGroup->AddSkillInto(newSkill);

	skillTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/clean.png");
	newSkill = new Dispel(3.f, skillTexture);
	newSkill->m_skillName = "Dispel";
	newSkill->m_skillDescription = "Clears a Debuff \nfrom the target";
	m_skillGroup->AddSkillInto(newSkill);

	skillTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/bigHeal.png");
	newSkill = new InfusionOfLight(10.f, skillTexture);
	newSkill->m_skillName = "Infusion Of Light";
	newSkill->m_skillDescription = "Heals the target in \nbig increments with \nHoly Light";
	m_skillGroup->AddSkillInto(newSkill);


	skillTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/protect.png");
	newSkill = new DivineShield(15.f, skillTexture);
	newSkill->m_skillName = "Divine Shield";
	newSkill->m_skillDescription = "Protects the target for\n 5 seconds, keeping him\n from taking damage";
	m_skillGroup->AddSkillInto(newSkill);


	m_castBar = new CastBar(this);
	m_castBar->SetPos(Vec2(800.f, 140.f));
}

Healer::~Healer()
{

}

void Healer::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (m_currentHP == 0)
	{
		if(m_currentAnim != healAnim::die)
		SetAnimation(healAnim::die, 99999999999999999999999.f);

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
		m_skillGroup->Update(deltaSeconds);
		m_uvs = PlayAnimation(m_currentAnim);
		return;
	}

	while (m_currentTarget->IsHeroDead())
	{
		int currentIndex = -1;
		for (int i = 0; i < m_level->m_party.size(); ++i)
		{
			if (m_level->m_party[i] == m_currentTarget)
			{
				currentIndex = i + 1;
				if (currentIndex >= 4)
				{
					currentIndex = 0;
				}
				break;
			}
		}
		m_currentTarget = m_level->m_party[currentIndex];
	}
		
	m_castBar->Update(deltaSeconds);
	m_skillGroup->Update(deltaSeconds);
	//set target
	for (int i = 0; i < m_level->m_party.size(); i++)
	{
		m_level->m_party[i]->m_isTargeting = false;
	}
	m_currentTarget->m_isTargeting = true;


	m_currentHP = GetClamped(m_currentHP, 0.f, m_maxHP);
	Vec2 pos = Vec2(m_positionX, FLOOR_HEIGHT + 50.f);
	m_bounds.SetCenter(pos);

	m_hpBar->Update();
	if (m_animWatch)
	{
		if (m_animWatch->HasDurationElapsed())
		{
			delete m_animWatch;
			m_currentAnim = healAnim::Idle;
			m_animWatch = nullptr;
		}
	}
	m_uvs = PlayAnimation(m_currentAnim);

	
}


void Healer::Render() const
{
	RenderChar();
	RenderSkillIcons();
}

void Healer::RenderUI() const
{
	RenderHPBars();
	//target indicator on char
	std::vector<Vertex_PCU> indicatorVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	AABB2 box = AABB2(Vec2(200.f,200.f), Vec2::ZERO);
	Vec2 pos = Vec2(m_currentTarget->GetPosX(), FLOOR_HEIGHT * 1.8f);
	box.SetCenter(pos);
	AABB2 uv = m_indicatorAnimDef->GetSpriteDefAtTime(Clock::GetSystemClock().GetTotalSeconds()).GetUVs();
	AddVertsForAABB2(indicatorVerts, box, Rgba8::WHITE, uv.m_mins, uv.m_maxs);
	g_theRenderer->BindTexture(&m_indicatorSpritesheet->GetTexture());
	g_theRenderer->DrawVertexArray((int)indicatorVerts.size(), indicatorVerts.data());

	RenderSkillIcons();
	m_castBar->Render();
}

float Healer::GetMaxHP() const
{
	return m_maxHP;
}

float Healer::GetPosX() const
{
	return m_positionX - 15.f;
}

Vec2 Healer::GetHPBarCenter() const
{
	return m_hpBar->m_bounds.GetCenter();
}

Vec2 Healer::GetMIDPlayer() const
{
	return m_bounds.GetCenter() + Vec2(0.f, 0.f);
}

Hero* Healer::GetCurrentTarget() const
{
	return m_currentTarget;
}

void Healer::UpdateUserInput(float deltaSeconds)
{
	if (IsHeroDead())
	{
		return;
	}
	
	if (g_theInput->IsKeyDown('A'))
	{
		m_positionX -= deltaSeconds * HEALER_MOVING_SPEED;
		m_positionX = GetClamped(m_positionX, 50.f, SCREEN_CAMERA_SIZE_X - 50.f);
		m_currentAnim = healAnim::walk;
		m_castBar->Interupt();
	}
	if (g_theInput->IsKeyDown('D'))
	{
		m_positionX += deltaSeconds * HEALER_MOVING_SPEED;
		m_positionX = GetClamped(m_positionX, 50.f, SCREEN_CAMERA_SIZE_X * 0.5f - 50.f);
		m_currentAnim = healAnim::walk;
		m_castBar->Interupt();
	}
	if (g_theInput->WasKeyJustReleased('D') || g_theInput->WasKeyJustReleased('A'))
	{
		m_currentAnim = healAnim::Idle;
	}
	if (g_theInput->WasKeyJustPressed('1') && !m_level->m_party[0]->IsHeroDead())
	{
		m_currentTarget = m_level->GetHeroByType(HeroType::healer);
	}
	if (g_theInput->WasKeyJustPressed('2') && !m_level->m_party[1]->IsHeroDead())
	{
		m_currentTarget = m_level->GetHeroByType(HeroType::tank);
	}
	if (g_theInput->WasKeyJustPressed('3') && !m_level->m_party[2]->IsHeroDead())
	{
		m_currentTarget = m_level->GetHeroByType(HeroType::warrior);
	}
	if (g_theInput->WasKeyJustPressed('4') && !m_level->m_party[3]->IsHeroDead())
	{
		m_currentTarget = m_level->GetHeroByType(HeroType::mage);
	}

	if (g_theInput->WasKeyJustPressed('Q') && !m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[0];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->OnStart(skill, 1.5f);
			SetAnimation(healAnim::smallHeal, 1.5f);
		}
	}
	if (g_theInput->WasKeyJustPressed('W') && !m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[1];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->OnStart(skill, 0.5f);
			SetAnimation(healAnim::hot, 0.5f);
		}
	}
	if (g_theInput->WasKeyJustPressed('E') && !m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[2];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->OnStart(skill, 0.5f);
			SetAnimation(healAnim::clean, 0.5f);
		}
	}
	if (g_theInput->WasKeyJustPressed('R') && !m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[3];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->OnStart(skill, 2.5f);
			SetAnimation(healAnim::bigHeal, 2.5f);
		}
	}
	if (g_theInput->WasKeyJustPressed('T') && !m_castBar->IsChannelling())
	{
		Skill* skill = m_skillGroup->m_skills[4];
		if (skill->IsSkillReadyToUse())
		{
			m_castBar->OnStart(skill, 0.5f);
			SetAnimation(healAnim::protect, 0.5f);
		}
	}
}

AABB2 Healer::PlayAnimation(healAnim const& anim)
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
	case healAnim::Idle:
		result = m_animDefs[(int)healAnim::Idle]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case healAnim::smallHeal:
		result = m_animDefs[(int)healAnim::smallHeal]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case healAnim::hot:
		result = m_animDefs[(int)healAnim::hot]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case healAnim::clean:
		result = m_animDefs[(int)healAnim::clean]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case healAnim::bigHeal:
		result = m_animDefs[(int)healAnim::bigHeal]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case healAnim::protect:
		result = m_animDefs[(int)healAnim::protect]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	case healAnim::walk:
		result = m_animDefs[(int)healAnim::walk]->GetSpriteDefAtTime(sec).GetUVs();
		break;
	case healAnim::die:
		result = m_animDefs[(int)healAnim::die]->GetSpriteDefAtTime(sec - startTime).GetUVs();
		break;
	default:
		break;
	}
	return result;
}

void Healer::SetAnimation(healAnim const& anim, float duration)
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

bool Healer::IsHeroDead() const
{
	if (m_currentHP == 0.f)
	{
		return true;
	}
	return false;
}



void Healer::RenderChar() const
{
	std::vector<Vertex_PCU> mainVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	AddVertsForAABB2(mainVerts, m_bounds, Rgba8::WHITE, m_uvs.m_mins, m_uvs.m_maxs);
	g_theRenderer->BindTexture(&m_spriteSheet->GetTexture());
	g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
}

void Healer::RenderHPBars() const
{
	m_hpBar->Render();
}

void Healer::RenderSkillIcons() const
{
	m_skillGroup->Render();
}

