set d201 d1 ; add
set d202 d2 ; sub
set d203 d3 ; mul
set d204 d4 ; div

tc d2 d1
ttsu d2 d1
jmp d2 d1
mov d5 d100

tc d2 d1
ttsu d2 d1
jmp d2 d1
mov d5 d101

tc d2 d1
ttsu d2 d1
jmp d2 d1
mov d5 d102

sub d102 d201 d150
tsnz d150
call d32

sub d102 d202 d150
tsnz d150
call d34

sub d102 d203 d150
tsnz d150
call d36

sub d102 d204 d150
tsnz d150
call d38

mov d103 d4
ts d2 d0 d2

nop
jmp d2 d1

add d100 d101 d103 ; PC 32
return

sub d100 d101 d103 ; PC 34
return

mul d100 d101 d103 ; PC 36
return

div d100 d101 d103 ; PC 38
return