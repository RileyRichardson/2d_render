#include "2d_render.h"
void move(Loop* loop){
    loop->getImageByID("1")->x+=4;
}
int WinMain(HINSTANCE inst,HINSTANCE prev,LPSTR cmd,int show){ 
    Loop loop(WINDOW_WIDTH,WINDOW_HEIGHT);
    loop.frame.read("MonaLisa.bmp");
    loop.createImage("1",200,200,50,50,1);
    loop.getImageByID("1")->createBlank(200,100,50);
    std::thread* wind_msgs=loop.launch(inst,show);
    int x=1;
    auto lambda=[&loop,&x]{
        loop.getImageByID("1")->x+=4;
        loop.getImageByID("1")->createBlank(x,x*20,x*30);
        x++;
    };
    loop.createTimer("t",Timer(100,lambda,true));
    wind_msgs->join();
    return 0;
}