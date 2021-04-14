This branch contains the code and compiled firmware for the WP-34s with complex lock mode.

The .bin firmware files are in realbuild.
At present the files with added library routines do not all fit into the calculator memory; this may change later.

The makeflash.cmd batch file in branches/complex_mode can be used to build the firmware files.

To save space the files are currently compiled with the following options in features.h:

* Pixel plotting commands disabled
* No "indirect constants"
* Bit's "Universal Dispatch" code now disabled as of 3903; it didn't work with the inverse normal xrom function
* Gudermannian functions disabled
* Matrix row-ops disabled
* Matrix LU decomposition disabled

At present the number of program steps that will fit into free flash memory is:
As of 3905:
Complex Lock plus above compile options:
* calc.bin - 6656 steps
* calc_xtal.bin - 5120 steps
* calc_ir.bin - 1536 steps
This, with ENTRY_RPN:
* calc.bin - 6656 steps
* calc_xtal.bin - 5120 steps
* calc_ir.bin - 1536 steps

Changes:
Release 3905:
* Bug fixed with the operation of entry_rpn. Pressing return with no number in the command line leaves
stack lift enabled, so that (for example) 0 Fill 2 ENTER ENTER 50 x x returns 200, not zero.
* Instructions included on how to set the calculator to be invoked when you press the Windows calculator
key - see file calckey_readme.txt in branches\complex_mode\windows\bin for instructions (also present in
trunk\windows\bin).
* Options added to features.h:
	* YREG_ON_BY_DEFAULT does what it says - a less extreme version of YREG_ALWAYS_ON.
	* DEFAULT_TO_SSIZE8 also does what it says.
	* DEFINE_DATE_YMD and DEFINE_DATE_DMY choose the initial date mode - there are issues with the 
	existing code for setting defaults.
	* REVERSE_ARROW makes angular conversions easier. Pressing the conversion arrow twice before
	a conversion reverses its direction, converting _into_ the current angular mode.
* Different features.h files added to the distribution:
	* features_standard.h: complex mode, no entry RPN.
	* features_standard_entryRPN.h: complex mode, entry RPN
	* features_nd.h: nd's working features file (will disappear from repository eventually)
	* features_JM.h: JM's working features file (will disappear from repository eventually)
To use these, copy them to features.h and compile in the usual way. features.h is initially a copy
of features_standard.h .

Release 3904:
* Bug fixed with how complex_enter handles stack lift - this caused entry_rpn to fail to work correctly.
* Matrix operations disabled to increase available flash memory for programs
* Indirect constants function disabled for the same reason

Release 3903:
* Have stopped using Universal Dispatch - now commented out in features.h. This was causing an error with the inverse normal distribution (and 
presumably other functions as well) on the real calculator, although not on the emulator. This error has been present in every previous version of 
Complex Lock Mode.
* Entry RPN has been added as an option in features.h (thanks to Jaco@cocoon-creations.com for the code for this). Versions of the emulator and the 
calculator firmware have been included with this feature turned on so that users can try it out.
* New emulator skins (also provided by Jaco@cocoon-creations.com) have been added to both the main branch and Complex Lock Mode.
* Those changes over the past year that didn't get applied to the complex lock branch have now been applied - so far as I am aware, everything is 
up-to-date.
* In features.h there is now the option to set various complex lock mode options as defaults - j rather than i, CPXYES automatically enabled, 
complex lock mode on by default.
* The VERS command now returns "34C..." rather than "34S..." for the complex lock firmware.

Release 3902:
* This didn't work. Don't use it!

Otherwise:
* Files are renamed so that this emulator does not clash with the standard emulator.
* Specifically, wp34s.op, wp34s.dat, wp34s-lib.dat, and wp34s-backup.dat all have "s" changed to "c" in their names.
* These files are referenced in: storage.c, compile_xrom.cmd, build_cats.cmd, Makefile, makelib.cmd, wp34s_asm.pl, and as a header file for the xrom project.
* All of these references have been changed. 
* flash-build.log now goes to trunk, not branches


