#include "ProductionPool.hpp"
#include <iostream>
#include <string.h>
ProductionPool::ProductionPool()
{

}

ProductionPool::ProductionPool(int NSymbolCnt)
{
    this->NSymbolCnt=NSymbolCnt;
    this->pool.resize(NSymbolCnt);
    this->hasEpsilonFirsts.resize(NSymbolCnt);
    this->NSymbolsFirsts.resize(NSymbolCnt);
    for(int i=0;i<NSymbolCnt;i++)
    {
        this->hasEpsilonFirsts[i]=0;
        this->NSymbolsFirsts[i].clear();
    }
}

void ProductionPool::insert(Production prod)
{
    this->pool[prod.getLeft()]=prod;
}

ProductionPool::~ProductionPool()
{

}

Production ProductionPool::find(int left)
{
    return this->pool[left];
}


const int maxn=10000;
static int visited[maxn];
void ProductionPool::dfs_for_NSymbol_first(const int NSymbol,set<int> &firsts)
{
    visited[NSymbol]=1;
    for(int i=0;i<this->pool[NSymbol].RightsSize();i++)
    {
        vector<Symbol> right=this->pool[NSymbol].getRight(i).second;
        if(right.size()==0)
        {
            this->pool[NSymbol].FirstHasEpsilon=1;
            this->hasEpsilonFirsts[NSymbol]=1;
        }
        else
        {
            Symbol firstSymbol=right[0];
            if(firstSymbol.type==Symbol::TSymbol)
            {
                firsts.insert(firstSymbol.index);
            }
            else if(firstSymbol.type==Symbol::NSymbol)
            {
                if(this->NSymbolsFirsts[firstSymbol.index].empty())
                {
                    if(!visited[firstSymbol.index])
                    {
                        visited[firstSymbol.index]=1;
                        dfs_for_NSymbol_first(firstSymbol.index,this->NSymbolsFirsts[firstSymbol.index]);
                    }
                    if(this->pool[firstSymbol.index].hasEpsilon)
                    {
                        this->hasEpsilonFirsts[NSymbol]=1;
                        this->pool[NSymbol].FirstHasEpsilon=1;
                    }
                    Symbol curSymbol=firstSymbol;
                    int next=1;
                    while(this->pool[curSymbol.index].hasEpsilon&&next<right.size()
                        &&right[next].type==Symbol::NSymbol)
                    {
                        dfs_for_NSymbol_first(right[next].index,this->NSymbolsFirsts[curSymbol.index]);
                        curSymbol=right[next];
                        next++;
                    }
                    if(this->pool[curSymbol.index].hasEpsilon&&next<right.size()&&right[next].type==Symbol::TSymbol)
                    {
                        firsts.insert(right[next].index);
                    }
                }
                firsts.insert(this->NSymbolsFirsts[firstSymbol.index].begin(),this->NSymbolsFirsts[firstSymbol.index].end());
            }
        }
    }
}

void ProductionPool::findFirsts()
{
    for(auto prod:pool)
    {
        if(prod.getLeft()!=-1)
        {
            memset(visited,0,sizeof(visited));
            visited[prod.getLeft()]=1;
            dfs_for_NSymbol_first(prod.getLeft(),this->NSymbolsFirsts[prod.getLeft()]);
        }
    }
}

int ProductionPool::hasEpsilon(int NSymbol)
{
    return this->hasEpsilonFirsts[NSymbol];
}

set<int> ProductionPool::getFirst(int index)
{
    return this->NSymbolsFirsts[index];
}

SingleProduction ProductionPool::getSingleProd(int index)
{
    for(auto prod:this->pool)
    {
        if(index<prod.endIndex)
        {
            for(int i=0;i<prod.RightsSize();i++)
            {
                if(index==prod.getRight(i).first)
                {
                    return SingleProduction(prod.getLeft(),prod.getRight(i).second);
                }
            }
        }
    }
    return SingleProduction(-1);
}