#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

constexpr int MAX_NUM_KEYCODE = 256;
bool static m_isQuitting = false;

class App
{
	friend class Game;
public :
	App();
	~App();
	void Startup();
	void Run();
	void Shutdown();
	void Runframe();
		
	bool IsQuitting() const { return m_isQuitting;  }
	bool HandleQuitRequested();

	SoundPlaybackID GetBGMPlaybackID() const;

	Camera* m_worldCamera = nullptr;
	static Camera* m_screenCamera;
	static Game* m_theGame;

private:
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();

	void LoadingAssets();
private:
	SoundPlaybackID BGM;
	bool isLoadedAssets = false;
	bool isRenderingLoadingScreen = false;
};