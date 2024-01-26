#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include <vector>

class App;
class Clock;
class Level;
class Effect;



enum class GameState
{
	NONE,
	MAIN_MENU,
	SETTING,
	PLAYING,
	COUNT
};

enum class MenuSelection
{
	START,
	SETTING,
	QUIT,
	Count
};

enum class SettingSelection
{
	MASTER_VOL,

};

struct Setting
{
	bool isUsingCurserControl = true;
	bool isUsingBGM = true;
};

class Game
{
public:
	Game(App* owner);
	~Game();
	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();
	void Shutdown();

	void EnterState(GameState state);
	void ExitState(GameState state);



public :
	App* m_app = nullptr;
	GameState m_curentState = GameState::COUNT;
	GameState m_nextState = GameState::MAIN_MENU;
	bool g_DebugMo = false;
	Level* m_currentLevel = nullptr;
	static Setting m_currentSetting;

private:
	void UpdateGameState();

	void EnterMainMenu();
	void EnterSetting();
	void EnterPlaying();

	void ExitMainMenu();
	void ExitSetting();
	void ExitPlaying();

	void UpdateMainMenu(float deltaSeconds);
	void UpdateSetting(float deltaSeconds);
	void UpdatePlaying(float deltaSeconds);

	void RenderMainMenu() const;
	void RenderSetting() const;
	void RenderPlaying() const;

	void PlayButtonSwithSound();
private:

	Clock* m_gameClock = nullptr;
	SoundPlaybackID m_menu_bgm;
	SoundPlaybackID m_play_bgm;
	MenuSelection m_currentSelection = MenuSelection::START;
	AABB2 m_menu_background_box;
	Texture* m_menu_background_texture = nullptr;
	std::vector<Effect*> m_menuEffects;
	AABB2 m_settingPanel;
	AABB2 m_settings_checkBox_Use_Cursor;
	AABB2 m_settings_checkBox_BGM;
};