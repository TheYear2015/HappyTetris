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


//�������
enum class BlockType
{
	Empty = 0,
	NeedFill = 1,//��Ҫ�������
	Gray = 8,//�ҿ�
	IBlock = 10,//����
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


//��Ϸ����
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
	//���ȫ�����ΪEmpty
	void Clean();

	//ɾ��һ��
	void DelLine(int line);

	//ɾ�����Ժ����䷽��������
	void FallBlockAfterDel();

	//�ж�һ���Ƿ�����
	bool IsFillFullLine(int line) const;

	//�ж�ָ���ĸ����Ƿ�Ϊ��
	bool IsBlockEmpty(int x, int y) const;

	//���÷���
	void SetBlock(int x, int y, BlockType block);

	//��ñ�����������
	std::array<int, Height> GetFillFullLines() const;

	//��þ�ֹ�ķ�������
	DataType GetStaticData() const;

	//��÷�������������Ϣ���������
	const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>& BlockFallInfo() const { return m_blockFallInfo; }

private:
	//��ֹ�ķ������ݣ������������еĿɲ����ķ���
	DataType m_staticData;
	
	//��¼��������������Ϣ���������
	std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> m_blockFallInfo;
};

//��������е�״̬
enum class BlockPlayStatus
{
	Op = 1,//������
	BlockFallEnd,//�������
	BlockClean,//������
	BlockFallFill,//�Զ������
	BeginOp,//���¿飬��ʼ����
	PlayEnd,//end
};

//����ķ���
class FallBlock
{
public:
	typedef std::array<std::pair<int, int>, 4> BlockData;

	//0~3���ĸ�����5�ǽ�����ʾ��λ��
	static std::map<BlockType, std::array<BlockData,5>> BlockSet;

public:
	BlockType Block() const;
	void Block(BlockType b);
	int Dir() const;
	void Dir(int dir);
	//��÷������״����
	const BlockData& GetBlockData() const;
	const BlockData& GetBlockData(int dir) const;

	//��÷������״����
	static const BlockData& GetBlockData(BlockType block, int dir);

private:
	//���������
	BlockType m_block = BlockType::IBlock;
	//����ķ��� �������� ��ʱ��(0,1,2,3)
	int m_blockDir = 0;

};

class PlayTetrisObserver
{
public:
	//���б�����ʱ
	virtual void OnlineClean(const std::array<int, TetrisData::Height>& lines) = 0;

	//���б����ٺ󣬷�������������
	virtual void OnBlocksFallFill(const std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>&) = 0;

	//�����������
	virtual void OnBlockFallEnd(BlockType block, const std::array<std::pair<int,int>, 4>& pos) = 0;

	//�����µķ���
	virtual void OnNewBlock(BlockType block, int dir, int x, int y) = 0;

	//�����ƶ�
	virtual void OnBlockMove(int x, int y) = 0;

	//������ת
	virtual void OnBlockTurn(int dir) = 0;

	//�޷������µķ���
	virtual void OnNewBlockFailed() = 0;
};

//������Ϸ�����߼�
class PlayTetris
{
public:
	PlayTetris();
	~PlayTetris();

public:
	void Frame(float delta);

	//���������֡��500msһ֡�����з�������ļ��㣬�µķ���Ĳ���
	void FallFrame();

	void ProcBlockFallEnd();

	void SetObserver(PlayTetrisObserver* observer);

	//�µ�һ��
	void NewRound();

	//���������ٶȵȼ�
	void SetFallLevel(int level);

	//���������ٶȵȼ�����
	void SetFallLevelRang(int min, int max);

	//������������
	int CleanLinesCount() const { return m_cleanLinesCount; }

public:

	//��ת���䷽��
	void TurnFallBlock();

	//ˮƽ�ƶ����䷽��
	void MoveBlock(int dir);

	//��������������������
	void FallBlockAfterDel();

	void ControlNewFallBlock();

	//�����÷�������
	void ControlBlockFall(int step);

	//�Ƿ��Ѿ�����
	bool IsEnd() const;

	void CleanAllBlock();

	int FallBlockCount() const { return m_fallBlockCount; }

	//�����һ�������Ϣ(0~2)
	std::pair<int, int> GetNextFallBlock(int index) const;

	//���Ԥ�������λ��
	std::pair<int, int> GetPreviewFallBlockPos() const;
	
private:
	//����һ���µķ��飬����ʼ����
	void NewFallBlock();

	//���䷽�飬step����Ĳ���
	void BlockFall(int step);

	//���䷽���Ƿ���Գ�����ָ����λ��
	bool IsFallBlockEnablePos(int x, int y, int dir) const;

	//���䷽���״̬
	BlockPlayStatus m_fallBlockStatus = BlockPlayStatus::BeginOp;

	//����ķ����λ��
	std::pair<int, int> m_fallBlockPos;

	//����ķ���
	FallBlock m_fallBlock;

	//��ֹ�ķ��������
	TetrisData m_data;

	//�Ƿ����䵽�ײ�
	bool IsFallOnEnd() const;

	//���䵽�ײ�
	void FallEnd();

	//��ʼ����������
	void BeginCleanFullLines();

	//����֡�ļ�ʱ
	float m_fallRunTime = 0.0f;

	//�����ٶȵȼ�
	int m_fallSpeedLevel = 1;

	//����������
	int m_cleanLinesCount = 0;

	//���ֵķ���ĸ���
	int m_fallBlockCount = 0;

	float m_fallDetal = 0.5f;

	//���䵽�׺�Ĺ̶�ʱ��
	const float m_fallEndDetal = 0.3f;

	//�����ٶȵȼ�����
	std::pair<int, int> m_fallSpeedLevelRang = { 1, 15 };

	PlayTetrisObserver* m_observer = nullptr;

	mutable BlockGenerator m_newFallBlockRand;

	mutable std::vector<std::pair<int,int>> m_newFallBlockInfo;
};

#endif // __GAME_LOGIC_H__
