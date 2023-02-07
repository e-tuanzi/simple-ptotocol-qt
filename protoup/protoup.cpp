#include "protoup.h"



void proto_up_msg_print(ProtoUpMsg & msg)
{
    QJsonDocument document;
    document.setObject(msg.body);
    QByteArray byte_array = document.toJson(QJsonDocument::Compact);
    QString json_str(byte_array);
    qDebug() << "Print: ";
    qDebug() << "version: " << msg.head.version << " code: " << msg.head.code << " type: " << msg.head.type << " length: " << msg.head.length;
    qDebug() << QString::fromLocal8Bit("Json Data: ") << json_str;
}

QJsonObject get_json_object_from_string(const QString json_str)
{
    QJsonDocument document = QJsonDocument::fromJson(json_str.toLocal8Bit().data());
    if (document.isNull())
    {
        qDebug() << "please check the string " << json_str.toLocal8Bit().data();
    }
    QJsonObject json_object = document.object();
    return json_object;
}

QString get_string_from_json_object(const QJsonObject & json_object)
{
    return QString(QJsonDocument(json_object).toJson(QJsonDocument::Compact));
}

void ProtoUpEncoder::head_encode(uint8_t *p_data, ProtoUpMsg * p_msg)
{
    // 协议头版本号为 1
    *(uint8_t *)p_data = PROTOUP_VERSION;
    ++p_data;

    // 协议头业务代码
    *(uint16_t *)p_data = htons(p_msg->head.code);
    p_data += 2;

    // 协议数据类型
    *(uint8_t *)p_data = p_msg->head.type;
    ++p_data;

    // 协议数据长度
    *(uint32_t *)p_data = htonl(p_msg->head.length);
}

uint8_t * ProtoUpEncoder::encode(ProtoUpMsg *p_msg)
{
    uint8_t * p_data = nullptr;
    QString json_str = get_string_from_json_object(p_msg->body);
    uint32_t len = PROTOUP_HEAD_SIZE + (uint32_t)json_str.size();
    if (len > PROTOUP_MAX_LEN)
    {
        qDebug()<<"the message is too long!";
        return nullptr;
    }
    p_msg->head.length = len;
    p_data = new uint8_t[len];
    head_encode(p_data, p_msg);
    // toLatin1 ascii编码 toLocal8Bit Unicode编码
    memcpy(p_data + PROTOUP_HEAD_SIZE, json_str.toLatin1(), json_str.size());
    return p_data;
}

bool ProtoUpDecoder::parser(uint8_t * p_data, ProtoUpMsg & msg)
{
    bool success;
    success = parser_head(p_data, msg);
    if (success == false)
    {
        qDebug() << "parser head failed!";
        return false;
    }
    success = parser_body(p_data, msg);
    if (success == false)
    {
        qDebug() << "parser body failed!";
        return false;
    }
    return true;
}


bool ProtoUpDecoder::parser_head(uint8_t * p_data, ProtoUpMsg & msg)
{
    ProtoUpHead head;
    head.version = *(uint8_t *)p_data;
    ++p_data;
    if (head.version != PROTOUP_VERSION)
    {
        qDebug()<<"the protocol version doesn't match!";
    }

    head.code = ntohs(*(uint16_t *)p_data);
    p_data += 2;

    head.type = *p_data;
    ++p_data;

    head.length = ntohl(*(uint32_t *)p_data);
    if (head.length > PROTOUP_MAX_LEN)
    {
        qDebug()<<"the message is too long!";
        return false;
    }
    msg.head = head;
    return true;
}

bool ProtoUpDecoder::parser_body(uint8_t * p_data, ProtoUpMsg & msg)
{
    uint32_t json_size = msg.head.length - PROTOUP_HEAD_SIZE;
    p_data += PROTOUP_HEAD_SIZE;
    QByteArray byte_array((char*)p_data, json_size);
    QString json_str;
    json_str = byte_array;
    QJsonObject json_object = get_json_object_from_string(json_str);
    msg.body = json_object;
    return true;
}

