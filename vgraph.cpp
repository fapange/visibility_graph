//#define cimg_use_jpeg
#include "CImg.h"// Include CImg library header.
#include <iostream>
#include "line.h"
#include "point.h"
#include "skiplist.h"
#include <cmath>
#include <list>

using namespace cimg_library;
using namespace std;

const unsigned char WHITE[] = { 255, 255, 255 };
const unsigned char GREY[] = { 100, 100, 100 };
const unsigned char BLACK[] = { 0, 0, 0 };
const unsigned char RED[] = { 255, 0, 0 };
const unsigned char GREEN[] = { 0, 255, 0 };
const unsigned char BLUE[] = { 0, 0, 255};
const int screen_size = 800;
//const int screen_size = 400;

//-------------------------------------------------------------------------------
//  Prototypes
//-------------------------------------------------------------------------------
void vgraph(double order);
double vectorsAngle( double x, double y, double basex, double basey);
double distance( Point * a, Point * b );
void initializeLineSegments(int row_col,Line *segs[]);
void initializePoints(int row_col,Line *segs[],Point *pointList[]);
void printVisibilityOfPoints(int numOfPoints,Point *pointList[]);
Point * searchPoint(int numOfPoints,Point *pointList[],Point* p );

static int test=0;

//-------------------------------------------------------------------------------
//  Main procedure
//-------------------------------------------------------------------------------
int main()
{
	cout << endl << endl << "Visibility Graph by Dave Coleman -------------------- " << endl << endl;

	//for( double order = 1; order < 2; order += 0.5 )
	{
		vgraph(1);
	}

	return EXIT_SUCCESS;	
}

void vgraph(double order)
{
	// Variables ----------------------------------------------------------------

	// Atomic operation counter
	atomic = 0;
	double total_atomic_space = 0;
	
	// Graphics:
	bool visual = true;
	bool live = true;

	CImg<unsigned char> img(screen_size,screen_size,1,3,20);
	CImgDisplay disp(img, "Visibility Graph");      // Display the modified image on the screen
	
	// Line segments:	
	int size = pow(10.0, order);
	int row_col = sqrt(size);
	int seg = row_col * row_col;
//	int seg = 9; //Nusrat

	
	// Generate space for SEG number of lines	
	Line * segs[seg];
	Point * pointList[seg*2];

	// Track what index we are on
	

	initializeLineSegments(row_col,segs);
	initializePoints(row_col,segs,pointList);
	
	int numOfPoints=seg*2;

	// Reusable pointer locations
	Line * l;
	Point * p;

	int center_id;
	bool isPointA;
		
	// Visit each vertex once and perform the visibility algorithm
   	for(int outer = 0; outer < 2*seg; ++outer)
	{
		++atomic;
		atomic_space = 0;
		
		// First or second number on each line?
		center_id = outer / 2;

		// Garbage Collect
		if( outer )
		{
			delete center;
			delete center_line;
		}
		
		//cout << "LINE ID: " << center_id << endl;
		if( ! (outer % 2) ) // is even
		{
			center = new Point( segs[center_id]->a->x, segs[center_id]->a->y );
			isPointA = true;
		}
		else // is even
		{
			center = new Point( segs[center_id]->b->x, segs[center_id]->b->y );
			isPointA = false;
		}

		// Center Line Calc:
		center_line = new Line( center->x, center->y, center->x+1, center->y );			

		// Add pointers to all points back to parent line
		center->parentLine = segs[center_id];
		
		// Draw sweeper:
		//img.draw_line( center->x, center->y, center->x+200, center->y, RED);
		if(visual)
			img.draw_circle( center->x, center->y, 6, RED);

		/*cout << "LINE ID " << center_id << " ";
		  if(isPointA)
		  cout << "A" << endl;
		  else 
		  cout << "B" << endl;
		*/
		


		// Datastructures:
		skiplist <Point*> angleList;		
		skiplist <Line*> edgeList;	

		// Algorithm -----------------------------------------------------------------
	
		// Draw segments and insert POINTS into skiplist ordered by ANGLE -------------
		for(int i = 0; i < seg; ++i)
		{
			++atomic;
			l = segs[i];

			// Add pointers to all points back to parent line
			l->a->parentLine = l;
			l->b->parentLine = l;

			// Reset visited flags
			l->visited = false;
			l->visitedStartPoint = false;

			if(visual)
				img.draw_line(l->a->x, l->a->y, l->b->x, l->b->y, WHITE);

			if( !(i == center_id && isPointA) ) // point is not line A
			{
				if(visual)
					img.draw_circle(l->a->x, l->a->y, 2, WHITE);
					
				// Calculate the angle from center line:
				l->a->theta = vectorsAngle( l->a->x, l->a->y, center->x, center->y );

				// Sort the verticies:		
				angleList.add( l->a);

   		//		cout << "Added A for line " << i << " theta " << l->a->theta << endl;
			//	cout << "POINT "; l->a->print(); cout << endl;
			}

			if( !(i == center_id && isPointA == false) ) // point is not line B
			{
				if(visual)
					img.draw_circle(l->b->x, l->b->y, 2, WHITE);

				// Calculate the angle from center line:
				l->b->theta = vectorsAngle( l->b->x, l->b->y, center->x, center->y );

				// Sort the verticies:		
				angleList.add( l->b);
			//	cout << "Added B for line " << i << " theta " << l->b->theta << endl;

				//cout << "POINT "; l->b->print(); cout << endl;
			}
						
			//cout << endl;
		}
		
		//disp.display(img);
		//img.save("result.png"); // save the image		
		//break;	
		// Test SkipList
	//	cout << "Angle List - points ordered CC from base line";
	//	angleList.printAll();


		// Initialize Edge List Of Lines -----------------------------------------------------
		for(int i = 0; i < seg; ++i)
		{
			++atomic;
			
			l = segs[i]; // get next line to check

			// check if the current line is connected to the center point
			if( l->id == ((Line*)center->parentLine)->id )
			{
				// one center's line
				//cout << "ONE CENTER'S LINE!!!" << endl;
			}
			else
			{
				// Check each line and see if it crosses scan line
				double xi, yi;
				l->center_intercept( xi, yi ); // these are reference parameters

				// Now we know that xi,yi is on center line.
				// Next we check if X is between a & b. We know a.x > b.x, thus:
				if( l->a->x >= xi && l->b->x <= xi )
				{
					// check that xi > center->x
					if( xi >= center->x )
					{
				
						// It does intersect
						edgeList.add( l );

						// Mark as opened, somewhere on line
						l->visited = true;
			
						// Visualize:
						if(visual)
							img.draw_line(l->a->x, l->a->y, l->b->x, l->b->y, GREEN);
					}
				}
			}
		}

		if(live)
			disp.display(img);
   		
		//cout << "Edge List:";
		//edgeList.printAll();

   		// Sweep --------------------------------------------------------------
		
		//sleep(1);
		//usleep(500*1000);
   		for(int i = 0; i < 2*seg - 1; ++i)
		{
			++atomic;
			// get max atomic space
			if( total_atomic_space < atomic_space )
				total_atomic_space = atomic_space;
		
			
			//cout << "\n\n\n --------------- STARTING NEW SWEEP ------------------ \n\n\n";
			
			//cout << "SWEEP VERTEX " << i << endl;
			//if( i > 0 )
			//	break;
			
			// take the first vertex in angular order
			p = angleList.pop();
			//cout << "Sweep at "; p->print();

			// Update the center_line to the sweep location and update m,b 
			center_line->b = p;
			center_line->updateCalcs();
			
			// Update center point to contain theta between baseline and
			// current point, so that our line function can cache
			center->theta = p->theta;
		
			// decide what to do with it
			l = (Line*)p->parentLine; // cast it
			//cout << "\t"; l->print();

			// check if the current line is connected to the center point
			if( l->id == ((Line*)center->parentLine)->id )
			{
				// one center's line
				// ignore
			}
			else if( l->visited  ) // remove it from edgeList
			{
				//cout << "remove" << endl;

				if( ! l->visitedStartPoint )
				{
					l->visited = false; // allow this line to be visisted again for its start point
				}
			
				// check if its first in the edge list. if it is, its VISIBLE
				if( edgeList.isRoot( l->id ) )
				{
					//cout << "Drawing Line" << endl;

					if(visual){
						img.draw_line( center->x, center->y, p->x, p->y, BLUE ); //Vis
						searchPoint(numOfPoints,pointList,p)->addVisible(searchPoint(numOfPoints,pointList,center));
						test++;
					}

				}

				// remove
				//cout << "Value: " << l->value() << " " << l->id << endl;
			
				edgeList.remove( l->value(), l->id );

				if(visual)
					img.draw_line(l->a->x, l->a->y, l->b->x, l->b->y, WHITE);
			}
			else // add it to edge list
			{
				//cout << "add" << endl;			
				l->visited = true; // mark it as having been visited somewhere
				l->visitedStartPoint = true; // mark it as having found the first vertex
			
				// Store distance of line from center 
				l->dist = distance( p, center );
			
				edgeList.add( l );

				// check if its first in the edge list. if it is, its VISIBLE
				if( edgeList.isRoot( l->id ) )
				{
					//cout << "Drawing Line" << endl;
					if(visual){
						img.draw_line( center->x, center->y, p->x, p->y, BLUE ); //Vis
						test++;
						searchPoint(numOfPoints,pointList,p)->addVisible(searchPoint(numOfPoints,pointList,center));
					}

				}

				if(visual)
					img.draw_line(l->a->x, l->a->y, l->b->x, l->b->y, GREEN);
			}

			//Nusrat
		//	cout << "Total atomic space " << seg << "," << total_atomic_space << endl;

			if(visual)
				img.draw_circle(p->x, p->y, 5, GREY);

			//debug
			//cout << "Edge List:";
			//edgeList.printAll();
			//angleList.printAll();
			//cout << endl << endl;
		
			if(live)
			{
				disp.display(img);
				//usleep(1*1000);
				//sleep(1);
			}
		}
		//cout << "breaking" << endl;
		//break;
		if(live)
		{
			//usleep(1*1000);
			disp.display(img);
		}

		//img.fill(20);
		//cout << outer << endl;
	}

	if(visual)
	{
		// Redraw obstacle lines just for fun:
		for(int i = 0; i < seg; ++i)
		{
			l = segs[i];
	
			img.draw_line(l->a->x, l->a->y, l->b->x, l->b->y, WHITE);
			img.draw_circle(l->a->x, l->a->y, 2, WHITE);
			img.draw_circle(l->b->x, l->b->y, 2, WHITE);

			//l->a->addVisible(l->b); //Bidirectional visibility
			//l->b->addVisible(l->a);//Bidirectional visibility
			searchPoint(numOfPoints,pointList,l->a)->addVisible(searchPoint(numOfPoints,pointList,l->b));
			test++;
		}
		disp.display(img);
	

		img.save("result.bmp"); // save the image
	}



	if(visual)
	{
		// Show window until user input:	
		while (!disp.is_closed()) {
			if (disp.is_keyESC() )
				break;
			disp.wait();
		}
	}

	printVisibilityOfPoints(seg*2,pointList);
	cout << "Test "<<test;

	// Garabage collect
	//delete [] segs;
	//free(segs);
}

//-------------------------------------------------------------------------------
//  Calculate Angle Btw 2 Vectors
//-------------------------------------------------------------------------------
double vectorsAngle( double x, double y, double basex, double basey)
{
	// Convert input point x & y to be vectors relative to base point
	double x2 = double(x - basex);
	double y2 = double(y - basey);

	// Hard code scan line to point right:
	double x1 = sqrt( x2*x2 + y2*y2 ); // make it with ratio?
	double y1 = 0.0;

	//cout << "x1: " << x1 << " - y1: " << y1 << endl;
	//cout << "x2: " << x2 << " - y2: " << y2 << endl;

	double stuff = (  (x1*x2)+(y1*y2)   ) / (  sqrt(x1*x1+y1*y1)  *  sqrt(x2*x2+y2*y2) );
	//cout << "Stuff: " << stuff << endl;

	// Calculate angle:	
	double result = acos( stuff );
	//cout << "Result: " << result << endl;

	// Now add PI if below middle line:
	if( y >= basey )
		result = 2*M_PI - result;
	
	//cout << "Result: " << result*180/M_PI << " degrees" << endl;

	return result;
}
//-------------------------------------------------------------------------------
//  Distance Btw 2 Points
//-------------------------------------------------------------------------------
double distance( Point * a, Point * b )
{
	return sqrt( pow(b->x - a->x, 2.0) + pow(b->y - a->y,2.0) );
}

void initializeLineSegments(int row_col,Line *segs[]){

	int index = 0;
	// Coordinates:
	double width = screen_size / row_col; // size of each grid box
	double margin = 0.1 * width;  // padding inside each box
	double top, bottom, left, right; // coordinates of box with padding
	// Now generate seg line segments
	for(int x = 0; x < row_col; ++x)
	{
		for(int y = 0; y < row_col; ++y)
		{
			top = y*width + margin;
			bottom = (y+1)*width - margin;
			left = x*width + margin;
			right = (x+1)*width - margin;

			// Create line segment in box of size width*width
			// x1, y1, x2, y2
			switch( rand() % 4 )
			{
			case 0: // verticle line
				segs[index] = new Line( left, top, left, bottom );
				break;
			case 1: // horizontal line
				segs[index] = new Line( left, top, right, top );
				break;
			case 2: // diagonal left to right
				segs[index] = new Line( left, top, right, bottom );
				break;
			case 3:
				segs[index] = new Line( left, bottom, right, top );
				break;
			}

			index++;
		}
	}


		//Nusrat
/*		segs[0] = new Line(40,140,240,40); // 0 first
		segs[1] = new  Line(80,500,160,200); // 1 second
		segs[2] = new   Line(80,500,100,300); // 2 third, later
		segs[3] = new  Line(100,300,160,200) ;// 3 far righ
		segs[4] = new Line(400,100,440,250); // 0 first
		segs[5] = new  Line(440,250,560,200); // 1 second
		segs[6] = new   Line(560,200,480,150); // 2 third, later
		segs[7] = new  Line(480,150,520,50) ;// 3 far righ
		segs[8] = new  Line(520,50,400,100) ;// 3 far righ
*/

}
void initializePoints(int row_col,Line *segs[],Point *pointList[]){

	int index=row_col*row_col; //Nusrat
		for(int i=0;i<index;i++){
				segs[i]->print();
				pointList[2*i]=segs[i]->a;
				pointList[2*i+1]=segs[i]->b;

		}

		/*for(int i=0;i<index*2;i++){
				pointList[i]->print();
		}*/
}
void printVisibilityOfPoints(int numOfPoints,Point *pointList[]){

	for(int i=0;i<numOfPoints;i++){
		pointList[i]->print();
		pointList[i]->printVisible();
	}
}

Point * searchPoint(int numOfPoints,Point *pointList[],Point* p ){

	for(int i=0;i<numOfPoints;i++){
		if((pointList[i]->x == p->x) && (pointList[i]->y == p->y)){
			return pointList[i];
		}

	}
	return NULL;
}
