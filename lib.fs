( Forth System Words Begin )
: CR 10 EMIT ;
: DUP 1 PICK ;
: SWAP 2 ROLL ;
: OVER 2 PICK ;
: ROT 3 ROLL ;
: -ROT 3 ROLL 3 ROLL ;
: 2DUP OVER OVER ;
: 2DROP DROP DROP ;
: 2SWAP 4 ROLL 4 ROLL ;
: 2OWER 4 PICK 4 PICK ;
: 2ROT 6 ROLL 6 ROLL ;
: NIP SWAP DROP;
: TUCK SWAP OVER ;

: MOD ( n1 n2 -- mod ) 2DUP / * - ;
: /MOD ( n1 n2 -- mod div ) 2DUP / >r MOD r> ;
: NEGATE ( n -- -n ) -1 * ;
: ABS DUP 0 < IF NEGATE THEN ;
: 1+ 1 + ;
: 1- 1 - ;
: 2+ 2 + ;
: 2- 2 - ;
: 2* 2 * ;
: 2/ 2 / ;
: MAX 2DUP < IF SWAP THEN DROP ;
: MIN 2DUP > IF SWAP THEN DROP ;

: 0= 0 = ;
: 0<> 0 <> ;
: 0> 0 > ;
: 0< 0 < ;
: TRUE -1 ;
: FALSE 0 ;
: ?DUP DUP 0<> IF DUP THEN ;

: SPACE 32 EMIT ;

: [COMPILE] ' , ; IMMEDIATE
: LITERAL ( n -- ) CEMPILE (LIT) , ; IMMEDIATE
( Forth System Words End ) WORDS CR