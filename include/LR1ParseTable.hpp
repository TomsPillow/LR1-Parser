#ifndef _LR1TABLE_HPP_
#define _LR1TABLE_HPP_
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "SymbolPool.hpp"
#include "ItemsPool.hpp"

typedef struct Action
{
    enum Type{Shift,Reduce,Error,ACC};
    Type type;
    int value;
    Action():type(Error),value(0){};
    Action(Type type,int value): type(type),value(value){};
} Action;

typedef struct Goto
{
    int nextState;
    Goto(): nextState(-1){};
    Goto(int nextState): nextState(nextState){};
}Goto;

class LR1ParseTable
{
    private:
        int row;
        int acol,gcol;
        SymbolPool* npool;
        SymbolPool* tpool;
        Action** ActionTable;
        Goto** GotoTable;
        void loadTableFromYML(const string &YmlPath);
    public:
        LR1ParseTable(const int &Row, SymbolPool* NPool, SymbolPool* TPool);
        LR1ParseTable(SymbolPool* NPool, SymbolPool* TPool,const string &YmlPath);
        ~LR1ParseTable();
        int getRow();
        int getAcol();
        int getGcol();
        Action getAction(const int &x,const int &y);
        Goto getGoto(const int &x,const int &y);
        void insertAction(const int &x,const int &y,const Action &a);
        void insertGoto(const int &x,const int &y,const Goto &g);
        void insertRow(const ItemsNode &node);
        void showTable(const string &path);
};

#endif