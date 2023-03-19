Color::Color():r(0),g(0),b(0){};
Color::Color(float r,float g, float b):r(r),g(g),b(b){};
Color::~Color(){};
Image::Image(int width, int height,int x, int y,float transparity):
    m_width(width),m_height(height),x(x),y(y),pixels(std::vector<Color>(width*height)),original(pixels),
    o_height(height),o_width(width),transparity_map(std::vector<float>(width*height)),visible(true),
    consistent_transparency(true),transperency(transparity){
        for(int i=0;i<width*height;i++){
            transparity_map[i]=transparity;
        }
        
    };
Image::Image(){};
Image::~Image(){};
Color Image::getColor(int x,int y) const{
    return pixels[y*m_width+x];  
}
Color Image::getColorSeq(int x, int y){
    return pixels[abs(y-m_height+1)*m_width+x];
}
void Image::setColor(const Color& color,int x,int y){
    pixels[y*m_width+x].r=color.r;
    pixels[y*m_width+x].g=color.g;
    pixels[y*m_width+x].b=color.b;
}
void Image::setColor(float r,float g,float b,int x,int y){
    pixels[y*m_width+x].r=r;
    pixels[y*m_width+x].g=g;
    pixels[y*m_width+x].b=b;
}
void Image::restoreOriginal(){
    pixels=original;
    m_width=o_width;
    m_height=o_height;
}
void Image::resize(int new_width, int new_height){
    restoreOriginal();
    changeWidth(new_width);
    changeHeight(new_height);
}
void Image::changeWidth(int new_width){
    std::vector<Color>WStreched(new_width*m_height);
    Color color;
    if(new_width>m_width){
        float accumulator=0;
        float strecth=(float)new_width/(float)m_width;;
        for(int y=0;y<m_height;y++){
            for(int x=0;x<m_width;x++){
                accumulator+=(strecth-(int)strecth);
                for(int i=0;i<(int)(strecth+accumulator);i++){
                    float weight=(float)i/(strecth+accumulator);
                    color.r=(float)((pixels[y*m_width+x].r*(1-weight))+(pixels[y*m_width+x+1].r*weight));
                    color.g=(float)((pixels[y*m_width+x].g*(1-weight))+(pixels[y*m_width+x+1].g*weight));
                    color.b=(float)((pixels[y*m_width+x].b*(1-weight))+(pixels[y*m_width+x+1].b*weight));
                    WStreched[y*new_width+(int)((float)x*strecth)+i]=color;
                }
                if(accumulator>1){
                    accumulator-=1;
                }
            }
        }
    pixels=WStreched;
    m_width=new_width;
    }else if(new_width<m_width){
        for(int y=0;y<m_height;y++){
            float shrink_fac=(float)m_width/(float)new_width;
            for(int x=0;x<new_width;x++){
                float red=0;
                float green=0;
                float blue=0;
                int X=(float)x*shrink_fac;
                for(int v=0;v<(int)shrink_fac;v++){
                    red+=pixels[y*m_width+X+v].r;
                    green+=pixels[y*m_width+X+v].g;
                    blue+=pixels[y*m_width+X+v].b;
                }
                WStreched[y*new_width+x].r=red/shrink_fac;
                WStreched[y*new_width+x].g=green/shrink_fac;
                WStreched[y*new_width+x].b=blue/shrink_fac;
            }
        }
        pixels=WStreched;
        m_width=new_width;
    }
}
void Image::changeHeight(int new_height){
    std::vector<Color>HStreched(new_height*m_width);
    pixels.resize(new_height*m_width);
    Color color;
    if(new_height>o_height){
        float accumulator=0;
        float strecth=(float)new_height/(float)o_height;
        for(int x=0;x<m_width;x++){
            for(int y=0;y<o_height;y++){
                accumulator+=(strecth-(int)strecth);
                for(int i=0;i<(int)(strecth+accumulator);i++){
                    float weight=(float)i/(strecth+accumulator);
                    color.r=(float)((pixels[y*m_width+x].r*(1-weight))+(pixels[(y+1)*m_width+x].r*weight));
                    color.g=(float)((pixels[y*m_width+x].g*(1-weight))+(pixels[(y+1)*m_width+x].g*weight));
                    color.b=(float)((pixels[y*m_width+x].b*(1-weight))+(pixels[(y+1)*m_width+x].b*weight));
                    HStreched[(int)((float)y*strecth+(float)i)*m_width+x]=color;
                }
                if(accumulator>1){
                    accumulator-=1;
                }
            }
        }
        pixels=HStreched;
        m_height=new_height;
    }else if(new_height<m_height){
        for(int x=0;x<m_width;x++){
            float shrink_fac=(float)m_height/(float)new_height;
            for(int y=0;y<new_height;y++){
                float red=0;
                float green=0;
                float blue=0;
                int Y=(float)y*shrink_fac;
                for(int v=0;v<(int)shrink_fac;v++){
                    red+=pixels[Y*m_width+x].r;
                    green+=pixels[(Y+v)*m_width+x].g;
                    blue+=pixels[(Y+v)*m_width+x].b;
                }
                HStreched[y*m_width+x].r=red/shrink_fac;
                HStreched[y*m_width+x].g=green/shrink_fac;
                HStreched[y*m_width+x].b=blue/shrink_fac;
            }
        }
        pixels=HStreched;
        m_height=new_height;
    }
}
void Image::read(str path){
    std::ifstream f;
    f.open(path,std::ios::in|std::ios::binary);
    if(!f.is_open()){
        return;
    }
    const int fileHeadSize=14;
    const int infoHeadSize = 40;
    unsigned char fileHeader[fileHeadSize];
    unsigned char infoHeader[infoHeadSize];
    f.read(reinterpret_cast<char*>(fileHeader),fileHeadSize);
    f.read(reinterpret_cast<char*>(infoHeader),infoHeadSize);
    int fileSize=fileHeader[2]+(fileHeader[3]<<8)+(fileHeader[4]<<16)+(fileHeader[5]<<24);
    m_width=infoHeader[4]+(infoHeader[5]<<8)+(infoHeader[6]<<16)+(infoHeader[7]<<24);
    m_height=infoHeader[8]+(infoHeader[9]<<8)+(infoHeader[10]<<16)+(infoHeader[11]<<24);
    pixels.resize(m_width*m_height);
    const int bits_padding=((4-(m_width*3)%4)%4);
    for(int y=0;y<m_height;y++){
        for(int x=0;x<m_width;x++){
            unsigned char color[3];
            f.read(reinterpret_cast<char*>(color),3);
            pixels[y*m_width+x].r=static_cast<float>(color[2])/255.0f;
            pixels[y*m_width+x].g=static_cast<float>(color[1])/255.0f;
            pixels[y*m_width+x].b=static_cast<float>(color[0])/255.0f;
        }
        f.ignore(bits_padding);
    }
    f.close();
    original=pixels;
    o_width=m_width;
    o_height=m_height;
}
void Image::write(str path) const{
    std::ofstream f;
    f.open(path, std::ios::out|std::ios::binary);
    if(!f.is_open()){
        return;
    }
    unsigned char bmp_pad[3];
    bmp_pad[0]=0;
    bmp_pad[1]=0;
    bmp_pad[2]=0;
    const int bits_padding=((4-(m_width*3)%4)%4);
    const int fileHeadSize=14;
    const int infoHeadSize = 40;
    const int fileSize=fileHeadSize+infoHeadSize+m_width*m_height*3+bits_padding*m_height;

    unsigned char fileHeader[fileHeadSize];
    //file type
    fileHeader[0]='B';
    fileHeader[1]='M';
    //file size
    fileHeader[2]=fileSize;
    fileHeader[3]=fileSize>>8;
    fileHeader[4]=fileSize>>16;
    fileHeader[5]=fileSize>>24;
    //reserved 1
    fileHeader[6]=0;
    fileHeader[7]=0;
    //reserved 2
    fileHeader[8]=0;
    fileHeader[9]=0;
    //pixels-data offset
    fileHeader[10]=fileHeadSize+infoHeadSize;
    fileHeader[11]=0;
    fileHeader[12]=0;
    fileHeader[13]=0;
    
    unsigned char infoHeader[infoHeadSize];
    //header size
    infoHeader[0]=infoHeadSize;
    infoHeader[1]=0;
    infoHeader[2]=0;
    infoHeader[3]=0;
    //image width
    infoHeader[4]=m_width;
    infoHeader[5]=m_width>>8;
    infoHeader[6]=m_width>>16;
    infoHeader[7]=m_width>>24;
    //image height
    infoHeader[8]=m_height;
    infoHeader[9]=m_height>>8;
    infoHeader[10]=m_height>>16;
    infoHeader[11]=m_height>>24;
    //planes
    infoHeader[12]=1;
    infoHeader[13]=0;
    //bit per pixels:RGB
    infoHeader[14]=24;
    infoHeader[15]=0;
    /*
    16-19: compression
    20-23: image size
    24-27: X pixelss per meter
    28-31: Y pixelss per meter
    32-35: total colors
    36-39: important colors
    */
    for(int l=16;l<40;l++){
        infoHeader[l]=0;
    }
    f.write(reinterpret_cast<char*>(fileHeader),fileHeadSize);
    f.write(reinterpret_cast<char*>(infoHeader),infoHeadSize);
    for(int y=0;y<m_height;y++){
        for(int x=0;x<m_width;x++){
            unsigned char r=static_cast<unsigned char>(getColor(x,y).r*255.0f);
            unsigned char g=static_cast<unsigned char>(getColor(x,y).g*255.0f);
            unsigned char b=static_cast<unsigned char>(getColor(x,y).b*255.0f);
            unsigned char color[]={b,g,r};
            f.write(reinterpret_cast<char*>(color),3);
        }
        f.write(reinterpret_cast<char*>(bmp_pad),bits_padding);
    }
    f.close();
}
void Image::createRandom(int width, int height){
    pixels=std::vector<Color>(width*height);
    m_width=width;
    o_width=width;
    m_height=height;
    o_height=height;
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            Color color;
            color.r=(float)(rand()%255)/255;
            color.g=(float)(rand()%255)/255;
            color.b=(float)(rand()%255)/255;
            pixels[y*width+x]=color;
        }
    }
    original=pixels;
}
void Image::normByWidth(){
    float aspect=(float)o_height/(float)o_width;
    resize(m_width,(int)(m_width*aspect));
}
void Image::normByHeight(){
    float aspect=(float)o_height/(float)o_width;
    resize((int)(m_height/aspect),m_height);
}
void Image::createBlank(int r,int g,int b){
    for(int y=0;y<m_height;y++){  
        for(int x=0;x<m_width;x++){
            pixels[y*m_width+x].r=(float)r/255;
            pixels[y*m_width+x].g=(float)g/255;
            pixels[y*m_width+x].b=(float)b/255;
        }
    }
}
void Image::clip(int x_left,int x_right,int y_top,int y_bottom){
    int new_width=x_right-x_left;
    int new_height=y_bottom-y_top;
    std::vector<Color>map_clipped(new_width*new_height);
    for(int x=0;x<new_width;x++){
        for(int y=0;y<new_height;y++){
            map_clipped[y*new_width+x]=pixels[(y+y_top)*m_width+x+x_left];
        }
    }
    m_width=new_width;
    m_height=new_height;
}
void Image::setTransparency(float trancparency,int x,int y){
    transparity_map[y*m_width+x]=trancparency;
}

MasterImage::MasterImage(int width_s, int height_s):
    width(width_s),height(height_s),pixels(std::vector<Color>(width_s*height_s))
    ,memory(VirtualAlloc(0,height_s*width_s*4,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE)),o_pixels(std::vector<Color>(width_s*height_s)){
    };
void MasterImage::setBits(){
    unsigned char* ref=(unsigned char *)memory;
    for(int i=0;i<width*height;i++){
        *ref++ =static_cast<unsigned char>(pixels[i].b*255.0f);
        *ref++ =static_cast<unsigned char>(pixels[i].g*255.0f);
        *ref++ =static_cast<unsigned char>(pixels[i].r*255.0f);
        *ref++;
    }
}
void MasterImage::randomize(){
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            pixels[y*width+x].r=(float)(rand()%255)/255;
            pixels[y*width+x].g=(float)(rand()%255)/255;
            pixels[y*width+x].b=(float)(rand()%255)/255;
        }
    }
    o_pixels=pixels;
}
void MasterImage::createBlank(int r,int g,int b){
    for(int y=0;y<height;y++){
        for(int x=0;x<width;x++){
            pixels[y*width+x].r=(float)r/255;
            pixels[y*width+x].g=(float)g/255;
            pixels[y*width+x].b=(float)b/255;
        }
    }
    o_pixels=pixels;
}
void MasterImage::setColor(const Color& color,int x,int y){
    pixels[y*width+x].r=color.r;
    pixels[y*width+x].g=color.g;
    pixels[y*width+x].b=color.b;
}
Color MasterImage::getColorSeq(int x, int y){
    return pixels[abs(y-height+1)*width+x];
}
Color MasterImage::getColor(int x, int y){
    return pixels[y*width+x];
}
void MasterImage::read(str path){
    Image im(0,0,0,0,1);
    im.read(path);
    im.resize(width,height);
    for(int x=0;x<width;x++){
        for(int y=0;y<height;y++){
            o_pixels[y*width+x]=im.getColorSeq(x,y);
        }
    }
    
}
void MasterImage::imposeText(str text){
    
}