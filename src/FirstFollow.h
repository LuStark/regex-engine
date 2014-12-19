#ifndef _FIRST_FOLLOW_
#define _FIRST_FOLLOW_
/* reference to the regex grammer defined in
 * file: Grammer
 * 
 * write the First Set for each token by hand
 */

/* ascii value for the special character:
 *  ( 40    ) 41    * 42      + 43   ? 63
 *  | 124   [ 91    ] 93      \ 92
 */

/* define the first set in the form of bit vector for all vn */

/* for grammer non-special : select all the character as
 * the elements as its firstset,  except '\', '+', '*', '?'
 * '|', '[', and '(' */
void initFirstSet();

extern int First_Regex[];
extern int First_Regex_union[];
extern int First_Regex_link[];
extern int First_Regex_closure[];
extern int First_Regex_top[];
extern int First_normal_char[];
extern int First_escape_char[];
extern int First_char_range[];
extern int First_choose_or_not[];
extern int First_range[];
extern int First_extend_regex[];
extern int First_string[];

void initFollowSet();

extern int Follow_Regex[];
extern int Follow_Regex_union[];
extern int Follow_Regex_link[];
extern int Follow_Regex_closure[];
extern int Follow_Regex_top[];
extern int Follow_normal_char[];
extern int Follow_escape_char[];
extern int Follow_char_range[];
extern int Follow_choose_or_not[];
extern int Follow_range[];
extern int Follow_extend_regex[];
extern int Follow_string[];

/* special-character: ?*+|([\. */

#endif
