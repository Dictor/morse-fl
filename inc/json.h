#ifndef HANGANG_VIEW_JSON
#define HANGANG_VIEW_JSON

#include <zephyr/data/json.h>

namespace hangang_view {
namespace json {

struct symbol {
  const char* name;
  int price;
  int percentile;
};

struct symbols {
  struct symbol symbols[30];
  size_t symbols_len;
};

int ParseSymbolJson(char* payload, int payload_len, struct json::symbols* data);
};  // namespace json
};  // namespace hangang_view

#endif