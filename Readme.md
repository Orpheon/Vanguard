Vanguard is a small open source project by fans of [Gang Garrison 2](http://www.ganggarrison.com/) attempting to create a demake in a similar vein of [Overwatch](https://playoverwatch.com).
We are not affiliated with Blizzard in any way, this is merely a fan project.

Game releases for windows can be found on the [main thread](http://www.ganggarrison.com/forums/index.php?topic=37227.0), people interested in developing should contact us.
Please note that this is still very much in alpha and not meant to be playable in its current state.

For modifying the sprites or adding new ones, one must run `python update_sprites.py` with [Pillow](https://python-pillow.org/) installed. The game does not require recompilation.

For compiling the game, one requires [Allegro](http://liballeg.org/download.html) v5.2 or above, [enet](http://enet.bespin.org/Downloads.html) v1.3.13 or above and [ASIO Standalone](https://think-async.com/Asio). If on linux, libdw-dev is also heavily recommended for stack traces.
Compile the game either with cmake, the python configure.py files that generate ninja scripts, or the codeblocks project file (although the latter two probably require extensive updating).
