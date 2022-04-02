#define ADD_FUN d36
#define SUB_FUN d38
#define MUL_FUN d40
#define DIV_FUN d42

#define ADD d201
#define SUB d202
#define MUL d203
#define DIV d204

#define NUM_1 d100
#define NUM_2 d101
#define OPT d102
#define RES d103

set ADD d1 ; add
set SUB d2 ; sub
set MUL d3 ; mul
set DIV d4 ; div

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG NUM_1

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG NUM_2

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG OPT

sub OPT ADD RES
tsz RES
jmp d0 d3
call ADD_FUN
goto d33

sub OPT SUB RES
tsz RES
jmp d0 d3
call SUB_FUN
goto d33

sub OPT MUL RES
tsz RES
jmp d0 d3
call MUL_FUN
goto d33

sub OPT DIV RES
tsz RES
jmp d0 d2
call DIV_FUN
goto d33

mov RES TX_REG ; PC 33

nop
jmp d2 d1

add NUM_1 NUM_2 RES ; PC 36
return

sub NUM_1 NUM_2 RES ; PC 38
return

mul NUM_1 NUM_2 RES ; PC 40
return

div NUM_1 NUM_2 RES ; PC 42
return