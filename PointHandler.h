/*
 * PointHandler.h
 *
 *  Created on: Sep 12, 2013
 *      Author: nut
 */

#ifndef POINTHANDLER_H_
#define POINTHANDLER_H_
#include <iostream>
#include "point.h"
#include <cmath>
#include <list>
#include "line.h"

void initializePoints(int row_col,Line *segs[],Point *pointList[]);
void printVisibilityOfPoints(int numOfPoints,Point *pointList[]);
Point * searchPoint(int numOfPoints,Point *pointList[],Point* p );
double distance( Point * a, Point * b );
Point * getPointById(Point *pointList[],int id,int numOfPoints);
void createSourceAndDestinationPoint(int index,Point *pointList[],double s_x1,double s_y1,double d_x2,double d_y2);


#endif /* POINTHANDLER_H_ */
