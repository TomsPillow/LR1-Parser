#include "LR1ParseTable.hpp"
#include <iostream>
LR1ParseTable::LR1ParseTable(const int &Row,SymbolPool* NPool,SymbolPool* TPool)
{
    this->row=Row;
    this->npool=NPool;
    this->tpool=TPool;
    this->acol=TPool->size()+1;
    this->gcol=NPool->size();
    this->ActionTable = new Action*[Row];
    this->GotoTable = new Goto*[Row];
    for(int i=0;i<Row;i++)
    {
        this->ActionTable[i] = new Action[this->acol];
        this->GotoTable[i] = new Goto[this->gcol];
    }
}

LR1ParseTable::LR1ParseTable(SymbolPool* NPool,SymbolPool* TPool,const string &YmlPath)
{
    this->npool=NPool;
    this->tpool=TPool;
    this->acol=TPool->size()+1;
    this->gcol=NPool->size();
    this->loadTableFromYML(YmlPath);
}

LR1ParseTable::~LR1ParseTable()
{
    for(int i=0;i<this->row;i++)
    {
        delete []this->ActionTable[i];
        delete []this->GotoTable[i];
        this->ActionTable[i]=NULL;
        this->GotoTable[i]=NULL;
    }
    delete []this->ActionTable;
    delete []this->GotoTable;
    this->ActionTable=NULL;
    this->GotoTable=NULL;
}

int LR1ParseTable::getRow()
{
    return this->row;
}

int LR1ParseTable::getAcol()
{
    return this->acol;
}

int LR1ParseTable::getGcol()
{
    return this->gcol;
}

Action LR1ParseTable::getAction(const int &x,const int &y)
{
    if(x<this->row&&y<this->acol)
    {
        return this->ActionTable[x][y];
    }
    return Action();
}

Goto LR1ParseTable::getGoto(const int &x,const int &y)
{
    if(x<this->row&&y<this->acol)
    {
        return this->GotoTable[x][y];
    }
    return Goto();
}

void LR1ParseTable::loadTableFromYML(const string &YmlPath)
{
    YAML::Node config=YAML::LoadFile(YmlPath);
    this->row=config["Table"].size();
    this->ActionTable = new Action*[this->row];
    this->GotoTable = new Goto*[this->row];
    for(int i=0;i<this->row;i++)
    {
        this->ActionTable[i] = new Action[this->acol];
        this->GotoTable[i] = new Goto[this->gcol];
    }
    int x=0,y=0;
    for(auto state:config["Table"])
    {
        y=0;
        for(auto action:state["State"]["Action"])
        {
            Action a=Action(Action::Type(action["type"].as<int>()),action["value"].as<int>());
            insertAction(x,y,a);
            y++;
        }
        y=0;
        for(auto goTo:state["State"]["Goto"])
        {
            Goto g=Goto(goTo["next"].as<int>());
            insertGoto(x,y,g);
            y++;
        }
        x++;
    }
}

void LR1ParseTable::insertRow(const ItemsNode &node)
{
    for(auto item:node.items.Its)
    {
        if(item.pos==item.right.size())
        {
            if(item.forward!=-1)
            {
                if(ActionTable[node.index][item.forward].value==0)
                {
                    ActionTable[node.index][item.forward]=Action(Action::Reduce,item.prodIndex);
                }
            }
            else if(item.left!=0)
            {
                if(ActionTable[node.index][this->acol-1].value==0)
                {
                    ActionTable[node.index][this->acol-1]=Action(Action::Reduce,item.prodIndex);
                }
            }
            else
                ActionTable[node.index][this->acol-1]=Action(Action::ACC,0);
        }
    }
    for(auto nextPair:node.mapper)
    {
        if(nextPair.second.type==Symbol::TSymbol)
        {
            ActionTable[node.index][nextPair.second.index]=Action(Action::Shift,nextPair.first);
        }
        else if(nextPair.second.type==Symbol::NSymbol)
        {
            GotoTable[node.index][nextPair.second.index]=Goto(nextPair.first);
        }
    }
}

void LR1ParseTable::insertAction(const int &x,const int &y,const Action &a)
{
    if(x<this->row&&y<this->acol)
    {
        
        this->ActionTable[x][y]=a;
    }
}

void LR1ParseTable::insertGoto(const int &x,const int &y,const Goto &g)
{
    if(x<this->row&&y<this->gcol)
    {
        this->GotoTable[x][y]=g;
    }
}

void LR1ParseTable::showTable(const string &path)
{
    ofstream fout(path);
    fout<<",";
    for(int i=0;i<this->tpool->size();i++)
    {

        fout<<"\""<<this->tpool->getString(i)<<"\",";
    }
    fout<<"\"$\",";
    for(int i=1;i<this->npool->size();i++)
    {
        fout<<"\""<<this->npool->getString(i)<<"\",";
    }
    fout<<endl;
    for(int i=0;i<this->row;i++)
    {
        fout<<i<<",";
        for(int j=0;j<this->acol;j++)
        {
            if(this->ActionTable[i][j].type==Action::Shift)
            {
                fout<<"S"<<this->ActionTable[i][j].value<<",";
            }
            else if(this->ActionTable[i][j].type==Action::Reduce)
            {
                
                fout<<"R"<<this->ActionTable[i][j].value<<",";
            }
            else if(this->ActionTable[i][j].type==Action::ACC)
            {
                fout<<"ACC,";
            }
            else
            {
                fout<<",";
            }
        }
        for(int j=1;j<this->gcol;j++)
        {
            if(GotoTable[i][j].nextState!=-1)
                fout<<GotoTable[i][j].nextState<<",";
            else
                fout<<",";
        }
        fout<<endl;
    }
}