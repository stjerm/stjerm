#!/bin/sh

if  [ ! -e bin ]; then
	mkdir bin
fi

mcs -checked+ -optimize+ -pkg:glib-sharp-2.0 -pkg:gtk-sharp-2.0 -pkg:glade-sharp-2.0 -pkg:vte-sharp-2.0 -out:bin/stjerm.exe src/* || exit 1

if [ ! -e ./install ]; then
	mkdir "./install"
	chmod 755 "./install"
fi

if [ ! -e ./install/stjerm ]; then
	mkdir "./install/stjerm"
	chmod 755 "./install/stjerm"
fi

if [ ! -e ./install/stjerm/usr ]; then
	mkdir "./install/stjerm/usr"
	chmod 755 "./install/stjerm/usr"
fi

if [ ! -e ./install/stjerm/usr/share ]; then
	mkdir "./install/stjerm/usr/share"
	chmod 755 "./install/stjerm/usr/share"
fi

if [ ! -e ./install/stjerm/usr/share/stjerm ]; then
	mkdir "./install/stjerm/usr/share/stjerm"
	chmod 755 "./install/stjerm/usr/share/stjerm"
fi

if [ ! -e ./install/stjerm/usr/bin ]; then
	mkdir "./install/stjerm/usr/bin"
	chmod 755 "./install/stjerm/usr/bin"
fi

cp ./bin/stjerm.exe "./install/stjerm/usr/share/stjerm"
chmod 644 "./install/stjerm/usr/share/stjerm/stjerm.exe"

cp ./README "./install/stjerm/usr/share/stjerm/"
chmod 644 "./install/stjerm/usr/share/stjerm/README"

cp ./data/stjerm.glade "./install/stjerm/usr/share/stjerm/"
chmod 644 "./install/stjerm/usr/share/stjerm/stjerm.glade"

dir=`dirname $0`

echo -e '#!/bin/sh\nbsname=`basename $0`\ndir=`dirname $0`\nexec mono $dir/../share/stjerm/stjerm.exe' > './install/stjerm/usr/bin/stjerm'
chmod 755 "./install/stjerm/usr/bin/stjerm"
