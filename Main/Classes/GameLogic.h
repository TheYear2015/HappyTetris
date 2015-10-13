#ifndef __GAME_LOGIC_H__
#define __GAME_LOGIC_H__

#include <array>
#include <map>
#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "cocostudio/CocoStudio.h"


class MyRand
{
public:
	void SRand(uint32_t s);
	uint32_t Rand() const;
private:
	mutable	uint32_t m_r = 11;
};


//块的类型
enum class BlockType
{
	Empty = 0,
	NeedFill = 1,//需要下落填充
	Gray = 8,//灰块
	IBlock = 10,//长条
	LBlock,
	JBlock,
	SBlock,
	ZBlock,
	OBlock,
	TBlock
};



class BlockGenerator
{
public:
	BlockType GetNewBlock();

	void SetSeed(uint32_t seed);
private:
	std::queue<BlockType> m_queue;

	MyRand m_rand;
};


//游戏数据
class TetrisData
{
public:
	static const int Width = 10;
	static const int Height = 20;

public:
	TetrisData();
	~TetrisData();

public:
	typedef  std::array<std::array<BlockType, Width>, Height> DataType;

public:
	//清空全部填充为Empty
	void Clean();

	//删除一行
	void DelLine(int line);

	//删除行以后下落方块进行填充
	void FallBlockAfterDel();

	//判断一行是否被填满
	bool IsFillFullLine(int line) const;

	//判断指定的格子是否为空
	bool IsBlockEmpty(int x, int y) const;

	//设置方块
	void SetBlock(int x, int y, BlockType block);

	//获得被填填满的行
	std::array<int, Height> GetFillFullLines() const;

	//获得静止的方块数据
	DataType GetStaticData() const;

	//获得方块下落填充的信息，从哪填到哪
	const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& BlockFallInfo() const { return m_blockFallInfo; }

private:
	//静止的方块数据，不包含下落中的可操作的方块
	DataType m_staticData;
	
	//记录方块下落填充的信息，从哪填到哪
	std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> m_blockFallInfo;
};

//方块进行中的状态
enum class BlockPlayStatus
{
	Op = 1,//操作中
	BlockFallEnd,//下落结束
	BlockClean,//消除中
	BlockFallFill,//自动填充中
	BeginOp,//出新块，开始操作
	PlayEnd,//end
};

//下落的方块
class FallBlock
{
public:
	typedef std::array<std::pair<int, int>, 4> BlockData;

	//0~3是四个方向，5是界面显示的位置
	static std::map<BlockType, std::array<BlockData,5>> BlockSet;

public:
	BlockType Block() const;
	void Block(BlockType b);
	int Dir() const;
	void Dir(int dir);
	//获得方块的形状数据
	const BlockData& GetBlockData() const;
	const BlockData& GetBlockData(int dir) const;

	//获得方块的形状数据
	static const BlockData& GetBlockData(BlockType block, int dir);

private:
	//方块的类型
	BlockType m_block = BlockType::IBlock;
	//方块的方向 上左下右 逆时针(0,1,2,3)
	int m_blockDir = 0;

};

class PlayTetrisObserver
{
public:
	//当行被销毁时
	virtual void OnlineClean(const std::array<int, TetrisData::Height>& lines) = 0;

	//当行被销毁后，方块下落填充结束
	virtual void OnBlocksFallFill(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>&) = 0;

	//方块下落结束
	virtual void OnBlockFallEnd(BlockType block, const std::array<std::pair<int,int>, 4>& pos) = 0;

	//生成新的方块
	virtual void OnNewBlock(BlockType block, int dir, int x, int y) = 0;

	//方块移动
	virtual void OnBlockMove(int x, int y) = 0;

	//方块旋转
	virtual void OnBlockTurn(int dir) = 0;

	//无法创建新的方块
	virtual void OnNewBlockFailed() = 0;
};

//方块游戏主体逻辑
class PlayTetris
{
public:
	PlayTetris();
	~PlayTetris();

public:
	void Frame(float delta);

	//方块下落的帧，500ms一帧，进行方块下落的计算，新的方块的产生
	void FallFrame();

	void ProcBlockFallEnd();

	void SetObserver(PlayTetrisObserver* observer);

	//新的一局
	void NewRound();

	//设置下落速度等级
	void SetFallLevel(int level);

	//设置下落速度等级区间
	void SetFallLevelRang(int min, int max);

	//获得下落的行数
	int CleanLinesCount() const { return m_cleanLinesCount; }

public:

	//旋转下落方块
	void TurnFallBlock();

	//水平移动下落方块
	void MoveBlock(int dir);

	//当块消除后进行下落填充
	void FallBlockAfterDel();

	void ControlNewFallBlock();

	//主动让方块下落
	void ControlBlockFall(int step);

	//是否已经结束
	bool IsEnd() const;

	void CleanAllBlock();

	int FallBlockCount() const { return m_fallBlockCount; }

	//获得下一个块的信息(0~2)
	std::pair<int, int> GetNextFallBlock(int index) const;

	//获得预览方块的位置
	std::pair<int, int> GetPreviewFallBlockPos() const;
	
private:
	//产生一个新的方块，并开始下落
	void NewFallBlock();

	//下落方块，step下落的步长
	void BlockFall(int step);

	//下落方块是否可以出现在指定的位置
	bool IsFallBlockEnablePos(int x, int y, int dir) const;

	//下落方块的状态
	BlockPlayStatus m_fallBlockStatus = BlockPlayStatus::BeginOp;

	//下落的方块的位置
	std::pair<int, int> m_fallBlockPos;

	//下落的方块
	FallBlock m_fallBlock;

	//静止的方块的数据
	TetrisData m_data;

	//是否下落到底部
	bool IsFallOnEnd() const;

	//掉落到底部
	void FallEnd();

	//开始消除满的行
	void BeginCleanFullLines();

	//下落帧的计时
	float m_fallRunTime = 0.0f;

	//下落速度等级
	int m_fallSpeedLevel = 1;

	//消除的行数
	int m_cleanLinesCount = 0;

	//出现的方块的个数
	int m_fallBlockCount = 0;

	float m_fallDetal = 0.5f;

	//下落到底后的固定时间
	const float m_fallEndDetal = 0.3f;

	//下落速度等级区间
	std::pair<int, int> m_fallSpeedLevelRang = { 1, 15 };

	PlayTetrisObserver* m_observer = nullptr;

	mutable BlockGenerator m_newFallBlockRand;

	mutable std::vector<std::pair<int,int>> m_newFallBlockInfo;
};

#endif // __GAME_LOGIC_H__
