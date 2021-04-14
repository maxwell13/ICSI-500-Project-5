/*
 * ErrorGen.cpp
 *
 *  Created on: Feb 24, 2021
 *      Author: maxwell
 */

#include "ErrorGen.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <math.h>

using namespace std;

//generates n errors
ErrorGen::ErrorGen(int n)
{
	 cout << "Generating " << n << " errors"	<< '\n';

	 int frame;
	 int byte;
	 int bit;

	string inputLine;
	ifstream infile ("filename.binf");
	 if (infile.is_open())
	  {
		 getline (infile,inputLine);
	  }
	  else cout << "Unable to open file";

	  int numInputChars = inputLine.length();

	 //if number of errors exceeds number of bits
	 if(n>numInputChars )
		 {
		 cout << "too many errors specified exiting";
		 return;
		 }


	 //sets error seed
	  srand (15);
	  int error;
	  bool errorUsed;

	  int errorsUsed[n];

	  for (int i=0;i<n;i++)
	  {

		  //the following block deals with collisions
		  errorUsed=true;
		  while(errorUsed)
		  {
			  errorUsed=false;
			  error = rand() %  numInputChars; // rand int range 0 to length errors
			  for (int j=0;j<n;j++)
			  {
				  if(error==errorsUsed[j])
				  {
					  errorUsed=true;
				  }
			  }

		  }
		  errorsUsed[i]=error;

		  //calculations for frame etc
		  frame = ceil(error/((67*8)));
		  byte = ceil((error - frame*(67*8))/8);
		  bit = ceil(error-byte*8- frame*(67*8));

		  //switches bit for new error
		  if(inputLine[error]=='1')
		  {
			  inputLine[error]='0';
			  cout <<" error at frame " <<  frame << " byte " <<  byte  << " bit " << bit << " 1 changed to " << inputLine[error] << '\n' ;

		  }
		  else if(inputLine[error]=='0')
		  {
			  inputLine[error]='1';
			  cout <<" error at frame " <<  frame << " byte " <<  byte  << " bit " << bit << " 0 changed to " << inputLine[error] << '\n' ;
		  }
	  }


      //writes error line to file
      ofstream bfile ("filename.binf");
  		if (bfile.is_open())
  		{
  			bfile << inputLine;
  			bfile.close();
  		}
  	else cout << "Unable to open file";




}
