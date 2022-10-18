#include <Http/initHandlers.h>

#include <Filesystem/tinydir.h>
#include <Http/Server.h>

namespace {

std::string urlDecode(const std::string &SRC) {
    std::string ret;
    i8 ch;
    u32 i, ii;
    for (i = 0; i < SRC.length(); i++) {
        if (SRC[i] == '%') {
            sscanf(SRC.substr(i + 1, 2).c_str(), "%x", &ii);
            ch = static_cast<i8>(ii);
            ret += ch;
            i = i + 2;
        } else {
            ret += SRC[i];
        }
    }
    return ret;
}

} // namespace

namespace http_get { inline namespace Http {

void initHanlders(Http::Server *server,
                  const std::string &shared_folder) {
    if (!server) return;

    server->addHandler(
            "GET /%", [shared_folder](const Http::Request *req) {
                std::string filename =
                        shared_folder + "/" +
                        urlDecode(utils::value("GET /%", req->query()));

                std::fstream fileOrDir(filename);
                fileOrDir.seekg(0, std::ios::end);
                tinydir_dir dir;
                if (fileOrDir.good()) {
                    fileOrDir.close();
                    return Http::Response().code(200, "Ok").fileData(
                            filename);
                } else if (tinydir_open_sorted(
                                   &dir, filename.c_str()) != -1) {
                    std::string payload = "<h1>Files:</h1>";
                    std::string html = "<li><a href=\"%\">%</a></li>\n";

                    for (u32 i = 1; i < dir.n_files; ++i) {
                        tinydir_file file;
                        tinydir_readfile_n(&dir, &file, i);

                        std::string name = file.name;
                        if (file.is_dir) {
                            name += '/';
                        }
                        payload += utils::strFmt(html, name, name);

                        tinydir_next(&dir);
                    }

                    tinydir_close(&dir);

                    return Http::Response()
                            .code(200, "Ok")
                            .content("text/html; charset=UTF-8")
                            .data(payload);
                }

                return Http::Response()
                        .code(404, "Not found")
                        .content("text/html; charset=UTF-8")
                        .data("<h1>No such file</h1>");
            });

    server->addHandler(
            "GET /", [shared_folder](const Http::Request *req) {
                tinydir_dir dir;
                tinydir_open_sorted(&dir, shared_folder.c_str());

                std::string payload = "<h1>Files:</h1>";
                std::string html = "<li><a href=\"%\">%</a></li>\n";

                for (u32 i = 2; i < dir.n_files; ++i) {
                    tinydir_file file;
                    tinydir_readfile_n(&dir, &file, i);

                    std::string name = file.name;
                    if (file.is_dir) {
                        name += '/';
                    }
                    payload += utils::strFmt(html, name, name);

                    tinydir_next(&dir);
                }

                tinydir_close(&dir);

                return Http::Response()
                        .code(200, "Ok")
                        .content("text/html; charset=UTF-8")
                        .data(payload);
            });
}

}} // namespace http_get::Http