#pragma once
#include <stdio.h>


#define DETAIL_PRINT
#define BIN_BIT_SAVE


#define CHECK_RETURN_0(pointer)  if(!pointer) {return 0;}
#define CHECK_RETURN_NULL(pointer) if(!pointer){ return NULL;}
#define CHECK_MSG_RETURN_0(pointer,str) if(!pointer){ printf(#str"\n"); return 0;}


#define CHECK_0_MSG_CLOSE_FILE(value,fp,str) if(value == 0){ printf(#str"\n"); fclose(fp); return 0;}
#define CHECK_NULL__MSG_CLOSE_FILE(value,fp,str) if(!value){ printf(#str "\n"); fclose(fp); return 0;}
#define MSG_CLOSE_RETURN_0(fp,str)  printf(#str"\n"); fclose(fp); return 0;
