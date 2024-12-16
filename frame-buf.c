#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

//https://stackoverflow.com/questions/64521652/create-xlib-window-with-a-frame-buffer-i-can-draw-directly-and-use-xputimage

#define WFALL_HEIGHT 400
#define WFALL_WIDTH 1024

int bug;

int *framebuf;

//===




void wf_scroll_down()
{
//Scroll all wflines down, starting from the bottom
    for(int ll = WFALL_HEIGHT; ll >=0 ; ll--)
    {
    for(int pp = 0;pp<WFALL_WIDTH;pp++)
        {
        framebuf[((ll+1)*WFALL_WIDTH)+WFALL_WIDTH+pp] = framebuf[((ll)* WFALL_WIDTH)+pp];
        }
    }
}
//===

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

//int *framebuf;
  
int root_x, root_y;
int mouse_x,mouse_y;
unsigned int mask; 
    
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

XSync(my_display, True); //wait for all requests to be completed

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

XMapWindow(my_display, my_win); //creates a Graphic Context (so there)
    
/*    Just me poking about - find out and re-instate !!!!!!!!!! */

// Should we loop waiting for next event - or - make our own main (waiting) loop ???
/*

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
   
    //XQueryPointer(display, w, root_return, child_return, root_x_return, root_y_return, win_x_return, win_y_return, mask_return)
    XQueryPointer(my_display, DefaultRootWindow(my_display), &root_window, &root_window, &root_x, &root_y, &mouse_x, &mouse_y, &mask);
    printf("Mouse coordinates (X: %d, Y: %d)\n", mouse_x, mouse_y);
    
    wf_scroll_down();
    
    }
*/

//sleep(1); //this is guesswork
//XPutImage(my_display, my_win, NormalGC, fb_image, 0, 0, 100, 200, fb_width, fb_height);

while(1)
    {
    usleep(50000);
    //printf(" Bug %d \n",bug++); 
    
    XPutImage(my_display, my_win, NormalGC, fb_image, 0, 0, 100, 200, fb_width, fb_height);
    wf_scroll_down();
    }

printf("No error\n");
return 0;
}

