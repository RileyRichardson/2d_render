#include "2d_render.h"
void move(Loop* loop){
    loop->getImageByID("1")->x+=4;
}
int WinMain(HINSTANCE inst,HINSTANCE prev,LPSTR cmd,int show){ 
    //this creates a loop instance. the width and height is the resolution the final image will be displayed in.
    Loop loop(WINDOW_WIDTH,WINDOW_HEIGHT);
    //frame is the background set of pixels used in rendering 
    loop.frame.read("MonaLisa.bmp");
    //create image take in a name for the image, width, height, x, y, transparity between 0 and 1, and has an optional paramater "render order"
    loop.createImage("1",200,200,50,50,1);
    //return a pointer to image with same name
    loop.getImageByID("1")->createBlank(200,100,50);
    //launch starts the loop, creates a window, and return a thread that has to be joined later in the program
    //it will join when the window is closed
    std::thread* wind_msgs=loop.launch(inst,show);
    int x=1;
    auto lambda=[&loop,&x]{
        loop.getImageByID("1")->x+=4;
        loop.getImageByID("1")->createBlank(x,x*20,x*30);
        x++;
    };
    //take a name for the timer and a timer which takes an interval in milliseconds, a callable, and a boolean true if the timer constantly repeats
    //and false if it is only called once
    loop.createTimer("t",Timer(100,lambda,true));
    //if this isn't joined than the window will close as soon as the main function finishes
    wind_msgs->join();
    return 0;
}
