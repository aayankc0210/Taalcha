#include "app.h"
#include "VaultManager.h"
#include "favicon.h"
#include "generator.h"
#include <filesystem>
#include <slint.h>
using namespace std;

filesystem::path get_vault_path() {
#if defined(_WIN32)
    const char* base = getenv("APPDATA");
    filesystem::path p = base ? base : ".";
    p /= "taalcha";
#elif defined(__APPLE__)
    const char* home = getenv("HOME");
    filesystem::path p = home ? home : ".";
    p /= "Library/Application Support/taalcha";
#else
    const char* home = getenv("HOME");
    filesystem::path p = home ? home : ".";
    p /= ".local/share/taalcha";
#endif
    filesystem::create_directories(p);
    return p;
}

static vector<Entry> all_entries;
static filesystem::path favicon_cache;

static EntryData to_slint(const Entry& e) {
    EntryData d;
    d.id         = e.id;
    d.entry_type = slint::SharedString(e.type);
    d.title      = slint::SharedString(e.title);
    d.username   = slint::SharedString(e.username);
    d.password   = slint::SharedString(e.password);
    d.note       = slint::SharedString(e.note);
    d.url        = slint::SharedString(e.url);
    if (!e.favicon_path.empty())
        d.favicon = slint::Image::load_from_path(slint::SharedString(e.favicon_path));
    return d;
}

static void push_filtered(AppWindow& app, const string& query) {
    auto vec = make_shared<slint::VectorModel<EntryData>>();
    for (auto& e : all_entries) {
        if (!query.empty()) {
            auto lower = [](string s) {
                for (auto& c : s) c = (char)tolower((unsigned char)c);
                return s;
            };
            string q = lower(query);
            if (lower(e.title).find(q)    == string::npos &&
                lower(e.username).find(q) == string::npos &&
                lower(e.note).find(q)     == string::npos)
                continue;
        }
        vec->push_back(to_slint(e));
    }
    app.set_entries(vec);
}

static void refresh_entries(AppWindow& app, VaultManager& vault) {
    all_entries = vault.get_entries();
    for (auto& e : all_entries) {
        if (e.type != "login" || e.url.empty() || !e.favicon_path.empty()) continue;
        string domain = domain_from_url(e.url);
        if (!domain.empty())
            e.favicon_path = fetch_favicon(domain, favicon_cache);
    }
    push_filtered(app, string(app.get_search_text()));
}

int main() {
    auto app = AppWindow::create();
    auto vault_path = get_vault_path();
    favicon_cache = vault_path / "favicons";

    VaultManager vault(vault_path);
    if (!vault.init()) return 1;

    app->set_current_page(vault.is_initialized() ? Page::Unlock : Page::Setup);

    app->set_gen_length(16);
    app->set_gen_upper(true);
    app->set_gen_digits(true);
    app->set_gen_symbols(true);

    app->on_generate_password([&]() {
        string pw = gen_password(
            app->get_gen_length(),
            app->get_gen_upper(),
            app->get_gen_digits(),
            app->get_gen_symbols());
        app->set_gen_password(slint::SharedString(pw));
        app->set_gen_score(password_score(pw));
    });

    app->on_setup_vault([&](slint::SharedString pw) {
        if (vault.setup(string(pw))) {
            app->set_current_page(Page::Vault);
            app->set_status_message("");
            refresh_entries(*app, vault);
        } else {
            app->set_status_message("Failed to create vault");
        }
    });

    app->on_unlock_vault([&](slint::SharedString pw) {
        if (vault.unlock(string(pw))) {
            app->set_current_page(Page::Vault);
            app->set_status_message("");
            refresh_entries(*app, vault);
        } else {
            app->set_status_message("Wrong password");
        }
    });

    app->on_lock_vault([&]() {
        vault.lock();
        app->set_current_page(Page::Unlock);
        app->set_selected_id(-1);
        app->set_search_text("");
        app->set_status_message("");
        app->set_gen_password("");
        all_entries.clear();
        app->set_entries(make_shared<slint::VectorModel<EntryData>>());
    });

    app->on_search_changed([&](slint::SharedString q) {
        app->set_search_text(q);
        push_filtered(*app, string(q));
    });

    app->on_add_login([&](slint::SharedString title, slint::SharedString user,
                          slint::SharedString pass, slint::SharedString url) {
        if (string(title).empty()) { app->set_status_message("Title required"); return; }
        vault.add_login(string(title), string(user), string(pass), { string(url) });
        app->set_status_message("Saved!");
        refresh_entries(*app, vault);
    });

    app->on_add_note([&](slint::SharedString title, slint::SharedString note) {
        if (string(title).empty()) { app->set_status_message("Title required"); return; }
        vault.add_note(string(title), string(note));
        app->set_status_message("Saved!");
        refresh_entries(*app, vault);
    });

    app->on_save_edit([&](int id, slint::SharedString user, slint::SharedString pass,
                          slint::SharedString note, slint::SharedString url) {
        for (auto& e : all_entries) {
            if (e.id != id) continue;
            if (e.type == "login")
                vault.update_login(id, string(user), string(pass), string(url));
            else
                vault.update_note(id, string(note));
            break;
        }
        refresh_entries(*app, vault);
        app->set_status_message("Updated!");
    });

    app->on_delete_entry([&](int id) {
        vault.delete_entry(id);
        app->set_selected_id(-1);
        refresh_entries(*app, vault);
    });

    app->on_entry_selected([&](int id) {
        for (const auto& e : all_entries) {
            if (e.id == id) {
                app->set_sel_type(slint::SharedString(e.type));
                app->set_sel_title(slint::SharedString(e.title));
                app->set_sel_username(slint::SharedString(e.username));
                app->set_sel_password(slint::SharedString(e.password));
                app->set_sel_note(slint::SharedString(e.note));
                app->set_sel_url(slint::SharedString(e.url));
                if (!e.favicon_path.empty())
                    app->set_sel_favicon(slint::Image::load_from_path(slint::SharedString(e.favicon_path)));
                else
                    app->set_sel_favicon(slint::Image());
                break;
            }
        }
    });

    app->run();
}