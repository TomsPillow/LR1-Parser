#ifndef _ITEMS_HPP_
#define _ITEMS_HPP_
#include <vector>
#include "Symbol.hpp"
#include "ProductionPool.hpp"

using namespace std;

class Item
{
    public:
        int left;
        int prodIndex;
        vector<Symbol> right;
        int pos;
        int forward;
        Item(const int &left, const vector<Symbol> &right, const int &ProdIndex, const int &pos, const int &forward);
        bool operator<(const Item &other) const;
        bool operator==(const Item &other) const;
};

class Items
{
    public:
        ProductionPool *PPool;
        vector<Item> Its;
        Items();
        Items(ProductionPool *PPool);
        Items(Item &Item,ProductionPool *PPool);
        Items(Production &sp, ProductionPool *PPool,const int &forward);
        ~Items();
        int go(const Symbol nextSymbol,Items &nextItems);
        void closure();
        void showItems();
        int size();
        bool ItemExisted(const Item &Item);
        bool operator==(const Items &Items) const;
};

#endif