#ifndef __GAME_SCORE_LAYER_H__
#define __GAME_SCORE_LAYER_H__

#include "BaseScene.h"

//游戏分数区域
class GameScoreLayer : public cocos2d::Layer
{
public:
	GameScoreLayer();
	virtual ~GameScoreLayer();

public:
	//设置游戏分数
	void SetGameScore(uint32_t score);

	//设置已销毁的层数
	void SetCleanLinesCount(uint32_t count);

	//设置已出现的方块的数量
	void SetFallBlockCount(uint32_t count);

	//设置速度等级
	void SetGameLevel(uint32_t level);

	//设置游戏进行时间
	void SetPlayTimeTick(uint32_t ticks);

public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init() override;

	// implement the "static create()" method manually
	CREATE_FUNC(GameScoreLayer);

	virtual void update(float delta) override;

private:
	cocos2d::ui::TextBMFont* m_gameSocre = nullptr;
	cocos2d::ui::TextBMFont* m_fallBlockCount = nullptr;
	cocos2d::ui::TextBMFont* m_cleanLineCount = nullptr;
	cocos2d::ui::TextBMFont* m_speedLevel = nullptr;
	cocos2d::ui::TextBMFont* m_playTimeTick = nullptr;



};



#endif // __GAME_SCORE_LAYER_H__
