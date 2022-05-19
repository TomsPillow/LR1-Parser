#ifndef _LPDEBUG_HPP_
#define _LPDEBUG_HPP_
#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class LPDebug
{
    private:
        string DebugPath;
        ofstream fout;
        string FormatNow()
        {
            time_t now=time(0);
            tm* t=localtime(&now);
            string year=to_string(t->tm_year+1900);
            string month=to_string(t->tm_mon);
            string day=to_string(t->tm_mday);
            string hour=to_string(t->tm_hour);
            string min=to_string(t->tm_min);
            string sec=to_string(t->tm_sec);
            return "["+year+"/"+month+"/"+day+"\t"
                +hour+":"+min+":"+sec+"]\t";
        }
    public:
        LPDebug()
        {
            this->DebugPath="./debug.txt";
            this->fout.open(this->DebugPath,ios::app);
        }
        LPDebug(const string &path)
        {
            this->DebugPath=path;
            this->fout.open(path,ios::app);
        };
        ~LPDebug()
        {
            this->fout.close();
        };
        void operator<<(const string &info)
        {
            fout<<FormatNow()<<info<<endl;
            fout.flush();
        };
};

#endif