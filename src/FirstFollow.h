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
void init_FirstSet();

extern int First_regex[];
extern int First_re_union_level[];
extern int First_re_link_level[];
extern int First_re_closure_level[];
extern int First_re_top_level[];
extern int First_non_special[];
extern int First_escape_character[];
extern int First_character[];
extern int First_character_range[];
extern int First_choose_or_not[];
extern int First_range[];
extern int First_closure_op[];

void init_FollowSet();

extern int Follow_regex[];
extern int Follow_re_union_level[];
extern int Follow_re_link_level[];
extern int Follow_re_closure_level[];
extern int Follow_re_top_level[];
extern int Follow_non_special[];
extern int Follow_escape_character[];
extern int Follow_character[];
extern int Follow_character_range[];
extern int Follow_choose_or_not[];
extern int Follow_range[];
extern int Follow_closure_op[];

/* special-character: ?*+|([\. */
    
#endif
