#include "GameScoreLayer.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;


GameScoreLayer::GameScoreLayer()
{

}

GameScoreLayer::~GameScoreLayer()
{

}

void GameScoreLayer::SetGameScore(uint32_t score)
{
	auto strScore = __String::createWithFormat("%d", score);
	m_gameSocre->setString(strScore->getCString());
}

void GameScoreLayer::SetCleanLinesCount(uint32_t count)
{
	auto strScore = __String::createWithFormat("%d", count);
	m_cleanLineCount->setString(strScore->getCString());
}

void GameScoreLayer::SetFallBlockCount(uint32_t count)
{
	auto strScore = __String::createWithFormat("%d", count);
	m_fallBlockCount->setString(strScore->getCString());
}

bool GameScoreLayer::init()
{
	if (!cocos2d::Layer::init())
		return false;

	auto rootN = CSLoader::createNode("scene/GameScoreLayer.csb");
	if (!rootN)
	{
		return false;
	}
	m_gameSocre = dynamic_cast<cocos2d::ui::TextBMFont*>(rootN->getChildByName("GameScoreLable"));
	m_speedLevel = dynamic_cast<cocos2d::ui::TextBMFont*>(rootN->getChildByName("SpeedLevel"));
	m_fallBlockCount = dynamic_cast<cocos2d::ui::TextBMFont*>(rootN->getChildByName("FallBlockCount"));
	m_cleanLineCount = dynamic_cast<cocos2d::ui::TextBMFont*>(rootN->getChildByName("CleanLineCount"));

	addChild(rootN);

	return true;

}

void GameScoreLayer::update(float delta)
{
	cocos2d::Layer::update(delta);
}

void GameScoreLayer::SetSpeedLevel(uint32_t level)
{
	auto strScore = __String::createWithFormat("%d", level);
	m_speedLevel->setString(strScore->getCString());
	
}
