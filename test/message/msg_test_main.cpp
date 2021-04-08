/*
 * @Author: your name
 * @Date: 2021-03-30 14:18:57
 * @LastEditTime: 2021-04-08 09:10:13
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CMQ\test\message\test_main.cpp
 */

#include <iostream>
#include "../../include/message/cmq_message.h"
using namespace std;

void printMsgInfo(const CMQ::Message::CmqMessage& msg)
{
    cout<<"id: "<<msg.id().toStdString()<<endl;
    cout<<"topic: "<<msg.topic().toStdString()<<endl;
    cout<<"timestamp: "<<QDateTime::fromMSecsSinceEpoch(msg.timeStamp()).toString().toStdString()<<endl;
    cout<<"key: "<<msg.key().toStdString()<<endl;
    cout<<"tag: "<<msg.tag().toStdString()<<endl;
    cout<<"qos: "<<(msg.qos() == CMQ::Message::QOS::AT_MOST_ONCE ? "AT_MOST_ONCE" : "OTHER")<<endl;
    cout<<"data: "<<std::string(msg.data().data())<<endl;
}

int main(int argc, char** argv)
{
    // QString topic("test topic");
    // QString id("testmsgid");
    // auto msg = CMQ::Message::CmqMessage(id, topic);
    // vector<char> data{'t', 'e', 's', 't'};
    // msg.resetData(data);
    // cout<<"Message created--------------"<<endl;
    // printMsgInfo(msg);

    // cout<<"---------------------------------------------------"<<endl;
    // msg.setMsgid("new id");
    // msg.setTopic("new topic");
    // msg.setQos(CMQ::Message::QOS::AT_LEAST_ONCE);
    // msg.setKey("new key");
    // msg.setTag("new tag");
    // vector<char> newData{'a', 'b', 'c', 'd'};
    // msg.appendData(newData);
    // cout<<"msg after modified-----------------------------------"<<endl;
    // printMsgInfo(msg);

    return 0;
}