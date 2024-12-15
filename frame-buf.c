#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//https://stackoverflow.com/questions/64521652/create-xlib-window-with-a-frame-buffer-i-can-draw-directly-and-use-xputimage

int bug;

int main(int argc, char **argv)
{
Display *my_display;
Window root_window;
Window parent;
Window my_win;
XImage * fb_image;
XSetWindowAttributes attrs;
XVisualInfo vinfo;
XVisualInfo *visual_list;
XVisualInfo visual_template;
Visual *visual;
XEvent event;

int nxvisuals;
int depth;
int my_win_height, my_win_width;
int fb_width, fb_height;

int *framebuf;
  
int root_x, root_y; //<--two
unsigned int mask; //<--three
    
//---    

my_display = XOpenDisplay(NULL);

nxvisuals = 0;
visual_template.screen = DefaultScreen(my_display);
visual_list = XGetVisualInfo (my_display, VisualScreenMask, &visual_template, &nxvisuals);

if (!XMatchVisualInfo(my_display, XDefaultScreen(my_display), 24, TrueColor, &vinfo))
    {
    fprintf(stderr, "no such visual\n");
    return 1;
    }

parent = XDefaultRootWindow(my_display);

XSync(my_display, True);

printf("creating RGBA child\n");

visual = vinfo.visual;
depth = vinfo.depth;

attrs.colormap = XCreateColormap(my_display, XDefaultRootWindow(my_display), visual, AllocNone);
attrs.background_pixel = 0;
attrs.border_pixel = 0;

//setup a framebuffer
fb_width = 1024;
fb_height = 400;
framebuf = (int *) malloc((fb_width*fb_height)*4);

//just some random patterns for now
for (int i = 0; i < (fb_width*fb_height); i++)
    {
    framebuf[i] = 0xFF00FFFF;
    }
    
for (int i = 0; i < (fb_width*100); i++)
    {
    framebuf[i] = 0x000000FF;
    }
    
for (int i = 0; i < (fb_width*50); i++)
    {
    framebuf[i] = 0x00000000i+ i * 65536 ; //0x0000ff00;
    }
    
my_win_width = 1224;
my_win_height = 800;    
        
//XCreateWindow(display, parent, x, y, width, height, border_width, depth,class, visual, valuemask, attributes)
my_win = XCreateWindow(my_display, parent,0, 0, my_win_width, my_win_height, 0, depth, InputOutput,
                        visual, CWBackPixel | CWColormap | CWBorderPixel, &attrs);

XStoreName(my_display, my_win, "Frobisher");
      

fb_image = XCreateImage(my_display, vinfo.visual, depth, ZPixmap, 0, (char *)framebuf, fb_width, fb_height, 8, fb_width*4);
    
if (fb_image == 0)
    {
    printf("fb image is null!\n");
    }
    
    
    //XQueryPointer(display, DefaultRootWindow(display), &root_window, &root_window, &root_x, &root_y, &root_x, &root_y, &mask); //<--four
    
    XQueryPointer(my_display, DefaultRootWindow(my_display), &root_window, &root_window, &root_x, &root_y, &root_x, &root_y, &mask); //<--four

    printf("Mouse coordinates (X: %d, Y: %d)\n", root_x, root_y);

     

   

    XSync(my_display, True);

//https://stackoverflow.com/questions/16122196/getting-mouseclick-coordinates-with-xlib
    XSelectInput(my_display, my_win,ButtonReleaseMask| ExposureMask | KeyPressMask);

    XGCValues gcv;
    unsigned long gcm;
    GC NormalGC;

    //gcm = GCForeground | GCBackground | GCGraphicsExposures;
    //gcv.foreground = BlackPixel(my_display, parent);
    //gcv.background = WhitePixel(my_display, parent);
    gcm = GCGraphicsExposures;
    gcv.graphics_exposures = 0;
    NormalGC = XCreateGC(my_display, parent, gcm, &gcv);

    XMapWindow(my_display, my_win);
    
 

/*    Just me poking about - find out and re-instate !!!!!!!!!! */

    while(!XNextEvent(my_display, &event))
    {
        switch(event.type)
        {
        case Expose:
            printf("I have been exposed!\n");
            //XPutImage(display, d, gc, image, src_x, src_y, dest_x, dest_y, width, height)
            XPutImage(my_display, my_win, NormalGC, fb_image, 0, 0, 100, 200, fb_width, fb_height);
            break;
            default:
                printf("Other \n");
        }
        //https://stackoverflow.com/questions/16122196/getting-mouseclick-coordinates-with-xlib
        XQueryPointer(my_display, DefaultRootWindow(my_display), &root_window, &root_window, &root_x, &root_y, &root_x, &root_y, &mask); //<--four

    printf("Mouse coordinates (X: %d, Y: %d)\n", root_x, root_y);
    }

//sleep(1); //this is guesswork

//XPutImage(my_display, win, NormalGC, ximage, 0, 0, 0, 0, width, height);

while(1)
{
    sleep(1);
    printf(" Bug %d \n",bug++); 
}

    printf("No error\n");

    return 0;
}

