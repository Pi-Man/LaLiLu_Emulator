#define PRINT_CHAR t222200
#define READ_CHAR t222201
#define TEMP t222202
#define ZERO t222210
#define LIFE t222211
#define PRINT_STRING t222212
#define FLAG t222220
#define OLD_CHAR t222221
#define CORRECT t222222
#define NINE t222122
#define FIVE t222121
#define TWO t222120
#define NUM_ANSWERS t222112
#define ANSWER_PICK t222111

#define GUESS_S t202000
#define ANSWER_S t202100
#define LIFE_S t100000
#define SUCCESS_S t100120
#define FAIL_S_1 t101020
#define FAIL_S_2 t102000
#define VOID t000000

#define ANSWER_LIST t002000

set ZERO '0'
set NINE t100
set FIVE d5
set TWO d2



call Init

call Reset

Main:

set PRINT_STRING GUESS_S
call Print_String
set PRINT_CHAR d10
call Print

add ZERO LIFE PRINT_CHAR
call Print
set PRINT_STRING LIFE_S
call Print_String

call Print_Newline

call Read
dec LIFE
set FLAG VOID

set FS_REG ANSWER_S
LOOP:

sub OLD_CHAR IND_REG VOID
ttsu STATUS_REG ZERO_BIT
jmp d0 d6

or FLAG TWO FLAG
sub FS_REG NINE FS_REG
mov OLD_CHAR IND_REG
add FS_REG NINE FS_REG
inc CORRECT

inc FS_REG
tsz IND_REG
goto LOOP

mov READ_CHAR OLD_CHAR

tsz FLAG
inc LIFE

sub CORRECT FIVE VOID
ttsu STATUS_REG ZERO_BIT
jmp d0 d10
set PRINT_STRING GUESS_S
call Print_String
set PRINT_CHAR d10
call Print
set PRINT_STRING SUCCESS_S
call Print_String
call Print_Newline
call Print_Newline
call Reset

tsz LIFE
jmp d0 d10
set PRINT_STRING FAIL_S_1
call Print_String
mov ANSWER_PICK PRINT_STRING
call Print_String
set PRINT_STRING FAIL_S_2
call Print_String
call Print_Newline
call Print_Newline
call Reset



goto Main


nop
jmp d2 d1

Reset:
set FS_REG GUESS_S
set IND_REG '*'
inc FS_REG
set IND_REG '*'
inc FS_REG
set IND_REG '*'
inc FS_REG
set IND_REG '*'
inc FS_REG
set IND_REG '*'
inc FS_REG
set IND_REG VOID

rnd ANSWER_PICK
mul ANSWER_PICK TWO ANSWER_PICK VOID
div ANSWER_PICK NUM_ANSWERS VOID ANSWER_PICK
set TEMP d6
mul ANSWER_PICK TEMP ANSWER_PICK VOID
set TEMP ANSWER_LIST
add ANSWER_PICK TEMP ANSWER_PICK

set TEMP ANSWER_S
sub TEMP ANSWER_PICK TEMP
mov ANSWER_PICK FS_REG

tsnz IND_REG
jmp d0 d7
mov IND_REG OLD_CHAR
add FS_REG TEMP FS_REG
mov OLD_CHAR IND_REG
sub FS_REG TEMP FS_REG
inc FS_REG
jmp d2 d7

set CORRECT VOID
set LIFE d7
set OLD_CHAR VOID
set READ_CHAR VOID

return

Print_String:
mov FS_REG TEMP
mov PRINT_STRING FS_REG
mov IND_REG PRINT_CHAR
call Print
inc FS_REG
tsz IND_REG
jmp d2 d4
mov TEMP FS_REG
return

Print_Newline:
set PRINT_CHAR d10;
call Print
return

Print:
mov PRINT_CHAR TX_REG
ttsnu TRX_REG TX_BIT
jmp d2 d1
return

Read:
ttsu TRX_REG RX_BIT
jmp d2 d1
mov RX_REG READ_CHAR
return

Init:

set LIFE d7

set FS_REG LIFE_S
set IND_REG ' '
inc FS_REG
set IND_REG 'G'
inc FS_REG
set IND_REG 'u'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG 'L'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'f'
inc FS_REG
set IND_REG 't'
inc FS_REG
set IND_REG d10
inc FS_REG
set IND_REG VOID

set FS_REG SUCCESS_S
set IND_REG 'C'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'n'
inc FS_REG
set IND_REG 'g'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 't'
inc FS_REG
set IND_REG 'u'
inc FS_REG
set IND_REG 'l'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 't'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'n'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG '!'
inc FS_REG
set IND_REG d10
inc FS_REG
set IND_REG VOID

set FS_REG FAIL_S_1
set IND_REG 'S'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'y'
inc FS_REG
set IND_REG ','
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG 't'
inc FS_REG
set IND_REG 'h'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG 'w'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'd'
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG 'w'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG VOID

set FS_REG FAIL_S_2
set IND_REG d10
inc FS_REG
set IND_REG 'T'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'y'
inc FS_REG
set IND_REG ' '
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 'g'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'n'
inc FS_REG
set IND_REG d10
inc FS_REG
set IND_REG VOID

set FS_REG ANSWER_LIST

inc NUM_ANSWERS
set IND_REG 'g'
inc FS_REG
set IND_REG 'u'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'h'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'k'
inc FS_REG
set IND_REG 'u'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'f'
inc FS_REG
set IND_REG 'u'
inc FS_REG
set IND_REG 'z'
inc FS_REG
set IND_REG 'z'
inc FS_REG
set IND_REG 'y'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'f'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'x'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 's'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'g'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'c'
inc FS_REG
set IND_REG 'k'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'f'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'n'
inc FS_REG
set IND_REG 'c'
inc FS_REG
set IND_REG 'h'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'f'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'y'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'j'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'k'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'd'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'k'
inc FS_REG
set IND_REG 'a'
inc FS_REG
set IND_REG 'z'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'j'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'k'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'p'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'n'
inc FS_REG
set IND_REG 'c'
inc FS_REG
set IND_REG 'h'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'b'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 't'
inc FS_REG
set IND_REG 'e'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG VOID
inc FS_REG

inc NUM_ANSWERS
set IND_REG 'b'
inc FS_REG
set IND_REG 'r'
inc FS_REG
set IND_REG 'o'
inc FS_REG
set IND_REG 'i'
inc FS_REG
set IND_REG 'l'
inc FS_REG
set IND_REG VOID
inc FS_REG

return