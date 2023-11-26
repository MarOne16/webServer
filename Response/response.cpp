#include "./webserver.hpp"

Response::Response(int status, std::vector<std::string> init_line, http_items &response_items)
{
    // this->status = status;

    this->status = status;
    this->response_items = response_items;
    this->init_line = init_line;
    // this->forbidden_req = "<html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden</h1><p>Access to this resource is forbidden.</p></body></html>";
    // this->bad_req = "<html><head><title>400 bad request</title></head><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    // this->HTTP_NOT_SUPPORTED = "<html><head><title>500 Internal Server Error</title></head><body><h1>500 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    this->Resource_not_found = "<html><body><head><title>404 Not Found</title></head><h1>404 Not Found</h1><p> Sorry, the page you're looking for doesn't exist. </p></body></html>";
}

std::string Response::build_response()
{
    if (this->status == 400)
    {
        this->other_response("400", "Bad Request");
    }
    else if (this->status == 505)
        this->other_response("505", "Version Not Supported");
    else if (this->status == 405)
    {
        this->other_response("405", "Method not allowed");
        // std::cout << "HTTP Method not allowed\r\n";
        // exit(1);
    }
    else if (this->status == 413)
        this->other_response("413", "Request-URI Too Long");
    else if (this->status == 505)
        this->other_response("505", "Not Implemented");
    else if (this->status == 411)
        this->other_response("411", "Length Required");
    else if (!this->response_items.location->return_code_url.empty())
    {
        std::cout << "Request-URI retun "   << std::endl; 
        std::string url = (this->response_items.location->root + this->response_items.Path);
        return_pages(this->response_items.location->return_code_url, url); // TODO: check if this redirected response work
    }
    else if (this->response_items.method == "GET")
    {
        this->build_GET();
    }
    else if (this->response_items.method == "DELETE")
    {
        this->build_DELETE();
    }
    else if (this->response_items.method == "POST")
    {
        this->build_POST();
    }
    return (response.str());
}

std::string Response::get_Content_type(std::string url)
{

    std::string contentTypes[23] = {
        "text/plain",
        "text/html",
        "text/css",
        "text/javascript",
        "text/xml",
        "application/json",
        "application/xml",
        "application/xhtml+xml",
        "application/pdf",
        "application/octet-stream",
        "image/jpeg",
        "image/png",
        "image/gif",
        "image/svg+xml",
        "audio/mpeg",
        "audio/wav",
        "video/mp4",
        "video/webm",
        "multipart/form-data",
        "multipart/byteranges",
        "application/x-www-form-urlencoded",
        "application/graphql",
        "application/vnd.api+json",
        // Add more as needed
    };
    std::string extension = url.substr(url.rfind(".") + 1);
    unsigned int i = 0;
    while (i < 23)
    {
        if (contentTypes[i].find(extension) != std::string::npos)
            return contentTypes[i];
        i++;
    }

    return "text/html;";
}


std::string Response::get_type(std::string extension)
{
    extension = trim(extension);
    std::cout << "extension: " << extension << std::endl;
    if (extension.compare("text/html; charset=UTF-8") == 0 || extension.compare("text/html;") == 0)
        return ".html";
    else if (extension.compare("application/json") == 0)
        return ".json";
    else if (extension.compare("image/x-icon") == 0)
        return ".ico";
    else if (extension.compare("image/jpeg") == 0)
        return "jpeg";
    else if (extension.compare("image/jpg") == 0)
        return ".jpg";
    else if (extension.compare("video/mp4") == 0)
        return ".mp4";
    else
        return ".txt";
}

std::string Response::get_Date()
{
    time_t now = time(0);
    std::stringstream str;

    // convert now to string form
    char *dt = ctime(&now);
    str << dt;
    return str.str().substr(0, str.str().size() - 1);
}

std::string Response::check_index_file(std::string &url)
{
    DIR *dir = opendir(url.c_str());
    std::vector<std::string> files = split_v(this->response_items.location->index, " "); // change by value depends on location
    if (dir == NULL)
        return "";
    struct dirent *entity;
    unsigned int i = 0;
    while (i < files.size())
    {
        entity = readdir(dir);
        while (entity != NULL)
        {
            if (entity->d_name == files[i])
                return files[i];
            entity = readdir(dir);
        }
        i++;
    }
    closedir(dir);
    return "";
}

void Response::build_GET()
{

    struct stat buffer;
    int status;
    std::string URI = this->response_items.location->root;                 // change by value depends on location
    std::string get_auto_index = this->response_items.location->autoindex; // change by getter
    std::string cgi_path = this->response_items.location->cgi_path;        // change path with valid path from config;
    std::string index;
    std::string autoIndexPage;
    URI += this->response_items.Path.substr(1); // TODO : check if path is beging with /
                std::cout << "uri: " << URI << std::endl;
    status = stat(URI.data(), &buffer);
    if (status != -1)
    {
        if (this->response_items.Extension.empty() == 1)
        {
            if (this->response_items.Path[this->response_items.Path.size() - 1] != '/')
                this->ft_redirect("300", this->response_items.Path + "/");
            else
            {
                index = check_index_file(URI);
                if (index.empty())
                {
                    DIR *dir = opendir(URI.c_str());
                    struct dirent *entity;
                    if (get_auto_index == "off")
                    {
                        URI += "index.html";
                        if (access(URI.c_str(), F_OK | W_OK) != 0)
                        {
                            this->other_response("403", " Forbidden");
                            return;
                        }
                        std::string content_body = read_file(URI.c_str());
                        this->ft_success_code("200", content_body, URI);

                    }
                    else
                    {
                        autoIndexPage = "<!DOCTYPE html>\n<html lang=\"en\">\n\
                                                    <head>\n\
                                                    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
                                                    <title>AUTO INDEX</title>\n\
                                                    </head>\n\
                                                    <body>\n\
                                                    <h1 style=\"margin-top:10px\">index of</h1>\n";
                        autoIndexPage = "<div style=\"margin-left:5px\">\n\
                                                    <hr>\n";
                        entity = readdir(dir);
                        while (entity != NULL)
                        {
                            std::string filename = entity->d_name;
                            if (entity->d_type == DT_DIR)
                                filename += '/';
                            autoIndexPage.append("<p style=\"margin:0%\"><a href='" + filename + "' /> " + filename + " </p>");
                            autoIndexPage.append("<br>");
                            entity = readdir(dir);
                        }
                        closedir(dir);
                        autoIndexPage += "\
                                        <hr>\n\
                                        </div>\n\
                                        </body>\n\
                                        </html>\n";
                        this->ft_success_code("200", autoIndexPage, URI);
                    }
                }
                else
                {
                    URI += index;
                    status = stat(URI.data(), &buffer);
                    if (status != -1)
                    {
                        this->response_items.Extension = URI.substr(URI.rfind('.'));
                        if (!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
                            this->other_response("204", " NO Content");
                        else
                        {
                            if (this->get_permission(URI) == -1)
                                this->other_response("404", " No Permission");
                            else if (this->get_permission(URI) == -2)
                                this->other_response("403", " Forbidden");
                            else
                            {
                                std::string content_body = read_file(URI);
                                this->ft_success_code("200", content_body, URI);
                            }
                        }
                    }
                    else
                    {
                        URI += "index.html";
                        std::string content_body = read_file(URI.c_str());
                        if (access(URI.c_str(), F_OK | W_OK) != 0)
                        {
                            this->other_response("403", " Forbidden");
                            return;
                        }
                        this->ft_success_code("200", content_body, URI);
                    }
                }
            }
        }
        else
        {
            if (!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
            {
                // this->ft_success_code("200", body resturn by  CGI script ); // TODO: ADD CGI script
            }
            else
            {
                if (this->get_permission(URI) == -1)
                     this->other_response("404", " No Permission");
                else if (this->get_permission(URI) == -2 || this->response_items.Extension == "php" || this->response_items.Extension == "py")
                    this->other_response("403", " Forbidden");
                else
                {
                    std::string content_body = read_file(URI);
                    this->ft_success_code("200", content_body, URI);
                }
            }
        }
    }
    else
    {
        this->other_response("404", " Not Found");
    }
}

void Response::build_DELETE()
{
    struct stat buffer;
    int status;
    std::string URI = this->response_items.location->root;                 // change by value depends on location
    std::string get_auto_index = this->response_items.location->autoindex; // change by getter
    std::string cgi_path = this->response_items.location->cgi_path;        // change path with valid path from config;
    URI += this->response_items.Path.substr(1);
    status = stat(URI.c_str(), &buffer);
    if (status != -1)
    {
        if (this->response_items.Extension.empty())
        {
            if (this->response_items.Path[this->response_items.Path.size() - 1] == '/')
            {
                if (!cgi_path.empty())
                {
                    std::string index = check_index_file(URI);
                    if (index.empty())
                        this->other_response("403", " Forbidden");
                    else
                    {
                        // run cgi on requested file with DELTE REQUEST_METHOD;
                    }
                }
                else
                {
                    int res = remove_all_files(URI.c_str());
                    if (res != 0)
                        this->other_response("403", " Forbidden");
                    else
                        this->other_response("204", "  NO Content");
                }
            }
            else
                this->other_response("409", " Conflict");
        }
        else
        {
            if (cgi_path.empty())
            {
                remove(URI.c_str());
                this->other_response("204", " NO Content");
            }
            else
                std::cout << "still case with CGI" << std::endl;
        }
    }
    else
        this->other_response("404", " Not Found");
}

void Response::build_POST()
{

       
    struct stat buffer;
    int status;
    std::string upload_enable = this->response_items.location->upload_enable; // replace it by location value
    std::string upload_store_directory = this->response_items.location->upload_store_directory;
    ;                                                                      // replace it by location value
    std::string URI = this->response_items.location->root;                 // change by value depends on location
    std::string get_auto_index = this->response_items.location->autoindex; // change by getter
    std::string cgi_path = this->response_items.location->cgi_path;        // change path with valid path from config;

    URI += this->response_items.Path.substr(1);
    std::string index;
    int pos = 0;
    unsigned int k = 0;
    std::stringstream ss;

    status = stat(URI.c_str(), &buffer);
    if (upload_enable == "off")
    {
        if (status != -1)
        {
            if (!this->response_items.Extension.empty())
            {
                if (!cgi_path.empty())
                    std::cout << "Returtn Code Depend on cgi ";
                else
                    this->other_response("403", " Forbidden");
            }
            else
            {
                if (this->response_items.Path[this->response_items.Path.size() - 1] == '/')
                    this->ft_redirect("300", this->response_items.Path + "/");
                else
                {
                    index = this->check_index_file(URI);
                    if (index.empty())
                        this->other_response("403", " Forbidden");
                    else
                    {
                        if (cgi_path.empty())
                            this->other_response("403", " Forbidden");
                        else
                        {
                            URI += index;
                            std::cout << "Returtn Code Depend on cgi";
                        }
                    }
                }
            }
        }
        else
            this->other_response("404", " Not Found");
    }
    else
    {
           
        //   std::cout << "url : " << this->response_items.location->upload_store_directory << std::endl; 
        std::string namefile;
        time_t current_time;
        std::ofstream file;
        std::vector<RequestBody *>::iterator it;
        if (this->response_items.Headers.find("Content-Type")->second.find("multipart/form-data") != std::string::npos)
        {
            it = this->response_items.ChunkedBody.begin();
            while (k < this->response_items.ChunkedBody.size())
            {
                if (!(*it)->ContentType.empty())
                {
                    if (!(*it)->ContentDisposition.empty())
                    {
                        pos = (*it)->ContentDisposition.find("filename=");
                        if (pos != -1)
                        {
                            namefile = (*it)->ContentDisposition.substr(pos + 10);
                            namefile = namefile.substr(0, namefile.find("\""));
                            // namefile = namefile.substr(0, namefile.rfind("."));
                        }
                    }
                    else
                    {
                        time(&current_time);
                        ss << static_cast<int>(current_time);
                        namefile += ss.str();
                        namefile += ".txt";
                    }
                    // std::cout << trim((*it)->ContentType.substr((*it)->ContentType.find(":") + 1)) << std::endl;
                    // namefile += this->get_type(trim((*it)->ContentType.substr((*it)->ContentType.find(":") + 1) ));
                    // std::cout << namefile << std::endl;
                    file.open(this->response_items.location->upload_store_directory + namefile, std::ios::out | std::ios::binary);
                    namefile.clear();
                    if (!file.is_open())
                    {
                        this->other_response("505", " Version Not Supported");
                        return;
                    }
                    if (!(*it)->Content.empty())
                    {
                        const size_t BUFFER_SIZE = 3000;
                        size_t totalBytesWritten = 0;
                        while (totalBytesWritten < (*it)->Content.size()) {
                            size_t bytesToWrite = std::min(BUFFER_SIZE, (*it)->Content.size() - totalBytesWritten);
                            file.write((*it)->Content.data() + totalBytesWritten, bytesToWrite);

                            // Check if there was an error during the write operation
                            if (!file) {
                                std::cerr << "Error writing to the output file." << std::endl;
                                break;
                            }

                            totalBytesWritten += bytesToWrite;
                        }
                   }
                    file.close();
                }
                it++;
                k++;
            }
            this->other_response("201", "Created");
        }
        else
            this->other_response("202", "Accepted");
    }
}

int Response::get_permission(std::string &file)
{
    if (access(file.c_str(), F_OK) == -1)
        return -1;
    if (access(file.c_str(), R_OK) == -1)
        return -2;
    return 0;
}

std::string Response::read_file(const std::string &filename)
{
    std::ifstream file(filename);
    std::stringstream ss;
    if (!file.is_open())
    {
        return "errot opne file";
    }
    ss << file.rdbuf();
    file.close();
    return ss.str();
}

void Response::not_found()
{
    std::string status = "404";
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, this->Resource_not_found, ((this->response_items.error_pages.find(status))->second));
    response << "HTTP/1.1 404 NOT FOUND\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << this->Resource_not_found.length() << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Set-Cookie: yummy_cookie=choco\r\n";
    response << "Date: " << this->get_Date() << "\r\n\r\n";
    response << this->Resource_not_found;
}

int Response::remove_all_files(const char *dirname)
{
    struct dirent *entity;
    DIR *dir = opendir(dirname);
    std::string filename;
    ;

    if (dir == NULL)
        return -1;
    entity = readdir(dir);
    while (entity != NULL)
    {

        filename = dirname;
        if (entity->d_name[0] != '.')
        {
            filename += entity->d_name;
            if (access(filename.c_str(), F_OK | W_OK) == -1)
            {
                return 1;
            }
            remove(filename.c_str());
        }
        entity = readdir(dir);
        filename.clear();
    }
    closedir(dir);
    return 0;
}

std::string Response::trim(std::string original)
{
    unsigned int begin_index = 0;
    unsigned int i = 0;
    if (original.size() == 0)
        return "";
    while (isblank(original[i]) != 0 && i <= original.size())
    {
        begin_index++;
        i++;
    }
    i = original.size() - 1;
    while (isblank(original[i]) != 0 && i != 0)
        i--;
    if (begin_index == original.size())
        return "";
    return original.substr(begin_index, i + 1);
}

void Response::return_pages(std::string &pages_return, std::string &url)
{
    std::vector<std::string> pages = split_v(pages_return, " ");
    // std::vector<std::string>::iterator it;
    switch (atoi(pages[0].c_str()))
    {
        std::cout << "hi " << std::endl;
    case 200:
        this->ft_success_code(pages[0], pages[1], url);
        break;
    case 301:
        this->ft_redirect(pages[0], pages[1]);
        break;
    case 400:
        this->ft_bad_request(pages[0], " Bad Request");
        break;
    case 404:
        this->other_response(pages[0], " Not Found");
        break;
    case 403:
        this->ft_forbidden_request(pages[0], " Forbidden");
        break;
    case 500:
        this->other_response(pages[0], " Internal Server Error");
         break;
    default:
        this->other_response(pages[0], pages[1]);
        break;
    }
}

void Response::ft_default_pages(std::string status, std::string &message, std::string &path)
{
    struct stat buffer;
    std::string filename;
    if (stat(this->response_items.error_pages.find(status)->second.c_str(), &buffer) != -1)
    {
        if (path[0] != '/')
            path = "/" + path;
        filename = this->response_items.location->root + path;
        message = this->read_file(filename);
    }
}

void Response::ft_success_code(std::string status, std::string message, std::string URI)
{
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " ok\r\n";
    response << "Content-length: " << message.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Content-Type: " << this->get_Content_type(URI) << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Date: " << this->get_Date() << "\r\n\r\n";
    response << message;
}

void Response::ft_redirect(std::string status, std::string message)
{
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " Moved Permanently\r\n";
    response << "Location: " << message << "\r\n";
    response << "Content-Length: 0\r\n";
    response << "Connection: close\r\n";
    response << "Content-Type: "
             << "text/html"
             << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Date: " << this->get_Date() << "\r\n\r\n";
}

void Response::ft_bad_request(std::string status, std::string message)
{
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    std::cout << "OK" << std::endl;
    response << "HTTP/1.1 " << status << " Bad Request\r\n";
    response << "Content-Length: " << message.length() << "\r\n";
    response << "Server: " << this->response_items.server << "\r\n";
    response << "Connection: close\r\n";
    response << "Content-Type: "
             << "text/html"
             << "\r\n";
    response << "Date: " << this->get_Date() << "\r\n";
    response << "\r\n"; // Blank line to separate headers and body
    response << message;
}

void Response::ft_forbidden_request(std::string status, std::string message)
{
    std::string body;
    body = "<!DOCTYPE html>\n<html lang=\"en\">\n\
                                                    <head>\n\
                                                    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    body.append("<title>" + status + " </title>\n");
    body += "</head>\n\
                                                    <body>\n\
                                                    <div style=\"display:flex; align-items:center; justify-content:center;  flex-direction:column;\">\n\
                                                    <h1 style=\"margin-top:1px\"></h1>\n";
    body.append("<div style=\"display:flex; align-items:center; justify-content:center; \"> <h1 style=\"margin-top:0px\"> " + status + "</h1> </div>\n");
    body += "\
                                        <hr style=\"width:100%; color:blue;\">\n\
                                        <p style=\"margin-top:0px\"> webserver 1337 http/1.1</p>\n\
                                        </div>\n\
                                        </body>\n\
                                        </html>\n";
    // response << body ;
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " Forbiden\r\n";
    response << "Location: " << message << "/\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Content-Type: "
             << "text/html"
             << "\r\n";
    response << "Connection: close\r\n\r\n";
    response << body;
}

void Response::other_response(std::string status, std::string desc)
{
    std::string body;
    body = "<!DOCTYPE html>\n<html lang=\"en\">\n\
                                                    <head>\n\
                                                    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    body.append("<title>" + status + desc + " </title>\n");
    body += "</head>\n\
                                                    <body>\n\
                                                    <div style=\"display:flex; align-items:center; justify-content:center;  flex-direction:column;\">\n\
                                                    <h1 style=\"margin-top:1px\"></h1>\n";
    body.append("<div style=\"display:flex; align-items:center; justify-content:center; \"> <h1 style=\"margin-top:0px\"> " + status + desc + "</h1> </div>\n");
    body += "\
                                        <hr style=\"width:100%; color:blue;\">\n\
                                        <p style=\"margin-top:0px\"> webserver 1337 http/1.1</p>\n\
                                        </div>\n\
                                        </body>\n\
                                        </html>\n";
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, body, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " " << desc << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Server: " << this->response_items.server << "\r\n";
    response << "Connection: close\r\n";
    response << "Date: " << this->get_Date() << "\r\n";
    response << "\r\n"; // Blank line to separate headers and body
    response << body;
}