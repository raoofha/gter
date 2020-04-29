/*
if [ "$1" == "xterm-send" ]; then xterm-send bash $0 $2 ; exit 1 ; fi
echo -ne "\033c"

sdir=`dirname $0`
odir=${sdir}/../bin
ofilename=`basename $0 .${0##*.}`
ofilepath=${odir}/${ofilename}

mkdir -p $odir
cd $odir

build="gcc -fPIC -shared $0 -o $ofilepath.so -ldl"

cmd=$1
echo $cmd...
if [ "$cmd" == "build" ]; then
time $build &&
LD_PRELOAD=$ofilepath.so xclock
fi
exit 1
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <dlfcn.h>

Window (*XCreateWindow_original)(Display*, Window, int, int, unsigned int, unsigned int, unsigned int, int, unsigned int, Visual*, unsigned long, XSetWindowAttributes*);
Window XCreateWindow(Display* display, Window parent, int x, int y, unsigned int width, unsigned int height, unsigned int borderWidth, int depth, unsigned int _class, Visual* visual, unsigned long valuemask, XSetWindowAttributes* attribs) {
  if (!XCreateWindow_original) XCreateWindow_original = dlsym(RTLD_NEXT, "XCreateWindow");
  Window root = RootWindow(display, DefaultScreen(display));
  Window w, pwin = 0;
  int childWindowWidth  = width;
  int childWindowHeight = height;
  char* pwinstr = getenv("WINDOWID");
  if (pwinstr) pwin = atoi(pwinstr);

  if ( parent == root && pwin ) {
    char* gterWindowXStr = getenv("WINDOWX");
    char* gterWindowYStr = getenv("WINDOWY");
    char* gterWindowWidthStr = getenv("WINDOWWIDTH");
    char* gterWindowHeightStr = getenv("WINDOWHEIGHT");
    char* rowsStr = getenv("ROWS");
    char* colsStr = getenv("COLS");
    char* rowStr = getenv("ROW");
    char* colStr = getenv("COL");
    int gterWindowX = 0;
    int gterWindowY = 0;
    int gterWindowWidth = width;
    int gterWindowHeight = height;
    int rows = 1;
    int cols = 1;
    int row = 0;
    int col = 0;
    if (gterWindowXStr) gterWindowX = atoi(gterWindowXStr);
    if (gterWindowYStr) gterWindowY = atoi(gterWindowYStr);
    if (gterWindowWidthStr) gterWindowWidth = atoi(gterWindowWidthStr);
    if (gterWindowHeightStr) gterWindowHeight = atoi(gterWindowHeightStr);
    if (rowsStr) rows = atoi(rowsStr);
    if (colsStr) cols = atoi(colsStr);
    if (rowStr) row = atoi(rowStr);
    if (colStr) col = atoi(colStr);

    Window proot;
    int px, py;
    unsigned int pw, ph, pborder, pdepth;
    XGetGeometry(display, pwin, &proot, &px, &py, &pw, &ph, &pborder, &pdepth);
    if ( gterWindowWidthStr && !strcmp(gterWindowWidthStr, "fit") ) {
      childWindowWidth  =  width > pw ? pw : width ;
    } else {
      childWindowWidth  = gterWindowWidth;
    }
    if ( gterWindowHeightStr && !strcmp(gterWindowHeightStr, "fit") ) {
      childWindowHeight = height > ph ? ph : height;
    } else {
      childWindowHeight = gterWindowHeight; 
    }

    w = XCreateWindow_original(display, parent, x, y, width, height, borderWidth, depth, _class, visual, valuemask, attribs);
    //XMoveResizeWindow(display, w, x, y, childWindowWidth, childWindowHeight);
    if ( gterWindowXStr || gterWindowYStr ) {
      XReparentWindow(display, w, pwin, gterWindowX, gterWindowY);
    } else {
      if ( row > rows ) row = 0;
      if ( col > cols ) col = 0;
      float rowHeight = ph / rows;
      float colWidth = pw / cols;
      //for(int i = 0 ; i < childWindowHeight / rowHeight; i += 1) putchar('\n');
      //printf("%d %d %d %d %d %d %f %f %d %d %d\n", rows, cols, row, col, pw, ph, rowHeight, colWidth, w, childWindowWidth, childWindowHeight);
      XReparentWindow(display, w, pwin, 0, rowHeight * (row));
    }
  } else {
    w = XCreateWindow_original(display, parent, x, y, width, height, borderWidth, depth, _class, visual, valuemask, attribs);
  }
  return w;
}
