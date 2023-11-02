#include "./webserver.hpp"

int main()
{
//     std::string requese = "GET /hello.htm HTTP/1.1 User-Agent: Mozilla/4.0 (compatible; MSIE5.01; Windows NT)
// Host: www.tutorialspoint.com
// Accept-Language: en-us
// Accept-Encoding: gzip, deflate
// Connection: Keep-Alive";

    Requese req("DELETE /hello.htm?gsdfjgdsj#issam HTTP/1.1\r\nUser-Agent:\nMozilla/4.0 (compatible; MSIE5.01; Windows NT)\r\nHost: www.tutorialspoint.com:156\r\nAccept-Language: en-us\r\nAccept-Encoding: gzip, deflate \r\nConnection: Keep-Alive\r\nDate: 2012-04-02\r\nContent-Type: multipart/form-data; boundary=---------------------------1234567890123456789012345678\r\n\r\n---------------------------1234567890123456789012345678\nContent-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\nThis is a plain text body\n---------------------------1234567890123456789012345678\nContent-Disposition: form-data; name=\"field2\"; filename=\"example.txt\"\nThis is a plain text2 body\n---------------------------1234567890123456789012345678");
}