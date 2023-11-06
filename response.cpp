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
        response << "\r\n"; // Blank line to separate headers and body
        response << this->bad_req;
        // std::cout << this->response.str() << std::endl;
    }
    if(this->status == 505)
    {
        response << "HTTP/1.1 505 Version Not Supported\r\n";
        response << "Content-Type: text/html\r\n";
        response << "Content-Length: " << this->HTTP_NOT_SUPPORTED.length() << "\r\n";
        response << "\r\n"; // Blank line to separate headers and body
        response << this->HTTP_NOT_SUPPORTED;
    }
    if(this->response_items.method == "GET")
    {
       struct stat buffer;
        int         status;
        std::string URI = "/mnt/d/web_server/root";

        URI += this->response_items.Path;
        status = stat(URI.c_str() ,  &buffer);
        if(status != -1)
        {
            response << "HTTP/1.1 200 ok\r\n";
        }
        else
        {
            response << "HTTP/1.1 404 ok\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << this->Resource_not_found.length() << "\r\n";
        response << "\r\n"; // Blank line to separate headers and body
        response << this->Resource_not_found;
        }
    }
    std::cout << response.str() << std::endl;
}