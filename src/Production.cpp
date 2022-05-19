#include "Production.hpp"

Production::Production()
{
    this->left=-1;
}

Production::Production(int left)
{
    this->left=left;
}

int Production::getLeft()
{
    return this->left;
}

int Production::RightsSize()
{
    return this->rights.size();
}

void Production::insert(const int index,vector<Symbol> right)
{
    this->rights.push_back(pair<int,vector<Symbol>>(index,right));
}

pair<int,vector<Symbol>> Production::getRight(const int i)
{
    return this->rights[i];
}