/**
*@file xcpTask.h
*@author Michael Wolf
*@date 21.06.2017
*@brief Interface between application business logic and XCP driver. Inits XCP driver and triggers XCP events in a loop.
*/

#ifndef XCPTASK_H_
#define XCPTASK_H_


/******************************************************************************/
/*------------------------ Function Prototypes--------------------------------*/
/******************************************************************************/
void xcpTaskInit(void);

void xcpTaskLoop();


#endif /* XCPTASK_H_ */
