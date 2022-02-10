set d100 d0
set d101 t222222
ts d7 d0 d2
ts d7 d1 d0 ; set timer speed to x3 (as it takes two cycles to check if zero)

mov d100 d8 ; set timer low
mov d100 d9 ; set timer high
ts d7 d2 d2 ; enable timer

tsz d8
jmp d2 d1 ; wait until timer zeros out (overflows)
ts d7 d2 d0 ; dissable timer

sub d101 d100 d3
ts d2 d0 d2 ; print delay
ttsa d2 d0
jmp d2 d1

incsz d100 ; reduce timer delay
goto d4