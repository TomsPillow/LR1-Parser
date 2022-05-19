#ifndef _TOKEN_HPP_
#define _TOKEN_HPP_
#include <string>
using namespace std;

typedef struct Token
{
    int isEnd;
    int line;
    int col;
    string value;
    string type;
    string numType;
    Token():isEnd(1){};
    Token(const int &line,const int &col,const string &v, const string &t,const string &nt)
        :line(line),col(col),value(v),type(t),numType(nt),isEnd(0){};
}Token;

#endif