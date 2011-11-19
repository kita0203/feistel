#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <ctype.h>
#define ever ;;i++

enum {block_size = 16};
enum {key_size = 8};
enum {rounds = 4};


void
echo(void)
{
    struct termios term;
    tcgetattr (1, &term);
    term.c_lflag ^= ECHO;
    tcsetattr (1, TCSANOW, &term);
}

char *
getstr(void)
{
    char * str;
    char d;
    int i = 0;
    struct termios termold, term;
    tcgetattr (1, &termold);
    tcgetattr (1, &term);
    term.c_lflag ^= ICANON;
    term.c_cc[VERASE] = 0;
    tcsetattr (1, TCSAFLUSH, &term);
    str = calloc(17, sizeof(char));
    for(ever)
    {
        echo();
        scanf("%c", &d);
        echo();
        if (d == 0x7F)
        {
            if (i != 0)
            {
                printf("\b \b");
                i--;
            }
            i--;
        }
        else
        {
           if (d == '\n')
           {
               break;
           }
           if (isdigit(d) || isalpha(d) || ispunct(d) || ' ')
           {
                printf("*");
                if (i < 16)
                {
                    str[i] = d;
                }
            }
            else
            {
                i--;
            }
        }
    }
    tcsetattr (1, TCSAFLUSH, &termold);
    while(i <= 16)
    {
        str[i] = '0';
        i++;
    }
    return str;
}




unsigned char **
keygen(unsigned char *key, int num)
{
    unsigned char **arr;
    int i, j;
    unsigned char tmp1, tmp2;
    arr = calloc(num, sizeof(unsigned char *));
    arr[0] = calloc(8, sizeof(unsigned char));
    arr[0][0] = key[3];
    arr[0][1] = key[5];
    arr[0][2] = key[6];
    arr[0][3] = key[7];
    arr[0][4] = key[1];
    arr[0][5] = key[4];
    arr[0][6] = key[2];
    arr[0][7] = key[0];
    tmp1 = 0;
    for(j = 0; j < 8; j++)
    {
        tmp1 <<= 3;
        tmp2 = arr[0][j] & 1;
        arr[0][j] >>= 1;
        arr[0][j] |= tmp1;
        tmp1 = tmp2;
    }
    arr[0][0] |= tmp1;
    for(i = 1; i < num; i++)
    {
        arr[i] = calloc(8, sizeof(unsigned char));
        arr[i][0] = arr[i - 1][3] ^ arr[i - 1][0];
        arr[i][1] = !arr[i - 1][5];
        arr[i][2] = arr[i - 1][6] ^ arr[i - 1][2];
        arr[i][3] = arr[i - 1][7];
        arr[i][4] = arr[i - 1][1];
        arr[i][5] = arr[i - 1][4] ^ arr[i - 1][7];
        arr[i][6] = arr[i - 1][2];
        arr[i][7] = arr[i - 1][0];
        tmp1 = 0;
        for(j = 0; j < 8; j++)
        {
            tmp1 <<= 3;
            tmp2 = arr[i][j] & 1;
            arr[i][j] >>= 1;
            arr[i][j] |= tmp1;
            tmp1 = tmp2;
        }
    }
    return arr;
}


void sbox(unsigned char *block, int size)
{
    unsigned char (*(*s))[16] = calloc(8, 16 * 16 * sizeof(unsigned char));
    unsigned char s0[16][16] = {
{251, 145, 218, 181, 252, 246, 95, 41, 124, 143, 2, 26, 19, 244, 29, 68},
{229, 85, 120, 160, 214, 212, 240, 146, 97, 158, 227, 172, 234, 215, 58, 10},
{0, 190, 248, 87, 186, 40, 8, 176, 219, 31, 52, 195, 22, 56, 7, 155},
{193, 198, 222, 207, 121, 1, 75, 236, 153, 197, 205, 247, 129, 9, 39, 182},
{105, 24, 162, 169, 93, 20, 6, 84, 14, 38, 209, 70, 100, 148, 37, 137},
{226, 163, 241, 245, 96, 254, 71, 185, 180, 255, 66, 174, 50, 189, 53, 67},
{57, 202, 206, 86, 200, 147, 220, 13, 33, 110, 141, 42, 43, 161, 136, 171},
{165, 3, 232, 46, 59, 4, 16, 170, 15, 102, 90, 23, 175, 64, 81, 117},
{221, 21, 178, 72, 62, 74, 201, 116, 17, 47, 250, 156, 88, 12, 217, 230},
{138, 18, 183, 233, 104, 239, 108, 159, 150, 125, 211, 111, 51, 92, 76, 94},
{5, 139, 166, 112, 11, 89, 177, 119, 194, 149, 173, 167, 73, 188, 69, 164},
{238, 48, 151, 30, 44, 213, 196, 45, 28, 122, 237, 142, 35, 36, 55, 99},
{131, 191, 225, 210, 204, 208, 157, 134, 179, 82, 78, 168, 123, 115, 113, 128},
{126, 184, 54, 83, 140, 203, 216, 118, 132, 127, 223, 187, 133, 63, 249, 25},
{106, 235, 98, 49, 101, 135, 114, 192, 130, 243, 77, 91, 103, 144, 80, 61},
{154, 199, 32, 224, 60, 242, 79, 27, 65, 228, 231, 253, 34, 107, 152, 109},
};


unsigned char s1[16][16] = {
{69, 105, 23, 74, 162, 155, 28, 84, 81, 35, 125, 167, 109, 97, 189, 121},
{98, 133, 16, 95, 166, 40, 148, 221, 78, 219, 54, 77, 39, 196, 17, 252},
{139, 227, 107, 245, 42, 49, 182, 70, 24, 217, 43, 228, 186, 223, 179, 19},
{123, 7, 197, 103, 243, 255, 8, 44, 176, 128, 60, 206, 6, 161, 191, 190},
{41, 9, 14, 149, 248, 168, 233, 111, 117, 237, 65, 225, 202, 129, 208, 214},
{29, 4, 246, 151, 141, 57, 83, 21, 203, 212, 205, 240, 140, 5, 27, 118},
{235, 93, 211, 220, 134, 0, 193, 164, 52, 144, 188, 192, 216, 232, 213, 132},
{2, 170, 242, 106, 59, 90, 147, 244, 187, 88, 30, 25, 113, 26, 61, 157},
{120, 91, 15, 80, 115, 127, 199, 31, 172, 62, 56, 249, 156, 18, 198, 51},
{53, 169, 171, 254, 116, 58, 108, 131, 204, 152, 72, 130, 20, 96, 46, 183},
{82, 67, 47, 10, 253, 194, 126, 238, 175, 234, 87, 163, 210, 165, 150, 230},
{173, 50, 94, 114, 184, 76, 160, 36, 86, 174, 143, 215, 11, 1, 226, 154},
{124, 38, 112, 177, 207, 100, 145, 136, 32, 37, 201, 92, 224, 63, 122, 101},
{135, 222, 64, 146, 153, 71, 159, 250, 239, 13, 209, 181, 45, 48, 75, 218},
{110, 73, 231, 85, 99, 158, 79, 33, 178, 138, 195, 180, 55, 102, 3, 22},
{247, 185, 251, 241, 137, 68, 236, 200, 142, 89, 229, 104, 119, 66, 34, 12},
};

unsigned char s2[16][16] = {
{112, 202, 21, 208, 145, 20, 63, 117, 246, 219, 100, 64, 6, 86, 53, 31},
{49, 236, 119, 7, 212, 169, 157, 234, 159, 54, 125, 179, 41, 245, 98, 216},
{225, 72, 239, 14, 226, 217, 123, 60, 18, 153, 65, 180, 174, 173, 97, 158},
{39, 70, 29, 148, 188, 12, 242, 237, 140, 162, 45, 141, 92, 177, 124, 114},
{240, 149, 17, 241, 106, 191, 107, 132, 161, 182, 154, 102, 9, 69, 58, 152},
{206, 165, 23, 155, 37, 0, 170, 44, 120, 164, 151, 136, 76, 34, 122, 172},
{111, 127, 52, 28, 103, 61, 178, 207, 10, 163, 104, 89, 16, 176, 56, 231},
{224, 3, 167, 96, 93, 137, 110, 183, 186, 192, 33, 144, 15, 221, 253, 126},
{229, 184, 143, 194, 248, 47, 147, 251, 254, 214, 83, 175, 218, 203, 84, 82},
{171, 213, 95, 210, 68, 51, 249, 196, 197, 247, 66, 139, 94, 235, 90, 99},
{38, 205, 80, 108, 223, 101, 181, 209, 91, 243, 199, 193, 146, 168, 227, 71},
{142, 189, 35, 5, 81, 250, 222, 75, 85, 200, 109, 150, 67, 25, 42, 134},
{73, 8, 40, 62, 30, 238, 230, 187, 57, 79, 87, 190, 22, 198, 11, 105},
{195, 43, 166, 1, 129, 46, 55, 74, 220, 255, 13, 131, 130, 138, 185, 48},
{27, 113, 59, 204, 4, 19, 128, 156, 244, 160, 2, 26, 252, 88, 77, 32},
{36, 115, 211, 24, 233, 116, 232, 215, 50, 78, 133, 201, 118, 121, 228, 135},
};

unsigned char s3[16][16] = {
{199, 254, 73, 15, 255, 144, 154, 75, 175, 121, 184, 10, 177, 221, 159, 161},
{54, 190, 6, 11, 198, 151, 192, 2, 41, 112, 97, 178, 64, 202, 251, 16},
{1, 214, 110, 191, 123, 193, 252, 21, 92, 58, 106, 143, 196, 7, 29, 157},
{24, 249, 187, 118, 69, 47, 160, 246, 182, 194, 204, 23, 80, 206, 167, 3},
{9, 99, 250, 211, 50, 247, 168, 243, 228, 213, 185, 216, 155, 170, 100, 131},
{164, 234, 60, 94, 53, 107, 84, 115, 203, 101, 96, 176, 148, 36, 102, 116},
{55, 37, 42, 83, 108, 210, 98, 33, 38, 239, 171, 212, 186, 44, 237, 165},
{74, 236, 195, 91, 93, 200, 136, 134, 113, 56, 87, 189, 61, 223, 139, 230},
{197, 76, 72, 172, 248, 105, 67, 122, 117, 88, 217, 229, 233, 78, 68, 141},
{12, 166, 173, 183, 162, 142, 235, 207, 128, 77, 238, 226, 27, 225, 46, 103},
{146, 65, 30, 104, 150, 52, 25, 20, 81, 14, 40, 39, 114, 13, 22, 17},
{95, 86, 227, 132, 215, 85, 220, 244, 130, 28, 34, 70, 133, 66, 49, 209},
{26, 62, 208, 158, 179, 59, 5, 140, 153, 0, 149, 4, 45, 232, 163, 90},
{169, 119, 218, 48, 135, 241, 71, 79, 124, 19, 224, 137, 222, 57, 180, 8},
{43, 242, 245, 127, 109, 145, 125, 181, 82, 35, 152, 129, 156, 89, 63, 253},
{147, 32, 231, 120, 174, 138, 188, 126, 219, 18, 111, 51, 205, 240, 31, 201},
};

unsigned char s4[16][16] = {
{68, 13, 168, 147, 75, 190, 160, 156, 207, 212, 255, 113, 153, 182, 24, 21},
{92, 53, 78, 22, 219, 61, 192, 138, 214, 103, 210, 243, 177, 124, 191, 65},
{73, 116, 221, 220, 152, 5, 238, 112, 213, 62, 172, 184, 223, 88, 253, 169},
{56, 39, 55, 9, 105, 20, 241, 227, 126, 51, 134, 185, 121, 81, 70, 161},
{6, 93, 173, 201, 94, 232, 180, 115, 162, 150, 101, 0, 205, 130, 49, 122},
{244, 242, 23, 159, 85, 84, 229, 4, 131, 179, 96, 66, 2, 176, 199, 42},
{52, 83, 166, 120, 58, 99, 57, 11, 32, 29, 109, 251, 236, 27, 128, 193},
{247, 127, 234, 186, 1, 119, 34, 102, 203, 165, 30, 132, 41, 139, 100, 123},
{36, 228, 208, 235, 111, 136, 17, 95, 91, 140, 10, 59, 151, 77, 63, 97},
{141, 110, 37, 71, 144, 72, 40, 252, 200, 7, 89, 202, 8, 33, 148, 183},
{230, 158, 175, 222, 28, 170, 76, 246, 82, 48, 237, 50, 226, 74, 206, 35},
{38, 178, 18, 19, 204, 86, 145, 44, 167, 149, 133, 14, 16, 98, 90, 64},
{114, 245, 189, 164, 217, 135, 80, 46, 154, 12, 187, 171, 248, 225, 106, 107},
{108, 239, 188, 163, 54, 25, 15, 216, 240, 87, 60, 26, 142, 174, 118, 146},
{69, 198, 67, 195, 137, 143, 197, 125, 43, 215, 224, 254, 231, 104, 218, 129},
{79, 194, 181, 31, 249, 3, 45, 157, 211, 209, 196, 117, 233, 155, 47, 250},
};

unsigned char s5[16][16] = {
{248, 71, 251, 220, 40, 221, 4, 148, 216, 242, 35, 48, 143, 182, 49, 82},
{20, 119, 132, 149, 244, 135, 240, 131, 108, 84, 195, 61, 33, 197, 46, 252},
{22, 14, 118, 196, 120, 231, 29, 77, 204, 126, 80, 229, 213, 86, 106, 75},
{1, 2, 52, 238, 100, 76, 67, 166, 162, 5, 167, 34, 210, 3, 70, 7},
{128, 103, 28, 217, 163, 65, 41, 170, 208, 97, 139, 38, 214, 36, 83, 151},
{192, 168, 9, 124, 161, 190, 175, 91, 111, 8, 215, 205, 133, 147, 117, 88},
{137, 102, 72, 186, 194, 243, 206, 55, 169, 73, 15, 44, 236, 223, 19, 96},
{45, 233, 141, 160, 185, 172, 90, 193, 179, 105, 224, 10, 56, 145, 26, 142},
{227, 202, 199, 115, 130, 87, 18, 189, 112, 94, 114, 110, 178, 153, 37, 127},
{254, 25, 89, 140, 154, 62, 50, 81, 30, 174, 209, 171, 138, 237, 64, 225},
{249, 17, 180, 95, 158, 247, 0, 69, 239, 11, 152, 39, 121, 59, 129, 159},
{78, 198, 85, 54, 177, 176, 207, 164, 157, 47, 60, 31, 43, 136, 32, 6},
{218, 12, 68, 241, 123, 13, 51, 93, 104, 63, 200, 57, 74, 226, 101, 245},
{144, 116, 232, 255, 58, 24, 183, 234, 188, 201, 98, 191, 203, 53, 211, 187},
{16, 146, 99, 109, 107, 156, 173, 219, 246, 23, 66, 79, 21, 165, 125, 184},
{92, 134, 253, 27, 250, 42, 230, 228, 222, 113, 181, 150, 235, 212, 155, 122},
};

unsigned char s6[16][16] = {
{246, 57, 59, 96, 169, 81, 107, 165, 251, 199, 254, 142, 128, 138, 205, 228},
{13, 134, 145, 146, 94, 6, 176, 95, 61, 172, 186, 167, 86, 98, 54, 231},
{72, 248, 97, 67, 178, 110, 136, 1, 245, 49, 140, 218, 11, 4, 62, 250},
{232, 78, 215, 227, 160, 170, 184, 64, 68, 58, 30, 45, 127, 210, 119, 39},
{177, 175, 88, 144, 211, 225, 233, 24, 80, 249, 150, 226, 8, 34, 33, 74},
{42, 171, 183, 244, 53, 194, 168, 27, 52, 188, 179, 51, 46, 204, 129, 20},
{190, 193, 101, 182, 90, 158, 73, 44, 216, 118, 220, 93, 85, 106, 14, 25},
{234, 133, 91, 63, 76, 148, 237, 209, 23, 32, 102, 115, 223, 135, 139, 60},
{82, 123, 141, 40, 84, 187, 195, 109, 103, 241, 161, 16, 252, 75, 229, 10},
{155, 236, 108, 200, 120, 206, 203, 114, 7, 208, 180, 174, 41, 50, 79, 181},
{36, 137, 214, 221, 253, 189, 255, 153, 130, 17, 198, 212, 99, 31, 126, 19},
{166, 3, 92, 71, 29, 222, 9, 235, 111, 116, 151, 112, 143, 124, 154, 156},
{240, 131, 113, 15, 37, 247, 66, 219, 173, 121, 201, 122, 238, 162, 87, 22},
{89, 159, 26, 197, 224, 213, 100, 47, 202, 243, 28, 65, 35, 125, 132, 70},
{242, 152, 38, 185, 5, 163, 239, 191, 55, 0, 21, 77, 207, 149, 56, 105},
{164, 2, 192, 12, 104, 43, 217, 117, 157, 69, 48, 18, 230, 83, 147, 196},
};

unsigned char s7[16][16] = {
{39, 90, 146, 214, 187, 166, 160, 112, 57, 137, 44, 12, 91, 97, 164, 86},
{199, 84, 99, 174, 64, 96, 58, 37, 4, 98, 103, 95, 121, 162, 108, 56},
{140, 5, 135, 42, 100, 126, 189, 157, 224, 148, 206, 185, 75, 218, 106, 20},
{36, 232, 212, 81, 33, 230, 119, 177, 79, 71, 68, 243, 70, 153, 252, 175},
{208, 231, 211, 94, 130, 192, 66, 181, 249, 72, 193, 131, 201, 210, 182, 41},
{152, 6, 142, 89, 120, 2, 171, 194, 63, 73, 138, 253, 15, 8, 31, 129},
{180, 165, 117, 40, 43, 155, 221, 109, 195, 141, 139, 7, 23, 1, 14, 9},
{101, 179, 227, 250, 67, 133, 38, 170, 225, 32, 16, 207, 172, 198, 65, 111},
{196, 244, 105, 219, 122, 55, 107, 88, 77, 78, 223, 46, 53, 173, 236, 191},
{184, 17, 245, 147, 34, 93, 228, 248, 128, 11, 24, 25, 22, 255, 13, 239},
{116, 234, 19, 47, 61, 10, 110, 216, 254, 3, 241, 168, 80, 113, 144, 220},
{217, 200, 158, 197, 118, 161, 163, 222, 45, 51, 240, 186, 229, 29, 114, 102},
{150, 178, 0, 50, 204, 149, 151, 237, 136, 21, 82, 246, 124, 60, 215, 143},
{62, 125, 115, 167, 54, 123, 49, 202, 85, 226, 154, 28, 132, 190, 156, 159},
{30, 251, 169, 27, 52, 48, 233, 205, 134, 76, 127, 26, 74, 209, 188, 87},
{176, 242, 18, 235, 213, 145, 69, 35, 183, 92, 247, 238, 83, 203, 59, 104},
};
    unsigned char c;
    int i, str, col, mask = 15; 
    s[0] = s0;
    s[1] = s1;
    s[2] = s2;
    s[3] = s3;
    s[4] = s4;
    s[5] = s5;
    s[6] = s6;
    s[7] = s7;
    for (i = 0; i < size; i++) {
        c = block[i];
        str = c >> (size / 2);
        col = c & mask; 
        c = s[i][str][col];
        block[i] = c;
    }
    if (s) free(s);
}

void reverse(unsigned char *block, int size) 
{
    int i;
    int p[] ={5,3,6,2,4,0,7,1};
    unsigned char *rev = calloc(size, sizeof(unsigned char));
    for (i = 0; i < size; i++) {
        rev[p[i]] = block[i];
    }
    memcpy(block, rev, size * sizeof(unsigned char));
    if (rev) free(rev);
}

unsigned char *f(unsigned char *block, int size,  unsigned char *key)
{
    int i;
    unsigned char *cip = calloc(size, sizeof(unsigned char));
    for (i = 0; i < size; i++) {
        cip[i] = block[i] ^ key[i];
    }
     sbox(cip, size);
     reverse(cip, size);
     return cip;
}



void cryptb (unsigned char *block, unsigned char **keys, unsigned char *left, unsigned char *right, unsigned char *cf, unsigned char *new_left)
{
    int i, j;
    for (i = 0; i < rounds; i++) {
        memcpy(left, block, block_size / 2);
        memcpy(right, block + block_size / 2, block_size / 2);
        cf = f(left, block_size / 2, keys[i]);
        for (j = 0; j < block_size / 2; j++) new_left[j] = right[j] ^ cf[j];
        memcpy(right, left, block_size / 2);
        memcpy(left, new_left, block_size / 2);
        memcpy(block, left, block_size / 2);
        memcpy(block + block_size / 2, right, block_size / 2);
        if (cf) free(cf);
    }
}

void decrypt(unsigned char *block, unsigned char **keys, unsigned char *left, unsigned char *right, unsigned char *cf, unsigned char *new_left)
{
    int i, j;
    for (i = rounds - 1; i >= 0; i--) {
        memcpy(left, block, block_size / 2);
        memcpy(right, block + block_size / 2, block_size / 2);
        cf = f(right, block_size / 2, keys[i]);
        for (j = 0; j < block_size / 2; j++) new_left[j] = left[j] ^ cf[j];
        memcpy(left, right, block_size / 2);
        memcpy(right, new_left, block_size / 2);
        memcpy(block, left, block_size / 2);
        memcpy(block + block_size / 2, right, block_size / 2);
        if (cf) free(cf);
    }
}

void free_arr(unsigned char **a)
{
    if (a && *a) {
        free(*a);
        *a = NULL;
    }
}


int main(int argc, char *argv[])
{
    if (argc <= 2) {
        if (argc  < 2) {
            printf("Too few arguments\n");
            exit(1);
        }
        if (!(strcmp(argv[1], "-help")) || !(strcmp(argv[1], "-h"))) execlp("cat", "cat", "HELP", NULL);
        else {
            printf("Bad arguments\n");
            exit(1);
        }
    }

    int mode = 0; 
    if (!(strcmp(argv[1], "-e")) || !(strcmp(argv[1], "-encrypt"))) {
        mode = 0;
    } else if (!(strcmp(argv[1], "-d")) || !(strcmp(argv[1], "-decrypt"))) {
            mode = 1;
    } else {
        printf("Bad arguments\n");
        exit(1);
    }
    unsigned char *block = calloc(block_size, sizeof(unsigned char));
    unsigned char *key = calloc(key_size, sizeof(unsigned char));
    unsigned char *left = calloc(block_size / 2, sizeof(unsigned char));
    unsigned char *right = calloc(block_size / 2, sizeof(unsigned char));
    unsigned char *cf = NULL;
    unsigned char *new_left = calloc(block_size / 2, sizeof(unsigned char));
    unsigned char **keys;
    unsigned char *p = NULL;
    char *str;
    int i, rc = 0, wc = 0, last_rc = 0;
    long x = 0;
    int fd1 = open(argv[2], O_RDONLY), fd2 = open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd1 == -1 || fd2 == -1) {
        printf("Bad file name\n");
        exit(1);
    }
    char c[3], *eptr = NULL;
    str = getstr();
    str[2 * key_size] = '\0';
    for (i = 0; i < key_size * 2; i+=2) {
        c[0] = str[i];
        c[1] = str[i + 1];
        c[2] = '\0';
        x = strtol(c, &eptr, 16);
        if (*eptr != '\0') {
            printf("Bad key\n");
            exit(1);
        }
        key[i / 2] = (unsigned char)x;
    }
    keys = keygen(key, rounds);
    memset(block, '\n', block_size);
    p = block;
    while ((rc = read(fd1, p, block_size - rc)) > 0) {
            p+=rc;
            if (rc == block_size) {
                if (mode == 0) {
                    cryptb(block, keys, left, right, cf, new_left);
                } else decrypt(block, keys, left, right, cf, new_left);
                p = block;
                wc =0;
                while ((wc = write(fd2, p, block_size - wc)) > 0) {
                    p+=wc;
                    if (wc == block_size) {
                        p = block;
                        rc = 0;
                        memset(block, '\n', block_size);
                        break;
                    }
                }
            }
            last_rc = rc; 
    }
    if (last_rc != 0) {
        if (mode == 0) {
            cryptb(block, keys, left, right, cf, new_left);
        } else decrypt(block, keys, left, right, cf, new_left);
        p = block;
        wc = 0;
        while ((wc = write(fd2, p, block_size - wc)) > 0) {   
            p+=wc;
            if (wc == block_size) break;
        }
    }

    free_arr(&block);
    free_arr(&left);
    free_arr(&right);
    free_arr(&key);
    free_arr(&cf);
    free_arr(&new_left);
    if (str) {
        free(str);
        str = NULL;
    }
    for (i = 0; i < rounds; i++) {
        if (keys[i]) free(keys[i]);
    }
    if (keys) {
        free(keys);
        keys = NULL;
    }
    close(fd1);
    close(fd2);
    printf("\n");
    return 0;
}
