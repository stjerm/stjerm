#!/bin/sh

# This was borrowed from Tilda (see tilda.sourceforge.net).
# Here is the message from the Tilda autogen.sh:
# This was borrowed from Gaim (see gaim.sf.net) and modified
# for our purposes. Thanks guys!

echo "This will do all the autotools stuff so that you can build"
echo "stjerm successfully."
echo
echo "When it is finished, take the usual steps to install:"
echo "./configure"
echo "make"
echo "make install"
echo

(automake --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have automake installed to compile stjerm";
	echo;
	exit;
}

(autoconf --version) < /dev/null > /dev/null 2>&1 || {
	echo;
	echo "You must have autoconf installed to compile stjerm";
	echo;
	exit;
}

echo "Generating configuration files for stjerm, please wait..."
echo;

aclocal $ACLOCAL_FLAGS || exit;
autoheader || exit;
automake --add-missing --copy;
autoconf || exit;
automake || exit;

