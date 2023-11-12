#include "./webserver.hpp"



Response::Response(int status, std::vector<std::string> init_line,  http_items response_items)
{
    // this->status = status;
    this->status = status;
    this->response_items = response_items;
    this->init_line = init_line; 
    this->bad_req =  "<html><body><h1>400 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    this->HTTP_NOT_SUPPORTED = "<html><body><h1>500 Bad Request</h1><p>Your request could not be understood by the server.</p></body></html>";
    this->Resource_not_found = "<html><body><h1>404 Bad Request</h1><p> URI Not found </p></body></html>";
} 

void Response::build_response()
{
    std::cout << this->status << " <<<<"  << std::endl;
    if(this->status == 400)
    {
        response << "HTTP/1.1 400 Bad Request\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << this->bad_req.length() << "\r\n";
        response << "Server: " << this->response_items.server << "\r\n";
        response << "Date: " << this->get_Date() <<  "\r\n";
        response << "\r\n"; // Blank line to separate headers and body
        response << this->bad_req;
        // std::cout << this->response.str() << std::endl;
    }
    else if (this->status == 505)
    {
        response << "HTTP/1.1 505 Version Not Supported\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << this->HTTP_NOT_SUPPORTED.length() << "\r\n";
        response << "Server: " << this->response_items.server << "\r\n";
         response << "Date: " << this->get_Date() <<  "\r\n";
        response << "\r\n"; // Blank line to separate headers and body
        response << this->HTTP_NOT_SUPPORTED;
    }
    else if(this->response_items.method == "GET")
    {
        this->build_GET();
    }
    else if(this->response_items.method == "DELETE")
    {
        std::cout << "DELETE METHOD" << std::endl;
        this->build_DELETE();
    }
    else if(this->response_items.method == "POST")
    {
        std::cout << "POST METHOD" << std::endl;
        this->build_POST();
    }
    std::cout << response.str() << std::endl;
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
   return str.str();
}

std::string Response::check_index_file()
{
    DIR *dir = opendir("root/dir/");
    std::string files[] = {"index", "index.html", "index.php"}; // change by value depends on location
    if(dir == NULL)
        return "";
    struct dirent* entity;
    int i = 0;
    while(i < files->length())
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
        std::string URI = "root"; //change by value depends on location

        URI += this->response_items.Path;
        status = stat("root/dir/",  &buffer);
        if(status != -1)
        {
            if(this->response_items.Extension.empty() == 1)
            {
                if(this->response_items.Path[this->response_items.Path.size() - 1] != '/')
                {
                    response << "HTTP/1.1 301 Moved Permanently\r\n";
                    response << "Location: " << this->response_items.Path << "/\r\n";
                    response << "Content-Length: 0\r\n";
                    response << "Connection: close\r\n\r\n";
                } 
                else
                {
                    std::string index  =  this->check_index_file();
                    std::cout << "---------------> :" << index << "\r\n";
                    std::string get_auto_index = "on"; // change by getter 
                    if(index.empty())
                    {
                        DIR *dir = opendir(this->response_items.Path.c_str());
                        struct dirent* entity;
                        if(get_auto_index == "off")
                        {
                            response << "HTTP/1.1 403 Forbiden\r\n";
                            response << "Location: " << this->response_items.Path << "/\r\n";
                            response << "Content-Length: 15\r\n";
                            response << "Connection: close\r\n\r\n";
                            response << "not found file";
                        }
                        else
                        {
                          
                            response << "HTTP/1.1 200 ok\r\n";
                            response << "Content-Length: 0\r\n";
                            response << "Connection: close\r\n";
                            response << "Date: " << this->get_Date()<< "\r\n\r\n";
                            
                            std::string autoIndexPage;
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
                                puts("hi");
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
                            response << autoIndexPage;
                            // add body for index
                        }

                    } 
                    else{
                        std::string cgi_path = "fgdasfiudfa"; //change path with valid path from config;
                        if(!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
                        {
                            response << "HTTP/1.1 200 ok\r\n";
                            response << "Content-Length: 0\r\n";
                            response << "Connection: close\r\n";
                            response << "Date: " << this->get_Date() << "\r\n\r\n";
                            response << "resturn of CGI";
                        }
                        else
                        {
                            URI +=index;
                            if(this->get_permission(URI) == -1)
                                this->not_found();
                             else if(this->get_permission(URI) == -2 ||  this->response_items.Extension == "php" || this->response_items.Extension == "py")
                            {
                                response << "HTTP/1.1 403 Forbidden\r\n";
                                response << "Content-Type: text/html\r\n";
                                response << "Content-Length: 46"<< "\r\n";
                                response << "Server: " << this->response_items.server << "\r\n";
                                response << "Date: " << this->get_Date() <<  "\r\n";
                                response << "\r\n"; // Blank line to separate headers and body
                                response << "You don't have permission to access the requested resource.";

                            }
                            else
                            {
                                //file with permissions
                                std::string content_body = read_file(URI);
                                response << "HTTP/1.1 200 ok\r\n";
                                response << "Content-Length: "<< content_body.length()<<"\r\n";
                                response << "Connection: close\r\n";
                                response << "Date: " << this->get_Date() << "\r\n\r\n";
                                response << content_body;
                            }

                            
                        }                
                        } 

                }
            }
            else
            {
                 std::string cgi_path = ""; //change path with valid path from config;
                if(!cgi_path.empty() && (this->response_items.Extension == "php" || this->response_items.Extension == "py"))
                {
                    response << "HTTP/1.1 200 ok\r\n";
                    response << "Content-Length: 0\r\n";
                    response << "Connection: close\r\n";
                    response << "Date: " << this->get_Date()<< "\r\n\r\n";
                    response << "resturn of CGI";
                }
                else
                {
                     if(this->get_permission(URI) == -1)
                            this->not_found();
                            else if(this->get_permission(URI) == -2 ||  this->response_items.Extension == "php" || this->response_items.Extension == "py")
                            {
                                response << "HTTP/1.1 403 Forbidden\r\n";
                                response << "Content-Type: text/html\r\n";
                                response << "Content-Length: 46"<< "\r\n";
                                response << "Server: " << this->response_items.server << "\r\n";
                                response << "Date: " << this->get_Date() <<  "\r\n";
                                response << "\r\n"; // Blank line to separate headers and body
                                response << "You don't have permission to access the requested resource.";

                            }
                            else
                            {
                                std::string content_body = read_file(URI);
                                response << "HTTP/1.1 200 ok\r\n";
                                response << "Content-Length: "<< content_body.length()<<"\r\n";
                                response << "Connection: close\r\n";
                                response << "Date: " << this->get_Date() << "\r\n\r\n";
                                response << content_body;
                            }
                    
                }
            }
        }
        else
            this->not_found();


}


void Response::build_DELETE()
{
    struct stat buffer;
    int         status;
    std::string URI = "root"; //change path with valid path from config;
    std::cout << "DELETE HI "  << std::endl;
    URI += this->response_items.Path;
    status = stat(URI.c_str() ,  &buffer);
    if(status != -1)
    {
     if(this->response_items.Extension.empty())
     {
        if(this->response_items.Path[this->response_items.Path.size() - 1] == '/')
        {
            std::string cgi_path = ""; //change path with valid path from config;
            if(!cgi_path.empty())
            {
                std::string index = check_index_file();
                if(index.empty())
                {
                    response << "HTTP/1.1 403 Forbidden\r\n";
                    response << "Content-Type: text/plain" << "\r\n";
                    response << "Content-length: 0" << "\r\n";
                    response << "Connection: close\r\n";
                    response  << "Date: " << this->get_Date() << "\r\n\r\n";

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
                    std::cout << "ERROR" << std::endl;
                    response << "HTTP/1.1 403 Forbidden\r\n";
                    response << "Content-Type: text/plain" << "\r\n";
                    response << "Content-length: 0" << "\r\n";
                    response << "Connection: close\r\n";
                    response  << "Date: " << this->get_Date() << "\r\n\r\n";
                }
                else
                {
                    response << "HTTP/1.1 204 NO Content\r\n";
                    response << "Content-Type: text/plain" << "\r\n";
                    response << "Content-length: 0" << "\r\n";
                    response << "Connection: close\r\n";
                    response  << "Date: " << this->get_Date() << "\r\n\r\n";
                }
            }
        }
        else 
        {
                response << "HTTP/1.1 409 Conflict\r\n";
                response << "Location: " << this->response_items.Path << "/\r\n";
                response << "Content-Length: 314\r\n";
                response << "Content-Type: text/plain" << "\r\n";
                response << "Connection: close\r\n";
                response  << "Date: " << this->get_Date() << "\r\n\r\n";
                response << "<h1>409 Conflict:<1><p>The requested operation cannot be completed due to a conflict with the current state of the resource.<p>";
        }
     }
     else
       {
        std::string cgi_path = ""; //change path with valid path from config;
        if(cgi_path.empty())
        {
            // std::cout << URI << std::endl; 
            // URI +=this->response_items.Path;
            remove(URI.c_str());
            response << "HTTP/1.1 204 NO Content\r\n";
            response << "Content-Type: text/plain" << "\r\n";
            response << "Content-length: 0" << "\r\n";
            response << "Connection: close\r\n";
            response  << "Date: " << this->get_Date() << "\r\n\r\n";
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
    std::string upload_enable = "on"; // replace it by location value
    std::string upload_store_directory ="/root"; // replace it by location value
    std::string URI = "root"; // get root form config;
    std::string cgi_path = "adsgfjdsg"; // get valid value form config
    std::cout << "POST HI "  << std::endl;
    URI += this->response_items.Path;
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
                    response << "HTTP/1.1 403 Forbiden\r\n";
                        response << "Location: " << this->response_items.Path << "/\r\n";
                        response << "Content-Length: 0\r\n";
                        response << "Content-Type: text/html";
                        response << "Connection: close\r\n\r\n";
                        // response << "";
                }
            }
            else
            {
                if(this->response_items.Path[this->response_items.Path.size() - 1] == '/')
                {
                    response << "HTTP/1.1 301 Moved Permanently\r\n";
                    response << "Location: " << this->response_items.Path << "/\r\n";
                    response << "Content-Length: 0\r\n";
                    response << "Connection: close\r\n\r\n";
                }
                else
                {
                    std::string index = this->check_index_file();
                    if(index.empty())
                    {
                        response << "HTTP/1.1 403 Forbiden\r\n";
                        response << "Location: " << this->response_items.Path << "/\r\n";
                        response << "Content-Length: 0\r\n";
                        response << "Content-Type: text/html";
                        response << "Connection: close\r\n\r\n";
                        // response << "";
                    }
                    else{
                        if(cgi_path.empty())
                        {
                            response << "HTTP/1.1 403 Forbiden\r\n";
                            response << "Location: " << this->response_items.Path << "/\r\n";
                            response << "Content-Length: 0\r\n";
                            response << "Content-Type: text/html";
                            response << "Connection: close\r\n\r\n"; 
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
   
        if(this->response_items.Headers.find("Content-Type")->second.find("multipart/form-data") != -1)
        {
                std::vector<RequestBody*>::iterator it;
                it = this->response_items.ChunkedBody.begin();
                std::string namefile;
                std::ofstream file;
                std::cout << "lenght of ChunkBody" <<  this->response_items.ChunkedBody.size() << std::endl;
                int pos = 0;
                int k = 0;
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
                        else
                        {

                            std::cout << "not open file make response to handle error";
                            return;
                        }
                        file.open(namefile);
                        if(!file.is_open())
                        {
                            std::cout << "not open file make response to handle error";
                        }
                        
                        
                    }
                    if(!(*it)->Content.empty())
                    {
                        file << (*it)->Content;
                    }
                        std::cout << "Content : " << (*it)->Content << std::endl;
                        std::cout << "ContentDisposition : " << (*it)->ContentDisposition << std::endl;
                     file.clear();
                    it++;
                    k++;
                }
        }
            //  std::cout << "supported :" << std::endl;
        response <<  "HTTP/1.1 202 Accepted" << "\r\n";
        response << "Content-Length: 0" <<  "\r\n";


    }
         std::cout << "uplaod the Post Request Body" << std::endl;
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
    return ss.str();

}


void  Response::not_found()
{
    std::string body = "<h1 style='color:red'>Error 404</h1><h2>OOPS! PAGE NOT FOUND :-(</h2><p>Sorry, the page you're looking for doesn't exist.</p>";

    response << "HTTP/1.1 404 NOT FOUND\r\n";
    response << "Content-Type: text/html\r\n";
    response << "Content-Length: " << body.length() << "\r\n";
    response << "Server: " << this->response_items.server << "\r\n";
    response << "Date: " << this->get_Date() <<  "\r\n\r\n";
    response << body;
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
        remove(entity->d_name);
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