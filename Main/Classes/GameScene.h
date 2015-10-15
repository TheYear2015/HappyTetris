#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include <array>
#include <vector>
#include "BaseScene.h"
#include "GameLogic.h"
#include "GameScoreLayer.h"

class GameScene : public XUtility::BaseScene<GameScene>
{
protected:
	virtual const char* GetSceneCsb() const override;

	virtual void OnSceneInited() override;

};

//方块主体区域
class PlayTetrisLayer : public cocos2d::Layer, public PlayTetrisObserver
{
public:
	PlayTetrisLayer();
	virtual ~PlayTetrisLayer();

public:
	//一局开始
	void NewRound();

	//一局结束
	void EndRound();

	void NewFallBlock(float);
	

public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init() override;

	// implement the "static create()" method manually
	CREATE_FUNC(PlayTetrisLayer);

	virtual void update(float delta) override;

	void TouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type);


private:
	PlayTetris m_logic;

	//操作帧
	void OperatorFrame();

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	void BeginHorMoveInput(int i);

	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	void EndHorMoveInput(int i);

	void SetFallBlockDir(int dir, bool isToEnd = false);

	float m_operatorFrameTime = 0.0f;

	//水平操作输入
	std::vector<int> m_horizontalMoveInput;

	//水平位移 0无位移，1向右，-1向左
	int m_horizontalMoveDir = 0;

	bool m_holdInput = false;

public:

	virtual void OnlineClean(const std::array<int, TetrisData::Height>& lines) override;

	virtual void OnBlocksFallFill(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>&) override;

	virtual void OnBlockFallEnd(BlockType block, const std::array<std::pair<int, int>, 4>& pos) override;

	virtual void OnNewBlock(BlockType block, int dir, int x, int y) override;

	virtual void OnBlockMove(int x, int y) override;

	virtual void OnBlockTurn(int dir) override;

	virtual void OnNewBlockFailed() override;

private:
	//所有固定的方块
	std::array<std::array<std::array<cocos2d::Sprite*,2>, TetrisData::Width>, TetrisData::Height> m_blockSpriteMap;

	void SetBlockOnPos(BlockType block, int x, int y);

	void RemoveLine(int line);

	void RemoveAllBlock();

	void DeleteBlockSprite(cocos2d::Node*);

private:
	//所有方块的root
	cocos2d::Node* m_rootNode = nullptr;

private:
	//下落方块图形单元
	class FallBlockSprite
	{

	public:
		//type 0 普通，1 预览
		FallBlockSprite(int type, cocos2d::Node* parent);

		~FallBlockSprite();

		void SetPosition(int x, int y);

		//设置方块方向
		void SetDir(int dir);

		//设置方块类型
		void SetBlockType(BlockType block);

		void End();

		void Begin();

		void SetScale(float scale);

	private:
		std::array<std::array<cocos2d::Sprite*, 2>, 4> m_blockSprite;
		int m_x;
		int m_y;
		int m_dir;
		int m_type = 0;
		float m_scale = 1.0f;
		BlockType m_block = BlockType::Empty;
		cocos2d::Node* m_parent = nullptr;

	};

	//后续将要出现的下落方块单元
	class NextFallBlockSprite
	{

	public:
		NextFallBlockSprite(cocos2d::Node* parent);

		~NextFallBlockSprite();

		void SetPosition(int x, int y);

		//设置方块方向
		void SetDir();

		//设置方块类型
		void SetBlockType(BlockType block);

		void SetScale(float scale);

	private:
		cocos2d::Node* m_root = nullptr;
		int m_x;
		int m_y;
		BlockType m_block = BlockType::Empty;
		cocos2d::Node* m_parent = nullptr;

	};

	//下落方块
	FallBlockSprite* m_fallBlock = nullptr;

	//方块下落预览
	FallBlockSprite* m_previewFallBlock = nullptr;

	cocos2d::Node* m_nextFallBlockRoot = nullptr;

	cocos2d::Node* m_UILayer = nullptr;

	//后续需要下落方块
	std::array<NextFallBlockSprite*, 3> m_nextFallBlock;

	//分数显示
	GameScoreLayer* m_scoreLayer = nullptr;

};



#endif // __GAME_SCENE_H__
