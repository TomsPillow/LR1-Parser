#ifndef _ITEMSPOOL_HPP_
#define _ITEMSPOOL_HPP_
#include "Items.hpp"
#include "SymbolPool.hpp"
#include <map>
#include <queue>

class ItemsNode
{
    public:
        int index;
        Items items;
        map<int,Symbol> mapper;
        ItemsNode(const int &index,Items &items);
};

class ItemsPool
{
    public:
        vector<ItemsNode> pool;
        queue<pair<int,Items>> que;
        ItemsPool(const int &SIndex,Items &StartItems);
        ~ItemsPool();
        int findItems(Items &Items);
        void getDFA(SymbolPool *spool,SymbolPool *npool);
};

#endif