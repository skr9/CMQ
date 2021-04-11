/*
 * @Author: ZHUYUEJIANG
 * @Date: 2021-03-29 14:26:48
 * @LastEditTime: 2021-04-11 07:43:15
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
    CmqMessage(const QString& msgid, const QString& topic, short packetIdentifier,  MqttPacketType mpt = MqttPacketType::PUBLISH,  Protocal protocal = Protocal::MQTT): 
    _imp_ptr(new CmqMessageImp(msgid, topic, packetIdentifier, protocal, mpt))
    {
    }

    CmqMessage(const QString& msgid, const QString& topic, short packetIdentifier, const vector<char>& data, MqttPacketType mpt = MqttPacketType::PUBLISH, Protocal protocal = Protocal::MQTT):
    _imp_ptr(new CmqMessageImp(msgid, topic, packetIdentifier, data, protocal, mpt))
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

    inline Protocal protocal()const{return _imp_ptr->_protocal;}
    inline void setProtocal(Protocal protocal){_imp_ptr->_protocal = protocal;}
    inline QString id()const{return _imp_ptr->_id;}
    inline void setMsgid(const QString& msgid){_imp_ptr->_id = msgid;}
    inline QString key()const{return _imp_ptr->_key;}
    inline void setKey(const QString& key){_imp_ptr->_key = key;}
    inline QString tag()const{return _imp_ptr->_tag;}
    inline void setTag(const QString& tag){_imp_ptr->_tag = tag;}
    inline QString topic()const{return _imp_ptr->_topic;}
    inline void setTopic(const QString& topic){_imp_ptr->_topic = topic;}
    inline qint64 timeStamp()const{return _imp_ptr->_timeStamp;}
    inline QOS qos()const{return _imp_ptr->_qos;}
    inline void setQos(QOS qos){_imp_ptr->_qos = qos;}
    inline MessageSource source()const{return _imp_ptr->_source;}
    inline void setSource(MessageSource src){_imp_ptr->_source = src;}
    inline MqttPacketType packetType()const{return _imp_ptr->_mpt;}
    inline void setMqttPacketType(MqttPacketType mpt){_imp_ptr->_mpt = mpt;}
    inline short packetIdentifier()const{return _imp_ptr->_packetIdentifier;}
    inline bool duplicated()const{return _imp_ptr->_duplicated;}
    inline void setDuplicateFlag(bool duplicated){_imp_ptr->_duplicated = duplicated;}

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
}

#endif
