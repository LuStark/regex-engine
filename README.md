simpleLex
=========
1. simpleLex是什么？
-------------------------
    Lex是Unix环境下著名的词法分析程序生成器，simpleLex是
    模仿Lex的一个简陋的版本。
    
2. simpleLex可以干啥?
-------------------------
    与Lex类似，simpleLex采用正则表达式描述每个token的识别  
    规则，simpleLex能为每一个正则表达式生成一个有限自动机， 
    自动机及识别函数以C源码方式展示。而这些信息都储存在名  
    为lex.yy.c中，采用编译器编译这份代码，得到的可执行程序  
    可以识别给定文本。

3. simpleLex的安装
-------------------------
####    1. 在项目根目录下输入make命令,这一步编译得到可执行文件simpleLex。
####    2. 输入make install（可能输入根用户权限），将simpleLex安装到/usr/bin。

4. simpleLex的使用。
----------------------------------
####1. 构造.l文件,格式如下: 
        {% 头文件，宏，全局变量 %} 
         
####2. 接下来是若干行，每一行代表一个正则表达式: 
        RegexName   RegexContent 
        (注: RegexContent中可以取给定命名的正则表达式，采用粘贴的方法) 
        正则表达式的定义结束时，用%%分割 

####3. 为指定的正则表达式添加匹配后的执行代码: 
        { RegexName } { code } 
        结束时用%%分割 

####4. main函数的设置暂且搁置。

5. 使用示例 
---------------------------------- 
####1. 构造.l文件: 
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

