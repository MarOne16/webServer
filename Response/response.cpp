#include "./webserver.hpp"

Response::Response(int status, std::vector<std::string> init_line, http_items &response_items)
{
    this->status = status;
    this->response_items = response_items;
    this->init_line = init_line;
}

void Response::ft_free(std::vector<RequestBody *>& arr)
{
    std::vector<RequestBody *>::iterator it = arr.begin();
    while(it != arr.end())
    {
        delete (*it);
        it++;
    }
}

std::string Response::build_response()
{
    if (this->status == 400)
        this->other_response("400", "Bad Request");
    else if (this->status == 505)
        this->other_response("505", "Version Not Supported");
    else if (this->status == 500)
        this->other_response("500", "Internal Server Error");
    else if (this->status == 405)
        this->other_response("405", "Method not allowed");
    else if (this->status == 414)
        this->other_response("414", "URI Too Long");
    else if (this->status == 413)
        this->other_response("413", "Content Too Large");
    else if (this->status == 411)
        this->other_response("411", "Length Required");
    else if (this->status == 501)
        this->other_response("501", "Not Implemented");
    else if (this->status == 404)
        this->other_response("404", "Not Found");
    else if (!this->response_items.location->return_code_url.empty())
        return_pages(this->response_items.location->return_code_url, this->response_items.Path);
    else if (this->response_items.method == "GET")
        this->build_GET();
    else if (this->response_items.method == "DELETE")
        this->build_DELETE();
    else if (this->response_items.method == "POST")
        this->build_POST();
    delete this->response_items.location;
    ft_free(this->response_items.ChunkedBody);
    return (response.str());
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
    URI += this->response_items.Path.substr(1);
    status = stat(URI.data(), &buffer);
    if (status != -1)
    {   
        if (this->response_items.Extension.empty())
        {
        
            if (this->response_items.Path[this->response_items.Path.size() - 1] != '/')
                this->ft_redirect("301", this->response_items.Path + "/");
            else
            {
                index = check_index_file(URI);
                if (index.empty())
                {
                    
                    if (get_auto_index == "off")
                    {
                        URI += "index.html";
                        if (access(URI.c_str(), F_OK | W_OK) != 0)
                        {
                            this->other_response("403", " Forbidden");
                            return;
                        }
                        this->ft_success_code("200",  read_file(URI.c_str()), URI);

                    }
                    else
                    {
                        
                        DIR *dir = opendir(URI.c_str());
                        struct dirent *entity;
                        if(dir != NULL)
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
                }
                else
                {
                    
                    URI += index;
                    this->response_items.Path += index;
                    status = stat(URI.data(), &buffer);
                    if (status != -1)
                    {
                        this->response_items.Extension = URI.substr(URI.rfind('.') + 1);
                        if (cgi_path != "" && (this->response_items.Extension == "php" || this->response_items.Extension == "py") )
                            responsecgi(GET_CGI_DATA(this->response_items));
                        else
                        {
                            if (this->get_permission(URI) == -1)
                                this->other_response("403", "  Forbidden");
                            else if (this->get_permission(URI) == -2)
                                this->other_response("403", " Forbidden");
                            else
                                this->ft_success_code("200",  read_file(URI), URI);
                        }
                    }
                    else
                    {
                        
                        URI += "index.html";
                        if (access(URI.c_str(), F_OK | W_OK) != 0)
                        {
                            this->other_response("403", " Forbidden");
                            return;
                        }
                        this->ft_success_code("200", read_file(URI.c_str()), URI);
                    }
                }
            }
        }
        else
        {
            if (cgi_path != " " && (this->response_items.Extension == "php" || this->response_items.Extension == "py") )
                            responsecgi(GET_CGI_DATA(this->response_items));
            else
            {
                if (this->get_permission(URI) == -1)
                     this->other_response("404", " No Permission");
                else if (this->get_permission(URI) == -2)
                    this->other_response("403", " Forbidden");
                else
                {
                    this->ft_success_code("200", read_file(URI), URI);
                    return;
                }
            }
        }
    }
    else
        this->other_response("404", " Not Found");
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
                if (cgi_path != "")
                {
                    
                    std::string index = check_index_file(URI);
                    this->response_items.Path += index;
                    if (index.empty())
                        this->other_response("403", " Forbidden");
                    else
                        this->other_response("405", "Method Not Allowed");
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
                if(access(URI.c_str(), F_OK | W_OK) == 0)
                {
                    remove(URI.c_str());
                    this->other_response("204", " NO Content");
                    
                }
                else
                    this->other_response("403", " Forbidden");
                return ;
            }
            else
                this->other_response("405", "Method Not Allowed");
        }
    }
    else
        this->other_response("404", " Not Found");
}

void Response::build_POST()
{
    struct stat buffer;
    int status;
    std::string upload_enable = this->response_items.location->upload_enable;
    std::string upload_store_directory = this->response_items.location->upload_store_directory;
    std::string URI = this->response_items.location->root;                 
    std::string get_auto_index = this->response_items.location->autoindex; 
    std::string cgi_path = this->response_items.location->cgi_path;        
    std::string index;
    int pos = 0;
    unsigned int k = 0;
    std::stringstream ss;
    const size_t BUFFER_SIZE = 3000;
    std::string namefile;
    time_t current_time;
    std::ofstream file;
    std::vector<RequestBody *>::iterator it;
    URI += this->response_items.Path.substr(1);
    status = stat(URI.c_str(), &buffer);
    if (upload_enable == "off")
    {
          
        if (status != -1)
        {
            if (!this->response_items.Extension.empty())
            {
     
                if (!cgi_path.empty())
                    responsecgi(GET_CGI_DATA(this->response_items));
                else
                {

                    this->other_response("403", " Forbidden");
                }
            }
            else
            {
                if (this->response_items.Path[this->response_items.Path.size() - 1] != '/')
                    this->ft_redirect("301", this->response_items.Path + "/");
                else
                {
                    index = this->check_index_file(URI);
                    this->response_items.Path += index;
                    if (index.empty())
                        this->other_response("403", " Forbidden");
                    else
                    {
                        if (cgi_path.empty())
                            this->other_response("403", " Forbidden");
                        else
                        {
                            // URI += index;
                            responsecgi(GET_CGI_DATA(this->response_items));

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
        is_path_outside_directoryy(upload_store_directory, URI);
        if(this->status != 200)
        {
            this->other_response("400", " FORBIDDEN");
            return;
        }
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
                        }
                    }
                    else
                    {
                        time(&current_time);
                        ss << static_cast<int>(current_time);
                        namefile += ss.str();
                        namefile += ".txt";
                    }
                    file.open(this->response_items.location->upload_store_directory + namefile, std::ios::out | std::ios::binary);
                    namefile.clear();
                    if (!file.is_open())
                    {
                        this->other_response("500", "Internal Server Error");
                        return;
                    }
                    if (!(*it)->Content.empty())
                    {
                        size_t totalBytesWritten = 0;
                        while (totalBytesWritten < (*it)->Content.size()) {
                            size_t bytesToWrite = std::min(BUFFER_SIZE, (*it)->Content.size() - totalBytesWritten);
                            file.write((*it)->Content.data() + totalBytesWritten, bytesToWrite);
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
        {
            time(&current_time);
            ss << static_cast<int>(current_time);
            namefile += ss.str();
            namefile += ".";
            namefile += "txt";
            file.open(this->response_items.location->upload_store_directory + namefile, std::ios::out | std::ios::binary);
            namefile.clear();
            if (!file.is_open())
            {
                this->other_response("500", "Internal Server Error");
                return;
            }
            file << this->response_items.Body;
            this->other_response("201", "Created");
        }
    }
}


void Response::return_pages(std::string &pages_return, std::string &url)
{
    std::vector<std::string> pages = split_v(pages_return, " ");
    if(url == pages[1])
    {
        this->ft_success_code(pages[0], read_file(pages[1]), url);
        return;
    }
    
     switch (atoi(pages[0].c_str()))
    {
    case 200:
        this->ft_success_code(pages[0], read_file(pages[1]), url);
        break;
    case 301:
        this->ft_redirect(pages[0], pages[1]);
        break;
    case 302:
        this->ft_redirect(pages[1], pages[1]);
        break;
    case 400:
        this->other_response(pages[0], " Bad Request");
        break;
    case 404:
        this->other_response(pages[0], " Not Found");
        break;
    case 403:
        this->other_response(pages[0], " Forbidden");
        break;
    case 500:
        this->other_response(pages[0], " Internal Server Error");
         break;
    default:
        this->other_response(pages[0], pages[1]);
        break;
    }
}



void Response::ft_success_code(std::string status, std::string message, std::string URI)
{
    std::string connection = (!this->response_items.Headers["Connection"].empty() ? this->response_items.Headers["Connection"] : "close");
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " ok\r\n";
    response << "Content-length: " << message.length() << "\r\n";
    response << "Connection:" << connection  <<  "\r\n";
    response << "Content-Type: " << this->get_Content_type(URI) << "\r\n";
    response << "Host: " << this->response_items.server_name << "\r\n";
    response << "Set-Cookie: yummy_cookie=darkmod;  Path=/websites/;\r\n";
    response << "Date: " << this->get_Date() << "\r\n\r\n";
    response << message;
   
    
}

void Response::ft_redirect(std::string status, std::string message)
{
     std::string connection = (!this->response_items.Headers["Connection"].empty() ? this->response_items.Headers["Connection"] : "close");
    response << "HTTP/1.1 " << status << " Moved Permanently\r\n";
    if (message.find("http", 0) == 0)
        response << "Location: " << message << "\r\n";
    else
    {
        if(message[0] != '/')
            response << "Location: " << "/" << message << "\r\n";
        else
             response << "Location: " << message << "\r\n";
    }
    response << "Content-Length: 0\r\n";
    response << "Connection:" << connection <<  "\r\n";
    response << "Content-Type: "<< "text/html" << "\r\n";
    response << "Host: " << this->response_items.server_name << "\r\n";
    response << "Date: " << this->get_Date() << "\r\n\r\n";
}


void Response::other_response(std::string status, std::string desc)
{
    std::string body;
    std::string connection = (!this->response_items.Headers["Connection"].empty() ? this->response_items.Headers["Connection"] : "close");
    body = "<!DOCTYPE html>\n<html lang=\"en\">\n\
                                                    <head>\n\
                                                    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    body.append("<title>" + status + " " +  desc + " </title>\n");
    body += "</head>\n\
                                                    <body>\n\
                                                    <div style=\"display:flex; align-items:center; justify-content:center;  flex-direction:column;\">\n\
                                                    <h1 style=\"margin-top:1px\"></h1>\n";
    body.append("<div style=\"display:flex; align-items:center; justify-content:center; \"> <h1 style=\"margin-top:0px\"> " + status + "  " + desc + "</h1> </div>\n");
    body += "\
                                        <hr style=\"width:100%; color:blue;\">\n\
                                        <p style=\"margin-top:0px\"> webserver 1337 http/1.1</p>\n\
                                        </div>\n\
                                        </body>\n\
                                        </html>\n";
    if (this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
    {
        ft_default_pages(status, body, (this->response_items.error_pages.find(status)->second));
    }
    response << "HTTP/1.1 " << status << " " << desc << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Server: " << this->response_items.server_name << "\r\n";
    response << "Connection:" << connection  <<  "\r\n";
    response << "Date: " << this->get_Date() << "\r\n";
    response << "\r\n"; // Blank line to separate headers and body
    response << body;
}