LRESULT CALLBACK wndProc(HWND wnd,UINT msg,WPARAM w,LPARAM l){
    switch(msg){
        case WM_DESTROY:{
            PostQuitMessage(0);
            return 0;
        }
    }
    return DefWindowProc(wnd,msg,w,l);
}
HWND createWindow(HINSTANCE inst, int width,int height){
    LPCSTR  WND_CLASS[20]  = {"Sample Window Class"};
    WNDCLASS wc = { };        // create a WNDCLASSEX struct and zero it
    wc.hInstance =      inst;                   // give it our program instance
    wc.lpfnWndProc =    wndProc;                // tell it to use our wndProc function to handle messages
    wc.lpszClassName =  *WND_CLASS;   // give this window class a name.

    RegisterClass(&wc);           // register our window class with Windows

    return CreateWindowEx(
        0,                              // Optional window styles.
        *WND_CLASS,                      // Window class
        NULL,                           // Window text
        WS_OVERLAPPEDWINDOW,            // Window stylez 
        // Size and position
        0, 0, width, height,
        NULL,       // Parent window    
        NULL,       // Menu
        inst,       // Instance handle
        NULL        // Additional application data
    );          // no extra parameter
}
int W_NORM(float a){return int((float)WINDOW_WIDTH*a);}
int H_NORM(float a){return int((float)WINDOW_HEIGHT*a);}