#include "GrammarPool.hpp"
#include <iostream>

static void split(const string &str, vector<string> &tokens, const string &delimiters = " ")
{
    string::size_type lastpos = str.find_first_not_of(delimiters, 0);
    string::size_type pos = str.find_first_of(delimiters, lastpos);
    while (string::npos != lastpos || string::npos != pos)
    {
        tokens.emplace_back(str.substr(lastpos, pos - lastpos));
        lastpos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastpos);
    }
}

GrammarPool::GrammarPool()
{
    this->NPool = new SymbolPool();
    this->TPool = new SymbolPool();
    this->ProdPool=NULL;
    this->lr1ParseTable=NULL;
}

GrammarPool::~GrammarPool()
{
    delete this->NPool;
    delete this->TPool;
    delete this->ProdPool;
    if(this->lr1ParseTable!=NULL)
        delete this->lr1ParseTable;
}

SymbolPool* GrammarPool::getNPool()
{
    return this->NPool;
}

SymbolPool* GrammarPool::getTPool()
{
    return this->TPool;
}


ProductionPool* GrammarPool::getProdPool()
{
    return this->ProdPool;
}

LR1ParseTable* GrammarPool::getLR1ParseTable()
{
    return this->lr1ParseTable;
}

void GrammarPool::loadGrammar(const string &path)
{
    this->lpDebug<<"Loading Grammar...";
    YAML::Node config = YAML::LoadFile(path);
    // load Grammar-name
    this->lpDebug<<"Grammar name:"+config["Grammar"]["name"].as<string>();

    // load Start-Symbol
    this->NPool->insert("S'");
    this->SSymbol = config["Grammar"]["Start-Symbol"].as<string>();

    // load Non-Terminal-Symbols
    for (auto iter = config["Grammar"]["Non-Terminal-Symbols"].begin();
         iter != config["Grammar"]["Non-Terminal-Symbols"].end(); iter++)
    {
        string NSymbol = iter->as<string>();
        this->NPool->insert(NSymbol);
    }
    this->lpDebug<<"Non-Terminal-Symbols have been loaded.";

    // load Terminal-Symbols
    for (auto iter = config["Grammar"]["Terminal-Symbols"].begin();
         iter != config["Grammar"]["Terminal-Symbols"].end(); iter++)
    {
        string TSymbol = iter->as<string>();
        this->TPool->insert(TSymbol);
    }
    this->lpDebug<<"Terminal-Symbols have been loaded.";

    this->ProdPool = new ProductionPool(config["Grammar"]["Non-Terminal-Symbols"].size() + 1);

    // load Production S'->SSymbol
    int ProdCnt = 0;
    Production sprod(0);
    vector<Symbol> sright;
    sright.emplace_back(this->NPool->find(SSymbol), Symbol::NSymbol);

    sprod.insert(ProdCnt, sright);
    ProdCnt++;
    this->ProdPool->insert(sprod);
    // load Productions
    YAML::Node productions = config["Grammar"]["Productions"];
    for (size_t i = 0; i < productions.size(); i++)
    {
        string leftStr = productions[i]["left"].as<string>();
        try
        {
            Production prod(NPool->find(leftStr));
            for (size_t j = 0; j < productions[i]["rights"].size(); j++)
            {
                vector<Symbol> right;
                vector<string> list;
                string rightStr = productions[i]["rights"][j].as<string>();
                split(rightStr, list, " ");
                right.clear();
                if (list.size() == 1 && list.front() == "epsilon")
                {
                    prod.hasEpsilon = 1;
                    prod.insert(ProdCnt, right);
                }
                else
                {
                    for (auto each : list)
                    {
                        Symbol symbol;
                        if (this->NPool->exist(each))
                        {
                            symbol.index = this->NPool->find(each);
                            symbol.type = symbol.NSymbol;
                        }
                        else if (this->TPool->exist(each))
                        {
                            symbol.index = this->TPool->find(each);
                            symbol.type = symbol.TSymbol;
                        }
                        else
                        {
                            throw SymbolNotFoundException(each);
                        }
                        right.push_back(symbol);
                    }
                    prod.insert(ProdCnt, right);
                }
                ProdCnt++;
            }
            prod.endIndex = ProdCnt;
            this->ProdPool->insert(prod);
        }
        catch (SymbolNotFoundException &e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"\t*** "+path+": Grammar->Productions["+to_string(i)+"].";
            exit(0);
        }
    }
    this->lpDebug<<"Productions have been loaded.";
}

void GrammarPool::buildLR1ParseTable()
{
    this->ProdPool->findFirsts();
    this->lpDebug<<"First set has been found.";
    Production sprod = this->ProdPool->find(0);
    Items Its0 = Items(sprod, this->ProdPool, -1);
    Its0.closure();
    this->ItsPool = new ItemsPool(0, Its0);
    this->ItsPool->getDFA(this->TPool, this->NPool);
    this->lr1ParseTable = new LR1ParseTable(this->ItsPool->pool.size(), this->NPool, this->TPool);
    for (auto node : this->ItsPool->pool)
    {
        this->lr1ParseTable->insertRow(node);
    }
    this->lpDebug<< "LR1-Parse-Table has been worked out.";
}

void GrammarPool::showLR1ParseTable(const string &path)
{
    this->lr1ParseTable->showTable(path);
}

void GrammarPool::loadLR1ParseTable(const string &path)
{
    this->lr1ParseTable = new LR1ParseTable(this->NPool, this->TPool, path);
    this->lpDebug<<"LR1-Parse-Table has been loaded.";
}

void GrammarPool::outLR1ParseTable(const string &path)
{
    YAML::Node out;
    ofstream fout(path);
    // output ParseTable
    for (int i = 0; i < this->lr1ParseTable->getRow(); i++)
    {
        // output action
        for (int j = 0; j < this->lr1ParseTable->getAcol(); j++)
        {
            Action a = this->lr1ParseTable->getAction(i, j);
            out["Table"][i]["State"]["Action"][j]["type"] = int(a.type);
            out["Table"][i]["State"]["Action"][j]["value"] = a.value;
        }
    }
    for (int i = 0; i < this->lr1ParseTable->getRow(); i++)
    {
        // output goto
        for (int j = 0; j < this->lr1ParseTable->getAcol(); j++)
        {
            Goto g = this->lr1ParseTable->getGoto(i, j);
            out["Table"][i]["State"]["Goto"][j]["next"] = g.nextState;
        }
    }
    fout << out;
}
