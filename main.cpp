#include "app.h"
#include "VaultManager.h"
#include <filesystem>
#include <slint.h>

std::filesystem::path get_vault_path() {
#if defined(_WIN32)
    const char* base = std::getenv("APPDATA");
    std::filesystem::path p = base ? base : ".";
    p /= "taalcha";
#elif defined(__APPLE__)
    const char* home = std::getenv("HOME");
    std::filesystem::path p = home ? home : ".";
    p /= "Library/Application Support/taalcha";
#else
    const char* home = std::getenv("HOME");
    std::filesystem::path p = home ? home : ".";
    p /= ".local/share/taalcha";
#endif
    std::filesystem::create_directories(p);
    return p;
}

int main() {
    auto app = AppWindow::create();
    auto vault_path = get_vault_path();

    VaultManager vault(vault_path);
    if (!vault.init()) return 1;

    app->set_current_page(vault.is_initialized() ? Page::Unlock : Page::Setup);

    app->on_setup_vault([&](slint::SharedString pw) {
        if (vault.setup(std::string(pw))) {
            app->set_current_page(Page::Vault);
            app->set_status_message("");
        } else {
            app->set_status_message("Failed to create vault");
        }
    });

    app->on_unlock_vault([&](slint::SharedString pw) {
        if (vault.unlock(std::string(pw))) {
            app->set_current_page(Page::Vault);
            app->set_status_message("");
        } else {
            app->set_status_message("Wrong password");
        }
    });

    app->run();
}
