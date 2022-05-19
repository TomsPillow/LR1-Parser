#ifndef _PRODUCTION_HPP_
#define _PRODUCTION_HPP_
#include <vector>
#include "Symbol.hpp"

using namespace std;
typedef struct SingleProduction
{
    int left;
    vector<Symbol> right;
    SingleProduction(int left,vector<Symbol> right):left(left),right(right){};
    SingleProduction(int left):left(left){};
}SingleProduction;


class Production
{
    private:
        int left;
        vector<pair<int,vector<Symbol>>> rights;
        
    public:
        int endIndex=-1;
        int hasEpsilon=0;
        int FirstHasEpsilon=0;
        Production();
        Production(int left);
        int RightsSize();
        int getLeft();
        void insert(const int index, vector<Symbol> right);
        pair<int,vector<Symbol> > getRight(const int i);
};

#endif
