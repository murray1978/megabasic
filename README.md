# MegaBasic

Mega Basic brings the BASIC programming language to an arduino mega.
The Basic Interpreter was cloned from Jerry Williams JR https://github.com/jwillia3/BASIC
which requires more memory than the Mega can supply.
Extra memory is provided by a 24LC256 or larger serial EEPROM  and a custom memory manager has been written to use

## Todo

	- Test, Test and more testing.
	- Currently the memory manager can store and retrive an integer or char from the eeprom.
	- `delete, delete[]`
	- `new, new[]`
	- `float, long, unsigned`
	
## Issues

 Currently the address of each variable will be stored in ram, the only benefit would be
 with arrays of floats, integer's and char's.