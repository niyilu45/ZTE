#include "main.h"

int Result = 0;
PointCacheStruct* ResultPointCache;
SearchTree* TreeClust;
int TreeClustNum;


int main(void){
	// Clock Start.


	// 1) Load CSV file.
	EdgeStruct* Edge;
	Edge = LoadCSV();

	// 2) Find the Circ
	FILE* Fid = NULL;
	// clear and create result.txt
	#ifdef __LINUX
	Fid = fopen(FileResult,"w");
	#else
	fopen_s(&Fid, FileResult,"w");
	#endif
	if(Fid == NULL){
		printf("Can not create result.txt!\n");
		return 0;
	}
	fclose(Fid);
	ResultPointCache = InitPointCache(MaxCircLen);
	int CircLen = 4;
	for(CircLen = 4;CircLen <= MaxCircLen; CircLen+=2){
		#ifdef __LINUX
		Fid = fopen(FileResult,"a+");
		#else
		fopen_s(&Fid, FileResult,"a+");
		#endif
clock_t start = clock();
		FindCirc(CircLen, Edge);
		printf("\r\nCirc %d = %d.", CircLen, Result);
clock_t end = (clock() - start)/CLOCKS_PER_SEC;
printf("\r\nTime is %ld s\n", end);
		fprintf(Fid,"The offerings with %2d names on the wooden tray have a maximum of %d\n",CircLen, Result);
		fclose(Fid);
	}

	// Free
	ReleasePointCache(&ResultPointCache);
	ReleaseEdge(&Edge);

	// Clock End.

	printf("\n");
	return 0;
}

int FindCirc(int CircLen, EdgeStruct* Edge){
	int ColCnt;
	int i,j;

	// 1) Init the ResultName cache
	Result = 0;
	ResultNameCacheStruct** ResultNameCache;
	int ResultNameCacheM = CircLen / 2 * Edge->M;
	int ResultNameCacheN = CircLen / 2 * Edge->N;
	ResultNameCache = (ResultNameCacheStruct**)malloc(sizeof(ResultNameCacheStruct) * ResultNameCacheM);
	for(int i=0;i<ResultNameCacheM;i++){
		ResultNameCache[i] = (ResultNameCacheStruct*)calloc(ResultNameCacheN, sizeof(ResultNameCacheStruct));
	}
	// 2) Set the most left side.
	Point Head;
	Point Tail;
	int NewPointNum;
	NewPointNum = CircLen / 2 - 2; // Get rid of Head and Tail.
	int MinColSumGap = 0;
	int CurMinColSum = 0;
	for(i=0;i<CircLen/2;i++){
		CurMinColSum += i;
	}
	MinColSumGap = CircLen/2;
	for(ColCnt=0;ColCnt<Edge->N-CircLen/2+1;ColCnt++){
		for(i=0;i<(Edge->ColEdge+ColCnt)->degree-1;i++){
			Tail.x = ColCnt;
			Tail.y = *((Edge->ColEdge+ColCnt)->Idx+i);
			TreeClustNum = (Edge->RowEdge+Tail.y)->degree;
			TreeClust = (SearchTree*)calloc(TreeClustNum, sizeof(SearchTree));
			for(int TreeCnt=0;TreeCnt<TreeClustNum;TreeCnt++){
				TreeClust[TreeCnt] = Insert(0, TreeClust[TreeCnt]);
				int TempX;
				int TempY;
				int Temp;
				TempX = *((Edge->RowEdge+Tail.y)->Idx+TreeCnt);
				for(j=0;j<(Edge->ColEdge+TempX)->degree;j++){
					Temp = *((Edge->ColEdge+TempX)->Idx+j);
					TreeClust[TreeCnt] = Insert(Temp-Tail.y, TreeClust[TreeCnt]);
				}
			}


			PushPointCache(Tail, ResultPointCache);
			Head.x = ColCnt;
			for(j=i+1;j<(Edge->ColEdge+ColCnt)->degree;j++){
				Head.y = *((Edge->ColEdge+ColCnt)->Idx+j);
				PushPointCache(Head, ResultPointCache);
				FindNewPoint(Head, Tail, NewPointNum, Edge, ResultNameCache);
				PopPointCache(ResultPointCache);
			}
			PopPointCache(ResultPointCache);

			for(int TreeCnt=0;TreeCnt<TreeClustNum;TreeCnt++){
				TreeClust[TreeCnt] = MakeEmpty(TreeClust[TreeCnt]);
			}
			free(TreeClust);
			TreeClust = NULL;
		}

		// To save memory, release part of ResultNameCache.
		for(j=CurMinColSum;j<CurMinColSum+MinColSumGap;j++){
			for(i=0;i<ResultNameCacheM;i++){
				ReleaseNameCache(ResultNameCache[i][j].Name);
			}
		}
		CurMinColSum += MinColSumGap;
	}

	// 3) Release the ResultName Cache
	NameStruct* TempName;
	NameStruct* CurName;
	/*
	for(i=0;i<ResultNameCacheM;i++){
		for(j=0;j<ResultNameCacheN;j++){
			ReleaseNameCache(ResultNameCache[i][j].Name);
		}
	}
	*/
	for(j=CurMinColSum;j<ResultNameCacheN;j++){
		for(i=0;i<ResultNameCacheM;i++){
			ReleaseNameCache(ResultNameCache[i][j].Name);
		}
	}

	for(i=0;i<ResultNameCacheM;i++){
		free(ResultNameCache[i]);
		ResultNameCache[i] = NULL;
	}
	free(ResultNameCache);
	ResultNameCache = NULL;
	return 0;
}

int ReleaseNameCache(NameStruct* Name){
	NameStruct* CurName;
	NameStruct* TempName;
	CurName = Name;

	while(CurName != NULL){
		TempName = CurName->next;
		free(CurName->xIdx);
		CurName->xIdx = NULL;
		free(CurName->yIdx);
		CurName->yIdx = NULL;
		free(CurName);
		CurName = TempName;
	}

	return 0;
}

int FindNewPoint(Point Head, Point Tail, int NewPointNum, EdgeStruct* Edge, ResultNameCacheStruct** ResultNameCache){
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
				FindNewPoint(NewPoint, Tail, NewPointNum-1, Edge, ResultNameCache);
				PopPointCache(ResultPointCache);
			}
			PopPointCache(ResultPointCache);
		}
	}
	else{
		/*
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
			for(j=1;j<(Edge->RowEdge+Tail.y)->degree;j++){
				Last1Point.x = *((Edge->RowEdge+Tail.y)->Idx+j);
				Last1Point.y = Tail.y;
				if(Last1Point.x == Last2Point.x){
					PushPointCache(Last1Point, ResultPointCache);
					if(PushNameCache(ResultPointCache, ResultNameCache)){
						Result++;
						//FileWritePointCache(ResultPointCache);
						PopPointCache(ResultPointCache);
						break;
					}
					PopPointCache(ResultPointCache);
				}
			}
			PopPointCache(ResultPointCache);
		}
		*/
		int ToFindDist;
		for(i=1;i<(Edge->RowEdge+Tail.y)->degree;i++){
			Last1Point.x = *((Edge->RowEdge+Tail.y)->Idx+i);
			Last1Point.y = Tail.y;
			if(Last1Point.x <= Tail.x){
				continue;
			}
			if(FindValXInPointCache(Last1Point.x, 2, ResultPointCache) >= 0){
				continue;
			}

			ToFindDist = Head.y - Tail.y;
			if(NULL != Find(ToFindDist, TreeClust[i])){
				Last2Point.x = Last1Point.x;
				Last2Point.y = Head.y;
				PushPointCache(Last2Point, ResultPointCache);
				PushPointCache(Last1Point, ResultPointCache);
				if(PushNameCache(ResultPointCache, ResultNameCache)){
					Result++;
					//FileWritePointCache(ResultPointCache);
					PopPointCache(ResultPointCache);
					PopPointCache(ResultPointCache);
					continue;
				}
				PopPointCache(ResultPointCache);
				PopPointCache(ResultPointCache);
			}
		}

		/*
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
			for(j=1;j<(Edge->RowEdge+Tail.y)->degree;j++){
				Last1Point.x = *((Edge->RowEdge+Tail.y)->Idx+j);
				Last1Point.y = Tail.y;
				if(Last1Point.x == Last2Point.x){
					PushPointCache(Last1Point, ResultPointCache);
					if(PushNameCache(ResultPointCache, ResultNameCache)){
						Result++;
						FileWritePointCache(ResultPointCache);
						PopPointCache(ResultPointCache);
						break;
					}
					PopPointCache(ResultPointCache);
				}
			}
			PopPointCache(ResultPointCache);
		}
		*/


	}
	return 0;
}

int PushNameCache(PointCacheStruct* ResultPointCache, ResultNameCacheStruct** ResultNameCache){
	int i;
	int xSum = 0;
	int ySum = 0;
	int* TempxIdx;
	int* TempyIdx;
	TempxIdx = (int*)malloc(sizeof(int) * ResultPointCache->CacheCnt/2);
	TempyIdx = (int*)malloc(sizeof(int) * ResultPointCache->CacheCnt/2);
	for(i=0;i<ResultPointCache->CacheCnt;i+=2){
		xSum += ResultPointCache->P[i].x;
		ySum += ResultPointCache->P[i].y;
		TempxIdx[i/2] = ResultPointCache->P[i].x;
		TempyIdx[i/2] = ResultPointCache->P[i].y;
	}

	NameStruct* NameTemp;
	NameStruct* CurName;
	NameStruct* LastName;
	CurName = ResultNameCache[ySum][xSum].Name;
	if(CurName == NULL){
		NameTemp = (NameStruct*)malloc(sizeof(NameStruct));
		NameTemp->next = NULL;
		NameTemp->xIdx = TempxIdx;
		NameTemp->yIdx = TempyIdx;

		ResultNameCache[ySum][xSum].Name = NameTemp;
		return 1;
	}
	else{
		int Duplicate = 0;
		LastName = CurName;
		while(CurName != NULL){
			Duplicate = CheckDuplicate(TempxIdx, CurName->xIdx, TempyIdx, CurName->yIdx, ResultPointCache->CacheCnt/2);
			if(Duplicate == 1){
				break;
			}
			LastName = CurName;
			CurName = CurName->next;
		}
		if(Duplicate == 0){
			NameTemp = (NameStruct*)malloc(sizeof(NameStruct));
			NameTemp->next = NULL;
			NameTemp->xIdx = TempxIdx;
			NameTemp->yIdx = TempyIdx;

			LastName->next = NameTemp;

			return 1;
		}
	}

	return 0;
}

int CheckDuplicate(int* xIdxA, int* xIdxB, int* yIdxA, int* yIdxB, int Len){
	int i,j;
	int Same;
	for(i=0;i<Len;i++){
		Same = 0;
		for(j=0;j<Len;j++){
			if(xIdxA[i] == xIdxB[j]){
				Same = 1;
			}
		}
		if(Same == 0){
			return 0;
		}
	}

	for(i=0;i<Len;i++){
		Same = 0;
		for(j=0;j<Len;j++){
			if(yIdxA[i] == yIdxB[j]){
				Same = 1;
			}
		}
		if(Same == 0){
			return 0;
		}
	}

	return 1;
}

PointCacheStruct* InitPointCache(int CacheLen){
	PointCacheStruct* PointCache;
	PointCache = (PointCacheStruct*)malloc(sizeof(PointCacheStruct));
	PointCache->CacheCnt = 0;
	PointCache->P = (Point*)malloc(sizeof(Point) * CacheLen);

	return PointCache;
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


EdgeStruct* LoadCSV(void){
	int i,j;
	EdgeStruct* Edge;

	// 1) Read CSV flie to Matrix.
	FILE* Fid = NULL;
	#ifdef __LINUX
	Fid = fopen(CSVName,"r");
	#else
	fopen_s(&Fid, CSVName,"r");
	#endif
	if(Fid == NULL){
		printf("Can not open Example.csv!\n");
		return NULL;
	}

	Edge = (EdgeStruct*)malloc(sizeof(EdgeStruct));

    char buffer[MaxMatrixN];
	char *line;
	char *record;
	char *p;
	int** Matrix;
	Matrix = (int**)malloc(sizeof(int*) * MaxMatrixM);
	for(i=0;i<MaxMatrixM;i++){
		Matrix[i] = (int*)malloc(sizeof(int) * MaxMatrixN);
	}
	int val;
	int M;
	int N;

	M = 0;;
	char* strtmp;
	while ((line = fgets(buffer, sizeof(buffer), Fid)) != NULL) {//当没有读取到文件末尾时循环继续
		strtmp = (char*)calloc(strlen(line) + 1, sizeof(char));
		memcpy(strtmp, line, sizeof(char) * (strlen(line) + 1));
		#ifdef __LINUX
		record = strtok_r(strtmp, ",",&p);
		#else
		record = strtok_s(strtmp, ",",&p);
		#endif
		N = 0;
		while(record != NULL){
			val = atoi(record);
			//printf("%d",val);
			Matrix[M][N] = val;
			#ifdef __LINUX
			record = strtok_r(NULL, ",", &p);
			#else
			record = strtok_s(NULL, ",", &p);
			#endif
			N++;
		}
		//printf("\n");
		M++;
		free(strtmp);
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

int FileWritePointCache(PointCacheStruct* PointCache){
	int i;
	FILE* Fid;
	#ifdef __LINUX
	Fid = fopen(FileDetail, "a+");
	#else
	fopen_s(&Fid, FileDetail, "a+");
	#endif
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

int ReleasePointCache(PointCacheStruct** PointCachePointer){
	PointCacheStruct* PointCache;
	PointCache = *PointCachePointer;

	free(PointCache->P);
	PointCache->P = NULL;
	free(*PointCachePointer);
	*PointCachePointer = NULL;

	return 0;
}
int ReleaseEdge(EdgeStruct** EdgePointer){
	int i,j;
	EdgeStruct* Edge;
	Edge = *EdgePointer;

	for(i=0;i<Edge->M;i++){
		free((Edge->RowEdge+i)->Idx);
		(Edge->RowEdge+i)->Idx = NULL;
	}
	free(Edge->RowEdge);
	Edge->RowEdge = NULL;

	for(i=0;i<Edge->N;i++){
		free((Edge->ColEdge+i)->Idx);
		(Edge->ColEdge+i)->Idx = NULL;
	}
	free(Edge->ColEdge);
	Edge->ColEdge = NULL;
	
	free(*EdgePointer);
	*EdgePointer = NULL;

	return 0;
}

SearchTree MakeEmpty(SearchTree T){
	if(T != NULL){
		MakeEmpty(T->Left);
		MakeEmpty(T->Right);
		free(T);
	}
	return NULL;
}

Position Find(int Dist, SearchTree T){
	if(T == NULL){
		return NULL;
	}
	if(Dist < T->Dist){
		return Find(Dist, T->Left);
	}
	else if(Dist > T->Dist){
		return Find(Dist, T->Right);
	}
	else{
		return T;
	}
}

Position FindMin(SearchTree T){
	if(T == NULL){
		return NULL;
	}
	else if(T->Left == NULL){
		return T;
	}
	else{
		return FindMin(T->Left);
	}
}

Position FindMax(SearchTree T){
	if(T != NULL){
		while(T->Right != NULL){
			T = T->Right;
		}
	}
	return T;
}

SearchTree Insert(int Dist, SearchTree T){
	if(T == NULL){
		T = malloc(sizeof(struct TreeNode));
		if(T == NULL){
			printf("Out of Space");
		}
		else{
			T->Dist = Dist;
			T->Left = NULL;
			T->Right = NULL;
		}
	}
	else{
		if(Dist < T->Dist){
			T->Left = Insert(Dist, T->Left);
		}
		else if(Dist > T->Dist){
			T->Right = Insert(Dist, T->Right);
		}
	}

	return T;
}

