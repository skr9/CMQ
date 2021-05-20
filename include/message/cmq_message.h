/*
 * @Author: ZHUYUEJIANG
 * @Date: 2021-03-29 14:26:48
 * @LastEditTime: 2021-05-01 11:21:47
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CMQ\message\cmq_message.hpp
 */

#ifndef CMQ_MESSAGE_H_
#define CMQ_MESSAGE_H_

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include "cmq_message_imp.h"
#include "../common/ISerializable.h"
using namespace std;

namespace CMQ::Message{

class CmqMessage:public Common::ISerializable{
public:
    CmqMessage() = delete;
    CmqMessage(const QString& topic, Protocal protocal = Protocal::MQTT): 
    _imp_ptr(new CmqMessageImp(topic, protocal))
    {
    }

    CmqMessage(const QString& topic, const vector<char>& data, Protocal protocal = Protocal::MQTT):
    _imp_ptr(new CmqMessageImp(topic, data, protocal))
    {
    }

    CmqMessage(const CmqMessage& msg):_imp_ptr(make_unique<CmqMessageImp>(*msg._imp_ptr))
    {
    }

    CmqMessage(CmqMessage&& msg)
    {
        _imp_ptr.swap(msg._imp_ptr);
        msg._imp_ptr = nullptr;
    }

    const CmqMessage& operator = (const CmqMessage& msg)
    {
        _imp_ptr.reset(new CmqMessageImp(* msg._imp_ptr));
        return *this;
    }

    ~CmqMessage(){}

    inline Protocal protocal()const{return _imp_ptr->protocal;}
    inline void setProtocal(Protocal protocal){_imp_ptr->protocal = protocal;}
    inline QString id()const{return _imp_ptr->id;}
    inline void setMsgid(const QString& msgid){_imp_ptr->id = msgid;}
    inline QString key()const{return _imp_ptr->key;}
    inline void setKey(const QString& key){_imp_ptr->key = key;}
    inline QString tag()const{return _imp_ptr->tag;}
    inline void setTag(const QString& tag){_imp_ptr->tag = tag;}
    inline QString topic()const{return _imp_ptr->topic;}
    inline void setTopic(const QString& topic){_imp_ptr->topic = topic;}
    inline qint64 timeStamp()const{return _imp_ptr->timeStamp;}
    inline QString source()const{return _imp_ptr->source;}
    inline void setSource(const QString& src){_imp_ptr->source = src;}
    inline const vector<char>& data()const{return _imp_ptr->_data; }
    inline void clearData(){ _imp_ptr->_data.clear();}
    inline void resetData(const vector<char>& newData){ _imp_ptr->_data = newData;}

    inline void resetData(vector<char>&& newData){ _imp_ptr->_data.swap(newData);}

    inline void appendData(CmqByte byte)
    {
        _imp_ptr->appendData(byte);
    }

    inline void appendData(const CmqByteArray& data)
    {
        _imp_ptr->appendData(data);
    }

    vector<char> serialize() override
    {
        return _imp_ptr->serialize();
    }

    void deserialize(const vector<char>& data) override
    {
        _imp_ptr->deserialize(data);
    }

private:
    CMQ_DECLARE_IMP(CmqMessage)
};

class CmqMqttMessage: public CmqMessage{
public:
    CmqMqttMessage() = delete;
    CmqMqttMessage(const QString& topic, short packetIdentifier, MqttPacketType mpt)
    :CmqMessage(topic), _imp_ptr(make_unique<CmqMqttMessageImp>(topic, packetIdentifier, mpt)){}
    CmqMqttMessage(const QString& topic, short packetIdentifier, const CmqByteArray& data, MqttPacketType mpt)
    :CmqMessage(topic, data), _imp_ptr(make_unique<CmqMqttMessageImp>(topic, packetIdentifier, data, mpt)){}
    inline QOS qos()const{return _imp_ptr->qos;}
    inline void setQos(QOS qos){_imp_ptr->qos = qos;}
    inline MqttPacketType packetType()const{return _imp_ptr->mpt;}
    inline void setMqttPacketType(MqttPacketType mpt){_imp_ptr->mpt = mpt;}
    inline short packetIdentifier()const{return _imp_ptr->packetIdentifier;}
    inline bool duplicated()const{return _imp_ptr->duplicated;}
    inline void setDuplicateFlag(bool duplicated){_imp_ptr->duplicated = duplicated;}

    inline short topicAlias()const{return _imp_ptr->topicAlias;}
    void setTopicAlias(short alias){_imp_ptr->topicAlias = alias;}

    const ConnectPacketField* connectField()const {return &(_imp_ptr->connectField);}
    void setConnectPacketFiled(const ConnectPacketField& cpf){_imp_ptr->connectField = cpf;}

    const ConnackPacketField* connackField()const{return &(_imp_ptr->connackField);}
    void setConnackPacketField(const ConnackPacketField& cpf){_imp_ptr->connackField = cpf;}

    const CommonField* commonField()const{return &(_imp_ptr->commonField);}
    void setCommonField(const CommonField& cf){_imp_ptr->commonField = cf;}

    qint8 subscriptionOptions()const{return _imp_ptr->subscriptionOptions;}
    void setSubscriptionOptions(qint8 opt){_imp_ptr->subscriptionOptions = opt;}

private:
    CMQ_DECLARE_IMP(CmqMqttMessage)
};
}

#endif
