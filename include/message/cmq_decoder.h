/*
 * @Author: your name
 * @Date: 2021-04-11 09:59:58
 * @LastEditTime: 2021-04-14 00:58:57
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

};
}
#endif
