# [![Logo]][Home] Communi [![Status]][CI]

A simple and elegant cross-platform [IRC] client for desktop platforms, based on [Qt] and the Communi [IRC framework].

![Screenshot](https://raw.githubusercontent.com/wiki/communi/communi-desktop/images/cute.png)

More screenshots: [Gallery](https://github.com/communi/communi-desktop/wiki/Gallery)

## Installation

Prerequisites: Git, Qt 5

#### Communi IRC framework

    git clone git://github.com/communi/libcommuni
    cd libcommuni
    qmake
    make
    sudo make install

#### Communi desktop client

    git clone git://github.com/communi/communi-desktop
    cd communi-desktop
    git submodule update --init
    qmake
    make
    sudo make install

## License

Communi is free software; you can redistribute and/or modify it under the terms of the [BSD](http://opensource.org/licenses/BSD-3-Clause) license.

## Contact

- Website: [github.com/communi/communi-desktop][Home]
- Issue tracker: [github.com/communi/communi-desktop/issues][Issues]
- IRC channel: `#communi` on irc.freenode.net

[Home]:          http://github.com/communi/communi-desktop
[Issues]:        http://github.com/communi/communi-desktop/issues
[Logo]:          https://raw.githubusercontent.com/communi/libcommuni/master/doc/communi.png
[Status]:        https://travis-ci.org/communi/communi-desktop.svg?branch=master
[CI]:            https://travis-ci.org/communi/communi-desktop
[Qt]:            http://www.qt.io
[IRC]:           http://en.wikipedia.org/wiki/Internet_Relay_Chat
[IRC framework]: http://communi.github.io
