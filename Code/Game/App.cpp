#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Window/Window.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Game/App.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/VertexUtils.hpp"

Renderer* g_theRenderer = nullptr;			
InputSystem* g_theInput = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;
DevConsole* g_theDevConsole = nullptr;

constexpr float MAX_FRAME_SEC = 1.f / 10.f;

Camera* App::m_screenCamera = nullptr;
Game* App::m_theGame = nullptr;

bool QuitAppCallbackFunction(EventArgs& args)
{
	UNUSED(args);
	m_isQuitting = true;
	return true;
}


App::App()
{
	BGM = 0;
	m_theGame = nullptr;

	m_worldCamera = new Camera();
	m_screenCamera = new Camera();
}

App::~App()
{
	delete g_theDevConsole;
	delete m_theGame;
	delete g_theAudio;
	delete g_theRenderer;
	delete g_theWindow;
	delete g_theInput;
	delete m_worldCamera;
	delete m_screenCamera;

	g_theDevConsole = nullptr;
	m_theGame = nullptr;
	g_theAudio = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;
	g_theInput = nullptr;
	m_worldCamera = nullptr;
	m_screenCamera = nullptr;
}

void App::Startup()
{
	m_worldCamera->SetOrthographicView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera->SetOrthographicView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	
	InputSystemConfig inputConfig;
	g_theInput = new InputSystem(inputConfig);

	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "Ima Healer";
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_camera = m_screenCamera;
	devConsoleConfig.m_renderer = g_theRenderer;

	g_theDevConsole = new DevConsole(devConsoleConfig);

	AudioSystemConfig audioConfig;
	g_theAudio = new AudioSystem(audioConfig);

	m_theGame = new Game(this);

	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();
	m_theGame->Startup();

	SubscribeEventCallbackFunction("quit", QuitAppCallbackFunction);

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_theRenderer;
	debugRenderConfig.m_startHidden = false;
	DebugRenderSystemStartup(debugRenderConfig);

	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Controls");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "QWERT    - Ability 1,2,3,4,5 from Left to Right of the Action Bar.");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Cursor over on Icons  - See more details about the effects");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "A/D   - Movements");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "1-4   - Choose Target");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "LMB click on HP Bar - Choose Target as well");

	FireEvent("debugrenderclear");
}


void App::Run()
{
	while (!m_isQuitting)			
	{
		Sleep(0); 
        Runframe();
	}
}

void App::Shutdown()
{	
	DebugRenderSystemShutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();
	m_theGame->Shutdown();
}

void App::Runframe()
{	
	float deltaSeconds = Clock::GetSystemClock().GetDeltaSeconds();

	BeginFrame();
	Update(deltaSeconds);
	Render();
	EndFrame();
}

bool App::HandleQuitRequested()
{
	return false;
}



SoundPlaybackID App::GetBGMPlaybackID() const
{
	return BGM;
}


void App::BeginFrame()
{
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theInput->BeginFrame();
	g_theAudio->BeginFrame();
	DebugRenderBeginFrame();
	

	if (g_theInput->WasKeyJustPressed(ESC_KEY))
	{
		if (m_theGame->m_curentState == GameState::MAIN_MENU)
		{
			m_isQuitting = true;
		}	
	}

	if (g_theInput->WasKeyJustPressed(KEYCODE_TILDE))
	{
		g_theDevConsole->ToggleOpen();
	}
}

void App::Update(float deltaSeconds)
{
	Clock::TickSystemClock();

	g_theInput->SetCursorMode(true, false);

	if (!isLoadedAssets && !isRenderingLoadingScreen)
	{
		isRenderingLoadingScreen = true;
	}
	else
	{
		LoadingAssets();
		isLoadedAssets = true;
	}

	m_theGame->Update(deltaSeconds);
	
}

void App::Render() const
{
	
	m_theGame->Render();
	if (!isLoadedAssets && isRenderingLoadingScreen)
	{
		Texture* loading = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/loadingScreen.jpg");
		AABB2 box = AABB2(Vec2::ZERO, Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
		std::vector<Vertex_PCU> loadingVerts;

		AddVertsForAABB2(loadingVerts, box, Rgba8::WHITE);
		g_theRenderer->SetBlendMode(BlendMode::OPAQUE);
		g_theRenderer->BindShader(nullptr);

		g_theRenderer->SetModelConstants();

		g_theRenderer->BeginCamera(*m_screenCamera);
		g_theRenderer->BindTexture(loading);
		g_theRenderer->DrawVertexArray((int)loadingVerts.size(), loadingVerts.data());
	}
	DebugRenderScreen(*m_screenCamera);
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	m_theGame->EndFrame();
	g_theAudio->EndFrame();
	g_theRenderer->EndFrame();
	DebugRenderEndFrame();
}

void App::LoadingAssets()
{
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Main_Menu.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/ui_arrow.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/settings_bg.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Background.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Floor.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/ghostFire.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/healer.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Idle.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Jump.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Run.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Get Hit.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Fall.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Death.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack3.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack2.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval Warrior Pack 3/Sprites/Attack1.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Idle.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Jump.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Run.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Get Hit.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Fall.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Death.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack3.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack2.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Medieval King Pack 2/Sprites/Attack1.png");
	
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Idle.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Run.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Get Hit.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Death.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard/Sprites/Attack.png");

	//---hp bar----

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/base.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/green.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/red.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/hpBar/top.png");

	//----indicators
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/arrow.png");
	//--------action bar-----------
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/actionBarDragon.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/smallHeal.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/bigHeal.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/clean.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/hot.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/protect.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/FLASHHEAL.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/bigHeal.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/HoT.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/cleanDebuff.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/divineShield.png");
	//-------------boss------------------------
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Take hit.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Run.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Jump.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Idle.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Fall.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Death.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Attack2.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Evil Wizard 2/Sprites/Attack1.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Dark VFX 1/shadowArrow.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/purpleSmall.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/curse.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Dark VFX 2/Dark VFX 2 (48x64).png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/loseScreen.jpg");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/purple_Effect.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Panel_1.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/greenCheck.png"); 
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/checkBox.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/taunt.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/winScreen.jpg");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/Ability_gouge.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/NEw pack blood/5_100x100px.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Icons/ability_kick.jpg");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/redNormal.png.png"); 
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/blue.png");

	g_theAudio->CreateOrGetSound("Data/Audio/7. Illidan and Akama.mp3");
	g_theAudio->CreateOrGetSound("Data/Audio/1. The Black Temple.mp3");

	g_theAudio->CreateOrGetSound("Data/Audio/casting.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_01.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_02.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_03.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/FX_80_Azeritekit_Cast_Med_04.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/sheild.wav");
	g_theAudio->CreateOrGetSound("Data/Audio/iceball.wav"); 
	g_theAudio->CreateOrGetSound("Data/Audio/fireball.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1A.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1B.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/m1hSwordHitChain1C.ogg");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_1.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_2.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_3.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_4.png");

	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_q.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_w.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_e.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_r.png");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/keyboard_t.png");

	g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_01.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_02.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/Spell_DH_Nemesis_Cast_03.ogg");
	g_theAudio->CreateOrGetSound("Data/Audio/CurseofTounges.ogg");
	g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Gravity-SpriteSheet.png");
}

