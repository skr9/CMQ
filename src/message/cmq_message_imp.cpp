/*
 * @Author: your name
 * @Date: 2021-04-07 11:59:36
 * @LastEditTime: 2021-04-07 12:00:28
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/src/message/cmq_message_imp.cpp
 */
#include "../../include/message/cmq_message_imp.h"
#include <QtCore/QDateTime>

namespace CMQ::Message{
CmqMessageImp::CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, Protocal protocal, MqttPacketType mpt):
_protocal(protocal), _id(msgid), _key(DEFAULT_KEY), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()),
_source(MessageSource::CLIENT), _mpt(mpt), _packetIdentifier(packetIdentifier), _duplicated(false), _data(INIT_DATA_SIZE)
{
}

CmqMessageImp::CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, const vector<char>& data, Protocal protocal, MqttPacketType mpt):
_protocal(protocal), _id(msgid), _key(DEFAULT_KEY), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()),
_source(MessageSource::CLIENT), _mpt(mpt), _packetIdentifier(packetIdentifier), _duplicated(false), _data(data)
{
}

vector<char> CmqMessageImp::serialize()
{
    return vector<char>();
}

void CmqMessageImp::deserialize(const vector<char>& data)
{

}
}