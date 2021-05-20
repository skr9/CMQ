/*
 * @Author: your name
 * @Date: 2021-04-07 11:54:32
 * @LastEditTime: 2021-05-13 14:09:02
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

class CmqMessageImp{
public:
    Protocal protocal;
    QString id;
    QString key;
    QString tag;
    QString topic;
    qint64 timeStamp;
    QString source;
    vector<char> _data;

    CmqMessageImp() = delete;
    CmqMessageImp(const QString& topic, Protocal protocal);
    CmqMessageImp(const QString& topic, const vector<char>& data, Protocal protocal);
    CmqMessageImp(const CmqMessageImp& imp) = default;

    void appendData(CmqByte byte)
    {
        _data.push_back(byte);
    }

    void appendData(const CmqByteArray& data)
    {
        _data.insert(_data.end(), data.begin(), data.end());
    }

    virtual vector<char> serialize();
    virtual void deserialize(const vector<char>& data);
};

//连接标志位，出现在CONNECT报文中
enum ConnectFlag{
    CLEAN_START = 0X02,
    WILL_FLAG = 0X04,
    WILL_QOS_L = 0X08,
    WILL_QOS_H = 0X10,
    WILL_RETAIN = 0X20,
    PASSWORD_FLAG = 0X40,
    USER_NAME_FLAG = 0X80,
};

 /*CONNECT 报文特有的内容*/
class ConnectPacketField{
public:
    explicit ConnectPacketField():version(0x05), connectFlag(0), requestResponseInfo(0),
    requestProblemInfo(0), willDelayInterval(0), userPropertyData(""), willTopic(""),
    willPlayload(),  userName(""), password("")
    {
    }
    qint8 version; //协议版本号，默认5，表示最新版本MQTT5， 3.1.1版本的对应值为4
    qint8 connectFlag; //连接标志位，包含：username flag, password flag, will retain, will QOS, will flag, clean start
    qint8 requestResponseInfo; //告知server端是否可以发送响应信息
    qint8 requestProblemInfo; 

    /*CONNECT报文中有效载荷内容*/
    int willDelayInterval; 
    QString userPropertyData;
    QString willTopic;
    QString userName;
    QString password;
    CmqByteArray willPlayload;

    inline static int USER_NAME_FLAG = 0b1000'0000;
    inline static int PASSWORD_FLAG = 0b0100'0000;
    inline static int WILL_FLAG = 0b0000'0100;
    inline static int WILL_RETAIN_FLAG = 0b0010'0000;
    inline static int CLEAN_START_FLAG = 0b0000'0010;

    bool hasUserName()const{return connectFlag & USER_NAME_FLAG;}
    void setUserNameFlag(bool flag = true)
    {
        connectFlag = flag ? (connectFlag | USER_NAME_FLAG) : (connectFlag & (~USER_NAME_FLAG));
    }

    bool hasPassword()const{return connectFlag & PASSWORD_FLAG;}
    void setPasswordFlag(bool flag = true)
    {
        connectFlag = flag ? (connectFlag | PASSWORD_FLAG) : (connectFlag & (~PASSWORD_FLAG));
    }

    bool hasWillMessage()const{return connectFlag & WILL_FLAG;}
    qint8 willQos()const{return connectFlag & 0b0001'1000;}
    bool retainWill()const{return connectFlag & WILL_RETAIN_FLAG;}
    void setWill(bool willFlag, qint8 willQos, bool retainWill = true)
    {
        if(willFlag){
            connectFlag = connectFlag | WILL_FLAG;
            if(willQos == 1){
                connectFlag &= 0b1110'1111;
            }
            else if(willQos == 2){
                connectFlag &= 0b1111'1111;
            }
            else{
                connectFlag &= 0b1110'0111;
            }
            connectFlag = retainWill ? (connectFlag | WILL_RETAIN_FLAG) : (connectFlag & (~WILL_RETAIN_FLAG)); 
        }
        else{
            connectFlag &= 0b1100'0011;
        }
    }

    bool cleanStart()const{return connectFlag & CLEAN_START_FLAG;}
    void setCleanStart(bool flag = true)
    {
        connectFlag = flag ? (connectFlag | CLEAN_START_FLAG) : (connectFlag & (~CLEAN_START_FLAG));
    }

};

//连接状态反馈码，出现在CONNACK报文中
enum ConnectReasonCode{
    SUCCESS = 0X00,
    UNSPECIFIED_ERROR = 0X80,
    MALFORMED_PACKET = 0X81,
    PROTOCAL_ERROR = 0X82,
    IMPLEMENTATION_SPECIFIC_ERROR = 0X83,
    UNSUPPORTED_PROTOCAL_VERSION = 0X84,
    CLIENT_IDENTIFIER_NOT_VALID = 0X85,
    BAD_USER_NAME_OR_PASSWORD = 0X86,
    NOT_AUTHORIZED = 0X87,
    SERVER_UNAVAILABLE = 0X88,
    SERVER_BUSY = 0X89,
    BANNED = 0X8A,
    BAD_AUTHENTICATION_METHOD = 0X8C,
    TOPIC_NAME_INVAILD = 0X90,
    PACKET_TOO_LARGE = 0X95,
    QUOTA_EXCEEDED = 0X97,
    PLAYLOAD_FORMATE_INVAILD = 0X99,
    RETAIN_NOT_SUPPORTED = 0X9A,
    QOS_NOT_SUPPORTED = 0X9B,
    USE_ANOTHER_SERVER = 0X9C,
    SERVER_MOVED = 0X9D,
    CONNECTION_RATE_EXCEEDED = 0X9F,
};


/*CONNACK报文标志位和属性*/
class ConnackPacketField{ 
public:
    qint8 sessionPresent; 
    qint8 connectReasonCode;
    qint8 retainAvailable;  
    qint8 wildcardSubscriptionAvailable;
    qint8 subscriptionIdentifiersAvailable;
    qint8 sharedSubscriptionAvailable;
    short serverKeepAlive;
    QString assignedClientIdentifier;
    QString responseInfo;
    QString serverReference;
};

//通用标志位或属性，出现在两个或多个报文中
class CommonField{
public:
    qint8 maxQos;
    qint8 playloadFormatIndicator;
    qint8 reasonCode;
    short receiveMaximum; //client可同时接收的QOS1和QOS2的消息数量
    short topicAliasMaximum;
    short messageExpiryInterval;
    short keepLiveInterval; //保活时间，单位秒，超过保活时间未收到任何消息，视为连接断开
    int sessionExpiryInterval; //会话过期时间，单位秒，连接断开后会话状态保持时间
    int maximumPacketSize; //client可接受的最大包大小，单位字节
    int subscriptionIdentifier;
    CmqByteArray authenticationData;
    CmqByteArray correlationData;
    QString clientIdentifier; //client标识符（clientID）
    QString authenticationMethod;
    QString responseTopic; 
    QString contentType;
    QString reasonString;
    QStringList userProperty;
};

//MQTT消息实现类
class CmqMqttMessageImp: public CmqMessageImp{
public:
    CmqMqttMessageImp() = delete;
    CmqMqttMessageImp(const QString& topic, short packetIdentifier, MqttPacketType mpt);
    CmqMqttMessageImp(const QString& topic, short packetIdentifier, const CmqByteArray& data, MqttPacketType mpt);
    CmqMqttMessageImp(const CmqMqttMessageImp& imp) = default;
    
    QOS qos;
    MqttPacketType mpt;    
    bool duplicated; //是否是重复发送的报文
    bool retain;
    short packetIdentifier; //mqtt报文标识符
    short topicAlias;

    ConnectPacketField connectField;
    ConnackPacketField connackField;
    CommonField commonField;

    //SUBSCRIBE报文标志位和属性
    qint8 subscriptionOptions;
};
}

#endif