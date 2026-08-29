# Godot-AudioStreamGME
An AudioStream GDExtension for Godot that plays video game music files

This extension provides a way to play video game files in godot, using the [libgme](https://github.com/libgme/game-music-emu) library.

# Building AudioStreamGME
Get [godot-cpp](https://github.com/godotengine/godot-cpp), and build it using the instructions of their github page

Get libgme, and build it using the instruction on their github page

Build this project by using "scons" in the "AudioStreamGME" folder.

An example Godot project is included in AudioStreamGME/project

# known issues

unpause doesn't seem to work

# untested features

AudioStreamGME.load_data hasn't been tested yet, because I have yet to figure out how to load video game music files as a resource in godot.




I am not a professional developer by any means. This project is provided "as is". It's mostly an exercise for me to see if I could do it.
If you want to fork this and improve it, go right ahead. I do ask that you give everyone in the legal section credit for their contributions.

Legal
-----
AudioStreamGME copyright (C) 2026 Peter Meijer
Game_Music_Emu library copyright (C) 2003-2009 Shay Green.
Sega Genesis YM2612 emulator copyright (C) 2002 Stephane Dallongeville.
MAME YM2612 emulator copyright (C) 2003 Jarek Burczynski, Tatsuyuki Satoh
Nuked OPN2 emulator copyright (C) 2017 Alexey Khokholov (Nuke.YKT)

