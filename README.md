# LR1-Parser

`LR1-Parser`可作为编译前端语法分析部件，基于LR(1)算法开发，包括LR(1)分析表生成器模块和分析栈模块。



## Usage

* 绘制LR(1)分析表；
* 对记号（Tokens）进行语法分析，给出语法错误位置及相关恢复策略；
* 生成抽象语法树（AST）；



## How to Build

### Third Party Library

yaml-cpp，项目包括已构建好相关的静态库，包括 `macOS/Linux` 平台下的 `LR1-Parser/lib/libyaml-cpp.a`，以及 `windows` 平台下的`libyaml-cppmd.lib（Release）`和`libyaml-cppmdd.lib（Debug）`。

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

   查看 `LR1-Parser/test` 目录，若存在相应的LR(1)分析表文件 `LR1-Parse-Table.yml` 和其视图文件 `LR1-Parse-Table.csv`（可用excel软件打开查看），以及构建的语法树视图 `AST.txt`。且 `LR1-Parser/` 目录下会存在 `debug.txt`文件，其中包含相关日志信息。



## Interface of Lex-Analysis

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



## Interface of Semantic-Analysis

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



## About Grammar

关于语法规则文件`Grammar.yml`，采用YAML文件格式，具体参数项如下例所示：

```yaml
Grammar:
  name: Example
  Start-Symbol: S
  Non-Terminal-Symbols:
    - S
    - A

  # 转义字符使用 "xxx" 囊括该终结符
  Terminal-Symbols:
   - +
   - "-"
   - num

  # 如果表达式右端包含转义字符使用 "xxx .. xxx" 囊括整个右端
  Productions:
    - left: S
      rights:
      	- A + S
      	- "A - S"
        - num

    - left: A
      rights:
        - num
        - num + num
        - "num - num"
```



## About Tokens

关于记号输入 `Tokens.txt` 文件，每个`Token`占用一行，每行采用空格分隔形式依次说明该 `Token` 的行号，列号，名字值，类型值，和数值类型。

例如：`"const PI=3.14;"`中的 `"3.14"`，假定其行号为1，其列号为10，名字值为`3.14`，类型值为`num`，数值类型为`real`。

但是该文件格式存在一定的缺陷，若名字值为空格，则无法识别，**故建议修改代码，使用Token接口连接词法和语法分析**。



## About Error Recovery

关于 `LR1-Parser` 的错误恢复，采用 `Burke-Fisher` 方法的 `Primary Phase` ，仅通过增（InsertRecovery），删（DeleteRecovery）和替换（ReplaceRecovery）对草稿栈进行推测，设定 `MINICHECK`，最小检查距离，若能通过，则可作为一种恢复方式。



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

语法分析模式说明：

* Default: 默认模式，包含错误恢复的语法分析，尽可能一次性分析出程序中存在的所有错误；
* Easy mode: 简单模式，遇到错误直接退出；
* debug mode: 调试模式，可在除windwos环境下使用，打印每次的分析动作及相关分析栈信息。



## Reference

* [A PRACTICAL METHOD FOR SYNTACTIC ERROR DIAGNOSIS AND RECOVERY, by Michael Burke Gerald A. Fisher Jr., Courant Institute New York University 251 Mercer Street New York, New York 10012](https://github.com/samona-omo/LR1-Parser/blob/main/docs/A%20PRACTICAL%20METHOD%20FOR%20SYNTACTIC%20ERROR%20DIAGNOSIS%20AND%20RECOVERY.pdf)

* A Powerful LR Error Recovery Mechanism in the Compiler Implementation System Environment, Mariza A. S. Bigonha, Roberto S. Bigonha 

* A Practical Minimum Distance Method for Syntax Error Handling, J. A. Dain, Department of Computer Science University of Warwick Coventry, CV4 7AL
