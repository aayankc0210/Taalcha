#pragma once
#include <filesystem>
#include <string>

// Extract domain from a URL string. Returns "" on failure.
// e.g. "https://github.com/foo" -> "github.com"
inline std::string domain_from_url(const std::string &url) {
  if (url.empty()) return "";
  std::string s = url;
  // strip scheme
  auto scheme_end = s.find("://");
  if (scheme_end != std::string::npos)
    s = s.substr(scheme_end + 3);
  // strip path
  auto slash = s.find('/');
  if (slash != std::string::npos)
    s = s.substr(0, slash);
  // strip port
  auto colon = s.find(':');
  if (colon != std::string::npos)
    s = s.substr(0, colon);
  return s;
}

// Fetch favicon for `domain` into `cache_dir/domain.png` using libcurl.
// Returns the path on success, "" on failure.
// Blocking — call from a background thread or on entry select (fine for a semester project).
std::string fetch_favicon(const std::string &domain,
                          const std::filesystem::path &cache_dir);
