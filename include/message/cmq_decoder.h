/*
 * @Author: your name
 * @Date: 2021-04-11 09:59:58
 * @LastEditTime: 2021-04-17 05:57:11
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_decoder.h
 */

#ifndef CMQ_DECODER_H_
#define CMQ_DECODER_H_

#include "cmq_message.h"

namespace CMQ::Message{
class CmqDecoder{
public:
    virtual ~CmqDecoder(){}

    virtual CmqMessage msgDecode(const CmqByteArray& packet) = 0;
};

class CmqMqttDecoder final:public CmqDecoder{
public:
    CmqMqttDecoder() = default;
    ~CmqMqttDecoder(){};

    CmqMessage msgDecode(const CmqByteArray& packet) override;

private:
    static int lengthCompute(const CmqByteArray& packet, int& currentPosition, int lengthByteStartPosition = 1);
    static bool hasIdentifier(MqttPacketType packetType, QOS qos);
    
};
}
#endif
