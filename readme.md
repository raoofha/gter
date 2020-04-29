# gter : embed a GUI app inside a terminal window or any other window

tested only with xterm, I also use i3 window manager.
this is much better solution than the previous approach.

![gter](gter.gif)

# requirement
* X11
* xterm
* bash

# compile and run
```
git clone https://github.com/raoofha/gter
cd gter
bash $PWD/src/setParentWindow.c build
# add it to your path
ln -s $PWD/src/gter ~/.local/bin/gter
chmod +x ~/.local/bin/gter
```

# set window parent on other windows
```
WINDOWID=$WINDOWID WINDOWX=0 WINDOWY=0 LD_PRELOAD=./bin/setParentWindow.so xclock
```
