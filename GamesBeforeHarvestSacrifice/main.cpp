#include <iostream>
#include <vector>
#include <algorithm>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
using namespace std;

vector<vector<int>> friends;
vector<vector<int>> res;
int m,n;
vector<vector<int>> M;
vector<int> txt;

// 构建AB两个存在的好友索引到一张表中
void relationship()
{
    for(int i=0;i<m;i++)
    {
        vector<int> tmp;
        for(int j=0;j<n;j++)
        {
            if(M[i][j]==1)
                tmp.push_back(j+m);
        }
        friends.push_back(tmp);
    }
    for(int j=0;j<n;j++)
    {
        vector<int> tmp;
        for(int i=0;i<m;i++)
        {
            if(M[i][j]==1)
                tmp.push_back(i);
        }
        friends.push_back(tmp);
    }
}

// 判断数组a，b是否完全相同,相同返回true，否则返回false
bool ifsame(vector<int> a,vector<int> b)
{
    bool res=true;
    for(int i=0;i<a.size();i++)
    {
        if(a[i]!=b[i])
            res=false;
    }
    return res;
}

//深度遍历+剪枝去重加速计算
void dfs(int head,int cur,int remained,vector<int>& path)
{
    if(remained==0)
    {
        if(cur==head)
        {
            res.push_back(path);
            return;
        }
        else
            return;
    }

    for(int i=0;i<friends[cur].size();i++)
    {
        int tmp=friends[cur][i];
        if (tmp<head)
            continue;
        if(remained>1 && tmp==head)
            continue;
        if(count(path.begin(),path.end(),tmp)>=1)
            continue;
        path.push_back(tmp);
        dfs(head,tmp,remained-1,path);
        path.pop_back();
    }
}

// 迭代计算4-14的环的结果，先做dfs，再去重，最后print结果
void mainfunction()
{
    int len=4;
    while(len<=14)
    {
        clock_t start, end;
        start = clock();

        for(int i=0;i<m;i++)
        {
            vector<int> path;
            dfs(i,i,len,path);
        } // 产生所有环

        for(int i=0;i<res.size();i++)
            sort(res[i].begin(),res[i].end());
        sort(res.begin(),res.end()); // 内部排序、整体排序

        int ans=1;
        for(int i=1;i<res.size();i++)
        {
            if(!ifsame(res[i],res[i-1]))
                ans++;
            else
                continue;
        }  // 统计不重复数量

        cout<<"Numbers of rings"<<len<<": "<<ans<<endl;
//        保存到txt中
        txt.push_back(ans);

        // 清空res，进入下一次迭代，减少内存消耗
        int s=res.size();
        for(int i=0;i<s;i++)
            res.pop_back();
        end=clock();

        // 输出迭代结果
        cout<<"Time consuming:"<<(double)(end-start)/CLOCKS_PER_SEC<<'s'<<endl;
        len+=2;
    }
    cout<<"Finish !!!"<<endl;
}

void saveTXT(){
    ofstream ofs("result.txt");
    int num=4;
    for(int i=0;i<txt.size();i++){
        ofs<<"木托盘上有"<<num<<"个名字的祭品最多有 : "<<txt[i]<<" 个"<<endl;
        num=num+2;
    }
    ofs.close();
}

int main()
{
    ifstream inFile("Example.csv", ios::in);
    string lineStr;
//    vector<vector<int>> M;
    while(getline(inFile, lineStr))
    {
        stringstream ss(lineStr);
        string str;
        vector<int> row;
        while(getline(ss,str,','))
        {
            if(str=="0")
                row.push_back(0);
            else
                row.push_back(1);
        }
        M.push_back(row);
    } // 读取文件，存放到M中

    m=M.size();
    n=M[0].size();
//    构建AB两个存在的好友索引到一张表中
    relationship();
//计算并打印结果
    mainfunction();
    //将结果保存到txt文档中
    saveTXT();
    return 0;
}