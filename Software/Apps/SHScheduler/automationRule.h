/* 
 * File:   automationRule.h
 * Author: Ali
 *
 * Created on 09 April 2015, 11:59
 */

#ifndef AUTOMATIONRULE_H
#define	AUTOMATIONRULE_H
#include <vector>
// define structure to hold the rules when retrieved from the database.
struct automationRule
{
char topic[50];
char payload[4];
char conditions[100][50];
unsigned int conditionCount;
unsigned int id;
bool isTemporary;
};

#endif	/* AUTOMATIONRULE_H */
