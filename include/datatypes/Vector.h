#pragma once
#include <standard.h>

#define DefineVec2Operator(c, op)\
c operator op(const c &other)\
{ return c(this->X op other.X, this->Y op other.Y); }\

#define DefineVec2OperatortoNum(c, t,op)\
c operator op(const t &num)\
{ return c(this->X op num, this->Y op num); }\

#define DefineVec2Type(classname, type) \
class classname\
{\
public:\
    classname(): X(0), Y(0) {}\
    classname(const type X, const type Y): X(X), Y(Y) {}\
    type X, Y;\
    DefineVec2Operator(classname, +)\
    DefineVec2Operator(classname, -)\
    DefineVec2Operator(classname, *)\
    DefineVec2Operator(classname, /)\
    \
    inline double getMagnitude() {return sqrt(X*X + Y*Y);}\
    \
    DefineVec2OperatortoNum(classname, type, +)\
    DefineVec2OperatortoNum(classname, type, -)\
    DefineVec2OperatortoNum(classname, type, *)\
    DefineVec2OperatortoNum(classname, type, /)\
};\

DefineVec2Type(Vector2i, int)
DefineVec2Type(Vector2f, float)
DefineVec2Type(Vector2d, double)