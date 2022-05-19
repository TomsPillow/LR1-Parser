#include <string.h>
#include <iostream>
#include "GrammarPool.hpp"
#include "ParseStack.hpp"
#include "cmdline.h"

int main(int argc, char *argv[])
{
	cmdline::parser cmdParser;
	cmdParser.add<string>("Grammar",'g',"Grammar YAML file path",true,"./Grammar.yml");
    cmdParser.add<string>("Tokens",'t',"Tokens TEXT file path for Syntax-Parse",false);
	cmdParser.add<string>("Built LR1-Table",'l',"Load previously built LR1-Table YAML file",false);
    cmdParser.add<string>("Output LR1-Table",'o',"LR1-Table YAML file path",false,"./LR1-Parse-Table.yml");
	cmdParser.add<string>("Show LR1-Table obviously",'s',"Show LR1-Table in a CSV file",false);
    cmdParser.add<string>("Show AST",'a',"TEXT file path of AST",false,"./ast.txt");
    cmdParser.add<string>("Easy mode",'e',"Exit when error occured",false);
    #ifndef _MSC_VER_
    cmdParser.add<string>("Debug mode",'d',"Parse Tokens with stacks-infos(not for windows)",false);
	#endif

    cmdParser.parse_check(argc,argv);
	
    // Add your Lex-parse code after you modified the cmdParser above
    // ...

	// Syntax-parse
	GrammarPool grammarPool;
	string gpath=cmdParser.get<string>("Grammar");
	grammarPool.loadGrammar(gpath);
	if(cmdParser.exist("Built LR1-Table"))
	{
		string lpath=cmdParser.get<string>("Built LR1-Table");
		grammarPool.loadLR1ParseTable(lpath);
	}
	else
	{
		grammarPool.buildLR1ParseTable();
        if(cmdParser.exist("Output LR1-Table"))
        {
            grammarPool.outLR1ParseTable(cmdParser.get<string>("Output LR1-Table"));
        }
	}

	if(cmdParser.exist("Show LR1-Table obviously"))
	{
		string spath=cmdParser.get<string>("Show LR1-Table obviously");
		grammarPool.showLR1ParseTable(spath);
	}

    if(cmdParser.exist("Tokens"))
    {
        string tpath=cmdParser.get<string>("Tokens");
        ParseStack parseStack(tpath,grammarPool.getLR1ParseTable(),
		grammarPool.getTPool(),grammarPool.getNPool(),grammarPool.getProdPool());

        #ifndef _MSC_VER_
        if(cmdParser.exist("Debug mode"))
        {
            parseStack.debugAll();
        }
        #endif
        if(cmdParser.exist("Easy mode"))
        {
            parseStack.parseAll();
        }
        else
        {
            vector<vector<Recovery*>> infos;
            if(!parseStack.parseAllWithRecovery(infos))
            {
                cout<<"Syntax Error.\nRepair Strategy num:"<<infos.size()<<endl;
                for(int i=0;i<infos.size();i++)
                {
                    cout<<"--------------------------------------------------"<<endl;
                    for(int j=0;j<infos[i].size();j++)
                        cout<<"*** "<<infos[i][j]->debug()<<endl;
                }
                return 0;
            }
        }

        if(cmdParser.exist("Show AST")&&!cmdParser.get<string>("Show AST").empty())
        {
            parseStack.showSyntaxTree(cmdParser.get<string>("Show AST"));
        }
        else
        {
            parseStack.showSyntaxTree();
        }
    }

	// Get Abstract Syntax Tree
	// SyntaxTree ast=parseStack.getSyntaxTree();

	// Add your Semantic-analysis code
	// ...
	return 0;
}