#ifndef _PRODUCTIONPOOL_HPP_
#define _PRODUCTIONPOOL_HPP_
#include "Production.hpp"
#include <set>
using namespace std;

class ProductionPool
{
    private:
        int NSymbolCnt;
        vector<Production> pool;
    public:
        vector<set<int> > NSymbolsFirsts;
        vector<int> hasEpsilonFirsts;
        ProductionPool();
        ProductionPool(int NSymbolCnt);
        ~ProductionPool();
        void insert(Production prod);
        Production find(int left);
        void findFirsts();
        int hasEpsilon(int NSymbol);
        set<int> getFirst(int NSymbol);
        SingleProduction getSingleProd(int index);
    private:
        void dfs_for_NSymbol_first(const int NSymbol,set<int> &firsts);
};

#endif