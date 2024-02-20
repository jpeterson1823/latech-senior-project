#include "networking/httpreqs.hpp"

Http::Request::Request(ReqType type, std::string rhost, std::string uri, std::string params, std::string data)
    : type(type), rhost(rhost), uri(uri), params(params), data(data) { }

void Http::Request::genString(std::string& buf) {
    // declare all string fields
    std::string type;

    // determine http request type
    switch (this->type) {
        case ReqType::HTTP_GET:
            type = "GET ";
            break;
        case ReqType::HTTP_POST:
            type = "POST ";
            break;
        
        // if type is unknown somehow, default to get request
        default:
            type = "GET ";
            break;
    }

    // generate request string and store in provided buffer
    buf = type + this->uri;

    if (params.length() != 0)
        buf += '?' + params;

    buf += " HTTP/1.1\r\n";
    buf += "Host: " + this->rhost + "\r\n";
    buf += "\r\n"; // add blank line

    if (this->data.length() != 0)
        buf += this->data;
}

void Http::Request::setRemoteHost (std::string rhost ) { this->rhost = rhost;   }
void Http::Request::setUri        (std::string uri   ) { this->uri = uri;       }
void Http::Request::setParams     (std::string params) { this->params = params; }
void Http::Request::setData       (std::string data  ) { this->data = data;     }


Http::GetReq::GetReq(std::string rhost, std::string uri, std::string params)
    : Request(ReqType::HTTP_GET, rhost, uri, params, "") {}
Http::PostReq::PostReq(std::string rhost, std::string uri, std::string data)
    : Request(ReqType::HTTP_POST, rhost, uri, "", data) {};