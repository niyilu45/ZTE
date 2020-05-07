#include "main.h"

int Result = 0;
PointCacheStruct* ResultPointCache;

int main(void){
	// Clock Start.
	clock_t start = clock();

	// 1) Load CSV file.
	EdgeStruct* Edge;
	Edge = LoadCSV();

	// 2) Find the Circ
	ResultPointCache = InitPointCache(MaxCircLen);
	int CircLen = 6;
	FindCirc(CircLen, Edge);
	printf("Circ %d = %d \n", CircLen, Result);


	// Free
	ReleasePointCache(&ResultPointCache);

	// Clock End.
	clock_t end = (clock() - start)/CLOCKS_PER_SEC;
	printf("\r\nTime is %ld s", end);
	return 0;
}

int FindCirc(int CircLen, EdgeStruct* Edge){
	int ColCnt;
	int i,j;

	// 1) Set the most left side.
	Point Head;
	Point Tail;
	int NewPointNum;
	NewPointNum = CircLen / 2 - 2; // Get rid of Head and Tail.
	for(ColCnt=0;ColCnt<Edge->N;ColCnt++){
		for(i=0;i<(Edge->ColEdge+ColCnt)->degree-1;i++){
			Tail.x = ColCnt;
			Tail.y = *((Edge->ColEdge+ColCnt)->Idx+i);
			PushPointCache(Tail, ResultPointCache);
			Head.x = ColCnt;
			for(j=i+1;j<(Edge->ColEdge+ColCnt)->degree;j++){
				Head.y = *((Edge->ColEdge+ColCnt)->Idx+j);
				PushPointCache(Head, ResultPointCache);
				FindNewPoint(Head, Tail, NewPointNum, Edge);
				PopPointCache(ResultPointCache);
			}
			PopPointCache(ResultPointCache);
		}
	}
	return 0;
}

int FindNewPoint(Point Head, Point Tail, int NewPointNum, EdgeStruct* Edge){
	int i,j;
	Point NewPoint;
	Point TempPoint;
	Point Last1Point;
	Point Last2Point;
	if(NewPointNum > 0){
		for(i=0;i<(Edge->RowEdge+Head.y)->degree;i++){
			TempPoint.x = *((Edge->RowEdge+Head.y)->Idx+i);
			TempPoint.y = Head.y;
			if(TempPoint.x <= Tail.x){
				continue;
			}
			if(FindValXInPointCache(TempPoint.x, 2, ResultPointCache) >= 0){
				continue;
			}
			PushPointCache(TempPoint, ResultPointCache);
			for(j=0;j<(Edge->ColEdge+TempPoint.x)->degree;j++){
				NewPoint.x = TempPoint.x;
				NewPoint.y = *((Edge->ColEdge+TempPoint.x)->Idx+j);
				if(FindValYInPointCache(NewPoint.y, 0, ResultPointCache) >= 0){
					continue;
				}
				PushPointCache(NewPoint, ResultPointCache);
				FindNewPoint(NewPoint, Tail, NewPointNum-1, Edge);
				PopPointCache(ResultPointCache);
			}
			PopPointCache(ResultPointCache);
		}
	}
	else{
		for(i=0;i<(Edge->RowEdge+Head.y)->degree;i++){
			Last2Point.x = *((Edge->RowEdge+Head.y)->Idx+i);
			Last2Point.y = Head.y;
			if(Last2Point.x <= Tail.x){
				continue;
			}
			if(FindValXInPointCache(Last2Point.x, 2, ResultPointCache) >= 0){
				continue;
			}
			PushPointCache(Last2Point, ResultPointCache);
			for(j=0;j<(Edge->RowEdge+Tail.y)->degree;j++){
				Last1Point.x = *((Edge->RowEdge+Tail.y)->Idx+j);
				Last1Point.y = Tail.y;
				if(Last1Point.x == Last2Point.x){
					PushPointCache(Last1Point, ResultPointCache);
					if(Last1Point.x > Tail.x){
						Result++;
						FileWritePointCache(ResultPointCache);
					}
					PopPointCache(ResultPointCache);
				}
			}
			PopPointCache(ResultPointCache);
		}

	}
	return 0;
}


PointCacheStruct* InitPointCache(int CacheLen){
	PointCacheStruct* PointCache;
	PointCache = (PointCacheStruct*)malloc(sizeof(PointCacheStruct));
	PointCache->CacheCnt = 0;
	PointCache->P = (Point*)malloc(sizeof(Point) * CacheLen);

	return PointCache;
}
int ReleasePointCache(PointCacheStruct** PointCachePointer){
	PointCacheStruct* PointCache;
	PointCache = *PointCachePointer;

	free(PointCache->P);
	PointCache->P = NULL;
	free(*PointCachePointer);
	*PointCachePointer = NULL;

	return 0;
}
int PushPointCache(Point P, PointCacheStruct* PointCache){
	PointCache->P[PointCache->CacheCnt++] = P;
	return 0;
}
Point PopPointCache(PointCacheStruct* PointCache){
	Point P;
	if(PointCache->CacheCnt <= 0){
		P.x = -1;
		P.y = -1;
		return P;
	}
	P = PointCache->P[--PointCache->CacheCnt];
	return P;
}

int FindValXInPointCache(int ValX, int StartIdx, PointCacheStruct* PointCache){
	int i = StartIdx;
	for(;i<PointCache->CacheCnt;i++){
		if(ValX == PointCache->P[i].x){
			return i;
		}
	}

	return -1;
}
int FindValYInPointCache(int ValY, int StartIdx, PointCacheStruct* PointCache){
	int i = StartIdx;
	for(;i<PointCache->CacheCnt;i++){
		if(ValY == PointCache->P[i].y){
			return i;
		}
	}

	return -1;
}

int FileWritePointCache(PointCacheStruct* PointCache){
	int i;
	FILE* Fid = fopen(FileDetail, "a+");
	if(Fid == NULL){
		return -1;
	}

	for(i=0;i<PointCache->CacheCnt;i++){
		fprintf(Fid,"(%d,%d)",PointCache->P[i].x, PointCache->P[i].y);
	}
	fprintf(Fid,"\n");
	fclose(Fid);
	return 0;
}


EdgeStruct* LoadCSV(void){
	int i,j;
	EdgeStruct* Edge;


	// 1) Read CSV flie to Matrix.
	FILE* Fid = NULL;
	if((Fid = fopen(CSVName,"r")) == NULL){
		printf("Can not open MyMatrix.csv!\n");
		return NULL;
	}

	Edge = (EdgeStruct*)malloc(sizeof(EdgeStruct));

    char buffer[MaxMatrixN];//20450这个数组大小也要根据自己文件的列数进行相应修改。
	char *line;
	char *record;
	int** Matrix;
	Matrix = (int**)malloc(sizeof(int*) * MaxMatrixM);
	for(i=0;i<MaxMatrixM;i++){
		Matrix[i] = (int*)malloc(sizeof(int) * MaxMatrixN);
	}
	int val;
	int M;
	int N;

	M = 0;
	while ((line = fgets(buffer, sizeof(buffer), Fid))!=NULL){//当没有读取到文件末尾时循环继续
		record = strtok(line, ",");
		N = 0;
		while(record != NULL){
			val = atoi(record);
			printf("%d",val);
			Matrix[M][N] = val;
			record = strtok(NULL, ",");
			N++;
		}
		printf("\n");
		M++;
	}
	fclose(Fid);
	Fid = NULL;
	printf("\r\n M = %d, N = %d\n",M,N);
	Edge->M = M;
	Edge->N = N;

	// 2) Change to store type of Edge.
	Edge->RowEdge = (RowEdgeStruct*)malloc(sizeof(RowEdgeStruct) * M);
	Edge->ColEdge = (ColEdgeStruct*)malloc(sizeof(ColEdgeStruct) * N);

	int Cnt;
	int* Temp;
	Temp = (int*)malloc(sizeof(int) * N);
	for(i=0;i<M;i++){
		Cnt = 0;
		for(j=0;j<N;j++){
			if(Matrix[i][j] == 1){
				Temp[Cnt] = j;
				Cnt++;
			}
		}
		(Edge->RowEdge+i)->degree = Cnt;
		(Edge->RowEdge+i)->Idx = (int*)malloc(sizeof(int) * Cnt);
		memcpy((Edge->RowEdge+i)->Idx, Temp, sizeof(int) * Cnt);
	}
	free(Temp);
	Temp = NULL;

	Temp = (int*)malloc(sizeof(int) * M);
	for(j=0;j<N;j++){
		Cnt = 0;
		for(i=0;i<M;i++){
			if(Matrix[i][j] == 1){
				Temp[Cnt] = i;
				Cnt++;
			}
		}
		(Edge->ColEdge+j)->degree = Cnt;
		(Edge->ColEdge+j)->Idx = (int*)malloc(sizeof(int) * Cnt);
		memcpy((Edge->ColEdge+j)->Idx, Temp, sizeof(int) * Cnt);
	}
	free(Temp);
	Temp = NULL;


	for(i=0;i<MaxMatrixM;i++){
		free(Matrix[i]);
		Matrix[i] = NULL;
	}
	free(Matrix);
	Matrix = NULL;

	return Edge;
}
