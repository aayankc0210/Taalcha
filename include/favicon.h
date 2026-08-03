#pragma once
#include <filesystem>
#include <string>
using namespace std;

// Extract domain from a URL string. Returns "" on failure.
// e.g. "https://github.com/foo" -> "github.com"
inline string domain_from_url(const string &url) {
  if (url.empty()) return "";
  string s = url;
  // strip scheme
  auto scheme_end = s.find("://");
  if (scheme_end != string::npos)
    s = s.substr(scheme_end + 3);
  // strip path
  auto slash = s.find('/');
  if (slash != string::npos)
    s = s.substr(0, slash);
  // strip port
  auto colon = s.find(':');
  if (colon != string::npos)
    s = s.substr(0, colon);
  return s;
}

// Fetch favicon for `domain` into `cache_dir/domain.png` using libcurl.
// Returns the path on success, "" on failure.
// Blocking — call from a background thread or on entry select (fine for a semester project).
string fetch_favicon(const string &domain,
                          const filesystem::path &cache_dir);