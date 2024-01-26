#pragma once
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"

class SpriteSheet;

class CharDef
{
public:
	CharDef();
	~CharDef();

	bool LoadFromXmlElement(XmlElement const& element);
	static std::vector<CharDef*> s_charDefinitions;
	static CharDef* GetCharDefByName(std::string name);
	std::string GetSoundByName(std::string name);


public:
	std::string m_name;
	int m_health = 1;
	IntVec2 m_cellCount;
	SpriteSheet* m_charSpriteSheet = nullptr;
};
