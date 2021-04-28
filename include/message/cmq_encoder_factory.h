/*
 * @Author: your name
 * @Date: 2021-04-27 12:39:36
 * @LastEditTime: 2021-04-28 01:07:45
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_encoder_factory.h
 */

#ifndef CMQ_ENCODER_FACTORY_H
#define CMQ_ENCODER_FACTORY_H

#include "cmq_encoder.h"

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

class CmqMqttPacketEncoder{
public:
    virtual CmqByteArray generateFixHeader(const CmqMqttMessage& msg) = 0;
    virtual CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) = 0;
    virtual CmqByteArray generatePlayload(const CmqMqttMessage& msg) = 0;
};

class CmqConnectPacketEncoder final : public CmqMqttPacketEncoder{
public:
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generatePlayload(const CmqMqttMessage& msg) override;
};

class CmqConnackPacketEncoder final : public CmqMqttPacketEncoder{
public:
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generatePlayload(const CmqMqttMessage& msg) override;
};

class CmqPublishPacketEncoder : public CmqMqttPacketEncoder{
public:
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generatePlayload(const CmqMqttMessage& msg) override;
};

class CmqPubackPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqPubrelPacketEncoder : public CmqPubackPacketEncoder{

};

class CmqPubrecPacketEncoder: public CmqPubackPacketEncoder{

};

class CmqPubcompPacketEncoder : public CmqPubackPacketEncoder{

};

class CmqSubscriblePacketEncoder: public CmqMqttPacketEncoder{

};

class CmqSubackPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqUnsubPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqUnsubackPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqDisconnectPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqPingreqPacketEncoder : public CmqMqttPacketEncoder{

};

class CmqPingrespPacketEncoder : public CmqMqttPacketEncoder{

};


class CmqMqttPacketEncoderFactory{
public:
    static shared_ptr<CmqMqttPacketEncoder> createPacketEncoder(MqttPacketType mpt)
    {
        switch (mpt)
        {
            case MqttPacketType::CONNECT: return make_shared<CmqConnackPacketEncoder>();
            case MqttPacketType::DISCONNECT: return make_shared<CmqDisconnectPacketEncoder>();
            case MqttPacketType::PINGREQ: return make_shared<CmqPingreqPacketEncoder>();
            case MqttPacketType::PINGRESP: return make_shared<CmqPingrespPacketEncoder>();
            case MqttPacketType::PUBACK: return make_shared<CmqPubackPacketEncoder>();
            case MqttPacketType::PUBCOMP: return make_shared<CmqPubcompPacketEncoder>();
            case MqttPacketType::PUBLISH: return make_shared<CmqPublishPacketEncoder>();
            case MqttPacketType::PUBREC: return make_shared<CmqPubrecPacketEncoder>();
            case MqttPacketType::PUBREL: return make_shared<CmqPubrelPacketEncoder>();
            case MqttPacketType::SUBACK: return make_shared<CmqSubackPacketEncoder>();
            case MqttPacketType::SUBSCRIBE: return make_shared<CmqSubscriblePacketEncoder>();
            case MqttPacketType::UNSUBACK: return make_shared<CmqUnsubackPacketEncoder>();
            case MqttPacketType::CONNACK: return make_shared<CmqConnackPacketEncoder>();
            case MqttPacketType::UNSUBSCRIBE: return make_shared<CmqUnsubPacketEncoder>();
            default: return nullptr;
        }
    }
};
};

#endif