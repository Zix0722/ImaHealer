#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Hero.hpp"
#include <vector>
#include "Effect.hpp"
#include "FloatingNumber.hpp"
#include "SkillEffect.hpp"
#include "Boss.hpp"
class Game;
class Healer;

enum class HeroType
{
	healer,
	tank,
	warrior,
	mage,
	cnt,
};

class floorBox
{
public:
	floorBox(Level* level);
	~floorBox();
	void Update();
	void Render() const;

	void OnStart();

public:
	Level* m_level = nullptr;
	AABB2 m_bound;
	AABB2 m_warningArea;
	Stopwatch* m_Watch = nullptr;
	bool m_didDmg = false;
	Texture* m_texture = nullptr;
	Effect* m_effect = nullptr;
	bool m_isUsing = false;
};

struct gameResult
{
public:
	bool isWin = false;
	bool isLose = false;
};

class Level
{
public:
	Level(Game* owner);
	~Level();
	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();
	void Shutdown();
	Hero* GetHeroByType(HeroType type);
	static void GenerateFloatingNum(float num, FloatingNumType type, Vec2 pos);
	static SkillEffect* GenerateSkillEffects(SpriteSheet* sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond = 20.f,
		SpriteAnimPlaybackType playbackType = SpriteAnimPlaybackType::LOOP, Hero* target = nullptr, float numDmg = 0.f);

	gameResult CheckGameResult() const;
private:
	void RenderChars() const;
	void RenderEffects() const;
	void RenderUI() const;

	void UpdateUserInput(float deltaSeconds);
	void RenderFloatingNumbers() const;
	void RenderToopTips() const;
	
	
public:
	Game* m_game = nullptr;
	AABB2 m_bgBox;
	AABB2 m_floorBox;
	Healer* m_player = nullptr;
	Boss* m_theBoss = nullptr;
	std::vector<Hero*> m_party;
	std::vector<Effect*> m_effects;
	static std::vector<FloatingNumber*>m_numbers;
	static std::vector<SkillEffect*>m_skillEffects;
	gameResult m_result;
	Stopwatch* m_resultStopwatch = nullptr;
	Stopwatch* m_floorPeriodWatch = nullptr;
	std::vector<floorBox> m_floors;
	bool m_startFloorDmg = false;
};