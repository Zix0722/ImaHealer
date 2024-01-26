#include "TankSkillSet.hpp"
#include "Game/GameCommon.hpp"
#include "HPBar.hpp"

Slam::Slam(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Slam::~Slam()
{

}

void Slam::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(250.f, 320.f);
}

Hero* Slam::GetCurrentTarget() const
{
	return m_target;
}

void Slam::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1C.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

void Slam::SpawnEffect()
{

}

void Slam::Cancel()
{

}

void Slam::SetTarget(Hero* hero)
{
	m_target = hero;
}

Execute::Execute(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Execute::~Execute()
{

}

void Execute::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(550.f, 1050.f);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1A.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

Hero* Execute::GetCurrentTarget() const
{
	return m_target;
}

void Execute::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
}

void Execute::SpawnEffect()
{

}

void Execute::Cancel()
{

}

void Execute::SetTarget(Hero* hero)
{
	m_target = hero;
}

Enrage::Enrage(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Enrage::~Enrage()
{

}

void Enrage::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(310.f, 390.f);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1B.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

Hero* Enrage::GetCurrentTarget() const
{
	return m_target;
}

void Enrage::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
}

void Enrage::SpawnEffect()
{

}

void Enrage::Cancel()
{

}

void Enrage::SetTarget(Hero* hero)
{
	m_target = hero;
}

Shout::Shout(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Shout::~Shout()
{

}

void Shout::SkillOnStart()
{

}

Hero* Shout::GetCurrentTarget() const
{
	return m_target;
}

void Shout::DoSkillLogic()
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
			if (current->m_buffName == "Be provoked")
			{
				current->Refresh();
				current->RefreshEffect();
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/taunt.png");
	Taunt* newDoT = new Taunt(5.f, texture, m_target);
	newDoT->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newDoT->m_CDWatch->Start();
	newDoT->m_isDebuff = true;
	m_target->m_hpBar->m_buffs.push_back(newDoT);
}

void Shout::SpawnEffect()
{

}

void Shout::Cancel()
{

}

void Shout::SetTarget(Hero* hero)
{
	m_target = hero;
}
