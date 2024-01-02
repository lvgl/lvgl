#ifndef _X11_FAKE_TYPEDEFS_H
#define _X11_FAKE_TYPEDEFS_H

typedef char* XPointer;
typedef unsigned char KeyCode;
typedef unsigned int  CARD32;
typedef unsigned long VisualID;
typedef unsigned long XIMResetState;
typedef unsigned long XID;
typedef XID Window;
typedef XID Colormap;
typedef XID Cursor;
typedef XID Drawable;
typedef void* XtPointer;
typedef XtPointer XtRequestId;
typedef struct Display Display;
typedef struct Screen Screen;
typedef struct Status Status;
typedef struct Visual Visual;
typedef struct Widget *Widget;
typedef struct XColor XColor;
typedef struct XClassHint XClassHint;
typedef struct XEvent XEvent;
typedef struct XFontStruct XFontStruct;
typedef struct XGCValues XGCValues;
typedef struct XKeyEvent XKeyEvent;
typedef struct XKeyPressedEvent XKeyPressedEvent;
typedef struct XPoint XPoint;
typedef struct XRectangle XRectangle;
typedef struct XSelectionRequestEvent XSelectionRequestEvent;
typedef struct XWindowChanges XWindowChanges;
typedef struct _XGC _XCG;
typedef struct _XGC *GC;
typedef struct _XIC *XIC;
typedef struct _XIM *XIM;
typedef struct _XImage XImage;

#endif
