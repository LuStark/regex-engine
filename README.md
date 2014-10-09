regex-engine(demo)
-------------------------
1.  regex-engine目前处于测试状态，对输入的正则表达式以及给定文本进行识别，
    识别成功的子串以空格隔开。
    
2.  simpleLex可以干啥?
-------------------------
    与Lex类似，simpleLex采用正则表达式描述每个token的识别  
    规则，simpleLex能为每一个正则表达式生成一个有限自动机， 
    自动机及识别函数以C源码方式展示。而这些信息都储存在名  
    为lex.yy.c中，采用编译器编译这份代码，得到的可执行程序  
    可以识别给定文本。

3. simpleLex的安装
-------------------------
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

