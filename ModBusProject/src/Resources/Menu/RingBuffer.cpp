/*
 * RingBuffer.cpp
 *
 *  Created on: 6 Mar 2020
 *      Author: Lauri
 */

#include "RingBuffer.h"

RingBuffer::RingBuffer() {
	for(int i = 0 ; i < bufSize; i++){
		items[i] = 0;
	}

}

RingBuffer::~RingBuffer() {
	// TODO Auto-generated destructor stub
}
void RingBuffer::add(int i){
	items[head] = i;
	if(++head == 20){
		head = 0;
	}
}
int RingBuffer::get(){
	if(tail != head){
		if(items[tail] != 0){
			int ret = items[tail];
			items[tail++] = 0;
			if(tail == 20){
				tail = 0;
			}
			return ret;
		}
	}
	return 0;
}


