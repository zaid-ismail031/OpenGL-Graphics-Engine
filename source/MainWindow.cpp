#include <windows.h>
#include <windowsx.h>
#include <thread>
#include <math.h>
#include <stdint.h>
#include <xinput.h>
#include <glad/glad.h>
#include "Shaders.h"
#include "Textures.h"
#include "ObjectLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool Running;
global_variable double ProgramElapsedTime;
global_variable bool mouseCaptured;

struct WindowDimensions
{
    int Width;
    int Height;
};

global_variable float CameraPositionZ = -3.0f;
global_variable float CameraPositionX = 0.0f;

global_variable glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
global_variable glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
global_variable glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

global_variable float yaw = -90;
global_variable float pitch = 0;


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

internal void UpdateCameraFront()
{
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
    glm::vec3 front;
    front.x = cos(glm::radians((float)yaw)) * cos(glm::radians((float)pitch));
    front.y = sin(glm::radians((float)pitch));
    front.z = sin(glm::radians((float)yaw)) * cos(glm::radians((float)pitch));
    cameraFront = glm::normalize(front);
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

internal void ThreeDimensionalRendering(Shader *shader) 
{
    // Create view matrix
    glm::mat4 view = glm::mat4(1.0f);
    // The LookAt function requires a position, target and up vector respectively
    view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Create projection matrix
    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    // Lighting calcs
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

    shader->setVec3("lightColor", lightColor);
    shader->setVec3("lightPos", lightPos);

    shader->setMat4("view", view);
    shader->setMat4("projection", projection);
}

//-------------------------------------------------------------------------------------------------------------

internal void HandleKeyboardInputs(WPARAM VKCode) 
{
    UpdateCameraFront();
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
        pitch += 5;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89)
            pitch = 89;
        if (pitch < -89)
            pitch = -89;
    }
    else if (VKCode == VK_DOWN) 
    {
        // Down Arrow Key
        pitch -= 5;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (pitch > 89)
            pitch = 89;
        if (pitch < -89)
            pitch = -89;
    }
    else if (VKCode == VK_LEFT) 
    {
        // Left Arrow Key
        yaw -= 5;
    }
    else if (VKCode == VK_RIGHT) 
    {
        // Right Arrow Key
        yaw += 5;
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

internal void SetMouseCaptured(HWND hwnd, bool captured)
{
    if (captured == mouseCaptured) 
    {
        return;   // ShowCursor is a counter, so don't double-call it
    }
    mouseCaptured = captured;

    if (captured)
    {
        RECT rect;
        GetClientRect(hwnd, &rect);
        MapWindowPoints(hwnd, nullptr, (POINT*)&rect, 2);   // client to screen coords
        ClipCursor(&rect);                                  // keep the cursor inside the window
        ShowCursor(FALSE);
    }
    else
    {
        ClipCursor(nullptr);
        ShowCursor(TRUE);
    }
}

//-------------------------------------------------------------------------------------------------------------

internal void MouseLook(float dx, float dy) 
{
    const float sensitivity = 0.1f;   // degrees per mouse count
    yaw += dx * sensitivity;
    pitch -= dy * sensitivity;        // raw Y grows downward, so subtract
    UpdateCameraFront();
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
        case WM_INPUT:
        {
            RAWINPUT raw;
            UINT size = sizeof(raw);
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &size, sizeof(RAWINPUTHEADER));

            if (mouseCaptured && raw.header.dwType == RIM_TYPEMOUSE && !(raw.data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
            {
                MouseLook((float)raw.data.mouse.lLastX, (float)raw.data.mouse.lLastY);
            }
            Result = DefWindowProc(hwnd, uMsg, wParam, lParam);   // required so Windows can clean up
        }
            break;
        case WM_LBUTTONDOWN:
        {
            SetMouseCaptured(hwnd, true);
        }
            break;
        case WM_RBUTTONDOWN:
        {
            
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
            if (wParam == VK_ESCAPE) 
            {
                SetMouseCaptured(hwnd, false);
            }
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
            ValidateRect(hwnd, NULL);

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
    WindowClass.lpszClassName = "OpenGL Graphics Engine";

    if (RegisterClassA(&WindowClass)) 
    {
        HWND WindowHandle = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "OpenGL Graphics Engine",
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
            RAWINPUTDEVICE rid = {};
            rid.usUsagePage = 0x01;   // generic desktop controls
            rid.usUsage     = 0x02;   // mouse
            rid.hwndTarget  = WindowHandle;
            RegisterRawInputDevices(&rid, 1, sizeof(rid));

            HDC DeviceContext = GetDC(WindowHandle);

            WindowDimensions Dimensions = GetWindowDimensions(WindowHandle);
            OpenGLFrameBufferSizeCallback(WindowHandle, Dimensions.Width, Dimensions.Height);

            Texture diffuseTexture("../data/models/eyeball/textures/Eye_D.jpg");
            Texture specularTexture("../data/models/eyeball/textures/Eye_D.jpg");
            Texture bumpTexture("../data/models/eyeball/textures/Eye_N.jpg");
            Texture emissiveTexture("../data/models/eyeball/textures/Eye_D.jpg");

            Shader shader("../data/shaders/Eyeballs.vert", "../data/shaders/Eyeballs.frag");

            ObjectLoader object("../data/models/eyeball/eyeball.obj", false);

            object.loadAllMeshes();
            
            glEnable(GL_DEPTH_TEST);
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
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // bind texture
                glActiveTexture(GL_TEXTURE0);
                diffuseTexture.use();
                glActiveTexture(GL_TEXTURE1);
                specularTexture.use();
                glActiveTexture(GL_TEXTURE2);
                bumpTexture.use();
                glActiveTexture(GL_TEXTURE3);
                emissiveTexture.use();

                // activate shader
                shader.use();

                // 3D projection
                ThreeDimensionalRendering(&shader);
                
                for (int i = 1; i < object.bufferVector.size(); i++)
                {
                    objl::Material material = object.loadMaterial(i);

                    glm::vec3 ambientColor(material.Ka.X, material.Ka.Y, material.Ka.Z);
                    glm::vec3 diffuseColor(material.Kd.X, material.Kd.Y, material.Kd.Z);
                    glm::vec3 specularColor(material.Ks.X, material.Ks.Y, material.Ks.Z);
                    glm::vec3 emissiveColor(material.Ke.X, material.Ke.Y, material.Ke.Z);

                    shader.setVec3("material.Ka", ambientColor);
                    shader.setVec3("material.Kd", diffuseColor);
                    shader.setVec3("material.Ks", specularColor);
                    shader.setVec3("material.Ke", emissiveColor);

                    shader.setFloat("material.Ns", material.Ns);
                    shader.setInt("map_Kd", diffuseTexture.textureID);
                    shader.setInt("map_Ks", specularTexture.textureID);
                    shader.setInt("map_Bump", bumpTexture.textureID);
                    shader.setInt("map_Ke", emissiveTexture.textureID);

                    glBindVertexArray(object.bufferVector[i].VAO);
                    glm::mat4 model = glm::mat4(1.0f);
                    float angle = 20.0f;
                    model = glm::rotate(model, (float)ProgramElapsedTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
                    shader.setMat4("model", model);

                    if (object.normalMapping) 
                    {
                        glDrawArrays(GL_TRIANGLES, 0, object.bufferVector[i].vertexCount);
                    }
                    else 
                    {
                        glDrawElements(GL_TRIANGLES, object.bufferVector[i].indexCount, GL_UNSIGNED_INT, 0);
                    }
                }
                
                glBindVertexArray(0);

                // Swap the front and back buffers
                SwapBuffers(DeviceContext);

                QueryPerformanceCounter(&end);
                ProgramElapsedTime += (double) (end.QuadPart - start.QuadPart) / frequency.QuadPart;
            }

            ReleaseDC(WindowHandle, DeviceContext);

            for (int i = 0; i < object.bufferVector.size(); i++) 
            {
                glDeleteVertexArrays(1, &object.bufferVector[i].VAO);
                glDeleteBuffers(1, &object.bufferVector[i].VBO);
            }
            
            glDeleteProgram(shader.ID);
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