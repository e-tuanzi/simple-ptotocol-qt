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

const uint8_t PROTOUP_VERSION = 13;
const uint32_t PROTOUP_MAX_LEN = 1000;
const int PROTOUP_HEAD_SIZE = 8U;
struct ProtoUpHead
{
    uint8_t version = 13;
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
    QByteArray encode(ProtoUpMsg & msg);
private:
    void head_encode(QByteArray & data, ProtoUpMsg & msg);
};

class ProtoUpDecoder
{
public:
    bool parser(QByteArray & data, ProtoUpMsg & msg);
private:
    bool parser_head(QByteArray & data, ProtoUpMsg & msg);
    bool parser_body(QByteArray & data, ProtoUpMsg & msg);
};

#endif // PROTOUP_H
