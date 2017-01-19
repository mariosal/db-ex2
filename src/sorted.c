#include "../include/sorted.h"
#include "../include/BF.h"
#include <stdio.h>

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
    int is_sort_file = -1;
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
    
    if(is_sort_file != 1) 
        return -1;


    return fd;


}



int Sorted_CloseFile(int fileDesc){
    
    if(BF_CloseFile(fileDesc)<0){
        BF_PrintError("cannot close file \n");
        return -1;
    }
    return 0;

}




int Sorted_IsertEntry(int fileDesc,Record record){
    int num_of_records_in_block;
    int blocks_num;
    int tail;
    void * block_ptr;
    int offset;
    blocks_num = BF_GetBlockCounter(fileDesc);
    
    if(blocks_num < 0 ){
        BF_PrintError("cannot get block counter\n");
        return -1;
    }
    
    else if (blocks_num == 0){
        fprintf(stderr,"file has not been initialized!");
        return -1;
    }
    
    else if (blocks_num == 1){
        /* an iparxei 1 block tote afto einai to block 0 opote prepei na dimiourgithei neo block gia tis eggrafes*/
        
        if(BF_AllocateBlock(fileDesc) <0 ){
            BF_PrintError("cannot allocate block\n)"
            return -1;            
        }

        tail = BF_GetBlockCounter(fileDesc);
        
        if(tail <0){
            BF_PrintError("cannot get block counter\n");
            return -1;
        }
        
        tail--; // o arithmos tou teleutaiou block einai ta block-1 
        
        if(BF_ReadBlock(fileDesc,0,&block_ptr)<0){
            BF_PrintError("cannot read block \n");
            return -1;   
        }

        /* set new tail at block 0  -> |is_sort_file|tail|*/

        memcpy(block+sizeof(int),(int *)&tail,sizeof(int));

        if(BF_WriteBlock(fileDesc,0) < 0){
            BF_PrintError("cannot write to block 0\n");
            return -1;
        }
        
        /* go to tail block and insert record -> num_of_records|record|record.. */

        if(BF_ReadBlock(fileDesc,tail,&block_ptr)<0){
            BF_PrintError("cannot get block counter\n");
            return -1;   
        }

        /* num of records in this block now is 1*/
        
        num_of_records_in_block = 1;
        
        memcpy(block_ptr  ,(int *)&num_of_records_in_block,sizeof(int));

        /* and now isert the record in block*/

        memcpy(block_ptr+sizeof(int),(record *)&record,sizeof(record);

        /* write changes to block*/

        if(BF_WriteBlock(fileDesc,tail) < 0){
            BF_PrintError("cannot write to block 0\n");
            return -1;
        }

        return 0;
 
    }
    
    else{
    /* alliws an iparxoun block diavase apo to block 0 pio einai to tail kai dokimase se afto na valeis tin eggrafi
     *  an den iparxei xwros sto tail block desmeuse ena kainourgio kai enimerwse ton dixti tail tou block 0 */
        
        if(BF_ReadBlock(fileDesc,0,&block_ptr)<0){
            BF_PrintError("cannot read block 0\n");
            return -1;   
        }
        
        // enimerwsi tou tail apo to block 0
        
        memcpy((int *)tail,block_ptr+sizeof(int),sizeof(int) );
        
        
        //pigaine sto tail block
        
        if(BF_ReadBlock(fileDesc,tail,&block_ptr)<0){
            BF_PrintError("cannot read block tail\n");
            return -1;   
        }

        /* elenxe an to tail block einai gemato me eggrafes */

        memcpy((int *)num_of_records_in_block, block_ptr, sizeof(int );

        if(num_of_records_in_block == max_records_in_block()){
            
            if( (tail=BF_AllocateBlock(fileDesc) ) < 0 ){
                BF_PrintError("cannot allocate block\n");
                return -1;
            }           
            /* enimerwsi tou tail sto block 0*/
            tail--;                 //teleutaio block einai ta block -1
            
            if(BF_ReadBlock(fileDesc,0,&block_ptr)<0){
                BF_PrintError("cannot read block 0\n");
                return -1;   
            }
   
            memcpy(block_ptr+sizeof(int),(int*)&tail,sizeof(int ));

            if(BF_WriteBlock(fileDesc,0) <0){
                BF_PrintError("cannot write to block 0\n");
                return -1;
            }
            
            /* eisagwgi tis eggrafis sto neo block */
            num_of_records_in_block = 1;

            if(BF_ReadBlock(fileDesc,tail,&block_ptr)<0){
                BF_PrintError("cannot read block\n");
                return -1;
            }
            
            memcpy(block_ptr,(int*)&num_of_records_in_block,sizeof(int));

            memcpy(block_ptr+sizeof(int),(Record*)&record),sizeof(record));

            if(BF_WriteBlock(fileDesc,tail) <0){
                BF_PrintError("cannot write block\n");
                return -1;
            }   

            return 0;
               
        }
        
        else if (num_of_records_in_block < max_records_in_block() ){
               
            offset = sizeof(int) + num_of_records_in_block * sizeof(Record);
            num_of_records_in_block++;
            
            //enimerwsi tou num_of_records
            memcpy(block,(int*)&num_of_records_in_block,sizeof(int));
            
            // eisagwgi tis eggrafis sto katallilo simeio    
            memcpy(block+offset,(Record *)&record,sizeof(record));
            
            if(BF_WriteBlock(fieldNo,tail)<0){
                BF_PrintError("cannot write to tail block\n");
                return -1;
            }

            return 0;

        }


    
    
    }



}


void Sorted_Sort_File(char * fileName, int fieldNo){
    

}


void Sorted_checkSortedFile(char * filename, int fieldNo){




}





void Sorted_GetAllEntries(int fileDesc,int * fieldNo,void * value){

}


