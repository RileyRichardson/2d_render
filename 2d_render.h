#pragma once
#include <thread>
#include <windows.h>
#include <vector>
#include <iostream>
#include <ctime>
#include <cstdio>
#include <fstream>
#include <mutex>
#include <functional>

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH 980
#endif
#ifndef WINDOW_HEIGHT 
#define WINDOW_HEIGHT 540
#endif
#ifndef TPS
#define TPS 60
#endif
#ifndef FPS
#define FPS 60
#endif
typedef const char* str;

struct Color{
    float r,g,b;
    Color();
    Color(float r,float g,float b);
    ~Color();
};

struct Image{
    Image(int width,int height,int x,int y,float transparity);
    Image();
    ~Image();
    int x,y; 
    int m_width;
    int m_height;  
    int render_order;
    float transperency;
    bool visible;
    bool consistent_transparency;
    Color getColor(int x, int y) const;
    Color getColorSeq(int x, int y);
    void setColor(const Color& color,int x,int y);
    void setColor(float r,float g,float b,int x,int y);
    void read(str path);
    void write(str path) const;
    void resize(int new_width, int new_height);
    void changeWidth(int new_width);
    void changeHeight(int new_height);
    void restoreOriginal();
    void createBlank(int r,int g,int b);
    void createRandom(int width,int height);
    void normByWidth();
    void normByHeight();
    void clip(int x_left,int x_right,int y_top,int y_bottom);
    void setTransparency(float trancparency,int x,int y);
    std::vector<Color>pixels;
    std::vector<float>transparity_map;
private:
    int o_width;
    int o_height;
    std::vector<Color>original;
};

struct MasterImage{
    MasterImage(int width, int height);
    int width;
    int height;  
    void createBlank(int r,int g,int b);
    void randomize();
    void setBits();
    void setColor(const Color& color,int x,int y);
    void read(str path);
    void imposeText(str text);
    Color getColorSeq(int x, int y);
    Color getColor(int x,int y);
    std::vector<Color>pixels;
    std::vector<Color>o_pixels;
    void* memory;  
};

struct Sprite{
    int x,y;
    int step=0;
    int render_order;
    double cps;
    int upc;
    int rpc;
    bool continuous;
    std::vector<Image> steps;
    Sprite(int x,int y);
    std::clock_t last_update;
};

struct Timer{
    bool repeats;
    long interval;
    std::clock_t last_update;
    Timer(long interval,auto& lambda,bool repeats);
    std::function<void()> function;
};

class Loop{
    BITMAPINFO bmi;
    double accumulator;
    void update();
    void render(HWND hwnd);
    std::vector<str>image_names;
    std::vector<str>sprite_names;
    std::vector<str>timer_names;
    std::vector<Image>images;
    std::vector<Sprite>sprites;
    std::vector<Timer>timers;
    void checkTimers();
    void loop(HINSTANCE inst,int show);
    std::thread message_loop;
public:
    bool updating;
    bool rendering;
    ~Loop();
    Loop(int width,int height);
    MasterImage frame;
    void step(HWND hwnd,int tps,int fps_cap=360);
    Image* getImageByID(str imageID);
    Sprite* getSpriteByID(str spriteID);
    Timer* getTimerByID(str timerID);
    void createImage(str imageID,int width,int height,int x, int y,float transperity,int render_order=0);
    void deleteImage(str imageID);
    void createSprite(str spriteID,int x,int y,int render_order);
    void createTimer(str timerID,Timer timer);
    void writeFrame(str path);
    std::thread* launch(HINSTANCE inst,int show);
    std::clock_t time;
};


#include "src/timer.cpp"
#include "src/bmp.cpp"
#include "src/sprite.cpp"
#include "src/lib.cpp"
#include "src/loop.cpp" 
