set d150 d6
set d151 d728

set d201 d0 ; 1 unique
set d202 d3 ; 2 unique
set d212 d4 ; 2 unique
set d203 d5 ; 3 unique
set d204 d6 ; 4 unique

rnd d100
rnd d101
rnd d102
rnd d103

div d100 d150 d98 d100
div d101 d150 d98 d101
div d102 d150 d98 d102
div d103 d150 d98 d103

clr d105

sub d100 d101 d104
tsz d104
inc d105

sub d100 d102 d104
tsz d104
inc d105

sub d100 d103 d104
tsz d104
inc d105

sub d101 d102 d104
tsz d104
inc d105

sub d101 d103 d104
tsz d104
inc d105

sub d102 d103 d104
tsz d104
inc d105


sub d105 d201 d104
tsnz d104
inc d221

sub d105 d202 d104
tsnz d104
inc d222

sub d105 d212 d104
tsnz d104
inc d222

sub d105 d203 d104
tsnz d104
inc d223

sub d105 d204 d104
tsnz d104
inc d224

decsz d151
jmp d7

mov d221 t222222
mov d222 t222222
mov d223 t222222
mov d224 t222222
