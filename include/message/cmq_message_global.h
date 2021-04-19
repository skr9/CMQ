/*
 * @Author: your name
 * @Date: 2021-04-14 13:20:32
 * @LastEditTime: 2021-04-14 13:22:40
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_message_global.h
 */

#ifndef CMQ_MESSAGE_GLOBAL_H
#define CMQ_MESSAGE_GLOBAL_H

namespace CMQ::Message{
//控制报文标志位，以下标志只出现在PUBLISH报文中，从MQTT3.1.1起被使用
#define MQTT_CONTROL_PACKET_FLAG_DUP 0x08    //重复分发标志位
#define MQTT_CONTROL_PACKET_FLAG_QOS_H 0x04  //PUBLISH报文服务质量标志高位
#define MQTT_CONTROL_PACKET_FLAG_QOS_L 0x02  //PUBLISH报文服务质量标志低位
#define MQTT_CONTROL_PACKET_FLAG_RETAIN 0x01 //PUBLISH报文的保留标志
}

#endif