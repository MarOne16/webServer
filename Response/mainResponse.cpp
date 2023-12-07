#include "./webserver.hpp"


std::string Get_response(server &server_data)
{
    
    // std::cout  << "-------------------------------- BEGIN PUBLIC" << std::endl;
    Requese *req = new Requese (server_data.request_content, server_data);
    Response res(req->status_response_code, req->Initial_Request_Line, req->response_items);
    delete req;
    return res.build_response();
}