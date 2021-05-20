/*
 * @Author: zhuyuejiang
 * @Date: 2021-04-07 09:00:04
 * @LastEditTime: 2021-04-29 10:11:58
 * @LastEditors: Please set LastEditors
 * @Description: 
 * 消息编码器，负责将CmqMessage编码（打包）为特定协议格式的报文，
 * 目前暂时只支持MQTT协议，后期计划同时支持AMQP
 * @FilePath: /CMQ/src/message/cmq_encoder.cpp
 */
#include "../../include/message/cmq_encoder.h"
#include "../../include/message/cmq_message_global.h"
#include "../../include/message/cmq_mqtt_packet_encoder.h"

namespace CMQ::Message{
/**
 * @description: 
 * MQTT协议编码函数，根据输入消息‘msg’内部信息，将消息打包成符合MQTT协议
 * 的报文，MQTT报文格式及相关要求已在文件头部说明，该函数依照如下逻辑进行编码：
 * 1. 首先编码固定报头，包括控制报文类型和控制报文标志，以及剩余长度信息；
 * 2. 根据消息对应的报文类型，编码可变报头
 * 3. 根据消息对应的报文类型，编码有效载荷，即应用数据部分
 * @param {const} CmqMessage： 消息对象
 * @return {*}
 */
CmqByteArray CmqMqttEncoder::msgEncode(const CmqMessage* msg)
{
    CmqByteArray packet;

    if(typeid(*msg) == typeid(CmqMqttMessage)){
        auto mqttPtr = (CmqMqttMessage*)(msg);
        auto encoder = CmqMqttPacketEncoderFactory::createPacketEncoder(mqttPtr->packetType());
        auto fixHeader = encoder->generateFixHeader(*mqttPtr);
        packet.insert(packet.end(), fixHeader.begin(), fixHeader.end());
        auto variableHeader = encoder->generateVariableHeader(*mqttPtr);
        packet.insert(packet.end(), variableHeader.begin(), variableHeader.end());
        auto playload = encoder->generatePayload(*mqttPtr);
        packet.insert(packet.end(), packet.begin(), packet.end());
    }

    // //第一步：固定报头-------------------------------------------------------
    // //报文类型
    // CmqByte fixHeader = static_cast<char>(msg.packetType()) << 4;
    // if(msg.duplicated()){
    //     fixHeader |= MQTT_CONTROL_PACKET_FLAG_DUP;
    // }
    // else{
    //     fixHeader |= 0x00;
    // }

    // //报文标志
    // if(msg.qos() == QOS::AT_LEAST_ONCE){
    //     fixHeader |= MQTT_CONTROL_PACKET_FLAG_QOS_L;
    // }
    // else if(msg.qos() == QOS::EXACTLY_ONCE){
    //     fixHeader |= MQTT_CONTROL_PACKET_FLAG_QOS_H;
    // }
    // packet.push_back(fixHeader);

    // //剩余长度
    // bool hasIden = CmqMqttEncoder::hasIdentifier(msg);
    // short remainLength = msg.data().size() + (hasIden ? 2 : 0);
    // auto bytes = CmqMqttEncoder::lengthEncode(remainLength);
    // packet.insert(packet.end(), bytes.begin(), bytes.end());

    // //第二步：可变报头(报文标识符)-----------------------------------------------
    // if(hasIden){
    //     packet.push_back(msg.packetIdentifier() & 0xFF00 >> 8);
    //     packet.push_back(msg.packetIdentifier() & 0x0F);
    // }

    // //第三步： 有效载荷----------------------------------------------------------
    // if(CmqMqttEncoder::hasPlayload(msg)){
    //     packet.insert(packet.end(), msg.data().begin(), msg.data().end());
    // }

    return packet;
}

CmqByteArray CmqAmqpEncoder::msgEncode(const CmqMessage* msg)
{
    return CmqByteArray();
}

/**
 * @description: 
 * 判断某条消息是否需要报文标识符
 * CONNECT      不需要
 * CONNACK      不需要
 * PUBLISH      QOS>0时需要
 * PUBACK       需要
 * PUBREC       需要
 * PUBREL       需要
 * PUBCOMP      需要
 * SUBSCRIBE    需要
 * SUBACK       需要
 * UNSUBSCRIBE  需要
 * UNSUBACK     需要
 * PINGREQ      不需要
 * PINGRESP     不需要
 * DISCONNECT   不需要
 * @param {const} CmqMessage
 * @return {*}
 */
// bool CmqMqttEncoder::hasIdentifier(const CmqMessage& msg)
// {
//     auto packetType = static_cast<short>(msg.packetType());
//     auto pub = static_cast<short>(MqttPacketType::PUBLISH);
//     auto unsuback = static_cast<short>(MqttPacketType::UNSUBACK);
//     if((packetType == pub && static_cast<short>(msg.qos()) > 0) || (packetType > pub && packetType <= unsuback)){
//         return true;
//     }

//     return false;
// }

/**
 * @description: 
 * 判断报文是否应该包含有效载荷：
 * CONNECT    需要
 * CONNACK    不需要
 * PUBLISH    可选
 * PUBACK     不需要
 * PUBREC     不需要
 * PUBREL     不需要
 * PUBCOMP    不需要
 * SUBSCRIBE  需要
 * SUBACK     需要
 * UNSUBSCRIBE 需要
 * UNSUBACK   不需要
 * PINGREQ    不需要
 * PINGRESP   不需要
 * DISCONNECT 不需要
 * @param {const} CmqMessage
 * @return {*}
 */
// bool CmqMqttEncoder::hasPlayload(const CmqMessage& msg)
// {
//     auto packetType = msg.packetType();
//     if(MqttPacketType::PUBLISH == packetType || MqttPacketType::CONNECT == packetType || MqttPacketType::SUBSCRIBE == packetType
//     || MqttPacketType::UNSUBSCRIBE == packetType || MqttPacketType::SUBACK == packetType){
//         return true;
//     }

//     return false;
// }

/**
 * @description: 
 * @param {int} length
 * @return {*}
 */

}