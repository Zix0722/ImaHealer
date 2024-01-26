#include "MageSkillSet.hpp"
#include "Game/GameCommon.hpp"
#include "SkillEffect.hpp"
#include "Level.hpp"
#include "Buff.hpp"
#include "HPBar.hpp"

FireBall::FireBall(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

FireBall::~FireBall()
{

}

void FireBall::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(1050.f, 1550.f);
}

Hero* FireBall::GetCurrentTarget() const
{
	return m_target;
}

void FireBall::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
}

void FireBall::SpawnEffect()
{
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/redNormal.png.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(9, 8), 8.f/9.f);
	SkillEffect* current = Level::GenerateSkillEffects(sheet,0 , 64, 60.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->m_isusingOBB = true;
	current->m_offset = Vec2(-80.f, -0.f);
	current->m_isUsingRotate90Degrees = true;
	current->m_position = m_startAt;
	current->SetDuration(2.3f);
	//current->m_bounds = AABB2(Vec2(130.f, 0.f), Vec2(0.f, 100.f));
	current->m_oBounds.m_halfDimensions = Vec2(105.f, 150.f);
	current->m_isStatic = false;
	current->m_skill = this;
	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/fireball.ogg");
	g_theAudio->StartSound(soundEffect, false, 1.f, 0.f, 1.f);
}

void FireBall::Cancel()
{

}

void FireBall::SetTarget(Hero* hero)
{
	m_target = hero;
}

Blizzard::Blizzard(float CD, Texture* texture)
	:Skill(CD, texture)
{

}

Blizzard::~Blizzard()
{

}

void Blizzard::SkillOnStart()
{
	m_num = -g_theRNG->RollRandomFloatInRange(150.f, 250.f);
}

Hero* Blizzard::GetCurrentTarget() const
{
	return m_target;
}

void Blizzard::DoSkillLogic()
{
	m_target->ChangeHpValue(m_num);
}

void Blizzard::SpawnEffect()
{
	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/blue.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(9, 8), 8.f / 9.f);
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 64, 60.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->m_offset = Vec2(-150.f, -150.f);
	current->m_isusingOBB = true;
	current->m_isUsingRotate90Degrees = true;
	current->m_position = m_startAt;
	current->SetDuration(1.3f);
	//current->m_bounds = AABB2(Vec2(130.f, 0.f), Vec2(0.f, 100.f));
	current->m_oBounds.m_halfDimensions = Vec2(105.f, 150.f);
	current->m_isStatic = false;
	current->m_skill = this;

	SoundID soundEffect = g_theAudio->CreateOrGetSound("Data/Audio/iceball.wav");
	g_theAudio->StartSound(soundEffect, false, 1.f, 0.f, 1.f);
}

void Blizzard::Cancel()
{

}

void Blizzard::SetTarget(Hero* hero)
{
	m_target = hero;
}
