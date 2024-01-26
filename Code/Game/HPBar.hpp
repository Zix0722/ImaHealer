#pragma once
#include "Hero.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Buff.hpp"

enum class HPSprite
{
	base,
	green,
	red,
	frame,
	cnt,
};

enum class slot
{
	one,
	two,
	third,
	four,
	boss,
	cnt,
};

class HPBar
{
public:
	HPBar();
	HPBar(Hero* owner);
	~HPBar();

	void Update();
	void Render() const;
	void SetDimensions(Vec2 dimension);
	void SetPosOnSlot(slot num);

	bool HasTheBuff(std::string name);
	void SetButtonTextrue(Texture* texture);

public:
	Hero* m_owner = nullptr;
	Vec2 m_pos;
	AABB2 m_bounds;
	AABB2 m_textBox;
	Texture* m_spriteSheet[(int)HPSprite::cnt];
	std::vector<Buff*> m_buffs;
	bool m_isBoss = false;
	Texture* m_buttonTexture = nullptr;
};