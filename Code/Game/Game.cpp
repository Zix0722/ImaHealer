#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Game/Level.hpp"
#include "Effect.hpp"
#include "ThirdParty/Squirrel/SmoothNoise.hpp"
#include "Engine/Renderer/DebugRender.hpp"


RandomNumberGenerator* g_theRNG = nullptr;
extern Renderer* g_theRenderer;
extern DevConsole* g_theDevConsole;
BitmapFont* m_gameFont = nullptr;
Setting Game::m_currentSetting;

Game::Game(App* owner)
	:m_app(owner)
{
	g_theRNG = new RandomNumberGenerator();
}

Game::~Game()
{
	delete m_gameClock;
}

void Game::Startup()
{
	m_app->m_screenCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_gameClock = new Clock();
	m_menu_background_box = AABB2(Vec2::ZERO, Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	m_gameFont = g_theRenderer->CreateOrGetBitmapFont("Data/Fonts/SquirrelFixedFont");
	m_menu_background_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Main_Menu.png");
	
	m_settingPanel = m_menu_background_box;
	m_settingPanel.SetDimensions(Vec2(900.f, 750.f));
	m_settings_checkBox_BGM.SetDimensions(Vec2(50.f,50.f));
	m_settings_checkBox_Use_Cursor.SetDimensions(Vec2(50.f, 50.f));

	m_settings_checkBox_Use_Cursor.SetCenter(m_settingPanel.GetPointAtUV(Vec2(0.8f, 0.8f)));
	m_settings_checkBox_BGM.SetCenter(m_settingPanel.GetPointAtUV(Vec2(0.8f, 0.7f)));

}

void Game::Update(float deltaSeconds)
{
	UpdateGameState();

	switch (m_curentState)
	{
	case GameState::NONE:
		break;
	case GameState::MAIN_MENU:
		UpdateMainMenu(deltaSeconds);
		break;
	case GameState::SETTING:
		UpdateSetting(deltaSeconds);
		break;
	case GameState::PLAYING:
		UpdatePlaying(deltaSeconds);
		break;
	case GameState::COUNT:
		break;
	default:
		break;
	}

}

void Game::Render() const
{
	switch (m_curentState)
	{
	case GameState::NONE:
		break;
	case GameState::MAIN_MENU:
		RenderMainMenu();
		break;
	case GameState::SETTING:
		RenderSetting();
		break;
	case GameState::PLAYING:
		RenderPlaying();
		break;
	case GameState::COUNT:
		break;
	default:
		break;
	}
}


void Game::EndFrame()
{
	for (int i = 0; i < m_menuEffects.size(); ++i)
	{
		if (m_menuEffects[i])
		{
			if (m_menuEffects[i]->m_needToDestory)
			{
				delete m_menuEffects[i];
				m_menuEffects[i] = nullptr;
			}
		}
	}
	if (m_currentLevel)
	{
		m_currentLevel->EndFrame();
	}
}

void Game::Shutdown()
{

}


void Game::EnterState(GameState state)
{
	switch (state)
	{
	case GameState::NONE:
		break;
	case GameState::MAIN_MENU:
		m_curentState = GameState::MAIN_MENU;
		EnterMainMenu();
		break;
	case GameState::SETTING:
		m_curentState = GameState::SETTING;
		EnterSetting();
		break;
	case GameState::PLAYING:
		m_curentState = GameState::PLAYING;
		EnterPlaying();
		break;
	case GameState::COUNT:
		break;
	default:
		break;
	}
}

void Game::ExitState(GameState state)
{
	switch (state)
	{
	case GameState::NONE:
		break;
	case GameState::MAIN_MENU:
		ExitMainMenu();
		break;
	case GameState::SETTING:
		ExitSetting();
		break;
	case GameState::PLAYING:
		ExitPlaying();
		break;
	default:
		break;
	}
}


void Game::EnterMainMenu()
{
	m_currentSelection = MenuSelection::START;
	m_menu_background_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Main_Menu.png");

	Texture* texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/ghostFire.png");
	SpriteSheet* sheet = new SpriteSheet(*texture, IntVec2(6,5), 1.f/1.2f);
	float x = 0.f, y = 0.f;
	for (int i = 0; i < 15; i++)
	{
		Effect* newEffect = new Effect(sheet, 0, 29 ,30.f, SpriteAnimPlaybackType::LOOP);
		x =  g_theRNG->RollRandomFloatInRange(0.f, SCREEN_CAMERA_SIZE_X);
		y =  g_theRNG->RollRandomFloatInRange(0.f, SCREEN_CAMERA_SIZE_Y);
		newEffect->m_position = Vec2(x, y);
		newEffect->SetSize(200.f, 250.f);
		newEffect->m_animDelay = g_theRNG->RollRandomFloatZeroToOne() * 500000.f;
		m_menuEffects.emplace_back(newEffect);
	}

	if (m_currentSetting.isUsingBGM)
	{
		m_menu_bgm = g_theAudio->CreateOrGetSound("Data/Audio/7. Illidan and Akama.mp3");
		m_menu_bgm = g_theAudio->StartSound(m_menu_bgm, true);
	}
}

void Game::EnterSetting()
{
	m_menu_background_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/settings_bg.png");
}

void Game::EnterPlaying()
{
	m_currentLevel = new Level(this);
	m_currentLevel->Startup();
	g_theAudio->StopSound(m_menu_bgm);
	if (m_currentSetting.isUsingBGM)
	{
		m_play_bgm = g_theAudio->CreateOrGetSound("Data/Audio/1. The Black Temple.mp3");
		m_play_bgm = g_theAudio->StartSound(m_play_bgm, true, 0.5f);
	}
}

void Game::ExitMainMenu()
{
	for (int vsIndex = 0; vsIndex < m_menuEffects.size(); ++vsIndex)
	{
		if (m_menuEffects[vsIndex])
		{
			delete m_menuEffects[vsIndex];
			m_menuEffects[vsIndex] = nullptr;
		}
	}
	m_menuEffects.clear();
}

void Game::ExitSetting()
{

}

void Game::ExitPlaying()
{
	m_currentLevel->Shutdown();
	g_theAudio->StopSound(m_play_bgm);
	delete m_currentLevel;
}

void Game::UpdateMainMenu(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed('A'))
	{
		if (((int)m_currentSelection - 1) == -1)
		{
			m_currentSelection = (MenuSelection)((int)MenuSelection::Count);
		}
		m_currentSelection = (MenuSelection)((int)m_currentSelection - 1);
		m_currentSelection = (MenuSelection)((int)m_currentSelection % (int)MenuSelection::Count);
		PlayButtonSwithSound();
	}
	if (g_theInput->WasKeyJustPressed('D'))
	{
		m_currentSelection = (MenuSelection)((int)m_currentSelection + 1);
		m_currentSelection = (MenuSelection)((int)m_currentSelection % (int)MenuSelection::Count);
		PlayButtonSwithSound();
	}
	if (g_theInput->WasKeyJustPressed(SPACE_KEY))
	{
		if (m_currentSelection == MenuSelection::START)
		{
			m_nextState = GameState::PLAYING;
		}
		else if (m_currentSelection == MenuSelection::SETTING)
		{
			m_nextState = GameState::SETTING;
		}
		else if (m_currentSelection == MenuSelection::QUIT)
		{
			FireEvent("quit");
		}
	}

	for (int vsIndex = 0; vsIndex < m_menuEffects.size(); ++vsIndex)
	{
		if (m_menuEffects[vsIndex])
		{
			m_menuEffects[vsIndex]->Update(deltaSeconds);

		}
	}

}

void Game::UpdateSetting(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed(ESC_KEY))
	{
		m_nextState = GameState::MAIN_MENU;
	}
	Vec2 cursorUV = g_theWindow->GetCursorNormalizeUVs();
	Vec2 cursorPos = App::m_screenCamera->GetPosAtUV(cursorUV);
	if (m_settings_checkBox_BGM.IsPointInside(cursorPos))
	{
		if (g_theInput->WasKeyJustPressed(KEYCODE_LMB))
		{
			m_currentSetting.isUsingBGM = !m_currentSetting.isUsingBGM;
			if (m_currentSetting.isUsingBGM)
			{
				g_theAudio->SetSoundPlaybackSpeed(m_menu_bgm, 1.f);
			}
			else
			{
				g_theAudio->SetSoundPlaybackSpeed(m_menu_bgm, 0.f);
			}
			
		}
	}
	else if (m_settings_checkBox_Use_Cursor.IsPointInside(cursorPos))
	{
		if (g_theInput->WasKeyJustPressed(KEYCODE_LMB))
		{
			m_currentSetting.isUsingCurserControl = !m_currentSetting.isUsingCurserControl;
		}
	}
}

void Game::UpdatePlaying(float deltaSeconds)
{
	if (m_currentLevel)
	{
		m_currentLevel->Update(deltaSeconds);
	}
}

void Game::RenderMainMenu() const
{
	std::vector<Vertex_PCU> bgVerts;
	AddVertsForAABB2(bgVerts, m_menu_background_box, Rgba8::WHITE);
	std::vector<Vertex_PCU> buttonVerts;
	Rgba8 leftArrowColor = Rgba8::WHITE;
	Rgba8 rightArrowColor = Rgba8::WHITE;
	if (g_theInput->IsKeyDown('D'))
	{
		rightArrowColor = Rgba8::GREY;
	}
	else if (g_theInput->IsKeyDown('A'))
	{
		leftArrowColor = Rgba8::GREY;
	}
	AddVertsForAABB2(buttonVerts, AABB2(Vec2(650.f, 150.f), Vec2(600.f, 100.f)), leftArrowColor);
	AddVertsForAABB2(buttonVerts, AABB2(Vec2(1000.f, 100.f), Vec2(1050.f, 150.f)), rightArrowColor);
	std::vector<Vertex_PCU> textVerts;
	AABB2 textBox = AABB2(Vec2(700.f, 100.f), Vec2(950.f, 150.f));
	AABB2 textShadowBox = AABB2(Vec2(702.f, 98.f), Vec2(952.f, 148.f));

	switch (m_currentSelection)
	{
	case MenuSelection::START:
		m_gameFont->AddVertsForTextInBox2D(textVerts, textShadowBox, 30.f, "START", Rgba8(100, 100, 100), 0.7f);
		m_gameFont->AddVertsForTextInBox2D(textVerts, textBox, 30.f, "START", Rgba8(200, 200, 200), 0.7f);
		break;
	case MenuSelection::SETTING:
		m_gameFont->AddVertsForTextInBox2D(textVerts, textShadowBox, 30.f, "SETTING", Rgba8(100, 100, 100), 0.7f);
		m_gameFont->AddVertsForTextInBox2D(textVerts, textBox, 30.f, "SETTING", Rgba8(200, 200, 200), 0.7f);
		break;
	case MenuSelection::QUIT:
		m_gameFont->AddVertsForTextInBox2D(textVerts, textShadowBox, 30.f, "QUIT", Rgba8(100, 100, 100), 0.7f);
		m_gameFont->AddVertsForTextInBox2D(textVerts, textBox, 30.f, "QUIT", Rgba8(200, 200, 200), 0.7f);
		break;
	case MenuSelection::Count:
		break;
	default:
		break;
	}

	g_theRenderer->ClearScreen(Rgba8::RED);
	g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
	g_theRenderer->BindShader(nullptr);

	g_theRenderer->SetModelConstants();

	g_theRenderer->BeginCamera(*m_app->m_screenCamera);
	g_theRenderer->BindTexture(m_menu_background_texture);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());

	for (int vsIndex = 0; vsIndex < m_menuEffects.size(); ++vsIndex)
	{
		if (m_menuEffects[vsIndex])
		{
			m_menuEffects[vsIndex]->Render();

		}
	}


	Texture* arrowTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/ui_arrow.png");
	g_theRenderer->BindTexture(arrowTexture);
	g_theRenderer->SetModelConstants();
	g_theRenderer->DrawVertexArray((int)buttonVerts.size(), buttonVerts.data());

	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)textVerts.size(), textVerts.data());

	g_theRenderer->EndCamera(*m_app->m_screenCamera);


	
	g_theRenderer->BeginCamera(*m_app->m_screenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theDevConsole->Render(AABB2(m_app->m_screenCamera->GetOrthoBottomLeft(), m_app->m_screenCamera->GetOrthoTopRight()));


	g_theRenderer->EndCamera(*m_app->m_screenCamera);
}

void Game::RenderSetting() const
{
	Texture* panelTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Panel_1.png");
	Texture* checkMark = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/greenCheck.png");
	Texture* checkBox = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/checkBox.png");
	AABB2 txtline1 = m_settings_checkBox_Use_Cursor;
	txtline1.SetDimensions(Vec2(400.f, 200.f));
	txtline1.SetCenter(m_settingPanel.GetPointAtUV(Vec2(0.4f, 0.8f)));
	AABB2 txtline2 = m_settings_checkBox_BGM;
	txtline2.SetDimensions(Vec2(400.f, 200.f));
	txtline2.SetCenter(m_settingPanel.GetPointAtUV(Vec2(0.4f, 0.7f)));

	std::vector<Vertex_PCU> bgVerts;
	std::vector<Vertex_PCU> panelVerts;
	std::vector<Vertex_PCU> checkBoxVerts;
	std::vector<Vertex_PCU> checkMarkVerts;
	std::vector<Vertex_PCU> txtVerts;

	
	AddVertsForAABB2(bgVerts, m_menu_background_box, Rgba8::WHITE);
	AddVertsForAABB2(panelVerts, m_settingPanel, Rgba8::WHITE);
	AddVertsForAABB2(checkBoxVerts, m_settings_checkBox_BGM, Rgba8::WHITE);
	AddVertsForAABB2(checkBoxVerts, m_settings_checkBox_Use_Cursor, Rgba8::WHITE);

	m_gameFont->AddVertsForTextInBox2D(txtVerts, txtline1, 20.f, "Using Cursor to cast abilities", Rgba8(200, 200, 200), 0.7f,Vec2(0.f,0.5f));
	m_gameFont->AddVertsForTextInBox2D(txtVerts, txtline2, 20.f, "Background Music", Rgba8(200, 200, 200), 0.7f, Vec2(0.f, 0.5f));

	if (m_currentSetting.isUsingBGM)
	{
		AABB2 markBox = m_settings_checkBox_BGM;
		markBox.SetDimensions(Vec2(75.f, 75.f));
		markBox.Translate(Vec2(7.f,7.f));
		AddVertsForAABB2(checkMarkVerts, markBox, Rgba8::WHITE);
	}
	if (m_currentSetting.isUsingCurserControl)
	{
		AABB2 markBox = m_settings_checkBox_Use_Cursor;
		markBox.SetDimensions(Vec2(75.f, 75.f));
		markBox.Translate(Vec2(7.f, 7.f));
		AddVertsForAABB2(checkMarkVerts, markBox, Rgba8::WHITE);
	}

	g_theRenderer->ClearScreen(Rgba8::RED);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	g_theRenderer->BindShader(nullptr);

	g_theRenderer->SetModelConstants();

	g_theRenderer->BeginCamera(*m_app->m_screenCamera);
	g_theRenderer->BindTexture(m_menu_background_texture);
	g_theRenderer->DrawVertexArray((int)bgVerts.size(), bgVerts.data());
	g_theRenderer->BindTexture(panelTexture);
	g_theRenderer->DrawVertexArray((int)panelVerts.size(), panelVerts.data());
	g_theRenderer->BindTexture(checkBox);
	g_theRenderer->DrawVertexArray((int)checkBoxVerts.size(), checkBoxVerts.data());
	g_theRenderer->BindTexture(checkMark);
	g_theRenderer->DrawVertexArray((int)checkMarkVerts.size(), checkMarkVerts.data());

	g_theRenderer->BindTexture(&m_gameFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)txtVerts.size(), txtVerts.data());


	g_theRenderer->EndCamera(*m_app->m_screenCamera);



	g_theRenderer->BeginCamera(*m_app->m_screenCamera);
	g_theRenderer->BindTexture(nullptr);
	g_theDevConsole->Render(AABB2(m_app->m_screenCamera->GetOrthoBottomLeft(), m_app->m_screenCamera->GetOrthoTopRight()));
	g_theRenderer->EndCamera(*m_app->m_screenCamera);
}

void Game::RenderPlaying() const
{
	if (m_currentLevel)
	{
		m_currentLevel->Render();
	}
}

void Game::PlayButtonSwithSound()
{

}

void Game::UpdateGameState()
{
	if (m_curentState != m_nextState)
	{
		ExitState(m_curentState);
		EnterState(m_nextState);
	}
}





