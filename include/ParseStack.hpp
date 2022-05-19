#ifndef _PARSESTACK_HPP_
#define _PARSESTACK_HPP_

#include <fstream>
#include <sstream>
#include <string>
#include <queue>
#include <stack>
#include "LPDebug.hpp"
#include "SymbolPool.hpp"
#include "ProductionPool.hpp"
#include "LR1ParseTable.hpp"
#include "Token.hpp"
#include "Symbol.hpp"
#include "SyntaxTree.hpp"
using namespace std;
class Recovery
{
    public:
    enum RecoveryType{Insert,Delete,Replace};
    virtual RecoveryType getType()=0;
    virtual int getLine()=0;
    virtual int getCol()=0;
    virtual string debug()=0;
    virtual string getErrToken()=0;
    virtual string getNewToken()=0;
};
class ParseStack
{
    private:
        int NodeCnt=0;
        LPDebug lpDebug;
        LR1ParseTable* parseTable;
        SymbolPool* TPool;
        SymbolPool* NPool;
        ProductionPool* PPool;
        queue<Token> input;
        stack<Symbol> symbolStack;
        stack<int> stateStack;
        SyntaxNodeStack syntaxNodeStack;
        SyntaxTree ast;
    public:
        ParseStack(const string &path,LR1ParseTable* parseTable,SymbolPool *TPool,SymbolPool *NPool,ProductionPool *PPool);
        ~ParseStack();
        bool end();
        bool parseAll();
        #ifndef _MSC_VER_
        void printStacks();
        bool debugAll();
        #endif
        bool parseAllWithRecovery(vector<vector<Recovery*>> &infos);
        bool parseByStep(queue<Token> &tempInput,stack<Symbol> &tempSymbolStack,stack<int> &tempStateStack);
        SyntaxTree getSyntaxTree();
        virtual void showSyntaxTree(const string &path);
        virtual void showSyntaxTree();
};

#endif