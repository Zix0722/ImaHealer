#include "CharDef.hpp"
#include "GameCommon.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
std::vector<CharDef*> CharDef::s_charDefinitions;


CharDef::CharDef()
{

}

CharDef::~CharDef()
{

}

bool CharDef::LoadFromXmlElement(XmlElement const& element)
{
	m_name = ParseXMLAttribute(element, "name", m_name);
	std::string spriteSheetPath; 
	spriteSheetPath = ParseXMLAttribute(element, "spriteSheet", spriteSheetPath).c_str();
	m_cellCount = ParseXMLAttribute(element, "cellCount", m_cellCount);
	Texture * usingTexture = g_theRenderer->CreateOrGetTextureFromFile(spriteSheetPath.c_str());
	m_charSpriteSheet = new SpriteSheet(*usingTexture, m_cellCount);
	
	return true;
}

CharDef* CharDef::GetCharDefByName(std::string name)
{
	for (int defIndex = 0; defIndex < s_charDefinitions.size(); ++defIndex)
	{
		if (s_charDefinitions[defIndex]->m_name == name)
		{
			return s_charDefinitions[defIndex];
		}
	}
	return nullptr;
}


