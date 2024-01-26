#include "Hero.hpp"
#include "FloatingNumber.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Level.hpp"
#include "Engine/Core/EngineCommon.hpp"

Hero::Hero()
{
	m_bounds = AABB2(Vec2(0.f, 0.f), Vec2(100.f, 100.f));
}

Hero::~Hero()
{

}

CastBar* Hero::GetCastBar() const
{
	return nullptr;
}

void Hero::SetCurrentTarget(Hero* target)
{
	UNUSED(target);
}

void Hero::ChangeHpValue(float num)
{
	FloatingNumType type;
	if (num > 0.f)
	{
		type = FloatingNumType::Heal;
		m_currentHP += num;
		
	}
	else
	{
		type = FloatingNumType::Damage;
		if (m_cannotBeDmg)
		{
			num = 0.f;
		}
		m_currentHP -= fabsf(num);
	}
	

	m_currentHP = GetClamped(m_currentHP, 0.f, GetMaxHP());
	Level::GenerateFloatingNum(num, type, GetMIDPlayer());
}

