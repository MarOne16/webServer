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
    if(this->status == 505)
    {
        response << "HTTP/1.1 505 Version Not Supported\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << this->HTTP_NOT_SUPPORTED.length() << "\r\n";
        response << "Server: " << this->response_items.server << "\r\n";
         response << "Date: " << this->get_Date() <<  "\r\n";
        response << "\r\n"; // Blank line to separate headers and body
        response << this->HTTP_NOT_SUPPORTED;
    }
    if(this->response_items.method == "GET")
    {
        this->build_GET();
    }
    if(this->response_items.method == "DELETE")
    {
        std::cout << "DELETE METHOD" << std::endl;
        this->build_DELETE();
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
    DIR *dir = opendir("root/dir");
    std::string files[] = {"index", "index.html", "index.php"};
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
    }
    closedir(dir);
    return "";
}

void Response::build_GET()
{

       struct stat buffer;
        int         status;
        std::string URI = "/mnt/d/web_server/root";

        URI += this->response_items.Path;
        status = stat(URI.c_str() ,  &buffer);
        // std::cout << this->status << "|||||" << std::endl;
        if(status != -1)
        {
            // response << "HTTP/1.1 200 ok\r\n";
            // response << "Content-Type: " << this->get_Content_type() << "\r\n";
            // response << "Server: " << this->response_items.server << "\r\n";
            // response << "Date: " << this->get_Date() <<  "\r\n";
            // response << "\r\n";
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
                    std::string get_auto_index = "off"; // change by getter 
                    if(index.empty())
                    {
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
                            response << "Connection: close\r\n\r\n";
                            // add body for index
                        }

                    } 
                    else{
                            std::cout << "check cgi" << std::endl;
                    }                   // // if()

                }
            }
            else
            {
                std::cout << "---------->" << this->response_items.Extension << std::endl;
                std::cout << "is file check cgi " << std::endl;
            }
        }
        else
        {
            response << "HTTP/1.1 404 ok\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << this->Resource_not_found.length() << "\r\n";
            response << "Server: " << this->response_items.server << "\r\n";
            response << "Date: " << this->get_Date() <<  "\r\n";
            response << "\r\n"; // Blank line to separate headers and body
            response << this->Resource_not_found;
        }
}



void Response::build_DELETE()
{
    struct stat buffer;
    int         status;
    std::string URI = "/mnt/d/web_server/root";

    URI += this->response_items.Path;
    status = stat(URI.c_str() ,  &buffer);
    if(status != -1)
    {
     if(this->response_items.Extension.empty())
     {
        if(this->response_items.Path[this->response_items.Path.size() - 1] == "/");
        {

        }
        else 
        {
                response << "HTTP/1.1 409 Conflict\r\n";
                response << "Location: " << this->response_items.Path << "/\r\n";
                response << "Content-Length: 0\r\n";
                response << "Connection: close\r\n\r\n";
        }
     }
     else
        std::cout << "is file" << std::endl;
    }
    else
    {
            response << "HTTP/1.1 404 ok\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << this->Resource_not_found.length() << "\r\n";
            response << "Server: " << this->response_items.server << "\r\n";
            response << "Date: " << this->get_Date() <<  "\r\n";
            response << "\r\n"; // Blank line to separate headers and body
            response << this->Resource_not_found; 
    }
}