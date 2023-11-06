#ifndef WEB_SERVER
#define WEB_SERVER

#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

        struct  RequestBody
       {
           std::string  ContentDisposition;
           std::string Content;
       };

        struct http_items
        {

            int chunked_body;
            std::string method;
            std::string Scheme_name;
            std::string Path;
            std::string Query_String ;
            std::string Fragment_iden;
            std::string http_version;
            std::map<std::string , std::string> Headers;
            std::vector<std::string> Req;
            int lenghtbody;
            std::string Body;
            std::vector<RequestBody*> ChunkedBody;
            std::string bondary;
            std::map<std::string , std::string> EncodedFormbody;
            std::string Extension;
       };

       
class Requese {
    private:
        // std::streamstring os;
        std::string req;
    public:
        http_items response_items;
        std::vector<std::string> Initial_Request_Line;
        int status_response_code;
       Requese(std::string req);
       void parser_init_line(std::string   Initial_Request_Line);
       void Headers_elements();
       int check_elemens(std::string& key);
       int is_alpha(std::string value);
       int check_date(std::string& value);
       int check_content_type(std::string& value);
        int check_more_element(std::string& key, std::string& value);
        std::string trim(std::string original);
        int check_host(std::string& value);
        int  check_Transfer_Encoding(std::string& value);
        int  check_connection(std::string& value);
    //    void  set_Initial_Request_line(std::string req) 
       class ErrorSyntax:  public std::exception{
            public:
                const char *what() const throw();

       };
};


class Response
{
    private:
        int status;
        http_items response_items;
        std::vector<std::string> init_line;
        std::stringstream response;
        std::string bad_req;
        std::string HTTP_NOT_SUPPORTED;
        std::string Resource_not_found;
    public:
        Response(int status,  std::vector<std::string> init_line,  http_items response_items);
        void build_response();
};


#endif