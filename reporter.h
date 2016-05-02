/* 
 * File: reporter.h
 * ------------------
 * This is where you declare any exported features of the libreporter
 * library. 
 */

#ifndef _REPORTER_H
#define _REPORTER_H


/* 
 * Function: InitReporter
 * ----------------------
 * Any program that wants to crash reporting from libreporter must
 * make a call to InitReporter once (preferably as first line of main())
 */
void InitReporter();

#endif
