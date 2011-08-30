
Experimental tool for translating japanese and/or chinese text to english/finnish.

Author: Ilkka Prusi
Contact ilkka.prusi@gmail.com

Summary:
Experimenting with Qt, Berkeley DB and ways to translate given text 
(in hiragana, katakana and kanji) to english/finnish etc.

Description:
In progress..

Mostly just for personal use to learn japanese language and simple translation helper.
Translation is just rudimentary dictionary/phrase lookup and won't even attempt grammar/syntax handling.

Some quick hacking to use dictionary files for help, namely EDICT (http://www.csse.monash.edu.au/~jwb/edict.html),
possibly others later also (needs improvements in db-layout first).


Licensing (open):
BSD-style ? GPL v3 ? (to be determined..)
..not yet sure 

Usage:
On first use, initialize translation database (use EDICT-dictionary, for example).
Copy to clipboard is captured and handled in program (no "selection" support in Windows to skip copy also..)

If all goes well some kind of rough translation should be shown..

