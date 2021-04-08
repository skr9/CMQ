/*
 * @Author: your name
 * @Date: 2021-04-07 12:05:56
 * @LastEditTime: 2021-04-07 12:06:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CMQ/include/common/cmq_global.h
 */
#ifndef CMQ_GLOBAL_H_
#define CMQ_GLOBAL_H_

#include <vector>
#include <memory>

namespace CMQ{

typedef char CmqByte;
typedef std::vector<CmqByte> CmqByteArray;

#define CMQ_DECLARE_IMP(Class)\
    friend class Class##Imp;\
    std::unique_ptr<Class##Imp> _imp_ptr;


}

#endif