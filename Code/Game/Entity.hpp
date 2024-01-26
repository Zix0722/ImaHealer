#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

class Game;

class Entity
{
public:
	Entity();
	Entity(Game* theGame, Vec3 startPos);
	Entity(Game* theGame, Vec2 startPos);
	virtual ~Entity();
	void virtual Update(float deltaSeconds) = 0;
	void virtual Render() const = 0;
	void virtual Die() = 0;;
	bool const IsOffscreen() const;
	bool const IsAlive() const;
	Vec2 GetForwardNormal() const;
	Vec2 GetVelocity() const;
	float GetOrientationDegrees() const;

private:
	Vec2	m_position;
	Vec2	m_velocity;
	float	m_orientationDegrees	= 0.f;
	float	m_angularVelocity		= 0.f;
	float	m_physicsRadius			= 5.f;
	float	m_cosmeticRadius		= 10.f;
	int		m_health				= 1;
	bool	m_isDead				= false;
	bool	m_isGarbage				= false;
	Game*	m_game					= nullptr;
};