#include "networking/httpreqs.hpp"

Http::Request::Request(ReqType type, std::string rhost, std::string uri, std::string params, std::string data)
    : type(type), rhost(rhost), uri(uri), params(params), data(data) { }

void Http::Request::genString(std::string& buf) {
    // declare all string fields
    std::string type;

    // determine http request type
    switch (this->type) {
        case ReqType::HTTP_GET:
            buf = "GET " + this->uri;
            if (params.length() != 0)
                buf += '?' + params;
            buf += " HTTP/1.1\r\n";
            buf += "Host: " + this->rhost + "\r\n\r\n";
            break;

        case ReqType::HTTP_POST:
            buf = "POST " + this->uri + " HTTP/1.1\r\n";
            buf += "Host: " + this->rhost + "\r\n";
            buf += "User-Agent: prism/1.0.0\r\n";
            buf += "Accept: */*\r\n";
            buf += "Content-Type: application/x-www-form-urlencoded\r\n";
            buf += "Content-Length: " + std::to_string(data.length()) + "\r\n";
            buf += "\r\n";
            buf += data;
            break;
    }
}

void Http::Request::setRemoteHost (std::string rhost ) { this->rhost = rhost;   }
void Http::Request::setUri        (std::string uri   ) { this->uri = uri;       }
void Http::Request::setParams     (std::string params) { this->params = params; }
void Http::Request::setData       (std::string data  ) { this->data = data;     }

Http::GetReq::GetReq(std::string rhost, std::string uri, std::string params)
    : Request(ReqType::HTTP_GET, rhost, uri, params, "") {}
Http::PostReq::PostReq(std::string rhost, std::string uri, std::string data)
    : Request(ReqType::HTTP_POST, rhost, uri, "", data) {};