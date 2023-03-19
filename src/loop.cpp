Loop::Loop(int width,int height):accumulator(0),frame(MasterImage(width,height)),time(std::clock()){
    bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;  // Use a negative value to indicate a top-down DIB
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
};
Loop::~Loop(){};
void Loop::render(HWND hwnd){
    Image* image;
    int s_rended=0;
    int i_rended=0;
    for(int i=0;i<images.size()+sprites.size();i++){
        if(sprites.size()>0&&images.size()>0){
            if(sprites[i-i_rended].render_order>images[i-s_rended].render_order){
                image=&images[i-s_rended];
                i_rended+=1;
            }else{
                image=&sprites[i-i_rended].steps[sprites[i-i_rended].step];
                image->x=sprites[i-i_rended].x;
                image->y=sprites[i-i_rended].y;
                s_rended+=1;
            }
        }else if(sprites.size()>0){
            image=&sprites[i].steps[sprites[i].step];
            image->x=sprites[i].x;
            image->y=sprites[i].y;
        }else if(images.size()>0){
            image=&images[i];
        }else{break;}
        float tran=image->transperency;
        if(image->y<frame.height&&image->y+image->m_height>0&&image->x+image->m_width>0&&image->x<frame.width&&image->visible){
        for(int y=0;y<image->m_height;y++){
            if(y+image->y<frame.height&&y+image->y>0){
            for(int x=0;x<image->m_width;x++){
                if(!image->consistent_transparency){
                    tran=image->transparity_map[y*image->m_width+x];
                }
                if(x+image->x<frame.width&&x+image->x>0){
                Color col=image->getColorSeq(x,y);
                col.r=col.r*tran+(1-tran)*frame.getColor(x+image->x,y+image->y).r;
                col.g=col.g*tran+(1-tran)*frame.getColor(x+image->x,y+image->y).g;
                col.b=col.b*tran+(1-tran)*frame.getColor(x+image->x,y+image->y).b;
                frame.setColor(col,x+image->x,y+image->y);  
                }
            }
            }
        }
        }
    }
    RECT rect;
    GetWindowRect(hwnd, &rect);
    int w_width=(rect.right)-(rect.left);
    int w_height=(rect.bottom)-(rect.top);
    frame.setBits();
    StretchDIBits(GetDC(hwnd),0,0,w_width,w_height,0,0,frame.width,frame.height,frame.memory,&bmi,DIB_RGB_COLORS,SRCCOPY);
    frame.pixels=frame.o_pixels;
}
void Loop::createTimer(str timerID,Timer timer){
    timers.push_back(timer);
    timer_names.push_back(timerID);
}
void Loop::checkTimers(){
    for(int i=0;i<timers.size();i++){
        if(std::clock()-timers[i].last_update>timers[i].interval){
            timers[i].function();
            if(timers[i].repeats){
                timers[i].last_update=std::clock()-((clock()-timers[i].last_update)-timers[i].interval);
            }else{
                timers.erase(timers.begin()+i);
                timer_names.erase(timer_names.begin()+i);
            }
        }
    }
}

void Loop::step(HWND hwnd,int tps,int fps_cap){
    while(accumulator>=1000/tps){
        update();
        accumulator-=1000/tps;
    }
    render(hwnd);
    accumulator+=1000/fps_cap;
    double delta = (double)(( std::clock() - time ) * 1000/ (double) CLOCKS_PER_SEC);
    checkTimers();
    if((1000/(double)fps_cap)-delta>0){
        Sleep((1000/(double)fps_cap)-delta);
    }
    time=std::clock();
}
Image* Loop::getImageByID(str imageID){
    int index=-1;
    for (int i=0;i<images.size();i++){
        if(imageID==image_names[i]){
            index=i;
            break;
        }
    }
    if(index!=-1){
        return &images[index];
    }else{
        return &images[0];
    }
}
Sprite* Loop::getSpriteByID(str spriteID){
    int index=-1;
    for(int i=0;i<sprites.size();i++){
        if(spriteID==sprite_names[i]){
            index=i;
        }
    }
    if(index!=-1){
        return &sprites[index];
    }else{
        return &sprites[0];
    }
}
Timer* Loop::getTimerByID(str timerID){
    int index=-1;
    for(int i=0;i<timers.size();i++){
        if(timerID==timer_names[i]){
            index=i;
        }
    }
    if(index!=-1){
        return &timers[index];
    }else{
        return &timers[0];
    }
}
void Loop::createImage(str imageID,int width,int height,int x, int y,float transperity,int render_order){
    Image image(width,height,x,y,transperity);
    image.render_order=render_order;
    int i=0;
    if(images.size()>0){
       int last_order=images[0].render_order;
        while(last_order<=render_order){
            i++;
            last_order=images[i].render_order;
        }
    }
    images.insert(images.begin()+i,image);
    image_names.insert(image_names.begin()+i,imageID);
}
void Loop::writeFrame(str path){
    for(int i=0;i<images.size();i++){
        for(int y=0;y<images[i].m_height;y++){
            if(y+images[i].y<frame.height&&y+images[i].y>0){
                for(int x=0;x<images[i].m_width;x++){
                    if(x+images[i].x<frame.width&&x+images[i].x>0){
                        frame.setColor(images[i].getColorSeq(x,y),x+images[i].x,y+images[i].y);
                    }
                }
            }
        }
    }
    Image frame_(frame.width,frame.height,0,0,1);
    for(int x=0;x<frame.width;x++){
        for(int y=0;y<frame.height;y++){
            frame_.setColor(frame.getColorSeq(x,y),x,y);
        }
    }
    frame_.write(path);
    frame.pixels=frame.o_pixels;
}
void Loop::deleteImage(str ImageID){
    int index;
    for (int i=0;i<images.size();i++){
        if(ImageID==image_names[i]){
            index=i;
            break;
        }
    }
    image_names.erase(image_names.begin()+index);
    images.erase(images.begin()+index);
}

void Loop::createSprite(str spriteID,int x,int y,int render_order){
    Sprite sprite(x,y);
    sprite.render_order=render_order;
    int i=0;
    if(sprites.size()>0){
        int last_order=sprites[0].render_order;
        while(last_order<=render_order){
            i++;
            last_order=sprites[i].render_order;
        }
    }
    sprites.insert(sprites.begin()+i,sprite);
    sprite_names.insert(sprite_names.begin()+i,spriteID);
}
void Loop::loop(HINSTANCE inst,int show){
    HWND wnd = createWindow(inst,WINDOW_WIDTH,WINDOW_HEIGHT);
    ShowWindow(wnd, show);
    MSG msg;
    while(WM_QUIT!=msg.message){
        while (PeekMessage(&msg, NULL,0,0,PM_REMOVE)){
            TranslateMessage(&msg);    
            DispatchMessage(&msg);      
        }
        step(wnd,TPS,FPS);
    } 
}

std::thread* Loop::launch(HINSTANCE inst,int show){
    message_loop=std::thread(loop,this,inst,show);
    return &message_loop;
}
void Loop::update(){

}