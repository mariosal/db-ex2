#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/BF.h"
#include "../include/sorted.h"
#define fileName "heapFile"

void insert_Entries(int fd) {
    int count=0;
 struct Record record;
  memset(&record, 0, sizeof(struct Record));
  while (scanf("%d,\"%14[^\"]\",\"%19[^\"]\",\"%24[^\"]\"", &record.id, record.name, record.surname, record.city) == 4) {
       if(Sorted_InsertEntry(fd,record)!=0){
        printf("failed to insert a Record!!!!!!!!!!\n");
        
        }
}
   
}


int main(int argc, char **argv) {
    
    int fd;
    int fieldNo;
   
    BF_Init();
    
    //create heap file
    if(Sorted_CreateFile(fileName) == -1  )
        printf("Error creating file!\n");
    
    fd = Sorted_OpenFile(fileName);
    if( fd == -1  )
        printf("Error opening file!\n");
    insert_Entries(fd);
    //closing file so it can be oppened in sort file 
    
    if(Sorted_CloseFile(fd)<0)
       printf("Error Closing file\n"); 
    
    
    
    /*
    
    //sort heap file using 2-way merge-sort
    
    if(Sorted_SortFile(fileName,0) == -1  )
        printf("Error sorting file!\n");
    
    if(Sorted_checkSortedFile("heapFileSorted0",0) == -1  )
        printf("Error sorting file!\n");
    
    //get all entries with value
    //char value[20];
    //strcpy(value, "Keratsini");
    fieldNo = 0;
    int value = 11903588;
    
    fd = Sorted_OpenFile("heapFileSorted0");
    if( fd == -1  )
        printf("Error opening file!\n");
    
    Sorted_GetAllEntries(fd,&fieldNo,&value);
    
    */
    return EXIT_SUCCESS;
}
