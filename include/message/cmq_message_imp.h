/*
 * @Author: your name
 * @Date: 2021-04-07 11:54:32
 * @LastEditTime: 2021-04-07 11:58:45
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_message_imp.h
 */
#ifndef CMQ_MESSAGE_IMP_H
#define CMQ_MESSAGE_IMP_H

#include "../common/cmq_global.h"
#include <vector>
#include <QtCore/QString>
using namespace std;

namespace CMQ::Message{
//消息服务质量
enum class QOS{
    AT_MOST_ONCE = 0,   //最多发送一次
    AT_LEAST_ONCE = 1,  //至少一次
    EXACTLY_ONCE = 2    //仅一次
};

//应用层消息协议
enum class Protocal{
    MQTT = 0,
    AMQP = 1
};

//消息发起方
enum class MessageSource{
    CLIENT = 0,  //由客户端发起，一般消息节点（除broker外）在CMQ种均为消息客户端
    SERVER = 1  //特指broker
};

//MQTT报文类型
enum class MqttPacketType{
    CONNECT = 1,      //客户端请求连接服务端
    CONNACK = 2,      //对客户端连接请求报文的确认报文
    PUBLISH = 3,      //发布消息
    PUBACK = 4,       //QOS1消息发布收到确认
    PUBREC = 5,       //发布收到（保证交互第一步）
    PUBREL = 6,       //发布释放（保证交互第二步）
    PUBCOMP = 7,      //QOS2消息发布完成（保证交互第三步）
    SUBSCRIBE = 8,    //客户端订阅请求
    SUBACK = 9,       //对客户端订阅请求的确认报文
    UNSUBSCRIBE = 10, //客户端取消订阅请求
    UNSUBACK = 11,    //对客户端取消订阅请求的确认报文
    PINGREQ = 12,     //心跳请求
    PINGRESP = 13,    //心跳响应
    DISCONNECT = 14   //客户端断开连接
};

static const QString DEFAULT_TAG("CMQ");
static const QString DEFAULT_KEY("CMQ_MSG");
static const int INIT_DATA_SIZE = 32;

class CmqMessageImp{
public:
    Protocal _protocal;
    QString _id;
    QString _key;
    QString _tag;
    QString _topic;
    qint64 _timeStamp;
    QOS _qos;
    MessageSource _source;
    MqttPacketType _mpt;    //mqtt协议下有效
    short _packetIdentifier; //mqtt报文标识符
    bool _duplicated; //是否是重复发送的报文
    vector<char> _data;

    CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, Protocal protocal, MqttPacketType mpt);
    CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, const vector<char>& data, Protocal protocal, MqttPacketType mpt);

    void appendData(const vector<char>& data)
    {
        _data.insert(_data.end(), data.begin(), data.end());
    }

    vector<char> serialize();
    void deserialize(const vector<char>& data);
};
}

#endif