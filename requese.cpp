#include "./webserver.hpp"
#include "webserver.hpp"


// GET /path/to/file/index.html HTTP/1.0 \r\n

Requese::Requese(std::string req):req(req),status_response_code(200)
{
    this->response_items.chunked_body = 0;
    this->response_items.lenghtbody = 0;
    std::string token;
    int i = 0;
    int pos = 0;
    std::string value;
    std::string key;
    try{
        //read Header request 
        while(pos != -1)
        {
            pos = req.find("\r\n");
            if(req[0] == '\r' && req[1] == '\n')
            {
                req = req.substr(2);
                break;
            }
            token = req.substr(0, pos);
            req = req.substr(pos + 2, req.length());
            response_items.Req.push_back(token);
            // std::cout << token << "\n";
            i++;
        } 
        //parser line-request 
        parser_init_line(response_items.Req[0]);
        // ckeck Headers and parser some special Headers
        Headers_elements();
        std::cout <<  "----" << std::endl;
        std::cout <<  this->status_response_code << std::endl;
        token.clear();
        // std::cout << req << std::endl;
        // store body 
        if(response_items.Headers["Content-Type"] == "application/x-www-form-urlencoded")
        {
            pos = req.find("&");
            // std::cout << 
            while(pos != -1 && !req.empty())
            {
                token = req.substr(0, pos);
                this->response_items.EncodedFormbody[token.substr(0, token.find("="))] = token.substr(token.find("=") + 1);
                token.clear();
                req = req.substr(pos + 1, req.length());
            }
        }
        else if(response_items.Headers["Content-Type"].find("multipart/form-data") != -1)
        {
            std::stringstream os(req);
            RequestBody *ele;
            int i = 0;    
            while (std::getline(os, token, '\n')) {
            ele = new RequestBody;
        while (token != this->response_items.bondary) {
            // std::cout << token << std::endl;

                if (token.find("Content-Disposition") != -1) 
                {
                    // puts("here");
                    ele->ContentDisposition = token;
                                    // std::cout << ">>>>>>>>>>" << token << std::endl;
                } 
                else {
                    // std::cout << ">>>>>>>>>>" << token << std::endl;
                   ele->Content += token;
                }
                std::getline(os, token, '\n');
        }
        if(!ele->ContentDisposition.empty() && !ele->Content.empty() )
        {
                this->response_items.lenghtbody +=  ele->Content.length();
                this->response_items.ChunkedBody.push_back(new RequestBody({ele->ContentDisposition , ele->Content}) );
                // std::cout << "|"  << ele->ContentDisposition << ele->Content  <<  "|"<< std::endl;
                delete ele;
        }
    //    for (const auto& it : this->response_items.ChunkedBody) {
    //     std::cout << "Dispotio-content: " << it->ContentDisposition  << ", Content: " << it->Content<< std::endl;
    }
    }
    else
    {
        this->response_items.Body =  req;
        this->response_items.lenghtbody +=  this->response_items.Body.length();
    }
    //add check max size and Extension for Path
    // if(this->response_items.Path.substr(this->response_items.Path.find('.') + 1) !=  this->response_items.Extension )
    //     this->status_response_code = 400;
    if(this->response_items.method ==  "GET" && this->response_items.lenghtbody != 0 )
        this->status_response_code = 400;
    if(this->response_items.method !=  "GET" && this->response_items.lenghtbody == 0)
        this->status_response_code = 400;
    if(this->response_items.lenghtbody != 0 && this->response_items.Headers.find("Content-Length") == this->response_items.Headers.end())
        this->status_response_code = 400;
    else if(atoi((this->response_items.Headers.find("Content-Length")->second).data()) != this->response_items.lenghtbody)
        this->status_response_code = 400;
    // std::cout << this->response_items.Headers.find("Content-Length")->second << std::endl;
    // if(this->response_items.lenghtbody != atoi(this->response_items.Headers.find("Content-Length")))
    }catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

std::string Requese::trim(std::string original)
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


void Requese::parser_init_line(std::string  Initial_Request_Line)
{
    std::stringstream line_init(Initial_Request_Line);
    std::string part;
    std::vector<std::string> line;
    // std::vector<std::string>::iterator it;
    std::string Methode[3] = {"GET", "POST", "DELETE"};
    int i;
    while(line_init >> part)
        line.push_back(part);
    this->response_items.method = line[0];
    this->response_items.http_version = line[2];
     if(line[1].find("?") != -1 && line[1].find("#") != -1)
     {
        this->response_items.Path = line[1].substr(0, line[1].find("?"));
        this->response_items.Query_String = line[1].substr(line[1].find('?') + 1 , line[1].find("#") -( line[1].find('?') + 1));
        this->response_items.Fragment_iden = line[1].substr(line[1].find("#") + 1, line[1].length());
     }
     else if(line[1].find("?") != -1  && line[1].find("#") == -1)
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
     if(this->response_items.Path.rfind(".") != -1)
        this->response_items.Extension = this->response_items.Path.substr(this->response_items.Path.rfind(".") + 1);
    else
         this->response_items.Extension  = "";
     std::cout <<  "|" <<  this->response_items.Extension << std::endl;
    if(line.size() != 3)
        this->status_response_code = 400;
    i = 0;
    while(i < 3)
    {
        if(Methode[i] == line[0])
            break;
        i++;
    }
    if(i == 3)
    {
        if(line[0] == "HEAD")
            this->status_response_code = 405;
        else
            this->status_response_code = 400;
    }
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
        // break;
        // if(*it[pos + 1]) == ' ')
        // {
        //     this->status_response_code = 400;
        //     break;
        // }
        if(pos == -1)
        {
            this->status_response_code = 400;
            break;
        }
        // break;
        key = trim((*it).substr(0 , pos));
        value = trim((*it).substr(pos + 1));
        if((*it).substr(pos + 1, 1).c_str()[0]  != 32)
        {
                std::cout << ":::::" << (*it).substr(pos + 1, 1).c_str()[0] << std::endl;
            this->status_response_code = 400;
            // break;
        }
        this->trim(key);
        this->trim(value);
        this->response_items.Headers[key] = value;
        if(key.empty() || value.empty() || check_elemens(key) == 0 || check_more_element(key, value) == 0 )
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
    // print HEaders Elements
    std::map<std::string, std::string>::iterator it1 = this->response_items.Headers.begin();
    while(it1 != this->response_items.Headers.end())
    {
        // std::cout << it1->first  << "   "<< it1->second << std::endl;
        it1++;
    }
    // std::cout << this->status_response_code << std::endl;
}


int  Requese::check_elemens(std::string& key)
{
    //  # Add your custom headers if needed
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
        // Add your custom headers if needed
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

int Requese::check_date(std::string& value)
{
    int  date[5];
    int max_days_in_month[12]= {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int i = 0;
    int pos = 0;
    while(value.size())
    {
        pos = value.find("-");
        if(pos != -1)
        {
            date[i] = atoi(value.substr(0, value.find("-")).data());
            if(date[i] <= 0 || (i == 1 && date[i] > 12) || (i == 2 && date[i] > max_days_in_month[date[i - 1] ]))
                return 0;
            value = value.substr(value.find("-") + 1 , value.size());
        }
        else
        {
            date[i]=atoi(value.data());
            if(date[i] <= 0 || (i == 1 && date[i] > 12) || (i == 2 && date[i] > max_days_in_month[date[i - 1] ]))
                return 0;
            value = "";
        }
        i++;
    }
    if(i != 3)
    {
        return 0;
    }
    return 1;
}

int Requese::check_content_type(std::string &value)
{
    int pos = -1;
    std::string token;
    std::vector<std::string> contentTypes = {
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
        "application/vnd.api+json"
        // Add more as needed
    };
    std::vector<std::string>::iterator it = contentTypes.begin();
    token = value;
    pos = token.find(";");
    if(pos != -1)
    {
        token = token.substr(0, pos);
        value = value.substr(pos + 1);
        if(token == "multipart/form-data")
            this->response_items.bondary = value.substr(value.find("boundary=") + 9);

    }
    while(it !=  contentTypes.end())
    {
        if(*it == token)
        {
            // this->response_items.Extension = token.substr(token.rfind("/") + 1);
            return 1;
        }

        it++;
    }
    return 0;
}

int Requese::check_Transfer_Encoding(std::string& value)
    {
        std::vector<std::string> transferEncodings = {
            "chunked",
            "gzip",
            "deflate",
            "compress",
            "identity"
        // Add more as needed
    };
    std::vector<std::string>::iterator it =  transferEncodings.begin();
    while(it != transferEncodings.end())
    {
        if(value == "chunked")
        {
            this->response_items.chunked_body = 1;
            return 1;
        }
        if(value == *it)
            return 1;
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
     std::vector<std::string> connectionValues = {
        "Keep-Alive",
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
     std::vector<std::string>::iterator it = connectionValues.begin();
    while(it !=  connectionValues.end())
    {
        if(*it == value)
            return 1;
        it++;
    }
    return 0;
}

int Requese::check_more_element(std::string& key, std::string& value)
{
    if(key == "Host")
        return (this->check_host(value));
    if(key == "Date")
        return (this->check_date(value));
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