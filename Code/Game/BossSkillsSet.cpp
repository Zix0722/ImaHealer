#include "BossSkillsSet.hpp"
#include "Game/GameCommon.hpp"
#include "SkillEffect.hpp"
#include "Level.hpp"
#include "Buff.hpp"
#include "HPBar.hpp"

ShadowArrow::ShadowArrow(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

ShadowArrow::~ShadowArrow()
{
	
}

void ShadowArrow::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(250.f, 350.f);
}

Hero* ShadowArrow::GetCurrentTarget() const
{
	return m_target;
}

void ShadowArrow::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
}

void ShadowArrow::SpawnEffect()
{
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_01.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.7f, 0.f, 1.f);
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Dark VFX 1/shadowArrow.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(10, 2));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 9, 18.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->m_isusingOBB = true;
	current->m_position = m_startAt;
	current->SetDuration(1.0f);
	//current->m_bounds = AABB2(Vec2(130.f, 0.f), Vec2(0.f, 100.f));
	current->m_oBounds.m_halfDimensions = Vec2(65.f, 50.f);
	current->m_isStatic = false;
	current->m_skill = this;
}

void ShadowArrow::Cancel()
{

}

void ShadowArrow::SetTarget(Hero* hero)
{
	m_target = hero;
}

DeathCurse::DeathCurse(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

DeathCurse::~DeathCurse()
{

}

void DeathCurse::SkillOnStart()
{

}

Hero* DeathCurse::GetCurrentTarget() const
{
	return m_target;
}

void DeathCurse::DoSkillLogic()
{
	if (m_target->m_hpBar->m_buffs.size() > 0)
	{
		for (int index = 0; index < m_target->m_hpBar->m_buffs.size(); ++index)
		{
			Buff* current = m_target->m_hpBar->m_buffs[index];
			if (!current)
			{
				continue;
			}
			if (current->m_buffName == "DeathCurse")
			{
				current->Refresh();
				current->RefreshEffect();
				SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/CurseofTounges.ogg");
				g_theAudio->StartSound(soundEffect, false, 0.7f, 0.f, 1.f);
			
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/curse.png");
	Curse* newDoT = new Curse(15.f, texture, m_target);
	newDoT->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newDoT->m_CDWatch->Start();
	newDoT->m_isDebuff = true;
	m_target->m_hpBar->m_buffs.push_back(newDoT);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/CurseofTounges.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.7f, 0.f, 1.f);

}

void DeathCurse::SpawnEffect()
{

}

void DeathCurse::Cancel()
{

}

void DeathCurse::SetTarget(Hero* hero)
{
	m_target = hero;
}

Eradication::Eradication(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Eradication::~Eradication()
{

}

void Eradication::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(200.f, 336.f);
}

Hero* Eradication::GetCurrentTarget() const
{
	return m_target;
}

void Eradication::DoSkillLogic()
{
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_02.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.7f, 0.f, 1.f);
	m_target->ChangeHpValue(m_num);
	m_target->m_neigbhorsNeighbor->ChangeHpValue(m_num);

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Dark VFX 2/Dark VFX 2 (48x64).png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(16, 1));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 15, 15.f, SpriteAnimPlaybackType::ONCE, m_target);
	SkillEffect* current2 = Level::GenerateSkillEffects(sheet, 0, 15, 15.f, SpriteAnimPlaybackType::ONCE, m_target->m_neigbhorsNeighbor);
	current->SetDuration(1.f);
	current2->SetDuration(1.f);
	current->m_bounds.SetDimensions(Vec2(150.f, 200.f));
	current2->m_bounds.SetDimensions(Vec2(150.f, 200.f));
	current->m_isStatic = true;
	current2->m_isStatic = true;
	current->m_position = m_target->GetMIDPlayer() + Vec2(0.f, 35.f);
	current2->m_position = m_target->m_neigbhorsNeighbor->GetMIDPlayer() + Vec2(0.f, 35.f);
}

void Eradication::SpawnEffect()
{

}

void Eradication::Cancel()
{

}

void Eradication::SetTarget(Hero* hero)
{
	m_target = hero;
}

BreakingStars::BreakingStars(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

BreakingStars::~BreakingStars()
{

}

void BreakingStars::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(50.f, 120.f);
}

Hero* BreakingStars::GetCurrentTarget() const
{
	return m_target;
}

void BreakingStars::DoSkillLogic()
{
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_03.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.7f, 0.f, 1.f);
	Hero* t1 = m_target->m_theParty[0];
	Hero* t2 = m_target->m_theParty[1];
	Hero* t3 = m_target->m_theParty[2];
	Hero* t4 = m_target->m_theParty[3];
	
	t1->ChangeHpValue(m_num);
	t2->ChangeHpValue(m_num + g_theRNG->RollRandomFloatInRange(-20.f, 20.f));
	t3->ChangeHpValue(m_num + g_theRNG->RollRandomFloatInRange(-20.f, 20.f));
	t4->ChangeHpValue(m_num + g_theRNG->RollRandomFloatInRange(-20.f, 20.f));

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/purple_Effect.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(8, 8));
	SkillEffect* current1 = Level::GenerateSkillEffects(sheet, 16, 23, 15.f, SpriteAnimPlaybackType::ONCE, t1);
	SkillEffect* current2 = Level::GenerateSkillEffects(sheet, 16, 23, 15.f, SpriteAnimPlaybackType::ONCE, t2);
	SkillEffect* current3 = Level::GenerateSkillEffects(sheet, 16, 23, 15.f, SpriteAnimPlaybackType::ONCE, t3);
	SkillEffect* current4 = Level::GenerateSkillEffects(sheet, 16, 23, 15.f, SpriteAnimPlaybackType::ONCE, t4);

	current1->SetDuration(1.f);
	current2->SetDuration(1.f);
	current3->SetDuration(1.f);
	current4->SetDuration(1.f);
	current1->m_bounds.SetDimensions(Vec2(150.f, 150.f));
	current2->m_bounds.SetDimensions(Vec2(150.f, 150.f));
	current3->m_bounds.SetDimensions(Vec2(150.f, 150.f));
	current4->m_bounds.SetDimensions(Vec2(150.f, 150.f));
	current1->m_isStatic = true;
	current2->m_isStatic = true;
	current3->m_isStatic = true;
	current4->m_isStatic = true;
	current1->m_position = t1->GetMIDPlayer() + Vec2(0.f, 15.f);
	current2->m_position = t2->GetMIDPlayer() + Vec2(0.f, 15.f);
	current3->m_position = t3->GetMIDPlayer() + Vec2(0.f, 15.f);
	current4->m_position = t4->GetMIDPlayer() + Vec2(0.f, 15.f);
}

void BreakingStars::SpawnEffect()
{

}

void BreakingStars::Cancel()
{

}

void BreakingStars::SetTarget(Hero* hero)
{
	m_target = hero;
}
