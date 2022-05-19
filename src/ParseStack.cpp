#include "ParseStack.hpp"
#include <vector>
#include <iostream>
#include <exception>

class InsertRecovery:public Recovery
{
    private:
    int line;
    int col;
    string insertToken;
    string nextToken;
    public:
    virtual RecoveryType getType(){return RecoveryType::Insert;};
    virtual string getErrToken(){return nextToken;};
    virtual string getNewToken(){return insertToken;};
    virtual int getLine(){return line;};
    virtual int getCol(){return col;};
    InsertRecovery(const int line,const int col,string insertToken,string nextToken)
    {
        this->line=line;
        this->col=col;
        this->insertToken=insertToken;
        this->nextToken=nextToken;
    };
    virtual string debug()
    {
        return "Insert \""+insertToken+"\" before \""+nextToken+"\" on line "+to_string(line)+" col "+to_string(col);
    }
};

class DeleteRecovery:public Recovery
{
    private:
    int line;
    int col;
    string deleteToken;
    public:
    virtual RecoveryType getType(){return RecoveryType::Delete;};
    virtual string getErrToken(){return deleteToken;};
    virtual string getNewToken(){return "";};
    virtual int getLine(){return line;};
    virtual int getCol(){return col;};
    DeleteRecovery(const int line,const int col,string deleteToken)
    {
        this->line=line;
        this->col=col;
        this->deleteToken=deleteToken;
    };
    virtual string debug()
    {
        return "Delete \""+deleteToken+"\" on line "+to_string(line)+" col "+to_string(col);
    }
};

class ReplaceRecovery:public Recovery
{
    private:
    int line;
    int col;
    string replaceToken;
    string newToken;
    public:
    virtual RecoveryType getType(){return RecoveryType::Replace;};
    virtual string getErrToken(){return replaceToken;};
    virtual string getNewToken(){return newToken;};
    virtual int getLine(){return line;};
    virtual int getCol(){return col;};
    ReplaceRecovery(const int line,const int col,string newToken,string replaceToken)
    {
        this->line=line;
        this->col=col;
        this->replaceToken=replaceToken;
        this->newToken=newToken;
    };
    virtual string debug()
    {
       return "Replace \""+replaceToken+"\" with \""+newToken+"\" on line "+to_string(line)+" col "+to_string(col);
    }
};



class SyntaxException
{
    private:
        Token etoken;
    public:
        SyntaxException(const Token &token)
        {
            etoken=token;
        }
        string what()
        {
            if(etoken.isEnd)
            {
                return "SyntaxException:*** the program misses its end.";
            }
            return "SyntaxException:*** ["+
                to_string(etoken.line)+","+to_string(etoken.col)+"]: unexpected <value: "+
                etoken.value+"   type: "+etoken.type+">\n"+
                "";
        }
};


static void split(const string &str,vector<string> &tokens,const string &delimiters=" ")
{
    string::size_type lastpos=str.find_first_not_of(delimiters,0);
    string::size_type pos=str.find_first_of(delimiters,lastpos);
    while(string::npos!=lastpos||string::npos!=pos)
    {
        tokens.push_back(str.substr(lastpos,pos-lastpos));
        lastpos=str.find_first_not_of(delimiters,pos);
        pos=str.find_first_of(delimiters,lastpos);
    }
}

ParseStack::ParseStack(const string &path,LR1ParseTable* parseTable,SymbolPool *TPool,SymbolPool *NPool,ProductionPool *PPool)
{
    this->syntaxNodeStack.clear();
    this->TPool=TPool;
    this->NPool=NPool;
    this->PPool=PPool;
    this->parseTable=parseTable;
    ifstream in(path,ios::in);
    stringstream tmp;
    tmp<<in.rdbuf();
    string content(tmp.str());
    in.close();
    // split content
    vector<string> tokens;
    split(content,tokens," \n");
    // read tokens
    for(int i=0;i<tokens.size();i=i+5)
    {
        this->input.push(Token(atoi(tokens[i].c_str()),atoi(tokens[i+1].c_str()),tokens[i+2],tokens[i+3],tokens[i+4]));
    }
    input.push(Token());
    this->lpDebug<<"Tokens have been read.";
    this->stateStack.push(0);
    this->symbolStack.push(Symbol());
}

ParseStack::~ParseStack()
{

}

SyntaxTree ParseStack::getSyntaxTree()
{
    return this->ast;
}


bool ParseStack::end()
{
    return this->input.empty();
}


bool ParseStack::parseByStep(queue<Token> &tempInput,stack<Symbol> &tempSymbolStack,stack<int> &tempStateStack)
{
    if(tempInput.empty())
    {
        return true;
    }
    Token curToken=tempInput.front();
    int index;
    int curState;
    Action a;
    if(curToken.isEnd)
    {
        index=this->TPool->size();
    }
    else
    {
        index=this->TPool->find(curToken.type);
    }
    curState=tempStateStack.top();
    // printf("curState=%d curToken=%s\n",curState,curToken.value.c_str());
    a=this->parseTable->getAction(curState,index);
    if(a.type==Action::ACC)
    {
        tempInput.pop();
        return true;
    }
    else if(a.type==Action::Shift)
    {
        Symbol curSymbol(index,Symbol::TSymbol);
        tempSymbolStack.push(curSymbol);
        tempStateStack.push(a.value);
        tempInput.pop();
    }
    else if(a.type==Action::Reduce)
    {
        SingleProduction sprod=this->PPool->getSingleProd(a.value);
        Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
        // judge symbols and states in stacks
        // ...
        while(!sprod.right.empty())
        {
            Symbol topSymbol=tempSymbolStack.top();
            int topState=tempStateStack.top();
            if(!(sprod.right.back()==topSymbol))
            {
                return false;
            }
            sprod.right.pop_back();
            tempStateStack.pop();
            tempSymbolStack.pop();
        }
        curState=tempStateStack.top();
        Goto g=this->parseTable->getGoto(curState,sprod.left);
        if(g.nextState==-1)
        {
            return false;
        }
        tempStateStack.push(g.nextState);
        tempSymbolStack.push(leftSymbol);
    }
    else
    {
        return false;
    }
    return true;
}

bool ParseStack::parseAll()
{
    while(!this->end())
    {
        Token curToken=this->input.front();
        int index;
        int curState;
        Action a;
        try
        {
            if(curToken.isEnd)
            {
                index=this->TPool->size();
            }
            else
            {
                index=this->TPool->find(curToken.type);
            }
            curState=this->stateStack.top();
            a=this->parseTable->getAction(curState,index);
            if(a.type==Action::ACC)
            {
                // SyntaxTree root
                this->ast.nodeList.push_back(syntaxNodeStack.back());
                syntaxNodeStack.pop_back();
                input.pop();
                this->lpDebug<<"Syntax right.";
                return true;
            }
            else if(a.type==Action::Shift)
            {
                Symbol curSymbol(index,Symbol::TSymbol);
                this->symbolStack.push(curSymbol);
                this->stateStack.push(a.value);
                this->syntaxNodeStack.push_back(SyntaxNode(this->NodeCnt++,curToken.line,curToken.col,curToken.value,curToken.type,curToken.numType));
                this->ast.insertNode(this->syntaxNodeStack.back());
                input.pop();
            }
            else if(a.type==Action::Reduce)
            {
                SingleProduction sprod=this->PPool->getSingleProd(a.value);
                Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
                SyntaxNode parent=SyntaxNode(this->NodeCnt++,this->NPool->getString(leftSymbol.index),this->NPool->getString(leftSymbol.index));
                // judge symbols and states in stacks
                // ...
                while(!sprod.right.empty())
                {
                    Symbol topSymbol=this->symbolStack.top();
                    int topState=this->stateStack.top();
                    if(!(sprod.right.back()==topSymbol))
                    {
                        // throw a exception seems better
                        throw SyntaxException(curToken);
                    }
                    auto back=this->syntaxNodeStack.back();
                    back.father=parent.id;
                    // parent.line=back.line;
                    // parent.col=back.col;
                    this->ast.nodeList[back.id].father=parent.id;
                    parent.son.insert(parent.son.begin(),back.id);
                    sprod.right.pop_back();
                    this->stateStack.pop();
                    this->symbolStack.pop();
                    // syntaxNodeStack is empty ?
                    this->syntaxNodeStack.pop_back();
                }
                curState=this->stateStack.top();
                Goto g=this->parseTable->getGoto(curState,sprod.left);
                if(g.nextState==-1)
                {
                    // throw a exception seems better
                    // ..
                    // return false;
                    throw SyntaxException(curToken);
                }
                this->stateStack.push(g.nextState);
                this->symbolStack.push(leftSymbol);
                this->syntaxNodeStack.push_back(parent);
                this->ast.insertNode(parent);
            }
            else
            {
                throw SyntaxException(curToken);
            }
        }
        catch(SymbolNotFoundException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"\t*** value:"+curToken.value+"\ttype:"+curToken.type;
            return false;
        }
        catch(SyntaxException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"Syntax Error.";
            return false;
        }
    }
    if(!this->symbolStack.empty()||!this->stateStack.empty())
    {
        this->lpDebug<<"Syntax Error.";
        return false;
    }
    return true;
}

void ParseStack::showSyntaxTree()
{
    this->ast.showTree();
}

void ParseStack::showSyntaxTree(const string &path)
{
    this->ast.showTree(path);
}

#ifndef _MSC_VER_
#define     COLOR_NONE      "\033[0m"
#define     L_BLUE          "\e[1;34m"  
#define     YELLOW          "\e[1;33m"
#define     L_GREEN         "\e[1;32m"
#define     L_PURPER        "\e[1;35m"
#define     L_RED           "\e[1;31m"
#define     UNDERLINE       "\e[4m"
#define     BOLD            "\e[1m"
#define     REVERSE         "\e[7m"
void ParseStack::printStacks()
{
    // state stack
    stack<int> temp_st_s=this->stateStack;
    printf(UNDERLINE"State Stack\n");
    printf(COLOR_NONE);
    while(!temp_st_s.empty())
    {
        printf(YELLOW"\t%d",temp_st_s.top());
        temp_st_s.pop();
    }
    printf(COLOR_NONE"\n");

    // symbol stack
    stack<Symbol> temp_sym_s=this->symbolStack;
    printf(UNDERLINE"Symbol Stack\n");
    printf(COLOR_NONE);
    while(!temp_sym_s.empty())
    {
        if(temp_sym_s.top().type==Symbol::NSymbol)
            printf(L_GREEN"\t%s",this->NPool->getString(temp_sym_s.top().index).c_str());
        else if(temp_sym_s.top().type==Symbol::TSymbol)
            printf(YELLOW"\t%s",this->TPool->getString(temp_sym_s.top().index).c_str());
        else
            printf(L_RED"\t-");
        temp_sym_s.pop();
    }
    printf(COLOR_NONE"\n");
}

bool ParseStack::debugAll()
{
    while(!this->end())
    {
        Token curToken=this->input.front();
        int index;
        int curState;
        Action a;
        try
        {
            if(curToken.isEnd)
            {
                index=this->TPool->size();
            }
            else
            {
                index=this->TPool->find(curToken.type);
            }
            curState=this->stateStack.top();
            a=this->parseTable->getAction(curState,index);
            if(a.type==Action::ACC)
            {
                // SyntaxTree root
                this->ast.nodeList.push_back(syntaxNodeStack.back());
                syntaxNodeStack.pop_back();
                input.pop();
                this->lpDebug<<"Syntax right.";
                return true;
            }
            else if(a.type==Action::Shift)
            {
                printf(COLOR_NONE"\n------------------------\n");
                printf(L_PURPER"Shift: ");
                printf(COLOR_NONE);
                printf(L_GREEN"%s",this->TPool->getString(index).c_str());
                printf(COLOR_NONE"\n");
                Symbol curSymbol(index,Symbol::TSymbol);
                this->symbolStack.push(curSymbol);
                this->stateStack.push(a.value);
                this->syntaxNodeStack.push_back(SyntaxNode(this->NodeCnt++,curToken.line,curToken.col,curToken.value,curToken.type,curToken.numType));
                this->ast.insertNode(this->syntaxNodeStack.back());
                input.pop();
            }
            else if(a.type==Action::Reduce)
            {
                SingleProduction sprod=this->PPool->getSingleProd(a.value);
                printf(COLOR_NONE"\n------------------------\n");
                printf(L_PURPER"Reduce: ");
                printf(COLOR_NONE);
                printf(L_GREEN"%s ->",this->NPool->getString(sprod.left).c_str());
                for(int i=0;i<sprod.right.size();i++)
                {
                    if(sprod.right[i].type==Symbol::NSymbol)
                    {
                        printf(L_GREEN" %s",this->NPool->getString(sprod.right[i].index).c_str());
                    }
                    else if(sprod.right[i].type==Symbol::TSymbol)
                    {
                        printf(L_BLUE" %s",this->TPool->getString(sprod.right[i].index).c_str());
                    }
                }
                printf(COLOR_NONE"\n");
                Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
                SyntaxNode parent=SyntaxNode(this->NodeCnt++,this->NPool->getString(leftSymbol.index),this->NPool->getString(leftSymbol.index));
                // judge symbols and states in stacks
                // ...
                while(!sprod.right.empty())
                {
                    Symbol topSymbol=this->symbolStack.top();
                    int topState=this->stateStack.top();
                    if(!(sprod.right.back()==topSymbol))
                    {
                        // throw a exception seems better
                        throw SyntaxException(curToken);
                    }
                    auto back=this->syntaxNodeStack.back();
                    back.father=parent.id;
                    // parent.line=back.line;
                    // parent.col=back.col;
                    this->ast.nodeList[back.id].father=parent.id;
                    parent.son.insert(parent.son.begin(),back.id);
                    sprod.right.pop_back();
                    this->stateStack.pop();
                    this->symbolStack.pop();
                    // syntaxNodeStack is empty ?
                    this->syntaxNodeStack.pop_back();
                }
                curState=this->stateStack.top();
                Goto g=this->parseTable->getGoto(curState,sprod.left);
                if(g.nextState==-1)
                {
                    // throw a exception seems better
                    // ..
                    // return false;
                    throw SyntaxException(curToken);
                }
                this->stateStack.push(g.nextState);
                this->symbolStack.push(leftSymbol);
                this->syntaxNodeStack.push_back(parent);
                this->ast.insertNode(parent);
            }
            else
            {
                throw SyntaxException(curToken);
            }
            printStacks();
        }
        catch(SymbolNotFoundException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"\t*** value:"+curToken.value+"\ttype:"+curToken.type;
            return false;
        }
        catch(SyntaxException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"Syntax Error.";
            return false;
        }
    }
    if(!this->symbolStack.empty()||!this->stateStack.empty())
    {
        this->lpDebug<<"Syntax Error.";
        return false;
    }
    return true;
}
#endif

bool ParseStack::parseAllWithRecovery(vector<vector<Recovery*>> &infos)
{
    int flag=1;
    while(!this->end())
    {
        Token curToken=this->input.front();
        int index;
        int curState;
        Action a;
        try
        {
            if(curToken.isEnd)
            {
                index=this->TPool->size();
            }
            else
            {
                index=this->TPool->find(curToken.type);
            }
            curState=this->stateStack.top();
            a=this->parseTable->getAction(curState,index);
            if(a.type==Action::ACC)
            {
                if(!flag)
                {
                    break;
                }
                // SyntaxTree root
                this->ast.nodeList.push_back(syntaxNodeStack.back());
                syntaxNodeStack.pop_back();
                input.pop();
                this->lpDebug<<"Syntax right.";
                return true;
            }
            else if(a.type==Action::Shift)
            {
                Symbol curSymbol(index,Symbol::TSymbol);
                this->symbolStack.push(curSymbol);
                this->stateStack.push(a.value);
                this->syntaxNodeStack.push_back(SyntaxNode(this->NodeCnt++,curToken.line,curToken.col,curToken.value,curToken.type,curToken.numType));
                this->ast.insertNode(this->syntaxNodeStack.back());
                input.pop();
            }
            else if(a.type==Action::Reduce)
            {
                SingleProduction sprod=this->PPool->getSingleProd(a.value);
                Symbol leftSymbol=Symbol(sprod.left,Symbol::NSymbol);
                SyntaxNode parent=SyntaxNode(this->NodeCnt++,this->NPool->getString(leftSymbol.index),this->NPool->getString(leftSymbol.index));
                // judge symbols and states in stacks
                // ...
                while(!sprod.right.empty())
                {
                    Symbol topSymbol=this->symbolStack.top();
                    int topState=this->stateStack.top();
                    if(!(sprod.right.back()==topSymbol))
                    {
                        // throw a exception seems better
                        throw SyntaxException(curToken);
                    }
                    auto back=this->syntaxNodeStack.back();
                    back.father=parent.id;
                    // parent.line=back.line;
                    // parent.col=back.col;
                    this->ast.nodeList[back.id].father=parent.id;
                    parent.son.insert(parent.son.begin(),back.id);
                    sprod.right.pop_back();
                    this->stateStack.pop();
                    this->symbolStack.pop();
                    // syntaxNodeStack is empty ?
                    this->syntaxNodeStack.pop_back();
                }
                curState=this->stateStack.top();
                Goto g=this->parseTable->getGoto(curState,sprod.left);
                if(g.nextState==-1)
                {
                    // throw a exception seems better
                    // ..
                    // return false;
                    throw SyntaxException(curToken);
                }
                this->stateStack.push(g.nextState);
                this->symbolStack.push(leftSymbol);
                this->syntaxNodeStack.push_back(parent);
                this->ast.insertNode(parent);
            }
            // Error Occurred, repair it with the Primary Recovery Phrase of Burke-Fisher
            else
            {
                vector<Recovery*> info;
                info.clear();
                flag=0;
                const int MINCHECK=15;
                // append current terminal-symbol into the top of input buffer
                queue<Token> backupInput=this->input;
                // keep current line and column
                int curLine=curToken.line;
                int curCol=curToken.col;
                // claim PS reserve current parse stack and input-copy 
                stack<int> stPS=this->stateStack;
                stack<Symbol> symPS=this->symbolStack;
                int curStatePS=curState;
                // Insertion
                int stepCnt=0;
                for(int i=0;i<this->parseTable->getAcol()-1;i++)
                {
                    stack<int> stPE=stPS;
                    stack<Symbol> symPE=symPS;
                    queue <Token> tempBackupInput=backupInput;
                    queue<Token> testInput;
                    stepCnt=0;
                    if(this->parseTable->getAction(curState,i).type!=Action::Error)
                    {
                        string testTSymbol=this->TPool->getString(i);
                        testInput.push(Token(curLine,curCol,testTSymbol,testTSymbol,testTSymbol));
                        while(!tempBackupInput.empty())
                        {
                            testInput.push(tempBackupInput.front());
                            tempBackupInput.pop();
                        }
                        stepCnt++;
                        while(parseByStep(testInput,symPE,stPE)&&stepCnt<MINCHECK)
                        {
                            stepCnt++;
                        }
                        if(stepCnt>=MINCHECK)
                        {
                            InsertRecovery *ir=new InsertRecovery(curLine,curCol,testTSymbol,curToken.value);
                            info.push_back(ir);
                        }
                    }
                }
                // Replace
                backupInput.pop();
                for(int i=0;i<this->parseTable->getAcol()-1;i++)
                {
                    stack<int> stPE=stPS;
                    stack<Symbol> symPE=symPS;
                    queue<Token> tempBackupInput=backupInput;
                    queue<Token> testInput;
                    stepCnt=0;
                    if(this->parseTable->getAction(curState,i).type!=Action::Error)
                    {
                        string testTSymbol=this->TPool->getString(i);
                        testInput.push(Token(curLine,curCol,testTSymbol,testTSymbol,testTSymbol));
                        while(!tempBackupInput.empty())
                        {
                            testInput.push(tempBackupInput.front());
                            tempBackupInput.pop();
                        }
                        stepCnt++;
                        while(parseByStep(testInput,symPE,stPE)&&stepCnt<MINCHECK)
                        {
                            stepCnt++;
                        }
                        if(stepCnt>=MINCHECK)
                        {
                            ReplaceRecovery *pr=new ReplaceRecovery(curLine,curCol,testTSymbol,curToken.value);
                            info.push_back(pr);
                        }
                    }
                }
                // Deletion
                stepCnt=0;
                stack<int> stPE=stPS;
                stack<Symbol> symPE=symPS;
                queue<Token> testInput=backupInput;
                while(parseByStep(testInput,symPE,stPE)&&stepCnt<MINCHECK)
                {
                    stepCnt++;
                }
                if(stepCnt>=MINCHECK)
                {
                    DeleteRecovery *dr=new DeleteRecovery(curLine,curCol,curToken.value);
                    info.push_back(dr);
                }
                if(!info.empty())
                {
                    Recovery *r=info.front();
                    if(r->getType()==Recovery::RecoveryType::Insert)
                    {
                        queue<Token> repairedInput;
                        repairedInput.push(Token(curLine,curCol,r->getNewToken(),r->getNewToken(),r->getNewToken()));
                        while(!this->input.empty())
                        {
                            repairedInput.push(input.front());
                            input.pop();
                        }
                        this->input=repairedInput;
                    }
                    else if(r->getType()==Recovery::RecoveryType::Replace)
                    {
                        this->input.pop();
                        queue<Token> repairedInput;
                        repairedInput.push(Token(curLine,curCol,r->getNewToken(),r->getNewToken(),r->getNewToken()));
                        while(!this->input.empty())
                        {
                            repairedInput.push(input.front());
                            this->input.pop();
                        }
                        this->input=repairedInput;
                    }
                    else if(r->getType()==Recovery::RecoveryType::Delete)
                    {
                        this->input.pop();
                    }
                    infos.push_back(info);
                }
                // can't repair the err
                else
                {
                    this->lpDebug<<"Syntax Error.";
                    return false;
                }
            }
        }
        catch(SymbolNotFoundException& e)
        {
            this->lpDebug<<e.what();
            this->lpDebug<<"\t*** value:"+curToken.value+"\ttype:"+curToken.type;
            return false;
        }
    }
    if(!flag)
    {
        this->lpDebug<<"Syntax Error.";
        this->lpDebug<<"Recovery strategies:";
        for(int i=0;i<infos.size();i++)
		{
            this->lpDebug<<"--------------------------------------------------";
            for(int j=0;j<infos[i].size();j++)
                this->lpDebug<<"\t*** "+infos[i][j]->debug();
		}
        return false;
    }
    return true;
}