#include "Level.hpp"
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "GameCommon.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Game/Healer.hpp"
#include "Tank.hpp"
#include "Warrior.hpp"
#include "Mage.hpp"
#include "HPBar.hpp"
#include "Boss.hpp"
#include "Engine/Math/MathUtils.hpp"

std::vector<FloatingNumber*> Level::m_numbers;
std::vector<SkillEffect*> Level::m_skillEffects;

floorBox::floorBox(Level* level)
	:m_level(level)
{
	m_Watch = new Stopwatch(3.f);
	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Gravity-SpriteSheet.png");
 	SpriteSheet* sheet = new SpriteSheet(*m_texture, IntVec2(4, 5));
 	m_effect = new Effect(sheet, 0, 19, 20.f, SpriteAnimPlaybackType::ONCE);
}

floorBox::~floorBox()
{
	//delete m_Watch;
	//delete m_effect;
}

void floorBox::Update()
{
	if (m_Watch->GetElapsedFration() > 0.8f)
	{
		if (m_didDmg)
		{
			return;
		}
		if (m_level->m_player->GetPosX() >= m_bound.m_mins.x && m_level->m_player->GetPosX() <= m_bound.m_maxs.x)
		{
			m_didDmg = true;
			m_level->m_player->ChangeHpValue(-666.f);
		}
	}
}

void floorBox::Render() const
{
	if (m_Watch->GetElapsedFration() < 0.5f)
	{
		unsigned char A = 50;
		float time = Clock::GetSystemClock().GetTotalSeconds();
		float lerp = sinf(15.f * time);
		A = static_cast<unsigned char>(RangeMap(lerp, -1.f,1.f,0.f, 50.f));
		Rgba8 currentColor = Rgba8(138, 43, 226, A);
		std::vector<Vertex_PCU> mainVerts;
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);

		AddVertsForAABB2(mainVerts, m_warningArea, currentColor);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
	}
	else if (m_Watch->GetElapsedFration() < 0.7f)
	{
		Rgba8 currentColor = Rgba8(138, 43, 226, 50);
		std::vector<Vertex_PCU> mainVerts;
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);

		AddVertsForAABB2(mainVerts, m_warningArea, currentColor);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());
	}
	else if (m_Watch->GetElapsedFration() < 1.f)
	{
		Rgba8 currentColor = Rgba8(138, 43, 226, 50);
		std::vector<Vertex_PCU> mainVerts;
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);

		AddVertsForAABB2(mainVerts, m_warningArea, currentColor);
		g_theRenderer->BindTexture(nullptr);
		g_theRenderer->DrawVertexArray((int)mainVerts.size(), mainVerts.data());

		std::vector<Vertex_PCU> EffectVerts;
		g_theRenderer->SetBlendMode(BlendMode::ALPHA);
		float timeForOnce = Clock::GetSystemClock().GetTotalSeconds() - (m_Watch->m_startTime + 3.f * 0.7f);
		AABB2 currentUV = m_effect->m_animDef->GetSpriteDefAtTime(timeForOnce).GetUVs();
		AddVertsForAABB2(EffectVerts, m_bound, Rgba8::WHITE, currentUV.m_mins, currentUV.m_maxs);
		g_theRenderer->BindTexture(&m_effect->m_spritesheet->GetTexture());
		g_theRenderer->DrawVertexArray((int)EffectVerts.size(), EffectVerts.data());

	}
}

void floorBox::OnStart()
{
	m_didDmg = false;
	m_Watch->Start();
}

Level::Level(Game* owner)
	:m_game(owner)
{
	m_bgBox = AABB2(Vec2::ZERO, Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_floorBox = AABB2(Vec2::ZERO, Vec2(SCREEN_CAMERA_SIZE_X, FLOOR_HEIGHT));
	m_floors.push_back(floorBox(this));
	m_floors.push_back(floorBox(this));
	m_floors.push_back(floorBox(this));
	m_floors.push_back(floorBox(this));
	m_floors[0].m_bound = AABB2(Vec2(0.f, 200.f),Vec2(200.f,400.f));
	m_floors[1].m_bound = AABB2(Vec2(200.f, 200.f), Vec2(400.f, 400.f));
	m_floors[2].m_bound = AABB2(Vec2(400.f, 200.f), Vec2(600.f, 400.f));
	m_floors[3].m_bound = AABB2(Vec2(600.f, 200.f), Vec2(800.f, 400.f));
	m_floors[0].m_warningArea = AABB2(Vec2(0.f, 0.f), Vec2(200.f, 200.f));
	m_floors[1].m_warningArea = AABB2(Vec2(200.f, 0.f), Vec2(400.f, 200.f));
	m_floors[2].m_warningArea = AABB2(Vec2(400.f, 0.f), Vec2(600.f, 200.f));
	m_floors[3].m_warningArea = AABB2(Vec2(600.f, 0.f), Vec2(800.f, 200.f));

}

Level::~Level()
{

}

void Level::Startup()
{
	m_theBoss = new Boss(this);
	m_player = new Healer(this);
	m_party.emplace_back(m_player);
	Tank* tank = new Tank(m_theBoss);
	m_party.emplace_back(tank);
	Warrior* warrior = new Warrior(m_theBoss);
	m_party.emplace_back(warrior);
	Mage* mage = new Mage(m_theBoss);
	m_party.emplace_back(mage);
	m_player->m_currentTarget = m_player;

	m_player->m_neigbhorsNeighbor = m_party[2];
	m_party[1]->m_neigbhorsNeighbor = m_party[3];
	m_party[2]->m_neigbhorsNeighbor = m_party[1];
	m_party[3]->m_neigbhorsNeighbor = m_party[2];

	m_player->m_theParty = m_party.data();
	m_party[1]->m_theParty = m_party.data();
	m_party[2]->m_theParty = m_party.data();
	m_party[3]->m_theParty = m_party.data();

	m_floorPeriodWatch = new Stopwatch(5.f);

}

void Level::Update(float deltaSeconds)
{
	UpdateUserInput(deltaSeconds);
	if (m_theBoss->m_currentHP <= 0.6f * m_theBoss->m_maxHP)
	{
		if (m_floorPeriodWatch)
		{
			if (!m_startFloorDmg)
			{
				m_startFloorDmg = true;
				int randIndex = g_theRNG->RollRandomIntInRange(0, 3);
				m_floorPeriodWatch->Start();
				switch (randIndex)
				{
				case 0:
					m_floors[0].m_Watch->Start();
					m_floors[0].m_isUsing = true;
					m_floors[2].m_Watch->Start();
					m_floors[2].m_isUsing = true;
					m_floors[1].m_isUsing = false;
					m_floors[3].m_isUsing = false;
					break;
				case 1:
					m_floors[1].m_Watch->Start();
					m_floors[1].m_isUsing = true;
					m_floors[3].m_Watch->Start();
					m_floors[3].m_isUsing = true;
					m_floors[0].m_isUsing = false;
					m_floors[2].m_isUsing = false;
					break;
				case 2:
					m_floors[0].m_Watch->Start();
					m_floors[0].m_isUsing = true;
					m_floors[3].m_Watch->Start();
					m_floors[3].m_isUsing = true;
					m_floors[1].m_isUsing = false;
					m_floors[2].m_isUsing = false;
					break;
				case 3:
					m_floors[1].m_Watch->Start();
					m_floors[1].m_isUsing = true;
					m_floors[2].m_Watch->Start();
					m_floors[2].m_isUsing = true;
					m_floors[0].m_isUsing = false;
					m_floors[3].m_isUsing = false;
					break;
				default:
					break;
				}
			}
			else
			{
				if (m_floorPeriodWatch->HasDurationElapsed())
				{
					m_startFloorDmg = false;
				}
			}
		}
	}
	
	m_result = CheckGameResult();
	if (m_result.isLose)
	{
		if (m_resultStopwatch)
		{

		}
		else
		{
			m_resultStopwatch = new Stopwatch(3.f);
			m_resultStopwatch->Start();
		}
	}
	else if (m_result.isWin)
	{
		if (m_resultStopwatch)
		{

		}
		else
		{
			m_resultStopwatch = new Stopwatch(3.f);
			m_resultStopwatch->Start();
		}
	}

	if (m_resultStopwatch && m_resultStopwatch->HasDurationElapsed())
	{
		return;
	}


	for (int charIndex = 0; charIndex < m_party.size(); ++charIndex)
	{
		Hero* hero = m_party[charIndex];
		if (hero)
		{
			hero->Update(deltaSeconds);
		}
	}
	// floating numbers
	for (int i = 0; i < m_numbers.size(); i++)
	{
		if (m_numbers[i])
		{
			m_numbers[i]->Update(deltaSeconds);
		}
	}

	

	m_theBoss->Update(deltaSeconds);

	for (int i = 0; i < m_skillEffects.size(); i++)
	{
		if (m_skillEffects[i])
		{
			m_skillEffects[i]->Update(deltaSeconds);
		}
	}

	for (int i = 0; i < 4; ++i)
	{
		m_floors[i].Update();
	}
}

void Level::Render() const
{
	std::vector<Vertex_PCU> bgVerts;
	AddVertsForAABB2(bgVerts, m_bgBox, Rgba8::WHITE);
	if (m_result.isLose && m_resultStopwatch->HasDurationElapsed())
	{
		Texture* loseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/loseScreen.jpg");
		g_theRenderer->BeginCamera(*m_game->m_app->m_screenCamera);
		g_theRenderer->BindTexture(loseTexture);
		g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
		return;
	}
	else if (m_result.isWin && m_resultStopwatch->HasDurationElapsed())
	{
		Texture* loseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/winScreen.jpg");
		g_theRenderer->BeginCamera(*m_game->m_app->m_screenCamera);
		g_theRenderer->BindTexture(loseTexture);
		g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
		return;
	}
	




	std::vector<Vertex_PCU> floorVerts;
	AddVertsForAABB2(floorVerts, m_floorBox, Rgba8::WHITE);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->BindShader(nullptr);

	g_theRenderer->SetModelConstants();

	Texture* bgTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Background.png");
	Texture* floorTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Floor.png");

	g_theRenderer->BeginCamera(*m_game->m_app->m_screenCamera);
	g_theRenderer->BindTexture(bgTexture);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
	g_theRenderer->BindTexture(floorTexture);
	g_theRenderer->DrawVertexArray((int)floorVerts.size(), floorVerts.data());

	for (int i = 0; i < 4; ++i)
	{
		if (m_floors[i].m_isUsing)
		{
			m_floors[i].Render();
		}

	}

	g_theRenderer->EndCamera(*m_game->m_app->m_screenCamera);

	RenderChars();
	RenderEffects();
	RenderUI();

	g_theRenderer->BeginCamera(*m_game->m_app->m_screenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theDevConsole->Render(AABB2(m_game->m_app->m_screenCamera->GetOrthoBottomLeft(), m_game->m_app->m_screenCamera->GetOrthoTopRight()));
	g_theRenderer->EndCamera(*m_game->m_app->m_screenCamera);

	

}

void Level::EndFrame()
{
	for (int i = 0; i < m_numbers.size(); i++)
	{
		if (m_numbers[i])
		{
			if (m_numbers[i]->m_isEnd)
			{
				delete m_numbers[i];
				m_numbers[i] = nullptr;
			}
		}
	}

	for (int i = 0; i < m_skillEffects.size(); i++)
	{
		if (m_skillEffects[i])
		{
			if (m_skillEffects[i]->m_needToDestory)
			{
				delete m_skillEffects[i];
				m_skillEffects[i] = nullptr;
			}
		}
	}

	if (m_party.size() == 0)
	{
		return;
	}

	for (int i = 0; i < m_party.size(); i++)
	{
		Hero* currenHero = m_party[i];
		if (currenHero)
		{
			HPBar* currentHPBar = currenHero->m_hpBar;
			if (currentHPBar)
			{
				for (int j = 0; j < currentHPBar->m_buffs.size(); ++j)
				{
					if (currentHPBar->m_buffs[j])
					{
						if (currentHPBar->m_buffs[j]->m_markForDelete == true)
						{
							delete currentHPBar->m_buffs[j];
							currentHPBar->m_buffs[j] = nullptr;
						}
					}
				}
			}
		}
	}

	HPBar* bossHPBar = m_theBoss->m_hpBar;
	if (bossHPBar)
	{
		for (int j = 0; j < bossHPBar->m_buffs.size(); ++j)
		{
			if (bossHPBar->m_buffs[j])
			{
				if (bossHPBar->m_buffs[j]->m_markForDelete == true)
				{
					delete bossHPBar->m_buffs[j];
					bossHPBar->m_buffs[j] = nullptr;
				}
			}
		}
	}
	
}

void Level::Shutdown()
{

}

Hero* Level::GetHeroByType(HeroType type)
{
	return m_party[(int)type];
}

void Level::GenerateFloatingNum(float num, FloatingNumType type, Vec2 pos)
{
	Level::m_numbers.push_back(FloatingNumber::SpawnNumber(num, type, pos));
}

SkillEffect* Level::GenerateSkillEffects(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond /*= 20.f*/, SpriteAnimPlaybackType playbackType /*= SpriteAnimPlaybackType::LOOP*/, Hero* target /*= nullptr*/, float numDmg /*= 0.f*/)
{
	SkillEffect* newSE = new SkillEffect(sheet, startSpriteIndex, endSpriteIndex, framesPerSecond, playbackType, target, numDmg);
	m_skillEffects.push_back(newSE);
	return newSE;
}

gameResult Level::CheckGameResult() const
{
	gameResult result;
	if (m_theBoss->IsHeroDead())
	{
		result.isWin = true;
		return result;
	}
	bool anyoneLive = false;
	for (int i = 0; i < m_party.size(); i++)
	{
		if (!m_party[i]->IsHeroDead())
		{
			anyoneLive = true;
		}
	}
	if (!anyoneLive)
	{
		result.isLose = true;
		return result;
	}

	return result;
}

void Level::RenderChars() const
{
	m_theBoss->Render();
	for (int charIndex = 0; charIndex < m_party.size(); ++charIndex)
	{
		Hero* hero = m_party[charIndex];
		if (hero)
		{
			hero->Render();
		}
	}

}

void Level::RenderEffects() const
{
	for (int vsIndex = 0; vsIndex < m_effects.size(); vsIndex++)
	{
		Effect* effect = m_effects[vsIndex];
		if (effect)
		{
			effect->Render();
		}
	}

	for (int i = 0; i < m_skillEffects.size(); i++)
	{
		if (m_skillEffects[i])
		{
			m_skillEffects[i]->Render();
		}
	}
}
void Level::RenderUI() const
{
	for (int charIndex = 0; charIndex < m_party.size(); ++charIndex)
	{
		Hero* hero = m_party[charIndex];
		if (hero)
		{
			hero->RenderUI();
		}
	}
	m_theBoss->RenderUI();
	RenderFloatingNumbers();

	RenderToopTips();

}

void Level::UpdateUserInput(float deltaSeconds)
{
	if (g_theInput->WasKeyJustPressed(ESC_KEY))
	{
		m_game->m_nextState = GameState::MAIN_MENU;
	}

	if (g_theInput->WasKeyJustPressed('F'))
	{
		float num = g_theRNG->RollRandomFloatInRange(100.f, 1000.f);
		m_numbers.push_back(FloatingNumber::SpawnNumber(num, FloatingNumType::Damage, m_player->m_bounds.GetCenter()));
	}

	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (Clock::GetSystemClock().IsPaused())
		{
			Clock::GetSystemClock().Unpause();
		}
		else
		{
			Clock::GetSystemClock().Pause();
		}
	}
	m_player->UpdateUserInput(deltaSeconds);
}

void Level::RenderFloatingNumbers() const
{
	for (int i = 0; i < m_numbers.size(); i++)
	{
		if (m_numbers[i])
		{
			m_numbers[i]->Render();
		}
	}
}

void Level::RenderToopTips() const
{
	for (int i = 0; i < m_party.size(); i++)
	{
		Hero* currenHero = m_party[i];
		if (currenHero)
		{
			HPBar* currentHPBar = currenHero->m_hpBar;
			if (currentHPBar)
			{
				for (int j = 0; j < currentHPBar->m_buffs.size(); ++j)
				{
					if (currentHPBar->m_buffs[j])
					{
						if (currentHPBar->m_buffs[j]->m_isCurserOnIcon)
						{
							currentHPBar->m_buffs[j]->RenderToolTips();
						}
					}
				}
			}
		}
	}

	HPBar* bossHPBar = m_theBoss->m_hpBar;
	if (bossHPBar)
	{
		for (int j = 0; j < bossHPBar->m_buffs.size(); ++j)
		{
			if (bossHPBar->m_buffs[j])
			{
				if (bossHPBar->m_buffs[j]->m_isCurserOnIcon)
				{
					bossHPBar->m_buffs[j]->RenderToolTips();
				}
			}
		}
	}

	if (m_player)
	{
		for (int skillIndex = 0; skillIndex < m_player->m_skillGroup->m_skills.size(); ++skillIndex)
		{
			if (m_player->m_skillGroup->m_skills[skillIndex]->m_isCurserOnIcon)
			{
				m_player->m_skillGroup->m_skills[skillIndex]->RenderToolTip();
			}
		}

	}
}


