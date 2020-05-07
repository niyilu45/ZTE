#ifndef __MAIN_H
#define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CSVName			"Example.csv"
#define FileResult		"Result.txt"
#define FileDetail		"ResultDetail.txt"
#define MaxMatrixM		3000
#define MaxMatrixN		3000
#define MaxCircLen		14


typedef struct ColEdgeStruct{
	int degree;
	int *Idx;
}ColEdgeStruct;
typedef struct RowEdgeStruct{
	int degree;
	int *Idx;
}RowEdgeStruct;
typedef struct structEdgeStruct{
	int M;
	int N;
	ColEdgeStruct* ColEdge;
	RowEdgeStruct* RowEdge;
}EdgeStruct;

typedef struct Point{
	int x;
	int y;
}Point;

typedef struct PointCacheStruct{
	int CacheCnt;
	Point* P;
}PointCacheStruct;

typedef struct NameStruct{
	struct NameStruct* next;
	int* xIdx;
	int* yIdx;
}NameStruct;
typedef struct ResultNameCacheStruct{
	struct NameStruct* Name;
}ResultNameCacheStruct;

EdgeStruct* LoadCSV(void);
int FindCirc(int CircLen, EdgeStruct* Edge);
int FindNewPoint(Point Head, Point Tail, int NewPointNum, EdgeStruct* Edge);
PointCacheStruct* InitPointCache(int CacheLen);
int ReleasePointCache(PointCacheStruct** PointCachePointer);
int PushPointCache(Point P, PointCacheStruct* PointCache);
Point PopPointCache(PointCacheStruct* PointCache);
int FindValXInPointCache(int ValX, int StartIdx, PointCacheStruct* PointCache);
int FindValYInPointCache(int ValY, int StartIdx, PointCacheStruct* PointCache);

int PushNameCache(PointCacheStruct* ResultPointCache);
int CheckDuplicate(int* xIdxA, int* xIdxB, int* yIdxA, int* yIdxB, int Len);

int FileWritePointCache(PointCacheStruct* PointCache);
#endif
