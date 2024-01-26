#include "WarriorSkillSet.hpp"
#include "Game/GameCommon.hpp"
#include "HPBar.hpp"
#include "Game/Hero.hpp"
#pragma once

SinisterStrike::SinisterStrike(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

SinisterStrike::~SinisterStrike()
{

}

void SinisterStrike::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(330.f, 444.f);
}

Hero* SinisterStrike::GetCurrentTarget() const
{
	return m_target;
}

void SinisterStrike::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1A.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);

}

void SinisterStrike::SpawnEffect()
{

}

void SinisterStrike::Cancel()
{

}

void SinisterStrike::SetTarget(Hero* hero)
{
	m_target = hero;
}

Eviscerate::Eviscerate(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Eviscerate::~Eviscerate()
{

}

void Eviscerate::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(730.f, 1444.f);
}

Hero* Eviscerate::GetCurrentTarget() const
{
	return m_target;
}

void Eviscerate::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1B.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

void Eviscerate::SpawnEffect()
{

}

void Eviscerate::Cancel()
{

}

void Eviscerate::SetTarget(Hero* hero)
{
	m_target = hero;
}

Garrote::Garrote(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Garrote::~Garrote()
{

}

void Garrote::SkillOnStart()
{

}

Hero* Garrote::GetCurrentTarget() const
{
	return m_target;
}

void Garrote::DoSkillLogic()
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
			if (current->m_buffName == "Bleed")
			{
				current->Refresh();
				current->RefreshEffect();
				SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1C.ogg");
				g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/Ability_gouge.png");
	Bleed* newDoT = new Bleed(15.f, texture, m_target);
	newDoT->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newDoT->m_CDWatch->Start();
	newDoT->m_isDebuff = true;
	m_target->m_hpBar->m_buffs.push_back(newDoT);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1C.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

void Garrote::SpawnEffect()
{

}

void Garrote::Cancel()
{

}

void Garrote::SetTarget(Hero* hero)
{
	m_target = hero;
}

Kick::Kick(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Kick::~Kick()
{

}

void Kick::SkillOnStart()
{

}

Hero* Kick::GetCurrentTarget() const
{
	return m_target;
}

void Kick::DoSkillLogic()
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
			if (current->m_buffName == "Kicked")
			{
				current->Refresh();
				current->RefreshEffect();
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/ability_kick.jpg");
	Kicked* newDoT = new Kicked(1.5f, texture, m_target);
	newDoT->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newDoT->m_CDWatch->Start();
	newDoT->m_isDebuff = true;
	m_target->m_hpBar->m_buffs.push_back(newDoT);
}

void Kick::SpawnEffect()
{

}

void Kick::Cancel()
{

}

void Kick::SetTarget(Hero* hero)
{
	m_target = hero;
}
