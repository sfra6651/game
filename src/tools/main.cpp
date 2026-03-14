#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <mutex>

#include "shared/protocol.h"
#include "shared/tcp.h"

static TcpServer server;

static void drawConnectionStatus() {
    ImGui::Begin("Connection");
    if (server.isConnected()) {
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Connected");
    } else {
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "Waiting for game...");
    }
    ImGui::End();
}

inline void drawEntities(WorldSnap &worldSnap) {
    if (ImGui::BeginTable("EntityTable" , 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("id");
        ImGui::TableSetupColumn("position");
        ImGui::TableSetupColumn("direction");
        ImGui::TableSetupColumn("speed");
        ImGui::TableSetupColumn("size");

        for (auto &entity: worldSnap.entities) {
            int id = entity.id;
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%d", id);
            ImGui::TableNextColumn(); ImGui::Text("%d, %d", worldSnap.positions[id].x, worldSnap.positions[id].y);
            ImGui::TableNextColumn(); ImGui::Text("%f.2, %f.2", worldSnap.directions[id].x, worldSnap.directions[id].y);
            ImGui::TableNextColumn(); ImGui::Text("%d", worldSnap.speeds[id].v);
            ImGui::TableNextColumn(); ImGui::Text("%d, %d", worldSnap.sizes[id].width, worldSnap.sizes[id].height);
        };

        ImGui::EndTable();
    };
};

int main() {
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(2000, 2000, "Dev Tools", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    // Scale UI
    ImGui::GetStyle().ScaleAllSizes(2.5f);
    io.FontGlobalScale = 2.5;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    DebugProtocol protocol{};
    WorldSnap worldSnap{};

    static std::mutex contentMutex;

    server.start();
    std::thread revcThread([&]() {
        while (server.running_) {
            DebugProtocol protocol{};
            if (protocol.recvFrom(server)) {
                std::lock_guard<std::mutex> lock(contentMutex);
                protocol.parseEntitySnaps(worldSnap);
            };
        };
    });

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        drawConnectionStatus();
        drawEntities(worldSnap);

        ImGui::Render();
        int displayW, displayH;
        glfwGetFramebufferSize(window, &displayW, &displayH);
        glViewport(0, 0, displayW, displayH);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    server.stop();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
