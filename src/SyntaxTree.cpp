#include "SyntaxTree.hpp"
#include <iostream>
#include <string>
// TSymbol
SyntaxNode::SyntaxNode(const int &id,const int &line,const int &col,const string &value,const string &type,const string &numType)
{
    this->son.clear();
    this->id=id;
    this->line=line;
    this->col=col;
    this->value=value;
    this->type=type;
    this->isTerminal=1;
    this->numType=numType;
    this->stringValue=value;
}

// NSymbol
SyntaxNode::SyntaxNode(const int &id,const string &value,const string &type)
{
    this->son.clear();
    this->id=id;
    this->isTerminal=0;
    this->value=value;
    this->type=type;
    this->line=-1;
    this->col=-1;
}

string SyntaxNode::to_string()
{
    if(this->isTerminal)
    {
        return "["+std::to_string(this->line)+","+std::to_string(this->col)+"]"
            +"<id: "+std::to_string(this->id)+"   value: "+this->value+"   type: "+this->type+">";
    }
    return std::string("[NSymbol]")+std::string("<id: ")+std::to_string(this->id)+"   value: "+this->value+"   type: "+this->type+">";
}


void SyntaxTree::insertNode(const SyntaxNode &node)
{
    this->nodeList.push_back(node);
    this->nodeNum++;
}

void SyntaxTree::dfsTravsersal(SyntaxNode &node,string &res,const int level)
{
    res.append(node.to_string());
    if(node.son.size()==0)
    {
        return ;
    }
    for(int i=0;i<node.son.size();i++)
    {
        res.append("\n");
        for(int k=0;k<=level;k++)
        {
            res.append("\t");
        }
        res.append("|--- ");
        SyntaxNode child=this->nodeList[node.son[i]];
        dfsTravsersal(child,res,level+1);
    }
    return ;
}

void SyntaxTree::showTree(const string &path)
{
    ofstream fout(path);
    string res;
    res.append(this->nodeList.back().to_string());
    for(int i=0;i<this->nodeList.back().son.size();i++)
    {
        res.append("\n\t|--- ");
        dfsTravsersal(this->nodeList[this->nodeList.back().son[i]],res,1);
    }
    fout << res;
}

void SyntaxTree::showTree()
{
    string res;
    res.append(this->nodeList.back().to_string());
    for(int i=0;i<this->nodeList.back().son.size();i++)
    {
        res.append("\n\t|--- ");
        dfsTravsersal(this->nodeList[this->nodeList.back().son[i]],res,1);
    }
    cout << res;
}
