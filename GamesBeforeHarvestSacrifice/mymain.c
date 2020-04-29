#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CSVName "MyMatrix.csv"
int main(void){
	FILE* Fid = NULL;
	if((Fid = fopen("MyMatrix.csv","r")) == NULL){
		printf("Can not open MyMatrix.csv!\n");
		return -1;
	}

	char *line,*record;
    char buffer[20450];//20450这个数组大小也要根据自己文件的列数进行相应修改。

	int val;
	while ((line = fgets(buffer, sizeof(buffer), Fid))!=NULL){//当没有读取到文件末尾时循环继续
		record = strtok(line, ",");
		while(record != NULL){
			val = atoi(record);
			printf("%d",val);
			record = strtok(NULL, ",");
		}
		printf("\n");
	}
	fclose(Fid);
	Fid = NULL;
	return 0;
}


