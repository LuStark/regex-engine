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
    1. 在项目根目录下输入make命令,这一步编译得到可执行文件simpleLex。
    2. 输入make install（可能输入根用户权限），将simpleLex安装到/usr/bin。

4. simpleLex的使用。
    1. 构造.l文件,格式如下: 
    
    2. 生成lex.yy.c文件: 

    3. 直接编译lex.yy.c文件，导入测试数据test.in: 
