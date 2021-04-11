/*
 * @Author: your name
 * @Date: 2021-04-07 11:59:36
 * @LastEditTime: 2021-04-11 07:56:18
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/src/message/cmq_message_imp.cpp
 */
#include "../../include/message/cmq_message_imp.h"
#include <QtCore/QDateTime>

namespace CMQ::Message{
CmqMessageImp::CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, Protocal protocal, MqttPacketType mpt):
_protocal(protocal), _id(msgid), _key(DEFAULT_KEY), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()),
_qos(QOS::AT_MOST_ONCE), _source(MessageSource::CLIENT), _mpt(mpt), _packetIdentifier(packetIdentifier), _duplicated(false), _data(0)
{
}

CmqMessageImp::CmqMessageImp(const QString& msgid, const QString& topic, short packetIdentifier, const vector<char>& data, Protocal protocal, MqttPacketType mpt):
_protocal(protocal), _id(msgid), _key(DEFAULT_KEY), _tag(DEFAULT_TAG), _topic(topic), _timeStamp(QDateTime::currentMSecsSinceEpoch()),
_qos(QOS::AT_MOST_ONCE), _source(MessageSource::CLIENT), _mpt(mpt), _packetIdentifier(packetIdentifier), _duplicated(false), _data(data)
{
}

CmqMessageImp::CmqMessageImp(const CmqMessageImp& imp):
_protocal(imp._protocal), _id(imp._id), _key(imp._key), _tag(imp._tag), _topic(imp._topic), _timeStamp(imp._timeStamp),
_qos(imp._qos), _source(imp._source), _mpt(imp._mpt), _packetIdentifier(imp._packetIdentifier), _duplicated(imp._duplicated)
{
    _data.insert(_data.end(), imp._data.begin(), imp._data.end());
}

vector<char> CmqMessageImp::serialize()
{
    return vector<char>();
}

void CmqMessageImp::deserialize(const vector<char>& data)
{

}
}