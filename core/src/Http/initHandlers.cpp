#include <Http/initHandlers.h>

#include <Filesystem/tinydir.h>
#include <Http/Server.h>


#ifdef __MINGW32__

static char sep = '\\';

#else

static char sep = '/';

#endif

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

static const std::string js_input = R"(
function doupload() {
    let files = document.getElementById("file").files;
    for (var i = 0; i < files.length; i++)
    {
        fetch(encodeURIComponent(files[i].name), {method:'PUT',body:files[i]});
    }
    alert('your file has been uploaded');
    document.getElementById("file").value = null;
    location.reload();
};
)";

static const std::string input_style = R"CSS(
<style>
.button {
  background-color: #8a92ce;
  border: none;
  color: black;
  padding: 95px 55px;
  text-align: center;
  text-decoration: none;
  display: inline-block;
  font-size: 16px;
  margin: 4px 2px;
  cursor: pointer;
}
</style>
)CSS";

static const std::string html_button = R"BUTTON(
<h1>Put file to folder:</h1>
<input  type = "file" 
        name = "file" 
        multiple="multiple"
        id = "file" 
        class ="button">
<button onclick = "doupload()" 
        class ="button"
        name = "submit"> Upload File </button>
)BUTTON";

std::string htmlFileList(tinydir_dir &dir, const i32 from = 1) {
    std::string payload = "<!DOCTYPE html><html><script>\n" + js_input + "\n</script>";
    payload += input_style;
    payload += "<h1>Files:</h1>";
    std::string html = "<li><a href=\"%\">%</a></li>\n";
    for (u32 i = from; i < dir.n_files; ++i) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, i);

        std::string name = file.name;
        if (file.is_dir) {
            name += '/';
        }
        payload += utils::strFmt(html, name, name);

        tinydir_next(&dir);
    }
    payload += html_button;

    return payload + "</html>";
}

http_get::Response getFile(const std::string &shared_folder,
                           const std::string &req_pattern,
                           const http_get::Request *req) {
    std::string filename =
            shared_folder + sep +
            urlDecode(utils::value(req_pattern, req->query()));

    std::fstream fileOrDir(filename);
    fileOrDir.seekg(0, std::ios::end);
    tinydir_dir dir;
    if (fileOrDir.good()) {
        fileOrDir.close();
        // return file
        return http_get::Response().code(200, "Ok").fileData(filename);
    } else if (tinydir_open_sorted(&dir, filename.c_str()) != -1) {
        std::string payload = htmlFileList(dir);
        tinydir_close(&dir);

        // return directory
        return http_get::Response()
                .code(200, "Ok")
                .content("text/html; charset=UTF-8")
                .data(payload);
    }

    return http_get::Response()
            .code(404, "Not found")
            .content("text/html; charset=UTF-8")
            .data("<h1>No such file</h1>");
}

http_get::Response getDir(const std::string &shared_folder,
                          const http_get::Request *req) {
    tinydir_dir dir;
    tinydir_open_sorted(&dir, shared_folder.c_str());

    std::string payload = htmlFileList(dir, 2);

    tinydir_close(&dir);

    return http_get::Response()
            .code(200, "Ok")
            .content("text/html; charset=UTF-8")
            .data(payload);
}

http_get::Response postFile(const std::string &shared_folder,
                            const std::string &req_pattern,
                            const http_get::Request *req) {
    std::string filename =
            shared_folder + sep +
            urlDecode(utils::value(req_pattern, req->query()));

    req->copyTo(filename);

    return http_get::Response()
            .code(201, "Created")
            .content("text/html; charset=UTF-8")
            .data("<h1>" + filename + "</h1>");
}

} // namespace

namespace http_get { inline namespace Http {

void initHanlders(Http::Server *server,
                  const std::string &shared_folder) {
    if (!server) return;

    server->addHandler("GET /%",
                       [shared_folder](const Http::Request *req) {
                           return getFile(shared_folder, "GET /%", req);
                       });

    server->addHandler("GET /",
                       [shared_folder](const Http::Request *req) {
                           return getDir(shared_folder, req);
                       });

    std::string post_handler = "PUT /%";
    server->addHandler(
            post_handler,
            [shared_folder, post_handler](const Http::Request *req) {
                return postFile(shared_folder, post_handler, req);
            });
}

}} // namespace http_get::Http