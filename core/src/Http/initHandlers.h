#pragma once

#include <string>

namespace http_get { inline namespace Http {

class Server;

void initHanlders(Server *server, const std::string &shared_folder);

}} // namespace http_get::Http