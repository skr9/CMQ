/*
 * @Author: your name
 * @Date: 2021-04-10 11:18:48
 * @LastEditTime: 2021-04-11 08:02:02
 * @LastEditors: Please set LastEditors
 * @Description: 
 * CmqMessage 单元测试文件，主要测试了构造函数，赋值操作以及数据相关操作函数
 * @FilePath: /CMQ/test/message/cmqmessage_test.cpp
 */

#include "gtest/gtest.h"
#include "../../include/message/cmq_message.h"
using namespace CMQ::Message;
#include <iostream>
#include <string>

#define TEST_SUIT_NAME CmqMessageTest

/**
 * @description: 
 * 不带数据的构造函数测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, CtorTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    CmqMessage msg(msgid, topic, packetIdentifier);
    EXPECT_EQ(msgid, msg.id());
    EXPECT_EQ(topic, msg.topic());
    EXPECT_EQ(packetIdentifier, msg.packetIdentifier());
    EXPECT_EQ(MqttPacketType::PUBLISH, msg.packetType());
    EXPECT_EQ(Protocal::MQTT, msg.protocal());
    EXPECT_EQ(DEFAULT_KEY, msg.key());
    EXPECT_EQ(DEFAULT_TAG, msg.tag());
    EXPECT_EQ(QOS::AT_MOST_ONCE, msg.qos());
    EXPECT_EQ(MessageSource::CLIENT, msg.source());
    EXPECT_EQ(false, msg.duplicated());
    EXPECT_EQ(0, msg.data().size());
}

/**
 * @description: 
 * 带有初始化数据的构造函数测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, CtorWithDataTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    vector<char> data{'t', 'e', 's', 't'};
    CmqMessage msg(msgid, topic, packetIdentifier, data);
    EXPECT_EQ(msgid, msg.id());
    EXPECT_EQ(topic, msg.topic());
    EXPECT_EQ(packetIdentifier, msg.packetIdentifier());
    EXPECT_EQ(MqttPacketType::PUBLISH, msg.packetType());
    EXPECT_EQ(Protocal::MQTT, msg.protocal());
    EXPECT_EQ(DEFAULT_KEY, msg.key());
    EXPECT_EQ(DEFAULT_TAG, msg.tag());
    EXPECT_EQ(QOS::AT_MOST_ONCE, msg.qos());
    EXPECT_EQ(MessageSource::CLIENT, msg.source());
    EXPECT_EQ(false, msg.duplicated());
    EXPECT_EQ(data.size(), msg.data().size());
    std::cout<<"data:"<<std::string(msg.data().data())<<endl;
}

/**
 * @description: 
 * 移动拷贝测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, moveCtorTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    vector<char> data{'t', 'e', 's', 't'};
    CmqMessage temp(msgid, topic, packetIdentifier, data);
    CmqMessage msg(std::move(temp));
    EXPECT_EQ(msgid, msg.id());
    EXPECT_EQ(topic, msg.topic());
    EXPECT_EQ(packetIdentifier, msg.packetIdentifier());
    EXPECT_EQ(MqttPacketType::PUBLISH, msg.packetType());
    EXPECT_EQ(Protocal::MQTT, msg.protocal());
    EXPECT_EQ(DEFAULT_KEY, msg.key());
    EXPECT_EQ(DEFAULT_TAG, msg.tag());
    EXPECT_EQ(QOS::AT_MOST_ONCE, msg.qos());
    EXPECT_EQ(MessageSource::CLIENT, msg.source());
    EXPECT_EQ(false, msg.duplicated());
    EXPECT_EQ(data.size(), msg.data().size());
    std::cout<<"data:"<<std::string(msg.data().data())<<endl;
}

/**
 * @description: 
 * 拷贝构造函数测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, copyCtorTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    vector<char> data{'t', 'e', 's', 't'};
    CmqMessage temp(msgid, topic, packetIdentifier, data);
    cout<<"temp message created...."<<endl;
    CmqMessage msg(temp);
    cout<<"message copied-----"<<endl;
    EXPECT_EQ(msgid, msg.id());
    EXPECT_EQ(topic, msg.topic());
    EXPECT_EQ(packetIdentifier, msg.packetIdentifier());
    EXPECT_EQ(MqttPacketType::PUBLISH, msg.packetType());
    EXPECT_EQ(Protocal::MQTT, msg.protocal());
    EXPECT_EQ(DEFAULT_KEY, msg.key());
    EXPECT_EQ(DEFAULT_TAG, msg.tag());
    EXPECT_EQ(QOS::AT_MOST_ONCE, msg.qos());
    EXPECT_EQ(MessageSource::CLIENT, msg.source());
    EXPECT_EQ(false, msg.duplicated());
    EXPECT_EQ(data.size(), msg.data().size());
    std::cout<<"data:"<<std::string(msg.data().data())<<endl;
}

/**
 * @description: 
 * 赋值操作符测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, assignTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    vector<char> data{'t', 'e', 's', 't'};
    CmqMessage temp(msgid, topic, packetIdentifier, data);
    CmqMessage msg("", "", 0);
    cout<<"before assign--------------"<<endl;
    msg = temp;
    cout<<"after assgin ---------------"<<endl;
    EXPECT_EQ(msgid, msg.id());
    EXPECT_EQ(topic, msg.topic());
    EXPECT_EQ(packetIdentifier, msg.packetIdentifier());
    EXPECT_EQ(MqttPacketType::PUBLISH, msg.packetType());
    EXPECT_EQ(Protocal::MQTT, msg.protocal());
    EXPECT_EQ(DEFAULT_KEY, msg.key());
    EXPECT_EQ(DEFAULT_TAG, msg.tag());
    EXPECT_EQ(QOS::AT_MOST_ONCE, msg.qos());
    EXPECT_EQ(MessageSource::CLIENT, msg.source());
    EXPECT_EQ(false, msg.duplicated());
    EXPECT_EQ(data.size(), msg.data().size());
    std::cout<<"data:"<<std::string(msg.data().data())<<endl;
}

/**
 * @description: 
 * 数据操作相关接口测试
 * @param {*}
 * @return {*}
 */
TEST(TEST_SUIT_NAME, dataOperationTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    CmqMessage msg(msgid, topic, packetIdentifier);
    vector<char> buff{'t', 'e', 's', 't', 'd', 'a', 't', 'a'};
    msg.appendData(buff);
    EXPECT_EQ(buff, msg.data());
    msg.appendData('v');
    std::cout<<"data after append v:"<<std::string(msg.data().data())<<endl;
    EXPECT_EQ(buff.size() + 1, msg.data().size());
    msg.resetData(buff);
    EXPECT_EQ(buff, msg.data());
    msg.clearData();
    EXPECT_EQ(0, msg.data().size());
}

