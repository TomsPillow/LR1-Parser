#ifndef _GRAMMARPOOL_HPP_
#define _GRAMMARPOOL_HPP_
#include <yaml-cpp/yaml.h>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include "LPDebug.hpp"
#include "SymbolPool.hpp"
#include "ProductionPool.hpp"
#include "ItemsPool.hpp"
#include "LR1ParseTable.hpp"

using namespace std;

class GrammarPool
{
    private:
        string SSymbol;
        SymbolPool *NPool;
        SymbolPool *TPool;
        ProductionPool *ProdPool;
        ItemsPool *ItsPool;
        LR1ParseTable *lr1ParseTable;
        LPDebug lpDebug;
    public:
        GrammarPool();
        ~GrammarPool();
        SymbolPool* getNPool();
        SymbolPool* getTPool();
        ProductionPool* getProdPool();
        LR1ParseTable* getLR1ParseTable();
        void loadGrammar(const string &path);
        void buildLR1ParseTable();
        void outLR1ParseTable(const string &path);
        void loadLR1ParseTable(const string &path);
        void showLR1ParseTable(const string &path);
};
#endif