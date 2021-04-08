/*
 * @Author: your name
 * @Date: 2021-03-30 09:42:31
 * @LastEditTime: 2021-03-31 10:11:35
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: \CMQ\common\ISerializable.hpp
 */
#ifndef ISERIALIZABLE_H_
#define ISERIALIZABLE_H_

#include <vector>
#include <iostream>
using namespace std;

namespace CMQ::Common{
class ISerializable{
public:
    virtual ~ISerializable() = default;

    virtual vector<char> serialize() = 0;
    virtual void deserialize(const vector<char>& data) = 0;
};
}

#endif