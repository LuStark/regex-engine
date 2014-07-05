simpleLex
=========
###1. simpleLex是什么？  
    Lex是Unix环境下著名的词法分析程序生成器，simpleLex是  
    模仿Lex的而制的玩具。


###2. simpleLex可以干啥?
    与Lex类似，simpleLex采用正则表达式描述每个token的识别  
    规则，simpleLex能为每一个正则表达式生成一个有限自动机， 
    自动机及识别函数以C源码方式展示。而这些信息都储存在名  
    为lex.yy.c中，采用编译器编译这份代码，得到的可执行程序  
    可以识别给定文本。

###3. simpleLex的功能展示

