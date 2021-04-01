/*
 * @Author: ZHUYUEJIANG
 * @Date: 2021-03-29 14:26:48
 * @LastEditTime: 2021-04-01 09:33:25
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CMQ\message\cmq_message.hpp
 */

#ifndef CMQ_MESSAGE_H_
#define CMQ_MESSAGE_H_

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/qmutex.h>
#include <vector>
#include "../common/ISerializable.hpp"
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

static const QString DEFAULT_TAG("CMQ");

class CmqMessage:public Common::ISerializable{
private:
    static const int INIT_DATA_SIZE = 32;
    QMutex _dataMutex;

public:
    CmqMessage(const QString& msgid, const QString& topic, Protocal protocal = Protocal::MQTT): _protocal(protocal),_id(msgid), 
    _key(topic), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()), _qos(QOS::AT_MOST_ONCE), _data(INIT_DATA_SIZE)
    {
    }

    CmqMessage(const QString& msgid, const QString& topic, const vector<char>& data, Protocal protocal = Protocal::MQTT):
    _protocal(protocal), _id(msgid), _key(topic), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()),
    _qos(QOS::AT_MOST_ONCE), _data(data)
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
        _data.swap(msg._data);
    }

    const CmqMessage& operator = (const CmqMessage& msg)
    {
        *this = msg;
        return *this;
    }

    ~CmqMessage(){}

    inline Protocal protocal()const{return _protocal;}
    inline void setProtocal(Protocal protocal){_protocal = protocal;}
    inline QString id()const{return _id;}
    inline void setMsgid(const QString& msgid){_id = msgid;}
    inline QString key()const{return _key;}
    inline void setKey(const QString& key){_key = key;}
    inline QString tag()const{return _tag;}
    inline void setTag(const QString& tag){_tag = tag;}
    inline QString topic()const{return _topic;}
    inline void setTopic(const QString& topic){_topic = topic;}
    inline qint64 timeStamp()const{return _timeStamp;}
    inline QOS qos()const{return _qos;}
    inline void setQos(QOS qos){_qos = qos;}

    inline const vector<char>& data()const{return _data; }
    inline void clearData(){ _data.clear();}
    inline void resetData(const vector<char>& newData){ _data = newData;}

    inline void resetData(vector<char>&& newData){ _data.swap(newData);}

    void appendData(const vector<char>& data)
    {
        _data.insert(_data.end(), data.begin(), data.end());
    }

    vector<char> serialize() override
    {
        return vector<char>();
    }

    void deserialize(const vector<char>& data) override
    {
        
    }

private:
    Protocal _protocal;
    QString _id;
    QString _key;
    QString _tag;
    QString _topic;
    qint64 _timeStamp;
    QOS _qos;
    vector<char> _data;
};
}

#endif
