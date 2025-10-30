
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

//======================
//======================
//Defining needed arrays:
//Defining S-boxes with their standard values:
unsigned long long S_boxes[8][4][16] =
{
    {
        // S1
        {14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
        {0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
        {4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
        {15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
    },
    {
        // S2
        {15,1,8,14,6,11,3,4,9,7,2,13,12,0,5,10},
        {3,13,4,7,15,2,8,14,12,0,1,10,6,9,11,5},
        {0,14,7,11,10,4,13,1,5,8,12,6,9,3,2,15},
        {13,8,10,1,3,15,4,2,11,6,7,12,0,5,14,9}
    },
    {
        // S3
        {10,0,9,14,6,3,15,5,1,13,12,7,11,4,2,8},
        {13,7,0,9,3,4,6,10,2,8,5,14,12,11,15,1},
        {13,6,4,9,8,15,3,0,11,1,2,12,5,10,14,7},
        {1,10,13,0,6,9,8,7,4,15,14,3,11,5,2,12}
    },
    {
        // S4
        {7,13,14,3,0,6,9,10,1,2,8,5,11,12,4,15},
        {13,8,11,5,6,15,0,3,4,7,2,12,1,10,14,9},
        {10,6,9,0,12,11,7,13,15,1,3,14,5,2,8,4},
        {3,15,0,6,10,1,13,8,9,4,5,11,12,7,2,14}
    },
    {
        // S5
        {2,12,4,1,7,10,11,6,8,5,3,15,13,0,14,9},
        {14,11,2,12,4,7,13,1,5,0,15,10,3,9,8,6},
        {4,2,1,11,10,13,7,8,15,9,12,5,6,3,0,14},
        {11,8,12,7,1,14,2,13,6,15,0,9,10,4,5,3}
    },
    {
        // S6
        {12,1,10,15,9,2,6,8,0,13,3,4,14,7,5,11},
        {10,15,4,2,7,12,9,5,6,1,13,14,0,11,3,8},
        {9,14,15,5,2,8,12,3,7,0,4,10,1,13,11,6},
        {4,3,2,12,9,5,15,10,11,14,1,7,6,0,8,13}
    },
    {
        // S7
        {4,11,2,14,15,0,8,13,3,12,9,7,5,10,6,1},
        {13,0,11,7,4,9,1,10,14,3,5,12,2,15,8,6},
        {1,4,11,13,12,3,7,14,10,15,6,8,0,5,9,2},
        {6,11,13,8,1,4,10,7,9,5,0,15,14,2,3,12}
    },
    {
        // S8
        {13,2,8,4,6,15,11,1,10,9,3,14,5,0,12,7},
        {1,15,13,8,10,3,7,4,12,5,6,11,0,14,9,2},
        {7,11,4,1,9,12,14,2,0,6,10,13,15,3,5,8},
        {2,1,14,7,4,10,8,13,15,12,9,0,3,5,6,11}
    }
};



//======================

bool bits48[8][6] = {0}; //for input bits
bool bits32[8][4] = {0}; //for output bits
unsigned long long subDecVal[8]; //the values of the 4-bit values from s-boxes that substitute the original 6-bits values (all in decimal)



//====================================================================================================
//====================================================================================================
//putting the decimal value of 48bits into a binary form in a boolean array
void get48Bits(unsigned long long decVal, bool bits[8][6])
{

    for(int i = 0; i<8 ; i++)
    {
        for(int j = 0; j< 6; j++)
        {
            bits[i][j] = (decVal & 1);
            decVal>>= 1;
        }
    }


}

//====================================================================================================
//====================================================================================================
//getting the 4-bit values from s-boxes that substitute the original 6-bits values (all in decimal)
void getSubDecVal(unsigned long long subDecVal[8], bool bits48[8][6])
{
    int k = 7;
    int row;
    int col;
    for(int i = 0; i<8; i++)
    {

        row = bits48[i][5]*2 + bits48[i][0]*1;
        col = bits48[i][4]*8 + bits48[i][3]*4 + bits48[i][2]*2 + bits48[i][1]*1;
        subDecVal[k] = S_boxes[k][row][col];
        k--; // this variable counts in reverse of the bits of the input as the least significant associated with the most significant in boxes and vice versa

    }


}
//====================================================================================================
//====================================================================================================
//get the bits that will represent the output in binary//MSB is bits[7][3] and LSB is bits[0][0]
void get32Bits(unsigned long long subDecVal[8], bool bits[8][4])
{
    int k = 7;
    for(int i = 0; i<8 ; i++)
    {
        for(int j = 0; j<4; j++)
        {
            bits[i][j] = (subDecVal[k] & 1);
            subDecVal[k]>>= 1;
        }
        k--;
    }

}
//====================================================================================================
//====================================================================================================
//main S-box function that get us the final decimal result with the use of all small functions implemented
unsigned long long S_box(unsigned long long decInput)
{
    int bitOrder = 0;
    unsigned long long finalDecOut = 0;
    get48Bits(decInput, bits48);
    getSubDecVal(subDecVal,bits48);
    get32Bits(subDecVal,bits32);
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            finalDecOut += bits32[i][j]*(pow(2,bitOrder));
            bitOrder++;
        }

    }
    return finalDecOut;


}

//====================================================================================================
//====================================================================================================





int main()
{

//for ex:
//printf("%llu\n",S_box(27)); // the correct output: 4020710478

//printf("%llu\n",S_box(16710908));
//correct output 4020714069
//1110 1111 1010 0111 0011 1010 0101 0101

    return 0;
}




