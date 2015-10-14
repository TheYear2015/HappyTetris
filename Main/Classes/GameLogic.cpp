#include "GameLogic.h"


const std::vector<uint32_t> NewFallBlockSeed =
{
	600096	 
	,	679689	 
	,	3532575	 
	,	4041326	 
	,	4648256	 
	,	9098805	 
	,	24996354	 
	,	41383670	 
	,	55501060	 
	,	57440446	 
	,	60163783	 
	,	60905549	 
	,	62028596	 
	,	67920479	 
	,	68755856	 
	,	86866161	 
	,	88398680	 
	,	93206168	 
	,	94319564	 
	,	105276704	 
};

TetrisData::TetrisData()
{
	Clean();
}

TetrisData::~TetrisData()
{

}

void TetrisData::Clean()
{
	for (auto& line : m_staticData)
		std::fill(line.begin(), line.end(), BlockType::Empty);
}

void TetrisData::DelLine(int line)
{
	m_staticData[line].fill(BlockType::NeedFill);
}

bool TetrisData::IsFillFullLine(int line) const
{
	auto& lineD = m_staticData[line];
	for (auto c : lineD)
	{
		if (c == BlockType::Empty)
			return false;
	}
	return true;
}

std::array<int, TetrisData::Height> TetrisData::GetFillFullLines() const
{
	std::array<int, TetrisData::Height> rev;
	std::fill(rev.begin(), rev.end(), -1);
	int i = 0;
	for (int j = 0; j < TetrisData::Height; ++j)
	{
		if (IsFillFullLine(j))
		{
			rev[i] = j;
			++i;
		}
	}
	return rev;
}

void TetrisData::FallBlockAfterDel()
{
	m_blockFallInfo.clear();
	for (int h = 0; h < TetrisData::Height - 1; ++h)
	{
		auto& c = m_staticData[h][0];
		if (c == BlockType::NeedFill)
		{
			int dh = h + 1;
			for (; dh < TetrisData::Height; ++dh)
			{
				if (m_staticData[dh][0] != BlockType::NeedFill)
				{
					break;
				}
			}
			if (dh != TetrisData::Height)
			{
				m_staticData[h] = m_staticData[dh];
				for (int w = 0; w < TetrisData::Width; ++w)
					m_blockFallInfo.push_back(std::make_pair(std::make_pair(w, dh), std::make_pair(w, h)));

				m_staticData[dh].fill(BlockType::NeedFill);
			}
		}
	}
	for (auto& l : m_staticData)
	{
		for (auto& c : l)
		{
			if (c == BlockType::NeedFill)
				c = BlockType::Empty;
		}
	}

}

TetrisData::DataType TetrisData::GetStaticData() const
{
	return m_staticData;
}

bool TetrisData::IsBlockEmpty(int x, int y) const
{
	if (y >= Height)
		return true;
	if (y >= 0 && y < Height && x >= 0 && x < Width)
		return m_staticData[y][x] == BlockType::Empty;
	return false;
}

void TetrisData::SetBlock(int x, int y, BlockType block)
{
	m_staticData[y][x] = block;
}


std::map<BlockType, std::array<FallBlock::BlockData, 5>> FallBlock::BlockSet;

const FallBlock::BlockData& FallBlock::GetBlockData(BlockType block, int dir)
{
	if (BlockSet.empty())
	{
		{
			auto& c = BlockSet[BlockType::IBlock];
			//
			c[0][0] = { -1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 1, 0 };
			c[0][3] = { 2, 0 };
			//
			c[1][0] = { 1, 1 };
			c[1][1] = { 1, 0 };
			c[1][2] = { 1, -1 };
			c[1][3] = { 1, -2 };
			//
			c[2][0] = { -1, 0 };
			c[2][1] = { 0, 0 };
			c[2][2] = { 1, 0 };
			c[2][3] = { 2, 0 };
			//
			c[3][0] = { 1, 1 };
			c[3][1] = { 1, 0 };
			c[3][2] = { 1, -1 };
			c[3][3] = { 1, -2 };
			//
			c[4][0] = { -1, 0 };
			c[4][1] = { 0, 0 };
			c[4][2] = { 1, 0 };
			c[4][3] = { 2, 0 };
		}
		{
			auto& c = BlockSet[BlockType::OBlock];
			//
			c[0][0] = { 0, 1 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 1, 0 };
			c[0][3] = { 1, 1 };
			//
			c[1][0] = { 0, 1 };
			c[1][1] = { 0, 0 };
			c[1][2] = { 1, 0 };
			c[1][3] = { 1, 1 };
			//
			c[2][0] = { 0, 1 };
			c[2][1] = { 0, 0 };
			c[2][2] = { 1, 0 };
			c[2][3] = { 1, 1 };
			//
			c[3][0] = { 0, 1 };
			c[3][1] = { 0, 0 };
			c[3][2] = { 1, 0 };
			c[3][3] = { 1, 1 };
			//
			c[4][0] = { 0, 1 };
			c[4][1] = { 0, 0 };
			c[4][2] = { 1, 0 };
			c[4][3] = { 1,1 };
		}
		{
			auto& c = BlockSet[BlockType::SBlock];
			//
			c[0][0] = { 1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 0, -1 };
			c[0][3] = { -1, -1 };
			//
			c[1][0] = { -1, 1 };
			c[1][1] = { 0, 0 };
			c[1][2] = { -1, 0 };
			c[1][3] = { 0, -1 };
			//
			c[2][0] = { 1, 0 };
			c[2][1] = { 0, 0 };
			c[2][2] = { 0, -1 };
			c[2][3] = { -1, -1 };
			//
			c[3][0] = { -1, 1 };
			c[3][1] = { 0, 0 };
			c[3][2] = { -1, 0 };
			c[3][3] = { 0, -1 };
			//
			c[4][0] = { 1, 1 };
			c[4][1] = { 0, 1 };
			c[4][2] = { 0, 0 };
			c[4][3] = { -1, 0 };
		}
		{
			auto& c = BlockSet[BlockType::ZBlock];
			//
			c[0][0] = { -1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 1, -1 };
			c[0][3] = { 0, -1 };
			//
			c[1][0] = { 1, 0 };
			c[1][1] = { 0, 0 };
			c[1][2] = { 0, -1 };
			c[1][3] = { 1, 1 };
			//
			c[2][0] = { -1, 0 };
			c[2][1] = { 0, 0 };
			c[2][2] = { 1, -1 };
			c[2][3] = { 0, -1 };
			//
			c[3][0] = { 1, 0 };
			c[3][1] = { 0, 0 };
			c[3][2] = { 0, -1 };
			c[3][3] = { 1, 1 };
			//
			c[4][0] = { -1, 1 };
			c[4][1] = { 0, 1 };
			c[4][2] = { 1, 0 };
			c[4][3] = { 0, 0 };
		}
		{
			auto& c = BlockSet[BlockType::TBlock];
			//
			c[0][0] = { -1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 0, -1 };
			c[0][3] = { 1, 0 };
			//
			c[1][0] = { 0, -1 };
			c[1][1] = { 0, 0 };
			c[1][2] = { -1, 0 };
			c[1][3] = { 0, 1 };
			//
			c[2][0] = { 1, -1 };
			c[2][1] = { 0, 0 };
			c[2][2] = { 0, -1 };
			c[2][3] = { -1, -1 };
			//
			c[3][0] = { 0, -1 };
			c[3][1] = { 0, 0 };
			c[3][2] = { 1, 0 };
			c[3][3] = { 0, 1 };
			//
			c[4][0] = { -1, 1 };
			c[4][1] = { 0, 1 };
			c[4][2] = { 0, 0 };
			c[4][3] = { 1, 1 };
		}
		{
			auto& c = BlockSet[BlockType::LBlock];
			//
			c[0][0] = { 1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { -1, 0 };
			c[0][3] = { -1, -1 };
			//
			c[1][0] = { 0, 1 };
			c[1][1] = { 0, 0 };
			c[1][2] = { 0, -1 };
			c[1][3] = { -1, 1 };
			//
			c[2][0] = { -1, -1 };
			c[2][1] = { 0, -1 };
			c[2][2] = { 1, -1 };
			c[2][3] = { 1, 0 };
			//
			c[3][0] = { 0, -1 };
			c[3][1] = { 0, 0 };
			c[3][2] = { 0, 1 };
			c[3][3] = { 1, -1 };
			//
			c[4][0] = { 1, 1 };
			c[4][1] = { 0, 1 };
			c[4][2] = { -1, 1 };
			c[4][3] = { -1, 0 };
		}
		{
			auto& c = BlockSet[BlockType::JBlock];
			//
			c[0][0] = { -1, 0 };
			c[0][1] = { 0, 0 };
			c[0][2] = { 1, 0 };
			c[0][3] = { 1, -1 };
			//
			c[1][0] = { 0, -1 };
			c[1][1] = { 0, 0 };
			c[1][2] = { 0, 1 };
			c[1][3] = { -1, -1 };
			//
			c[2][0] = { 1, -1 };
			c[2][1] = { 0, -1 };
			c[2][2] = { -1, -1 };
			c[2][3] = { -1, 0 };
			//
			c[3][0] = { 0, 1 };
			c[3][1] = { 0, 0 };
			c[3][2] = { 0, -1 };
			c[3][3] = { 1, 1 };
			//
			c[4][0] = { -1, 1 };
			c[4][1] = { 0, 1 };
			c[4][2] = { 1, 1 };
			c[4][3] = { 1, 0 };
		}
	}
	return BlockSet[block][dir % 5];
}

const FallBlock::BlockData& FallBlock::GetBlockData() const
{
	return GetBlockData(m_block, m_blockDir);
}

const FallBlock::BlockData& FallBlock::GetBlockData(int dir) const
{
	return GetBlockData(m_block, dir);

}

BlockType FallBlock::Block() const
{
	return m_block;
}

void FallBlock::Block(BlockType b)
{
	m_block = b;
}

int FallBlock::Dir() const
{
	return m_blockDir;
}

void FallBlock::Dir(int dir)
{
	m_blockDir = dir % 4;
}


//消除每行的分数计算
static int GetCleanLinesScore(int level, int linesCount)
{
	const int lC[] = { 1, 3, 5, 8 };
	return 40 * level*lC[linesCount - 1];
}

PlayTetris::PlayTetris()
{

}

PlayTetris::~PlayTetris()
{

}

bool PlayTetris::IsFallOnEnd() const
{
	int y = m_fallBlockPos.second - 1;
	if (y < 0)
		return true;
	int x = m_fallBlockPos.first;

	return !IsFallBlockEnablePos(x, y, m_fallBlock.Dir());
}

void PlayTetris::FallEnd()
{
	int y = m_fallBlockPos.second;
	int x = m_fallBlockPos.first;
	auto& fallD = m_fallBlock.GetBlockData();
	for (int i = 0; i < 4; ++i)
	{
		auto& d = fallD[i];
		m_data.SetBlock(x + d.first, y + d.second, m_fallBlock.Block());
	}
	if (m_observer)
	{
		std::array<std::pair<int, int>, 4> pos;
		for (int i = 0; i < 4; ++i)
		{
			pos[i].first = x + fallD[i].first;
			pos[i].second = y + fallD[i].second;
		}
		m_observer->OnBlockFallEnd(m_fallBlock.Block(), pos);
	}
}

void PlayTetris::TurnFallBlock()
{
	if (m_fallBlockStatus == BlockPlayStatus::Op)
	{
		//判断是否能旋转
		int newDir = m_fallBlock.Dir() + 1;

		bool rev = IsFallBlockEnablePos(m_fallBlockPos.first, m_fallBlockPos.second, newDir);
		if (rev)
		{
			m_fallBlock.Dir(newDir);
			//能旋转
			if (m_observer)
				m_observer->OnBlockTurn(m_fallBlock.Dir());
		}

		//wall kicks 当不能旋转时，上，右，左，分别进行尝试位移，看是否能旋转成功
		int o = 1;
		//上
		for (o = 1; o < 3 && !rev; ++o)
		{
			rev = IsFallBlockEnablePos(m_fallBlockPos.first, m_fallBlockPos.second + o, newDir);
			if (rev)
			{
				m_fallBlock.Dir(newDir);
				//能旋转
				if (m_observer)
					m_observer->OnBlockTurn(m_fallBlock.Dir());
				
				m_fallBlockPos.second += o;
				if (m_observer)
					m_observer->OnBlockMove(m_fallBlockPos.first, m_fallBlockPos.second);
				break;
			}
		}
		//右
		for (o = 1; o < 3 && !rev; ++o)
		{
			rev = IsFallBlockEnablePos(m_fallBlockPos.first+o, m_fallBlockPos.second, newDir);
			if (rev)
			{
				m_fallBlock.Dir(newDir);
				//能旋转
				if (m_observer)
					m_observer->OnBlockTurn(m_fallBlock.Dir());

				m_fallBlockPos.first += o;
				if (m_observer)
					m_observer->OnBlockMove(m_fallBlockPos.first, m_fallBlockPos.second);
				break;
			}
		}
		//左
		for (o = 1; o < 3 && !rev; ++o)
		{
			rev = IsFallBlockEnablePos(m_fallBlockPos.first-o, m_fallBlockPos.second, newDir);
			if (rev)
			{
				m_fallBlock.Dir(newDir);
				//能旋转
				if (m_observer)
					m_observer->OnBlockTurn(m_fallBlock.Dir());

				m_fallBlockPos.first -= o;
				if (m_observer)
					m_observer->OnBlockMove(m_fallBlockPos.first, m_fallBlockPos.second);
				break;
			}
		}
	}

}

void PlayTetris::FallFrame()
{
	switch (m_fallBlockStatus)
	{
	case BlockPlayStatus::BeginOp:
	{
		//产生新的方块，开始下落
		NewFallBlock();
		break;
	}
	case BlockPlayStatus::Op:
	{
		//下落，可操作
		if (IsFallOnEnd())
		{
			m_fallBlockStatus = BlockPlayStatus::BlockFallEnd;
			break;
		}
		else
		{
			BlockFall(1);
		}
	}

	}
}

void PlayTetris::NewFallBlock()
{
	m_fallBlockPos.first = TetrisData::Width / 2;
	m_fallBlockPos.second = TetrisData::Height - 1;

	if (m_newFallBlockInfo.empty())
	{
		auto bt = m_newFallBlockRand.GetNewBlock();
		m_fallBlock.Block(bt);
		m_fallBlock.Dir(0);
	}
	else
	{
		auto f = m_newFallBlockInfo.front();
		m_fallBlock.Block((BlockType)(f.first));
		m_fallBlock.Dir(0);
		m_newFallBlockInfo.erase(m_newFallBlockInfo.begin());
	}

	GetNextFallBlock(0);

	if (!IsFallBlockEnablePos(m_fallBlockPos.first, m_fallBlockPos.second, m_fallBlock.Dir()))
	{
		m_fallBlockStatus = BlockPlayStatus::PlayEnd;

		for (int y = 1; y < 5; ++y)
		{
			if (IsFallBlockEnablePos(m_fallBlockPos.first, m_fallBlockPos.second + y, m_fallBlock.Dir()))
			{
				m_fallBlockPos.second += y;
				break;
			}
		}
	}
	else
	{
		m_fallBlockStatus = BlockPlayStatus::Op;

	}
	++m_fallBlockCount;
	if (m_observer)
		m_observer->OnNewBlock(m_fallBlock.Block(), m_fallBlock.Dir(), m_fallBlockPos.first, m_fallBlockPos.second);

	if (m_fallBlockStatus == BlockPlayStatus::PlayEnd)
		if (m_observer)
			m_observer->OnNewBlockFailed();

}

void PlayTetris::BeginCleanFullLines()
{
}

void PlayTetris::Frame(float delta)
{
	//下落控制
	m_fallRunTime += delta;
	if (m_fallBlockStatus == BlockPlayStatus::BlockFallEnd)
	{
		if (m_fallRunTime >= m_fallEndDetal)
		{
			ProcBlockFallEnd();
			m_fallRunTime -= m_fallEndDetal;
		}
	}
	else
	{
		if (m_fallRunTime >= m_fallDetal)
		{
			FallFrame();
			m_fallRunTime -= m_fallDetal;
		}
	}
}

void PlayTetris::SetObserver(PlayTetrisObserver* observer)
{
	m_observer = observer;
}

void PlayTetris::BlockFall(int step)
{
	int x = m_fallBlockPos.first;
	int y = m_fallBlockPos.second;
	int dir = m_fallBlock.Dir();
	int i = 1;
	for (; i <= step; ++i)
	{
		if (!IsFallBlockEnablePos(x, y - i, dir))
		{
			break;
		}
	}
	--i;
	if (i > 0)
	{
		m_fallBlockPos.second -= i;
		if (m_observer)
			m_observer->OnBlockMove(m_fallBlockPos.first, m_fallBlockPos.second);
	}
}

bool PlayTetris::IsFallBlockEnablePos(int x, int y, int dir) const
{
	auto& fallD = m_fallBlock.GetBlockData(dir);
	for (int i = 0; i < 4; ++i)
	{
		if (!m_data.IsBlockEmpty(x + fallD[i].first, y + fallD[i].second))
		{
			return false;
		}
	}
	return true;
}

void PlayTetris::MoveBlock(int dir)
{
	if (m_fallBlockStatus != BlockPlayStatus::Op)
		return;

	int i = 0;
	switch (dir)
	{
	case -1:
		i = -1;
		break;
	case 1:
		i = 1;
		break;
	}
	if (i != 0)
	{
		if (IsFallBlockEnablePos(m_fallBlockPos.first + i, m_fallBlockPos.second, m_fallBlock.Dir()))
		{
			//能水平位移
			m_fallBlockPos.first += i;
			if (m_observer)
				m_observer->OnBlockMove(m_fallBlockPos.first, m_fallBlockPos.second);
		}
	}
}

void PlayTetris::FallBlockAfterDel()
{
	m_data.FallBlockAfterDel();
	auto& f = m_data.BlockFallInfo();
	if (m_observer)
		m_observer->OnBlocksFallFill(f);
}

void PlayTetris::ControlBlockFall(int step)
{
	if (m_fallBlockStatus == BlockPlayStatus::Op)
	{
		BlockFall(step);
		m_fallRunTime = 0.0f;
	}
}

bool PlayTetris::IsEnd() const
{
	return m_fallBlockStatus == BlockPlayStatus::PlayEnd;
}

void PlayTetris::CleanAllBlock()
{
	m_data.Clean();
}

void PlayTetris::ControlNewFallBlock()
{
	m_fallBlockStatus = BlockPlayStatus::BeginOp;
}

std::pair<int, int> PlayTetris::GetNextFallBlock(int index) const
{
	while (m_newFallBlockInfo.size() < 3)
	{
		m_newFallBlockInfo.push_back(std::make_pair(
			((int)m_newFallBlockRand.GetNewBlock())
			, 0));
	}
	return m_newFallBlockInfo[index];
}

void PlayTetris::NewRound()
{
	m_newFallBlockInfo.clear();
	m_newFallBlockRand.SetSeed(NewFallBlockSeed[0]);// rand() % NewFallBlockSeed.size()]);
	CleanAllBlock();
	m_fallSpeedLevel = 1;
	m_fallSpeedLevelRang = { 1, 15 };
	m_fallDetal = 0.5f;
	m_fallBlockCount = 0;
	m_gameScore = 0;
	m_cleanLinesCount = 0;
	m_gameLevel = 1;
}

void PlayTetris::SetFallLevel(int level)
{
	level = std::max(m_fallSpeedLevelRang.first, level);
	level = std::min(m_fallSpeedLevelRang.second, level);
	m_fallSpeedLevel = level;

	m_fallDetal = std::max(0.09f, 0.5f - (level - 1)* 0.0266f);

}

void PlayTetris::SetFallLevelRang(int min, int max)
{
	m_fallSpeedLevelRang.first = min;
	m_fallSpeedLevelRang.second = max;

}

void PlayTetris::ProcBlockFallEnd()
{
	//
	FallEnd();

	//开始进计算，销毁

	//判断是否需要消除
	auto fullLines = m_data.GetFillFullLines();
	if (fullLines[0] != -1)
	{
		//需要消除
		m_fallBlockStatus = BlockPlayStatus::BlockClean;
		//消除行
		int lineCount = 0;
		for (auto line : fullLines)
		{
			if (line >= 0)
			{
				m_data.DelLine(line);
				++lineCount;
			}
		}
		m_cleanLinesCount += lineCount;
		m_gameLevel = m_cleanLinesCount / 10 + 1;
		m_gameScore += GetCleanLinesScore(m_gameLevel, lineCount);
		SetFallLevel(m_gameLevel);
		if (m_observer)
			m_observer->OnlineClean(fullLines);

	}
	else
	{
		m_fallBlockStatus = BlockPlayStatus::BeginOp;
	}
}

std::pair<int, int> PlayTetris::GetPreviewFallBlockPos() const
{
	int x = m_fallBlockPos.first;
	int y = m_fallBlockPos.second;
	int step = 21;
	int dir = m_fallBlock.Dir();
	int i = 1;
	for (; i <= 21; ++i)
	{
		if (!IsFallBlockEnablePos(x, y - i, dir))
		{
			break;
		}
	}
	--i;
	if (i > 0)
	{
		y -= i;
	}

	return{x,y};
}


void MyRand::SRand(uint32_t s)
{
	m_r = s;
}

uint32_t MyRand::Rand() const
{
	return(((m_r = m_r * 214013L
		+ 2531011L) >> 16) & 0x7fff);
}

BlockType BlockGenerator::GetNewBlock()
{
	if (m_queue.size() < 7)
	{
		//生成数据
		std::array<BlockType, 8> bb;
		for (int i = 0; i < bb.size(); ++i)
		{
			bb[i] = (BlockType)((int)BlockType::IBlock + m_blockIndex);
			m_blockIndex = (m_blockIndex + 1) % 7;
		}
		for (int i = 0; i < bb.size(); ++i)
		{
			int j = m_rand.Rand() % bb.size();
			if (j  != i)
			{
				auto b = bb[i];
				bb[i] = bb[j];
				bb[j] = b;
			}
		}
		for (auto b : bb)
		{
			m_queue.push( b);
		}

	}
	auto b = m_queue.front();
	m_queue.pop();
	return b;
}

void BlockGenerator::SetSeed(uint32_t seed)
{
	m_rand.SRand(seed);
	m_blockIndex = 0;
}
