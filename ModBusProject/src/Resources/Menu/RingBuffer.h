/*
 * RingBuffer.h
 *
 *  Created on: 6 Mar 2020
 *      Author: Lauri
 */


#ifndef SRC_RESOURCES_RINGBUFFER_H_
#define SRC_RESOURCES_RINGBUFFER_H_
#define bufSize 20
/*
 * Fixed size fifo ringBuffer, overwrites oldest event
 */
class RingBuffer{
public:
	RingBuffer();
	virtual ~RingBuffer();
	void add(int);
	int get();
private:
	int items[bufSize];
	int head = 0;
	int tail = 0;
};

#endif /* SRC_RESOURCES_RINGBUFFER_H_ */
