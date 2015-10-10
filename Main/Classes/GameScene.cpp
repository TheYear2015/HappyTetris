#include "GameScene.h"
#include "cocostudio/CocoStudio.h"
#include "ui/CocosGUI.h"

USING_NS_CC;

using namespace cocostudio::timeline;

const char* GameScene::GetSceneCsb() const
{
	return "scene/PlayGame.csb";
}

void GameScene::OnSceneInited()
{
	//创建Layer
	auto lay = PlayTetrisLayer::create();
	if (lay)
	{
		addChild(lay);
	}
}

cocos2d::Sprite* CreatBlockSprite(BlockType block)
{
	int index = (int)block - (int)BlockType::IBlock;
	const std::array<std::string, 7> blockTag = { "img/red_t.png", "img/o_t.png", "img/blue_t.png", "img/p_t.png", "img/g_t.png", "img/y_t.png", "img/c_t.png" };
	return cocos2d::Sprite::create(blockTag[index]);
}

#define BLOCK_SIZE 32
#define BLOCK_CENTER 16

PlayTetrisLayer::PlayTetrisLayer()
{
	m_nextFallBlock.fill(nullptr);
	for (auto& line : m_blockSpriteMap)
	{
		line.fill({ nullptr, nullptr });
	}
}

PlayTetrisLayer::~PlayTetrisLayer()
{
	delete m_fallBlock;
	for (auto n : m_nextFallBlock)
	{
		delete n;
	}
}

void PlayTetrisLayer::update(float delta)
{
	if (m_holdInput)
	{
		//连续位移操作
		m_operatorFrameTime += delta;
		if (m_operatorFrameTime >= 0.05f)
		{
			m_operatorFrameTime -= 0.05f;
			OperatorFrame();
		}
	}

	//驱动逻辑帧
	m_logic.Frame(delta);

	cocos2d::Layer::update(delta);

}

void PlayTetrisLayer::OperatorFrame()
{

	if (m_horizontalMoveDir == 0 && !m_horizontalMoveInput.empty())
	{
		m_horizontalMoveDir = m_horizontalMoveInput.front();
	}

	if (m_horizontalMoveDir == 1)
	{//右
		m_logic.MoveBlock(1);
	}
	else if(m_horizontalMoveDir == -1)
	{//左
		m_logic.MoveBlock(-1);
	}
	m_horizontalMoveDir = 0;
}

void PlayTetrisLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	int i = 0;
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		i = -1;
		event->stopPropagation();
		m_horizontalMoveDir = -1;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		i = 1;
		event->stopPropagation();
		m_horizontalMoveDir = 1;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_UP_ARROW:
		event->stopPropagation();
		m_logic.ControlBlockFall(21);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		event->stopPropagation();
		m_logic.ControlBlockFall(1);
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_SPACE:
		event->stopPropagation();
		if (m_logic.IsEnd())
		{
			//重新开始
			NewRound();

		}
		else
		{
			//旋转
			m_logic.TurnFallBlock();

		}
		break;
	}
	if (i != 0)
	{
		m_logic.MoveBlock(m_horizontalMoveDir);
		//启动连续位移
		m_holdInput = true;
		m_operatorFrameTime = -0.33f;
		//
		m_horizontalMoveInput.insert(m_horizontalMoveInput.begin(), i);
	}
}

void PlayTetrisLayer::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
	int i = 0;
	switch (keyCode)
	{
	case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
		i = -1;
		break;
	case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
		i = 1;
		break;
	}

	if (i != 0)
	{
		auto it = std::remove(m_horizontalMoveInput.begin(), m_horizontalMoveInput.end(), i);
		m_horizontalMoveInput.erase(it, m_horizontalMoveInput.end());
	}

	if (m_horizontalMoveInput.empty())
	{
		m_holdInput = false;
	}
}

bool PlayTetrisLayer::init()
{
	//////////////////////////////
	// 1. super init first
	if (!Layer::init())
	{
		return false;
	}

	auto n = CSLoader::createNode("scene/PlayTetrisLayer.csb");
	if (!n)
	{
		return false;
	}
	auto size = Director::getInstance()->getVisibleSize();
	n->setContentSize(size);
	addChild(n);
	m_rootNode = n->getChildByName("Root");
	m_fallBlock = new FallBlockSprite(m_rootNode);

	m_nextFallBlockRoot = CSLoader::createNode("scene/NextFallBlockLayer.csb");
	addChild(m_nextFallBlockRoot);
	m_nextFallBlockRoot->setPosition(0, 0);

	for (int i = 0; i < m_nextFallBlock.size(); ++i)
	{
		m_nextFallBlock[i] = new FallBlockSprite(m_nextFallBlockRoot);
		m_nextFallBlock[i]->SetPosition(32, 600 - i * 100);
		m_nextFallBlock[i]->SetScale(0.7f);
	}
	m_scoreLayer = GameScoreLayer::create();
	addChild(m_scoreLayer);

	auto listenerKeyboard = EventListenerKeyboard::create();
	listenerKeyboard->onKeyPressed = CC_CALLBACK_2(PlayTetrisLayer::onKeyPressed, this);
	listenerKeyboard->onKeyReleased = CC_CALLBACK_2(PlayTetrisLayer::onKeyReleased, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);

	m_logic.SetObserver(this);

	this->scheduleUpdate();

	NewRound();
	return true;
}

void PlayTetrisLayer::OnlineClean(const std::array<int, TetrisData::Height>& lines)
{
	for (auto i : lines)
	{
		if (i < 0)
			break;
		RemoveLine(i);
	}

	this->scheduleOnce([&](float dt) { m_logic.FallBlockAfterDel(); }, 0.2f,"fallBlockAfterDel");

	int lineC = m_logic.CleanLinesCount();
	m_logic.SetFallLevel(lineC / 10 + 1);

	m_scoreLayer->SetCleanLinesCount(lineC);
	m_scoreLayer->SetGameScore(lineC * 1000);
	m_scoreLayer->SetSpeedLevel(lineC / 10 + 1);

}

void PlayTetrisLayer::OnBlocksFallFill(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& data)
{
	for (auto& p : data)
	{
		auto sp = m_blockSpriteMap[p.first.second][p.first.first];
		if (sp[0])
		{
			m_blockSpriteMap[p.first.second][p.first.first] = { nullptr, nullptr };
			m_blockSpriteMap[p.second.second][p.second.first] = sp;

			//move to
			auto actionTo = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(p.second.first * BLOCK_SIZE + BLOCK_CENTER, p.second.second * BLOCK_SIZE + BLOCK_CENTER));
			sp[0]->runAction(actionTo);
			actionTo = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(p.second.first * BLOCK_SIZE + BLOCK_CENTER + 9, p.second.second * BLOCK_SIZE + BLOCK_CENTER - 3));
			sp[1]->runAction(actionTo);
		}
	}

	this->scheduleOnce((SEL_SCHEDULE)&PlayTetrisLayer::NewFallBlock, 0.15f);
}

void PlayTetrisLayer::OnBlockFallEnd(BlockType block, const std::array<std::pair<int, int>, 4>& pos)
{
	//获得数据,产生方块
	for (auto& p : pos)
	{
		SetBlockOnPos(block, p.first, p.second);
	}

	m_fallBlock->End();

}

void PlayTetrisLayer::OnNewBlock(BlockType block, int dir, int x, int y)
{
	m_fallBlock->SetBlockType(block);
	m_fallBlock->SetDir(dir);
	m_fallBlock->Begin();
	OnBlockMove(x,y);

	for (int i = 0; i < 3; ++i)
	{
		auto d = m_logic.GetNextFallBlock(i);
		m_nextFallBlock[i]->SetBlockType((BlockType)d.first);
		m_nextFallBlock[i]->SetDir(0);
	}
}

void PlayTetrisLayer::OnBlockMove(int x, int y)
{
	m_fallBlock->SetPosition(x * BLOCK_SIZE + BLOCK_CENTER, y * BLOCK_SIZE + BLOCK_CENTER);
}

void PlayTetrisLayer::OnBlockTurn(int dir)
{
	SetFallBlockDir(dir);
}

std::array<cocos2d::Sprite*, 2> PlayTetrisLayer::CreateBlockS(BlockType block)
{
	std::array<cocos2d::Sprite*, 2> rev;
	auto sp = CreatBlockSprite(block);
	sp->setLocalZOrder((int)block + 100);
	sp->setOpacity(255);
	rev[0] = sp;

	sp = CreatBlockSprite(block);
	sp->setLocalZOrder((int)block);
	sp->setOpacity(100);
	rev[1] = sp;

	return rev;

}

void PlayTetrisLayer::NewRound()
{
	//清除以前的数据
	RemoveAllBlock();
	m_logic.NewRound();

	//生成随机方块，该步骤可以没有
	m_logic.ControlNewFallBlock();

	m_scoreLayer->SetGameScore(0);
	m_scoreLayer->SetSpeedLevel(1);
	m_scoreLayer->SetCleanLinesCount(0);
}

void PlayTetrisLayer::SetFallBlockDir(int dir, bool isToEnd)
{
	m_fallBlock->SetDir(dir);
}

void PlayTetrisLayer::SetBlockOnPos(BlockType block, int x, int y)
{
	auto sp = CreateBlockS(block);
	sp[0]->setPosition(x * BLOCK_SIZE + BLOCK_CENTER, y * BLOCK_SIZE + BLOCK_CENTER);
	sp[1]->setPosition(x * BLOCK_SIZE + BLOCK_CENTER + 9, y * BLOCK_SIZE + BLOCK_CENTER - 3);
	m_rootNode->addChild(sp[0]);
	m_rootNode->addChild(sp[1]);
	m_blockSpriteMap[y][x] = sp;

}

void PlayTetrisLayer::RemoveLine(int line)
{
	auto& cellS = m_blockSpriteMap[line];
	for (auto& b : cellS)
	{
		if (b[0])
		{
			auto rotate = cocos2d::RotateBy::create(0.2f, 180);
			auto fun1 = cocos2d::CallFuncN::create(CC_CALLBACK_1(PlayTetrisLayer::DeleteBlockSprite, this));
			b[0]->runAction(cocos2d::Sequence::create(
				cocos2d::Spawn::create(rotate, cocos2d::ScaleBy::create(0.2f, 0.1f), nullptr)
				, fun1, nullptr));

			auto rotate2 = cocos2d::RotateBy::create(0.2f, 180);
			auto fun2 = cocos2d::CallFuncN::create(CC_CALLBACK_1(PlayTetrisLayer::DeleteBlockSprite, this));
			b[1]->runAction(cocos2d::Sequence::create(
				cocos2d::Spawn::create(rotate2, cocos2d::ScaleBy::create(0.2f, 0.01f), nullptr)
				, fun2, nullptr));

			b = { nullptr };
		}
	}
}

void PlayTetrisLayer::OnNewBlockFailed()
{
}

void PlayTetrisLayer::RemoveAllBlock()
{
	for(auto& cellS : m_blockSpriteMap)
		for (auto& b : cellS)
		{
			if (b[0])
			{
				m_rootNode->removeChild(b[0]);
				m_rootNode->removeChild(b[1]);
				b = { nullptr };
			}
		}

}

void PlayTetrisLayer::NewFallBlock(float)
{
	m_logic.ControlNewFallBlock();
}

void PlayTetrisLayer::DeleteBlockSprite(cocos2d::Node* sp)
{
	m_rootNode->removeChild(sp);
}

void PlayTetrisLayer::DeleteNextFallBlockNode(cocos2d::Node* node)
{

}

PlayTetrisLayer::FallBlockSprite::FallBlockSprite(cocos2d::Node* parent)
	:m_parent(parent)
{
	m_root = cocos2d::Node::create();
	parent->addChild(m_root);
}

PlayTetrisLayer::FallBlockSprite::~FallBlockSprite()
{
	m_parent->removeChild(m_root);
}

void PlayTetrisLayer::FallBlockSprite::SetPosition(int x, int y)
{
	m_root->setPosition(x, y);
}

void PlayTetrisLayer::FallBlockSprite::SetDir(int dir)
{
	auto& d = FallBlock::GetBlockData(m_block, dir);
	for (int i = 0; i < 4; ++i)
	{
		auto n = m_root->getChildByTag(i+1);
		n->setPosition(d[i].first * BLOCK_SIZE, d[i].second * BLOCK_SIZE);
	}
}

void PlayTetrisLayer::FallBlockSprite::SetBlockType(BlockType block)
{
	if (m_block != block)
	{
		m_block = block;
		m_root->removeAllChildren();

		//创建指定的方块
		auto sp = CreatBlockSprite(block);
		m_root->addChild(sp,0, 1);
		sp = CreatBlockSprite(block);
		m_root->addChild(sp, 0, 2);
		sp = CreatBlockSprite(block);
		m_root->addChild(sp, 0, 3);
		sp = CreatBlockSprite(block);
		m_root->addChild(sp, 0, 4);
	}
}

void PlayTetrisLayer::FallBlockSprite::End()
{
	m_root->setVisible(false);
}

void PlayTetrisLayer::FallBlockSprite::Begin()
{
	m_root->setVisible(true);
}

void PlayTetrisLayer::FallBlockSprite::SetScale(float scale)
{
	m_root->setScale(scale);
}
