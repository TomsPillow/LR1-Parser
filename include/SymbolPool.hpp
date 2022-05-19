#ifndef _SYMBOLPOOL_HPP_
#define _SYMBOLPOOL_HPP_
#include <map>
#include <vector>
#include <string>
#include <exception>
using namespace std;

class SymbolNotFoundException{
    private:
        string symbol;
    public:
        SymbolNotFoundException(string symbol);
        string what();
};


class SymbolPool
{
    private:
        map<string,int> mapper;
        vector<string> pool;
    public:
        SymbolPool();
        ~SymbolPool();
        void insert(string symbol);
        int exist(string symbol);
        int find(string symbol);
        int size();
        string getString(const int index);
};

#endif