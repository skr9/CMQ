/*
 * @Author: zhuyuejiang
 * @Date: 2021-04-07 09:00:04
 * @LastEditTime: 2021-04-21 13:22:09
 * @LastEditors: Please set LastEditors
 * @Description: 
 * 消息编码器，负责将CmqMessage编码（打包）为特定协议格式的报文，
 * 目前暂时只支持MQTT协议，后期计划同时支持AMQP
 * @FilePath: /CMQ/src/message/cmq_encoder.cpp
 */
#include "../../include/message/cmq_encoder.h"
#include "../../include/message/cmq_message_global.h"

namespace CMQ::Message{

//MQTT 报文结构
/************************************************************
 * Bit          7  6  5  4   |   3  2  1  0                 *
 * ---------------------------------------------------------*
 * byte1        控制报文类型    控制报文标志位     | 固定     *
 * byte2...      剩      余     长         度    | 报头      *
 * -----------------------------------------------------    *
 * byte1...       可     变    报     头                     *
 * -----------------------------------------------------    *
 * byte1...     有    效    载    荷(Playload)              *
 * 
 * 其中，根据报文类型不同，固定报头、可变报头以及有效载荷
 * 的格式/内容各有不同，详细请参见：third_party/mqtt-doc
 ************************************************************/

/************************************************************************************************************************
* 剩余长度 = 可变报头字节数 + 有效载荷字节数， 剩余长度本身占据1到4个字节，采用变长度编码，具体计算方法如下：
* 假设剩余字节数（即可变报头字节数 + 有效载荷字节数）为 X， 则剩余长度报文段字节值可如下计算：
* do{
*   encodeByte = X MOD 128
*   X = X DIV 128
*   if (X > 0):
*       encodeByte = encodeByte OR 128
*   endif
*   byte(n) = encodeByte   # n表示第几个字节，如byte(0)表示剩余长度的第一个字节，最多4个字节
* }while(X > 0)
*   
* 示例：
* 假设剩余长度为1000
* 1000 = 104 + 7 * 128
* 则剩余长度报文段第一个字节的低7位值为：1101000，最高位为1，表示有后续字节； 第二个字节的低位值为0x07,最高位为0，表示无后续字节
**************************************************************************************************************************/

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
CmqByteArray CmqMqttEncoder::msgEncode(const CmqMessage& msg)
{
    CmqByteArray packet;

    //第一步：固定报头-------------------------------------------------------
    //报文类型
    CmqByte fixHeader = static_cast<char>(msg.packetType()) << 4;
    if(msg.duplicated()){
        fixHeader |= MQTT_CONTROL_PACKET_FLAG_DUP;
    }
    else{
        fixHeader |= 0x00;
    }

    //报文标志
    if(msg.qos() == QOS::AT_LEAST_ONCE){
        fixHeader |= MQTT_CONTROL_PACKET_FLAG_QOS_L;
    }
    else if(msg.qos() == QOS::EXACTLY_ONCE){
        fixHeader |= MQTT_CONTROL_PACKET_FLAG_QOS_H;
    }
    packet.push_back(fixHeader);

    //剩余长度
    bool hasIden = CmqMqttEncoder::hasIdentifier(msg);
    short remainLength = msg.data().size() + (hasIden ? 2 : 0);
    auto bytes = CmqMqttEncoder::lengthEncode(remainLength);
    packet.insert(packet.end(), bytes.begin(), bytes.end());

    //第二步：可变报头(报文标识符)-----------------------------------------------
    if(hasIden){
        packet.push_back(msg.packetIdentifier() & 0xFF00 >> 8);
        packet.push_back(msg.packetIdentifier() & 0x0F);
    }

    //第三步： 有效载荷----------------------------------------------------------
    if(CmqMqttEncoder::hasPlayload(msg)){
        packet.insert(packet.end(), msg.data().begin(), msg.data().end());
    }

    return packet;
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
bool CmqMqttEncoder::hasIdentifier(const CmqMessage& msg)
{
    auto packetType = static_cast<short>(msg.packetType());
    auto pub = static_cast<short>(MqttPacketType::PUBLISH);
    auto unsuback = static_cast<short>(MqttPacketType::UNSUBACK);
    if((packetType == pub && static_cast<short>(msg.qos()) > 0) || (packetType > pub && packetType <= unsuback)){
        return true;
    }

    return false;
}

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
bool CmqMqttEncoder::hasPlayload(const CmqMessage& msg)
{
    auto packetType = msg.packetType();
    if(MqttPacketType::PUBLISH == packetType || MqttPacketType::CONNECT == packetType || MqttPacketType::SUBSCRIBE == packetType
    || MqttPacketType::UNSUBSCRIBE == packetType || MqttPacketType::SUBACK == packetType){
        return true;
    }

    return false;
}

/**
 * @description: 
 * @param {int} length
 * @return {*}
 */
CmqByteArray CmqMqttEncoder::lengthEncode(int length)
{
    int x = length;
    CmqByteArray lengthBytes;
    do{
        auto byte = x % 128;
        x = x / 128;
        if(x > 0){
            byte = byte | 128;
        }
        lengthBytes.push_back(byte);
    }while(x > 0);
    return lengthBytes;
}
}