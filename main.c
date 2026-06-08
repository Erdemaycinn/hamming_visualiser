#include <alloca.h>
#include <stdatomic.h>
#include <stdint.h>
#include <sys/types.h>
#define TB_IMPL
#include "termbox2.h"
#include <stdio.h>  // snprintf kullanmak için
#include <string.h> // strlen kullanmak içina,

const int WIDTH_LIMIT = 50;
const int HEIGHT_LIMIT = 20;

const char *SIZE_MESSAGE = "canvas is not big enough";
const char *GREETER_MESSAGE = "Error Correction Visualiser";


typedef struct{
    int value;
    uint16_t bgColor;
    uint16_t fgColor;
}HammingData;

HammingData hammingData[32];
HammingData hammingDataClone[32];
enum current_state {
    TYPING_DATA,
    TRAVERSAL,
    NORMAL
};
enum current_menu {
    GREETING,
    HAMMING
};

int bitLength = 0;
int currX = 0, currY = 0;
int flip = 0;

enum current_menu curr_menu = GREETING;
enum current_state curr_state= NORMAL;

char inputText[30];
int restored = 0;
int errorindex;
int exitFlag = 0;
int msg_x;
char restoremsg1[50];
char restoremsg2[50];
char restoremsg3[50];



void handleEvents() {
    struct tb_event ev;
    tb_poll_event(&ev);
    if (ev.type == TB_EVENT_KEY) {
        if (ev.ch == 'q'){
            exitFlag = 1;



        }else if (curr_menu == GREETING){
            if(ev.key == TB_KEY_ENTER){
                curr_menu = HAMMING;
            }

        }else if (curr_menu == HAMMING){
            switch (curr_state) {
            case NORMAL:

                if (ev.ch == 'E'|| ev.ch == 'e')
                    curr_state = TYPING_DATA;
                if (ev.ch == 'B'|| ev.ch == 'b')
                    curr_menu = GREETING;
                if (ev.ch == 'T'|| ev.ch == 't'){
                    if(!bitLength){return;}
                    curr_state = TRAVERSAL;
                }
                break;

            case TYPING_DATA:

                if (ev.key == TB_KEY_ESC){
                    curr_state = NORMAL;
                }else if(ev.key == TB_KEY_ENTER){
                    curr_state = NORMAL;
                }else if (ev.ch == '1'){
                if(strlen(inputText)>25){return;}
                    inputText[strlen(inputText)] = '1';
                    inputText[strlen(inputText)+1] = '\0';
                }else if (ev.ch == '0'){
                    if(strlen(inputText)>25){return;}
                    inputText[strlen(inputText)] = '0';
                    inputText[strlen(inputText)+1] = '\0';
                }else if (ev.key == TB_KEY_BACKSPACE){
                    if(strlen(inputText)==0){return;}
                    inputText[strlen(inputText)] = '\0';
                }else if (ev.key == TB_KEY_BACKSPACE2){
                    if(strlen(inputText)==0){return;}
                    inputText[strlen(inputText)-1] = '\0';
                }
                break;

            case TRAVERSAL:

                if (ev.key == TB_KEY_ARROW_UP){
                    if(!currY){return;}
                    currY--;
                    flip = 0;
                } else if (ev.key == TB_KEY_ARROW_DOWN){
                    switch (bitLength) {
                    case 8:
                        if(currY >= 1){return;}
                    break;
                    case 16: 
                        if(currY >= 3){return;}
                    break;
                    case 32:
                        if(currY >= 3){return;}
                    break;
                    }
                    currY++;
                    flip = 0;
                }else if (ev.key == TB_KEY_ARROW_RIGHT){
                    switch (bitLength) {
                    case 8:
                        if(currX >= 3){return;}
                        break;
                        case 16: 
                            if(currX >= 3){return;}
                        break;
                        case 32:
                            if(currX >= 7){return;}
                        break;
                        }
                        currX++;
                        flip = 0;

                }else if (ev.key == TB_KEY_ARROW_LEFT){
                    if(!currX){return;}
                    currX--;
                    flip = 0;
                }else if (ev.key == TB_KEY_ESC){
                    curr_state= NORMAL;
                }else if (ev.key == TB_KEY_ENTER){
                    flip = !flip;
                    curr_state = NORMAL;
                }
            }
        }
    }
}

void checkSize() {
    while (tb_width() < WIDTH_LIMIT || tb_height() < HEIGHT_LIMIT) {
        if (exitFlag) {return;}

        tb_clear();

        msg_x = (tb_width() - strlen(SIZE_MESSAGE)) / 2;
        tb_print(msg_x, tb_height() / 2 - 1, TB_WHITE, TB_DEFAULT, SIZE_MESSAGE);
        char termSizeNtf_p1[50], termSizeNtf_p2[50], termSizeNtf_p3[50];
        strcpy(termSizeNtf_p2, " x ");
        snprintf(termSizeNtf_p1, sizeof(termSizeNtf_p1), "%d", tb_width());
        snprintf(termSizeNtf_p3, sizeof(termSizeNtf_p3), "%d", tb_height());
        msg_x = (tb_width() - strlen(termSizeNtf_p1) - strlen(termSizeNtf_p2) - strlen(termSizeNtf_p3)) / 2;
        tb_print(msg_x, tb_height() / 2, (tb_width() < WIDTH_LIMIT) ? TB_RED : TB_GREEN, TB_DEFAULT, termSizeNtf_p1);
        msg_x += strlen(termSizeNtf_p1);
        tb_print(msg_x, tb_height() / 2, TB_WHITE, TB_DEFAULT, termSizeNtf_p2);
        msg_x += strlen(termSizeNtf_p2);
        tb_print(msg_x, tb_height() / 2, (tb_height() < HEIGHT_LIMIT) ? TB_RED : TB_GREEN, TB_DEFAULT, termSizeNtf_p3);
        tb_present();
        handleEvents();
    }
}

void greeter() {
    tb_clear();

    msg_x = (tb_width()-strlen(GREETER_MESSAGE))/2;
    tb_print(msg_x,tb_height()/3,TB_GREEN,TB_DEFAULT,GREETER_MESSAGE);
    char msg[40] = " press enter to proceed ";
    msg_x = (tb_width()-strlen(msg))/2;
    tb_print(msg_x,tb_height()*2/3,TB_BLACK,TB_WHITE,msg);
}

void processHamming(){
    
    if(strlen(inputText)<5){
        bitLength = 8;
    }else if(strlen(inputText)<12){
        bitLength = 16;
    }else if(strlen(inputText)<27){
        bitLength = 32;
    }

    hammingData[0].value = 0;
    hammingData[0].bgColor = TB_MAGENTA;
    hammingData[0].fgColor = TB_DEFAULT;
    
    // Places input to hamming data,
    for (int i = 1, k = 0; i < 32; i++){
        hammingData[i].fgColor = TB_DEFAULT;
        if(!(i & (i - 1))){
            hammingData[i].value = 0;
            hammingData[i].bgColor = TB_GREEN;
            continue; // skipping 0 and 2's powers
        }
        if (k < strlen(inputText)){
            hammingData[i].value = (inputText[k++] == '1')? 1:0;
        }else{
            hammingData[i].value = 0; // fill blanks with 0 
        }
    }
    for(int i=1;i<bitLength;i++){
        if(i&1){
            hammingData[1].value ^= hammingData[i].value;
        }
        if((i>>1)&1){
            hammingData[2].value ^= hammingData[i].value;
        }
        if((i>>2)&1){
            hammingData[4].value ^= hammingData[i].value;
        }
        if((i>>3)&1){
            hammingData[8].value ^= hammingData[i].value;
        }
        if((i>>4)&1){
            hammingData[16].value ^= hammingData[i].value;
        }   
    }
    for(int i=1;i<bitLength;i++){
        hammingData[0].value^=hammingData[i].value;
    }
    memcpy(hammingDataClone, hammingData, sizeof(hammingData));
}

void writeMatrix(HammingData data[], int centerX,int centerY, int bitLength,int select){
    int charXcord, charYcord, index;
    
    switch (bitLength) {
        case 8:
            if (centerX < 2){
                centerX = 2;
            }if (centerY < 5){
                centerY = 5;
            }
            for(int y = 0; y < 2;y++){
                for(int x = 0; x < 4;x++){
                    if(select && (curr_state == TRAVERSAL)){
                        data[currY*4+currX].fgColor |= TB_BLINK;
                        data[currY*4+currX].fgColor |= TB_BOLD;
                        data[currY*4+currX].bgColor |= TB_BLINK;
                    }
                    index = y*4+x;
                    charXcord = centerX-2+x;
                    charYcord = centerY-1+y;
                    tb_set_cell(charXcord,charYcord,(data[index].value)?'1':'0',data[index].fgColor,data[index].bgColor);
                } 
            }
        break;

        case 16:
            if (centerX < 2){
                centerX = 2;
            }if (centerY < 7){
                centerY = 7;
            }
            for(int y = 0; y < 4;y++){
                for(int x = 0; x < 4;x++){
                    if(select && (curr_state == TRAVERSAL)){
                        data[currY*4+currX].fgColor |= TB_BLINK;
                        data[currY*4+currX].fgColor |= TB_BOLD;
                        data[currY*4+currX].bgColor |= TB_BLINK;
                    }
                    index = y*4+x;
                    charXcord = centerX-2+x;
                    charYcord = centerY-2+y;
                    tb_set_cell(charXcord,charYcord,(data[index].value)?'1':'0',data[index].fgColor,data[index].bgColor);
                } 
            }
        break;
        case 32:
            if (centerX < 4){
                centerX = 4;
            }if (centerY < 7){
                centerY = 7;
            }
            for(int y = 0; y < 4;y++){
                for(int x = 0; x < 8;x++){
                    if(select && (curr_state == TRAVERSAL)){ 
                        data[currY*8+currX].fgColor |= TB_BLINK;
                        data[currY*8+currX].fgColor |= TB_BOLD;
                        data[currY*8+currX].bgColor |= TB_BLINK;
                    }
                    index = y*8+x;
                    charXcord = centerX-4+x;
                    charYcord = centerY-2+y;
                    tb_set_cell(charXcord,charYcord,(data[index].value)?'1':'0',data[index].fgColor,data[index].bgColor);
                }
            }
        break;
    }
}
void restoreHamming(){
    int p1=0,p2=0,p3=0,p4=0,p5=0,gp=0;
        for(int i = 0;i < bitLength;i++){
        gp^=hammingDataClone[i].value;
        if(i&1){
            p1 ^= hammingDataClone[i].value;
        }
        if((i>>1)&1){
            p2 ^= hammingDataClone[i].value;
        }
        if((i>>2)&1){
            p3 ^= hammingDataClone[i].value;
        }
        if((i>>3)&1){
            p4 ^= hammingDataClone[i].value;
        }
        if((i>>4)&1){
            p5 ^= hammingDataClone[i].value;
        }
    }
    errorindex = p1 | (p2 << 1) | (p3 << 2) | (p4 << 3) | (p5 << 4);
    
    snprintf(restoremsg1, sizeof(restoremsg1), "Binary Format (p5,p4,p3,p2,p1): %d%d%d%d%d",p5,p4,p3,p2,p1);
    snprintf(restoremsg2, sizeof(restoremsg2), "Error Index: %d",errorindex);
    snprintf(restoremsg3, sizeof(restoremsg3), "Global Parity: %d",gp);
}


void hammingMenu() {
    tb_clear();

    tb_print(1,1,TB_RED,TB_DEFAULT,"B");
    tb_print(2, 1,TB_WHITE, TB_DEFAULT, "ack");
    tb_print(7,1,TB_RED,TB_DEFAULT,"E");
    tb_print(8, 1,TB_WHITE, TB_DEFAULT, "nter new data");
    tb_print(23,1,TB_RED,TB_DEFAULT,"ESC");
    tb_print(27,1,TB_WHITE,TB_DEFAULT," normal mode");
    tb_print(1,4,TB_WHITE,TB_DEFAULT,"input: ");
    tb_print(1,2, TB_RED, TB_DEFAULT,"T");
    tb_print(2,2, TB_WHITE, TB_DEFAULT,"reversal");
    tb_print(12,2 ,TB_RED ,TB_DEFAULT, "ENTER");
    tb_print(18,2, TB_WHITE,TB_DEFAULT, "toggle bit \"during traversal\"");
     
    tb_print(8,4,TB_WHITE,TB_DEFAULT,inputText);
    tb_print(tb_width()-15,1,TB_WHITE,TB_DEFAULT,"mode:");
    if(flip) {
        char msg[50];
        snprintf(msg, sizeof(msg),"X:%d , Y:%d (%d. bit) is flipped",currX+1,currY+1,(bitLength == 32)?currY*8+currX:currY*4+currX);
        tb_print(1,5,TB_WHITE,TB_DEFAULT,msg);
    }
    switch (curr_state) {
        case TYPING_DATA:
            tb_print(8+strlen(inputText),4,TB_WHITE,TB_DEFAULT,"█");
            tb_print(tb_width()-9, 1, TB_GREEN, TB_DEFAULT, "Typing");
            break;
        case NORMAL:
            tb_print(tb_width()-9, 1, TB_BLUE, TB_DEFAULT, "Normal");
            break;
        case TRAVERSAL:
            tb_print(tb_width()-9, 1, TB_MAGENTA, TB_DEFAULT, "Traversal");
            break;
    }

    if (strlen(inputText)){
        processHamming();
        if(flip){
            hammingDataClone[(bitLength == 32)?currY*8+currX:currY*4+currX].value = !hammingDataClone[(bitLength == 32)?currY*8+currX:currY*4+currX].value;
        }
        restoreHamming();
        writeMatrix(hammingData, tb_width()/4, tb_height()/2, bitLength, 1);
        writeMatrix(hammingDataClone, tb_width()/4*3,tb_height()/2,bitLength, 1);
    }
    tb_print(tb_width()/4-5,6,TB_WHITE|TB_BOLD,TB_DEFAULT,"Before Flip"); 
    tb_print(tb_width()/4*3-5,6,TB_WHITE|TB_BOLD,TB_DEFAULT,"After Flip");
    tb_print(1, tb_height()-5, TB_WHITE, TB_DEFAULT, restoremsg1);
    tb_print(1, tb_height()-4, TB_WHITE, TB_DEFAULT, restoremsg2);
    tb_print(1, tb_height()-3, TB_WHITE, TB_DEFAULT, restoremsg3);
    tb_print(1, tb_height()-2, (!errorindex)?TB_GREEN:TB_RED, TB_DEFAULT, (!errorindex)?"No Error Found":"Error Found");

}


int main() {
    tb_init();
    while (!exitFlag) {
        

        switch (curr_menu) {
        case 0:
            greeter();
            break;
        case 1:
            hammingMenu();
            break;
        }

        checkSize();
        tb_present();

        handleEvents();
    }
    tb_shutdown();
    return 0;
}
