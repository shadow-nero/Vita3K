// memory_view.cpp
#include "memory_view.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cctype>

namespace gui {

void draw_memory_view(GuiState &gui, EmuEnvState &emuenv) {
    ImGui::Begin("Memory View", &gui.debug_menu.memory_view_dialog);

    ImGui::Text("Memory Start Address");
    ImGui::SameLine();
    ImGui::InputText("##memory_start", gui.memory_start_address, 9, ImGuiInputTextFlags_CharsHexadecimal);

    ImGui::Text("Size (bytes)");
    ImGui::SameLine();
    ImGui::InputInt("##memory_size", &gui.memory_size);

    if (ImGui::Button("View Memory")) {
        uint32_t address = static_cast<uint32_t>(std::stol(gui.memory_start_address, nullptr, 16));
        size_t size = gui.memory_size;
        uint8_t *memory = emuenv.mem.data() + address;
        gui.memory_data.clear();
        gui.memory_data.insert(gui.memory_data.end(), memory, memory + size);
    }

    if (ImGui::Button("Dump Memory")) {
        dump_memory(gui.memory_data.data(), gui.memory_data.size(), "memory_dump.bin");
    }

    ImGui::Separator();
    ImGui::BeginChild("memory_data", ImVec2(0, 0), true);
    const int bytes_per_row = 16;
    for (size_t i = 0; i < gui.memory_data.size(); i += bytes_per_row) {
        ImGui::Text("%08X: ", static_cast<uint32_t>(i));
        for (size_t j = 0; j < bytes_per_row; ++j) {
            if (i + j < gui.memory_data.size()) {
                ImGui::SameLine();
                ImGui::Text("%02X ", gui.memory_data[i + j]);
            }
        }
    }
    ImGui::EndChild();

    ImGui::Separator();
    ImGui::Text("Search for String");
    ImGui::InputText("##search_term", gui.memory_search_term, 256);
    if (ImGui::Button("Search")) {
        gui.search_results = search_strings(gui.memory_data.data(), gui.memory_data.size(), gui.memory_search_term);
    }

    if (!gui.search_results.empty()) {
        ImGui::Separator();
        ImGui::Text("Search Results:");
        for (size_t result : gui.search_results) {
            ImGui::Text("Found at offset: %08X", static_cast<uint32_t>(result));
        }
    }

    ImGui::End();
}

void dump_memory(const uint8_t* memory, size_t size, const std::string& filename) {
    std::ofstream file(filename, std::ios::binary);
    if (file.is_open()) {
        file.write(reinterpret_cast<const char*>(memory), size);
        file.close();
        std::cout << "Memory dumped to " << filename << std::endl;
    } else {
        std::cerr << "Failed to open file for dumping memory." << std::endl;
    }
}

std::vector<size_t> search_strings(const uint8_t* memory, size_t size, const std::string& search_term) {
    std::vector<size_t> results;
    size_t term_length = search_term.length();
    for (size_t i = 0; i <= size - term_length; ++i) {
        if (std::memcmp(memory + i, search_term.c_str(), term_length) == 0) {
            results.push_back(i);
        }
    }
    return results;
}

} // namespace gui
