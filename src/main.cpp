#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include "auth_handler.h"
#include <string>
#include <thread>
#include <chrono>
#include <mutex>
#include <atomic>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 500;

class LoginUI {
private:
    char username[256] = "";
    char key[256] = "";
    std::atomic<bool> loginInProgress{false};
    std::string errorMessage = "";
    std::string statusMessage = "";
    AuthHandler authHandler;
    std::atomic<bool> isLoggedIn{false};
    std::mutex messageMutex;
    std::thread loginThread;
    std::atomic<bool> shutdownRequested{false};

public:
    LoginUI() : authHandler() {}
    
    ~LoginUI() {
        shutdownRequested.store(true);
        if (loginThread.joinable()) {
            loginThread.join();
        }
    }

    void Render() {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
        
        ImGui::Begin("BR MODS EXTERNAL", nullptr, 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        ImVec2 windowSize = ImGui::GetWindowSize();
        float centerX = windowSize.x * 0.5f;
        
        ImGui::SetCursorPosY(60);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.9f, 0.9f, 1.0f));
        const char* title = "BR MODS EXTERNAL";
        float titleWidth = ImGui::CalcTextSize(title).x;
        ImGui::SetCursorPosX(centerX - titleWidth * 0.5f);
        ImGui::SetWindowFontScale(2.0f);
        ImGui::Text("%s", title);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosY(120);
        const char* subtitle = "Log in to start session";
        float subtitleWidth = ImGui::CalcTextSize(subtitle).x;
        ImGui::SetCursorPosX(centerX - subtitleWidth * 0.5f);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::Text("%s", subtitle);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosY(180);
        float inputWidth = 400.0f;
        ImGui::SetCursorPosX(centerX - inputWidth * 0.5f);
        
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.2f, 0.5f, 0.5f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.2f, 0.6f, 0.6f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.2f, 0.6f, 0.6f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        
        ImGui::PushItemWidth(inputWidth);
        ImGui::InputTextWithHint("##username", "Username", username, IM_ARRAYSIZE(username));
        
        ImGui::SetCursorPosX(centerX - inputWidth * 0.5f);
        ImGui::InputTextWithHint("##key", "License Key", key, IM_ARRAYSIZE(key), 
            ImGuiInputTextFlags_Password);
        
        ImGui::PopItemWidth();
        ImGui::PopStyleColor(4);
        
        ImGui::SetCursorPosY(280);
        float buttonWidth = 150.0f;
        float buttonHeight = 40.0f;
        ImGui::SetCursorPosX(centerX - buttonWidth * 0.5f);
        
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.9f, 0.9f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.0f, 0.8f, 0.8f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
        
        bool loginDisabled = loginInProgress.load() || strlen(username) == 0 || strlen(key) == 0;
        
        if (loginDisabled) {
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.5f);
        }
        
        if (ImGui::Button(loginInProgress.load() ? "Logging in..." : "Login", ImVec2(buttonWidth, buttonHeight)) && !loginDisabled) {
            PerformLogin();
        }
        
        if (loginDisabled) {
            ImGui::PopStyleVar();
        }
        
        ImGui::PopStyleColor(4);
        
        {
            std::lock_guard<std::mutex> lock(messageMutex);
            if (!errorMessage.empty()) {
                ImGui::SetCursorPosY(340);
                float msgWidth = ImGui::CalcTextSize(errorMessage.c_str()).x;
                ImGui::SetCursorPosX(centerX - msgWidth * 0.5f);
                
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.3f, 0.1f, 0.1f, 0.8f));
                
                ImGui::BeginChild("ErrorBox", ImVec2(msgWidth + 40, 50), true);
                ImGui::SetCursorPosY(15);
                ImGui::SetCursorPosX(20);
                ImGui::Text("%s", errorMessage.c_str());
                ImGui::EndChild();
                
                ImGui::PopStyleColor(2);
            }
            
            if (!statusMessage.empty()) {
                ImGui::SetCursorPosY(340);
                float msgWidth = ImGui::CalcTextSize(statusMessage.c_str()).x;
                ImGui::SetCursorPosX(centerX - msgWidth * 0.5f);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
                ImGui::Text("%s", statusMessage.c_str());
                ImGui::PopStyleColor();
            }
        }

        ImGui::End();
    }

    void PerformLogin() {
        if (loginThread.joinable()) {
            loginThread.join();
        }
        
        loginInProgress.store(true);
        {
            std::lock_guard<std::mutex> lock(messageMutex);
            errorMessage = "";
            statusMessage = "Validating...";
        }
        
        loginThread = std::thread([this]() {
            AuthResult result = authHandler.ValidateKey(username, key);
            
            if (!shutdownRequested.load()) {
                std::lock_guard<std::mutex> lock(messageMutex);
                loginInProgress.store(false);
                statusMessage = "";
                
                if (result.success) {
                    isLoggedIn.store(true);
                    errorMessage = "";
                    statusMessage = "Login successful!";
                } else {
                    errorMessage = result.message;
                }
            }
        });
    }

    bool IsLoggedIn() const { return isLoggedIn.load(); }
};

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
    glfwSetErrorCallback(glfw_error_callback);
    
    if (!glfwInit()) {
        return 1;
    }

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "BR MODS EXTERNAL", nullptr, nullptr);
    if (window == nullptr) {
        return 1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 0.0f;
    style.FrameRounding = 4.0f;
    style.FramePadding = ImVec2(12, 8);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    LoginUI loginUI;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        loginUI.Render();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
