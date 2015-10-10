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

//������������
class PlayTetrisLayer : public cocos2d::Layer, public PlayTetrisObserver
{
public:
	PlayTetrisLayer();
	virtual ~PlayTetrisLayer();

public:
	//һ�ֿ�ʼ
	void NewRound();

	//һ�ֽ���
	void EndRound();

	void NewFallBlock(float);
	

public:
	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init() override;

	// implement the "static create()" method manually
	CREATE_FUNC(PlayTetrisLayer);

	virtual void update(float delta) override;

private:
	PlayTetris m_logic;

	//����֡
	void OperatorFrame();

	virtual void onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	virtual void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) override;

	void SetFallBlockDir(int dir, bool isToEnd = false);

	float m_operatorFrameTime = 0.0f;

	//ˮƽ��������
	std::vector<int> m_horizontalMoveInput;

	//ˮƽλ�� 0��λ�ƣ�1���ң�-1����
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
	std::array<std::array<std::array<cocos2d::Sprite*,2>, TetrisData::Width>, TetrisData::Height> m_blockSpriteMap;

	void SetBlockOnPos(BlockType block, int x, int y);

	void RemoveLine(int line);

	void RemoveAllBlock();

	void DeleteBlockSprite(cocos2d::Node*);

	void DeleteNextFallBlockNode(cocos2d::Node*);

private:
	//����ָ�����͵ķ���
	std::array<cocos2d::Sprite*, 2> CreateBlockS(BlockType block);

	cocos2d::Node* m_rootNode = nullptr;

private:
	//���䷽��ͼ�ε�Ԫ
	class FallBlockSprite
	{

	public:
		FallBlockSprite(cocos2d::Node* parent);

		~FallBlockSprite();

		void SetPosition(int x, int y);

		//���÷��鷽��
		void SetDir(int dir);

		//���÷�������
		void SetBlockType(BlockType block);

		void End();

		void Begin();

		void SetScale(float scale);

	private:
		cocos2d::Node* m_root = nullptr;
		int m_x;
		int m_y;
		BlockType m_block = BlockType::Empty;
		cocos2d::Node* m_parent = nullptr;

	};

	//���䷽��
	FallBlockSprite* m_fallBlock;
	
	cocos2d::Node* m_nextFallBlockRoot = nullptr;

	std::array<FallBlockSprite*, 3> m_nextFallBlock;

	GameScoreLayer* m_scoreLayer = nullptr;

};



#endif // __GAME_SCENE_H__