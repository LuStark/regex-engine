#include "NFA.h"
#include "constant.h"
#include "typedef.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

/* 定义一个最小优先级的符号 */
#define MIN_ORITY_SIGN 1

/* 在栈外符号的优先级 */
static int  OutPri[128] ;

/* 在栈中符号的优先级 */
static int  InPri[128] ; 


static Index    charHashTable[256];

static Index    charHashTable_Init( )
{
    /* 除了下面的字符，其他字符映射结果为0,代表普通字符 */
    charHashTable['('] = 1;
    charHashTable[')'] = 2;
    charHashTable['*'] = 3;
    charHashTable['+'] = 3;
    charHashTable['?'] = 3;
    charHashTable['|'] = 4;
    charHashTable['\\'] = 5;
    charHashTable['['] = 6;
    charHashTable[']'] = 7;
}

/* LinkExist表： LinkExist[指示当前字符和下一字符之间是否存在连结关系 */
static int  LinkExist[8][8]=
                        {
                           /*C, (, ), R, |, \, [, ] */
                            {1, 1, 0, 0, 0, 1, 1, 0}, /*     普通字符(C)     */
                            {0, 0, 0,-1,-1, 0, 0, 0}, /*     (        */
                            {1, 1, 0, 0, 0, 1, 1, 0}, /*     )        */
                            {1, 1, 0,-1, 0, 1, 1, 0}, /*     *+?(R)      */
                            {0, 0,-1,-1,-1, 0, 0, 0}, /*     |        */
                            {0, 0, 0, 0, 0, 0, 0, 0}, /*     \        */ 
                            {0, 0, 0, 0, 0, 0, 0,-1}, /*     [        */
                            {1, 1, 0, 0, 0, 1, 1, 0}  /*     ]       */
                        };

static void 
PriorityInit( )
{
    InPri['('] = 0; InPri['|'] = 3; InPri['&'] = 5 ;
    InPri['*'] = 7; InPri['+'] = 7; InPri['?'] = 7 ;

    OutPri[ 1 ] = -1;
    OutPri[')'] = 1; OutPri['|'] = 2; OutPri['&'] = 4;
    OutPri['*'] = 6; OutPri['+'] = 6; OutPri['?'] = 7;
    OutPri['('] = 8;
}

/* 对于给定的当前字符i和已知的下一个字符j，Curr_Next_Table[ hash(i) ][ hash(j) ]
 * 有三种可能的值： -1, 0, 1。分别代表，不合法，预处理过程中两字符间无须添加&，
 * 两字符间须添加& */

#define     hasLinkBetween(currch,nextch)\
            LinkExist[charHashTable[currch]][charHashTable[nextch]]

// 确认字符c是否为运算符号 
static int isOperator( xchar_t c )
{ 
    if( c=='*' || c=='+' || c=='?' || 
        c=='|' || c=='&' || c==MIN_ORITY_SIGN)
        return 1 ;
    else
        return 0 ;
}

// 确认字符c是否为优先级界定符号 
static int orderOperator( xchar_t c )
{
    if( c =='(' || c==')' )
      return 1;
    else
      return 0;
}

static int InPriority( xchar_t c )
{
    return InPri[ (int)c ] ;
}

static int OutPriority( xchar_t c )
{
    return OutPri[ (int)c ] ;
}

NFA *CreateOneNFA( xchar_t c )
{
    NFA *nfa ;
    
    Edge *e ;

    nfa = malloc( sizeof( NFA ) );
    isNullPointer( nfa );
    
    /* 初始化唯一的两个状态点 */
    nfa -> start = allocStatus();
    nfa -> end   = allocStatus();
    
    /* 初始化唯一的边 */
    {
    e = malloc( sizeof( Edge ) );
    e -> from_Status = nfa -> start ;
    e -> to_Status = nfa -> end ;
    e -> matchContent = malloc( sizeof(c) ) ;
    e -> matchContent[ 0 ] = c ;
    }

    {
    nfa -> start -> ID = 1;
    nfa -> start -> numOfOutEdges = 1;
    nfa -> start -> OutEdges = malloc( sizeof( Edge* ) * 
                                nfa -> start -> numOfOutEdges ) ;
    nfa -> start -> OutEdges[ 0 ] = e ;

    nfa -> end -> ID = 2;
    nfa -> end -> InEdges = malloc( sizeof( Edge* ) * 
                                nfa -> end -> numOfInEdges ) ;
    nfa -> end -> numOfInEdges = 1;
    nfa -> end -> InEdges[ 0 ] = e ;
    nfa -> end -> FinalStatus = true ;
    }

    /* 初始化nfa的Edge数组 */
    {
    nfa -> numOfEdges = 1 ;
    nfa -> edgeArray = malloc( sizeof(Edge*) * nfa->numOfEdges );
    nfa -> edgeArray[ 0 ] = e ; 
    }
    /* 初始化nfa的Status状态数组 */
    {
        nfa -> numOfStatus = 2;
        nfa -> Status = malloc( sizeof(Status*) * nfa->numOfStatus ) ;
        isNullPointer( nfa->Status ); 
        nfa -> Status[ 0 ] = nfa -> start ;
        nfa -> Status[ 0 ] -> ID = 0;
        nfa -> Status[ 1 ] = nfa -> end ;
        nfa -> Status[ 1 ] -> ID = 1;
    }

    nfa -> numOfStatus = 2 ;
    return nfa ;
}

NFA *CreateNullNFA( ) 
{
    NFA *nfa ;
    Edge *e ;

    nfa = malloc( sizeof( NFA ) );
    isNullPointer( nfa );
    
    /* 初始化唯一的两个状态点 */
    nfa -> start = allocStatus();
    nfa -> end   = allocStatus();
    
    /* 初始化唯一的边 */
    {
    e = malloc( sizeof( Edge ) );
    e -> from_Status = nfa -> start ;
    e -> to_Status = nfa -> end ;
    }

    /* 初始化起始点和终点 */
    {
    nfa -> start -> ID = 1;
    nfa -> start -> numOfOutEdges = 1;
    nfa -> start -> OutEdges = malloc( sizeof( Edge* ) * 
                                nfa -> start -> numOfOutEdges ) ;
    nfa -> start -> OutEdges[ 0 ] = e ;

    nfa -> end -> ID = 2;
    nfa -> end -> InEdges = malloc( sizeof( Edge* ) * 
                                nfa -> end -> numOfInEdges ) ;
    nfa -> end -> numOfInEdges = 1;
    nfa -> end -> InEdges[ 0 ] = e ;
    nfa -> end -> FinalStatus = true ;
    }

    /* 初始化nfa的Edge数组 */
    {
    nfa -> numOfEdges = 1 ;
    nfa -> edgeArray = malloc( sizeof(Edge*) * nfa->numOfEdges );
    nfa -> edgeArray[ 0 ] = e ; 
    }

    /* 初始化nfa的Status状态数组 */
    {
        nfa -> numOfStatus = 2;
        nfa -> Status = malloc( sizeof(Status*) * nfa->numOfStatus ) ;
        isNullPointer( nfa->Status ); 
        nfa -> Status[ 0 ] = nfa -> start ;
        nfa -> Status[ 0 ] -> ID = 0;
        nfa -> Status[ 1 ] = nfa -> end ;
        nfa -> Status[ 1 ] -> ID = 1;
    }

    nfa -> numOfStatus = 2 ;
    return nfa ;
}

void 
linkTwoStatus_by_AnEdge( Status *fromS , 
                         Status *toS ,
                         Edge   *e
                        )
{
    if( !e || !fromS || !toS )
        return ;
    e -> from_Status = fromS ;
    e -> to_Status = toS ;
    
    if( fromS -> OutEdges != NULL )
        fromS->OutEdges = realloc(fromS -> OutEdges,(fromS->numOfOutEdges+1)*sizeof(Edge*) ) ;
    else
        fromS -> OutEdges = malloc( sizeof(Edge*) );

    fromS -> OutEdges[ fromS->numOfOutEdges ++ ] = e ;
    toS -> InEdges = realloc( toS -> InEdges, (toS->numOfInEdges+1)*sizeof( Edge* ) );
    toS -> InEdges[ toS -> numOfInEdges ++ ] = e ;

}

Edge
*allocEpsilonEdge( )
{
    Edge *e ;
    e = malloc( sizeof(Edge ));
    if (!e ) 
        return NULL ;
    e -> hasEpsilon = true ;
    return e ;
}

Status  
*allocStatus( )
{
    Status *s;
    s = malloc( sizeof(Status) );
    isNullPointer( s ) ;
    return s;
}

NFA 
*Link ( NFA *frontNFA, NFA *tailNFA )
{
    int i, j, numStatus ;
    NFA *combined_NFA;
    NFA *frontNFAcopy, *tailNFAcopy ;
    Edge *e ;

    e = malloc( sizeof(Edge) * 1 ) ;
    e -> hasEpsilon = true ;
    
    //StatusIDPlus( tailNFA, frontNFA -> numOfStatus ) ;
    frontNFAcopy = CopyNFA( frontNFA );
    tailNFAcopy = CopyNFA( tailNFA );

    frontNFAcopy -> end -> FinalStatus = false ;
    tailNFAcopy -> end -> FinalStatus = false ;
    linkTwoStatus_by_AnEdge( frontNFAcopy->end, tailNFAcopy->start, e ) ;

    combined_NFA = malloc( sizeof( NFA ) ) ;
    combined_NFA -> start = frontNFAcopy -> start ;
    combined_NFA -> end = tailNFAcopy -> end ;
    combined_NFA -> end -> FinalStatus = true ;

    /* 构造合成nfa的状态数组 */
    numStatus = frontNFAcopy -> numOfStatus + tailNFAcopy -> numOfStatus ;
    combined_NFA -> numOfStatus = numStatus ;
    combined_NFA -> Status = malloc( sizeof(Status*) * numStatus);

    for( i = 0; i < numStatus; i++ )
    {
        if( i < frontNFAcopy -> numOfStatus )
        {
            combined_NFA -> Status[ i ] = frontNFAcopy -> Status[ i ];
        }else{
            j = frontNFA -> numOfStatus ;
            combined_NFA->Status[ i ] = tailNFAcopy -> Status[ i-j ];
        }
    }

    /* 构造合成nfa的边数组 */
    combined_NFA -> numOfEdges = frontNFAcopy -> numOfEdges + tailNFAcopy -> numOfEdges + 1;
    combined_NFA -> edgeArray = malloc( sizeof(Edge*) * combined_NFA->numOfEdges);

    for( i = 0; i < frontNFAcopy->numOfEdges; i++ )
        combined_NFA -> edgeArray[ i ] = frontNFAcopy -> edgeArray[ i ];

    combined_NFA -> edgeArray[ frontNFA->numOfEdges ] = e ;

    for( i = 0; i < tailNFAcopy->numOfEdges; i++ )
        combined_NFA -> edgeArray[ i+frontNFA->numOfEdges+1  ] = tailNFAcopy -> edgeArray[ i ];

    /* ID 问题 */
    for( i = 0; i < combined_NFA->numOfStatus; i++ )
        combined_NFA->Status[ i ] -> ID = i;

    return combined_NFA ;
}

NFA 
*Union ( NFA *nfa1, NFA *nfa2 )
{
    int i, j;
    NFA  *combineNFA, *nfa1copy, *nfa2copy ;
    Status *start, *end ;
    Edge *e1_start, *e2_start ;
    Edge *e1_end, *e2_end ;

    nfa1copy = CopyNFA( nfa1 ) ;
    nfa2copy = CopyNFA( nfa2 ) ;
    nfa1copy -> end -> FinalStatus = false;
    nfa2copy -> end -> FinalStatus = false;

    start = malloc( sizeof( Status ) );
    end   = malloc( sizeof( Status ) );

    e1_start = allocEpsilonEdge( );
    e2_start = allocEpsilonEdge( );
    e1_end = allocEpsilonEdge( );
    e2_end = allocEpsilonEdge( );

    linkTwoStatus_by_AnEdge( start, nfa1copy -> start, e1_start);
    linkTwoStatus_by_AnEdge( start, nfa2copy -> start, e2_start);
    
    linkTwoStatus_by_AnEdge( nfa1copy -> end, end , e1_end );
    linkTwoStatus_by_AnEdge( nfa2copy -> end, end , e2_end );

    combineNFA = malloc( sizeof( NFA ) );
    combineNFA -> start = start ;
    combineNFA -> end   = end ;
    combineNFA -> end -> FinalStatus = true ;

    /* 构造合成nfa的状态数组 */
    combineNFA -> numOfStatus = 2 + nfa1copy->numOfStatus + nfa2copy->numOfStatus ;
    combineNFA -> Status = malloc( sizeof(Status*) * combineNFA->numOfStatus );
    
    /* 初始化nfa状态数组的第一个状态 */
    combineNFA -> Status[0] = start ;
    
    for( i = 0; i < nfa1copy -> numOfStatus; i++ )
        combineNFA -> Status[ i+1 ] = nfa1copy -> Status[ i ];
    for( i = 0; i < nfa2copy -> numOfStatus; i++ )
        combineNFA -> Status[ i+1+nfa1copy->numOfStatus ] = nfa2copy->Status[i];

    combineNFA -> Status[ combineNFA->numOfStatus-1 ] = end ;
    

    // 储存边集合 
    {
    combineNFA -> numOfEdges = nfa1copy->numOfEdges + nfa2copy->numOfEdges + 4;
    combineNFA -> edgeArray = malloc( sizeof(Edge*) * combineNFA->numOfEdges );

    combineNFA -> edgeArray[ 0 ] = e1_start;
    combineNFA -> edgeArray[ 1 ] = e2_start;

    for( i = 0; i < nfa1copy -> numOfEdges; i++ )
        combineNFA -> edgeArray[ i+2 ] = nfa1copy -> edgeArray[ i ];
    
    for( i = 0; i < nfa2copy -> numOfEdges; i++ )
        combineNFA -> edgeArray[ i+2+nfa1copy->numOfEdges ] = nfa2copy -> edgeArray[ i ] ;

    combineNFA -> edgeArray[ nfa1copy->numOfEdges + nfa2copy->numOfEdges + 2 ] = e1_end;
    combineNFA -> edgeArray[ nfa1copy->numOfEdges + nfa2copy->numOfEdges + 3 ] = e2_end;
    }

    //ID
    for( i = 0; i < combineNFA->numOfStatus; i++ )
        combineNFA -> Status[ i ] -> ID = i;
    return combineNFA ;
}

NFA 
*Closure( NFA *nfa )
{
    Edge *start_to_nfastart, *nfaEnd_to_nfastart, *nfaEnd_to_end, *start_to_end;
    Status *start, *end ;
    NFA *nfacopy;
    NFA *newnfa;

    int i, numE, numS;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc( sizeof( NFA ) );
    isNullPointer( newnfa );

    /* 构造边数组 */
    newnfa->numOfEdges = nfacopy -> numOfEdges + 4;
    newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    isNullPointer( newnfa->edgeArray );
    /* 构造状态数组 */
    newnfa->numOfStatus = nfacopy -> numOfStatus + 2 ;
    newnfa -> Status = malloc( sizeof(Status*) * newnfa->numOfStatus );
    isNullPointer( newnfa->Status );
    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa -> start = allocStatus();
    newnfa -> Status[ numS++ ] = newnfa -> start ;
    newnfa -> end = allocStatus();
    newnfa -> Status[ newnfa->numOfStatus -1 ] = newnfa->end;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy -> numOfStatus; i++ )
        newnfa->Status[ numS ++ ] = nfacopy->Status[ i ] ;


    /* 初始状态到原nfa的start */
    start_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, nfacopy->start, start_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = start_to_nfastart ;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy->numOfEdges; i++ )
        newnfa -> edgeArray[ numE++ ] = nfacopy -> edgeArray[ i ];

    /* 原nfa的end -> 原nfa的start */
    nfaEnd_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, nfacopy->start, nfaEnd_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_nfastart ;

    /* 原nfa的end状态 -> 新nfa的end状态 */
    nfaEnd_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, newnfa->end, nfaEnd_to_end ) ;
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_end ;
    
    /* 新nfa的start -> 新nfa的end */
    start_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, newnfa->end, start_to_end );
    newnfa -> edgeArray[ numE++ ] = start_to_end ;

    if( numE != newnfa->numOfEdges ){
        printf("numE != numOfEdges ");
        while(1){}
    }

    for( i = 0; i < newnfa->numOfStatus; i++ ){
        newnfa -> Status[ i ]->ID = i;
    }
    return newnfa ;
}

NFA
*Option( NFA *nfa )
{
    Edge *start_to_nfastart, *nfaEnd_to_end, *start_to_end;
    Status *start, *end ;
    NFA *nfacopy;
    NFA *newnfa;

    int i, numE, numS;
    numE = numS = 0 ;

    nfacopy = CopyNFA(nfa);
   
    newnfa = malloc( sizeof( NFA ) );
    isNullPointer( newnfa );

    /* 构造边数组 */
    newnfa->numOfEdges = nfacopy -> numOfEdges + 3;
    newnfa -> edgeArray = malloc( sizeof(Edge*) * newnfa->numOfEdges );
    isNullPointer( newnfa->edgeArray );
    /* 构造状态数组 */
    newnfa->numOfStatus = nfacopy -> numOfStatus + 2 ;
    newnfa -> Status = malloc( sizeof(Status*) * newnfa->numOfStatus );
    isNullPointer( newnfa->Status );
    /* 构造初始，终止状态, 并加入到Status数组上 */
    newnfa -> start = allocStatus();
    newnfa -> Status[ numS++ ] = newnfa -> start ;
    newnfa -> end = allocStatus();
    newnfa -> Status[ newnfa->numOfStatus -1 ] = newnfa->end;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy -> numOfStatus; i++ )
        newnfa->Status[ numS ++ ] = nfacopy->Status[ i ] ;


    /* 初始状态到原nfa的start */
    start_to_nfastart = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, nfacopy->start, start_to_nfastart );
    newnfa -> edgeArray[ numE++ ] = start_to_nfastart ;

    /* 利用原nfa的状态数组，构造新nfa的状态数组 */
    for( i = 0; i < nfacopy->numOfEdges; i++ )
        newnfa -> edgeArray[ numE++ ] = nfacopy -> edgeArray[ i ];

    /* 原nfa的end状态 -> 新nfa的end状态 */
    nfaEnd_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( nfacopy->end, newnfa->end, nfaEnd_to_end ) ;
    newnfa -> edgeArray[ numE++ ] = nfaEnd_to_end ;
    
    /* 新nfa的start -> 新nfa的end */
    start_to_end = allocEpsilonEdge();
    linkTwoStatus_by_AnEdge( newnfa->start, newnfa->end, start_to_end );
    newnfa -> edgeArray[ numE++ ] = start_to_end ;

    if( numE != newnfa->numOfEdges ){
        printf("numE != numOfEdges ");
        while(1){}
    }

    for( i = 0; i < newnfa->numOfStatus; i++ ){
        newnfa -> Status[ i ]->ID = i;
    }
    return newnfa ;
}

NFA
*Repeat_atleast_one( NFA *nfa )
{
    NFA *nfa1, *nfa2, *newnfa ;

    nfa1 = CopyNFA( nfa ) ;
    nfa2 = Closure( nfa ) ;

    newnfa = Link( nfa1, nfa2 ) ;
    return newnfa;
}

/* 预处理正则表达式：  1.对于&,添加转义。2.为某些字符之间添加&. */

xchar_t
*PreprocessExpression( xchar_t *RegExp )
{
    xchar_t     buffer[ MAXLEN_REGEXP ], *newExpression;
    xchar_t     currch, nextch;
    int         i,sizeOfBuffer;
    int         length;
    int         tcode;

    /* 8位状态指示器。 其中右边第一位表明是否可选
     * 右边第二位表明是否为当前字符转义（为1则转义）
     */
    char    state;
    
    i =  state= sizeOfBuffer = 0;
    length  =   wcslen(RegExp);
   
    charHashTable_Init();
    /* 检查表达式中第一个字符是否为普通字符,或者左括号 */
    tcode = charHashTable[ RegExp[0] ];
    if( tcode==2 || tcode==3 || tcode==4 || tcode==7 )
    {
        wprintf(L"Illegal expression!\n");
        return NULL;
    }

    i = 0;
    while( i<length-1 ) 
    {
        switch( RegExp[i] )
        {
        case '\\':
            i++;
        case '&':
            buffer[sizeOfBuffer++] = '\\';
            state |= 2;
            break;
        case '[':
            state |= 1;
            break;
        case ']':
            /* 清空可选符号 */
            state &= 2;
            break;
        }
        if( i==length-1) break;

        currch  =   RegExp[i];
        nextch  =   RegExp[i+1];

        buffer[ sizeOfBuffer++ ] = currch;
        
        tcode = hasLinkBetween( currch, nextch );

        if( (state&2) == 2 )
        {
            /* currch是被转义字符 */
            tcode   =   hasLinkBetween(0,nextch);
            state &= 1;
        }

        if( (state&2)!=2 && tcode==-1 ){
            return NULL;
        }

        /* 不是可选范围内是加&的必要条件 */
        if( (state & 1)==0 )
        {
            /* state&2==2表明是转义字符(curr被转义), 所以视为普通字符 
             * 若普通字符（这里以'a'代替） 与 nextch 在判断表中为1,可加&
             */
            if( tcode==1 || ((state&2)==2 && LinkExist['a'][nextch]==1) )
            {
                buffer[ sizeOfBuffer++ ] = '&';
                /* 清空第2位 */
                state &= 1;

            }
        }
        i++;
    }
    buffer[ sizeOfBuffer++ ] = RegExp[ length-1 ];
    buffer[ sizeOfBuffer++ ] = '\0';

    newExpression   =   malloc( sizeof(xchar_t)*sizeOfBuffer );
    isNullPointer( newExpression );
    wcscpy( newExpression,buffer );

    return newExpression;
}

NFA *RegexpToNFA( xchar_t *regExp )
{
    int     i, SizeOfExp ;
    NFA     *nfaStack[ MAXLEN_REGEXP ], *newnfa, *newnfa2, *nfa1, *nfa2 ;
    Edge    *e ;
    xchar_t opeStack[ MAXLEN_REGEXP ], *newExp ;
    /* NFA栈栈顶指针 */
    int     nfatop ;
    /* 运算栈栈顶指针 */
    int     opetop ;

    xchar_t ch ;
    xchar_t sh ;

    nfatop = opetop = -1 ;

    /* 初始化静态变量 */
    PriorityInit( );

    charHashTable_Init();
    newExp      =   PreprocessExpression( regExp );
    if( newExp==NULL )
        return;
    SizeOfExp   =   wcslen( newExp ) ;


    for( i = 0; i < SizeOfExp || opetop>=0 ; i++ )
    {
        ch = newExp[ i ] ;
        if( i >= SizeOfExp )
            ch = MIN_ORITY_SIGN ; 
 
        if( isOperator( ch ) || orderOperator( ch ) ){
            if( opetop >=0 && OutPriority( ch ) < InPriority( opeStack[opetop] ) )
            {
                /* 从栈顶提取的运算符 */
                sh      = opeStack[ opetop-- ];
                /* 提取newnfa 和 newnfa2 ，准备进行 sh 运算 */
                nfa1    = nfaStack[ nfatop   ];
                nfa2    = nfaStack[ nfatop-1 ];
                switch( sh )
                {
                case '&':
                    nfatop -= 2;
                    newnfa = Link( nfa2, nfa1 ) ;
                    break;
                case '|':
                    nfatop -= 2;
                    newnfa = Union( nfa2, nfa1 );
                    break;

                case '*':
                    /* 闭包指示符号，表示重复0次或者多次 */
                    nfatop -= 1;
                    newnfa = Closure( nfa1 );
                    break;

                case '+':
                    /* 重复1次或者以上 */
                    nfatop -= 1;
                    newnfa = Repeat_atleast_one( nfa1 );
                    break;

                case '?':
                    /* 可选的正则表达式 */
                    nfatop -= 1;
                    newnfa = Option( nfa1 ) ;
                    break;

                default:
                    break;
                }
                nfaStack[ ++nfatop ] = newnfa ;
                if( ch == ')' )
                {
                    i--;
                }else if( ch != MIN_ORITY_SIGN )
                {
                    opeStack[ ++opetop ] = ch ;
                }
            }
            else
            {
                if( ch==')' )
                {
                    /* 这时运算符栈的栈顶必然是 '(' */
                    if( opeStack[ opetop ] != '(' )
                    {
                        printf("分析程序出错，括号无法配对\n");
                        exit(1);
                    }
                    /* 消去( */
                    opetop--;
                }
                else
                {
                    opeStack[ ++opetop ] = ch ;
                }
            }
        }
        else
        {
            /* 即使检测到非运算符号，也需要分情况讨论, 并构建NFA */
            switch( ch )
            {
            case '.':
                /* 正则表达式中出现 . 就代表任意字符 */
                newnfa  = CreateNullNFA( );
                newnfa->start->OutEdges[ 0 ]->AllowedAnyChar = true;
                break;
            case '\\':
                /* 出现 \a  反斜杠后面的字符被转义，如: \*, 代表*这个字符。 */
                ch = newExp[ ++i ];
                switch( ch )
                {
                case 'w': 
                case 'W': 
                        newnfa = CharRange( L"[A-Za-z0-9_]",0 );
                        break;
                case 's':
                case 'S':
                        newnfa = CreateOneNFA( ' ' );
                        newnfa2= CreateOneNFA( '\t' );
                        newnfa = Union( newnfa, newnfa2 );
                        break;
                case 'n':
                        newnfa = CreateOneNFA('\n');
                        break;
                default:
                        newnfa = CreateOneNFA( ch );
                        break;
                }
                break;
            case '[':
                /* 出现 [ 时，应该扫描这个表达式，返回一个选择的NFA */
                newnfa = CharRange( newExp, i );
                while( newExp[i]!=']' ) i++;
                break;
            default:
                /* 普通的字符 */
                newnfa = CreateOneNFA( ch );
            }
            nfaStack[ ++nfatop ] = newnfa ;

        }

    }
    /* 预计的结果： nfa栈中最后只剩一个nfa */
    free( newExp );
    return nfaStack[ 0 ] ;
}


/* 能自动扫描表示范围的正则表达式，
 * 返回一个NFA, 并且能够将扫描器前移
 *  到 ']' 后。 */
/* RegExp[ start_offset ] = '[' */
NFA *CharRange( xchar_t *RegExp, int start_offset )
{
    int     i ;
    int     end_offset ;
    bool    inThisRange ;
    xchar_t fromChar, toChar, *content ; 
    NFA     *newnfa ;
    Range   *range;

    int     rangeBufferSize = 0;
    Range   RangeBuffer[ MAXLEN_REGEXP ];
    int     charBufferSize = 0;
    xchar_t CharBuffer[ MAXLEN_REGEXP ];

    i = start_offset + 1;

    if(  RegExp[i] != '^' )
        inThisRange = true ;
    else
    {
        inThisRange = false ; i++ ;
    }
    
    fromChar = toChar = L'\0';
    /* 正则表达式 RegExp已确认合法 */
    while( RegExp[ i ] != ']' )
    {
        if( RegExp[ i ] == '-' )
        {
            if( fromChar != '\0' )
            {
                toChar = RegExp[ ++i ] ;
                /* 到达边界 */
                if( toChar == ']' )
                {
                    /* 将fromChar和'-'添加到字符集合里面 */
                    CharBuffer[ charBufferSize++ ] = fromChar ;
                    CharBuffer[ charBufferSize++ ] = '-';
                    break;
                }
                if( toChar < fromChar )
                {
                    printf("范围错误。\n");
                    while(1){}
                }
                /* record the range in the buffer */
                RangeBuffer[ rangeBufferSize ].from = fromChar ;
                RangeBuffer[ rangeBufferSize ].to   = toChar;
                rangeBufferSize ++;

                fromChar = L'\0';
            }
            else{
                /* '-' 是范围 */
                if( RegExp[i+1] == ']' )
                    CharBuffer[ charBufferSize++ ] = '-';
                fromChar = '-'; 
            }
        }
        else{
            /* 如果下一个字符是 '-', 标记RegExp[i] 为fromChar */
            if( RegExp[ i+1 ]!=']' && RegExp[ i+1 ]=='-' )
            {
                fromChar = RegExp[ i ] ;
            }
            else
            {
                /* 添加至字符集 */
                CharBuffer[ charBufferSize++ ] = RegExp[i];
            }
        }
        i++ ;
    }
    CharBuffer[ charBufferSize ] = '\0';
    
    if( rangeBufferSize > 0 )
    {
        range = malloc( sizeof(Range) * rangeBufferSize ) ;
        isNullPointer( range );
        for( i = 0; i < rangeBufferSize; i++ )
        {
            range[i].from = RangeBuffer[i].from ;
            range[i].to   = RangeBuffer[i].to ;
        }
    }
    if( charBufferSize > 0 )
    {
        content = malloc( sizeof(xchar_t) * (charBufferSize+1) ) ;
        isNullPointer( content ) ;
        for( i = 0; i < charBufferSize; i++ )
            content[ i ] = CharBuffer[ i ];
    }

    newnfa = CreateNullNFA( );
    if( inThisRange == true )
    {
        if( rangeBufferSize )
        {
            newnfa -> edgeArray[0] -> numOfMatchRange = rangeBufferSize ;
            newnfa -> edgeArray[0] -> matchRange = range ;
        }
        if( charBufferSize )
        {
            newnfa -> edgeArray[0] -> matchContent = content ;
        }
    }else
    {
        if( rangeBufferSize )
        {
            newnfa -> edgeArray[0] -> numOfInMatchRange = rangeBufferSize ;
            newnfa -> edgeArray[0] -> inMatchRange = range ;
        }
        if( charBufferSize ){
            newnfa -> edgeArray[0] -> inMatchContent = content ;
        }
    }

    return newnfa ;
}


void EdgeArrayAppend( Edge ***edgeArray, int *Size, Edge *newEdge )
{
    if( *Size == 0 )
    {
        *edgeArray = malloc( sizeof(Edge*) );
        *edgeArray[ 0 ] = newEdge ;
        *Size = *Size + 1;
    }
    else
    {
        *edgeArray = realloc( *edgeArray, (*Size+1)*sizeof(Edge*) );
        *edgeArray[ *Size ] = newEdge ;
        *Size = *Size + 1;
    }
}

NFA *CopyNFA( NFA * nfa )
{
    int i, id ;
    NFA *copyNFA ;
    Edge *e, **InEdges, **OutEdges ;
    Status *fromStatus, *toStatus;

    copyNFA = malloc( sizeof( NFA ) );
    /* 首先，复制nfa状态数组中所有状态数组，除InEdges和OutEdges */
    isNullPointer( copyNFA );
    copyNFA -> numOfStatus = nfa -> numOfStatus ;
    copyNFA -> Status = malloc( sizeof( Status* ) * copyNFA->numOfStatus );
    for( i = 0; i < copyNFA -> numOfStatus; i++ )
    {
        copyNFA -> Status[ i ] = allocStatus(); 
        copyNFA -> Status[ i ]->ID = i;
        copyNFA -> Status[ i ] -> numOfInEdges = 0 ;
        copyNFA -> Status[ i ] -> InEdges = NULL ;
        copyNFA -> Status[ i ] -> numOfOutEdges = 0 ;
        copyNFA -> Status[ i ] -> OutEdges = NULL ;
    }

    /* 第二步，复制所有边(除了指向前后Status的储存单元 */
    copyNFA -> numOfEdges = nfa -> numOfEdges ;
    copyNFA -> edgeArray = malloc( sizeof( Edge* ) * copyNFA->numOfEdges );
    for( i = 0; i < copyNFA -> numOfEdges; i++ )
    {
        copyNFA -> edgeArray[ i ] = CopyEdge( nfa->edgeArray[i] );
        /* set from_Status for E[i] */
        id = nfa -> edgeArray[i] -> from_Status -> ID ;
        copyNFA -> edgeArray[ i ] -> from_Status = copyNFA->Status[ id ];
        
        /* set to_Status for E[i] */
        id = nfa -> edgeArray[i] -> to_Status -> ID ;
        copyNFA -> edgeArray[ i ] -> to_Status = copyNFA->Status[ id ];

    }

    /* 第三部，针对edgeArray的每一条边，生成每个Status的InEdges和OutEdges */
    for( i = 0; i < copyNFA -> numOfEdges; i++ )
    {
        e = copyNFA -> edgeArray[ i ] ;
        fromStatus = e -> from_Status;
        toStatus = e -> to_Status ;

        /* 更新fromStatus 的OutEdges */
        id = fromStatus -> ID ;
        if( fromStatus -> numOfOutEdges == 0 )
            fromStatus -> OutEdges = malloc( sizeof(Edge*) * 1 );
        else
            fromStatus -> OutEdges = realloc( 
                                        fromStatus->OutEdges, 
                                        (fromStatus->numOfOutEdges+1) * sizeof(Edge*) 
                                        );
        fromStatus -> OutEdges[ fromStatus->numOfOutEdges ++ ] = e;

        //EdgeArrayAppend(&copyNFA->Status[id]->OutEdges,&(copyNFA -> Status[id] -> numOfOutEdges),e);
        
        id = toStatus -> ID ;
        //EdgeArrayAppend(&copyNFA -> Status[id] -> InEdges,&(copyNFA -> Status[id] -> numOfInEdges),e);
        if( toStatus -> numOfInEdges == 0 )
            toStatus -> InEdges = malloc( sizeof(Edge*) ) ;
        else
            toStatus -> InEdges = realloc( 
                                        toStatus->InEdges, 
                                        (toStatus->numOfInEdges+1) *sizeof(Edge*) 
                                        );
        toStatus -> InEdges[ toStatus->numOfInEdges ++ ] = e;
        
    }

    /* 最后，更新start 和 end */
    copyNFA -> start = copyNFA -> Status[ 0 ] ;
    copyNFA -> end   = copyNFA -> Status[ copyNFA->numOfStatus-1 ] ;
    copyNFA -> end -> FinalStatus = true ;
    return copyNFA ;

}

Edge *CopyEdge( Edge *e )
{
    Edge *newedge ;
    newedge = malloc( sizeof( Edge ) );
    /* 值拷贝 */
    *newedge = *e ;
    newedge -> from_Status = newedge -> to_Status = NULL ;

    if( e->matchContent != NULL )
        wcscpy( newedge->matchContent, e->matchContent );

    if( e->inMatchContent != NULL )
        wcscpy( newedge -> inMatchContent, e -> inMatchContent ) ;
        
    RangeCopy( newedge -> matchRange , e->matchRange, e->numOfMatchRange) ;
    RangeCopy( newedge -> inMatchRange, e->inMatchRange, e->numOfInMatchRange ) ; 
    
    return newedge ;

}

void RangeCopy(
                Range *toRangeArray,
                Range *fromRangeArray,
                int size
            )
{
    int i ;
    if( size <= 0 )
        return ;
    toRangeArray = malloc( sizeof( Range ) * size ) ;
    isNullPointer( toRangeArray ) ;

    for( i = 0; i < size; i++ )
        toRangeArray[ i ] = fromRangeArray[ i ];
}


bool
itemInSet( int id, int Set[], int size )
{
    int i;
    for( i = 0; i < size; i++ )
        if( id == Set[i] )
            return true;
    return false;
}

void 
generateCondSet( NFA *nfa, Condition *CondSet,  int *size )
{
    Condition   cache[ MAXLEN_REGEXP ];
    int     i, j, cacheSize = 0 ;
    int     is_repeat_condition;

    for( i = 0; i < nfa->numOfEdges; i++ )
    {
        if( nfa->edgeArray[i]->hasEpsilon == false )
        {
            /* 遍历cache集合中所有条件，观察有无重复的条件 */
            is_repeat_condition = 0;
            for( j=0; j<cacheSize; j++ )
            {
                if( EqualCondition( nfa->edgeArray[i], cache[j] ))
                {
                    is_repeat_condition = 1;
                    break;
                }
            }
            if( is_repeat_condition == 0 )
                cache[ cacheSize++ ] = nfa -> edgeArray[i]; 
        }
    }
//    CondSet = malloc( sizeof(Condition) * cacheSize ) ;
//    isNullPointer( CondSet ) ;
    /* 返回条件集合的大小 */
    *size = cacheSize ;

    for( i = 0; i < *size; i++ )
      CondSet[i] = cache[i];

}

bool isMatchCondition( Condition c, xchar_t a )
{
    int i ;
    if( c->matchContent )
    {
        for( i = 0; c->matchContent[i] != L'\0'; i++ )
            if( a == c->matchContent[i] ) return true;
    }
    if( c->inMatchContent )
    {
        for( i = 0; c->inMatchContent[i] != L'\0'; i++ )
            if( a == c->inMatchContent[i] ) return false;
    }

    for( i = 0; i < c->numOfMatchRange; i++ )
        if( c->matchRange[i].from <= a && a <= c->matchRange[i].to )
            return true;

    for( i = 0; i < c->numOfInMatchRange; i++ )
        if( c->inMatchRange[i].from <= a && a <= c->inMatchRange[i].to )
            return false;
    
    /* 若numOfInMatchRange大于0, 或者 inMatchContent不为空,
     * 表明c不在[^...]的范围内，可以通行 */
    if( c->numOfInMatchRange > 0 || c->inMatchContent )
        return true;

    return false;
}

bool
EqualCondition( Condition c1, Condition c2 )
{
    xchar_t *a, *b;
    int     i ;

    if( c1->AllowedAnyChar != c2->AllowedAnyChar )
      return false;

    if( wstrcmp_allow_nullpointer( c1->matchContent,c2->matchContent )!=0 )
      return false;
    if( wstrcmp_allow_nullpointer( c1->inMatchContent,c2->inMatchContent)!=0 )
      return false;

    if( c1->numOfMatchRange != c2->numOfMatchRange )
      return false;

    for( i=0; i < c1->numOfMatchRange; i++ )
      if( !RangeEqual( c1->matchRange[i],c2->matchRange[i] ))
        return false;

    if( c1->numOfInMatchRange != c2->numOfInMatchRange )
      return false;
    
    for( i=0; i < c1->numOfInMatchRange; i++ )
      if( !RangeEqual( c1->inMatchRange[i],c2->inMatchRange[i] ))
        return false;

    return true;
}

int
Status_Transfer_Under_Condition(const Status *status, Condition cond)
{
    int i;
    for(i=0; i < status->numOfOutEdges; i++ )
      if(EqualCondition( status->OutEdges[i], cond ))
        return status->OutEdges[i]->to_Status->ID;
    return -1;
}

void
freeEdge( Edge *edge )
{
    int i;
    if( edge->numOfMatchRange > 0 )
        free( edge->matchRange );
    if( edge->matchContent != NULL )
        free( edge->matchContent );
    if( edge->inMatchRange != NULL )
        free( edge->inMatchRange );
    if( edge->numOfInMatchRange > 0)
        free( edge->inMatchRange );

}

void
freeNFA( NFA *nfa )
{
    int i;
    for( i=0; i < nfa->numOfEdges; i++ )
        freeEdge( nfa->edgeArray[i] );
    free( nfa -> edgeArray );
    for( i=0; i < nfa->numOfStatus; i++ )
        free( nfa->Status[i] );
    
    free(nfa->Status);
}

