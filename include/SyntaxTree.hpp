#ifndef _SYNTAXTREE_HPP_
#define _SYNTAXTREE_HPP_
#include "Symbol.hpp"
#include <fstream>
#include <vector>
using namespace std;

class SyntaxNode
{
    public:
        int id;
        int father;
        int line;
        int col;
        int isTerminal;
        string value;
        string stringValue;// only TSymbol
        string type;
        vector<int> son;
        string numType;
        // TSymbol
        SyntaxNode(const int &id,const int &line,const int &col,const string &value,const string &type,const string &numType);
        // NSymbol
        SyntaxNode(const int &id,const string &value,const string &type);
        string to_string();
};

typedef vector<SyntaxNode> SyntaxNodeStack;
class SyntaxTree
{
    public:
        vector<SyntaxNode> nodeList;
        int nodeNum;
        SyntaxTree(){nodeList.clear();nodeNum=0;};
        ~SyntaxTree(){};
        void insertNode(const SyntaxNode &node);
        virtual void showTree(const string &path);
        virtual void showTree();
    private:
        void dfsTravsersal(SyntaxNode &node,string &res,const int level);
};
#endif