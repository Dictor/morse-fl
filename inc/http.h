#ifndef HANGANG_VIEW_HTTP
#define HANGANG_VIEW_HTTP

namespace hangang_view {
int HttpInit();
int HttpGet(char* host, char* path);
};  // namespace hangang_view

#endif