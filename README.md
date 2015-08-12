
# FABL-ng - FAlling BLocks Next Generation

Origin: https://bitbucket.org/erikedlund/fabl-ng

Mirror: https://github.com/edlund/fabl-ng

Play a game where controlling falling blocks is the main game mechanic.
It's a bit reminiscent of a certain famous puzzle game beginning with
"T", but adds many new features, shapes and an extra dimension to offer
more complicated and challenging gameplay. It should be easy to learn
and a good way to spend a mindless hour every now and again.

Unlike most similar 3d puzzle games which requires that a layer is filled
before it's removed, fabl ng works by matching lines, where a full line
is removed, this is true both for width and depth. The idea is that this
mechanic should speed up the gameplay and make it more challenging;
careless matching of lines leaves a lot of junk which is difficult
to get rid of.

fabl ng also features multiple shapes falling simultaneously, with the
shape spawn time decreasing as the level is increasing, making high
levels much harder (this feature can be turned off, which is good for
casual sessions).

To balance the added difficulty of multiple falling shapes and faster
accumulating junk "powers" have been added. Powers charge up as the game
progresses and allows the player to get extra skill points and to get rid
of hard-to-clear junk. 

## History

I got the idea for this game in the summer of 2014 when watching the
AVGN VirtualBoy episode. Quoting the part about VirtualBoy Tetris 3d:

        "Anyone who's familiar with Tetris will immediately understand
		the concept. Blocks fall down and you have to put them together
		without leaving gaps. Anytime you fill a whole row they disappear
		and the only difference is that it is in 3d. But that means it
		moves along a lot slower because there's a lot more space to fill."

		Angry Video Game Nerd - Episode 42 https://youtu.be/OyVAp0tOk5A

After looking around a little at similar games, such as Blockout and
various freeware clones it seemed to me that main problem with the slow
and somewhat boring gameplay was that you have to fill an entire layer
to get rid of it.

By borrowing the idea from normal Tetris that a full line should be
removed and applying that rule to both width and depth I was certain that
the gameplay would be more stressful, fun and chaotic.

Fast forward a year to this summer when I decided to try to implement the
idea using SDL and OpenGL. Someone smarter than me could probably have done
a better job faster, but after about 2.5 months of work I had a working build
that was nearly feature complete in regards to gameplay mechanics.

With most of the summer break gone and a new semester approaching quickly
I made and attempt to Greenlight the game (which failed miserably) in order
to see if I could get any form of compensation for time spent and motivation
to get to work on the feature needed in order to defend a commercial release
(such as a tutorial, better options menu, more accessibility features,
general refinement/polish and so on). The main reason for the overall
negative response seems to have been due to the graphics and to some of
the aesthetic choices made by me (I can hardly blame people for it) and not
so much the overall idea (although some commentors failed to see how fabl
differentiates itself from games like VB Tetris 3d and Blockout).

As such I decided to clean up the project as it were and release it as
open source instead. It seems like a bad idea to just let it bitrot
in my backup archives over time. The game should be playable, so why
not let those that liked the idea try it out?

## Future

I still believe in the idea, so I'll probably keep working on fabl when I
have time to spare and feel like revisiting it. I might also use it in
order to learn "proper" OpenGL (2.0+) as it would be rather nice to use
a fabl upgrade project as a concrete goal for the exercise.

I would also like to add support for online highscore lists sometime
in the future, where anyone can host a highscore list and configure their
game to use any list they'd like. This wasn't really an option if fabl
was to be released commercially as hosting cost, service availability
and cheat prevention would've been too big issues to fix for me alone.
But when warranty is no longer a critical issue that feature could be
provided as-is without it causing any major problems.

## Trailer/Screenshots

View the trailer here: https://www.youtube.com/watch?v=b_wd0qmW7AM

Screenshots can be found in `resources/screenshot`.

## How to play

The controls you need to know are the following:

*Blocks*

 * Move blocks: `W` `A` `S` `D`
 * Rotate blocks: `Q` `E`
 * Change rotation mode: `R`
 * Drop block: `space`
 * Drop block one Y-unit: `lctrl`
 * Lift block one Y-unit: `lshift`
 * Next falling block: `PageUp`
 * Prev falling block: `PageDown`

*Camera*

  * Tilt and rotate: `up` `down` `left` `right`
  * Zoom camera in/out: `home` `end`
  * Move camera up/down: `insert` `delete`

*Powers*

 * KillShape: `1` (removed the currently active shape)
 * ColorClear: `2` (clear all blocks of the same color as the active shape)
 * VaporizeFoundation: `3` (remove all bottom blocks)
 * SpikeTrap: `4` (push "spikes" through the bottom layers)
 * GravityBeam: `5` (drop all blocks as far as they can fall, step-by-step)

*Misc*

 * Pause: `esc` (pausing will reset your power charge!)

### Windows

To play the game, double-click `play-release.bat` in the project root. If
everything works out, the game should start.

### Linux

It's assumed that Linux users can build the game themselves, it shouldn't be
especially hard. Make sure that the following packages are installed:

 * GL
 * GLU
 * Glew
 * SDL2
 * SDL2_mixer
 * FreeType2
 * catch
 * pthread
 * scons

Once the build environment is up and running it should be sufficient to type:

        $ make

in the project root to build fabl. Once the build is done, start the game by
running:

        $ ./run.sh -b Debug

Good luck!

## License

FABL-ng - FAlling BLocks Next Generation
Copyright 2015 (C) Erik Edlund <erik.edlund@32767.se>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

--

## incompetech.com Music

### "Ouroboros"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Dance Monster"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Electrodoodle"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Latin Industries"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Rocket"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Level Up"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Reformat"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Rhinoceros"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Severe Tire Damage"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

### "Pinball Spring"

Kevin MacLeod (incompetech.com)

Licensed under Creative Commons: By Attribution 3.0

http://creativecommons.org/licenses/by/3.0/

--

## littlerobotsoundfactory.com sound effects

### "8-Bit Sound Effects Library"

Little Robot Sound Factory (littlerobotsoundfactory.com)

Licensed under Creative Commons: By Attribution 3.0

https://creativecommons.org/licenses/by/3.0/

--

