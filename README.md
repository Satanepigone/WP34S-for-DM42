# WP34c calculator for DM42 hardware

## A project to bring the WP34c calculator firmware to the DM42 hardware. 

###**Note:** all of the new features below are available in the `build/` folder of this branch of the project. You want either `wp34c.pgm`, `wp34c_long.pgm`, or `wp34c_top.pgm`. These differ only in their displays: the first is most like the wp34c, while `wp34c_top.pgm` adds an extra top row for annunciators. Most people seem to prefer `wp34c_top.pgm`.

####**New!**
No major changes.
The EXITing from printing now works more reliably. 
Printing the stack or a block of registers in complex lock mode now prints in complex form by default, respecting RECT or POLAR display setting.
Update of calculator help file - still a work in progress.

####*Older* 
Printing can now be interrupted by pressing the EXIT key. It's checked for once per line; you don't need to press it repeatedly or to hold it down. The PRT annunciator now flashes once per line as well.

####*Older* 
IR printing is now supported (first version). I have tested this on an HP 82240B printer and all commands seem to be working. I don't see how sending the wrong control codes could damage your printer, but be aware that it's **not** my responsibility if they do!

There are a few differences from the printing commands of the WP34s (of which I have no direct experience.)

  * Print mode 3 (Serial) is not supported, as that would be silly.
  * Printing can be turned on and off, using the [P].ON and [P].OFF commands ('P' is the printer icon) in the MODE catalogue (also available from the PRINT menu - see below). When on, a new annunciator (PRT) appears on the screen. When off, no print commands print anything, although print mode, delay time, etc., can still be changed.
  * Pressing SHIFT 3 (PRINT) brings up a menu with lots of nice printing commands in it. These and the other printing commands are also available from the usual catalogues (see the WP34s manual for details).
  * Due to a bug, the first version of the code printed a blank line every other line. I liked this so much that I added it as an option - [P]DBSP with an argument of 1 turns this on, while an argument of 0 turns it off.
  * The default print delay of 1.8s seems fine on my battery-powered printer.

####**New!** 
The WP34S stopwatch feature is now included. To use it, use the STOPW command in the X.FCN menu (shift shift 3). So far as I can see, all of the stopwatch features described in the manual work on the DM42. 

Note that the stopwatch continues to "run" even if the DM42 is turned off. This isn't a problem: the stopwatch works by recording the ticks from the real-time clock when it is started, and then subtracting this from the current ticks whenever a time has to be displayed. When a time isn't displayed, nothing special is happening, apart from the blinking "=" sign.

####**New!** 
There is now an "Entry RPN" (eRPon and eRPoff in the MODES menu). There is a document describing this in the help/ folder.

####**New!**
The user-defined menu feature has been developed further. There is a document describing its operation in the help/ folder. (Early adopters who called their user menu program MNU will have to rename it, or type the name in by hand. Sorry.)

###**The WP34c calculator...**
...is almost the same as the WP34s, but with some interface changes to make working with complex numbers more straightforward.

<img src="help/screens/wp34c_02.png" width="400">

When complex-lock mode is activated, entering a number displays "Real:" on the status line. Pressing "CPX" then terminates the entry of the real part and displays "Imag:", allowing the entry of the imaginary part. Entry in polar form, with "Length:" and "Angle:" displayed, is also supported.

<img src="help/screens/wp34c_03.png" width="400">

Once entered, (almost) every keyboard operation operates in complex mode, without the need to press "CPX" first. Even STO and RCL store and recall a pair of registers at a time. The aim is to make working with complex numbers as simple as working with real numbers.

<img src="help/screens/wp34c_04.png" width="400">

One disadvantage is that in complex-lock mode there are no programming features. However, by turning complex-lock mode off all of the features of the WP34s become available. On the real calculator the WP34c lacked some of the WP34s features due to limited space for code; this isn't the case on the DM42.

As with the WP34s there are three versions of the WP34c with differing displays - normal, long, and top. The program files are, respectively, build/wp34c.pgm, build/wp34c_long.pgm, and build/wp34c_top.pgm. The installation procedure is the same as for the WP34s .pgm files.

Just as with the WP34s, SwissMicros neither provide nor support this code. All of the code is covered either by the GPL3 licence or by the BSD 3-clause licence, both available in this repository.

You can find documentation for the original WP34c calculator in the help/ folder. There is now also updated documentation that describes how the DM42 version works.

As yet, there is *still* no WP34c-specific help file for the calculator.

Nigel (UK)