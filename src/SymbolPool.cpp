#include "SymbolPool.hpp"

SymbolNotFoundException::SymbolNotFoundException(string symbol)
{
    this->symbol=symbol;
}

string SymbolNotFoundException::what()
{
    return "Exception: \""+symbol+"\""+" is Not Found.";
}

SymbolPool::SymbolPool()
{
    this->mapper.clear();
    this->pool.clear();
}

SymbolPool::~SymbolPool()
{
    
}


void SymbolPool::insert(string symbol)
{
    int index=pool.size();
    if(this->mapper.find(symbol)==this->mapper.end())
    {
        this->mapper.insert(pair<string,int>(symbol,index));
        this->pool.push_back(symbol);
    }
}

int SymbolPool::exist(string symbol)
{
    return this->mapper.find(symbol)!=this->mapper.end();
}

int SymbolPool::find(string symbol)
{
    if(this->mapper.find(symbol)==this->mapper.end())
    {
        throw SymbolNotFoundException(symbol);
    }
    return this->mapper[symbol];
}

string SymbolPool::getString(const int index)
{
    if(index<this->pool.size())
    {
        return this->pool[index];
    }
    return "index out of range";
}

int SymbolPool::size()
{
    return this->pool.size();
}