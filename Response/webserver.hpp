#ifndef WEB_SERVER
#define WEB_SERVER

#include "../cgi_bin/cgi.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <exception>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <dirent.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../serveurs-clients/servers.hpp"
// #include "../cgi/cgi.hpp"


// typedef struct cgi_data
// {
//     http_items &response_tools;
//     envirmoment env_server;
//     std::string body;
//     std::string cgi_response;
//     unsigned int status_code;
//     std::string status_message;
//     std::map<std::string, std::string> cgi_headers;

//     // Constructor with initializer list to initialize the reference member
//     cgi_data(http_items &rt, const envirmoment &es, const std::string &b)
//         : response_tools(rt), env_server(es), body(b) {}
// } cgi_data;


struct RequestBody
{
    std::string ContentDisposition;
    std::string Content;
    std::string ContentType;
};

class http_items
{
public:
    int chunked_body;
    std::string method;
    std::string Scheme_name;
    std::string Path;
    std::string Query_String;
    std::string Fragment_iden;
    std::string http_version;
    int port;
    std::string server_name;
    std::map<std::string, std::string> Headers;
    std::vector<std::string> Req;
    int lenghtbody;
    std::string Body;
    std::vector<RequestBody *> ChunkedBody;
    std::string bondary;
    std::map<std::string, std::string> EncodedFormbody;
    std::string Extension;
    std::string server;
    s_location *location;
    bool connection;
    std::map<std::string, std::string> error_pages;

    // std::string root;
    // std::string index;
    // std::string cgi_path;
    // std::string autoindex;
    // std::string cgi_extension;
    // std::string allowed_methods;
    // std::string return_code_url;
    // std::string upload_store_directory;
    // std::string upload_enable;
};


class Requese
{
private:
    // std::streamstring os;
    std::string req;

public:
    http_items response_items;
    std::vector<std::string> Initial_Request_Line;

    int status_response_code;
    Requese(std::string req, server &server_data);
    void parser_init_line(std::string Initial_Request_Line, std::string &methods);
    void Headers_elements();
    int check_elemens(std::string &key);
    int is_alpha(std::string value);
    // int check_date(std::string &value);
    int check_content_type(std::string &value);
    int check_more_element(std::string &key, std::string &value);
   
    int check_host(std::string &value);
    int check_Transfer_Encoding(std::string &value);
    int check_connection(std::string &value);
    void check_connection(server &server_data);
    // std::string find_location(std::map<std::string , s_location>& location, std::string& PATH);
    std::string find_location(server &server_data, std::string &PATH);
    //    void  set_Initial_Request_line(std::string req)
    class ErrorSyntax : public std::exception
    {
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
    std::string forbidden_req;
    

public:
    Response(int status, std::vector<std::string> init_line, http_items &response_items);
    std::string  build_response();
    // std::string get_Content_type();
    std::string get_Content_type(std::string url);
    std::string get_type(std::string extension);
    std::string get_Date();
    std::string check_index_file(std::string & url);
    void build_GET();
    void build_POST();
    void build_DELETE();
    int get_permission(std::string &file);
    std::string read_file(const std::string &filename);
    // void parser_output_cgi(cgi_data& cgiData);
    void responsecgi( const cgi_data& cgidata);
    int remove_all_files(const char *dirname);
    std::string trim(std::string original);
    void return_pages(std::string& pages_return, std::string& url);
    void ft_success_code(std::string status, std::string message,  std::string URI);
    void ft_redirect(std::string status, std::string message);
    // void ft_bad_request(std::string status, std::string message);
    // void ft_forbidden_request(std::string status, std::string message);
    void other_response(std::string status, std::string desc);
    void ft_default_pages(std::string status, std::string& message, std::string& path);

};

int getServerId(std::map<unsigned int, server> &serv, int port, std::string server_name);
std::vector<std::string> split_v(std::string &str, std::string delimiter);
std::string Get_response(server &server_data);
std::string  parserbody(std::string reqbody);
std::string trim(std::string original);
#endif