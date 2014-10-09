regex-engine(demo)
-------------------------
1.  regex-engine目前处于测试状态，对输入的正则表达式以及给定文本进行匹配，
    识别成功的子串以空格隔开。
    
2.  目前能匹配基本正则表达式，以及两种扩展表达式:正反向预查
     而且我相信只要能精简数据结构，以后可以更容易引入更多功能：
     例如：重复，匿名捕获，任意字符识别等等。

3.  安装及使用

####1. 在项目根目录下执行make, 得到simpleLex: 
        $ make 
####2. 在获取根用户权限下，执行以下安装命令可将simpleLex安装到/usr/bin: 
        $ make install

4. 使用示例
---------------------------------- 
####1. .l文件格式: 
        1. %{ 包含所有全局变量，头文件，宏定义 %} 

        2. 接下来是若干行，每一行代表一个正则表达式: 
            RegexName    RegexExpression 

        3. 为指定的正则表达式添加匹配后的执行代码: 
            %% { RegexName } { code } %%


####2. .l文件示例:
        %{  
            #include <stdio.h> 
        %} 
        digit   [0-9]+ 
        space   \s* 
        word    [A-Za-z]+ 
 
        %% 
        { digit } { printf("NUM"); } 
        { space } { printf(" "); } 
        { word } { printf("WORD"); } 
        %% 

####2. 执行以下命令，得到lex.yy.c文件: 
            $ simpleLex lexfile.l 

####3. 编译lex.yy.c，得到Lex可执行文件: 
            $ cc -o Lex lex.yy.c 

####4. 随意构造test.in文件: 
            313 abc 432  fewfwe f3 

####5. 词法识别：  
            $ ./Lex test.in 
            NUM WORD NUM WORD WORDNUM 

