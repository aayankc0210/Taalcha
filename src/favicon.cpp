#include "favicon.h"
#include <curl/curl.h>
#include <fstream>
#include <filesystem>

static size_t write_cb(void* ptr, size_t size, size_t nmemb, void* userdata) {
    auto* buf = static_cast<std::string*>(userdata);
    buf->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

std::string fetch_favicon(const std::string& domain,
                          const std::filesystem::path& cache_dir) {
    if (domain.empty()) return "";

    std::filesystem::create_directories(cache_dir);

    std::filesystem::path out_path = cache_dir / (domain + ".png");

    if (std::filesystem::exists(out_path))
        return out_path.string();

    std::string url = "https://www.google.com/s2/favicons?domain=" + domain + "&sz=64";

    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string body;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &body);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode res = curl_easy_perform(curl);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK || body.empty() || http_code != 200) return "";

    // Google returns a 1x1 grey PNG when it has no favicon — skip those (68 bytes)
    if (body.size() < 200) return "";

    std::ofstream f(out_path, std::ios::binary);
    if (!f) return "";
    f.write(body.data(), (std::streamsize)body.size());

    return out_path.string();
}
