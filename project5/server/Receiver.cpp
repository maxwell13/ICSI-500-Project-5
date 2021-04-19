//============================================================================
// Name        : prototo.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Receiver.h"
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

using namespace std;


/*
 should also include routines for
 reading bits from a file (i.e., reception of bits)
  converting bits into characters
*/
class recPhysical {       // The class
public:
    // converts  binary to interger and removes and checks odd parity
    int convertBinaryToDecInt(string revNum) {

        int decVal = 0;
        char charNum[7];


        //reverses string
        int sum = 0;
        int k = 7;

        // only 7 bits for parity bit
        for (int i = 0; i < 7; i++) {
            sum = sum + int(revNum[k]);
            charNum[i] = revNum[k];
            k--;
        }

        //adds parity bit
        sum = sum + int(revNum[0]);

        //returns if odd parity is not maintained
        if (sum % 2 == 0) { cout << "ERRROR not odd bit parity" << '\n'; }


//        cout << charNum;
//        cout << '\n';

        //converts intext.txt to int
        int num = atoi(charNum);

        // set base value to 1
        int base = 1;

        int temp = num;
        int lastDig;

        //converts binary to dec
        while (temp) {
            lastDig = temp % 10;
            temp = temp / 10;

            decVal += lastDig * base;

            base = base * 2;
        }


        return decVal;
    }

    //returns a string which represent a byte starting at start position
    string getByte(int startPos, string inputLine) {

        char Byte[8];

        int iter = 0;
        for (int i = startPos; i < startPos + 8; i++) {
            Byte[iter] = inputLine[i];
            iter++;
        }

        return Byte;
    }

    //converts a byte to a character
    char convertBinaryToChar(string byte) {
        int temp = convertBinaryToDecInt(byte);
        char w = static_cast<char>(temp);
        return w;

    }


};


/*deframing (separating those control characters from data characters). */
class recDataLink {       // The class
public:             // Access specifier
    int addSynAndControl(int block, string inputLine) {

        string holder;
        int inputLength;
        recPhysical bitRead;
        if (bitRead.convertBinaryToDecInt(bitRead.getByte(0 + block * (67 * 8), inputLine)) != 22) {

            cout << "ERROR not right control";
            cout << '\n';
            return -1;
        }
        if (bitRead.convertBinaryToDecInt(bitRead.getByte(8 + block * (67 * 8), inputLine)) != 22) {
            cout << "ERROR not right control";
            cout << '\n';
            return -1;
        }

        //returns number of bytes to read
        return bitRead.convertBinaryToDecInt(bitRead.getByte(16 + block * (67 * 8), inputLine));

    }


};


//application layer for reciver
recApp::recApp(string bin) {

    //create output file
    ofstream bfile("filename.outf");


    int numBlocks = ceil(bin.length() / (67 * 8));
    // cout << '\n' << "number of bits is " << bin.length();

    // cout << '\n' << "number of blocks " << numBlocks << '\n';

    int block_adjust;

    //loops through each block

    for (int j = 0; j <= numBlocks; j++) {

        recDataLink dataControl;
        //gets the number of bytes and verifines the syn numbers
        int numBytes = dataControl.addSynAndControl(j, bin);

        if (numBytes == -1) {
            cout << "error in transmission";
            return;
        }

        recPhysical inputToOutPut;

        for (int i = 3; i < numBytes + 3; i++) {

            bfile << inputToOutPut.convertBinaryToChar(inputToOutPut.getByte(i * 8 + j * (67 * 8), bin));
            // cout << inputToOutPut.convertBinaryToChar(inputToOutPut.getByte(i * 8 + j * (67 * 8), bin));

        }

    }
    bfile.close();

    return;
}


