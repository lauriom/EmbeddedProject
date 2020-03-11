/*
 * LimitedInt.cpp
 *
 *  Created on: 10 Mar 2020
 *      Author: Lauri
 */

#include "LimitedInt.h"

LimitedInt::LimitedInt(int val,int min,int max):val(val), min(min), max(max) {

}

LimitedInt::~LimitedInt() {
	// TODO Auto-generated destructor stub
}
void LimitedInt::setLims(int Min, int Max){
	min = Min;
	max = Max;
}

LimitedInt LimitedInt::operator++(){
	if ((val + 1) <= max){
		++val;
	}
	return *this;

}
LimitedInt LimitedInt::operator--(){
	if ((val - 1) >= min){
		--val;
	}
	return *this;
}
LimitedInt LimitedInt::operator++(int){
	LimitedInt buf = *this;
	this->operator ++();
	*this = buf;
	return buf;
}
LimitedInt LimitedInt::operator--(int){
	LimitedInt buf = *this;
	this->operator --();
	return buf;
}
LimitedInt::operator int() const{
	return val;
}
bool LimitedInt::operator==(int i){
	if(val == i){
		return true;
	}
	return false;
}

LimitedInt LimitedInt::operator=(int i){
	if(i >= min && i <= max){
		val = i;
	}else if(i > max){
		val = max;
	}else/* if(i < min)*/{
		val = min;
	}
	return *this;
}

LimitedInt LimitedInt::operator+=(int i){
	if((val + i) < max) {
		val += i;
	}
	else {
		val = max;
	}
	return *this;
}
LimitedInt LimitedInt::operator-=(int i){
	if((val - i) > min){
		val -= i;
	}else{
		val = min;
	}
	return *this;
}
int LimitedInt::operator+(int i){
	return val+i;
}
int LimitedInt::operator-(int i){
	return val-i;
}


