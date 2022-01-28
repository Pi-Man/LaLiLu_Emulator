#INFO

This project is directly inspired by Mili's songs [Ga1ahad and Scientific Witchery]<https://www.youtube.com/watch?v=d-nxW9qBtxQ> and [Sl0t]<https://www.youtube.com/watch?v=WpE98Jn6dAY>
 (other possibly related songs include RTRT, world.execute(me);, and Witch's Invitation, however theses songs are not directly related to this project)

It consists of two programs, an assembler and an emulator.  The assembler will read an assembler file(s) passed by command line arguments.  
It will then output a text file comprised of the "words" `ta`, `tu`, `la`, `li`, and `lu`.
* `ta` indicates the beginning of an instruction id and does not hold a value itself.
* `tu` indicates the beginning of an argument and does not hold a value itself.
* `la` is a trit (ternary digit) with value 0.
* `li` is a trit with value 1.
* `lu` is a trit with value 2.

#Assembler

The assembler currently only reads from the first file given as an argument and outputs to the file listed after the -o flag (defaulting to out.scwt).  
It uses a custom instruction set that is being developed alongside the Assembler and Emulator.

Currently the Assembler can only read instructions and ignore comments.  Any other text will try to be read as an instruction and will generate an error (skipping the line entirely and continuing with the rest of the program)

##instruction set

all "source" and "destination" arguments are memory addresses

"value" arguments are value literals

"none" indicates that the instruction does not take any arguments

|name|id|arguments|description|
|clr|1|destination|sets the destination value to 0
|set|2|destination value|sets the destination value to "value"
|mov|3|source destination|sets the destination value to the source value
|rnd|4|destination|sets the destination value to a random number
|add|9|source source destination|adds the first source to the second source and stores result in destination
|sub|10|source source destination|subtracts the second source from the first source and stores result in destination
|mul|11|source source destination|multiplies the first source with the second source and stores result in description
|div|12|source source destination(quotient) destination(remainder)|divides the first source by the second source and stores the quotient in the first destination and the remainder in the second destination
|dec|18|source|decrements the source value
|decsz|19|source|decrements the source value and skips the next instruction if the result is zero
|inc|20|source|increments the source value
|incsz|21|source|increments the source value and skips the next instruction if the result is zero
|tsz|27|source|tests the source value and skips the next instruction if the value is zero
|tsnz|28|source|tests the source value and skips the next instruction if the value is not zero
|and|36|source source destination|performs trit wise and operation on the two source values and stores result in destination
|or|37|source source destination|performs trit wise inclusive or operation on the two source values and stores result in destination
|xor|38|source source destination|performs trit wise exclusive or operation on the two source values and stores result in destination
|cmp|39|source destination|performs the compliment of the source value and stores result in destination
|rl|40|source source destination|rotates the first source value to the left the number of times specified by the second value and stores the result in destination
|rlc|41|source source destination|rotates the first source value to the left the number of times specified by the second value using the carry trit as an extra trit in the source and stores the result in destination
|rr|42|source source destination|rotates the first source value to the right the number of times specified by the second value and stores the result in destination
|rrc|43|source source destination|rotates the first source value to the right the number of times specified by the second value using the carry trit as an extra trit in the source and stores the result in destination
|tc|45|source value|clears the trit in the source value indicated by the given value
|ts|46|source value value|sets the trit in the source value indicated by the first value to the second value
|ttsa|47|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is zero
|ttsna|48|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is not zero
|ttsi|49|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is one
|ttsni|50|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is not one
|ttsu|51|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is two
|ttsnu|52|source value|test the trit in the source value indicated by the given value and skips the next instruction if the value is not two
|call|54|value|pushes the current program counter to the call stack and sets the program counter so the next instruction executed is at the location indicated by the given value
|return|55|none|pops a value from the call stack and sets the program counter so the next instruction executed is at the location indicated by the value
|goto|56|value|sets the program counter so the next instruction executed is at the location indicated by the given value
|jmp|57|value value|sets the program counter so the next instruction executed is at the location indicated by the second value relative to the current program counter.  If the first value is 0, the second value is taken to be positive, if the first value is non-zero the second value is taken to be negative
|nop|58|none|is loaded as an instruction but does not perform any operations