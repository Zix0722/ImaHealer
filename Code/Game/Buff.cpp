#include "Buff.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "GameCommon.hpp"
#include "App.hpp"
#include "SkillEffect.hpp"
#include "Level.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/Texture.hpp"

Buff::Buff()
{

}

Buff::Buff(float CD, Texture* texture)
	:Skill(CD, texture)
{
	m_tooltipsBox.SetDimensions(Vec2(200.f, 200.f));
}

Buff::~Buff()
{

}

void Buff::Update(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	m_bounds.SetCenter(m_pos);
	m_tooltipsBox.SetPosAsTopLeft(m_pos);
	DoSkillLogic();
	if (m_CDWatch->HasDurationElapsed())
	{
		m_markForDelete = true;
	}

	Vec2 cursorUV = g_theWindow->GetCursorNormalizeUVs();
	Vec2 cursorPos = App::m_screenCamera->GetPosAtUV(cursorUV);
	float width = m_tooltipsBox.GetDimensions().x;
	float length = m_tooltipsBox.GetDimensions().y;
	if (m_pos.x < m_tooltipsBox.GetCenter().x) // on right side
	{
		if (m_pos.x + width >= SCREEN_CAMERA_SIZE_X)
		{
			if (m_pos.y < m_tooltipsBox.GetCenter().y) // on top
			{
				if (m_pos.y + length >= SCREEN_CAMERA_SIZE_Y)
				{
					m_tooltipsBox.SetPosAsTopRight(cursorPos);
				}
			}
			else if (m_pos.y > m_tooltipsBox.GetCenter().y) // on bottom
			{
				if (m_pos.y - length >= 0.f)
				{
					m_tooltipsBox.SetPosAsBottomRight(cursorPos);
				}
			}
		}
	}
	else if (m_pos.x > m_tooltipsBox.GetCenter().x) // on the left side 
	{
		if (m_pos.x + width >= SCREEN_CAMERA_SIZE_X)
		{
			if (m_pos.y < m_tooltipsBox.GetCenter().y) // on top
			{
				if (m_pos.y + length >= SCREEN_CAMERA_SIZE_Y)
				{
					m_tooltipsBox.SetPosAsTopLeft(cursorPos);
				}
			}
			else if (m_pos.y > m_tooltipsBox.GetCenter().y) // on bottom
			{
				if (m_pos.y - length >= 0.f)
				{
					m_tooltipsBox.SetPosAsBottomLeft(cursorPos);
				}
			}
		}
	}

	if (m_bounds.IsPointInside(cursorPos))
	{
		m_isCurserOnIcon = true;
		m_tooltipsBox.SetPosAsTopLeft(cursorPos);
	}
	else
	{
		m_isCurserOnIcon = false;
	}
}

void Buff::Render() const
{
	if (m_markForDelete)
	{
		return;
	}

	std::vector<Vertex_PCU> Verts;
	std::vector<Vertex_PCU> outlineVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);

	std::vector<Vertex_PCU> txtVerts;
	std::vector<Vertex_PCU> tintVerts;
	std::string str;
	float remainCD = m_CDWatch->m_duration - (Clock::GetSystemClock().GetTotalSeconds() - m_CDWatch->m_startTime);
	str = Stringf("%.f", remainCD);
	if (!m_CDWatch->HasDurationElapsed())
	{
		m_gameFont->AddVertsForTextInBox2D(txtVerts, m_bounds, 15.f, str, Rgba8(255, 50, 50), 0.7f);
		AddVertsForAABB2(tintVerts, m_bounds, Rgba8(0, 0, 0, 100));
		AddVertsForAABB2(Verts, m_bounds, Rgba8::WHITE);
	}
	else
	{
		AddVertsForAABB2(Verts, m_bounds, Rgba8::WHITE);
	}

	Rgba8 outlineColor = Rgba8::GREEN;
	if (m_isDebuff)
	{
		outlineColor = Rgba8::RED;
	}

	AddVertsForLineSegment2D(outlineVerts, m_bounds.GetPointAtUV(Vec2(0.1f, 0.1f)), m_bounds.GetPointAtUV(Vec2(0.1f, 0.9f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_bounds.GetPointAtUV(Vec2(0.1f, 0.1f)), m_bounds.GetPointAtUV(Vec2(0.9f, 0.1f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_bounds.GetPointAtUV(Vec2(0.9f, 0.9f)), m_bounds.GetPointAtUV(Vec2(0.1f, 0.9f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_bounds.GetPointAtUV(Vec2(0.9f, 0.9f)), m_bounds.GetPointAtUV(Vec2(0.9f, 0.1f)), 1.1f, outlineColor);

	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)Verts.size(), Verts.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)tintVerts.size(), tintVerts.data());
	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)outlineVerts.size(), outlineVerts.data());
}

void Buff::RenderToolTips() const
{
	std::vector<Vertex_PCU> bgVerts;
	std::vector<Vertex_PCU> iconVerts;
	std::vector<Vertex_PCU> descriptionBgVerts;
	std::vector<Vertex_PCU> NameTxtVerts;
	std::vector<Vertex_PCU> descriptionTxtVerts;
	std::vector<Vertex_PCU> outlineVerts;

	float width = m_tooltipsBox.GetDimensions().x;
	float length = m_tooltipsBox.GetDimensions().y;
	AABB2 nameLineBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.5f, m_tooltipsBox.m_mins.y + length * 0.6f),
							  Vec2(m_tooltipsBox.m_mins.x + width * 0.9f, m_tooltipsBox.m_mins.y + length * 0.9f));
	AABB2 descriptionBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.1f, m_tooltipsBox.m_mins.y + length * 0.1f),
							     Vec2(m_tooltipsBox.m_mins.x + width * 0.9f, m_tooltipsBox.m_mins.y + length * 0.5f));
	AABB2 iconBox = AABB2(Vec2(m_tooltipsBox.m_mins.x + width * 0.1f, m_tooltipsBox.m_mins.y + length * 0.6f),
						  Vec2(m_tooltipsBox.m_mins.x + width * 0.4f, m_tooltipsBox.m_mins.y + length * 0.9f));
	AddVertsForAABB2(bgVerts, m_tooltipsBox, Rgba8(4, 45, 56,230));
	AddVertsForAABB2(descriptionBgVerts, descriptionBox, Rgba8(250, 250, 0, 255));
	AddVertsForAABB2(iconVerts, iconBox, Rgba8(255,255,255,180));

	m_gameFont->AddVertsForTextInBox2D(NameTxtVerts, nameLineBox, 30.f, m_buffName, Rgba8(250, 250, 100), 0.7f);
	m_gameFont->AddVertsForTextInBox2D(descriptionTxtVerts, descriptionBox, 10.f, m_buffDescription, Rgba8(4, 45, 56, 255), 0.7f,Vec2(.5f,.5f));

	Rgba8 outlineColor = Rgba8(250, 250, 100);

	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.05f)), m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.95f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.05f)), m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.05f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.95f)), m_tooltipsBox.GetPointAtUV(Vec2(0.05f, 0.95f)), 1.1f, outlineColor);
	AddVertsForLineSegment2D(outlineVerts, m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.95f)), m_tooltipsBox.GetPointAtUV(Vec2(0.95f, 0.05f)), 1.1f, outlineColor);

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
	g_theRenderer->DrawVertexArray((int)descriptionBgVerts.size(), descriptionBgVerts.data());


	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexArray((int)iconVerts.size(), iconVerts.data());

	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)NameTxtVerts.size(), NameTxtVerts.data());
	g_theRenderer->DrawVertexArray((int)descriptionTxtVerts.size(), descriptionTxtVerts.data());

	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawVertexArray((int)outlineVerts.size(), outlineVerts.data());
}

void Buff::SetNameAndDescription(std::string const& name, std::string const& description)
{
	m_buffName = name;
	m_buffDescription = description;
}

void Buff::Refresh()
{
	//float duration = m_CDWatch->m_duration;
	m_CDWatch->Start();
}

Hero* Buff::GetCurrentTarget() const
{
	return nullptr;
}

void Buff::Cancel()
{

}

void Buff::SetTarget(Hero* hero)
{
	UNUSED(hero);
}

void Buff::SkillOnStart()
{

}


void Buff::DoSkillLogic()
{

}

void Buff::SpawnEffect()
{

}

HoT::HoT(float CD, Texture* texture, Hero* target)
	:Buff(CD, texture)
{
	SetNameAndDescription("Rejuvenation", "It will heal a \ncertain amount of HP\n each 2s");
	m_target = target;
	m_hotWatch = new Stopwatch(1.93f);
	m_hotWatch->Start();

	Texture* effectTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/HoT.png");
	SpriteSheet* sheet = new SpriteSheet(*effectTexture, IntVec2(8, 8));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 8, 15, 30.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->SetDuration(8.f);
	current->m_bounds.SetDimensions(Vec2(90.f, 90.f));
	current->m_position = m_target->GetMIDPlayer();
	current->m_isStatic = true;
	m_effect = current;
}

HoT::HoT()
{

}

HoT::~HoT()
{
	delete m_hotWatch;
}

void HoT::DoSkillLogic()
{
	if (m_hotWatch->HasDurationElapsed())
	{
		float rand = g_theRNG->RollRandomFloatInRange(160.f, 200.f);
		m_target->ChangeHpValue(rand);
		m_hotWatch->Start();
	}
}

void HoT::RefreshEffect()
{
	m_effect->m_lifeWatch->Start();
}

sheild::sheild()
{

}

sheild::sheild(float CD, Texture* texture, Hero* target)
	:Buff(CD, texture)
{
	SetNameAndDescription("Divine Shield", "invincible!");
	m_target = target;

	Texture* effectTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/divineShield.png");
	SpriteSheet* sheet = new SpriteSheet(*effectTexture, IntVec2(5, 3));
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 14, 30.f, SpriteAnimPlaybackType::PINGPONG, m_target);
	current->SetDuration(5.f);
	current->m_bounds.SetDimensions(Vec2(90.f, 90.f));
	current->m_position = m_target->GetMIDPlayer() + Vec2(0.f, 70.f);
	current->m_isStatic = true;
	m_effect = current;
}

sheild::~sheild()
{
	m_target->m_cannotBeDmg = false;
}

void sheild::DoSkillLogic()
{
	m_target->m_cannotBeDmg = true;
}

void sheild::RefreshEffect()
{
	m_effect->m_lifeWatch->Start();
}

Curse::Curse()
{

}

Curse::Curse(float CD, Texture* texture, Hero* target)
	:Buff(CD,texture)
{
	SetNameAndDescription("DeathCurse", "It periodically\n causes higher and\n higher damage");
	m_target = target;
	m_dotWatch = new Stopwatch(0.93f);
	m_dotWatch->Start();

	Texture* effectTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/purpleSmall.png");
	SpriteSheet* sheet = new SpriteSheet(*effectTexture, IntVec2(7, 6), 6.f / 7.f);
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 39, 60.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->SetDuration(15.f);
	current->m_bounds.SetDimensions(Vec2(390.f, 390.f));
	current->m_position = m_target->GetMIDPlayer();
	current->m_isStatic = true;
	m_effect = current;
}

Curse::~Curse()
{
	
}

void Curse::DoSkillLogic()
{
	if (m_dotWatch->HasDurationElapsed())
	{
		float rand = g_theRNG->RollRandomFloatInRange(20.f, 30.f);
		m_target->ChangeHpValue(-rand * scale);
		m_dotWatch->Start();
		scale++;
	}
}

void Curse::RefreshEffect()
{
	m_effect->m_lifeWatch->Start();
}

Taunt::Taunt()
{

}

Taunt::Taunt(float CD, Texture* texture, Hero* target)
	:Buff(CD, texture)
{
	SetNameAndDescription("Be provoked", "His attacks will\n mainly target on\n the Tank");
	m_target = target;

}

Taunt::~Taunt()
{

}

void Taunt::DoSkillLogic()
{
	m_target->SetCurrentTarget(m_user);
}

void Taunt::RefreshEffect()
{

}

Bleed::Bleed()
{

}

Bleed::Bleed(float CD, Texture* texture, Hero* target)
	:Buff(CD, texture)
{
	SetNameAndDescription("Bleed", "Enemies periodically\nlose health\nstackable");
	m_target = target;
	m_dotWatch = new Stopwatch(1.93f);
	m_dotWatch->Start();

	Texture* effectTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/NEw pack blood/5_100x100px.png");
	SpriteSheet* sheet = new SpriteSheet(*effectTexture, IntVec2(6, 5), 5.f / 6.f);
	SkillEffect* current = Level::GenerateSkillEffects(sheet, 0, 28, 60.f, SpriteAnimPlaybackType::LOOP, m_target);
	current->SetDuration(15.f);
	current->m_bounds.SetDimensions(Vec2(390.f, 390.f));
	current->m_position = m_target->GetMIDPlayer();
	current->m_isStatic = true;
	m_effect = current;
}

Bleed::~Bleed()
{

}

void Bleed::DoSkillLogic()
{
	if (m_dotWatch->HasDurationElapsed())
	{
		float rand = g_theRNG->RollRandomFloatInRange(100.f, 200.f);
		m_target->ChangeHpValue(-rand * m_stack);
		m_dotWatch->Start();
	}
}

void Bleed::RefreshEffect()
{
	m_effect->m_lifeWatch->Start();
	if (m_stack < 4)
	{
		m_stack++;
	}
}

Kicked::Kicked()
{

}

Kicked::Kicked(float CD, Texture* texture, Hero* target)
	:Buff(CD, texture)
{
	SetNameAndDescription("Kicked", "Enemies cannot\ncast any\nspells");
	m_target = target;

}

Kicked::~Kicked()
{

}

void Kicked::DoSkillLogic()
{
	m_target->GetCastBar()->Interupt();
}

void Kicked::RefreshEffect()
{

}
