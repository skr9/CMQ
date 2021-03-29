/*
 * @Author: ZHUYUEJIANG
 * @Date: 2021-03-29 14:26:48
 * @LastEditTime: 2021-03-29 17:42:55
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CMQ\message\cmq_message.hpp
 */

#ifndef CMQ_MESSAGE_H_
#define CMQ_MESSAGE_H_

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QDateTime>

#define DEFAULT_TAG "CMQ"

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

class CmqMessage{
public:
    explicit CmqMessage(const QString& msgid, const QString& topic, Protocal protocal = Protocal::MQTT): _protocal(protocal),
    _id(msgid), _key(topic), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()), _qos(QOS::AT_MOST_ONCE)
    {
    }

    CmqMessage(CmqMessage&& msg)
    {
        _protocal = msg._protocal;
        _id = msg._id;
        _key = msg._key;
        _tag = msg._tag;
        _topic = msg._topic;
        _timeStamp = msg._timeStamp;
        _qos = msg._qos;
        _data.append(msg.data());
    }

    const CmqMessage& operator = (const CmqMessage& msg)
    {
        *this = msg;
        return *this;
    }

    virtual ~CmqMessage(){};

    inline Protocal protocal()const{return _protocal;}
    inline QString id()const{return _id;}
    inline QString key()const{return _key;}
    inline QString tag()const{return _tag;}
    inline QString topic()const{return _topic;}
    inline qint64 timeStamp()const{return _timeStamp;}
    inline QOS qos()const{return _qos;}
    inline const QByteArray& data()const{return _data;}

private:
    Protocal _protocal;
    QString _id;
    QString _key;
    QString _tag;
    QString _topic;
    qint64 _timeStamp;
    QOS _qos;
    QByteArray _data;
};
}

#endif
