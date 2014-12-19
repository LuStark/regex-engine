#include "LL1.h"

#define MAX 150

wchar_t regex[MAX];
int  currentIndex= 0;
int  LL1_finished_symbol = 0;

typedef unsigned char firstSet;
typedef unsigned char followSet;

void getRegex ()
{
    int i;
    wchar_t c;

    i = 0;
    while ((c=getwchar()) != '\n')
        regex[i++] = c;
    regex[i++] = '$';
    regex[i] = '\0';
}

wchar_t *getT()
{
    int     i;
    wchar_t c, *T, buffer[500];

    i = 0;
    while ((c=getwchar()) != EOF)
        buffer[i++] = c;
    buffer[i] = '\0';
    T = malloc(sizeof(wchar_t)*i);
    assert(T);
    wcscpy(T, buffer);

    return T;

}


static void match (wchar_t obj)
{
    if (LL1_finished_symbol==1)
        return;
    if (regex[currentIndex]==obj)
    {
        wprintf (L"匹配 %lc\n", obj);
        currentIndex++;
        if (regex[currentIndex]=='$')
        {
            wprintf (L"匹配完成.\n");
            LL1_finished_symbol = 1;
        }
    }
    else
    {
        wprintf (L"%lc Don't match %lc\n", regex[currentIndex], obj);
        exit(1);
    }
}

static int error (wchar_t funcName[], wchar_t c)
{
    wprintf (L"%ls中出现无法匹配的字符%c\n", funcName, c);
    exit (1);
}


void cpy(char *pattern)
{
    printf("%s\n", pattern);
    exit(0);
}

void testRegex(char *testfile)
{
    wchar_t regex[150];

    FILE    *f = fopen(testfile, "r");

    /* 目前不支持空格,换行 */
    while ((fwscanf(f, L"%ls", regex)!=EOF))
    {
        regex[wcslen(regex)] = '$';
        regex[wcslen(regex)+1] = '\0';
        
        wprintf(L"%ls\n", regex);
    }
    fclose(f);
}



regexNode   LL1_Regex()
{
    /* firstSet of this symbol is: all except
       ) | + * ? 
     * followSet:   $,)
     */
    wchar_t     c;
    regexNode   r;

    r = Regex_union();

    return r;
}

regexNode   Regex_union() 
{
    wchar_t     c;
    regexNode   re, re2;

    re = Regex_link();
    c = regex[currentIndex];
    while (c == '|')
    {
        match (L'|');
        re2 = Regex_link();

        /* 需要明白的是，Union结束以后，re->nfa以及re2->nfa都会释放内存 */
        re->nfa = Union(re->nfa, re2->nfa);
        c = regex[currentIndex];
    }
    return re;
}

regexNode   Regex_link() 
{
    wchar_t     c;
    NFA         nfa1, nfa2, nfa;
    regexNode   re, re2;

    re = Regex_closure();
 
    c = regex[currentIndex];
    while (First_Regex_closure[c] == 1)
    {
        re2 = Regex_closure();

        nfa = Link(re->nfa, re2->nfa);
        re -> nfa = nfa;

        c = regex[currentIndex];
    }
    //construct_table(re);
    return re;
}

regexNode   Regex_closure()
{
    wchar_t     c, op;
    regexNode   r;
    NFA         nfa, nfa2;
    
    r = Regex_top();
    op = regex[currentIndex];
    
    if (op == '*')
    {
        match('*');
        nfa2 = Closure(r->nfa);
        r->nfa = nfa2;

    }else if (op == '+'){
        match('+');
        nfa2 = Repeat_atleast_one(r->nfa);
        r->nfa = nfa2;

    }else if (op == '?'){
        match('?');
        nfa2 = Option(r->nfa);
        r->nfa = nfa2;

    }else if (op == '{'){
        match('{');
        int n, m;
        n = Integer();

        c = regex[currentIndex];
        if (c == ',')
        {
            match(',');
            c = regex[currentIndex];
            if (c == '}')
            {
                match('}');
            }else{
                m = Integer();
            }
        }

    }

    return r;
}

int Integer()
{
    int     num, factor;
    wchar_t c;
    
    num = 0;
    factor = 1;
    while (c>='0' && c<='9')
    {
        printf("匹配 %c\n", c);
        currentIndex++;
        num = factor*num + (c-'0');
        factor *= 10;
    }
    return num; 
}


regexNode   Regex_top()
{
    regexNode r;
    wchar_t c;

    c = regex[currentIndex];
    
    if (First_normal_char[c] == 1)
    {
        r = normal_character();
    }
    else if (c == '(')
    {
        match ('(');
        c = regex[currentIndex];
        if (c=='?')
        {
            match ('?');
            r = extend_regex();
        }
        else
        {
            r = LL1_Regex();
        }
        match (')');
    }
    else if (c == '\\')
    {
        r = escape_character();
    }
    else if (c == '[')
    {
        r = character_range();
    }
    else
    {
        error (L"regex_top()", c);
    }

    return r;
}

regexNode   extend_regex()
{
    wchar_t     c;
    regexNode   r;

    c = regex[currentIndex];

    if (c == '<'){
        match('<');
        match('=');
        LL1_Regex();

    }else if (c == '='){
        match('=');
        LL1_Regex();        

    }else if (c == '#'){
        match('#');
        match('<');
        string();
        match('>');
        LL1_Regex();

    }else if (First_extend_regex[c] == 1) {
        LL1_Regex();
    }

    return r;
}

regexNode   normal_character()
{
    wchar_t c;
    regexNode   r;
    
    r = alloc_regexNode();

    c = regex[currentIndex];
    
    if (First_normal_char[c] == 1)
    {
        match (c);
        r->nfa = CreateSingleNFA (c);
    }
    else
        error (L"LL1_noncharacter()", c);

    return r;
}

regexNode   escape_character()
{
    wchar_t     c;
    regexNode   r;

    r = alloc_regexNode();

    c = regex[currentIndex];
    if (c != '\\')
        error (L"LL1_noncharacter()", c);

    match ('\\');
    c = regex[currentIndex];
    r->nfa = CreateSingleNFA (c);
    match (c);

    return r;
}

int LL1_choose_or_not()
{
    wchar_t c;
    c = regex[currentIndex];
    
    if (Follow_choose_or_not[c] == 1)
        return 0;

    if (c == '^')
        match ('^');
    else
        error (L"LL1_character()", c);
}

Range range()
{
    wchar_t c1, c2;
    Range   r;
    
    c1 = regex[currentIndex];
    if (First_range[c1] == 1)
    {
        match (c1);
        if (regex[currentIndex] == '-')
        {
            match ('-');
            c2 = regex[currentIndex];
            if (c2 != ']')
            {
                match (c2);
                r.from = c1;
                r.to = c2;
            }
            else
            {
                wprintf(L"范围不应该包含']'\n");
                exit(1);
            }
        }
        else
        {
            /* 如果只有单个字符，范围上下界取该字符 */
            r.from = c1;
            r.to = c1;
        }
    }
    else
        error (L"range()", c1);

    return r;
}

regexNode   character_range()
{
    wchar_t c;
    Range r, bufferRange[280];
    int sizeBuffer;
    Status  start, end;
    Edge   e;
    NFA  p;
    int     i;
    regexNode   re;

    re = alloc_regexNode();

    // 创建两个顶点一条边的NFA
    p = CreateNFA (2,1);
    start = getStartStatus (p);
    end   = getEndStatus (p);
    e     = getEdge (p,0);
    
    match ('[');

    setMatchRangeOrNot (e, LL1_choose_or_not());

    c = regex[currentIndex];
    sizeBuffer = 0;
    while (First_range[c] == 1)
    {
        r = range();
        bufferRange[sizeBuffer++] =  r;
        
        c = regex[currentIndex];
        if (c==']')
            break;
    }

    for (i=0; i<sizeBuffer; i++)
        addRange(e, bufferRange[i]);

    match (']');

    link_Two_Status_In_Automaton (p, 0, 1, 0);

    re->nfa = p; 

    return re;
}

char *string()
{
}




int main ()
{
    int         i;
    NFA         nfa;
    DFA         dfa;
    regexNode   re;
    wchar_t     str[100], pattern[100];
    wchar_t     *T;

    setlocale(LC_CTYPE, "");

    //testRegex("testCase");
 
    init_FirstSet();
    init_FollowSet();
    wprintf(L"输入正则表达式: ");
    wprintf(L"\n");
    get_Regex();

    re = LL1_Regex();


    /*
    wprintf(L"输入正文:");
    T = getT();
    wprintf(L"\n");
    //wchar_t *T = L"abcd";

    int start, end;
    start = 0;

    matchContext(T, re);
    if (re_match2(re, T, &start, &end))
    {
        wprintf(L"识别成功!\n");
    }
    dfa = Subset_Construct(re->nfa);
    print_Automaton(dfa);

    init_Regex(re, dfa);

    wprintf(L"输入待识别串: ");
    while (wscanf(L"%ls", str) != EOF)
        if (Recognition(*re, str))
            wprintf(L"识别成功!\n");
    */
    return 0;
}
