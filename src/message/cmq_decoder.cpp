/*
 * @Author: zhuyuejiang
 * @Date: 2021-04-14 00:59:19
 * @LastEditTime: 2021-04-17 05:57:59
 * @Description: 
 * 消息解码器文件，目前暂时只实现了mqtt协议格式消息的解码
 * mqtt协议格式详见消息编码器文件cmq_encoder.cpp
 * @FilePath: /CMQ/src/message/cmq_decoder.cpp
 */

#include "../../include/message/cmq_decoder.h"
#include "../../include/message/cmq_message_global.h"

namespace CMQ::Message{
/**
 * @description: 
 * mqtt协议格式消息解码，是mqtt消息编码器的反向操作：
 * 1. 第一步：读取固定报文头，确定报文类型，以及qos和是否重复标志
 * 2. 第二步：读取剩余长度
 * 3. 第三步：根据报文类型和qos级别，确定是否有报文标识符，如有，
 *            则解析报文标识符
 * 4. 第四步：剩余内容均为数据，读取
 * @param 
 * 按照mqtt协议格式编码后的消息字节数组
 * @return 
 * 解码后的消息
 */    
CmqMessage CmqMqttDecoder::msgDecode(const CmqByteArray& packet)
{
    Q_ASSERT(packet.size() >= 2);

    //第一步：读取固定报头，确定重发标志和QOS
    CmqByte fixHeader = packet.front();
    int packetType = fixHeader >> 4;
    union packTypeUion{
        int intVal;
        MqttPacketType enumType;
    };
    packTypeUion ptUnion;
    ptUnion.intVal = packetType;
    MqttPacketType pt = ptUnion.enumType;
    bool duplicated = (fixHeader & MQTT_CONTROL_PACKET_FLAG_DUP) > 0;
    QOS qos = QOS::AT_MOST_ONCE;
    if(fixHeader & MQTT_CONTROL_PACKET_FLAG_QOS_H > 0){
        qos = QOS::EXACTLY_ONCE;
    }
    else if(fixHeader & MQTT_CONTROL_PACKET_FLAG_QOS_L){
        qos = QOS::AT_LEAST_ONCE;
    }

    //第二步：解析剩余长度
    int currentPos = 1;
    int remainingBytesCount = CmqMqttDecoder::lengthCompute(packet, currentPos);

    //第三步：确定报文标识符
    short packetIdentifer = 0;
    if(packet.size() > (currentPos + remainingBytesCount) && hasIdentifier(pt, qos)){
        short hbyte = packet.at(currentPos);
        short lbyte = packet.at(currentPos + 1);
        packetIdentifer = (hbyte << 8) | lbyte;
        currentPos += 2;
        remainingBytesCount -= 2;
    }

    CmqMessage msg("", "", packetIdentifer, pt);

    //第四步：读取数据内容
    if(remainingBytesCount > 0){
        CmqByteArray data(packet.begin() + currentPos, packet.end());
        msg.appendData(data);
    }
}

/**
 * @description: 
 * @param {const} CmqByteArray
 * @param {int} lengthByteStartPosition
 * @return {*}
 */
int CmqMqttDecoder::lengthCompute(const CmqByteArray& packet, int& currentPosition, int lengthByteStartPosition)
{
    bool hasNext = false;
    int factor = 1;
    int length = 0;
    currentPosition = lengthByteStartPosition;
    do{
        if(packet.size() <= currentPosition){
            break;
        }

        auto byte = packet.at(currentPosition);
        
        length += byte % 128 * factor;
        hasNext = (byte > 127);
        factor *= (hasNext ? 128 : 1);
        ++currentPosition;
    }while(hasNext);

    return length;
}

bool CmqMqttDecoder::hasIdentifier(MqttPacketType packetType, QOS qos)
{
    short intTypeVal = static_cast<short>(packetType);
    if((MqttPacketType::PUBLISH == packetType && static_cast<short>(qos) > 0)
      || (intTypeVal >= static_cast<short>(MqttPacketType::PUBACK) && intTypeVal <= static_cast<short>(MqttPacketType::UNSUBACK))){
        return true;
    }

    return false;
}
}