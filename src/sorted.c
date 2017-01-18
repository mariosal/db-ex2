#include "../include/sorted.h"
#include "../include/BF.h"
#include <string.h>

int max_records_in_block(void){
    /* epistrefei to megisto plithos egrafwn se ena block wste na sigrinw ton arithmo twn egrafwn tou block me to megisto kai na kserw     an xwrane alles egrafes se afto to block*/
    return ( (BLOCK_SIZE-sizeof(int) )/sizeof(Record)   );

}






int Sorted_CreateFile(char * filename ){
    int is_sort_file=1;
    int fd=-1;
    void * block=NULL;
    if(BF_CreateFile(filename)<0){
        BF_PrintError("cannot create bf file\n");
        return -1;
    }
    
    if((fd=BF_OpenFile(filename))<0){
        BF_PrintError("cannot open bf file\n");
        return -1;
    }
    
    if(BF_AllocateBlock(fd)<0){
        BF_PrintError("cannot allocate block in bf file\n");
        return -1;
    }
    
    if(BF_ReadBlock(fd,0,&block)<0){
        BF_PrintError("cannot read block 0 in bf file\n");
        return -1;
    }
    
    memcpy(block,(int *)&is_sort_file,sizeof(int));
    
    if(BF_WriteBlock(fd,0)<0){
        BF_PrintError("cannot write block 0 in bf file\n");
        return -1;
    }

    if(BF_CloseFile(fd)<0){
        BF_PrintError("cannot close file\n");
        return -1;
    }
    return 0; 
 
}


int Sorted_OpenFile(char * filename){
    int fd;
    int is_sort_file=-1;
    void * block;
    if(( fd=BF_OpenFile(filename)) <0){
        BF_PrintError("cannot open file\n");
        return -1;
    }
    if(BF_ReadBlock(fd,0,&block)<0){
        BF_PrintError("cannot read block 0\n");
        return -1;
    }
    
    memcpy((int*)&is_sort_file,block,sizeof(int));
    
    if(is_sort_file!=1) 
        return -1;


    return 0;


}



int Sorted_CloseFile(int fileDesc){
    if(BF_CloseFile(fileDesc)<0){
        BF_PrintError("cannot close file \n");
        return -1;
    }
    return 0;

}




int Sorted_IsertEntry(int fileDesc,Record record){



}


void Sorted_Sort_File(char * fileName, int fieldNo){


}


void Sorted_checkSortedFile(char * filename, int fieldNo){




}





void Sorted_GetAllEntries(int fileDesc,int * fieldNo,void * value){

}


