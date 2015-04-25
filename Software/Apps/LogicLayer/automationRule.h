/* 
 * File:   automationRule.h
 * Author: Ali
 *
 * Created on 09 April 2015, 11:59
 */

#ifndef AUTOMATIONRULE_H
#define	AUTOMATIONRULE_H
#include <vector>

struct automationRule
{
char topic[20];
char payload[4];
char conditions[100][50];
int conditionCount;
int id;
bool isTemporary;
int TypeId;
};

#endif	/* AUTOMATIONRULE_H */
