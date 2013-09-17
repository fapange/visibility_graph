/* Skip List
   CSCI 5454 Algorithms
   Dave Coleman | david.t.coleman@colorado.edu
   
   2/2/2012

   Implementation of Skip Lists
*/

#include <math.h>
#include <iostream>
#include <cstdlib>
#include "node.h"
//#include "point.h"
using namespace std;

// -----------------------------------------------------------------------------------
// Skip List Class
// -----------------------------------------------------------------------------------
template <class T>
class skiplist{
   	// used for testing
 public:
	int maxLevel;
	
 private:
	node<T> *root;

	// -----------------------------------------------------------------------------------
	// Get Random Level
	// -----------------------------------------------------------------------------------	
	int getRandLevel()
	{
		int randResult = 1;
		int level = 0;
		while(randResult)
		{
			
			randResult = rand() % 2;
			if(randResult)
			{
				++level;
			}
			
			if(level > maxLevel)
			{
				randResult = 0; // to end the while loop
			}
		}
		return level;
			
	}
	// -----------------------------------------------------------------------------------
	// Create New Node
	// -----------------------------------------------------------------------------------	
	node<T>* createNode( int level, int height, T data)
	{
		// Check if we are below level 0
		if(level < 0)
		{
			return NULL;
		}
		else // make a new node below
		{
			node<T> *newNode = new node<T>();			
			newNode->level = level;
			newNode->next = NULL;
			newNode->below = createNode( level - 1, height, data);
			newNode->height = height;
			newNode->data = data;

			atomic_space++;
			
			return newNode;
		}
	}	
	
 public:

	// Constructor:
	skiplist()
	{
		root = NULL;
		maxLevel = 0;

		srand ( time(NULL) ); // seed the random generator
	}
	// -----------------------------------------------------------------------------------
	// ADD
	// -----------------------------------------------------------------------------------
	void add( T data)
	{
		//cout << "ADD: ";
		//data.print();
		
		// Special Cases -------------------------------------------------------------------

		if(!root) // no root has been established yet
		{
			root = createNode( 0, 0, data);
			return;
		}
		//cout<<root->data->id<<" root->data->value() "<<root->data->value()<<" -" <<data->id<<" data->value() "<<data->value();
		//cout<<endl;
		if( root->data->value() > data->value() ) // new value goes before root
		{
			T temp_data = root->data;
			node<T> *n = root;

			for(int l = maxLevel; l >= 0; --l)
			{
				atomic += 1;
				// change the root to the new value
				n->data = data;
				n = n->below;
			}
			data = temp_data;
		}

		// Regular insert after root -------------------------------------------------------

		// Determine what level this new node will be at
		int level = getRandLevel();
	   
		// If new node is at whole new level, go ahead and update root node to be higher
		if(level > maxLevel)
		{
			maxLevel ++;
			node<T> *newRoot = new node<T>();
			newRoot->data = root->data;
			newRoot->next = NULL;
			newRoot->below = root;
			newRoot->level = maxLevel;
			root = newRoot;

			atomic_space++;			
		}

		// Create the new node
		node<T> *newNode = createNode( level, level, data);
					
		// Now add the node to the list
		node<T> *i = root; 

		// Loop down through all levels
		for(int l = maxLevel; l >= 0; --l)
		{
			atomic += 1;
			// move forward until we hit a value greater than ours
			while( i->next != NULL )
			{
				atomic += 1;
				if( i->next->data->value() > data->value() ) // insert before i.next
				{
					break;
				}
				i = i->next;
			}

			// Check if we should add a pointer at this level
			if( l <= level )
			{
				newNode->next = i->next;
				i->next = newNode;
				
				// Now move the new node pointer one level down:
   				newNode = newNode->below;
			}

			// Always move the i node pointer one level down:
			i = i->below;			
		}

	}
	// -----------------------------------------------------------------------------------
	// Find
	// -----------------------------------------------------------------------------------
	bool find(double x)
	{
		node<T> *i = root;

		// Special case: skip list is empty
		if( !root )
		{
			return false;
		}

		// Special case: check root
		if( root->data->value() == x)
		{
			return true;
		}

		for(int l = maxLevel; l >= 0; --l)
		{
			atomic += 1;
			// move forward until we hit a value greater than ours
			while( i->next != NULL )
			{
				atomic += 1;
				if(  i->next ->data->value() > x ) // x is not found on this level
				{
					break;
				}
				else if(  i->next->data->value() == x ) // bingo!
				{
					return true;
				}
				
				i = i->next;
			}

			// Always move the i node<T> pointer one level down:
			i = i->below;			
		}	

		return false;
	}
	// -----------------------------------------------------------------------------------
	// REMOVE
	// the id is to confirm the correct node, just in case x is not unique
	// -----------------------------------------------------------------------------------
	bool remove(double x, int id)
	{
		node<T> *i = root;

		// Special case: remove root --------------------------------------------------------
		if( root->data->value() == x && root->data->id == id)
		{
			// Get level 0 of root
			for(int l = root->level; l > 0; --l)
			{
				atomic += 1;
				atomic_space --;
				//cout << "Level " << l << endl;
				i = i->below;
			}

			// Check if there are any more nodes
			if( !i->next ) // the skip list is empty
			{
				root = NULL;
				maxLevel = 0;
					
				return true;
			}

			// Change value of root to next node
			node<T> *n = root;
			node<T> *nextNode = i->next;

			for(int l = maxLevel; l >= 0; --l)
			{
				atomic += 1;
				// change the root to the new value
				n->data = nextNode->data;

				// update next pointer if the next next exists
				if( n->next )
				{
					n->next = n->next->next;					
				}

				// Move down to next level				
   				n = n->below;
			}

			return true;
		}

		// Normal case: remove after root -----------------------------------------------
		bool found = false;
		
		for(int l = maxLevel; l >= 0; --l)
		{
			atomic += 1;
			// move forward until we hit a value greater than ours
			while( i->next != NULL )
			{
				atomic += 1;
				// remove this one, confirmed by id
			    if( i->next->data->value() == x && i->next->data->id == id ) 
				{
					found = true;
					atomic_space --;
					
					// pass through the pointer if exists
					if( i->next )
					{
						i->next = i->next->next;
					}
					else
					{
						i->next = NULL;
					}
					break;
				}
				else if( i->next->data->value() > x ) // x is not found on this level
				{
					break;
				}

				i = i->next;
			}

			// Always move the i node pointer one level down:
			i = i->below;
		}	

   		return found;
	}	
	// -----------------------------------------------------------------------------------
	// POP FROM FRONT
	// -----------------------------------------------------------------------------------
	T pop()
	{
		node<T> *i = root;

		// Store the first item on the list that we want to later return
		T result = root->data;
		/*
		  cout << "POP WITH VALUE: " << root->value << " - ";
		  result.print();
		  cout << endl;
		*/
		
		// Check if skip list is empty
		if( !root )
		{
			cout << "An error has occured: skip list is empty";
			exit(1);
		}

		// Get level 0 of root
		for(int l = root->level; l > 0; --l)
		{
			atomic += 1;
						
			i = i->below;
		}

		// Check if there are any more nodes
		if( !i->next ) // the skip list is empty
		{
			root = NULL;
			maxLevel = 0;
					
			return result;
		}

		// Change value of root to next node
		node<T> *n = root;
		node<T> *nextNode = i->next;

		for(int l = maxLevel; l >= 0; --l)
		{
			atomic += 1;
			atomic_space --;
			// change the root to the new value
			n->data = nextNode->data;
			
			// update next pointer if the next next exists
			if( n->next )
			{
				n->next = n->next->next;					
			}

			// Move down to next level				
			n = n->below;

		}

		return result;
	}
	// --------------------------------------------------------------------------------
	// Is Root
	// -----------------------------------------------------------------------------------
	bool isRoot(int id)
	{

		if( !root ) // there is no root!
		{
			std::cout << "there is no root!" << std::endl;
			return false;
		}
	    return (root->data->id == id);
	}
	// -----------------------------------------------------------------------------------
	// PRINT ALL
	// -----------------------------------------------------------------------------------
	void printAll()
	{
		std::cout << std::endl << "LIST --------------------------------" << std::endl;

		// Special case: skiplist is empty
		if( !root )
		{
			std::cout << "----------------------------------" << std::endl;
			return;
		}

		node<T> i = *root;
		
		// Get level 0 of root
		for(int l = root->level; l > 0; --l)
		{
			//cout << "Level " << l << " - ";
			//i.data->print();
			//cout << endl;
			i = *(i.below);
		}
		//std::cout << "we are on level " << i.level;
		//std::cout << std::endl;

		// Hack: update root 0 level with maxLevel count, because we don't update this
		// when growing root level size
		i.height = maxLevel;
			
		int counter = 0;
		bool done = false;
		
		while(!done)
		{
			std::cout << counter;
			
			for(int l = i.height; l >= 0; --l)
			{
				std::cout << " | ";
			}
		//	std::cout << " " << i.data->value()*180/M_PI  << " - ";
			std::cout << " " << i.data->value()  << " - ";
			i.data->print();
			
			counter ++;

			if( i.next )
			{
				node<T> *ii = i.next;
				i = *ii;
			}
			else
			{
				done = true;
			}
		}
	
		std::cout << "-------------------------------------" << std::endl << std::endl;
	}
};
