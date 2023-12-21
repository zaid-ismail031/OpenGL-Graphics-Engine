#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

struct OffScreenBuffer
{
    BITMAPINFO Info;
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

global_variable bool Running;
global_variable OffScreenBuffer BackBuffer;

struct WindowDimensions
{
    int Width;
    int Height;
};

//-------------------------------------------------------------------------------------------------------------

WindowDimensions GetWindowDimensions(HWND Window) 
{
    WindowDimensions Dimensions;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Dimensions.Width = ClientRect.right - ClientRect.left;
    Dimensions.Height = ClientRect.bottom - ClientRect.top;

    return Dimensions;
}

//-------------------------------------------------------------------------------------------------------------

internal void RenderWeirdGradient(OffScreenBuffer Buffer, int XOffset, int YOffset) 
{
    uint8_t *Row = (uint8_t *)Buffer.Memory;

    for (int Y = 0; Y < Buffer.Height; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;

        for (int X = 0; X < Buffer.Width; ++X) 
        {
            uint8_t Blue = X + XOffset;
            uint8_t Green = Y + YOffset;

            *Pixel++ = (Green << 8 )| Blue;
        }

        Row += Buffer.Pitch;
    }
}

//-------------------------------------------------------------------------------------------------------------

internal void ResizeDIBSection(OffScreenBuffer *Buffer, int Width, int Height) 
{
    if (Buffer->Memory) 
    {
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;

    Buffer->Info.bmiHeader.biSize = sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth = Buffer->Width;
    Buffer->Info.bmiHeader.biHeight = -Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes = 1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression = BI_RGB;
    
    int BitmapMemorySize = (Buffer->Width * Buffer->Height) * Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Buffer->Width * Buffer->BytesPerPixel;
}

//-------------------------------------------------------------------------------------------------------------

internal void Win32UpdateWindow(OffScreenBuffer Buffer, HDC DeviceContext, int ClientWidth, int ClientHeight) 
{
    StretchDIBits(
        DeviceContext,
        0, 0, ClientWidth, ClientHeight,
        0, 0, Buffer.Width, Buffer.Height,
        Buffer.Memory,
        &Buffer.Info,
        DIB_RGB_COLORS, SRCCOPY
    );
}


//-------------------------------------------------------------------------------------------------------------

LRESULT CALLBACK WindowProc(HWND hwnd,
UINT uMsg,
WPARAM wParam,
LPARAM lParam) 
{
    LRESULT Result = 0;

    switch(uMsg) 
    {
        case WM_SIZE:
        {   
            
        }
            break;
        case WM_DESTROY:
        {
            Running = false;
        }
            break;
        case WM_CLOSE: 
        {
            Running = false;
        }
            break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }
            break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(hwnd, &Paint);

            WindowDimensions Dimensions = GetWindowDimensions(hwnd);

            Win32UpdateWindow(BackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height);
            EndPaint(hwnd, &Paint);
            Result = 0;
        }
            break;
        default:
        {
            OutputDebugStringA("default\n");
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
            break;
    }

    return Result;
}

//-------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpCmdLine,
                    int nCmdShow) 
{
    WNDCLASS WindowClass = {};

    ResizeDIBSection(&BackBuffer, 1280, 720);

    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "Windows App";

    if (RegisterClassA(&WindowClass)) 
    {
        HWND WindowHandle = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "Windows App",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            hInstance,
            0
        );

        if (WindowHandle != NULL) 
        {
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while (Running) 
            {
                MSG Message;
                while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) 
                {
                    if (Message.message == WM_QUIT) 
                    {
                        Running = false;
                    }

                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                
                PAINTSTRUCT Paint;
                HDC DeviceContext = GetDC(WindowHandle);
            
                WindowDimensions Dimensions = GetWindowDimensions(WindowHandle);

                Win32UpdateWindow(BackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height);

                RenderWeirdGradient(BackBuffer, XOffset, YOffset);
                ReleaseDC(WindowHandle, DeviceContext);

                XOffset++;
                YOffset++;
            }
        }
        else 
        {
            //TODO
        }
    }
    else 
    {
        //TODO
    }

    return 0;
}

//-------------------------------------------------------------------------------------------------------------