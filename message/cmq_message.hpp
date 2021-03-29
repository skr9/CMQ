
#ifndef CMQ_MESSAGE_H_
#define CMQ_MESSAGE_H_

#include <QtCore/QString>

namespace CMQ::Message{
class CmqMessage{
public:
    CmqMessage() = default;
    virtual ~CmqMessage() = 0;

private:
    qint64 _id;
    QString _key;
    QString _tag;
    QString _topic;
    qint64 _timeStamp;
};
}

#endif
