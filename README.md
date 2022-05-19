# LR1-Parser

`LR1-Parser`可作为编译前端语法分析部件，基于LR(1)算法开发，包括LR(1)分析表生成器模块和分析栈模块。



## Usage

* 绘制LR(1)分析表；
* 对记号（Tokens）进行语法分析，给出语法错误位置及相关恢复策略；
* 生成抽象语法树（AST）；



## How to Build

### Third Party Library

yaml-cpp，项目包括已构建好相关的静态库，包括 `macOS/Linux` 平台下的 `LR1-Parser/lib/libyaml-cpp.a`，以及 `windows` 平台下的`libyaml-cppmd.lib`（Release）和`libyaml-cppmdd.lib（Debug）`。

### Environment and Softwares

* macOS/linux + cmake(version above 3.0.0) + Clang
* windows + cmake(version above 3.0.0) + Visual Studio 14 2015(or versions above)

### Build it

* 对于 `windows` 系统，建议直接使用cmake-gui编译，注意 `windows` 系统下，可执行文件`LR1-Parser`所在位置可能处于`build`目录下的某个子目录下；

* 对于 `macOS/linux` 进入项目目录 `LR1-Parser/`下，命令行输入下列命令：

   ```shell
   mkdir build
   cmake -S . -B build
   cd build
   make
   ```

   完成编译后，LR1-Parser即出现在 `LR1-Parser/build`目录下。

   测试：`LR1-Parser/test`下给出了一组 `Pascal` 语法规则文件（Grammar-Test.yml）和词法分析后的记号文件（Tokens-Test.txt）

   ```shell
   cd ..
   sh test.sh
   ```

   查看 `LR1-Parser/test` 目录，若存在相应的LR(1)分析表文件 `LR1-Parse-Table.yml` 和其视图文件 `LR1-Parse-Table.csv`（可用excel软件打开查看），以及构建的语法树视图 `AST.txt`。



## Interface For Lex-Analysis

```c++
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
```



## Interface For Semantic-Analysis

```c++
class SyntaxNode
{
    public:
        int id;
        int father;
        int line;
        int col;
        int isTerminal;
        string value;
        // Only TSymbol
        string stringValue;
        string type;
        vector<int> son;
        string numType;
        // TSymbol
        SyntaxNode(const int &id,const int &line,const int &col,const string &value,const string &type,const string &numType);
        // NSymbol
        SyntaxNode(const int &id,const string &value,const string &type);
        string to_string();
};
```



## Help

在`LR1-Parser/build`目录下输入`./LR1-Parser -?` ，可得到用法提示：

```shell
usage: ./LR1-Parser --Grammar=string [options] ... 
options:
  -g, --Grammar                     Grammar YAML file path (string)
  -t, --Tokens                      Tokens TEXT file path for Syntax-Parse (string [=])
  -l, --Built LR1-Table             Load previously built LR1-Table YAML file (string [=])
  -o, --Output LR1-Table            LR1-Table YAML file path (string [=./LR1-Parse-Table.yml])
  -s, --Show LR1-Table obviously    Show LR1-Table in a CSV file (string [=])
  -a, --Show AST                    TEXT file path of AST (string [=./ast.txt])
  -e, --Easy mode                   Exit when error occured (string [=])
  -d, --Debug mode                  Parse Tokens with stacks-infos(not for windows) (string [=])
  -?, --help                        print this message
```
