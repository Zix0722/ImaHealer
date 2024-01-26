#include "HPBar.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Game/App.hpp"
#include "Game.hpp"
#include "Game/Level.hpp"
#include "Game/Healer.hpp"


HPBar::HPBar()
{
	m_bounds.SetDimensions(Vec2(500.f, 50.f));
	m_textBox.SetDimensions(Vec2(500.f, 50.f));
	m_spriteSheet[(int)HPSprite::base] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/base.png");
	m_spriteSheet[(int)HPSprite::green] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/green.png");
	m_spriteSheet[(int)HPSprite::red] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/red.png");
	m_spriteSheet[(int)HPSprite::frame] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/top.png");
}

HPBar::HPBar(Hero* owner)
	:m_owner(owner)
{
	m_bounds.SetDimensions(Vec2(300.f, 20.f));
	m_textBox.SetDimensions(Vec2(300.f, 20.f));
	m_spriteSheet[(int)HPSprite::base] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/base.png");
	m_spriteSheet[(int)HPSprite::green] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/green.png");
	m_spriteSheet[(int)HPSprite::red] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/red.png");
	m_spriteSheet[(int)HPSprite::frame] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/top.png");
}

HPBar::~HPBar()
{

}

void HPBar::Update()
{
	if (!m_isBoss)
	{
		m_bounds.SetDimensions(Vec2(300.f, 20.f));
		m_textBox.SetDimensions(Vec2(300.f, 20.f));
	}
	else
	{
		m_bounds.SetDimensions(Vec2(800.f, 40.f));
		m_textBox.SetDimensions(Vec2(800.f, 40.f));
	}

	m_bounds.SetCenter(m_pos);
	m_textBox.SetCenter(m_pos);
	if (m_owner->m_isTargeting)
	{
		m_bounds.SetCenter(m_pos + Vec2(50.f, 0.f));
		m_textBox.SetCenter(m_pos + Vec2(50.f, 0.f));
		m_bounds.SetDimensions(Vec2(360.f, 24.f));
		m_textBox.SetDimensions(Vec2(360.f, 24.f));
	}

	//buffs;
	float buffLineX = m_bounds.m_mins.x;
	float buffLineY = m_bounds.m_mins.y - 10.f;
	int renderIndex = 0;
	for (int buffIndex = 0; buffIndex < m_buffs.size(); buffIndex++)
	{
		Buff* current = m_buffs[buffIndex];
		
		if (current == nullptr)
		{
			continue;
		}

		buffLineX = m_bounds.m_mins.x + 20.f * (renderIndex) + 10.f;
		current->m_pos = Vec2(buffLineX, buffLineY);
		current->Update(0.f);
		renderIndex++;
	}

	Vec2 cursorUV = g_theWindow->GetCursorNormalizeUVs();
	Vec2 cursorPos = App::m_screenCamera->GetPosAtUV(cursorUV);

	if (m_bounds.IsPointInside(cursorPos) && Game::m_currentSetting.isUsingCurserControl)
	{
		if (g_theInput->WasKeyJustPressed(KEYCODE_LMB))
		{
			if (!m_owner->IsHeroDead())
			{
				App::m_theGame->m_currentLevel->m_player->m_currentTarget = m_owner;
			}
		}
	}
}

void HPBar::Render() const
{

	
	//base
	std::vector<Vertex_PCU> baseVerts;
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	AddVertsForAABB2(baseVerts, m_bounds, Rgba8::WHITE);
	g_theRenderer->BindTexture(m_spriteSheet[(int)HPSprite::base]);
	g_theRenderer->DrawVertexArray((int)baseVerts.size(), baseVerts.data());

	//green
	if (m_owner->m_beHealing)
	{
		std::vector<Vertex_PCU> greenVerts;
		AABB2 greenBox = m_bounds;
		float width = greenBox.m_maxs.x - greenBox.m_mins.x;
		float percentage = (m_owner->m_currentHP + m_owner->m_healingAmount) / m_owner->GetMaxHP();
		percentage = GetClamped(percentage, 0.f, 1.f);
		width = width * percentage;
		greenBox.m_maxs.x = width + greenBox.m_mins.x;

		AddVertsForAABB2(greenVerts, greenBox, Rgba8::WHITE);
		g_theRenderer->BindTexture(m_spriteSheet[(int)HPSprite::green]);
		g_theRenderer->DrawVertexArray((int)greenVerts.size(), greenVerts.data());
	}
	
	std::vector<Vertex_PCU> redVerts;
	AABB2 redBox = m_bounds;
	float width = redBox.m_maxs.x - redBox.m_mins.x;
	float percentage = (m_owner->m_currentHP) / m_owner->GetMaxHP();
	width = width * percentage;
	redBox.m_maxs.x = width + redBox.m_mins.x;

	AddVertsForAABB2(redVerts, redBox, Rgba8::WHITE);
	g_theRenderer->BindTexture(m_spriteSheet[(int)HPSprite::red]);
	g_theRenderer->DrawVertexArray((int)redVerts.size(), redVerts.data());

	//frame
	std::vector<Vertex_PCU> topVerts;
	if (!m_owner->m_isTargeting)
	{
		AddVertsForAABB2(topVerts, m_bounds, Rgba8::WHITE);
	}
	else
	{
		AddVertsForAABB2(topVerts, m_bounds, Rgba8::YELLOW);
	}

	g_theRenderer->BindTexture(m_spriteSheet[(int)HPSprite::frame]);
	g_theRenderer->DrawVertexArray((int)topVerts.size(), topVerts.data());

	//text
	std::vector<Vertex_PCU> txtVerts;
	std::string str;
	str = Stringf("%.f / %.f ", m_owner->m_currentHP, m_owner->GetMaxHP());
	if (!m_owner->m_isTargeting)
	{
		m_gameFont->AddVertsForTextInBox2D(txtVerts, m_textBox, 15.f, str, Rgba8(200, 200, 200), 0.7f);
	}
	else
	{
		m_gameFont->AddVertsForTextInBox2D(txtVerts, m_textBox, 15.f, str, Rgba8::YELLOW, 0.7f);
	}
	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());

	//button icon
	if (m_buttonTexture)
	{
		std::vector<Vertex_PCU> buttonVerts;
		AABB2 iconBox = AABB2(Vec2(0.f, 0.f), Vec2(20.f, 20.f));
		iconBox.SetCenter(m_bounds.GetCenter() + Vec2(-200.f, 0.f));
		AddVertsForAABB2(buttonVerts, iconBox, Rgba8::WHITE);
		g_theRenderer->BindTexture(m_buttonTexture);
		g_theRenderer->DrawVertexArray((int)buttonVerts.size(), buttonVerts.data());
	}
	

	for (int buffIndex = 0; buffIndex < m_buffs.size(); buffIndex++)
	{
		Buff* current = m_buffs[buffIndex];
		if (current)
		{
			current->Render();
		}
	}
}

void HPBar::SetDimensions(Vec2 dimension)
{
	m_bounds.SetDimensions(dimension);
	m_textBox.SetDimensions(dimension);
}



void HPBar::SetPosOnSlot(slot num)
{
	switch (num)
	{
	case slot::one:
		m_pos = Vec2(250.f, 700.f);
		break;
	case slot::two:
		m_pos = Vec2(250.f, 650.f);
		break;
	case slot::third:
		m_pos = Vec2(250.f, 600.f);
		break;
	case slot::four:
		m_pos = Vec2(250.f, 550.f);
		break;
	case slot::boss:
		m_pos = Vec2(800.f, 770.f);
		break;
	default:
		break;
	}
}

bool HPBar::HasTheBuff(std::string name)
{
	for (int buffIndex = 0; buffIndex < m_buffs.size(); buffIndex++)
	{
		Buff* current = m_buffs[buffIndex];
		if (current == nullptr)
		{
			continue;
		}
		if (current->m_buffName == name)
		{
			return true;
		}
		
	}
	return false;
}

void HPBar::SetButtonTextrue(Texture* texture)
{
	m_buttonTexture = texture;
}

