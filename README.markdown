Freemon
===========
Freemon is a simple GTK+-based GUI program for working with [Freescale's](http://www.freescale.com/) low-cost [FRDM-KL25Z development board](http://www.freescale.com/webapp/sps/site/prod_summary.jsp?code=FRDM-KL25Z).

Freemon combines a simple serial terminal window with the ability to upload new code to the board.
Written for Linux, Freemon "knows" how to detect connected FRDM-KL25Z boards, saving you from figuring out device names yourself.

I wrote it to use in combination with [Andrew Payne's](https://github.com/payne92) excellent [bare-metal-arm](https://github.com/payne92/bare-metal-arm) project, to make developing for the FRDM-KL25Z board more convenient in Linux.

Instead of figuring out which `/dev/ttyACM?` device to use in order to connect a stand-alone terminal program (which are often optimized for working with a modem) and copying binaries in a shell, Freemon handles both of these needs in an integrated way.
This is totally against the wonderful spirit of Linux and so on, but it's also useful so why not.

Freemon is Open Source and Free Software, licensed under the GPLv3.

#Table of Contents#

1. [Features](#features)
1. [Installation](#install)
1. [Connecting to a Board](#connect)
1. [Usage](#use)

#Features#
<a id="features"></a>
To avoid "disappointed downloader"-syndrome, here's a quick overview of Freemon's features, so you know what you get:

1. Automatic detection of *connected* FRDM-KL25Z boards (Linux only, works by analyzing the output of `dmesg`).
2. Terminal emulator to view output from a board's OpenSDA serial "debug port".
3. Single-button upload of code to the board.
4. Tabbed interface supports connecting to any number of boards from the same host computer.

And, perhaps just as importantly, here's what it is *not*:

1. It's not an IDE.
2. It's not a compiler.
2. It's not a debugger.


#Installation#
<a id="install"></a>
At the moment, Freemon is not available in any pre-packaged form, so you need to build it yourself.
It requires the [VteTerminal](https://developer.gnome.org/vte/unstable/VteTerminal.html) library, so you're going to need that first.
In Ubuntu, you can install it by running `sudo apt-get install libvte-2.90-dev` in a shell.
Make sure you get the GTK+ 3.x version of VteTerminal, since Freemon uses GTK+ 3.x.

Then do the following to build Freemon:

1. Grab the code (`git clone git://github.com/unwind/freemon.git`)
2. Using a terminal, enter the top-level `freemon/` directory
3. Type `make`
4. There is no `install` target, so just run it from the source directory by typing `./src/freemon`.


#Connecting to a Board#
<a id="connect"></a>
When you start Freemon, it will open its main window.
The window looks like this initially:

![Initial Main Window](https://github.com/unwind/freemon/raw/master/doc/screenshots/mainwindow-initial.png "Initial Main Window")

Assuming you have at least one FRDM-KL25Z board connected to your computer, click the Refresh button to run Freemon's auto-detection code.

If some boards were found, they will be listed in the menu accessible by clicking the downward arrow next to the Connect button.

![Targets Menu](https://github.com/unwind/freemon/raw/master/doc/screenshots/mainwindow-targets.png "Targets Menu")

You can pick a board in the menu to connect to, or click the Connect button to open a connection to the first unconnected target board.

Each connected target opens a new tab, containing a terminal window and some supporting widgets:

![Connected Target](https://github.com/unwind/freemon/raw/master/doc/screenshots/mainwindow-connected.png "Connected Target")


#Usage#
<a id="use"></a>
The big terminal area accepts keypresses and sends each character read from your keyboard to the board's serial port.
Characters received from the board are also displayed.
There is no "local echo" option, so if the software running on the FRDM-KL25Z isn't implementing echo, you won't see what you type.

You can right-click in the terminal to access a minimalistic menu with a Reset command, handy to clear the window.


##Uploading Code##
To upload new software to the board, first pick the binary you want to run by clicking the filechooser button labelled "Binary".

Clicking the button opens up a standard GTK+ filechooser window, which will help you locate the file to upload.
The chooser is filtered to only show files in the formats the FRDM-KL25Z's [OpenSDA](http://www.pemicro.com/opensda/index.cfm) implementation supports: `*.bin`, `*.s19` and `*.srec` are supported.

Once a file has been picked, it's size in KB is shown to the right of the filechooser button.

Click the Upload button to copy the selected file to the FRDM-KL25Z board, which will automatically reset and boot into the new software.


#Feedback#
Please contact the author, Emil Brink (by e-mailing &lt;emil@obsession.se&gt;) regarding any bugs, comments, or thoughts about Gitbrowser.
Enjoy.
