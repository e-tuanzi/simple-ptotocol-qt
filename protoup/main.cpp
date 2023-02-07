#include "widget.h"
#include "protoup.h"
#include <QApplication>

void test_msg_print()
{
    ProtoUpMsg msg;
    msg.head.code = 200;
    msg.head.length = 10;
    msg.body.insert("name", "tuanzi");
    msg.body.insert("age", 22);
    proto_up_msg_print(msg);
}

void test_msg_encode_decode()
{
    ProtoUpMsg msg;
    msg.head.code = 200;
    msg.body.insert("name", "tuanzi");
    msg.body.insert("age", 22);
    proto_up_msg_print(msg);
    ProtoUpEncoder encoder;
    ProtoUpDecoder decoder;
    uint8_t * p_data = encoder.encode(&msg);
    ProtoUpMsg msg1;
    decoder.parser(p_data, msg1);
    proto_up_msg_print(msg1);
}

int main(int argc, char *argv[])
{
//    QApplication a(argc, argv);
//    Widget w;
//    w.show();
//    test_msg_print();
    test_msg_encode_decode();
//    return a.exec();
    return 0;
}
