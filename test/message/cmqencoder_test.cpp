/*
 * @Author: your name
 * @Date: 2021-04-11 10:11:03
 * @LastEditTime: 2021-04-14 00:48:03
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/test/message/cmqencoder_test.cpp
 */

#include "cmqmessage_test_global.h"
#include "../../include/message/cmq_encoder.h"

TEST(TEST_SUIT_NAME, msgEncodeTest)
{
    QString msgid("testmsg");
    QString topic("testtopic");
    short packetIdentifier = 12345;
    vector<char> data;
    for(int i = 0; i < 10000; ++i){
        data.push_back('t');
    }
    CmqMessage msg(msgid, topic, packetIdentifier, data);
    msg.setQos(QOS::AT_LEAST_ONCE);
    CmqMqttEncoder encoder;
    auto bytes = encoder.msgEncode(msg);
    EXPECT_EQ(bytes.size(), 10005);
    cout<<"packet bytes:"<<bytes.size()<<endl;
    cout<<std::string(bytes.data())<<endl;
}