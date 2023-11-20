#include "./webserver.hpp"



Response::Response(int status, std::vector<std::string> init_line,  http_items& response_items)
{
    // this->status = status;
    this->status = status;
    this->response_items = response_items;
    this->init_line = init_line;
    this->forbidden_req ="<html><head><title>403 Forbidden</title></head><body><h1>403 Forbidden</h1><p>Access to this resource is forbidden.</p></body></html>";
    this->bad_req = "<html><head><title>400 bad request</title></head><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    this->HTTP_NOT_SUPPORTED = "<html><head><title>500 Internal Server Error</title></head><body><h1>500 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    this->Resource_not_found = "<html><body><head><title>404 Not Found</title></head><h1>404 Not Found</h1><p> Sorry, the page you're looking for doesn't exist. </p></body></html>";
} 

std::string Response::build_response()
{
    
    if(this->status == 400)
    {
        ft_bad_request("400", this->bad_req);
    }
    else if (this->status == 505)
         this->other_response("505", "Version Not Supported", this->HTTP_NOT_SUPPORTED);
    else if(this->status == 405)
        {
            this->other_response("405", "Method not allowed", "<html><head><title>405 Method Not Allowed</title></head><body><h1>405 Method Not Allowed</h1><p>The requested method is not allowed for this resource.</p></bod></html>");
            // std::cout << "HTTP Method not allowed\r\n";
            // exit(1);
        }
    else if(this->status == 413)
        this->other_response("413", "Request-URI Too Long", "");
    else if(this->status == 505)
        this->other_response("505", "Not Implemented", "");
    else if(this->status == 411)
        this->other_response("411", "Length Required", "");
    else if(this->response_items.method == "GET")
    {
        this->build_GET();
    }
    else if(this->response_items.method == "DELETE")
    {
        this->build_DELETE();
    }
    else if(this->response_items.method == "POST")
    {
        this->build_POST();
    }
    return  (response.str());
}



std::string Response::get_Content_type()
{
    std::string extension = this->response_items.Path.substr(this->response_items.Path.rfind(".") + 1);
	// if (_request.get_header_value("Content-Type:").size())
	// 	return _request.get_header_value("Content-Type:");
	if (extension.compare("html") == 0 || extension.compare("php") == 0)
		return "text/html; charset=UTF-8";
	else if (extension.compare("json") == 0)
		return "application/json";
	else if (extension.compare("ico") == 0)
		return "image/x-icon";
	else if (extension.compare("jpeg") == 0)
		return "image/jpeg";
	else if (extension.compare("jpg") == 0)
		return "image/jpg";
	else
		return "text/plain";
}

std::string Response::get_Content_type(std::string extension)
{
	// if (_request.get_header_value("Content-Type:").size())
	// 	return _request.get_header_value("Content-Type:");
	if (extension.compare("text/html; charset=UTF-8") == 0 || extension.compare("text/html;") == 0)
		return ".html";
	else if (extension.compare("application/json") == 0)
		return ".json";
	else if (extension.compare("image/x-icon") == 0)
		return ".ico";
	else if (extension.compare("image/jpeg") == 0)
		return "jpeg";
	else if (extension.compare("mage/jpg") == 0)
		return ".jpg";
	else
		return ".txt";
}


std::string Response::get_Date()
{
    time_t now = time(0);
    std::stringstream str;
   
   // convert now to string form
   char* dt = ctime(&now);
   str << dt;
   return str.str().substr(0, str.str().size() -  1);
}

std::string Response::check_index_file(std::string & url)
{
    DIR *dir = opendir(url.c_str());
    std::vector<std::string>files = split_v(this->response_items.location->index, " "); // change by value depends on location
    if(dir == NULL)
        return "";
    struct dirent* entity;
    unsigned int i = 0;
    while(i < files.size())
    {
        entity = readdir(dir);
        while(entity != NULL)
        {
            // std::cout << entity->d_name << std::endl;
            if(entity->d_name == files[i])
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
        int         status;
        std::string URI = this->response_items.location->root;//change by value depends on location
        std::string get_auto_index = this->response_items.location->autoindex; // change by getter 
        std::string cgi_path = this->response_items.location->cgi_path; //change path with valid path from config;
        std::string index;
        std::string autoIndexPage;

        URI += this->response_items.Path.substr(1); // TODO : check if path is beging with /
         std::cout << "here :" << URI << std::endl;
        if(!this->response_items.location->return_code_url.empty())
        {
            return_pages(this->response_items.location->return_code_url); // TODO: check if this redirected response work
            return;
        }
        status = stat(URI.data(),  &buffer);
        if(status != -1)
        {
            if(this->response_items.Extension.empty() == 1)
            {
                if(this->response_items.Path[this->response_items.Path.size() - 1] != '/')
                    this->ft_redirect("300", this->response_items.Path + "/");
                else
                {
                    index  =  check_index_file(URI);
                    if(index.empty())
                    {
                        DIR *dir = opendir(this->response_items.Path.c_str());
                        struct dirent* entity;
                        if(get_auto_index == "off")
                        {
                           this->ft_forbidden_request("403", this->forbidden_req);
                        }
                        else
                        {
                          
                                    autoIndexPage = "<!DOCTYPE html>\n<html lang=\"en\">\n\
                                                    <head>\n\
                                                    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
                                                    <title>AUTO INDEX</title>\n\
                                                    </head>\n\
                                                    <body>\n\
                                                    <div style=\"margin-left: 5%; margin-top:10%;\">\n\
                                                    <hr>\n";
                            entity = readdir(dir);
                            while(entity != NULL)
                            {
                                std::string filename = entity->d_name;
                                if(entity->d_type == DT_DIR)
                                    filename += '/';
                                autoIndexPage.append("<p><a href='" + filename +"' /> " + filename + " </p>");
                                autoIndexPage.append("<br>");
                                entity = readdir(dir);
                            }   
                            closedir(dir);
                            autoIndexPage += "\
                                        <hr>\n\
                                        </div>\n\
                                        </body>\n\
                                        </html>\n";
                            this->ft_success_code("200", autoIndexPage);
                           
                            // add body for index
                        }

                    } 
                    else{
                        URI +=index;
                        status = stat(URI.data(), &buffer);
                        if( status != -1)
                        {
                            this->response_items.Extension = URI.substr(URI.rfind('.'));
                            if(!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
                            {
                                this->other_response("204", "NO Content", "index page default");
                            }
                            else
                            {
                                if(this->get_permission(URI) == -1)
                                    this->not_found();
                                else if(this->get_permission(URI) == -2)
                                {
                                    this->ft_forbidden_request("403", this->forbidden_req);

                                }
                                else
                                {
                                    std::string content_body = read_file(URI);
                                    this->ft_success_code("200", content_body);
                                }
                            }                
                        }
                        else
                        {
                            std::cout << "defaultpage " << std::endl;
                            std::string content_body = read_file(URI);
                            this->ft_success_code("200", content_body);
                        }
                        } 
                }
            }
            else
            {
                //  std::string cgi_path = ""; //change path with valid path from config;
                if(!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
                {
                      // this->ft_success_code("200", body resturn by  CGI script );
                }
                else
                {
                     if(this->get_permission(URI) == -1)
                            this->not_found();
                    else if(this->get_permission(URI) == -2 ||  this->response_items.Extension == "php" || this->response_items.Extension == "py")
                    {
                         this->ft_forbidden_request("403", this->forbidden_req);

                    }
                    else
                    {
                        std::string content_body = read_file(URI);
                        this->ft_success_code("200", content_body);
                    }
                    
                }
            }
        }
        else
        {
           
            this->not_found();
        }


}


void Response::build_DELETE()
{
    struct stat buffer;
    int         status;
    std::string URI = this->response_items.location->root; //change by value depends on location
    std::string get_auto_index = this->response_items.location->autoindex; // change by getter 
    std::string cgi_path = this->response_items.location->cgi_path; //change path with valid path from config;
    std::cout << "DELETE HI "  << std::endl;
    URI += this->response_items.Path.substr(1);
    status = stat(URI.c_str() ,  &buffer);
    if(status != -1)
    {
     if(this->response_items.Extension.empty())
     {
        if(this->response_items.Path[this->response_items.Path.size() - 1] == '/')
        {
            if(!cgi_path.empty())
            {
                std::string index = check_index_file(URI);
                if(index.empty())
                {
                    this->ft_forbidden_request("403", this->forbidden_req);

                }
                else
                {
                    // run cgi on requested file with DELTE REQUEST_METHOD;
                }
            }
            else
            {
                int res = remove_all_files(URI.c_str());
                if(res == 1)
                {
                    this->ft_forbidden_request("403", this->forbidden_req);
                }
                else
                {
                    this->other_response("204", "  NO Content", "");
                }
            }
        }
        else 
        {
              this->other_response("409", " Conflict", "<html><head><title>409  Conflict</title></head><body><h1>409 Conflict:<1><p>The requested operation cannot be completed due to a conflict with the current state of the resource.<p><body><html>");
        }
     }
     else
       {
        if(cgi_path.empty())
        {
            remove(URI.c_str());
            this->other_response("204", "NO Content", "");
        }
        else
        {
            std::cout << "still case with CGI" << std::endl;
        }
       }
    }
    else
           this->not_found();
}


void Response::build_POST()
{
    
    struct stat buffer;
    int         status;
    std::string upload_enable = this->response_items.location->upload_enable; // replace it by location value
    std::string upload_store_directory = this->response_items.location->upload_store_directory;; // replace it by location value
    std::string URI = this->response_items.location->root; //change by value depends on location
    std::string get_auto_index = this->response_items.location->autoindex; // change by getter 
    std::string cgi_path = this->response_items.location->cgi_path; //change path with valid path from config;
   
  
    URI += this->response_items.Path.substr(1);
    std::cout << URI << std::endl;
    std::string index ;
    int pos = 0;
    unsigned int  k = 0;
    std::stringstream ss;
    
    status = stat(URI.c_str() ,  &buffer);
    if(upload_enable == "off")
    {
        std::cout << "status: " << buffer.st_mode << std::endl;
        if(status != -1)
        {
            if(!this->response_items.Extension.empty())
            {
                if(!cgi_path.empty())
                {
                        std::cout << "Returtn Code Depend on cgi ";
                }
                else
                {
                    this->ft_forbidden_request("403", this->forbidden_req);
                        // response << "";
                }
            }
            else
            {
                if(this->response_items.Path[this->response_items.Path.size() - 1] == '/')
                {
                    this->ft_redirect("300", this->response_items.Path + "/");
                    // response << "HTTP/1.1 301 Moved Permanently\r\n";
                    // response << "Location: " << this->response_items.Path << "/\r\n";
                    // response << "Content-Length: 0\r\n";
                    // response << "Connection: close\r\n\r\n";
                }
                else
                {
                    index = this->check_index_file(URI);
                    if(index.empty())
                    {
                          this->ft_forbidden_request("403", this->forbidden_req);
                        // response << "";
                    }
                    else{
                        if(cgi_path.empty())
                        {
                              this->ft_forbidden_request("403", this->forbidden_req);
                        }
                        else
                        {
                            URI += index;
                            std::cout << "Returtn Code Depend on cgi";
                        }

                    }

                }
            }
                // std::cout << "supported upload mode: " << this->response_items.Extension << std::endl;
        }
        else
        {
            this->not_found();
        }
    }
    else
    {
   
        if(this->response_items.Headers.find("Content-Type")->second.find("multipart/form-data")  != std::string::npos)
        {
                std::vector<RequestBody*>::iterator it;
                 std::string namefile;
                std::ofstream file;
                time_t current_time;
                it = this->response_items.ChunkedBody.begin();
                std::cout << "lenght of ChunkBody" <<  this->response_items.ChunkedBody.size() << std::endl;
                while(k < this->response_items.ChunkedBody.size())
                {
                    if(!(*it)->ContentDisposition.empty())
                    {
                        pos = (*it)->ContentDisposition.find("name=");
                        if(pos != -1)
                        {
                            namefile = (*it)->ContentDisposition.substr(pos + 5);
                            pos = (*it)->ContentDisposition.find("Content-Disposition=");
                            namefile += get_Content_type(trim((*it)->ContentDisposition.substr(pos + 21)));
                            std::cout << "file name :" << namefile << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "here is not valid content-disposition " << std::endl;
                        time(&current_time);
                        ss <<  static_cast<int>(current_time);
                        namefile += ss.str(); 
                        namefile  += ".txt";
                        std::cout <<  namefile  << "not open file make response to handle error";
                    }
                    file.open(namefile);
                    namefile.clear();
                    if(!file.is_open())
                    {
                       this->not_found();
                       return;
                    } 
                    if(!(*it)->Content.empty())
                    {
                        file << (*it)->Content;
                    }
                     file.close();
                    it++;
                    k++;
                }
        }
        this->other_response("200", "Accepted", "");
        // response <<  "HTTP/1.1 202 Accepted" << "\r\n";
        // response << "Content-Length: 0" <<  "\r\n";
    }
}



int Response::get_permission(std::string& file)
{
    if(access(file.c_str(), F_OK) == -1)
        return -1;
    if(access(file.c_str(), R_OK) == -1)
        return -2;
    return 0;
}

std::string Response::read_file(const std::string& filename)
{
    std::ifstream file(filename);
    std::stringstream ss;
    if(!file.is_open())
    {
        return "errot opne file";
    }
    ss << file.rdbuf();
    file.close();
    return ss.str();

}


void  Response::not_found()
{
    std::string status = "200";
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status,  this->Resource_not_found, ((this->response_items.error_pages.find(status))->second));
    response << "HTTP/1.1 404 NOT FOUND\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << this->Resource_not_found.length() << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Set-Cookie: yummy_cookie=choco\r\n"; 
    response << "Date: " << this->get_Date() <<  "\r\n\r\n";
    response << this->Resource_not_found;
}



int Response::remove_all_files(const char *dirname)
{
    struct dirent* entity;
     DIR *dir = opendir(dirname);
    if(dir == NULL)
        return -1;
    entity = readdir(dir);
    while(entity != NULL)
    {
        if(access(entity->d_name, W_OK) != 0)
            return 1;
         ;
        entity = readdir(dir);
    }
    closedir(dir);
    return 0;
    }

std::string Response::trim(std::string original)
{
    unsigned int begin_index = 0;
    unsigned int i = 0;
    if(original.size() == 0)
        return "";
    while(isblank(original[i]) != 0 && i <= original.size())
    {
        begin_index++;
        i++;
    }
    i = original.size() - 1;
    while(isblank(original[i]) != 0 && i != 0)
        i--;
     if(begin_index == original.size())
        return "";
    return original.substr(begin_index, i + 1);
}


void Response::return_pages(std::string& pages_return)
{
    std::vector<std::string> pages = split_v(pages_return, " ");
    // std::vector<std::string>::iterator it;
    switch (atoi(pages[0].c_str()))
    {
        std::cout << "hi " << std::endl;
        case 200 :
            // this->ft_success_code("200", pages[1]);
            break;
        case 301 :
            // this->ft_redirect(pages[0], pages[1]);
            break;
        case 400 :
            // this->ft_bad_request(pages[0], pages[1]);
            break;
        case 404 :
           this->not_found();
            break;
        case 403 :
            // this->ft_forbidden_request(pages[0], pages[1]);
        case 500 :
            std::cout << "error" << std::endl;  
            break;
        default :
           this->not_found();
            break;
    }
}

void Response::ft_default_pages(std::string status, std::string& message, std::string& path)
{
    struct stat buffer;
    std::string filename;
    if(stat(this->response_items.error_pages.find(status)->second.c_str(), &buffer) != -1)
        {
            if(path[0] != '/')
                path = "/" + path;
            filename = this->response_items.location->root +  path;
            message = this->read_file(filename);
        }
}

void Response::ft_success_code(std::string status , std::string message)
{
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 "<< status << " ok\r\n";
    response << "Content-length: "<< message.length() << "\r\n";
    response << "Connection: close\r\n";
    response << "Content-Type: " << "text/html" << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Date: " << this->get_Date()<< "\r\n\r\n";
    response << message;
}

void Response::ft_redirect(std::string status, std::string message)
{
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " <<  status <<  " Moved Permanently\r\n";
    response << "Location: " << message << "\r\n";
    response << "Content-Length: 0\r\n";
    response << "Connection: close\r\n";
    response << "Content-Type: " << this->get_Content_type() << "\r\n";
    response << "Host: " << this->response_items.server << "\r\n";
    response << "Date: " << this->get_Date()<< "\r\n\r\n";
}


void Response::ft_bad_request(std::string status, std::string message)
{
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status <<  " Bad Request\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << message.length() << "\r\n";
    response << "Server: " << this->response_items.server << "\r\n";
    response << "Connection: close\r\n";
    response << "Date: " << this->get_Date() <<  "\r\n";
    response << "\r\n"; // Blank line to separate headers and body
    response << message;
}

void Response::ft_forbidden_request(std::string status , std::string message)
{
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " Forbiden\r\n";
    response << "Location: " << message << "/\r\n";
    response << "Content-Length: "<< message.length() << "\r\n";
    response << "Content-Type: text/html";
    response << "Connection: close\r\n\r\n";
    response << message ;
}

void Response::other_response(std::string status, std::string message, std::string desc)
{
    if(this->response_items.error_pages.find(status) != this->response_items.error_pages.end())
        ft_default_pages(status, message, (this->response_items.error_pages.find(status)->second));
    response << "HTTP/1.1 " << status << " " << desc << "\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << message.length() << "\r\n";
    response << "Server: " << this->response_items.server << "\r\n";
    response << "Connection: close\r\n";
    response << "Date: " << this->get_Date() <<  "\r\n";
    response << "\r\n"; // Blank line to separate headers and body
    response << message;
}