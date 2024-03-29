#define ADD d201
#define SUB d202
#define MUL d203
#define DIV d204

#define NUM_1 d100
#define NUM_2 d101
#define OPT d102
#define RES d103

set ADD '+' ; add
set SUB '-' ; sub
set MUL '*' ; mul
set DIV '/' ; div

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
goto SWITCH_END

sub OPT SUB RES
tsz RES
jmp d0 d3
call SUB_FUN
goto SWITCH_END

sub OPT MUL RES
tsz RES
jmp d0 d3
call MUL_FUN
goto SWITCH_END

sub OPT DIV RES
tsz RES
jmp d0 d2
call DIV_FUN
goto SWITCH_END

SWITCH_END:
mov RES TX_REG ; PC 33

nop
jmp d2 d1

ADD_FUN:
add NUM_1 NUM_2 RES ; PC 36
return

SUB_FUN:
sub NUM_1 NUM_2 RES ; PC 38
return

MUL_FUN:
mul NUM_1 NUM_2 RES ; PC 40
return

DIV_FUN:
div NUM_1 NUM_2 RES ; PC 42
return