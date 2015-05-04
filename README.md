# stjerm Terminal Emulator

## This project is in need of a steward. If you are interested in helping maintain stjerm, please reach out.

## Requirements

pkgconfig, gtk2, vte

### Ubuntu Requirements

If you're compiling from source on Ubuntu, you'll need:

    sudo apt-get install build-essential automake libglib2.0-dev libvte-dev

## Installation

    ./autogen.sh
    ./configure
    make
    sudo make install


## Running

Simply run:

    stjerm

and all available options will be listed.
