/*
 * @Author: your name
 * @Date: 2021-04-27 12:39:36
 * @LastEditTime: 2021-05-20 00:54:24
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/message/cmq_encoder_factory.h
 */

#ifndef CMQ_MQTT_PACKET_ENCODER_H
#define CMQ_MQTT_PACKET_ENCODER_H

//MQTT 报文结构
/************************************************************
 * Bit          7  6  5  4   |   3  2  1  0                 *
 * ---------------------------------------------------------*
 * byte1        控制报文类型    控制报文标志位     | 固定     *
 * byte2...      剩      余     长         度    | 报头      *
 * -----------------------------------------------------    *
 * byte1...       可     变    报     头                     *
 * -----------------------------------------------------    *
 * byte1...     有    效    载    荷(PayLoad)              *
 * 
 * 其中，根据报文类型不同，固定报头、可变报头以及有效载荷
 * 的格式/内容各有不同，详细请参见：third_party/mqtt-doc
 ************************************************************/

#include "cmq_encoder.h"

namespace CMQ::Message{
//mqtt报文编码器抽象基类
class CmqMqttPacketEncoder{
public:
    virtual CmqByteArray generateFixHeader(const CmqMqttMessage& msg) = 0;
    virtual CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) = 0;
    virtual CmqByteArray generatePayload(const CmqMqttMessage& msg) = 0;

protected:
    /**
     * @description: 
     * 将剩余报文长度按照剩余长度计算规则编码成字节组
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
     * @param {int} length
     * @return {*}
     */
    CmqByteArray remainingLengthEncode(int length)
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

    CmqByteArray int2Bytes(int value)
    {
        CmqByteArray bytes;
        bytes.push_back(qint8(value>>24));
        bytes.push_back(qint8(value>>16));
        bytes.push_back(qint8(value>>8));
        bytes.push_back(qint8(value));
        return bytes;
    }
};

//Connect报文编码器类
class CmqConnectPacketEncoder : public CmqMqttPacketEncoder{
public:
    /**
     * @description: 
     * create connect packet fix header: packet type + remaining length
     * @param {const} CmqMqttMessage
     * @return {*}
     */
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override
    {
        int remainingLength = 10 + propertiesLength(msg) + msg.data().size();
        CmqByteArray header;
        header.push_back(0x10);  //packet type 0x01 + reserved 0x00
        auto remainginLenghtBytes = remainingLengthEncode(remainingLength);
        header.insert(header.end(), remainginLenghtBytes.begin(), remainginLenghtBytes.end());
        return header;
    }
    
    /**
     * @description: 
     * 编码connect报文的可变报头，前10个字节为固定内容，之后根据需要后接各种属性
     * @param {const} CmqMqttMessage
     * @return {*}
     */
    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override
    {
        CmqByteArray header;

        //protocal name
        header.push_back(0x00); //Length MSB(0)
        header.push_back(0x04); //Length LSB(4)
        header.push_back('M');
        header.push_back('Q');
        header.push_back('T');
        header.push_back('T');

        //protocal version: current version 5.0 (0x05)
        header.push_back(0x05);

        //connect flags
        header.push_back(msg.connectField()->connectFlag);

        //keep live interval
        header.push_back(msg.commonField()->keepLiveInterval & 0xFF00 >> 8); //keep live interval MSB
        header.push_back(msg.commonField()->keepLiveInterval & 0x00FF); //keep live interval LSB

        //property length
        auto lenBytes = convertLen2Bytes(propertiesLength(msg));
        header.insert(header.end(), lenBytes.begin(), lenBytes.end());

        //session expiry interval
        if(isMsgContainsSessionExpiryIntervalProperty(msg)){
            header.push_back(SESSION_EXPIRY_INTERVAL_IDENTIFIER);
            auto bytes = int2Bytes(msg.commonField()->sessionExpiryInterval);
            header.insert(header.end(), bytes.begin(), bytes.end());
        }

        //receive maximum
        if(isMsgContainsReceiveMaximumProperty(msg)){
            header.push_back(RECEIVE_MAXIMUM_IDENTIFIER);
            short rmax = msg.commonField()->receiveMaximum;
            header.push_back(qint8(rmax>>8));
            header.push_back(qint8(rmax));
        }

        //maximum packet size
        if(isMsgContainsMaximumPacketSizeProperty(msg)){
            header.push_back(MAXIMUM_PACKET_SIZE_IDENTIFIER);
            auto mpzBytes = int2Bytes(msg.commonField()->maximumPacketSize);
            header.insert(header.end(), mpzBytes.begin(), mpzBytes.end());
        }

        //topic alias maximum
        if(isMsgContainsTopicAliasMaximumProperty(msg)){
            header.push_back(TOPIC_ALIAS_MAXIMUM_IDENTIFIER);
            short topicAliasMax = msg.commonField()->topicAliasMaximum;
            header.push_back(qint8(topicAliasMax >> 8));
            header.push_back(qint8(topicAliasMax));
        }

        //request respone info
        if(isMsgContainsRequestResponeInfoProperty(msg)){
            header.push_back(REQUEST_RESPONSE_INFO_IDENTIFIER);
            header.push_back(msg.connectField()->requestResponseInfo);
        }

        //request problem info
        if(isMsgContainsRequestProblemInfoProperty(msg)){
            header.push_back(REQUEST_PROBLEM_INFO_IDENTIFIER);
            header.push_back(msg.connectField()->requestProblemInfo);
        }

        //user property
        if(!msg.commonField()->userProperty.isEmpty()){
            for(auto userProperty : msg.commonField()->userProperty){
                header.push_back(USER_PROPERTY_IDENTIFIER);
                auto bytes = userProperty.toLocal8Bit();
                header.insert(header.end(), bytes.begin(), bytes.end());
            }
        }

        //authentication method
        if(!msg.commonField()->authenticationMethod.isEmpty()){
            header.push_back(AUTHENTICATION_METHOD_IDENTIFIER);
            auto bytes = msg.commonField()->authenticationMethod.toLocal8Bit();
            header.insert(header.end(), bytes.begin(), bytes.end());
        }

        //authentication data
        if(msg.commonField()->authenticationData.size() > 0){
            header.push_back(AUTHENTICATION_DATA_IDENTIFIER);
            auto bytes = msg.commonField()->authenticationData;
            header.insert(header.end(), bytes.begin(), bytes.end());
        }

        return header;
    }

    /**
     * @description: 
     * 编码connect报文的有效载荷（payload）部分
     * @param {const} CmqMqttMessage
     * @return {*}
     */
    CmqByteArray generatePayload(const CmqMqttMessage& msg) override
    {
        CmqByteArray payload;

        //client identifier(ClientID), connect packet MUST contain client id
        qint8 clientIdLen = msg.commonField()->clientIdentifier.length();
        payload.push_back(clientIdLen);
        auto clientId = msg.commonField()->clientIdentifier.toLocal8Bit();
        payload.insert(payload.end(), clientId.begin(), clientId.end());

        //will
        if(msg.connectField()->hasWillMessage()){
            //will properties
            //properties length
            qint64 len = 5 + 2 + 5 + 1 + msg.commonField()->contentType.length()
                       + 1 + msg.commonField()->responseTopic.length()
                       + 1 + msg.commonField()->correlationData.size()
                       + 1 + msg.commonField()->userProperty.join("").size();
            auto bytes = convertLen2Bytes(len);
            payload.insert(payload.end(), bytes.begin(), bytes.end());

            payload.push_back(WILL_DELAY_INTERVAL_IDENTIFIER); //will delay interval
            auto intervalBytes = int2Bytes(msg.connectField()->willDelayInterval);
            payload.insert(payload.end(), intervalBytes.begin(), intervalBytes.end());
            payload.push_back(PAYLOAD_FORMAT_INDICATOR_IDENTIFIER); //payload format indicator
            payload.push_back(msg.commonField()->playloadFormatIndicator > 0 ? 1 : 0);
            payload.push_back(MESSAGE_EXPIRY_INTERVAL_IDENTIFIER); //message expiry interval
            auto meiBytes = int2Bytes(msg.commonField()->messageExpiryInterval);
            payload.insert(payload.end(), meiBytes.begin(), meiBytes.end());
            payload.push_back(CONTENT_TYPE_IDENTIFIER); //content type
            auto ctBytes = msg.commonField()->contentType.toLocal8Bit();
            payload.insert(payload.end(), ctBytes.begin(), ctBytes.end());
            payload.push_back(RESPONSE_TOPIC_IDENTIFIER); //Response topic
            auto rtBytes = msg.commonField()->responseTopic.toLocal8Bit();
            payload.insert(payload.end(), rtBytes.begin(), rtBytes.end());
            payload.push_back(CORRELATION_DATA_IDENTIFIER); //correlation data 
            auto cdBytes = msg.commonField()->correlationData;
            payload.insert(payload.end(), cdBytes.begin(), cdBytes.end());
            payload.push_back(USER_PROPERTY_IDENTIFIER); //user property
            auto upBytes = msg.commonField()->userProperty.join("").toLocal8Bit();
            payload.insert(payload.end(), upBytes.begin(), upBytes.end());

            //will topic
            int topicLen = msg.connectField()->willTopic.length();
            auto tlBytes = int2Bytes(topicLen);
            payload.insert(payload.end(), tlBytes.begin(), tlBytes.end());
            auto bytes = msg.connectField()->willTopic.toLocal8Bit();
            payload.insert(payload.end(), bytes.begin(), bytes.end());

            //will payload
            int wpLen = msg.connectField()->willPlayload.size();
            auto wlBytes = int2Bytes(wpLen);
            payload.insert(payload.end(), wlBytes.begin(), wlBytes.end());
            auto wpBytes = msg.connectField()->willPlayload;
            payload.insert(payload.end(), wpBytes.begin(), wpBytes.end());
        }

        //user name
        if(msg.connectField()->hasUserName()){
            int unLen = msg.connectField()->userName.length();
            auto ulBytes = int2Bytes(unLen);
            payload.insert(payload.end(), ulBytes.begin(), ulBytes.end());
            auto unBytes = msg.connectField()->userName.toLocal8Bit();
            payload.insert(payload.end(), unBytes.begin(), unBytes.end());
        }

        //password
        if(msg.connectField()->hasPassword()){
            int pwLen = msg.connectField()->password.length();
            auto plBytes = int2Bytes(pwLen);
            payload.insert(payload.end(), plBytes.begin(), plBytes.end());
            auto pwByte = msg.connectField()->password.toLocal8Bit();
            payload.insert(payload.end(), pwByte.begin(), pwByte.end());
        }
    }

protected:
    inline bool isMsgContainsSessionExpiryIntervalProperty(const CmqMqttMessage& msg){return msg.commonField()->sessionExpiryInterval > 0;}
    inline bool isMsgContainsReceiveMaximumProperty(const CmqMqttMessage& msg){return msg.commonField()->receiveMaximum > 0;}
    inline bool isMsgContainsMaximumPacketSizeProperty(const CmqMqttMessage& msg){return msg.commonField()->maximumPacketSize > 0;}
    inline bool isMsgContainsTopicAliasMaximumProperty(const CmqMqttMessage& msg){return msg.commonField()->topicAliasMaximum > 0;}
    inline bool isMsgContainsRequestResponeInfoProperty(const CmqMqttMessage& msg){return msg.connectField()->requestResponseInfo == 1;}
    inline bool isMsgContainsRequestProblemInfoProperty(const CmqMqttMessage& msg){return msg.connectField()->requestProblemInfo == 0;}

    /**
     * @description: 
     * 计算connect报文中包含的所有属性字节长度
     * @param {const} CmqMqttMessage
     * @return {*}
     */
    virtual qint64 propertiesLength(const CmqMqttMessage& msg)
    {
        quint64 len = 0;
        len += isMsgContainsSessionExpiryIntervalProperty(msg) ? 5 : 0; //identifier byte and four byte session expiry interval
        len += isMsgContainsReceiveMaximumProperty(msg) ? 3 : 0; //identifier byte and two byte receive maximum
        len += isMsgContainsMaximumPacketSizeProperty(msg) ? 5 : 0; //identifier byte and four byte maximum packet size
        len += isMsgContainsTopicAliasMaximumProperty(msg) ? 3 : 0; //identifier byte and 2 byte topic alias maximum
        len += isMsgContainsRequestResponeInfoProperty(msg) ? 2 : 0; //identifier byte and 1 byte request responese info
        len += isMsgContainsRequestProblemInfoProperty(msg) ? 2 : 0; //identifier byte and 1 byte request problem info

        //user properties length: 1 byte identifier  + length of user property string
        auto userProperty = msg.commonField()->userProperty;
        len += (userProperty.isEmpty() ? 0 : (userProperty.count() + userProperty.join("").length()));
        
        //authen method length: 1 byte identifier + length of authen method string
        auto authenMethod = msg.commonField()->authenticationMethod;
        len += (authenMethod.isEmpty() ? 0 : 1 + authenMethod.length());

        //authen data length: 1 byte identifier + length of authen data byte array
        if(msg.commonField()->authenticationData.size() > 0){
            len += (1 + msg.commonField()->authenticationData.size());
        }

        return len;
    }

    /**
     * @description: 
     * 将属性字节长度值转换为字节组
     * @param {qint64} len
     * @return {*}
     */
    CmqByteArray convertLen2Bytes(qint64 len)
    {
        //convert propeties length to bytes
        CmqByteArray bytes;
        CmqByteArray buffer;
        for(short shiftBytes = 8; shiftBytes < 8; ++shiftBytes){
            qint8 byte = len >> (shiftBytes * 8); //get lowest byte value
            buffer.push_back(byte); 

            //if this byte is not none,then push back all buffered bytes
            if(byte & 0xFF){
                bytes.insert(bytes.end(), buffer.begin(), buffer.end()); 
                buffer.clear();
            }
        }
        
        return bytes;
    }
};

//Connack报文编码器
class CmqConnackPacketEncoder final : public CmqConnectPacketEncoder{
public:
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override
    {
        CmqByteArray fixHeader;
        fixHeader.push_back(0b0010'0000);
        auto propertiesLen = propertiesLength(msg);
        int remaingLength = 2 + convertLen2Bytes(propertiesLen).size() + propertiesLen;
        auto bytes = remainingLengthEncode(remaingLength);
        fixHeader.insert(fixHeader.end(), bytes.begin(), bytes.end());
        return fixHeader;
    }

    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override
    {
        CmqByteArray variableHeader;

        //session present flag
        variableHeader.push_back(msg.connackField()->sessionPresent);

        //connect reason code
        variableHeader.push_back(msg.connackField()->connectReasonCode);

        //connack properties
        //properties length
        auto lenBytes = convertLen2Bytes(propertiesLength(msg));
        variableHeader.insert(variableHeader.end(), lenBytes.begin(), lenBytes.end());
        //session expiry interval
        if(isMsgContainsSessionExpiryIntervalProperty(msg)){
            variableHeader.push_back(SESSION_EXPIRY_INTERVAL_IDENTIFIER);
            auto seiBytes = int2Bytes(msg.commonField()->sessionExpiryInterval);
            variableHeader.insert(variableHeader.end(), seiBytes.begin(), seiBytes.end());
        }
        //receive maximum
        if(isMsgContainsReceiveMaximumProperty(msg)){
            variableHeader.push_back(RECEIVE_MAXIMUM_IDENTIFIER);
            auto recvMax = msg.commonField()->receiveMaximum;
            variableHeader.push_back(qint8(recvMax >> 8));
            variableHeader.push_back(qint8(recvMax));
        }
        //maximum qos
        if(isMsgContainsMaxQosProperty(msg)){
            variableHeader.push_back(MAXIMUM_QOS_IDENTIFIER);
            variableHeader.push_back(msg.commonField()->maxQos);
        }
        //retain available
        if(isMsgContainsRetainAvailableProperty(msg)){
            variableHeader.push_back(RETAIN_AVAILABLE_IDENTIFIER);
            variableHeader.push_back(msg.connackField()->retainAvailable);
        }
        //maximum packet size
        if(isMsgContainsMaximumPacketSizeProperty(msg)){
            variableHeader.push_back(MAXIMUM_PACKET_SIZE_IDENTIFIER);
            auto mpsBytes = int2Bytes(msg.commonField()->maximumPacketSize);
            variableHeader.insert(variableHeader.end(), mpsBytes.begin(), mpsBytes.end());
        }
        //assigned client identifier
        auto assignedClientId = msg.connackField()->assignedClientIdentifier;
        if(!assignedClientId.isEmpty()){
            variableHeader.push_back(ASSIGNED_CLIENT_IDENTIFIER);
            auto aciBytes = assignedClientId.toLocal8Bit();
            variableHeader.insert(variableHeader.end(), aciBytes.begin(), aciBytes.end());
        }
        //topic alias maximum
        if(isMsgContainsTopicAliasMaximumProperty(msg)){
            variableHeader.push_back(TOPIC_ALIAS_MAXIMUM_IDENTIFIER);
            variableHeader.push_back(qint8(msg.commonField()->topicAliasMaximum >> 8));
            variableHeader.push_back(qint8(msg.commonField()->topicAliasMaximum));
        }
        //reason string
        if(!msg.commonField()->reasonString.isEmpty()){
            variableHeader.push_back(REASON_STRING_IDENTIFIER);
            auto reasonBytes = msg.commonField()->reasonString.toLocal8Bit();
            variableHeader.insert(variableHeader.end(), reasonBytes.begin(), reasonBytes.end());
        }
        //user property
        if(!msg.commonField()->userProperty.isEmpty()){
            variableHeader.push_back(USER_PROPERTY_IDENTIFIER);
            auto userBytes = msg.commonField()->userProperty.join("").toLocal8Bit();
            variableHeader.insert(variableHeader.end(), userBytes.begin(), userBytes.end());
        }
        //wildcard subsciption available
        if(isMsgContainsWildcardProperty(msg)){
            variableHeader.push_back(WILDCARD_SUBSCRIPTION_AVAILABLE_IDENTIFIER);
            variableHeader.push_back(msg.connackField()->wildcardSubscriptionAvailable > 0 ? 1 : 0);
        }
        //subscription identifer available
        if(isMsgContainsSubscriptionIdentifiersProperty(msg)){
            variableHeader.push_back(SUBSCRIPTION_ID_AVAILABLE_IDENTIFIER);
            variableHeader.push_back(msg.connackField()->subscriptionIdentifiersAvailable > 0 ? 1 : 0);
        }
        //server keep alive
        if(msg.connackField()->serverKeepAlive > 0){
            variableHeader.push_back(SERVER_KEEP_ALIVE_IDENTIFER);
            auto interval = msg.connackField()->serverKeepAlive;
            variableHeader.push_back(qint8(interval >> 8));
            variableHeader.push_back(qint8(interval));
        }
        //response information
        auto responseInfo = msg.connackField()->responseInfo;
        if(!responseInfo.isEmpty()){
            variableHeader.push_back(RESPONSE_INFORMATION_IDENTIFIER);
            auto riBytes = responseInfo.toLocal8Bit();
            variableHeader.insert(variableHeader.end(), riBytes.begin(), riBytes.end());
        }
        //server reference
        auto serverRef = msg.connackField()->serverReference;
        if(!serverRef.isEmpty()){
            variableHeader.push_back(SERVER_REFERENCE_IDENTIFIER);
            auto srBytes = serverRef.toLocal8Bit();
            variableHeader.insert(variableHeader.end(), srBytes.begin(), srBytes.end());
        }
        //authentication method
        auto method = msg.commonField()->authenticationMethod;
        if(!method.isEmpty()){
            variableHeader.push_back(AUTHENTICATION_METHOD_IDENTIFIER);
            auto methodBytes = method.toLocal8Bit();
            variableHeader.insert(variableHeader.end(), methodBytes.begin(), methodBytes.end());
        }
        //authentication data
        auto authenData = msg.commonField()->authenticationData;
        if(authenData.size() > 0){
            variableHeader.push_back(AUTHENTICATION_DATA_IDENTIFIER);
            variableHeader.insert(variableHeader.end(), authenData.begin(), authenData.end());
        }
    }

protected:
    inline bool isMsgContainsMaxQosProperty(const CmqMqttMessage& msg)
    {
        qint8 maxQos = msg.commonField()->maxQos;
        if(0 == maxQos || 1 == maxQos){
            return true;
        }

        return  false;
    }

    inline bool isMsgContainsRetainAvailableProperty(const CmqMqttMessage& msg)
    {
        qint8 retain = msg.connackField()->retainAvailable;
        if(0 == retain || 1 == retain){
            return true;
        }

        return false;
    }

    inline bool isMsgContainsWildcardProperty(const CmqMqttMessage& msg)
    {
        qint8 wildcard = msg.connackField()->wildcardSubscriptionAvailable;
        if(0 == wildcard || 1 == wildcard){
            return true;
        }

        return false;
    }

    inline bool isMsgContainsSubscriptionIdentifiersProperty(const CmqMqttMessage& msg)
    {
        qint8 sia = msg.connackField()->subscriptionIdentifiersAvailable;
        if(0 == sia || 1 == sia){
            return true;
        }

        return false;
    }

    inline bool isMsgContainsSharedSubscriptionProperty(const CmqMqttMessage& msg)
    {
        qint8 ssa = msg.connackField()->sharedSubscriptionAvailable;
        if(0 == ssa || 1 == ssa){
            return true;
        }

        return false;
    }
    
    qint64 propertiesLength(const CmqMqttMessage& msg) override
    {
        quint64 len = 0;
        len += isMsgContainsSessionExpiryIntervalProperty(msg) ? 5 : 0; //identifier byte and four byte session expiry interval
        len += isMsgContainsReceiveMaximumProperty(msg) ? 3 : 0; //identifier byte and two byte receive maximum
        len += isMsgContainsMaxQosProperty(msg) ? 2 : 0;
        len += isMsgContainsRetainAvailableProperty(msg) ? 2 : 0;
        len += isMsgContainsMaximumPacketSizeProperty(msg) ? 5 : 0; //identifier byte and four byte maximum packet size
        len += isMsgContainsTopicAliasMaximumProperty(msg) ? 3 : 0; //identifier byte and 2 byte topic alias maximum
        len += isMsgContainsRequestResponeInfoProperty(msg) ? 2 : 0; //identifier byte and 1 byte request responese info

        //assigned client identifier length: 1 + string length
        auto assignedClientId = msg.connackField()->assignedClientIdentifier;
        len += assignedClientId.isEmpty() ? 0 : 1 + assignedClientId.length();

        //reason string length: 1 + string length
        auto reasonString = msg.commonField()->reasonString;
        len += reasonString.isEmpty() ? 0 : 1 + reasonString.length();

        //user properties length: 1 byte identifier  + length of user property string
        auto userProperty = msg.commonField()->userProperty;
        len += (userProperty.isEmpty() ? 0 : (userProperty.count() + userProperty.join("").length()));
        
        //wildcard subscription available
        len += isMsgContainsWildcardProperty(msg) ? 2 : 0;

        //subscription identifiers available property length
        len += isMsgContainsSubscriptionIdentifiersProperty(msg) ? 2 : 0;

        //shared subscription available
        len += isMsgContainsSharedSubscriptionProperty(msg) ? 2 : 0;

        //server keep alive
        if(msg.connackField()->serverKeepAlive > 0){
            len += 3;
        }

        //server reference
        auto serverRef = msg.connackField()->serverReference;
        len += serverRef.isEmpty() ? 0 : 1 + serverRef.length();

        //authen method length: 1 byte identifier + length of authen method string
        auto authenMethod = msg.commonField()->authenticationMethod;
        len += (authenMethod.isEmpty() ? 0 : 1 + authenMethod.length());

        //authen data length: 1 byte identifier + length of authen data byte array
        if(msg.commonField()->authenticationData.size() > 0){
            len += (1 + msg.commonField()->authenticationData.size());
        }

        return len;
    }
};

//Publish报文编码器
class CmqPublishPacketEncoder : public CmqMqttPacketEncoder{
public:
    CmqByteArray generateFixHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generateVariableHeader(const CmqMqttMessage& msg) override;
    CmqByteArray generatePayload(const CmqMqttMessage& msg) override;
};

//Puback报文编码器
class CmqPubackPacketEncoder : public CmqMqttPacketEncoder{

};

//PUBREL报文编码器
class CmqPubrelPacketEncoder : public CmqPubackPacketEncoder{

};

//PUBREC报文编码器
class CmqPubrecPacketEncoder: public CmqPubackPacketEncoder{

};

//PUBCOM报文编码器
class CmqPubcompPacketEncoder : public CmqPubackPacketEncoder{

};

//Subscrible报文编码器
class CmqSubscriblePacketEncoder: public CmqMqttPacketEncoder{

};

//Suback报文编码器
class CmqSubackPacketEncoder : public CmqMqttPacketEncoder{

};

//Unsubscrible报文编码器
class CmqUnsubPacketEncoder : public CmqMqttPacketEncoder{

};

//Unsuback报文编码器
class CmqUnsubackPacketEncoder : public CmqMqttPacketEncoder{

};

//Disconnect报文编码器
class CmqDisconnectPacketEncoder : public CmqMqttPacketEncoder{

};

//PINGREQ报文编码器
class CmqPingreqPacketEncoder : public CmqMqttPacketEncoder{

};

//PINGRESP报文编码器
class CmqPingrespPacketEncoder : public CmqMqttPacketEncoder{

};

/**
 * @description: 
 * 一个简单工厂类，用以帮助调用方快速根据报文类型创建相应的报文编码器
 */
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