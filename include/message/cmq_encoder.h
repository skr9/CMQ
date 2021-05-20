/*
 * @Author: your name
 * @Date: 2021-04-07 12:06:59
 * @LastEditTime: 2021-04-29 10:12:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_encoder.h
 */
#ifndef CMQ_ENCODER_H_
#define CMQ_ENCODER_H_

#include <exception>
#include "../common/cmq_global.h"
#include "cmq_message.h"

namespace CMQ::Message{
/**
 * @description: 
 * 简单工厂方法工具类，用于辅助创建编码器
 * @param {*}
 * @return {*}
 */
class CmqEncoderGenerator{
public:
    static shared_ptr<CmqEncoder> createEncoder(Protocal protocal)
    {
        if(Protocal::MQTT == protocal){
            return make_shared<CmqMqttEncoder>();
        }
        else if(Protocal::AMQP == protocal){
            return make_shared<CmqAmqpEncoder>();
        }
        else{
            return nullptr;
        }
    }
};

//编码器基类
class CmqEncoder{
public:
    virtual ~CmqEncoder(){}

    virtual CmqByteArray  msgEncode(const CmqMessage* msg) = 0;
};

//MQTT协议编码器类
class CmqMqttEncoder final: public CmqEncoder{
public:
    CmqMqttEncoder() = default;
    ~CmqMqttEncoder(){};

    CmqByteArray msgEncode(const CmqMessage* msg) override;

private:
    static CmqByteArray lengthEncode(int length);
    static bool hasIdentifier(const CmqMessage& msg);
    static bool hasPlayload(const CmqMessage& msg);
};

//预留类，具体功能暂未实现
class CmqAmqpEncoder final : public CmqEncoder{
public:
    CmqAmqpEncoder() = default;
    ~CmqAmqpEncoder(){}

    CmqByteArray msgEncode(const CmqMessage* msg) override;  //目前未实现具体功能，无意义
};
}

#endif