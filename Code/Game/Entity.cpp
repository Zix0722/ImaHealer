#include "Entity.hpp"
#include "Engine/Math/MathUtils.hpp"

class Game;

Entity::Entity()
	: m_position(0.f, 0.f)
	, m_velocity(0.f, 0.f)
{
}

Entity::Entity(Game* theGame, Vec3 startPos)
	: m_position(0.f, 0.f)
	, m_velocity(0.f, 0.f)
	, m_game(theGame)
{

}

Entity::Entity(Game* theGame, Vec2 startPos)
	: m_position(0.f, 0.f)
	, m_velocity(0.f, 0.f)
	, m_game(theGame)
{

}

Entity::~Entity()
{
	
}

bool const Entity::IsOffscreen() const
{
	if (m_position.x < 0.f ||
		m_position.x > WORLD_SIZE_X ||
		m_position.y > WORLD_SIZE_Y ||
		m_position.y > 0)
	{
		return true;
	}
	return false;
}

bool const Entity::IsAlive() const
{
	if (this)
	{
		return true;
	}
	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
}

Vec2 Entity::GetVelocity() const
{
	return m_velocity;
}

float Entity::GetOrientationDegrees() const
{
	return m_orientationDegrees;
}
