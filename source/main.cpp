#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <thread>
#include <math.h>
#include <stdint.h>
#include <xinput.h>
#include <glad/glad.h>
#include "shaders.h"
#include "text.h"
#include "textures.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
global_variable bool mouseCaptured;


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

global_variable float Rectangle_With_Texture[] =
{
    // positions       // colors    // texture coords
    0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f // top left
};

float Cube_Vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

// world space positions of our cubes
glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3( 2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3( 1.3f, -2.0f, -2.5f),
    glm::vec3( 1.5f,  2.0f, -2.5f),
    glm::vec3( 1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

global_variable float CameraPositionZ = -3.0f;
global_variable float CameraPositionX = 0.0f;

global_variable glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
global_variable glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
global_variable glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

global_variable short yaw = -90;
global_variable short pitch = 0;

struct OpenGLData 
{
    unsigned int EBO;
    unsigned int VBO;
    unsigned int VAO;
    unsigned int shaderProgram;
};

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

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);s
}

//-------------------------------------------------------------------------------------------------------------

internal void OpenGLElementBufferObject(OpenGLData *data, float *VerticesInput, size_t VerticesSize, unsigned int *IndicesInput, size_t IndicesSize) 
{
    // Vertex Buffer Object and Vertex Array Object
    glGenVertexArrays(1, &data->VAO);
    glGenBuffers(1, &data->VBO);

    glBindVertexArray(data->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, data->VBO);

    glBufferData(GL_ARRAY_BUFFER, VerticesSize, (const void *)VerticesInput, GL_STATIC_DRAW);

    // Element Buffer Object
    glGenBuffers(1, &data->EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,  IndicesSize, (const void *)IndicesInput, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

//-------------------------------------------------------------------------------------------------------------

internal void ThreeDimensionalRendering(Shader *shader) 
{
    //model = glm::rotate(model, (float)ProgramElapsedTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

    float camX = sin(ProgramElapsedTime) * 10.0f;
    float camZ = cos(ProgramElapsedTime) * 10.0f;

    // Create view matrix
    glm::mat4 view = glm::mat4(1.0f);
    // The LookAt function requires a position, target and up vector respectively
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    //view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

    // Create projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

//-------------------------------------------------------------------------------------------------------------

internal void HandleKeyboardInputs(WPARAM VKCode) 
{
    const float cameraSpeed = 0.1f; // adjust accordingly
    if (VKCode == 0x57) 
    {
        // W Key
        cameraPos += cameraSpeed * cameraFront;
    }
    else if (VKCode == 0x41) 
    {
        // A Key
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    else if (VKCode == 	0x53) 
    {
        // S Key
        cameraPos -= cameraSpeed * cameraFront;
    }
    else if (VKCode == 0x44) 
    {
        // D Key
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
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
        pitch += 10;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89)
            pitch = 89;
        if (pitch < -89)
            pitch = -89;

        float floatYaw = (float)yaw;
        float floatPitch = (float)pitch;

        glm::vec3 front;
        front.x = cos(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        front.y = sin(glm::radians(floatPitch));
        front.z = sin(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        cameraFront = glm::normalize(front);
    }
    else if (VKCode == VK_DOWN) 
    {
        // Down Arrow Key
        pitch -= 10;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89)
            pitch = 89;
        if (pitch < -89)
            pitch = -89;

        float floatYaw = (float)yaw;
        float floatPitch = (float)pitch;

        glm::vec3 front;
        front.x = cos(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        front.y = sin(glm::radians(floatPitch));
        front.z = sin(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        cameraFront = glm::normalize(front);
    }
    else if (VKCode == VK_LEFT) 
    {
        // Left Arrow Key
        yaw -= 10;
        float floatYaw = (float)yaw;
        float floatPitch = (float)pitch;

        glm::vec3 front;
        front.x = cos(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        front.y = sin(glm::radians(floatPitch));
        front.z = sin(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        cameraFront = glm::normalize(front);
    }
    else if (VKCode == VK_RIGHT) 
    {
        // Right Arrow Key
        yaw += 10;
        float floatYaw = (float)yaw;
        float floatPitch = (float)pitch;

        glm::vec3 front;
        front.x = cos(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        front.y = sin(glm::radians(floatPitch));
        front.z = sin(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
        cameraFront = glm::normalize(front);
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

internal void MouseLook(HWND *hwnd, short *lastX, short *lastY) 
{
    TRACKMOUSEEVENT MouseEvent;

    MouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    MouseEvent.dwFlags = TME_HOVER;
    MouseEvent.hwndTrack = *hwnd;
    MouseEvent.dwHoverTime = HOVER_DEFAULT;

    bool IsTrackingMouse = TrackMouseEvent(&MouseEvent);

    POINT cursorPosition;
    GetCursorPos(&cursorPosition);

    //short xPos = GET_X_LPARAM(lParam); 
    //short yPos = GET_Y_LPARAM(lParam);
    long xPos = cursorPosition.x;
    long yPos = cursorPosition.y;

    if (fabs(xPos - *lastX) < 0.1 || fabs(yPos - *lastY) < 0.1) 
    {
        return;
    }

    short originalX = 400;
    short originalY = 300;

    const float sensitivity = 0.01f;

    short xOffset = (xPos - originalX) * sensitivity;
    short yOffset = (originalY - yPos) * sensitivity;

    *lastX = (short)xPos;
    *lastY = (short)yPos;

    yaw += xOffset;
    pitch += yOffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89)
        pitch = 89;
    if (pitch < -89)
        pitch = -89;
    
    float floatYaw = (float)yaw;
    float floatPitch = (float)pitch;

    glm::vec3 front;
    front.x = cos(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
    front.y = sin(glm::radians(floatPitch));
    front.z = sin(glm::radians(floatYaw)) * cos(glm::radians(floatPitch));
    cameraFront = glm::normalize(front);
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
        case WM_LBUTTONDOWN:
        {
            //mouseCaptured = SetCapture(hwnd); // Capture mouse
        }
            break;
        case WM_RBUTTONDOWN:
        {
            //mouseCaptured = false; // Capture mouse
        }
            break;
        case WM_MOUSEMOVE:
        {

        }
            break;
        case WM_MOUSEHOVER:
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
            HandleKeyboardInputs(wParam);
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
                //HandleKeyboardInputs(wParam);
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
            // Initialize last MOUSE X and Y to centre of screen since we are rendering at 800x600
            short lastMouseX = 400; 
            short lastMouseY = 300;
            HDC DeviceContext = GetDC(WindowHandle);

            WindowDimensions Dimensions = GetWindowDimensions(WindowHandle);
            OpenGLFrameBufferSizeCallback(WindowHandle, Dimensions.Width, Dimensions.Height);

            OpenGLData data;
            Texture texture("../data/textures/darwin.jpg");
            Shader shader("../data/shaders/3D.vert", "../data/shaders/3D.frag");
            OpenGLVertexArrayObject(&data, Cube_Vertices, sizeof(Cube_Vertices));
            //OpenGLElementBufferObject(&data, Rectangle_With_Texture, sizeof(Rectangle_With_Texture), Indices, sizeof(Indices));
            glEnable(GL_DEPTH_TEST);
            Running = true;
            while (Running) 
            {
                //if (mouseCaptured) {
                //    MouseLook(&WindowHandle, &lastMouseX, &lastMouseY);
                //}
                
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
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                
                // bind texture
                glActiveTexture(GL_TEXTURE0);
                texture.use();

                // activate shader
                shader.use();

                // 3D projection
                ThreeDimensionalRendering(&shader);
                
                glBindVertexArray(data.VAO);
                // Draw a triangle from the vertices
                // Create model matrix
                for (unsigned int i = 0; i < 10; i++) 
                {
                    glm::mat4 model = glm::mat4(1.0f);
                    model = glm::translate(model, cubePositions[i]);
                    float angle = 20.0f * i;
                    //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                    model = glm::rotate(model, (float)ProgramElapsedTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
                    shader.setMat4("model", model);

                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                // Draw a rectangle from the vertices
                //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                glBindVertexArray(0);

                // Swap the front and back buffers
                SwapBuffers(DeviceContext);

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