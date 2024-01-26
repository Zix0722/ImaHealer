#pragma once
#include "CharDef.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec2.hpp"


class HPBar;
class CastBar;

class Hero
{
public:
	Hero();
	~Hero();

	void virtual Update(float deltaSeconds) = 0;
	void virtual Render() const = 0;
	void virtual RenderUI() const = 0;
	float virtual GetMaxHP() const = 0;
	float virtual GetPosX() const = 0;
	Vec2 virtual GetMIDPlayer() const = 0;
	Vec2 virtual GetHPBarCenter() const = 0;
	virtual CastBar* GetCastBar() const;
	virtual bool IsHeroDead() const = 0;
	virtual Hero* GetCurrentTarget() const = 0;
	virtual void SetCurrentTarget(Hero* target);
	void ChangeHpValue(float num);

	bool m_beHealing = false;
	bool m_cannotBeDmg = false;
	float m_currentHP = 0.f;
	float m_healingAmount = 0.f;
	bool m_isTargeting = false;
	AABB2 m_bounds;
	HPBar* m_hpBar = nullptr;
	Hero* m_neigbhorsNeighbor = nullptr;
	Hero** m_theParty = nullptr;
protected:
	CharDef* m_def = nullptr;
	AABB2 m_uvs;
};