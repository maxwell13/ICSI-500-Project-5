//============================================================================
// Name        : prototo.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "transmitter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <fstream>
#include <streambuf>
#include <sstream>
#include <math.h>


using namespace std;




/*
should contain routines to handle jobs such as
converting a character into a binary bit pattern,
calculating, and including a parity bit,
writing bits into an output file (i.e., transmission of bits).
*/
class tranPhysical {       // The class
public:
    // converts decimal interger to binary and adds odd parity
	// puts results in output file returns the resulting integer as well
    int * convertIntToBinary(int n){

    	//initialze and res to zero
    	static int binary[8];
        static int binaryLSB[8];
    	for (int k = 0;k<8 ;k++){
    		 binary[k]=0;
    		 binaryLSB[k]=0;
    	}

    	int sumBits=0;
        int i = 0;
        // interates through all positions finding the reminder of divisions by 2
        while (n > 0) {
            binary[i] = n % 2;
            sumBits=sumBits+(binary[i]);
            n = n / 2;
            i++;
        }


        // setting order to LSB
    	for (int j = i - 1; j >= 0; j--){
    		// off set by 1 for odd parity bit
    		binaryLSB[j+1] = binary[j];
    //		cout << binaryLSB[j];
    	}

        //if sumBits is even add parity
        if  (sumBits % 2==0){
     	    	binaryLSB[0]=1;
        }

        //writes bit to file
        ofstream  bfile;
        bfile.open("filename.binf", std::ios_base::app);
    		if (bfile.is_open())
    		{
    			for(int k=0;k<8;k++)
    			{
    				bfile << binaryLSB[k];
    			}

//    			bfile << '\n';

    		bfile.close();
    		}
    	else cout << "Unable to open file";

        return binaryLSB;
    }

    // converts char to binary and outputs
    int * convertCharToBinary(char s){
        return  convertIntToBinary(int(s));
    }

};


/*should contain routines for
 * framing (putting two SYN characters,
 *  a LENGTH character, and data into a frame) */

class transDataLink {       // The class
  public:             // Access specifier
	void addSynAndControl(int n)
	{
		 tranPhysical ouput;
		 ouput.convertIntToBinary(22);
		 ouput.convertIntToBinary(22);
		 ouput.convertIntToBinary(n);

	}



};





//application layer
transApp::transApp()
	{


		// read in input
		string inputLine;



		 std::ifstream infile("filename.inpf");
;


		 if (infile.is_open())
		  {

			 //create output file
			 ofstream bfile ("filename.binf");

			 std::stringstream buffer;
			 buffer << infile.rdbuf();
			 inputLine = buffer.str();

			 cout << "the following  is input" << '\n';
			 cout << inputLine;
			 cout << '\n';


			 int numBlocks = ceil(inputLine.length()/64);
			 int sizeLastBlock =  inputLine.length()- (numBlocks)*64;

			 //for the number of blocks
			 for (int j=0;j<= numBlocks;j++)
			 {

				 //if last block
				 if(j==numBlocks)
				 {
					//format head of block
					transDataLink lead;
					lead.addSynAndControl(sizeLastBlock);

					tranPhysical inputToOutput;

					// convert input line to binary ascii with parity bit
					// interates through each input char
					for(int i = j*64 ; i <  j*64+sizeLastBlock ; i ++ ){
						 inputToOutput.convertCharToBinary(inputLine[i]);
					}
				 }
				 else
				 {
					 //format head of block
					 transDataLink lead;
					 lead.addSynAndControl(64);
					 tranPhysical inputToOutput;

					// convert input line to binary ascii with parity bit
					// interates through each input char
					for(int i = j*64 ; i < (j+1)*64 ; i ++ ){
					 inputToOutput.convertCharToBinary(inputLine[i]);

					}
				 }




			  }
		  }
		 // error
		  else cout << "Unable to open file";



	return;
	}



