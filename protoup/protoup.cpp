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

void ProtoUpEncoder::head_encode(QByteArray & data, ProtoUpMsg & msg)
{
    uint8_t * p_id = (uint8_t *)data.data();
    // 协议头版本号为 1
    *(uint8_t *)p_id = PROTOUP_VERSION;
    ++p_id;

    // 协议头业务代码
    *(uint16_t *)p_id = htons(msg.head.code);
    p_id += 2;

    // 协议数据类型
    *(uint8_t *)p_id = msg.head.type;
    ++p_id;

    // 协议数据长度
    *(uint32_t *)p_id = htonl(msg.head.length);

    return;
}

QByteArray ProtoUpEncoder::encode(ProtoUpMsg & msg)
{
    QString json_str = get_string_from_json_object(msg.body);
    uint32_t len = PROTOUP_HEAD_SIZE + (uint32_t)json_str.size();
    if (len > PROTOUP_MAX_LEN)
    {
        qDebug()<<"the message is too long!";
        return nullptr;
    }
    QByteArray data;
    data.resize(len);
    msg.head.length = len;
    head_encode(data, msg);
    // toLatin1 ascii编码 toLocal8Bit Unicode编码
    memcpy(data.data() + PROTOUP_HEAD_SIZE, json_str.toLatin1(), json_str.size());
    return data;
}

bool ProtoUpDecoder::parser(QByteArray &data, ProtoUpMsg & msg)
{
    bool success;
    success = parser_head(data, msg);
    if (success == false)
    {
        qDebug() << "parser head failed!";
        return false;
    }
    success = parser_body(data, msg);
    if (success == false)
    {
        qDebug() << "parser body failed!";
        return false;
    }
    return true;
}


bool ProtoUpDecoder::parser_head(QByteArray &data, ProtoUpMsg & msg)
{
    ProtoUpHead head;
    uint8_t * p_id = (uint8_t *)data.data();

    head.version = *(uint8_t *)p_id;
    ++p_id;
    if (head.version != PROTOUP_VERSION)
    {
        qDebug()<<"the protocol version doesn't match!";
    }

    head.code = ntohs(*(uint16_t *)p_id);
    p_id += 2;

    head.type = *p_id;
    ++p_id;

    head.length = ntohl(*(uint32_t *)p_id);
    if (head.length > PROTOUP_MAX_LEN)
    {
        qDebug()<<"the message is too long!";
        return false;
    }
    msg.head = head;
    return true;
}

bool ProtoUpDecoder::parser_body(QByteArray &data, ProtoUpMsg & msg)
{
    QString json_str;
    json_str = data.mid(8);
    QJsonObject json_object = get_json_object_from_string(json_str);
    msg.body = json_object;
    return true;
}

