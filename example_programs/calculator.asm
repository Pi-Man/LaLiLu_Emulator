set d201 d1 ; add
set d202 d2 ; sub
set d203 d3 ; mul
set d204 d4 ; div

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG d100

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG d101

ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG d102

sub d102 d201 d150
tsnz d150
call d28

sub d102 d202 d150
tsnz d150
call d30

sub d102 d203 d150
tsnz d150
call d32

sub d102 d204 d150
tsnz d150
call d34

mov d103 TX_REG

nop
jmp d2 d1

add d100 d101 d103 ; PC 28
return

sub d100 d101 d103 ; PC 30
return

mul d100 d101 d103 ; PC 32
return

div d100 d101 d103 ; PC 34
return