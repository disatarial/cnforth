." System Words loading... " cr

: rot 3 roll ;
: -rot 3 roll 3 roll ;
: 2dup over over ;
: 2drop drop drop ;
: 2swap 4 roll 4 roll ;
: 2over 4 pick 4 pick ;
: 2rot 6 roll 6 roll ;
: nip swap drop ;
: tuck swap over ;
: 3dup 3 pick 3 pick 3 pick ;

: mod 2dup / * - ;
: /mod 2dup / >r mod r> ;
: negate 0 swap - ;
: abs dup 0 < if negate then ;
: max 2dup > if drop else nip then ;
: min 2dup < if drop else nip then ;

." System Words successfully loaded! " cr

: test_ok   ." test OK!" cr ;
: test_fail ." test Fail!" cr  ;
: test = if test_ok else test_fail then ;

." Fib List Test: Print 8 Fib list numbers " cr
: fib_ do  .s 2dup + loop ;
: fib  0 1 2swap fib_ .s ;
: test_fib 6 0 fib + + + + + + + 33 test ;
test_fib .s

." Factorial Test: Us recursive to Print Eight's Factorial " cr
variable n
: n@ n @ ;
: n-- n@ 1 - n ! ;
: factorial n@ 0 > if n@ n-- myself * else 1 then .s ; 
see factorial
: test_fact 8 n ! factorial 40320 test ;
test_fact .s

." Matrix: Test nested statements " cr
: star 42 emit ;
: space 32 emit ;
: matrix swap 0 do dup >r 0 do star space loop cr r> loop cr ;
5 4 matrix
 