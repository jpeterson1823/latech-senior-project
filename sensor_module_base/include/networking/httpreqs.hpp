#pragma once

#include <string>

namespace Http {

    enum ReqType {
        HTTP_GET,
        HTTP_POST
    };

    class Request {
    private:
        ReqType type;
        std::string rhost;
        std::string uri;
        std::string params;
        std::string data;

    public:
        Request(
            ReqType type,
            std::string rhost,
            std::string uri,
            std::string params,
            std::string data
        );
        
        void genString(std::string& buf);
        void setRemoteHost(std::string rhost);
        void setUri(std::string uri);
        void setParams(std::string params);
        void setData(std::string data);
    };


    class GetReq : public Request {
    public:
        GetReq(
            std::string rhost,
            std::string uri,
            std::string params
        );
    };

    class PostReq : public Request {
    public:
        PostReq(
            std::string rhost,
            std::string uri,
            std::string data
        );
    };

}