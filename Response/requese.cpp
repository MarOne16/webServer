#include "./webserver.hpp"

void Requese::check_connection(server& server_data)
{
    // std::cout << "Checking connection : " <<   this->response_items.Headers["Connection"] << std::endl;
    std::string value = (!this->response_items.Headers["Connection"].empty() ? this->response_items.Headers["Connection"] : "close");
    server_data.connection = (value == "keep-alive") ? true : false;
}

Requese::Requese(std::string req, server& server_data):req(req),status_response_code(200)
{
//   std::cout << req << std::endl;
    this->response_items.location = new s_location;
    this->response_items.lenghtbody = 0;
    this->response_items.error_pages = server_data.error_pages;
    std::string token;
    int i = 0;
    int pos = 0;
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
            // std::cout  << token <<  std::endl;
            i++;
        } 
        if (response_items.Req.empty())
        {
            this->status_response_code = 400;
            return;
        }
        parser_init_line(response_items.Req[0], this->response_items.location->allowed_methods);
        find_location(server_data, this->response_items.Path);     
        parser_init_line(response_items.Req[0], this->response_items.location->allowed_methods);
        //parser line-request 
     
        Headers_elements();
        check_connection(server_data);
        
        // ckeck Headers and parser some special Headers
         if(this->response_items.Headers.find("Transfer-Encoding") != this->response_items.Headers.end() &&
            (this->response_items.Headers.find("Transfer-Encoding"))->second != "chunked")
                this->status_response_code = 501;
        if(this->response_items.method ==  "POST" && this->response_items.Headers.find("Transfer-Encoding") == this->response_items.Headers.end() &&
                this->response_items.Headers.find("Content-Length") == this->response_items.Headers.end())
                this->status_response_code = 411;    

        if(!req.empty())
        {
            // store body
            if(this->response_items.Headers["Transfer-Encoding"] == "chunked")
                req =  parserbody(req);
            if(response_items.Headers["Content-Type"] == "application/x-www-form-urlencoded")
            {
                pos = req.find("&");
                    this->response_items.lenghtbody += req.length();
                while(pos != -1 && !req.empty())
                {
                    token = req.substr(0, pos);
                    this->response_items.EncodedFormbody[token.substr(0, token.find("="))] = token.substr(token.find("=") + 1);
                    token.clear();
                    req = req.substr(pos + 1, req.length());
                }
            }
            else if(response_items.Headers["Content-Type"].find("multipart/form-data") != std::string::npos)
            {
                if(req[req.length() - 1] != '\n')
                    req +='\n';
                std::stringstream os(req);
                RequestBody *ele;
                std::ofstream file;
                
                while (std::getline(os, token, '\n'))
                {
                    ele = new RequestBody;
                        token = token.substr(0, token.size() - 1);;
                        int i =0;
                        while(token.compare(this->response_items.bondary) != 0)
                        {
                            i++;
                            if (token.find("Content-Disposition") != std::string::npos) 
                            {
                                    ele->ContentDisposition = token;
                                    token.clear();
                            }
                            if (token.find("Content-Type") != std::string::npos  ) 
                            {
                                    
                                    ele->ContentType = token;
                                    std::getline(os, token, '\n');
                                    token.clear();
                            }
                            else
                            {

                                
                                ele->Content.append(token);
                                if(!ele->Content.empty())
                                    ele->Content.append("\n");
                                token.clear();
                            }
                            std::getline(os, token, '\n');
                            if(os.eof() ||  token.find(this->response_items.bondary) != std::string::npos)
                                break;
                        }
                        file.close();
                        if(!ele->Content.empty() )
                        {
                            if(!ele->ContentDisposition.empty())
                            {
                                this->response_items.lenghtbody +=  ele->ContentDisposition.length();
                            }
                            this->response_items.lenghtbody +=  ele->Content.length();
                            ele->Content.pop_back();
                            this->response_items.ChunkedBody.push_back(ele);
                        
                        }
                }
        }
        else
        {
            this->response_items.Body =  req;
            this->response_items.lenghtbody +=  this->response_items.Body.length();
        }
    }
   if(this->response_items.lenghtbody > atoi(server_data.max_body_size.c_str())) // TODO: check size 
        this->status_response_code = 413;
   if(this->response_items.Path.length() > 2048)
        this->status_response_code = 414;
    if(this->response_items.method !=  "POST" && this->response_items.lenghtbody != 0 )
        this->status_response_code = 400;
    if(this->response_items.method ==  "POST" && this->response_items.lenghtbody == 0)
        this->status_response_code = 400;
    if(this->response_items.Headers.find("Transfer-Encoding")->second == "chunked" && this->response_items.lenghtbody == 0  )
        this->status_response_code = 400;
    else if(this->response_items.Headers.find("Content-Length") != this->response_items.Headers.end() && atoi((this->response_items.Headers.find("Content-Length")->second).data()) != (int)req.length())
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


void Requese::parser_init_line(std::string  Initial_Request_Line, std::string& methods )
{
    std::stringstream line_init(Initial_Request_Line);
    std::string part;
    std::vector<std::string> line;
    std::string url_caracteres ="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";
    std::string del = " ";
    std::string str = "POST GET DELETE";
    std::vector<std::string> Methode = split_v(methods, del);
    std::vector<std::string> defaultMethods = split_v(str, " ");
    unsigned int  i;

   
    while(line_init >> part)
        line.push_back(part);
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
    if(line.size() != 3)
        this->status_response_code = 400;
    i = 0;
    while(i < Methode.size())
    {
        if(Methode[i] == line[0])
        {
            this->status_response_code = 200;
            break;
        }
        else
            this->status_response_code = 405;
        i++;
    }
    if(i == Methode.size() || std::find(defaultMethods.begin(), defaultMethods.end(), line[0]) == defaultMethods.end())
            this->status_response_code = 405;
    if(this->response_items.http_version != "HTTP/1.1")
         this->status_response_code = 505;
    i = 0;
    while(this->response_items.Path[i])
    {
        if(url_caracteres.find(this->response_items.Path[i])  == std::string::npos)
        {
            this->status_response_code = 400;
            break;
        }
        i++;
    }

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
        key = trim((*it).substr(0 , pos));
        value = trim((*it).substr(pos + 1));
        if((*it).substr(pos + 1, 1).c_str()[0]  != 32)
            this->status_response_code = 400;
        // this->trim(key);
        // this->trim(value);
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


std::string Requese::find_location(server& server_data, std::string& PATH)
{
    std::map<std::string , s_location> location = server_data.locations;
    std::map<std::string , s_location>::iterator it;
    int pos = 0;
    std::string Path = PATH;


    this->response_items.port = server_data.port;
    this->response_items.server_name = server_data.server_name;
    if(!this->response_items.Extension.empty())
    {
        it = location.begin();
        while(it != location.end())
        {
            if(it->first.find(this->response_items.Extension) != std::string::npos)
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
                return Path;
            }
            it++;
        }
    }
    // pos = Path.rfind("/");
    while(pos != -1)
    {
       
        it = location.find(Path);
        if(it != location.end())
        {
            // std::cout << "location: " << it->second.index << " " << std::endl;
            this->response_items.location->allowed_methods = it->second.allowed_methods;
            this->response_items.location->root = it->second.root;
            this->response_items.location->index = it->second.index;
            this->response_items.location->cgi_extension = it->second.cgi_extension;
            this->response_items.location->return_code_url = it->second.return_code_url;
            this->response_items.location->upload_store_directory = it->second.upload_store_directory;
            this->response_items.location->cgi_path = it->second.cgi_path;
            this->response_items.location->upload_enable = it->second.upload_enable;
            this->response_items.location->autoindex =it->second.autoindex;
            return Path;
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
        
    }
    else
        this->status_response_code = 404;
    return Path;
}