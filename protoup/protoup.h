#include<QJsonValue>
#include<QJsonArray>
#include<QJsonObject>
#include<QJsonDocument>
#include<QString>
#include<QDebug>
#include <WinSock2.h>
#include <iostream>

#ifndef PROTOUP_H
#define PROTOUP_H

const uint8_t PROTOUP_VERSION = 1;
const uint32_t PROTOUP_MAX_LEN = 1000;
const int PROTOUP_HEAD_SIZE = 8U;
struct ProtoUpHead
{
    uint8_t version = 1;
    uint16_t code;
    uint8_t type = 0;   // 0默认为json数据
    uint32_t length;

};

struct ProtoUpMsg
{
    ProtoUpHead head;
    QJsonObject body;
};

void proto_up_msg_print(ProtoUpMsg & msg);

class ProtoUpEncoder
{
public:
    uint8_t * encode(ProtoUpMsg * p_msg);
private:
    void head_encode(uint8_t * p_data, ProtoUpMsg * p_msg);
};

class ProtoUpDecoder
{
public:
    bool parser(uint8_t * p_data, ProtoUpMsg & msg);
private:
    bool parser_head(uint8_t * p_data, ProtoUpMsg & msg);
    bool parser_body(uint8_t * p_data, ProtoUpMsg & msg);
};
#endif // PROTOUP_H
