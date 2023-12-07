#include "./webserver.hpp"

// int main()
// {
// //     std::string requese = "GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// // Host: www.tutorialspoint.com
// // Accept-Language: en-us
// // Accept-Encoding: gzip, deflate
// // Connection: Keep-Alive";

//     Requese *req = new Requese ("DELETE /dir/ HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com:156\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate \r\nConnection: Keep-Alive\r\nDate: 2012-04-02\r\nContent-Type: multipart/form-data; boundary=NextField\r\nContent-Length: 80\r\n\r\nNextField\nContent-Disposition: form-data; name=”Job”\n100\nNextField\n2xbcbbjkhkbhdfbh999\nsfadksgfdksf" );
//     std::cout  << "-------------------------------- BEGIN PUBLIC" << std::endl;
//     Response res(req->status_response_code, req->Initial_Request_Line, req->response_items);
//     res.build_response();

// }


void show_data_server(server &server_data)
{
    std::cout << RED  << "server_name: " << server_data.server_name << std::endl;
    std::cout  << "host: " << server_data.host << std::endl;
    std::cout  << "port: " << server_data.port << std::endl;
    std::cout  << "max_body_size: " << server_data.max_body_size << std::endl;
    std::cout  << "Default path: " << server_data.default_location << std::endl;
    std::cout  << "error_pages: " << std::endl;
    std::map<std::string, std::string>::iterator it2 = server_data.error_pages.begin();
    while (it2 != server_data.error_pages.end())
    {
        /*std::cout  << it2->first << ": " << it2->second << std::endl;*/
        it2++;
    }
    std::map<std::string, location>::iterator it3 = server_data.locations.begin();
    while (it3 != server_data.locations.end())
    {
        std::cout  << "locations: " << it3->first << std::endl;
        std::cout  << "root: " << it3->second.root << std::endl;
        std::cout  << "index: " << it3->second.index << std::endl;
        std::cout  << "cgi_path: " << it3->second.cgi_path << std::endl;
        std::cout  << "autoindex: " << it3->second.autoindex << std::endl;
        std::cout  << "cgi_extension: " << it3->second.cgi_extension << std::endl;
        std::cout  << "fastcgi_pass: " << it3->second.fastcgi_pass << std::endl;
        std::cout  << "allowed_methods: " << it3->second.allowed_methods << std::endl;
        std::cout  << "return_code_url: " << it3->second.return_code_url << std::endl;
        std::cout  << "upload_store_directory: " << it3->second.upload_store_directory << RESET<< std::endl;
        it3++;
    }
}

std::string Get_response(server &server_data)
{
    // show_data_server(server_data);
    std::cout  << "-------------------------------- BEGIN PUBLIC" << std::endl;
    Requese *req = new Requese (server_data.request_content, server_data);
    Response res(req->status_response_code, req->Initial_Request_Line, req->response_items);
    return res.build_response();
}