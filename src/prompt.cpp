#include "prompt.hpp"

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include <nfd.hpp>

namespace fs = std::filesystem;

// Prompts the user for folder containing model files
fs::path prompt_dir() {
    NFD::Guard nfd_guard{};
    NFD::UniquePath nfd_path;

    // Prompt for path
    auto res = NFD::PickFolder(nfd_path, fs::current_path().c_str());

    fs::path path;
    if (res == NFD_OKAY) {
        path = fs::path(nfd_path.get());

        // Older xdg-portal versions don't support selecting folders,
        // so in that case we use the parent path of the selected file.
        if (fs::is_directory(path) == false) {
            path = path.parent_path();
        }
    } else {
        std::cerr << "Failed to pick folder, using current path as fallback\n";
        path = fs::current_path();
    }

    return path;
}

std::vector<std::string> prompt_model_paths() {
    std::vector<std::string> model_paths;

    auto dir = prompt_dir();

    // NOTE: Non-recursive directory iterator is used, since otherwise the models from
    // tinyobjectloader submodule will be loaded, which cause errors.
    for (auto const& entry : fs::directory_iterator(dir)) {
        if (entry.path().extension().string() == ".obj") {
            model_paths.emplace_back(entry.path().string());
        }
    }

    return model_paths;
}
