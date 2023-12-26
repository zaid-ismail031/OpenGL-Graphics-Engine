#include <windows.h>
#include <math.h>
#include <stdint.h>
#include <xinput.h>
#include <glad/glad.h>
#include "shaders.h"
#include "text.h"

#define internal static
#define local_persist static
#define global_variable static

// WIN32 STUFF IS THANKS TO MOLLY ROCKET'S HANDMADE HERO WEB SERIES AT https://handmadehero.org/ 
// OPENGL STUFF IS THANKS TO THE BOOK "LEARN OPENGL - GRAPHICS PROGRAMMING" BY JOEY DE VRIES

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
global_variable double ProgramElapsedTime;

struct WindowDimensions
{
    int Width;
    int Height;
};

global_variable float Vertices[] = 
{
    -0.5f, -0.5f, 0.0f,
    0.5f, -0.5f, 0.0f,
    0.0f, 0.5f, 0.0f
};

global_variable float Rect_Vertices_Overlap[] 
{
    // first triangle
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, 0.5f, 0.0f, // top left
    // second triangle
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
};

global_variable float Rect_Vertices[] = 
{
    0.5f, 0.5f, 0.0f, // top right
    0.5f, -0.5f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f // top left
};

global_variable unsigned int Indices[] = 
{   // note that we start from 0!
    0, 1, 3, // first triangle
    1, 2, 3 // second triangle
};

global_variable float Vertices_With_Color[] =
{
    // positions       // colors
    0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f, 0.5f, 0.0f,   0.0f, 0.0f, 1.0f // top
};

struct OpenGLData 
{
    unsigned int EBO;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int shaderProgram;
};

const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *vertexShaderSourceWithColor = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos, 1.0);\n"
" ourColor = aColor;\n"
"}\0";

const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

const char *uniformFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec4 ourColor;\n"
"void main()\n"
"{\n"
" FragColor = ourColor;\n"
"}\0";

const char *fragShaderForColorVertex = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
" FragColor = vec4(ourColor, 1.0);\n"
"}\0";

#define XINPUTGETSTATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE* pState)
#define XINPUTSETSTATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION* pVibration)

typedef XINPUTGETSTATE(XGetState);
typedef XINPUTSETSTATE(XSetState);

XINPUTGETSTATE(XInputGetStateStub)
{
    return 0;
}

XINPUTSETSTATE(XInputSetStateStub)
{
    return 0;
}

global_variable XGetState *DyXInputGetState = XInputGetStateStub;
global_variable XSetState *DyXInputSetState = XInputSetStateStub;

//-------------------------------------------------------------------------------------------------------------

void *GetAnyGLFuncAddress(const char *name)
{
    void *p = (void *)wglGetProcAddress(name);
    if(p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1) )
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        p = (void *)GetProcAddress(module, name);
    }

    return p;
}

//-------------------------------------------------------------------------------------------------------------

internal void Win32LoadXInput(void) 
{
    HMODULE XInputLibrary = LoadLibraryA("xinput1_3.dll");

    if (XInputLibrary) 
    {
        // GetProcAddress loads the address of the function in xinput.dll
        DyXInputGetState = (XGetState *)GetProcAddress(XInputLibrary, "XInputGetState");
        DyXInputSetState = (XSetState *)GetProcAddress(XInputLibrary, "XInputSetState");
    }
}

//-------------------------------------------------------------------------------------------------------------

internal WindowDimensions GetWindowDimensions(HWND Window) 
{
    WindowDimensions Dimensions;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);

    Dimensions.Width = ClientRect.right - ClientRect.left;
    Dimensions.Height = ClientRect.bottom - ClientRect.top;

    return Dimensions;
}


//-------------------------------------------------------------------------------------------------------------

internal void OpenGLFrameBufferSizeCallback(HWND Window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//-------------------------------------------------------------------------------------------------------------

internal void OpenGLVertexArrayObject(OpenGLData *data, float *VerticesInput, size_t VerticesSize) 
{
    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &data->VAO);
    glGenBuffers(1, &data->VBO);

    glBindVertexArray(data->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);

    glBufferData(GL_ARRAY_BUFFER, VerticesSize, (const void *)VerticesInput, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(0));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------

internal void OpenGLElementBufferObject(OpenGLData *data) 
{
    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &data->VAO);
    glGenBuffers(1, &data->VBO);

    glBindVertexArray(data->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(Rect_Vertices), Rect_Vertices, GL_STATIC_DRAW);

    // Element Buffer Object
    glGenBuffers(1, &data->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

//-------------------------------------------------------------------------------------------------------------

internal void OpenGLCompileShaders(OpenGLData *data, const char *VertexShaderSource, const char *FragmentShaderSource) 
{
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &VertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &FragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // Link shaders
    data->shaderProgram = glCreateProgram();
    glAttachShader(data->shaderProgram, vertexShader);
    glAttachShader(data->shaderProgram, fragmentShader);
    glLinkProgram(data->shaderProgram);

    glUseProgram(data->shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

//-------------------------------------------------------------------------------------------------------------

internal void AlterFragmentShader(OpenGLData *data, const char *FragmentShaderSource) 
{
    int vertexColorLocation = glGetUniformLocation(data->shaderProgram, "ourColor");

    if (vertexColorLocation != -1) 
    {
        float greenValue = (sin(ProgramElapsedTime) / 2) + 0.5;
        glUseProgram(data->shaderProgram);
        glUniform4f(vertexColorLocation, 0, greenValue, 0, 1);
    }
}

//-------------------------------------------------------------------------------------------------------------

internal void HandleKeyboardInputs(WPARAM VKCode) 
{
    if (VKCode == 0x57) 
    {
        // W Key
    }
    else if (VKCode == 0x41) 
    {
        // A Key
    }
    else if (VKCode == 	0x53) 
    {
        // S Key
    }
    else if (VKCode == 0x44) 
    {
        // D Key
    }
    else if (VKCode == 0x51) 
    {
        // Q Key
    }
    else if (VKCode == 0x45) 
    {
        // E Key
    }
    else if (VKCode == VK_UP) 
    {
        // Up Arrow Key
    }
    else if (VKCode == VK_DOWN) 
    {
        // Down Arrow Key
    }
    else if (VKCode == VK_LEFT) 
    {
        // Left Arrow Key
    }
    else if (VKCode == VK_RIGHT) 
    {
        // Right Arrow Key
    }
    else if (VKCode == VK_ESCAPE) 
    {
        // Escape Key
    }
    else if (VKCode == VK_SPACE) 
    {
        // Spacebar
    }
}


//-------------------------------------------------------------------------------------------------------------

internal void RenderWeirdGradient(OffScreenBuffer *Buffer, int XOffset, int YOffset) 
{
    uint8_t *Row = (uint8_t *)Buffer->Memory;

    for (int Y = 0; Y < Buffer->Height; ++Y)
    {
        uint32_t *Pixel = (uint32_t *)Row;

        for (int X = 0; X < Buffer->Width; ++X) 
        {
            uint8_t Blue = X + XOffset;
            uint8_t Green = Y + YOffset;
            uint8_t Red = Y + YOffset;

            *Pixel++ = (Red << 16) |(Green << 8 )| Blue;

            // Example of a bit-shift using binary numbers
            // 01001011 00000000 00000000  (Red shifted)
            // 00000000 01001011 00000000  (Green shifted)
            // 00000000 00000000 00011001  (Blue)
            // ---------------------------
            // 01001011 01001011 00011001  (Result)
        }

        Row += Buffer->Pitch;
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

internal void Win32UpdateWindow(OffScreenBuffer *Buffer, HDC DeviceContext, int ClientWidth, int ClientHeight) 
{
    StretchDIBits(
        DeviceContext,
        0, 0, ClientWidth, ClientHeight,
        0, 0, Buffer->Width, Buffer->Height,
        Buffer->Memory,
        &Buffer->Info,
        DIB_RGB_COLORS, SRCCOPY
    );
}

//-------------------------------------------------------------------------------------------------------------

internal LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
    LRESULT Result = 0;

    switch(uMsg) 
    {
        case WM_SIZE:
        {   
            WindowDimensions Dimensions = GetWindowDimensions(hwnd);
            OpenGLFrameBufferSizeCallback(hwnd, Dimensions.Width, Dimensions.Height);
        }
            break;
        case WM_CREATE:
        {
            PIXELFORMATDESCRIPTOR pfd =
            {
                sizeof(PIXELFORMATDESCRIPTOR),
                1,
                PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
                PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
                32,                   // Colordepth of the framebuffer.
                0, 0, 0, 0, 0, 0,
                0,
                0,
                0,
                0, 0, 0, 0,
                24,                   // Number of bits for the depthbuffer
                8,                    // Number of bits for the stencilbuffer
                0,                    // Number of Aux buffers in the framebuffer.
                PFD_MAIN_PLANE,
                0,
                0, 0, 0
            };

            HDC DeviceContext = GetDC(hwnd);
            int pf = ChoosePixelFormat(DeviceContext, &pfd);
            SetPixelFormat(DeviceContext, pf, &pfd);
            HGLRC hglrc = wglCreateContext(DeviceContext);
            wglMakeCurrent(DeviceContext, hglrc);
            
            if (!gladLoadGLLoader((GLADloadproc)GetAnyGLFuncAddress))
            {
                // Handle the error
                MessageBoxA(0, "Failed to initialize GLAD", "Error", MB_ICONERROR);
                return -1;
            }
            //MessageBoxA(0,(char*)glGetString(GL_VERSION), "OPENGL VERSION",0);
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
        case WM_SYSKEYDOWN:
        {

        }
            break;
        case WM_SYSKEYUP:
        {
            
        }
            break;
        case WM_KEYDOWN:
        {
            
        }
            break;
        case WM_KEYUP:
        {
            // lParam is a 32-bit bit-field and we want to check if the 30th bit is set
            // So 1 bit shifted by 30 will do this:
            // Before shift: 00000000 00000000 00000000 00000001 (binary representation of 1 as a 32-bit integer)
            // After shift: 01000000 00000000 00000000 00000000

            bool WasDown = ((lParam & (1 << 30)) != 0);
            bool IsDown = ((lParam & (1 << 31)) == 0);
            if (WasDown != IsDown) 
            {
                HandleKeyboardInputs(wParam);
            }
        }
            break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(hwnd, &Paint);

            WindowDimensions Dimensions = GetWindowDimensions(hwnd);

            Win32UpdateWindow(&BackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height);
            EndPaint(hwnd, &Paint);
            Result = 0;
        }
            break;
        default:
        {
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
            break;
    }

    return Result;
}

//-------------------------------------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) 
{
    Win32LoadXInput();
    WNDCLASS WindowClass = {};
    //ResizeDIBSection(&BackBuffer, 1280, 720);
    
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
            HDC DeviceContext = GetDC(WindowHandle);
            
            WindowDimensions Dimensions = GetWindowDimensions(WindowHandle);
            OpenGLFrameBufferSizeCallback(WindowHandle, Dimensions.Width, Dimensions.Height);

            OpenGLData data;
            Shader shader("../data/shaders/ColorTriangle.vert", "../data/shaders/ColorTriangle.frag");
            //OpenGLCompileShaders(&data, vertexShaderSourceWithColor, fragShaderForColorVertex);
            OpenGLVertexArrayObject(&data, Vertices_With_Color, sizeof(Vertices_With_Color));
            //OpenGLElementBufferObject(&data);

            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while (Running) 
            {
                LARGE_INTEGER frequency, start, end;

                QueryPerformanceFrequency(&frequency);
                QueryPerformanceCounter(&start);

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
                
                for (DWORD ControllerIndex = 0; ControllerIndex < XUSER_MAX_COUNT; ++ControllerIndex) 
                {
                    XINPUT_STATE ControllerState;

                    if(DyXInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS) 
                    {
                        XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;

                        bool DPadUp = (Pad->wButtons && XINPUT_GAMEPAD_DPAD_UP); 
                        bool DPadDown = (Pad->wButtons && XINPUT_GAMEPAD_DPAD_DOWN); 
                        bool DPadLeft = (Pad->wButtons && XINPUT_GAMEPAD_DPAD_LEFT); 
                        bool DPadRight = (Pad->wButtons && XINPUT_GAMEPAD_DPAD_RIGHT); 

                        bool Start = (Pad->wButtons && XINPUT_GAMEPAD_START); 
                        bool Select = (Pad->wButtons && XINPUT_GAMEPAD_BACK); 

                        bool L1 = (Pad->wButtons && XINPUT_GAMEPAD_LEFT_SHOULDER); 
                        bool R1 = (Pad->wButtons && XINPUT_GAMEPAD_RIGHT_SHOULDER); 

                        bool AButton = (Pad->wButtons && XINPUT_GAMEPAD_A); 
                        bool BButton = (Pad->wButtons && XINPUT_GAMEPAD_B); 
                        bool XButton = (Pad->wButtons && XINPUT_GAMEPAD_X); 
                        bool YButton = (Pad->wButtons && XINPUT_GAMEPAD_Y);

                        int16_t StickX = Pad->sThumbLX;
                        int16_t StickY = Pad->sThumbLY;
                    }
                    else 
                    {
                        // Controller not available
                    }
                }

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);
                
                //glUseProgram(data.shaderProgram);
                shader.use();
                glBindVertexArray(data.VAO);
                // Draw a triangle from the vertices
                glDrawArrays(GL_TRIANGLES, 0, 3);

                //AlterFragmentShader(&data, uniformFragmentShaderSource);

                // Draw a rectangle from the vertices
                //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                glBindVertexArray(0);

                // Swap the front and back buffers
                SwapBuffers(DeviceContext);

                //WindowDimensions Dimensions = GetWindowDimensions(WindowHandle);
                
                //RenderWeirdGradient(&BackBuffer, XOffset, YOffset);
                //Win32UpdateWindow(&BackBuffer, DeviceContext, Dimensions.Width, Dimensions.Height);
                
                //XOffset++;
                //YOffset++;

                QueryPerformanceCounter(&end);
                ProgramElapsedTime += (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
            }

            ReleaseDC(WindowHandle, DeviceContext);
            glDeleteVertexArrays(1, &data.VAO);
            glDeleteBuffers(1, &data.VBO);
            glDeleteProgram(data.shaderProgram);
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