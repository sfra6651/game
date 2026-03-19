#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <atomic>
#include <mutex>

#include "shared/message.h"
#include "shared/snapshot.h"
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

inline void EntitiesTable(WorldSnap &worldSnap) {
    if (ImGui::BeginTable("EntityTable" , 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg)) {
        ImGui::TableSetupColumn("id");
        ImGui::TableSetupColumn("position");
        ImGui::TableSetupColumn("direction");
        ImGui::TableSetupColumn("speed");
        ImGui::TableSetupColumn("size");
        ImGui::TableHeadersRow();

        for (int i = 0; i <= worldSnap.entities.count; i++) {
            int id = worldSnap.entities.get(id).id;
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%d", id);
            ImGui::TableNextColumn(); ImGui::Text("%d, %d", worldSnap.getStore<Position>().get(id).x, worldSnap.getStore<Position>().get(id).y);
            ImGui::TableNextColumn(); ImGui::Text("%f.2, %f.2", worldSnap.getStore<Direction>().get(id).x, worldSnap.getStore<Direction>().get(id).y);
            ImGui::TableNextColumn(); ImGui::Text("%d", worldSnap.getStore<Speed>().get(id).v);
            ImGui::TableNextColumn(); ImGui::Text("%d, %d", worldSnap.getStore<Size>().get(id).width, worldSnap.getStore<Size>().get(id).height);
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

    WorldSnap worldSnap{};

    std::mutex contentMutex;
    std::atomic<bool> paused{false};

    server.start();
    TcpConnection &conn = server.connection();

    // Recv thread: just receives snapshots and updates worldSnap
    std::thread recvThread([&]() {
        while (server.running_) {
            if (!server.isConnected()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                continue;
            }

            MessageHeader header{};
            std::vector<uint8_t> payload;
            if (!recvMessage(conn, header, payload)) {
                server.connected_ = false;
                paused = false;
                continue;
            }

            if (header.type == MessageType::FRAME_SNAPSHOT) {
                int count = header.payloadSize / sizeof(EntitySnapshot);
                std::vector<EntitySnapshot> snaps(count);
                memcpy(snaps.data(), payload.data(), header.payloadSize);

                std::lock_guard<std::mutex> lock(contentMutex);
                createWorldSnap(snaps, worldSnap);
            }
        }
    });

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        drawConnectionStatus();

        // Debug controls
        ImGui::Begin("Controls");
        if (server.isConnected()) {
            if (paused) {
                if (ImGui::Button("Resume")) {
                    paused = false;
                    sendMessage(conn, MessageType::RESUME);
                }
                ImGui::SameLine();
                if (ImGui::Button("Step")) {
                    sendMessage(conn, MessageType::STEP);
                }
            } else {
                if (ImGui::Button("Pause")) {
                    paused = true;
                    sendMessage(conn, MessageType::PAUSE);
                }
            }
        }
        ImGui::End();

        {
            std::lock_guard<std::mutex> lock(contentMutex);
            //EntitiesTable(worldSnap);
        }

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
    if (recvThread.joinable()) recvThread.join();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
