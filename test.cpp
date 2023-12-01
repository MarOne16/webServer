#include <iostream>
#include <string>
#include <sstream>



int hexadecimalToDecimal(std::string hexVal) 
{ 
    int len = hexVal.size(); 
  
    int base = 1; 
  
    int dec_val = 0; 

    for (int i = len - 1; i >= 0; i--) { 
        if (hexVal[i] >= '0' && hexVal[i] <= '9') { 
            dec_val += (int(hexVal[i]) - 48) * base; 
            base = base * 16; 
        } 
        else if (hexVal[i] >= 'A' && hexVal[i] <= 'F') { 
            dec_val += (int(hexVal[i]) - 55) * base; 
        }
        else if (hexVal[i] >= 'a' && hexVal[i] <= 'f') { 
            dec_val += (int(hexVal[i]) - 55) * base; 
        }
            base = base * 16; 
        } 
    return dec_val; 
} 

int main()
{
    std::string reqbody="7\r\nMozilla\r\n0\r\n11\r\nDeveloper Network\r\n0\r\n\r\n";

std::stringstream ss;
std::stringstream body;
ss << reqbody;

int pos = 0 ;
std::string token;
int hash = -1;
 while(pos != -1 && hash)
{
    pos = reqbody.find("\r\n");
    if(reqbody[0] == '\r' && reqbody[1] == '\n')
    {
        reqbody = reqbody.substr(2);
        break;
    }
    token = reqbody.substr(0, pos);
    hash = std::stoi(token, 0, 16);
    if(hash != 0)
    {
        
        token = reqbody.substr(pos + 2 , hash + 2);
        reqbody = reqbody.substr(pos + hash + 2 + 2);
        body << token;
    }
    // response_items.Req.push_back(token);
    // i++;
} 
return 0;

}