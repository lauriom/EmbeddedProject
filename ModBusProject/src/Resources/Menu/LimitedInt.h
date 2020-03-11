/*
 * LimitedInt.h
 *
 *  Created on: 10 Mar 2020
 *      Author: Lauri
 */

#ifndef SRC_RESOURCES_MENU_LIMITEDINT_H_
#define SRC_RESOURCES_MENU_LIMITEDINT_H_

class LimitedInt {
public:
	LimitedInt(int in,int min,int max);
	virtual ~LimitedInt();
	void setLims(int min, int max);

	LimitedInt operator++();
	LimitedInt operator--();
	LimitedInt operator++(int const);
	LimitedInt operator--(int const);
	operator int() const;
	bool operator ==(int const);
	LimitedInt operator=(int const);
	LimitedInt operator+=(int const);
	LimitedInt operator-=(int const);
	int operator-(int const);
	int operator+(int const);
private:
	int val;
	int min, max;
};

#endif /* SRC_RESOURCES_MENU_LIMITEDINT_H_ */
