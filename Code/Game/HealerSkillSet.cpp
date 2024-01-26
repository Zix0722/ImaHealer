#include "HealerSkillSet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Level.hpp"
#include "HPBar.hpp"
#include "Buff.hpp"

FlashHeal::FlashHeal(float CD, Texture* texture)
	:Skill(CD, texture)
{
	m_buttonTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_q.png");
}

void FlashHeal::SkillOnStart()
{
	m_num = g_theRNG->RollRandomFloatInRange(300.f, 400.f);
	m_target->m_beHealing = true;
	m_target->m_healingAmount = m_num;
}

Hero* FlashHeal::GetCurrentTarget() const
{
	return m_target;
}

void FlashHeal::DoSkillLogic()
{
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_01.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);

	m_target->ChangeHpValue(m_num);
	m_target->m_beHealing = false;
	m_target->m_healingAmount = 0.f;

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/FLASHHEAL.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(5, 4));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 17, 20.f, SpriteAnimPlaybackType::ONCE, m_target);
	current->SetDuration(1.5f);
	current->m_bounds.SetDimensions(Vec2(180.f, 180.f));
	current->m_position = m_target->GetMIDPlayer();
	current->m_isStatic = true;

}

void FlashHeal::Cancel()
{
	m_target->m_beHealing = false;
	m_target->m_healingAmount = 0.f;
	m_target = nullptr;

}

void FlashHeal::SetTarget(Hero* hero)
{
	m_target = hero;
}

Rejuvenation::Rejuvenation(float CD, Texture* texture)
	:Skill(CD, texture)
{
	m_buttonTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_w.png");
}

void Rejuvenation::SkillOnStart()
{

}

Hero* Rejuvenation::GetCurrentTarget() const
{
	return m_target;
}

void Rejuvenation::DoSkillLogic()
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
			if (current->m_buffName == "Rejuvenation")
			{
				current->Refresh();
				current->RefreshEffect();
				SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_01.ogg");
				g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 2.f);
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/hot.png");
	HoT* newHoT = new HoT(8.f, texture, m_target);
	newHoT->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newHoT->m_CDWatch->Start();
	m_target->m_hpBar->m_buffs.push_back(newHoT);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_01.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 2.f);
}

void Rejuvenation::Cancel()
{
	m_target = nullptr;
}

void Rejuvenation::SetTarget(Hero* hero)
{
	m_target = hero;
}

Dispel::Dispel(float CD, Texture* texture)
	: Skill(CD, texture)
{
	m_buttonTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_e.png");
}

void Dispel::SkillOnStart()
{

}

Hero* Dispel::GetCurrentTarget() const
{
	return m_target;
}

void Dispel::DoSkillLogic()
{
	for (int buffIndex = 0; buffIndex < m_target->m_hpBar->m_buffs.size(); ++buffIndex)
	{
		Buff* current = m_target->m_hpBar->m_buffs[buffIndex];
		if (current)
		{
			if (current->m_isDebuff)
			{
				current->m_effect->m_needToDestory = true;
				current->m_markForDelete = true;
				
			}
		}
	}
	
	
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/cleanDebuff.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(5, 5));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 23, 23.f, SpriteAnimPlaybackType::ONCE, m_target);
	current->SetDuration(1.0f);
	current->m_bounds.SetDimensions(Vec2(130.f, 130.f));
	current->m_isStatic = true;
	current->m_position = m_target->GetMIDPlayer() + Vec2(0.f, 10.f);

	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_02.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

void Dispel::Cancel()
{
	m_target = nullptr;
}

void Dispel::SetTarget(Hero* hero)
{
	m_target = hero;
}

InfusionOfLight::InfusionOfLight(float CD, Texture* texture)
	: Skill(CD, texture)
{
	m_buttonTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_r.png");
}

void InfusionOfLight::SkillOnStart()
{
	m_num = g_theRNG->RollRandomFloatInRange(700.f, 1000.f);
	m_target->m_beHealing = true;
	m_target->m_healingAmount = m_num;
}

Hero* InfusionOfLight::GetCurrentTarget() const
{
	return m_target;
}

void InfusionOfLight::DoSkillLogic()
{
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_03.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
	m_target->ChangeHpValue(m_num);
	m_target->m_beHealing = false;
	m_target->m_healingAmount = 0.f;

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/bigHeal.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(9, 1));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 8, 12.f, SpriteAnimPlaybackType::ONCE, m_target);
	current->SetDuration(1.5f);
	current->m_bounds.SetDimensions(Vec2(100.f, 200.f));
	current->m_isStatic = true;
	current->m_position = m_target->GetMIDPlayer() + Vec2(0.f, 30.f);
}

void InfusionOfLight::Cancel()
{
	m_target->m_beHealing = false;
	m_target->m_healingAmount = 0.f;
	m_target = nullptr;
}

void InfusionOfLight::SetTarget(Hero* hero)
{
	m_target = hero;
}

DivineShield::DivineShield(float CD, Texture* texture)
	: Skill(CD, texture)
{
	m_buttonTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_t.png");
}

void DivineShield::SkillOnStart()
{

}

Hero* DivineShield::GetCurrentTarget() const
{
	return m_target;
}

void DivineShield::DoSkillLogic()
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
			if (current->m_buffName == "Divine Shield")
			{
				current->Refresh();
				current->RefreshEffect();
				SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_04.ogg");
				g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
				return;
			}
		}
	}
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/protect.png");
	sheild* newBuff = new sheild(5.f, texture, m_target);
	newBuff->m_bounds.SetDimensions(Vec2(20.f, 20.f));
	newBuff->m_CDWatch->Start();
	m_target->m_hpBar->m_buffs.push_back(newBuff);
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_04.ogg");
	g_theAudio->StartSound(soundEffect, false, 0.2f, 0.f, 1.f);
}

void DivineShield::Cancel()
{
	m_target = nullptr;
}

void DivineShield::SetTarget(Hero* hero)
{
	m_target = hero;
}
