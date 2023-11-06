#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "include.h"

const char* AppClass = "AppClass";
const char* AppName = "Programs";
HWND hwnd = NULL;

LPDIRECT3D9 g_pD3D = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;
D3DPRESENT_PARAMETERS g_d3dpp = {};
int WindowWidth = 550;
int WindowHeight = 340;
bool open = true;

int testSlider = 0;
bool testBool = false;

HRESULT CreateDeviceD3D(HWND hWnd) {
    g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!g_pD3D) return E_FAIL;

    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return E_FAIL;

    return S_OK;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    if (g_pd3dDevice->Reset(&g_d3dpp) == D3DERR_INVALIDCALL) IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) return 0;
        break;
    case WM_NCHITTEST:
    {
        ImVec2 mousePos = ImGui::GetMousePos();
        if (mousePos.y < 25 && mousePos.x < WindowWidth - 25)
        {
            LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
            if (hit == HTCLIENT) hit = HTCAPTION;
            return hit;
        }
        else break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void RenderTheme()
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImFontConfig font;
    font.FontDataOwnedByAtlas = false;
    //io.Fonts->AddFontFromMemoryTTF((void*)fontData, sizeof(fontData), 11.0f, &font);

    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_MenuBarBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Border
    colors[ImGuiCol_Border] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_BorderShadow] = ImVec4{ 0.0f, 0.0f, 0.0f, 0.24f };

    // Text
    colors[ImGuiCol_Text] = ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f };
    colors[ImGuiCol_TextDisabled] = ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f };

    // Headers
    colors[ImGuiCol_Header] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{ 0.13f, 0.13f, 0.17, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_CheckMark] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };

    // Popups
    colors[ImGuiCol_PopupBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 0.92f };

    // Slider
    colors[ImGuiCol_SliderGrab] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.54f };
    colors[ImGuiCol_SliderGrabActive] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.54f };

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.13f, 0.13, 0.17, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.24, 0.24f, 0.32f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.2f, 0.22f, 0.27f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };

    // Scrollbar
    colors[ImGuiCol_ScrollbarBg] = ImVec4{ 0.1f, 0.1f, 0.13f, 1.0f };
    colors[ImGuiCol_ScrollbarGrab] = ImVec4{ 0.16f, 0.16f, 0.21f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4{ 0.19f, 0.2f, 0.25f, 1.0f };
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4{ 0.24f, 0.24f, 0.32f, 1.0f };

    // Seperator
    colors[ImGuiCol_Separator] = ImVec4{ 0.44f, 0.37f, 0.61f, 1.0f };
    colors[ImGuiCol_SeparatorHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 1.0f };
    colors[ImGuiCol_SeparatorActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 1.0f };

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{ 0.44f, 0.37f, 0.61f, 0.29f };
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{ 0.74f, 0.58f, 0.98f, 0.29f };
    colors[ImGuiCol_ResizeGripActive] = ImVec4{ 0.84f, 0.58f, 1.0f, 0.29f };

    auto& style = ImGui::GetStyle();
    style.TabRounding = 4;
    style.ScrollbarRounding = 9;
    style.WindowRounding = 7;
    style.GrabRounding = 3;
    style.FrameRounding = 3;
    style.PopupRounding = 4;
    style.ChildRounding = 4;

    style.FrameBorderSize = 0;
}

void RenderGui()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Once);
    {
        ImGui::Begin(AppName, &open, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::SetCursorPos({ 26.f,37.f });
        ImGui::PushItemWidth(56.000000);
        ImGui::Text("Browsers");
        ImGui::PopItemWidth();

        ImGui::SetCursorPos({ 19.f,66.f });

        ImGui::BeginChild("child0", { 207.f,88.f }, true);

        ImGui::SetCursorPos({ 17.f,15.f });
        if (ImGui::Button("firefox", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.mozilla.org/ru/firefox/new/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 112.f,15.f });
        if (ImGui::Button("chrome", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.google.com/intl/ru/chrome/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 112.f,48.f });
        if (ImGui::Button("opera", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.opera.com/ru", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 17.f,48.f });
        if (ImGui::Button("opera gx", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.opera.com/ru/gx", NULL, NULL, SW_SHOWDEFAULT);
        }

        ImGui::EndChild();

        ImGui::SetCursorPos({ 26.f,181.f });
        ImGui::PushItemWidth(35.000000);
        ImGui::Text("Other");
        ImGui::PopItemWidth();
        ImGui::SetCursorPos({ 260.f,37.f });
        ImGui::PushItemWidth(63.000000);
        ImGui::Text("Launchers");
        ImGui::PopItemWidth();

        ImGui::SetCursorPos({ 250.f,66.f });

        ImGui::BeginChild("child1", { 207.f,88.f }, true);

        ImGui::SetCursorPos({ 15.f,15.f });
        if (ImGui::Button("discord", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://discord.com/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 109.f,15.f });
        if (ImGui::Button("telegram", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://desktop.telegram.org/?setln=ru", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 15.f,48.f });
        if (ImGui::Button("steam", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://store.steampowered.com/?l=russian", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 109.f,48.f });
        if (ImGui::Button("epic games", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://store.epicgames.com/ru/", NULL, NULL, SW_SHOWDEFAULT);
        }

        ImGui::EndChild();

        ImGui::SetCursorPos({ 19.f,210.f });

        ImGui::BeginChild("child2", { 442.f,88.f }, true);

        ImGui::SetCursorPos({ 17.f,15.f });
        if (ImGui::Button("radmin vpn", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.radmin-vpn.com/ru/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 112.f,15.f });
        if (ImGui::Button("obs studio", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://obsproject.com/ru", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 207.f,15.f });
        if (ImGui::Button("qbittorrent", { 85.f,19.f }))
        {
            ShellExecute(0, "open", "https://www.qbittorrent.org/download", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 305.f,15.f });
        if (ImGui::Button("cheat engine", { 92.f,19.f }))
        {
            ShellExecute(0, "open", "https://github.com/cheat-engine/cheat-engine#download", NULL, NULL, SW_SHOWDEFAULT);
        }

        ImGui::EndChild();

        ImGui::SetCursorPos({ 19.f,210.f });

        ImGui::BeginChild("child2", { 442.f,88.f }, true);

        ImGui::SetCursorPos({ 17.f,49.f });
        if (ImGui::Button("notepad++", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://notepad-plus-plus.org/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 112.f,49.f });
        if (ImGui::Button("mpc-hc", { 80.f,19.f }))
        {
            ShellExecute(0, "open", "https://mpc-hc.org/", NULL, NULL, SW_SHOWDEFAULT);
        }
        ImGui::SetCursorPos({ 207.f,49.f });
        if (ImGui::Button("extreme injector", { 118.f,19.f }))
        {
            ShellExecute(0, "open", "https://github.com/master131/ExtremeInjector", NULL, NULL, SW_SHOWDEFAULT);
        }

        ImGui::EndChild();
    }
    ImGui::End();
}

int main()
{
    RECT desktop;
    GetWindowRect(GetDesktopWindow(), &desktop);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, AppClass, NULL };

    RegisterClassEx(&wc);
    hwnd = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED, AppClass, AppName, WS_POPUP, (desktop.right / 2) - (WindowWidth / 2), (desktop.bottom / 2) - (WindowHeight / 2), WindowWidth, WindowHeight, 0, 0, wc.hInstance, 0);

    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, ULW_COLORKEY);

    if (CreateDeviceD3D(hwnd) < 0) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    RenderTheme();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        if (!open) ExitProcess(EXIT_SUCCESS);
        std::this_thread::sleep_for(std::chrono::milliseconds(8));

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowSize(ImVec2(WindowWidth, WindowHeight), ImGuiCond_Once);

        RenderGui();

        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}