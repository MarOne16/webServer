#include "./webserver.hpp"

int main()
{
//     std::string requese = "GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive";

    Requese *req = new Requese ("POST /dir/index3.php HTTP/1.1\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com:156\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate \r\nConnection: Keep-Alive\r\nDate: 2012-04-02\r\nContent-Type: Content-Type: multipart/form-data; boundary=NextField\r\nContent-Length: 125\r\n\r\nNextField\nContent-Disposition: form-data; name=”Job”\n100\nNextField\nContent-Disposition: form-data; name=”Priority”\n2xbcbbjkhkbhdfbh999\nsfadksgfdksf" );
    std::cout << "-------------------------------- BEGIN PUBLIC" << std::endl;
    Response res(req->status_response_code, req->Initial_Request_Line, req->response_items);
    res.build_response();

}