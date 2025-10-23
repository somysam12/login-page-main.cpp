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

const int WINDOW_WIDTH = 900;
const int WINDOW_HEIGHT = 600;

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
        
        ImGui::Begin("Login Sys By @Tgshaitaan", nullptr, 
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoMove | 
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoTitleBar);

        ImVec2 windowSize = ImGui::GetWindowSize();
        float centerX = windowSize.x * 0.5f;
        float centerY = windowSize.y * 0.5f;
        
        float panelWidth = 450.0f;
        float panelHeight = 420.0f;
        float panelX = centerX - panelWidth * 0.5f;
        float panelY = centerY - panelHeight * 0.5f;
        
        ImGui::SetCursorPos(ImVec2(panelX, panelY));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.15f, 0.95f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.35f, 0.5f));
        
        ImGui::BeginChild("LoginPanel", ImVec2(panelWidth, panelHeight), true);
        
        ImGui::SetCursorPosY(40);
        
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.8f, 1.0f, 1.0f));
        const char* title = "Login Sys";
        ImGui::SetWindowFontScale(2.2f);
        float titleWidth = ImGui::CalcTextSize(title).x;
        ImGui::SetCursorPosX((panelWidth - titleWidth) * 0.5f);
        ImGui::Text("%s", title);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosY(90);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.7f, 0.7f, 0.75f, 1.0f));
        const char* subtitle = "By @Tgshaitaan";
        float subtitleWidth = ImGui::CalcTextSize(subtitle).x;
        ImGui::SetCursorPosX((panelWidth - subtitleWidth) * 0.5f);
        ImGui::Text("%s", subtitle);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosY(130);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
        ImGui::SetWindowFontScale(0.85f);
        const char* desc = "Secure Authentication System";
        float descWidth = ImGui::CalcTextSize(desc).x;
        ImGui::SetCursorPosX((panelWidth - descWidth) * 0.5f);
        ImGui::Text("%s", desc);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::Dummy(ImVec2(0, 20));
        
        float inputWidth = 360.0f;
        float inputX = (panelWidth - inputWidth) * 0.5f;
        
        ImGui::SetCursorPosX(inputX);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.95f, 1.0f));
        ImGui::SetWindowFontScale(0.9f);
        ImGui::Text("Username");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosX(inputX);
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.18f, 0.18f, 0.22f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.22f, 0.22f, 0.28f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.25f, 0.32f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(15, 12));
        
        ImGui::PushItemWidth(inputWidth);
        ImGui::InputTextWithHint("##username", "Enter your username", username, IM_ARRAYSIZE(username));
        
        ImGui::Dummy(ImVec2(0, 8));
        
        ImGui::SetCursorPosX(inputX);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.95f, 1.0f));
        ImGui::SetWindowFontScale(0.9f);
        ImGui::Text("License Key");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        
        ImGui::SetCursorPosX(inputX);
        ImGui::InputTextWithHint("##key", "Enter your license key", key, IM_ARRAYSIZE(key), 
            ImGuiInputTextFlags_Password);
        
        ImGui::PopItemWidth();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
        
        ImGui::Dummy(ImVec2(0, 20));
        
        float buttonWidth = 360.0f;
        float buttonHeight = 45.0f;
        float buttonX = (panelWidth - buttonWidth) * 0.5f;
        ImGui::SetCursorPosX(buttonX);
        
        bool loginDisabled = loginInProgress.load() || strlen(username) == 0 || strlen(key) == 0;
        
        if (loginDisabled) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.3f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.25f, 0.25f, 0.3f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.25f, 0.3f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.55f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.25f, 0.55f, 0.95f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        }
        
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
        
        if (ImGui::Button(loginInProgress.load() ? "AUTHENTICATING..." : "LOGIN", ImVec2(buttonWidth, buttonHeight)) && !loginDisabled) {
            PerformLogin();
        }
        
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(4);
        
        {
            std::lock_guard<std::mutex> lock(messageMutex);
            
            if (!errorMessage.empty()) {
                ImGui::Dummy(ImVec2(0, 8));
                float msgWidth = ImGui::CalcTextSize(errorMessage.c_str()).x + 30;
                float msgX = (panelWidth - msgWidth) * 0.5f;
                ImGui::SetCursorPosX(msgX);
                
                ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.8f, 0.2f, 0.2f, 0.3f));
                ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.9f, 0.3f, 0.3f, 0.6f));
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
                
                ImGui::BeginChild("ErrorBox", ImVec2(msgWidth, 40), true);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.4f, 0.4f, 1.0f));
                ImGui::SetCursorPosY(11);
                ImGui::SetCursorPosX(15);
                ImGui::Text("%s", errorMessage.c_str());
                ImGui::PopStyleColor();
                ImGui::EndChild();
                
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);
            }
            
            if (!statusMessage.empty()) {
                ImGui::Dummy(ImVec2(0, 8));
                float msgWidth = ImGui::CalcTextSize(statusMessage.c_str()).x;
                float msgX = (panelWidth - msgWidth) * 0.5f;
                ImGui::SetCursorPosX(msgX);
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 1.0f, 0.6f, 1.0f));
                ImGui::Text("%s", statusMessage.c_str());
                ImGui::PopStyleColor();
            }
        }
        
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(2);
        
        ImGui::SetCursorPos(ImVec2(10, WINDOW_HEIGHT - 25));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.45f, 0.7f));
        ImGui::SetWindowFontScale(0.75f);
        ImGui::Text("v1.0.0 | Secure Login System");
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();

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

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Login Sys By @Tgshaitaan", nullptr, nullptr);
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
    style.FrameRounding = 6.0f;
    style.FramePadding = ImVec2(12, 8);
    style.GrabRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.12f, 1.0f);

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
        glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
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
