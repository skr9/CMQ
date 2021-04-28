/*
 * @Author: your name
 * @Date: 2021-04-07 11:59:36
 * @LastEditTime: 2021-04-26 13:20:26
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/src/message/cmq_message_imp.cpp
 */
#include "../../include/message/cmq_message_imp.h"
#include <QtCore/QDateTime>

#define DEFAULT_SOURCE_TAG "0.0.0.0:0"  //默认的消息发起方标识符

namespace CMQ::Message{
CmqMessageImp::CmqMessageImp(const QString& topic, Protocal protocal):
protocal(protocal), id(""), key(DEFAULT_KEY), tag(DEFAULT_TAG), topic(topic), timeStamp(QDateTime::currentMSecsSinceEpoch()),
 source(DEFAULT_SOURCE_TAG), _data(0)
{
}

CmqMessageImp::CmqMessageImp(const QString& topic, const vector<char>& data, Protocal protocal):
protocal(protocal), id(""), key(DEFAULT_KEY), tag(DEFAULT_TAG), topic(topic), timeStamp(QDateTime::currentMSecsSinceEpoch()),
 source(DEFAULT_SOURCE_TAG), _data(data)
{
}

vector<char> CmqMessageImp::serialize()
{
    return vector<char>();
}

void CmqMessageImp::deserialize(const vector<char>& data)
{

}

CmqMqttMessageImp::CmqMqttMessageImp(const QString& topic, short packetIdentifier, MqttPacketType mpt)
:CmqMessageImp(topic, Protocal::MQTT), qos(QOS::AT_MOST_ONCE), mpt(mpt), packetIdentifier(packetIdentifier),
duplicated(false), retain(false), topicAlias(0), subscriptionOptions(0){

}

CmqMqttMessageImp::CmqMqttMessageImp(const QString& topic, short packetIdentifier, const CmqByteArray& data, MqttPacketType mpt)
:CmqMessageImp(topic, data, Protocal::MQTT),qos(QOS::AT_MOST_ONCE), mpt(mpt), packetIdentifier(packetIdentifier),
duplicated(false), retain(false), topicAlias(0), subscriptionOptions(0){

}
}