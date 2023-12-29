#include "troicefile.h"

static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nShowCmd
)
{
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Troice Python Package Manager", WS_POPUP, 100, 100, 0, 0, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 15.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    ImFont* font_18 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 22.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    //ImFont* font_17 = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simsun.ttc", 20.0f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
        static bool loadMainWin = false;

        if (show_demo_window) {
            ImGui::Begin(u8"欢迎使用-Troice     “长街长,烟花繁,你挑灯回看.短亭短,红尘辗,我把萧再叹”", &show_demo_window, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize({ 637.0f,334.0f });
            ImGui::StyleColorsLight();

            style.FrameRounding = 6;
            static bool disable_mouse_wheel = false;
            static bool disable_menu = true;

            ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
            if (disable_mouse_wheel)
                window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
            if (!disable_menu)
                window_flags |= ImGuiWindowFlags_MenuBar;
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild("ChildR", ImVec2(610, 250), true, window_flags);
            if (!disable_menu && ImGui::BeginMenuBar())
            {
                if (ImGui::BeginMenu("Menu_"))
                {
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }

            static std::string Notice_ = "本产品为无稚(Troice)开发的Python包管理器\n"

                "注意事项:\n"
                "1.本工具仅为Python模块统一管理工具，不提供其他服务。\n"
                "2.若使用下载到的模块编写恶意程序造成的破坏与损失与作者无关。\n"
                "3.禁止修改本程序。\n"
                "4.此程序为免费程序，禁止交易！\n"
                "5.若使用内存修改工具修改本程序的任何内容(含条款)，请在24小时内复原！\n"
                "6.以上条款如有违反，作者有权追究责任。\n"
                "\n"
                "中华人民共和国计算机软件保护条例，根据《中华人民共和国著作权法》，\n"
                "为了保护计算机软件著作权人的权益而制定，自2002年1月1日起施行。\n"
                "第八条软件著作权人享有下列各项权利：\n"
                "(一)发表权，即决定软件是否公之于众的权利；\n"
                "(二)署名权，即表明开发者身份，在软件上署名的权利；\n"
                "(三)修改权，即对软件进行增补、删节，或者改变指令、语句顺序的权\n"
                "利；\n"
                "(四)复制权，即将软件制作一份或者多份的权利；\n"
                "(五)发行权，即以出售或者赠与方式向公众提供软件的原件或者复制件\n"
                "的权利；\n"
                "(六)出租权，即有偿许可他人临时使用软件的权利，但是软件不是出租\n"
                "的主要标的的除外；\n"
                "(七)信息网络传播权，即以有线或者无线方式向公众提供软件，使公众\n"
                "可以在其个人选定的时间和地点获得软件的权利；\n"
                "(八)翻译权，即将原软件从一种自然语言文字转换成另一种自然语言文\n"
                "字的权利；\n"
                "(九)应当由软件著作权人享有的其他权利。\n\n";
            ImGui::Text(TroiceFunction::TBS(Notice_).c_str());
            static std::string ok = "当用户打开并勾选下方“我同意以上条约并开始使用”的时候就标志着用户已详细阅读\n"
                "该条款且同意！\n";
            ImGui::TextColored({ 1.00f,0.00f,0.00f,0.50f }, TroiceFunction::TBS(ok).c_str());

            ImGui::EndChild();
            ImGui::PopStyleVar();

            static bool check_ = false;
            ImGui::Checkbox(u8"我同意以上条约并开始使用", &check_);
            if (check_) {
                for (int j = 0; j < 20; ++j) {
                    ImGui::SameLine();
                    ImGui::Text(" ");
                }
                ImGui::SameLine();
                if (ImGui::Button(u8"开始使用")) {
                    loadMainWin = true;
                    ImGui::StyleColorsDark();
                    show_demo_window = false;
                }
            }

            ImGui::End();
        }

        if (loadMainWin)
        {
            static float f = 0.0f;
            static int counter = 0;
            static bool gb = false;

            ImGui::Begin(u8"Troice Python Package Manager", &gb, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
            ImGui::SetWindowSize({ 720.0f,470.0f });
            
            //style.GrabRounding = 6;
            style.FrameRounding = 6;
            static bool disable_mouse_wheel = false;
            static bool disable_menu = true;

            ImGui::PushFont(font_18);
            ImGui::TextColored({ 0.61f,0.14f,0.73f,0.50f }, u8"Troice Python Package Manager");
            ImGui::PopFont();
            // Child: rounded border
            {
                ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;
                if (disable_mouse_wheel)
                    window_flags |= ImGuiWindowFlags_NoScrollWithMouse;
                if (!disable_menu)
                    window_flags |= ImGuiWindowFlags_MenuBar;
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild("ChildR", ImVec2(500, 400), true, window_flags);
                if (!disable_menu && ImGui::BeginMenuBar())
                {
                    if (ImGui::BeginMenu("Menu"))
                    {
                        printf("1");
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenuBar();
                }
                if (col::color == 1) {
                    ImGui::PushFont(font_18);
                    ImGui::TextColored({ 1.0f,0.0f,0.0f,0.5f }, u8"必看:使用本工具前请确保本机Python环境是完整的！！！");
                    ImGui::PopFont();
                    static std::string Notice = "本产品为无稚(Troice)开发的Python包管理器\n"
                        "功能：\n"
                        "    模块信息：\n"
                        "        扫描本机Python模块(双通道)\n"
                        "        扫描可更新的模块\n"
                        "    详细信息：\n"
                        "        模块详细信息查询\n"
                        "    安装模块\n"
                        "    卸载模块\n"
                        "\n"
                        "中华人民共和国计算机软件保护条例，根据《中华人民共和国著作权法》，\n"
                        "为了保护计算机软件著作权人的权益而制定，自2002年1月1日起施行。\n"
                        "第八条软件著作权人享有下列各项权利：\n"
                        "(一)发表权，即决定软件是否公之于众的权利；\n"
                        "(二)署名权，即表明开发者身份，在软件上署名的权利；\n"
                        "(三)修改权，即对软件进行增补、删节，或者改变指令、语句顺序的权\n"
                        "利；\n"
                        "(四)复制权，即将软件制作一份或者多份的权利；\n"
                        "(五)发行权，即以出售或者赠与方式向公众提供软件的原件或者复制件\n"
                        "的权利；\n"
                        "(六)出租权，即有偿许可他人临时使用软件的权利，但是软件不是出租\n"
                        "的主要标的的除外；\n"
                        "(七)信息网络传播权，即以有线或者无线方式向公众提供软件，使公众\n"
                        "可以在其个人选定的时间和地点获得软件的权利；\n"
                        "(八)翻译权，即将原软件从一种自然语言文字转换成另一种自然语言文\n"
                        "字的权利；\n"
                        "(九)应当由软件著作权人享有的其他权利。\n\n";
                    ImGui::Text(TroiceFunction::TBS(Notice).c_str());
                    ImGui::TextColored( FontColor::FontColor_1, u8"“我也曾经是有信念的 但是现在已经不重要了“");
                }
                if (col::color == 2) {
                    ImGui::SeparatorText(u8"主查看信息");
                    if (ImGui::Button(u8"获取本机安装的Python包(通道1)")) {
                        message::packageList = TroiceFunction::getCmdResult("pip list");

                    }
                    ImGui::SameLine();

                    if (ImGui::Button(u8"获取本机安装的Python包(通道2)")) {
                        message::packageList = TroiceFunction::getCmdResult("pip freeze");

                    }
                    ImGui::Text(TroiceFunction::TBS(message::packageList).c_str());

                    ImGui::SeparatorText(u8"其余查看信息");
                    if (ImGui::Button(u8"扫描可更新的库")) {
                        message::UpdatableLibrary = "下方的是需要更新的库（可不更新）:\n" + TroiceFunction::getCmdResult("pip list -o");

                    }
                    ImGui::Text(TroiceFunction::TBS(message::UpdatableLibrary).c_str());
                }
                if (col::color == 3) {
                    ImGui::Text(u8"模块名:");
                    ImGui::SameLine();
                    ImGui::InputText(u8"##请输入模块名", InPut::name, 1024);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"查询详细信息", { 95.0f, 20.0f })) {
                        std::string str = TroiceFunction::getCmdResult("pip show " + (std::string)InPut::name);

                        int i;
                        for (i = 0; i < str.length(); ++i)
                        {
                            message::ModuleDetails[i] = str[i];
                        }

                        message::ModuleDetails[i] = '\0';

                    }
                    ImGui::InputTextMultiline(u8"##模块的详细信息", message::ModuleDetails, 1024, { 400.0f, 350.0f });
                }

                if (col::color == 4) {
                    ImGui::Text(u8"使用该功能前请确保网络连接通畅");
                    ImGui::SeparatorText(u8"安装模块");
                    ImGui::InputText("##名字", InPut::name_install, 1024);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"安装")) {
                        std::string str = InPut::name_install;
                        std::string str_ = "pip install -i https://pypi.tuna.tsinghua.edu.cn/simple " + str;
                        std::string returnvalue = "install OK!!!\n\n" + TroiceFunction::getCmdResult(str_);
                        int i;

                        for (i = 0; i < returnvalue.length(); ++i)
                        {
                            message::char_returnvalue[i] = returnvalue[i];
                        }

                        message::char_returnvalue[i] = '\0';
                    }
                    ImGui::SameLine();
                    TroiceFunction::HelpMarker(u8"点击以后即可自动下载安装模块\n安装后即可在Python调用\n默认使用清华源");
                    ImGui::InputTextMultiline(u8"##uinstall返回值", message::char_returnvalue, 4096, { 400.0f, 250.0f });
                    
                }

                if (col::color == 5) {
                    ImGui::Text(u8"卸载模块前确保已安装此模块");
                    ImGui::SeparatorText(u8"卸载模块");
                    ImGui::InputText("##名字_", InPut::name_uninstall, 1024);
                    ImGui::SameLine();
                    if (ImGui::Button(u8"卸载")) {
                        std::string str = InPut::name_uninstall;
                        std::string str_ = "pip uninstall " + str + " -y";
                        std::string returnvalue = "uninstall OK!!!\n\n" + TroiceFunction::getCmdResult(str_);
                        int i;

                        for (i = 0; i < returnvalue.length(); ++i)
                        {
                            message::char_returnvalue_[i] = returnvalue[i];
                        }

                        message::char_returnvalue_[i] = '\0';
                    }
                    ImGui::SameLine();
                    TroiceFunction::HelpMarker(u8"点击以后即可自动卸载模块(无需验证)\n卸载后即不可在Python调用");
                    ImGui::InputTextMultiline(u8"##uninstall返回值", message::char_returnvalue_, 4096, { 400.0f, 250.0f });
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();
            }

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

            float ButtonPosY = 30;
            ImGui::SetCursorPos({ 530.0f,ButtonPosY });
            static float color_number = 6;
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 1 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"主页", { 160.0f,50.0f }))
            {
                col::color = 1;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 2 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"模块信息", {160.0f,50.0f}))
            {
                col::color = 2;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 3 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"详细信息", { 160.0f,50.0f }))
            {
                col::color = 3;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 4 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"安装模块", { 160.0f,50.0f }))
            {
                col::color = 4;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 5 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"卸载模块", { 160.0f,50.0f }))
            {
                col::color = 5;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            ImGui::PushStyleColor(ImGuiCol_Button, col::color == 6 ? ImVec4(1.0f, 0.5f, 0.3f, 1.0f) : (ImVec4)ImColor::HSV(color_number / 7.0f, 0.6f, 0.6f));
            if (ImGui::Button(u8"退出程序", { 160.0f,50.0f }))
            {
                exit(0);
                col::color = 6;
            }
            ImGui::PopStyleColor();

            ButtonPosY += 65.0f;
            ImGui::SetCursorPos({ 530.0f, ButtonPosY });
            static int style_idx = 0;
            ImGui::SetNextItemWidth(100);
            if (ImGui::Combo(u8"修改主题", &style_idx, u8"暗色主题\0亮色主题\0经典主题\0"))
            {
                switch (style_idx)
                {
                case 0: ImGui::StyleColorsDark(); FontColor::FontColor_1 = { 1.0f,1.0f,0.0f,0.5f }; break;
                case 1: ImGui::StyleColorsLight(); FontColor::FontColor_1 = { 0.67f,0.07f,0.87f,0.5f }; break;
                case 2: ImGui::StyleColorsClassic(); FontColor::FontColor_1 = { 1.0f,1.0f,0.0f,0.5f }; break;
                }
            }

            ImGui::End();
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions
bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
        res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

#ifndef WM_DPICHANGED
#define WM_DPICHANGED 0x02E0 // From Windows SDK 8.1+ headers
#endif

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        g_ResizeWidth = (UINT)LOWORD(lParam); 
        g_ResizeHeight = (UINT)HIWORD(lParam);
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) 
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    case WM_DPICHANGED:
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
        {
            const RECT* suggested_rect = (RECT*)lParam;
            ::SetWindowPos(hWnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
        break;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
