/*
echo -ne "\033c"
ofilename=`basename $0 .${0##*.}`

g++ $0 -o $ofilename -Wall -g -std=c++1y -lX11 &&
./gter.sh

exit 1
*/
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Copyright (C) 2013-2017 Chuan Ji <ji@chu4n.com>                          *
 *                                                                           *
 *  Licensed under the Apache License, Version 2.0 (the "License");          *
 *  you may not use this file except in compliance with the License.         *
 *  You may obtain a copy of the License at                                  *
 *                                                                           *
 *   http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                           *
 *  Unless required by applicable law or agreed to in writing, software      *
 *  distributed under the License is distributed on an "AS IS" BASIS,        *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 *  See the License for the specific language governing permissions and      *
 *  limitations under the License.                                           *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

extern "C" {
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <assert.h>
#include <string.h>
}
#include <map>

// Represents a 2D size.
template <typename T>
struct Size {
  T width, height;

  Size() = default;
  Size(T w, T h)
      : width(w)
      , height(h)
  {
  }

  ::std::string ToString() const;
};

// Outputs a Size<T> as a string to a std::ostream.
template <typename T>
::std::ostream& operator<<(::std::ostream& out, const Size<T>& size);

// Represents a 2D position.
template <typename T>
struct Position {
  T x, y;

  Position() = default;
  Position(T _x, T _y)
      : x(_x)
      , y(_y)
  {
  }

  ::std::string ToString() const;
};

// Represents a 2D vector.
template <typename T>
struct Vector2D {
  T x, y;

  Vector2D() = default;
  Vector2D(T _x, T _y)
      : x(_x)
      , y(_y)
  {
  }

  ::std::string ToString() const;
};

// Outputs a Size<T> as a string to a std::ostream.
template <typename T>
::std::ostream& operator<<(::std::ostream& out, const Position<T>& pos);

// Position operators.
template <typename T>
Vector2D<T> operator-(const Position<T>& a, const Position<T>& b);
template <typename T>
Position<T> operator+(const Position<T>& a, const Vector2D<T>& v);
template <typename T>
Position<T> operator+(const Vector2D<T>& v, const Position<T>& a);
template <typename T>
Position<T> operator-(const Position<T>& a, const Vector2D<T>& v);

// Size operators.
template <typename T>
Vector2D<T> operator-(const Size<T>& a, const Size<T>& b);
template <typename T>
Size<T> operator+(const Size<T>& a, const Vector2D<T>& v);
template <typename T>
Size<T> operator+(const Vector2D<T>& v, const Size<T>& a);
template <typename T>
Size<T> operator-(const Size<T>& a, const Vector2D<T>& v);

// Joins a container of elements into a single string, with elements separated
// by a delimiter. Any element can be used as long as an operator << on ostream
// is defined.
template <typename Container>
::std::string Join(const Container& container, const ::std::string& delimiter);

// Joins a container of elements into a single string, with elements separated
// by a delimiter. The elements are converted to string using a converter
// function.
template <typename Container, typename Converter>
::std::string Join(
    const Container& container,
    const ::std::string& delimiter,
    Converter converter);

// Returns a string representation of a built-in type that we already have
// ostream support for.
template <typename T>
::std::string ToString(const T& x);

// Returns a string describing an X event for debugging purposes.
extern ::std::string ToString(const XEvent& e);

// Returns a string describing an X window configuration value mask.
extern ::std::string XConfigureWindowValueMaskToString(unsigned long value_mask);

// Returns the name of an X request code.
extern ::std::string XRequestCodeToString(unsigned char request_code);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                               IMPLEMENTATION                              *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <algorithm>
#include <sstream>
#include <vector>

template <typename T>
::std::string Size<T>::ToString() const
{
  ::std::ostringstream out;
  out << width << 'x' << height;
  return out.str();
}

template <typename T>
::std::ostream& operator<<(::std::ostream& out, const Size<T>& size)
{
  return out << size.ToString();
}

template <typename T>
::std::string Position<T>::ToString() const
{
  ::std::ostringstream out;
  out << "(" << x << ", " << y << ")";
  return out.str();
}

template <typename T>
::std::ostream& operator<<(::std::ostream& out, const Position<T>& size)
{
  return out << size.ToString();
}

template <typename T>
::std::string Vector2D<T>::ToString() const
{
  ::std::ostringstream out;
  out << "(" << x << ", " << y << ")";
  return out.str();
}

template <typename T>
::std::ostream& operator<<(::std::ostream& out, const Vector2D<T>& size)
{
  return out << size.ToString();
}

template <typename T>
Vector2D<T> operator-(const Position<T>& a, const Position<T>& b)
{
  return Vector2D<T>(a.x - b.x, a.y - b.y);
}

template <typename T>
Position<T> operator+(const Position<T>& a, const Vector2D<T>& v)
{
  return Position<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Position<T> operator+(const Vector2D<T>& v, const Position<T>& a)
{
  return Position<T>(a.x + v.x, a.y + v.y);
}

template <typename T>
Position<T> operator-(const Position<T>& a, const Vector2D<T>& v)
{
  return Position<T>(a.x - v.x, a.y - v.y);
}

template <typename T>
Vector2D<T> operator-(const Size<T>& a, const Size<T>& b)
{
  return Vector2D<T>(a.width - b.width, a.height - b.height);
}

template <typename T>
Size<T> operator+(const Size<T>& a, const Vector2D<T>& v)
{
  return Size<T>(a.width + v.x, a.height + v.y);
}

template <typename T>
Size<T> operator+(const Vector2D<T>& v, const Size<T>& a)
{
  return Size<T>(a.width + v.x, a.height + v.y);
}

template <typename T>
Size<T> operator-(const Size<T>& a, const Vector2D<T>& v)
{
  return Size<T>(a.width - v.x, a.height - v.y);
}

template <typename Container>
::std::string Join(const Container& container, const ::std::string& delimiter)
{
  ::std::ostringstream out;
  for (auto i = container.cbegin(); i != container.cend(); ++i) {
    if (i != container.cbegin()) {
      out << delimiter;
    }
    out << *i;
  }
  return out.str();
}

template <typename Container, typename Converter>
::std::string Join(
    const Container& container,
    const ::std::string& delimiter,
    Converter converter)
{
  ::std::vector<::std::string> converted_container(container.size());
  ::std::transform(
      container.cbegin(),
      container.cend(),
      converted_container.begin(),
      converter);
  return Join(converted_container, delimiter);
}

template <typename T>
::std::string ToString(const T& x)
{
  ::std::ostringstream out;
  out << x;
  return out.str();
}

class WindowManager {
  public:
  // Creates a WindowManager instance for the X display/screen specified by the
  // argument string, or if unspecified, the DISPLAY environment variable. On
  // failure, returns nullptr.
  static WindowManager* Create(
      const std::string& display_str = std::string());

  ~WindowManager();

  // The entry point to this class. Enters the main event loop.
  void Run();

  private:
  // Invoked internally by Create().
  WindowManager(Display* display);
  // Frames a top-level window.
  void Frame(Window w, bool was_created_before_window_manager);
  // Unframes a client window.
  void Unframe(Window w);

  // Event handlers.
  void OnCreateNotify(const XCreateWindowEvent& e);
  void OnDestroyNotify(const XDestroyWindowEvent& e);
  void OnReparentNotify(const XReparentEvent& e);
  void OnMapNotify(const XMapEvent& e);
  void OnUnmapNotify(const XUnmapEvent& e);
  void OnConfigureNotify(const XConfigureEvent& e);
  void OnMapRequest(const XMapRequestEvent& e);
  void OnConfigureRequest(const XConfigureRequestEvent& e);
  void OnButtonPress(const XButtonEvent& e);
  void OnButtonRelease(const XButtonEvent& e);
  void OnMotionNotify(const XMotionEvent& e);
  void OnKeyPress(const XKeyEvent& e);
  void OnKeyRelease(const XKeyEvent& e);

  // Xlib error handler. It must be static as its address is passed to Xlib.
  static int OnXError(Display* display, XErrorEvent* e);
  // Xlib error handler used to determine whether another window manager is
  // running. It is set as the error handler right before selecting substructure
  // redirection mask on the root window, so it is invoked if and only if
  // another window manager is running. It must be static as its address is
  // passed to Xlib.
  static int OnWMDetected(Display* display, XErrorEvent* e);
  // Whether an existing window manager has been detected. Set by OnWMDetected,
  // and hence must be static.
  static bool wm_detected_;
  // A mutex for protecting wm_detected_. It's not strictly speaking needed as
  // this program is single threaded, but better safe than sorry.

  // Handle to the underlying Xlib Display struct.
  Display* display_;
  // Handle to root window.
  const Window root_;
  // Maps top-level windows to their frame windows.
  ::std::map<Window, Window> clients_;

  // The cursor position at the start of a window move/resize.
  Position<int> drag_start_pos_;
  // The position of the affected window at the start of a window
  // move/resize.
  Position<int> drag_start_frame_pos_;
  // The size of the affected window at the start of a window move/resize.
  Size<int> drag_start_frame_size_;

  // Atom constants.
  const Atom WM_PROTOCOLS;
  const Atom WM_DELETE_WINDOW;
};

using ::std::max;
using ::std::string;

bool WindowManager::wm_detected_;

WindowManager* WindowManager::Create(const string& display_str)
{
  // 1. Open X display.
  const char* display_c_str = display_str.empty() ? nullptr : display_str.c_str();
  Display* display = XOpenDisplay(display_c_str);
  if (display == nullptr) {
    printf("Failed to open X display %s\n", XDisplayName(display_c_str));
    return nullptr;
  }
  // 2. Construct WindowManager instance.
  return new WindowManager(display);
}

WindowManager::WindowManager(Display* display)
    : display_(display)
    , root_(DefaultRootWindow(display_))
    , WM_PROTOCOLS(XInternAtom(display_, "WM_PROTOCOLS", false))
    , WM_DELETE_WINDOW(XInternAtom(display_, "WM_DELETE_WINDOW", false))
{
}

WindowManager::~WindowManager()
{
  XCloseDisplay(display_);
}

int scroll_height = 0;
int number_of_windows = 0;
int root_window_x;
int root_window_y;
unsigned int root_window_width;
unsigned int root_window_height;
unsigned int root_window_border_width;
unsigned int root_window_depth;
Window returned_root;
unsigned int commandline_height;
Window commandline_window;

void WindowManager::Run()
{
  // 1. Initialization.
  //   a. Select events on root window. Use a special error handler so we can
  //   exit gracefully if another window manager is already running.
  {
    wm_detected_ = false;
    XSetErrorHandler(&WindowManager::OnWMDetected);
    XSelectInput(
        display_,
        root_,
        SubstructureRedirectMask | SubstructureNotifyMask);
    XSync(display_, false);
    if (wm_detected_) {
      printf("Detected another window manager on display %s\n", XDisplayString(display_));
      return;
    }
  }
  //   b. Set error handler.
  XSetErrorHandler(&WindowManager::OnXError);
  //   c. Grab X server to prevent windows from changing under us.
  XGrabServer(display_);
  //   d. Reparent existing top-level windows.
  //     i. Query existing top-level windows.
  Window returned_root, returned_parent;
  Window* top_level_windows;
  unsigned int num_top_level_windows;
  XQueryTree(
      display_,
      root_,
      &returned_root,
      &returned_parent,
      &top_level_windows,
      &num_top_level_windows);
  assert(returned_root == root_);
  //     ii. Frame each top-level window.
  for (unsigned int i = 0; i < num_top_level_windows; ++i) {
    Frame(top_level_windows[i], true);
  }
  //     iii. Free top-level window array.
  XFree(top_level_windows);
  //   e. Ungrab X server.
  XUngrabServer(display_);

  if (XGetGeometry(display_, DefaultRootWindow(display_), &returned_root, &root_window_x, &root_window_y, &root_window_width, &root_window_height, &root_window_border_width, &root_window_depth)) {
    printf("%d %d\n", root_window_width, root_window_height);
  }

  // 2. Main event loop.
  for (;;) {
    // 1. Get next event.
    XEvent e;
    XNextEvent(display_, &e);
    //printf("Received event: \n");

    // 2. Dispatch event.
    switch (e.type) {
    case CreateNotify:
      OnCreateNotify(e.xcreatewindow);
      break;
    case DestroyNotify:
      OnDestroyNotify(e.xdestroywindow);
      break;
    case ReparentNotify:
      OnReparentNotify(e.xreparent);
      break;
    case MapNotify:
      OnMapNotify(e.xmap);
      break;
    case UnmapNotify:
      OnUnmapNotify(e.xunmap);
      break;
    case ConfigureNotify:
      OnConfigureNotify(e.xconfigure);
      break;
    case MapRequest:
      OnMapRequest(e.xmaprequest);
      break;
    case ConfigureRequest:
      OnConfigureRequest(e.xconfigurerequest);
      break;
    case ButtonPress:
      OnButtonPress(e.xbutton);
      break;
    case ButtonRelease:
      OnButtonRelease(e.xbutton);
      break;
    case MotionNotify:
      // Skip any already pending motion events.
      while (XCheckTypedWindowEvent(
          display_, e.xmotion.window, MotionNotify, &e)) {
      }
      OnMotionNotify(e.xmotion);
      break;
    case KeyPress:
      OnKeyPress(e.xkey);
      break;
    case KeyRelease:
      OnKeyRelease(e.xkey);
      break;
    default:
      printf("Ignored event\n");
    }
  }
}

void WindowManager::Frame(Window w, bool was_created_before_window_manager)
{
  // Visual properties of the frame to create.
  const unsigned int BORDER_WIDTH = 1;
  const unsigned long BORDER_COLOR = 0xffffff;
  const unsigned long BG_COLOR = 0x000000;

  // We shouldn't be framing windows we've already framed.
  assert(!clients_.count(w));

  // 1. Retrieve attributes of window to frame.
  XWindowAttributes x_window_attrs;
  assert(XGetWindowAttributes(display_, w, &x_window_attrs));

  // 2. If window was created before window manager started, we should frame
  // it only if it is visible and doesn't set override_redirect.
  if (was_created_before_window_manager) {
    if (x_window_attrs.override_redirect || x_window_attrs.map_state != IsViewable) {
      return;
    }
  }

  Window frame;
  unsigned int frame_width = (unsigned int) x_window_attrs.width < root_window_width ? x_window_attrs.width : root_window_width;
  unsigned int frame_height = (unsigned int) x_window_attrs.height < root_window_height - commandline_height ? x_window_attrs.height : root_window_height - commandline_height;
  if (number_of_windows) {
    // 3. Create frame.
    
    frame = XCreateSimpleWindow(
        display_,
        root_,
        0,                                                                         //x_window_attrs.x,
        root_window_height - commandline_height - frame_height - BORDER_WIDTH, //gy, //x_window_attrs.y,
        frame_width,
        frame_height,
        BORDER_WIDTH,
        BORDER_COLOR,
        BG_COLOR);

    scroll_height += frame_height + BORDER_WIDTH;
    for( const auto& client : clients_ ) {
      if ( client.second != commandline_window ) {
      int window_x;
      int window_y;
      unsigned int window_width;
      unsigned int window_height;
      unsigned int window_border_width;
      unsigned int window_depth;
      Window returned_root;
      if (XGetGeometry(display_, client.second , &returned_root, &window_x, &window_y, &window_width, &window_height, &window_border_width, &window_depth)) {
        printf("%d %d\n", window_width, window_height);
        XMoveResizeWindow(display_, client.second, window_x, window_y - frame_height - BORDER_WIDTH, window_width, window_height);
      }
      }
    }
  } else {
    frame = XCreateSimpleWindow(
        display_,
        root_,
        0,                                     //x_window_attrs.x,
        root_window_height - x_window_attrs.height, //x_window_attrs.y,
        root_window_width,
        x_window_attrs.height,
        0,
        BORDER_COLOR,
        BG_COLOR);

    commandline_height = x_window_attrs.height;
    commandline_window = frame;

    auto stateAbove = XInternAtom(display_, "_NET_WM_STATE_ABOVE", True);
    XChangeProperty(display_, frame, XInternAtom(display_, "_NET_WM_STATE", False), XA_ATOM, 32, PropModeReplace, (unsigned char *) &stateAbove, 1);
  }

  number_of_windows += 1;

  // 4. Select events on frame.
  XSelectInput(
      display_,
      frame,
      SubstructureRedirectMask | SubstructureNotifyMask);
  // 5. Add client to save set, so that it will be restored and kept alive if we
  // crash.
  XAddToSaveSet(display_, w);
  // 6. Reparent client window.
  XReparentWindow(
      display_,
      w,
      frame,
      0, 0); // Offset of client window within frame.
  // 7. Map frame.
  XMapWindow(display_, frame);
  // 8. Save frame handle.
  clients_[w] = frame;
  // 9. Grab universal window management actions on client window.
  //   c. Kill windows with alt + f4.
  XGrabKey(
      display_,
      XKeysymToKeycode(display_, XK_F4),
      Mod1Mask,
      w,
      false,
      GrabModeAsync,
      GrabModeAsync);
  //   d. Switch windows with alt + tab.
  XGrabKey(
      display_,
      XKeysymToKeycode(display_, XK_Tab),
      Mod1Mask,
      w,
      false,
      GrabModeAsync,
      GrabModeAsync);

  printf("Framed window %ld [%ld]", w, frame);
}

void WindowManager::Unframe(Window w)
{
  assert(clients_.count(w));

  // We reverse the steps taken in Frame().
  const Window frame = clients_[w];
  // 1. Unmap frame.
  XUnmapWindow(display_, frame);
  // 2. Reparent client window.
  XReparentWindow(
      display_,
      w,
      root_,
      0, 0); // Offset of client window within root.
  // 3. Remove client window from save set, as it is now unrelated to us.
  XRemoveFromSaveSet(display_, w);
  // 4. Destroy frame.
  XDestroyWindow(display_, frame);
  // 5. Drop reference to frame handle.
  clients_.erase(w);

  printf("Unframed window %ld [%ld]", w, frame);
}

void WindowManager::OnCreateNotify(const XCreateWindowEvent& e) {}

void WindowManager::OnDestroyNotify(const XDestroyWindowEvent& e) {}

void WindowManager::OnReparentNotify(const XReparentEvent& e) {}

void WindowManager::OnMapNotify(const XMapEvent& e) {}

void WindowManager::OnUnmapNotify(const XUnmapEvent& e)
{
  // If the window is a client window we manage, unframe it upon UnmapNotify. We
  // need the check because we will receive an UnmapNotify event for a frame
  // window we just destroyed ourselves.
  if (!clients_.count(e.window)) {
    printf("Ignore UnmapNotify for non-client window %ld\n", e.window);
    return;
  }

  // Ignore event if it is triggered by reparenting a window that was mapped
  // before the window manager started.
  //
  // Since we receive UnmapNotify events from the SubstructureNotify mask, the
  // event attribute specifies the parent window of the window that was
  // unmapped. This means that an UnmapNotify event from a normal client window
  // should have this attribute set to a frame window we maintain. Only an
  // UnmapNotify event triggered by reparenting a pre-existing window will have
  // this attribute set to the root window.
  if (e.event == root_) {
    printf("Ignore UnmapNotify for reparented pre-existing window %ld", e.window);
    return;
  }

  Unframe(e.window);
}

void WindowManager::OnConfigureNotify(const XConfigureEvent& e) {}

void WindowManager::OnMapRequest(const XMapRequestEvent& e)
{
  // 1. Frame or re-frame window.
  Frame(e.window, false);
  // 2. Actually map window.
  XMapWindow(display_, e.window);
}

void WindowManager::OnConfigureRequest(const XConfigureRequestEvent& e)
{
  XWindowChanges changes;
  changes.x = e.x;
  changes.y = e.y;
  changes.width = e.width;
  changes.height = e.height;
  changes.border_width = e.border_width;
  changes.sibling = e.above;
  changes.stack_mode = e.detail;
  if (clients_.count(e.window)) {
    const Window frame = clients_[e.window];
    XConfigureWindow(display_, frame, e.value_mask, &changes);
    printf("Resize [%ld] to %d %d\n", frame, e.width, e.height);
  }
  XConfigureWindow(display_, e.window, e.value_mask, &changes);
  printf("Resize %ld to %d %d\n", e.window, e.width, e.height);
}

void WindowManager::OnButtonPress(const XButtonEvent& e)
{
  assert(clients_.count(e.window));
  const Window frame = clients_[e.window];

  // 1. Save initial cursor position.
  drag_start_pos_ = Position<int>(e.x_root, e.y_root);

  // 2. Save initial window info.
  Window returned_root;
  int x, y;
  unsigned width, height, border_width, depth;
  assert(XGetGeometry(
      display_,
      frame,
      &returned_root,
      &x, &y,
      &width, &height,
      &border_width,
      &depth));
  drag_start_frame_pos_ = Position<int>(x, y);
  drag_start_frame_size_ = Size<int>(width, height);

  // 3. Raise clicked window to top.
  XRaiseWindow(display_, frame);
}

void WindowManager::OnButtonRelease(const XButtonEvent& e) {}

void WindowManager::OnMotionNotify(const XMotionEvent& e)
{
  assert(clients_.count(e.window));
  const Window frame = clients_[e.window];
  const Position<int> drag_pos(e.x_root, e.y_root);
  const Vector2D<int> delta = drag_pos - drag_start_pos_;

  if (e.state & Button1Mask) {
    // alt + left button: Move window.
    const Position<int> dest_frame_pos = drag_start_frame_pos_ + delta;
    XMoveWindow(
        display_,
        frame,
        dest_frame_pos.x, dest_frame_pos.y);
  } else if (e.state & Button3Mask) {
    // alt + right button: Resize window.
    // Window dimensions cannot be negative.
    const Vector2D<int> size_delta(
        max(delta.x, -drag_start_frame_size_.width),
        max(delta.y, -drag_start_frame_size_.height));
    const Size<int> dest_frame_size = drag_start_frame_size_ + size_delta;
    // 1. Resize frame.
    XResizeWindow(
        display_,
        frame,
        dest_frame_size.width, dest_frame_size.height);
    // 2. Resize client window.
    XResizeWindow(
        display_,
        e.window,
        dest_frame_size.width, dest_frame_size.height);
  }
}

void WindowManager::OnKeyPress(const XKeyEvent& e)
{
  if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_F4))) {
    // alt + f4: Close window.
    //
    // There are two ways to tell an X window to close. The first is to send it
    // a message of type WM_PROTOCOLS and value WM_DELETE_WINDOW. If the client
    // has not explicitly marked itself as supporting this more civilized
    // behavior (using XSetWMProtocols()), we kill it with XKillClient().
    Atom* supported_protocols;
    int num_supported_protocols;
    if (XGetWMProtocols(display_,
            e.window,
            &supported_protocols,
            &num_supported_protocols)
        && (::std::find(supported_protocols,
                supported_protocols + num_supported_protocols,
                WM_DELETE_WINDOW)
               != supported_protocols + num_supported_protocols)) {
      printf("Gracefully deleting window %ld\n", e.window);
      // 1. Construct message.
      XEvent msg;
      memset(&msg, 0, sizeof(msg));
      msg.xclient.type = ClientMessage;
      msg.xclient.message_type = WM_PROTOCOLS;
      msg.xclient.window = e.window;
      msg.xclient.format = 32;
      msg.xclient.data.l[0] = WM_DELETE_WINDOW;
      // 2. Send message to window to be closed.
      assert(XSendEvent(display_, e.window, false, 0, &msg));
    } else {
      printf("Killing window %ld\n", e.window);
      XKillClient(display_, e.window);
    }
  } else if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_Tab))) {
    // alt + tab: Switch window.
    // 1. Find next window.
    auto i = clients_.find(e.window);
    assert(i != clients_.end());
    XSetWindowBorder(display_, i->second, 0x000000);
    ++i;
    if (i == clients_.end()) {
      i = clients_.begin();
    }

    XSetWindowBorder(display_, i->second, 0x0000FF);

    // 2. Raise and set focus.
    XRaiseWindow(display_, i->second);
    XSetInputFocus(display_, i->first, RevertToPointerRoot, CurrentTime);

    int window_x;
    int window_y;
    unsigned int window_width;
    unsigned int window_height;
    unsigned int window_border_width;
    unsigned int window_depth;
    Window returned_root;
    if (i->second != commandline_window && XGetGeometry(display_, i->second , &returned_root, &window_x, &window_y, &window_width, &window_height, &window_border_width, &window_depth)) {
      printf("%d %d\n", window_width, window_height);
      unsigned int dy = 0;
      if ( window_y < 0) {
        dy = - window_y;
      } else if ( window_y + window_height > root_window_height - commandline_height ) {
        dy = root_window_height - commandline_height - window_height - window_y;
      }
      for( const auto& client : clients_ ) {
        if ( client.second != commandline_window ) {
          int window2_x;
          int window2_y;
          unsigned int window2_width;
          unsigned int window2_height;
          unsigned int window2_border_width;
          unsigned int window2_depth;
          Window returned2_root;
          if (XGetGeometry(display_, client.second , &returned2_root, &window2_x, &window2_y, &window2_width, &window2_height, &window2_border_width, &window2_depth)) {
            XMoveResizeWindow(display_, client.second, window2_x, window2_y + dy, window2_width, window2_height);
          }
        }
      }
    }
  } else if ((e.state & Mod1Mask) && (e.keycode == XKeysymToKeycode(display_, XK_Pointer_Button4))) {
    printf("wheel up\n");
  }
  printf("keypressed %d %d\n", e.state, e.keycode);
}

void WindowManager::OnKeyRelease(const XKeyEvent& e) {}

int WindowManager::OnXError(Display* display, XErrorEvent* e)
{
  const int MAX_ERROR_TEXT_LENGTH = 1024;
  char error_text[MAX_ERROR_TEXT_LENGTH];
  XGetErrorText(display, e->error_code, error_text, sizeof(error_text));
  //LOG(ERROR) << "Received X error:\n"
  //           << "    Request: " << int(e->request_code)
  //           << " - " << XRequestCodeToString(e->request_code) << "\n"
  //           << "    Error code: " << int(e->error_code)
  //           << " - " << error_text << "\n"
  //           << "    Resource ID: " << e->resourceid;
  // The return value is ignored.
  return 0;
}

int WindowManager::OnWMDetected(Display* display, XErrorEvent* e)
{
  // In the case of an already running window manager, the error code from
  // XSelectInput is BadAccess. We don't expect this handler to receive any
  // other errors.
  assert(e->error_code == BadAccess);
  // Set flag.
  wm_detected_ = true;
  // The return value is ignored.
  return 0;
}

int main(int argc, char** argv)
{
  WindowManager* window_manager = WindowManager::Create();
  if (!window_manager) {
    printf("Failed to initialize window manager.\n");
    return EXIT_FAILURE;
  }

  window_manager->Run();

  return EXIT_SUCCESS;
}
