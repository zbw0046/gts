#pragma once
#include "Cell.h"
#include "ConstDefine.h"
#include "MBB.h"
#include "QueryResult.h"
#include "Trajectory.h"
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<vector>
#include "cudaKernel.h"
#include <map>
#include"FVTable.h"

using namespace std;
extern Trajectory* tradb;

typedef struct QuadtreeNode {
	int NodeID;
	uint8_t level;
	int numPoints;
	bool isLeaf;
	QuadtreeNode *parent = NULL, *UL = NULL, *UR = NULL, *DL = NULL, *DR = NULL;
	MBB mbb;
}QuadtreeNode;

//记录node及其在gpu内地址的对应关系，用于防止重复复制
typedef struct NodeAddrTable {
	int NodeStartCellID;
	void* ptr;
}NodeAddrTable;

class Grid
{
public:
	Grid();
	~Grid();
	Grid(const MBB& mbb, float val_cell_size);
	int addTrajectoryIntoCell(Trajectory &t);
	int WhichCellPointIn(SamplePoint p);
	int addDatasetToGrid(Trajectory* db,int traNum);
	int writeCellsToFile(int* cellNo, int cellNum,string file);
	//rangeQuery函数，输入Bounding box，输出轨迹编号和对应顺序下的采样点
	int rangeQuery(MBB & bound, CPURangeQueryResult * ResultTable, int* resultSetSize);
	int rangeQueryGPU(MBB & bound, CPURangeQueryResult * ResultTable, int* resultSetSize);
	int SimilarityQuery(Trajectory &qTra, Trajectory **candTra, int candSize, float *EDRdistance);
	int buildQuadTree(int level, int id, QuadtreeNode* pNode, QuadtreeNode *parent);
	//rangeQuery批量
	int rangeQueryBatch(MBB *bounds, int rangeNum, CPURangeQueryResult *ResultTable, int *resultSetSize);
	int findMatchNodeInQuadTree(QuadtreeNode *node, MBB& bound, vector<QuadtreeNode*> *cells);
	int rangeQueryBatchGPU(MBB *bounds, int rangeNum, CPURangeQueryResult *ResultTable, int *resultSetSize);
	int findMatchNodeInQuadTreeGPU(QuadtreeNode *node, MBB& bound, vector<QuadtreeNode*> *cells, cudaStream_t stream, int queryID);
	//SimilarityQuery
	int SimilarityQueryBatch(Trajectory *qTra, int queryTrajNum, int *EDRdistance, int kValue);
	int SimilarityExecuter(SPoint* queryTra, SPoint** candidateTra, int queryLength, int* candidateLength, int candSize, int *resultArray);
	int SimilarityQueryBatchOnGPU(Trajectory * qTra, int queryTrajNum, int * topKSimilarityTraj, int kValue);


	//Grid索引包含的坐标范围
	MBB range;
	float cell_size; //length of a cell
	int cellNum_axis; // 每行/列有几个cell
	int cellnum; //upper(area(grid)/area(cell))，保证能放下所有cell
	Cell* cellPtr; //存储cell的入口
	ofstream fout;//文件输出接口
	int totalPointNum; //grid内点个数
	int trajNum;
	QuadtreeNode *root;

	vector<cellBasedTraj> cellBasedTrajectory; //cellbasedtrajectory，二元组：（cell编号数组地址，数组长度）

	SPoint* allPoints;//存储所有点的数组
	Point* allPointsPtrGPU;
	DPoint *allPointsDeltaEncoding;//Delta Encoding后的点
	RangeQueryStateTable* stateTableRange;
	map<int, void*> nodeAddrTable;
	int stateTableLength;
	int nodeAddrTableLength;
	FVTable freqVectors;
	
	



};

