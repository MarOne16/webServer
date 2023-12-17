#include "./webserver.hpp"

void Requese::check_connection(server& server_data)
{
    std::string value = (!this->response_items.Headers["Connection"].empty() ? this->response_items.Headers["Connection"] : "close");
    server_data.connection = (value == "keep-alive") ? true : false;
}


void Requese::check_methods(std::string& method, std::string& methods)
{
    std::string del = " ";
    std::vector<std::string> Methode = split_v(methods, del);
    unsigned  int i = 0;
    while(i < Methode.size())
    {
        if(Methode[i] == trim(method))
        {
            this->status_response_code = 200;
            break;
        }
        else
        {
            this->status_response_code = 405;

        }
        i++;
    }

}

void  Requese::is_path_outside_directoryy(std::string path, std::string directory) {
    char abs_path[PATH_MAX];
    char abs_directory[PATH_MAX];
    // char *new_path = NULL;
    // Get the absolute paths of the given path and directory
    while(findFile(path) == 0 && path != "/")
    {
        path = path.substr(0, path.rfind('/'));
    }
    if (realpath(path.c_str(), abs_path) == NULL || realpath(directory.c_str(), abs_directory) == NULL) {
       if(strncmp(abs_path, realpath(directory.c_str(), abs_directory), strlen(abs_directory)) == 0) 
            this->status_response_code = 404;
        else
            this->status_response_code = 400;
        return;
    }
    if(strncmp(abs_path, abs_directory, strlen(abs_directory)) != 0)
    {
        this->status_response_code = 400;
    }
}

void Requese::parser_uri(std::string uri)
{
    
    std::string url_caracteres ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    std::string  new_path;
    int number = 0;
    int i = 0;
    while(uri[i])
    {
        if(url_caracteres.find(uri[i])  == std::string::npos)
        {
            this->status_response_code = 400;
            break;
        }
        else if(uri[i] == '%' && (isdigit(uri[i + 1]) || (uri[i  + 1 ] >= 'A' && uri[i + 1 ] <= 'F')) && (isdigit(uri[i + 2]) || (uri[i + 2] >= 'A' && uri[i + 2] <= 'F')))
        {
            number = std::stoi(uri.substr(i + 1 , 3), 0, 16);
            new_path = uri.substr(0, i);
            new_path += static_cast<unsigned char>(number);
            new_path += uri.substr(i + 3);
            uri = new_path;
                    
        }
        i++;
    }
    if(uri[0] == '/')
        uri = uri.substr(1);
    this->response_items.Path = uri;
    is_path_outside_directoryy((this->response_items.location->root + uri).c_str(), this->response_items.location->root.c_str());
}

Requese::Requese(std::string req, server& server_data):req(req),status_response_code(200)
{
    this->response_items.location = new s_location;
    this->response_items.lenghtbody = 0;
    this->response_items.error_pages = server_data.error_pages;
    std::string token;
    unsigned long  i = 0;
    int  pos = 0;
    std::string value;
    std::string key;
    if (req.empty())
    {
        this->status_response_code = 400;
        return;
    }
    try{
        while(pos != -1)
        {
            pos = req.find("\r\n");
            if((req[0] == '\r' && req[1] == '\n'))
            {
                req = req.substr(2);
                break;
            }
            token = req.substr(0, pos);
            req = req.substr(pos + 2, req.length());
            response_items.Req.push_back(token);
            i++;
        } 
        if (response_items.Req.empty())
        {
            this->status_response_code = 400;
            return;
        }
        parser_init_line(response_items.Req[0]);
        if(this->status_response_code != 200)
            return ; 

        find_location(server_data, this->response_items.Path);
        check_methods(this->response_items.method, this->response_items.location->allowed_methods);
        if(this->status_response_code != 200)
            return ;
        parser_uri(this->response_items.Path);
        if(this->status_response_code != 200)
            return ;
        //parser line-request 
     
        Headers_elements();
        if(this->status_response_code != 200)
            return ;
        check_connection(server_data);
        
        
        // ckeck Headers and parser some special Headers
         if(this->response_items.Headers.find("Transfer-Encoding") != this->response_items.Headers.end() &&
            (this->response_items.Headers.find("Transfer-Encoding"))->second != "chunked")
            {
                this->status_response_code = 501;
                return ;
            }
        if(this->response_items.method ==  "POST" && this->response_items.Headers.find("Transfer-Encoding") == this->response_items.Headers.end() &&
                this->response_items.Headers.find("Content-Length") == this->response_items.Headers.end())
                {
                    this->status_response_code = 411;    
                    return ;
                }
        if(this->response_items.method ==  "POST" && this->response_items.Headers.find("Content-Type") == this->response_items.Headers.end())
        {
            this->status_response_code = 415;
            return ;
        }
        if(!req.empty())
        {
            // store body
            if(this->response_items.Headers["Transfer-Encoding"] == "chunked")
                req =  parserbody(req);
            this->response_items.Body =  req;
            // if(response_items.Headers["Content-Type"] == "application/x-www-form-urlencoded")
            // {
            //     pos = req.find("&");
            //         this->response_items.lenghtbody += req.length();
            //     while(pos != -1 && !req.empty())
            //     {
            //         token = req.substr(0, pos);
            //         this->response_items.EncodedFormbody[token.substr(0, token.find("="))] = token.substr(token.find("=") + 1);
            //         token.clear();
            //         req = req.substr(pos + 1, req.length());
            //     }
            // }
            if(response_items.Headers["Content-Type"].find("multipart/form-data") != std::string::npos)
            {
                    size_t bondary_start = 0;
        size_t boundary_end = 0;
        size_t EndPos = 0;
        size_t start = 0;
        std::string Headers;
        bondary_start = req.find(this->response_items.bondary + "--", bondary_start);
        if(bondary_start == std::string::npos)
        {
            this->status_response_code = 400;
            return ;
        }
        bondary_start = 0;
        while(bondary_start < req.length())
        {
                bondary_start = req.find(this->response_items.bondary , bondary_start);
                if(bondary_start == std::string::npos)
                {
                    this->status_response_code = 400;
                    return;
                }
                else
                {
                    RequestBody *ele;
                    ele = new RequestBody;
                    start = bondary_start + this->response_items.bondary.length() + 2;
                bondary_start = req.find("Content-Disposition", start);
                if(bondary_start == std::string::npos)
                {
                    this->status_response_code = 400;
                    delete ele;
                    return;
                }
                EndPos = req.find("\r\n\r\n", start);
                Headers = req.substr(bondary_start, EndPos - bondary_start);
                if(Headers.find("Content-Disposition") != std::string::npos)
                {
                    start = Headers.find("\r\n");
                    if(bondary_start != std::string::npos)
                            ele->ContentDisposition = Headers.substr(Headers.find("Content-Disposition"), start - Headers.find("Content-Disposition"));
                    start += 2;
                    if(Headers.find("Content-Type") != std::string::npos)
                        ele->ContentType = Headers.substr(start, Headers.length() - start);
                    
                }

                EndPos += 4;
                boundary_end = req.find(this->response_items.bondary, EndPos);
                if(boundary_end == std::string::npos)
                    boundary_end = req.length();
                ele->Content = req.substr(EndPos, boundary_end - EndPos - 2);
                if(!ele->Content.empty() )
                {
                    if(!ele->ContentDisposition.empty())
                        this->response_items.lenghtbody +=  ele->ContentDisposition.length();
                    this->response_items.lenghtbody +=  ele->Content.length();
                    this->response_items.ChunkedBody.push_back(ele);
                }
                else
                    delete ele;
                }
        }
    }
        else
        {
            this->response_items.Body =  req;
            this->response_items.lenghtbody +=  this->response_items.Body.length();
        }
    }
   if(this->response_items.lenghtbody > stosize_t(server_data.max_body_size))
        this->status_response_code = 413;
   if(this->response_items.Path.length() > 2048)
        this->status_response_code = 414;
    if(this->response_items.method != "POST" && this->response_items.lenghtbody != 0 )
        this->status_response_code = 400;
    if(this->response_items.method ==  "POST" && this->response_items.lenghtbody == 0)
    {
        this->status_response_code = 400;
        return ;
    }
    if(this->response_items.Headers.find("Transfer-Encoding")->second == "chunked" && this->response_items.lenghtbody == 0  )
        this->status_response_code = 400;
    else if(this->response_items.Headers.find("Content-Length") != this->response_items.Headers.end() 
            && stosize_t((this->response_items.Headers.find("Content-Length")->second)) != req.length()
            && stosize_t((this->response_items.Headers.find("Content-Length")->second)) != this->response_items.lenghtbody )
            this->status_response_code = 400;
    
    }catch(std::exception& e)
    {
        std::cout  << e.what() << std::endl;
    }
}

std::string trim(std::string original)
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
    return original.substr(begin_index, (i - begin_index )  + 1);
}


void Requese::parser_init_line(std::string Initial_Request_Line)
{
    std::stringstream line_init(Initial_Request_Line);
    std::string part;
    std::vector<std::string> line;
    std::string str = "POST GET DELETE";
    std::vector<std::string> defaultMethods = split_v(str, " ");
    // unsigned int  i;

   
    while(line_init >> part)
        line.push_back(part);
    if(line.size() != 3)
    {
        this->status_response_code = 400;
        return ;
    }
        
    this->response_items.method = line[0];
    this->response_items.http_version = line[2];
     if(line[1].find("?") != std::string::npos && line[1].find("#") != std::string::npos)
     {
        this->response_items.Path = line[1].substr(0, line[1].find("?"));
        this->response_items.Query_String = line[1].substr(line[1].find('?') + 1 , line[1].find("#") -( line[1].find('?') + 1));
        this->response_items.Fragment_iden = line[1].substr(line[1].find("#") + 1, line[1].length());
     }
     else if(line[1].find("?") != std::string::npos  && line[1].find("#") == std::string::npos)
     {
        this->response_items.Path = line[1].substr(0, line[1].find("?"));
        this->response_items.Query_String = line[1].substr((line[1].find("?") + 1), line[1].length());
        this->response_items.Fragment_iden = "";
     }
     else
     {
        this->response_items.Path = line[1].substr(0, line[1].size());
          this->response_items.Fragment_iden = "";
          this->response_items.Query_String = "";
     }
     if(this->response_items.Path.rfind(".") != std::string::npos)
     {
        if(this->response_items.Path.rfind(".") > this->response_items.Path.rfind("/"))
            this->response_items.Extension = this->response_items.Path.substr(this->response_items.Path.rfind(".") + 1);
        else
            this->response_items.Extension  = "";

     }
    else
         this->response_items.Extension  = "";
    if(std::find(defaultMethods.begin(), defaultMethods.end(), line[0]) == defaultMethods.end())
            this->status_response_code = 405;
    if(this->response_items.http_version != "HTTP/1.1")
         this->status_response_code = 505;
}



void Requese::Headers_elements()
{
    std::vector<std::string>::iterator it = this->response_items.Req.begin();
    it++;
    int pos = 0;
    std::string key;
    std::string value;
    while(it !=  this->response_items.Req.end())
    {
        pos = (*it).find(":");
        if(pos == -1)
        {
            this->status_response_code = 400;
            break;
        }
        // break;
        if((*it).substr(pos + 1, 1).c_str()[0]  != 32)
            this->status_response_code = 400;
        key = trim((*it).substr(0 , pos));
        value = trim((*it).substr(pos + 1));
        this->response_items.Headers[key] = value;
        if(key.empty() || value.empty() || check_more_element(key, value) == 0 )
        {
            this->status_response_code = 400;
            return ;
        }
        it++;
    }
    if(response_items.Headers.find("Host") == response_items.Headers.end())
    {
        this->status_response_code = 400;
        return ;
    }
}


int  Requese::check_elemens(std::string& key)
{
    std::string http_request_headers[] = {
        "Accept",
        "Accept-Charset",
        "Accept-Encoding",
        "Accept-Language",
        "Authorization",
        "Cache-Control",
        "Connection",
        "Content-Length",
        "Content-Type",
        "Cookie",
        "Date",
        "Expect",
        "From",
        "Host",
        "If-Match",
        "If-Modified-Since",
        "If-None-Match",
        "If-Range",
        "If-Unmodified-Since",
        "Max-Forwards",
        "Origin",
        "Pragma",
        "Proxy-Authorization",
        "Range",
        "Referer",
        "TE",
        "Transfer-Encoding",
        "Upgrade",
        "User-Agent",
        "Via",
        "Warning",
        "Content-Disposition",
    };

    unsigned  int i = 0;
    while(i < 30)
    {
        if(http_request_headers[i] == key.data())
            return 1;
        i++;
    }
    return 0;
}

int Requese::is_alpha(std::string value)
{
    int i = 0;
    while(value[i])
    {
        if(isdigit(value[i] == 0))
            return 0;
        i++;
    }
    return 1;
}

int Requese::check_content_type(std::string &value)
{
    int pos = -1;
    std::string token;
    std::string contentTypes[] = {
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
    };
    unsigned int it = 0;
    token = value;
    pos = token.find(";");
    if(pos != -1)
    {
        token = token.substr(0, pos);
        value = value.substr(pos + 1);
        if(token == "multipart/form-data")
        {
            this->response_items.bondary = "--";
            this->response_items.bondary += value.substr(value.find("boundary=") + 9);
        }
        return 1;
    }
    while(it <  contentTypes->length())
    {
        if(contentTypes[it].compare(token))
        {
            return 1;
        }

        it++;
    }
    return 0;
}

int Requese::check_Transfer_Encoding(std::string& value)
    {
        std::string transferEncodings[] = {
            "chunked",
            "gzip",
            "deflate",
            "compress",
            "identity"
    };
    unsigned  it =  0;
    while(it < transferEncodings->length())
    {
        if(value == "chunked")
        {
            this->response_items.chunked_body = 1;
            return 1;
        }
        if(value == transferEncodings[it])
            return 1;
        it++;
    }
    return 0;
}

int Requese::check_host(std::string&value)
{
    size_t dots = 0;
    std::string host; 
    std::string port; 
    int pos = value.find(":");
    if(pos != -1)
    {
        host = value.substr(0 , pos);
        port = value.substr(pos + 1 , value.length());
        if(port.empty() || host.empty())
            return 0;
    }
    else
        host = value;
    this->response_items.server = host;
    if(!port.empty())
    {
        for (size_t i = 1; i < port.length(); ++i)
        {
                if(!isdigit(port[i]))
                    return 0;
        }
    }
    if(isalpha(value[0]))
    {
        for (size_t i = 1; i < host.length(); ++i) {
            if (!std::isalnum(host[i]) &&  host[i] != '.') {
                return 0;
            }
        }
        return 1;
    }
    for (size_t i = 0; i < host.length(); ++i) {
        if (!std::isdigit(host[i]) && host[i] != '.') {
            return 0;
        }
        if (host[i] == '.') {
            dots++;
        }
    }
    if(dots != 3)
        return 0;
    return 1;
}

int Requese::check_connection(std::string& value)
{
    std::string connectionValues[] = {
        "keep-alive",
        "close",
        "Upgrade",
        "TE, close",
        "Trailers",
        "Proxy-Authorization",
        "Upgrade, WebSocket",
        "Sec-WebSocket-Key",
        "Sec-WebSocket-Version",
        "Sec-WebSocket-Accept"
        // Add more as needed
    };
    unsigned int  it = 0;
    while(it <  connectionValues->length())
    {
        if(connectionValues[it] == value)
            return 1;
        it++;
    }
        return 0;
}

int Requese::check_more_element(std::string& key, std::string& value)
{
    if(key == "Host")
        return (this->check_host(value));
    if(key == "Content-Length")
        return (this->is_alpha(value));
    if( key == "Content-Type")
        return check_content_type(value);
    if(key == "Transfer-Encoding")
        return check_Transfer_Encoding(value);
    if(key == "Connection")
        return   check_connection(value);
    return 1;
}

const char *Requese::ErrorSyntax::what() const throw()
{
    return "Error  requese Headers or body";
}


void Requese::find_location(server& server_data, std::string& PATH)
{
    std::map<std::string , s_location> location = server_data.locations;
    std::map<std::string , s_location>::iterator it;
    std::map<std::string , s_location>::iterator it2;
    int pos = 0;
    std::string Path = PATH;


    this->response_items.port = server_data.port;
    this->response_items.server_name = server_data.server_name;
    if(!this->response_items.Extension.empty() &&( this->response_items.Extension == "php" || this->response_items.Extension == "py"))
    {
        it2 = location.begin();
        while(it2 != location.end())
        {
            if(it2->first.find(this->response_items.Extension) != std::string::npos)
            {
                this->response_items.location->allowed_methods = it2->second.allowed_methods;
                this->response_items.location->root = it2->second.root;
                this->response_items.location->index = it2->second.index;
                this->response_items.location->cgi_extension = it2->second.cgi_extension;
                this->response_items.location->return_code_url = it2->second.return_code_url;
                this->response_items.location->upload_store_directory = it2->second.upload_store_directory;
                this->response_items.location->cgi_path = it2->second.cgi_path;
                this->response_items.location->upload_enable = it2->second.upload_enable;
                this->response_items.location->autoindex = it2->second.autoindex;
                return ;
            }
            it2++;
        }
    }
    // pos = Path.rfind("/");
    while(pos != -1)
    {
        it = location.find(Path);
        it2 = location.begin();
         while(it2 != location.end())
        {
            if(it2->first.find(Path) != std::string::npos)
            {
                this->response_items.location->allowed_methods = it2->second.allowed_methods;
                this->response_items.location->root = it2->second.root;
                this->response_items.location->index = it2->second.index;
                this->response_items.location->cgi_extension = it2->second.cgi_extension;
                this->response_items.location->return_code_url = it2->second.return_code_url;
                this->response_items.location->upload_store_directory = it2->second.upload_store_directory;
                this->response_items.location->cgi_path = it2->second.cgi_path;
                this->response_items.location->upload_enable = it2->second.upload_enable;
                this->response_items.location->autoindex = it2->second.autoindex;
                return ;
            }
            it2++;
        }
        pos = Path.rfind("/"); 
        Path = Path.substr(0, pos);
    }
    Path = "/";
    it = location.find(Path);
    if(it != location.end())
    {
        this->response_items.location->allowed_methods = it->second.allowed_methods;
        this->response_items.location->root = it->second.root;
        this->response_items.location->index = it->second.index;
        this->response_items.location->cgi_extension = it->second.cgi_extension;
        this->response_items.location->return_code_url = it->second.return_code_url;
        this->response_items.location->upload_store_directory = it->second.upload_store_directory;
        this->response_items.location->cgi_path = it->second.cgi_path;
        this->response_items.location->upload_enable = it->second.upload_enable;
        this->response_items.location->autoindex = it->second.autoindex;
        return ;
    }
    else
        this->status_response_code = 404;
}