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

    return cocos2d::Sprite::createWithSpriteFrameName(blockTag[index]);
};

std::array<cocos2d::Sprite*, 2> CreateBlockAndShadow (BlockType block, bool needShdow = true)
{
    std::array<cocos2d::Sprite*, 2> rev;
    auto sp = CreatBlockSprite(block);
    sp->setLocalZOrder((int)block + 100);
    sp->setOpacity(255);
    rev[0] = sp;
    if (!needShdow)
    {
        sp->setOpacity(100);
    }

    rev[1] = nullptr;
    if(needShdow)
    {
        sp = CreatBlockSprite(block);
        sp->setLocalZOrder((int)block);
        sp->setOpacity(100);
        rev[1] = sp;
    }

    return rev;
}

std::array<cocos2d::Sprite*, 2> CreateBlockAndShadow(BlockType block, cocos2d::Node* parent, bool needShdow = true)
{
    auto a = CreateBlockAndShadow(block, needShdow);
    for (auto c : a)
        if (c)
            parent->addChild(c);
    return a;
}


//示例
// ComRender *render = static_cast<ComRender*>(_rootNode->getChildByTag(10025)->getComponent("GUIComponent"));
// Widget* widget = static_cast<cocos2d::ui::Widget*>(render->getNode());
// Button* button = static_cast<Button*>(widget->getChildByName("Button_156"));
// button->addTouchEventListener(CC_CALLBACK_2(UIComponentTest::touchEvent, this));


#define BLOCK_SIZE 32
#define BLOCK_CENTER 16

#define BLOCK_POS_X_2_PX(x) (x * BLOCK_SIZE + BLOCK_CENTER)
#define BLOCK_POS_X_2_PX_S(x) (x * BLOCK_SIZE + BLOCK_CENTER + 5)

#define BLOCK_POS_Y_2_PY(y) (y * BLOCK_SIZE + BLOCK_CENTER)
#define BLOCK_POS_Y_2_PY_S(y) (y * BLOCK_SIZE + BLOCK_CENTER + (10.0f - (float)y) / 10.0f * 5)

#define CTRL_MOVE_BLOCK_DETAL 0.03f

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
    delete m_previewFallBlock;
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
        if (m_operatorFrameTime >= CTRL_MOVE_BLOCK_DETAL)
        {
            m_operatorFrameTime -= CTRL_MOVE_BLOCK_DETAL;
            OperatorFrame();
        }
    }

    //驱动逻辑帧
    m_logic.Frame(delta);

	m_scoreLayer->SetPlayTimeTick(m_logic.GetGamePlayMS()/10);

    cocos2d::Layer::update(delta);

}

void PlayTetrisLayer::OperatorFrame()
{

    if (m_horizontalMoveDir == 0 && !m_horizontalMoveInput.empty())
    {
        m_horizontalMoveDir = m_horizontalMoveInput.front();
    }

    if (m_horizontalMoveDir == 1)
    {
        //右
        m_logic.MoveBlock(1);
    }
    else if(m_horizontalMoveDir == -1)
    {
        //左
        m_logic.MoveBlock(-1);
    }
    m_horizontalMoveDir = 0;
}

void PlayTetrisLayer::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        event->stopPropagation();
        BeginHorMoveInput(-1);
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        event->stopPropagation();
        BeginHorMoveInput(1);
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
}

void PlayTetrisLayer::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event)
{
    switch (keyCode)
    {
    case cocos2d::EventKeyboard::KeyCode::KEY_LEFT_ARROW:
        EndHorMoveInput(-1);
        break;
    case cocos2d::EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
        EndHorMoveInput(1);
        break;
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
    m_previewFallBlock = new FallBlockSprite(1, m_rootNode);
    m_fallBlock = new FallBlockSprite(0, m_rootNode);

	m_nextFallBlock[0] = new NextFallBlockSprite(n->getChildByName("Next1"));
    m_nextFallBlock[0]->SetPosition(0,0);

	m_nextFallBlock[1] = new NextFallBlockSprite(n->getChildByName("Next2"));
    m_nextFallBlock[1]->SetPosition(0, 0);

	m_nextFallBlock[2] = new NextFallBlockSprite(n->getChildByName("Next3"));
    m_nextFallBlock[2]->SetPosition(0, 0);

    m_scoreLayer = GameScoreLayer::create();
    addChild(m_scoreLayer);

    m_UILayer = CSLoader::createNode("scene/UILayer.csb");
    addChild(m_UILayer);

	size = cocos2d::Director::getInstance()->getVisibleSize();
	m_UILayer->setContentSize(size);

	auto r = m_UILayer->getChildByName("CtrlRoot");
    auto btn = dynamic_cast<ui::Button*>(r->getChildByName("LeftMove"));
    btn->addTouchEventListener(CC_CALLBACK_2(PlayTetrisLayer::TouchEvent, this));
    btn = dynamic_cast<ui::Button*>(r->getChildByName("RightMove"));
    btn->addTouchEventListener(CC_CALLBACK_2(PlayTetrisLayer::TouchEvent, this));
	btn = dynamic_cast<ui::Button*>(r->getChildByName("DownMove"));
	btn->addTouchEventListener(CC_CALLBACK_2(PlayTetrisLayer::TouchEvent, this));
	btn = dynamic_cast<ui::Button*>(r->getChildByName("FallMove"));
	btn->addTouchEventListener(CC_CALLBACK_2(PlayTetrisLayer::TouchEvent, this));


    auto listenerKeyboard = EventListenerKeyboard::create();
    listenerKeyboard->onKeyPressed = CC_CALLBACK_2(PlayTetrisLayer::onKeyPressed, this);
    listenerKeyboard->onKeyReleased = CC_CALLBACK_2(PlayTetrisLayer::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listenerKeyboard, this);


	auto listener = EventListenerTouchOneByOne::create();
	listener->onTouchBegan = CC_CALLBACK_2(PlayTetrisLayer::onTouchBegan, this);
	listener->onTouchMoved = CC_CALLBACK_2(PlayTetrisLayer::onTouchMoved, this);
	listener->onTouchEnded = CC_CALLBACK_2(PlayTetrisLayer::onTouchEnded, this);
	listener->onTouchCancelled = CC_CALLBACK_2(PlayTetrisLayer::onTouchCancelled, this);
	listener->setSwallowTouches(true);//不向下传递触摸
	_eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

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
        {
            break;
        }
        RemoveLine(i);
    }

    this->scheduleOnce([&](float dt)
    {
        m_logic.FallBlockAfterDel();
    }, 0.2f,"fallBlockAfterDel");

    int lineC = m_logic.CleanLinesCount();
    m_scoreLayer->SetCleanLinesCount(lineC);
    m_scoreLayer->SetGameScore(m_logic.GetGameScore());
    m_scoreLayer->SetGameLevel(m_logic.GetGameLevel());

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
            auto actionTo = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(BLOCK_POS_X_2_PX(p.second.first), BLOCK_POS_Y_2_PY(p.second.second)));
            sp[0]->runAction(actionTo);
            actionTo = cocos2d::MoveTo::create(0.1f, cocos2d::Vec2(BLOCK_POS_X_2_PX_S(p.second.first), BLOCK_POS_Y_2_PY_S(p.second.second)));
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
    m_previewFallBlock->End();

}

void PlayTetrisLayer::OnNewBlock(BlockType block, int dir, int x, int y)
{
    m_fallBlock->SetBlockType(block);
    m_fallBlock->SetDir(dir);
    m_fallBlock->Begin();
    m_previewFallBlock->SetBlockType(block);
    m_previewFallBlock->SetDir(dir);
    m_previewFallBlock->Begin();
    OnBlockMove(x,y);

	if (m_logic.IsEnd())
	{
		m_nextFallBlock[0]->SetShow(false);
	}
	else
	{
		for (int i = 0; i < 3; ++i)
		{
			auto d = m_logic.GetNextFallBlock(i);
			m_nextFallBlock[i]->SetBlockType((BlockType)d.first);
			m_nextFallBlock[i]->SetDir();
			m_nextFallBlock[i]->SetShow(true);
		}
	}
}

void PlayTetrisLayer::OnBlockMove(int x, int y)
{
    m_fallBlock->SetPosition(x, y);
    //刷新预览方块的位置
    auto pos = m_logic.GetPreviewFallBlockPos();
    m_previewFallBlock->SetPosition(pos.first, pos.second);

}

void PlayTetrisLayer::OnBlockTurn(int dir)
{
    SetFallBlockDir(dir);
}

void PlayTetrisLayer::NewRound()
{
    //清除以前的数据
    RemoveAllBlock();
    m_logic.NewRound();

    //生成随机方块，该步骤可以没有
    m_logic.ControlNewFallBlock();

    m_scoreLayer->SetGameScore(0);
    m_scoreLayer->SetGameLevel(1);
    m_scoreLayer->SetCleanLinesCount(0);
}

void PlayTetrisLayer::SetFallBlockDir(int dir, bool isToEnd)
{
    m_fallBlock->SetDir(dir);
    m_previewFallBlock->SetDir(dir);
    //刷新预览方块的位置
    auto pos = m_logic.GetPreviewFallBlockPos();
    m_previewFallBlock->SetPosition(pos.first, pos.second);

}

void PlayTetrisLayer::SetBlockOnPos(BlockType block, int x, int y)
{
    auto sp = CreateBlockAndShadow (block);
    sp[0]->setPosition(BLOCK_POS_X_2_PX(x), BLOCK_POS_Y_2_PY(y));
    sp[1]->setPosition(BLOCK_POS_X_2_PX_S(x), BLOCK_POS_Y_2_PY_S(y));
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
	//游戏结束
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

void PlayTetrisLayer::TouchEvent(cocos2d::Ref *pSender, cocos2d::ui::Widget::TouchEventType type)
{
    switch (type)
    {
    case cocos2d::ui::Widget::TouchEventType::BEGAN:
    {
        ui::Button *btn = (ui::Button*)pSender;
        if (btn->getName().compare("LeftMove") == 0)
        {
            BeginHorMoveInput(-1);
        }
        else if (btn->getName().compare("RightMove") == 0)
        {
            BeginHorMoveInput(1);
        }
		else if (btn->getName().compare("DownMove") == 0)
		{
			m_logic.ControlBlockFall(1);
		}
		else if (btn->getName().compare("FallMove") == 0)
		{
			m_logic.ControlBlockFall(21);
		}

    }
    break;
    case cocos2d::ui::Widget::TouchEventType::MOVED:
        break;
    case cocos2d::ui::Widget::TouchEventType::ENDED:
    {
        ui::Button *btn = (ui::Button*)pSender;
        if (btn->getName().compare("LeftMove") == 0)
        {
            EndHorMoveInput(-1);
        }
        else if (btn->getName().compare("RightMove") == 0)
        {
            EndHorMoveInput(1);
        }

    }
    break;
    case cocos2d::ui::Widget::TouchEventType::CANCELED:
    {
        ui::Button *btn = (ui::Button*)pSender;
        if (btn->getName().compare("LeftMove") == 0)
        {
            EndHorMoveInput(-1);
        }
        else if (btn->getName().compare("RightMove") == 0)
        {
            EndHorMoveInput(1);
        }

    }
    break;
    default:
        break;
    }
}

void PlayTetrisLayer::BeginHorMoveInput(int i)
{
    if (i != 0)
    {
        m_horizontalMoveDir = i;
        m_logic.MoveBlock(m_horizontalMoveDir);
        //启动连续位移
        m_holdInput = true;
        m_operatorFrameTime = -0.33f;
        //
        m_horizontalMoveInput.insert(m_horizontalMoveInput.begin(), i);
    }
}

void PlayTetrisLayer::EndHorMoveInput(int i)
{
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

bool PlayTetrisLayer::onTouchBegan(Touch *touch, Event *unused_event)
{
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
	return true;
}

void PlayTetrisLayer::onTouchMoved(Touch *touch, Event *unused_event)
{
}

void PlayTetrisLayer::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void PlayTetrisLayer::onTouchCancelled(Touch *touch, Event *unused_event)
{
}

PlayTetrisLayer::FallBlockSprite::FallBlockSprite(int type, cocos2d::Node* parent)
    :m_parent(parent)
    , m_type(type)
{
    for (auto c : m_blockSprite)
        c.fill(nullptr);
}

PlayTetrisLayer::FallBlockSprite::~FallBlockSprite()
{
    for (auto c : m_blockSprite)
        for (auto s : c)
            m_parent->removeChild(s);
}

void PlayTetrisLayer::FallBlockSprite::SetPosition(int x, int y)
{
    m_x = x;
    m_y = y;
    SetDir(m_dir);
}

void PlayTetrisLayer::FallBlockSprite::SetDir(int dir)
{
    m_dir = dir;
    auto& d = FallBlock::GetBlockData(m_block, dir);
    for (int i = 0; i < 4; ++i)
    {
        int x = d[i].first + m_x;
        int y = d[i].second + m_y;
        auto n = m_blockSprite[i];
        n[0]->setPosition(BLOCK_POS_X_2_PX(x), BLOCK_POS_Y_2_PY(y));
        if (m_type == 0)
            n[1]->setPosition(BLOCK_POS_X_2_PX_S(x), BLOCK_POS_Y_2_PY_S(y));
    }
}

void PlayTetrisLayer::FallBlockSprite::SetBlockType(BlockType block)
{
    if (m_block != block)
    {
        m_block = block;
        for (auto c : m_blockSprite)
            for (auto s : c)
                m_parent->removeChild(s);

        //创建指定的方块和投影
        m_blockSprite[0] = CreateBlockAndShadow (block, m_parent, m_type == 0);
        m_blockSprite[1] = CreateBlockAndShadow(block, m_parent, m_type == 0);
        m_blockSprite[2] = CreateBlockAndShadow(block, m_parent, m_type == 0);
        m_blockSprite[3] = CreateBlockAndShadow(block, m_parent, m_type == 0);

    }
}

void PlayTetrisLayer::FallBlockSprite::End()
{
    for (auto c : m_blockSprite)
        for (auto s : c)
            if (s)
                s->setVisible(false);
}

void PlayTetrisLayer::FallBlockSprite::Begin()
{
    for (auto c : m_blockSprite)
        for (auto s : c)
            if (s)
                s->setVisible(true);
}

void PlayTetrisLayer::FallBlockSprite::SetScale(float scale)
{
    m_scale = scale;
}

PlayTetrisLayer::NextFallBlockSprite::NextFallBlockSprite(cocos2d::Node* parent)
    :m_parent(parent)
{
    m_root = cocos2d::Node::create();
    parent->addChild(m_root);
}


PlayTetrisLayer::NextFallBlockSprite::~NextFallBlockSprite()
{
    m_parent->removeChild(m_root);
}

void PlayTetrisLayer::NextFallBlockSprite::SetPosition(int x, int y)
{
    m_root->setPosition(x, y);
}

void PlayTetrisLayer::NextFallBlockSprite::SetDir()
{
    auto& d = FallBlock::GetBlockData(m_block, 4);
    for (int i = 0; i < 4; ++i)
    {
        auto n = m_root->getChildByTag(i + 1);
        n->setPosition(BLOCK_POS_X_2_PX(d[i].first), BLOCK_POS_Y_2_PY(d[i].second));
        n = m_root->getChildByTag(i + 11);
        n->setPosition(BLOCK_POS_X_2_PX(d[i].first) + 5, BLOCK_POS_Y_2_PY(d[i].second) + 5);
    }
}

void PlayTetrisLayer::NextFallBlockSprite::SetBlockType(BlockType block)
{
    if (m_block != block)
    {
        m_block = block;
        m_root->removeAllChildren();

        auto sp = CreateBlockAndShadow (block);
        m_root->addChild(sp[0], 0, 1);
        m_root->addChild(sp[1], 0, 11);
        sp = CreateBlockAndShadow (block);
        m_root->addChild(sp[0], 0, 2);
        m_root->addChild(sp[1], 0, 12);
        sp = CreateBlockAndShadow (block);
        m_root->addChild(sp[0], 0, 3);
        m_root->addChild(sp[1], 0, 13);
        sp = CreateBlockAndShadow (block);
        m_root->addChild(sp[0], 0, 4);
        m_root->addChild(sp[1], 0, 14);
    }
}

void PlayTetrisLayer::NextFallBlockSprite::SetScale(float scale)
{
    m_root->setScale(scale);
}

void PlayTetrisLayer::NextFallBlockSprite::SetShow(bool isShow)
{
	m_root->setVisible(isShow);
}
