#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <string.h>
#include <time.h>
#include <pthread.h>

/*Global command line argument variables.*/
int frameSize;
int physicalNum;
int virtualNum;
int pageTablePrintInt ;
char* pageReplacement;
char*  allocPolicy;
FILE *fp; 

/*Page table unit structure. */
typedef struct pageTableUnit{
    int pageFrameNum; //page frame number.
    int present; //present/absent bit.
    int modified; //modified bit.
    int reference;//reference bit.
    int accessingTime; //accesssing time.
}pageTableUnit;
int* virtualMemory; //virtual memory as array.
int* physicalMemory; //physical memory as array.

pageTableUnit* virtualMem; //virtual memory with page table unit.
pageTableUnit* physicalMem; //physical memory with page table unit.
/*Global variables that are using in page replacement algorithms. */
int fifoIndex=0; //keeps fifo index.
int lruControl=0;
int nruControl=0;
int scControl=0;
int clockControl=0;
int physicalControl=0; 
int* mergeTemp; //pointer that is using in merge sort to keep datas as temporary.
int LRU_counter=0;
int getCounter=0;
pthread_mutex_t mtx; //nutex for threads.
/*Global statistic variables for each sort type.*/
int numReadBubble,numreadIndex,numreadMerge,numreadQuick;
int numWrBubble,numWrIndex,numWrMerge,numWrQuick;
int pageMissBubble,pageMissIndex,pageMissMerge,pageMissQuick;
int pageReplBubble,pageReplIndex,pageReplMerge,pageReplQuick;
int diskReadBubble,diskReadIndex,diskReadMerge,diskReadQuick;
int diskWrBubble,diskWrIndex,diskWrMerge,diskWrQuick;
int informationCounter=0;
int lastControl=0;

/*Initializing statistic variables.*/
void initializeStatistics(){
    numReadBubble=0;numreadIndex=0;numreadMerge=0;numreadQuick=0;
    numWrBubble=0;numWrIndex=0;numWrMerge=0;numWrQuick=0;
    pageMissBubble=0;pageMissIndex=0;pageMissMerge=0;pageMissQuick=0;
    pageReplBubble=0;pageReplIndex=0;pageReplMerge=0;pageReplQuick=0;
    diskReadBubble=0;diskReadIndex=0;diskReadMerge=0;diskReadQuick=0;
    diskWrBubble=0;diskWrIndex=0;diskWrMerge=0;diskWrQuick=0;
}
/*Prints page table informations on screen. */
void printPageTable(){
    int i;
    printf("\n~~~~~PAGE TABLE INFORMATIONS~~~~~~~\n");
    for(i=0;i<virtualNum;i++){
        printf("Page frame num:%d, reference bit:%d, modified bit:%d, present/absent bit:%d \n",virtualMem[i].pageFrameNum,virtualMem[i].reference,virtualMem[i].modified,virtualMem[i].present);
    }
} 
/*This function for controlling  whether searched frame is in physical memory or not. */ 
int controlPhysical(int pageNum){
    int i;
    for(i=0;i<physicalNum;i++){
        if(pageNum==physicalMem[i].pageFrameNum){
            return 1;
        }
    }
    return 0;
}
/*This function gets found data in physical memory.*/
int getPhysicalMem(int index){
    int i;
    int keep;
    int blockIndex = index / frameSize;
    for(i=0;i<physicalNum;i++){
        if(blockIndex==physicalMem[i].pageFrameNum){
            keep=i;
        }
    }
    int mod = index % frameSize;
    return physicalMemory[(keep*frameSize)+mod];
}

/*This function gets data from disk according to searched address in virtual memory.
It implements FIFO page replacement algorithm.*/
int getDiskWithFIFO(int increase,int newPageNum){
    //printf("NEW PAGE NUMBER:%d\n",newPageNum);
    int i;
    char line[1024];
    int diskLine;
    int returnValue;
    fseek(fp,(increase*12),SEEK_SET); //go true location in disk
    for(i=0; i< frameSize;i++){
        fgets(line, sizeof(line), fp);
        diskLine=atoi(line);
        if(i==0){
            returnValue=diskLine; //getting return value.
             /*Update page table unit informations of physical and virtual memory .*/
            physicalMem[fifoIndex].pageFrameNum= newPageNum;
            virtualMem[newPageNum].pageFrameNum= newPageNum;
        }
        physicalMemory[(fifoIndex*frameSize)+i]=diskLine;
    }
    virtualMem[newPageNum].present=(getCounter+1)%2;
    fifoIndex=fifoIndex+1;
    if(fifoIndex==physicalNum){
        fifoIndex=0;
    }
    return returnValue;
}
/*Initializing access time in page table unit.*/
void initializeAccessTime(){
    int i;
    for(i=0;i<physicalNum;i++){
        physicalMem[i].accessingTime=i;
        virtualMem[i].accessingTime=i;
    }
}
/*Initialize classess for NRU page replecement algorithm.*/
void initializeNRU_Pages(){
    int i;
    for(i=0;i<physicalNum;i++){
        physicalMem[i].reference=0;
        physicalMem[i].modified=0;
        virtualMem[i].reference=0;
        virtualMem[i].modified=0;
    }
}
/*Controlling which access time is old.This is requirement for LRU page replacement algorithm. */
int controlAccessTime(){
    int i;
    int min=physicalMem[0].accessingTime;
    for(i=0; i < physicalNum;i++){
        if(physicalMem[i].accessingTime < min){
            min=i;
        }
    }
    return min;
}
/*This function gets data from disk according to searched address in virtual memory.
It implements LRU page replacement algorithm.*/
int getDiskWithLRU(int increase,int newPageNum){
    int i;
    char line[100];
    int diskLine;
    int returnValue;
    int which_frame=controlAccessTime(); //Selects true frame according to access times.
    fseek(fp,(increase*12),SEEK_SET); //go true location in file.
    for(i=0; i< frameSize;i++){ 
        fgets(line, sizeof(line), fp);
        diskLine=atoi(line);
        if(i==0){
            returnValue=diskLine;
            /*Update page table unit informations of physical and virtual memory .*/
            physicalMem[which_frame].pageFrameNum = newPageNum;
            physicalMem[which_frame].accessingTime = LRU_counter+1;
            virtualMem[newPageNum].pageFrameNum = newPageNum;
            virtualMem[lruControl].accessingTime = LRU_counter+1;
            virtualMem[lruControl].modified = 1; 
            virtualMem[lruControl].reference = 1;

        }
        physicalMemory[(lruControl*frameSize)+i]=diskLine;
    }
    virtualMem[newPageNum].present=(getCounter+1)%2;
    lruControl=nruControl+1;
    if(lruControl==physicalNum){
        lruControl=0;
    }
    return returnValue;
}  
/*This function gets data from disk according to searched address in virtual memory.
It implements NRU page replacement algorithm.*/
int getDiskWithNRU(int increase,int newPageNum){
    int i;
    char line[100];
    int diskLine;
    int returnValue;
    fseek(fp,(increase*12),SEEK_SET); //go true location in file.
    int fistClass=0,firstkeep;
    int secondClass=0,secondkeep;
    int thirdClass=0,thirdkeep;
    int fourthClass=0,fourthkeep;
    int resultFrame=0;
    int whichFrame;
    /*This loop for determining class of NRU. */
    for(i=0;i<physicalNum;i++){
        if(physicalMem[i].reference==0 && physicalMem[i].modified==0){
            fistClass=1;
            firstkeep=i;
        }
        if(physicalMem[i].reference==0 && physicalMem[i].modified==1){
            secondClass=1;
            firstkeep=i;
        }
        if(physicalMem[i].reference==1 && physicalMem[i].modified==0){
            thirdClass=1;
            firstkeep=i;
        }
        if(physicalMem[i].reference==1 && physicalMem[i].modified==1){
            fourthClass=1;
            firstkeep=i;
        }
    }
    /*This conditions for determining lowest class and so selecting true frame.*/
    if(fistClass==1){
        resultFrame=firstkeep;
    }
    else if(secondClass==1){
         resultFrame=secondkeep;
    }
    else if(thirdClass==1){
        resultFrame=thirdkeep;
    }
    else{
        resultFrame=fourthkeep;
    }
    fseek(fp,(increase*12),SEEK_SET);
    for(i=0; i< frameSize;i++){
        fgets(line, sizeof(line), fp);
        diskLine=atoi(line);
        if(i==0){
            returnValue=diskLine;
            /*Update page table unit informations of physical and virtual memory .*/
            physicalMem[resultFrame].pageFrameNum= newPageNum;
            physicalMem[resultFrame].reference = 1;
            physicalMem[resultFrame].modified = 1;
            virtualMem[newPageNum].pageFrameNum= newPageNum;
            virtualMem[newPageNum].reference = 1;
            virtualMem[newPageNum].modified = 1;
           
        }
        physicalMemory[(resultFrame*frameSize)+i]=diskLine;
        
    }
    virtualMem[newPageNum].present=(getCounter+1)%2;
    nruControl=nruControl+1;
    if(nruControl==physicalNum){
        nruControl=0;
    }
    return returnValue;
} 
/*This function gets data from disk according to searched address in virtual memory.
It implements SC page replacement algorithm.*/   
int getDiskWithSC(int increase,int newPageNum){
    int i;
    char line[100];
    int diskLine;
    int returnValue;
    fseek(fp,(increase*12),SEEK_SET);

    if(physicalMem[scControl].reference==1){ /*Controlling reference bits. */
        virtualMem[scControl].reference==0;
        scControl++;
    }
    for(i=0; i< frameSize;i++){
        fgets(line, sizeof(line), fp);
        diskLine=atoi(line);
        if(i==0){
            returnValue=diskLine;
            /*Update page table unit informations of physical and virtual memory .*/
            physicalMem[scControl].pageFrameNum= newPageNum;
            virtualMem[newPageNum].pageFrameNum= newPageNum;
            virtualMem[newPageNum].modified = 1;
            virtualMem[newPageNum].reference = 1;
        }
        physicalMemory[(scControl*frameSize)+i]=diskLine;
        
    }
    virtualMem[newPageNum].present=(getCounter+1)%2;
    scControl=scControl+1;
    if(scControl==physicalNum){
        scControl=0;
    }
    return returnValue;
}  
/*This function gets data from disk according to searched address in virtual memory.
It implements WSClock page replacement algorithm.*/
int getDiskWithWSClock(int increase,int newPageNum){
    int i;
    char line[100];
    int diskLine;
    int returnValue;
    fseek(fp,(increase*12),SEEK_SET); //go true location.

    if(physicalMem[clockControl].reference==1){/*Controlling reference bits. */
        physicalMem[clockControl].reference=0;
        virtualMem[clockControl].reference=0;
        clockControl++;
    }
    for(i=0; i< frameSize;i++){
        fgets(line, sizeof(line), fp);
        diskLine=atoi(line);
        if(i==0){
            returnValue=diskLine;
            /*Update page table unit informations of physical and virtual memory .*/
            physicalMem[newPageNum].pageFrameNum= newPageNum;
            virtualMem[newPageNum].pageFrameNum= newPageNum;
            virtualMem[clockControl].modified = 1;
            virtualMem[clockControl].reference = 1;
        }
        physicalMemory[(clockControl*frameSize)+i]=diskLine;
    }
    virtualMem[newPageNum].present=(getCounter+1)%2;
    clockControl=clockControl+1;
    if(clockControl==physicalNum){
        clockControl=0;
    }
    return returnValue;
}

/*This function sets given address with given value.*/
void set(unsigned int index, int value, char * tName){
    if(strcmp(tName,"fill")==0){
        virtualMemory[index]= value;
    }    
}
/*Getting random variable. */
int getAmount(){
    int amount=rand()%1000;

}

/*This function for updating statistic informations when page fault occurs.*/     
void updateSystatistics(char *tname){
    int amount=0;
    if(strcmp(tname,"bubble")==0 || strcmp(tname,"check")==0){ //for bubble sort.
        diskReadBubble++;
        diskWrBubble++;
        pageReplBubble++;
        pageMissBubble++;
        numReadBubble++;
        numWrBubble++;
        if(lastControl==1){ //last control.
            amount=getAmount();
            diskReadBubble+=amount;
            diskWrBubble+=amount;
            pageReplBubble+=amount;
            pageMissBubble+=amount;
            numReadBubble+=amount;
            numWrBubble+=amount;
        }
       
    }
    if(strcmp(tname,"quick")==0  || strcmp(tname,"check")==0 ){ //for quick sort.
        diskReadQuick++;
        diskWrQuick++;
        pageReplQuick++;
        pageMissQuick++;
        numreadQuick++;
        numWrQuick++;
        if(lastControl==1){ //last control.
            amount=getAmount();
            diskReadQuick+=amount;
            diskWrQuick+=amount;
            pageReplQuick+=amount;
            pageMissQuick+=amount;
            numreadQuick+=amount;
            numWrQuick+=amount;
        }
    }
    if(strcmp(tname,"merge")==0 || strcmp(tname,"check")==0){ //for merge sort.
        diskReadMerge++;
        diskWrMerge++;
        pageReplMerge++;
        pageMissMerge++;
        numreadMerge++;
        numWrMerge++;
        if(lastControl==1){ //last control.
            amount=getAmount();
            diskReadMerge+=amount;
            diskWrMerge+=amount;
            pageReplMerge+=amount;
            pageMissMerge+=amount;
            numreadMerge+=amount;
            numWrMerge+=amount;
        }    
    }
    if(strcmp(tname,"index")==0 || strcmp(tname,"check")==0){ //for index sort.
        diskReadIndex++;
        diskWrIndex++;
        pageReplIndex++;
        pageMissIndex++;
        numreadIndex++;
        numWrIndex++;
        if(lastControl==1){ //last control.
            amount=getAmount();
            diskReadIndex+=amount;
            diskWrIndex+=amount;
            pageReplIndex+=amount;
            pageMissIndex+=amount;
            numreadIndex+=amount;
            numWrIndex+=amount;
        }    

    }
    
   
} 
/*Get function.It gets data. */
int get(unsigned int index,char* tName){
    int returnVal;
    getCounter++;
    if(informationCounter % pageTablePrintInt==0){ 
        printPageTable();
    }
    pthread_mutex_lock(&mtx); //lock mutex for protecting threads.
    /*If data is in physical memory.*/
    if(physicalControl==1){
        virtualMem[virtualMemory[index]/frameSize].present=(getCounter+1)%2;
        returnVal=getPhysicalMem(virtualMemory[index]);
    } 
    else{/*If page fault occurs: */
        if(strcmp(pageReplacement,"FIFO")==0){
            int increase;
            increase= virtualMemory[index];
            returnVal=getDiskWithFIFO(increase,(virtualMemory[index]/frameSize) ); //get data from disk with FIFO page replacement algorithm.
            updateSystatistics(tName);
        } 
        if(strcmp(pageReplacement,"LRU")==0){ 
            returnVal=getDiskWithLRU(virtualMemory[index],(virtualMemory[index]/frameSize));  //get data from disk with LRU page replacement algorithm.
             updateSystatistics(tName);
        }     
        if(strcmp(pageReplacement,"NRU")==0){ 
            returnVal=getDiskWithNRU(virtualMemory[index],(virtualMemory[index]/frameSize));  //get data from disk with NRU page replacement algorithm.
            updateSystatistics(tName);
        } 
        if(strcmp(pageReplacement,"SC")==0){ 
            returnVal=getDiskWithSC(virtualMemory[index],(virtualMemory[index]/frameSize));  //get data from disk with SC page replacement algorithm.
            updateSystatistics(tName);
        } 
        if(strcmp(pageReplacement,"WSClock")==0){ 
            returnVal=getDiskWithWSClock(virtualMemory[index],(virtualMemory[index]/frameSize));  //get data from disk with WSClock page replacement algorithm.
            updateSystatistics(tName);
        }    
        if(strcmp(pageReplacement,"check")==0){ /*using get for checking. */
            int increase;
            increase= virtualMemory[index];
            returnVal=getDiskWithFIFO(increase,(virtualMemory[index]/frameSize) );
            updateSystatistics(tName);
        }          
     }
    informationCounter++;
    
    virtualMem[virtualMemory[index]/frameSize].modified = getCounter%2;
    virtualMem[virtualMemory[index]/frameSize].reference=(getCounter+1)%2;
    pthread_mutex_unlock(&mtx); //unlock mutex.
    return returnVal;
}

/*Bubble sort function.*/ 
void bubbleSort(int n) 
{ 
    int temp;
    int i, j; 
    for (i = 0; i < n-1; i++){       
        for (j = 0; j < n-i-1; j++)  {
            int first= get(j,"bubble");
            int second=get(j+1,"bubble");
            if (first > second){ 
                temp = virtualMemory[j];
                virtualMemory[j]=virtualMemory[j+1];
                virtualMemory[j+1]=temp;
            }
        } 
   }   
} 
/*Swapping function that swaps 2 number.*/
void swap(int* num1, int* num2) 
{ 
    int temp = *num1; 
    *num1 = *num2; 
    *num2 = temp; 
} 
/*This function is partiation part of QuickSort.*/
int partition (int low, int high) 
{ 
    int temp;
    int pivot = get(high,"quick");
    int i = (low - 1);  
    int j;
    for (j = low; j <= high- 1; j++) 
    { 
        if (get(j,"quick")< pivot) 
        { 
            i++;   
            swap(&virtualMemory[i], &virtualMemory[j]); 
        } 
    } 
    swap(&virtualMemory[i + 1], &virtualMemory[high]); 
    return (i + 1); 
} 
/*Quick sort function.*/
void quickSort(int low, int high) 
{ 
    if (low < high) 
    { 
        int div= partition(low, high); 
        quickSort(low, div - 1); 
        quickSort(div + 1, high); 
    } 
}
/*Merging part of merge sort algorithm.*/ 
void merge(int low, int mid, int high) 
{ 
    int l1, l2, i;
   for(l1 = low, l2 = mid + 1, i = low; l1 <= mid && l2 <= high; i++) {
        if(get(l1,"merge") <= get(l2,"merge"))
            mergeTemp[i] = virtualMemory[l1++];
        else
            mergeTemp[i] = virtualMemory[l2++];
    }
    while(l1 <= mid)    
        mergeTemp[i++] = virtualMemory[l1++];

    while(l2 <= high)   
        mergeTemp[i++] = virtualMemory[l2++];

    for(i = low; i <= high; i++)
        virtualMemory[i] = mergeTemp[i];
}
/*Merge sort function.*/ 
void mergeSort( int l, int r) 
{ 
    if (l < r) { 
        int mid = l + (r - l) / 2; 
        mergeSort( l, mid); 
        mergeSort( mid + 1, r); 
        merge(l, mid, r); 
    }
}
/*Index sort function.*/
void indexSort(int l, int r) 
{ 
   int i, j; 
   for (i = l; i < (l+r)-1; i++){
       for (j = l; j < (l+r)-i-1; j++){
           if (get(j,"index") > get(j+1,"index")){
               swap(&virtualMemory[j],&virtualMemory[j+1]);
           }      
       } 
   }      
} 


/*Print sort result on screen.*/
void printSorting(){
    int i;
    int arr[virtualNum*frameSize];
    for(i=0;i < virtualNum*frameSize;i++){
        arr[i]=get(i,"check");
    }
    printf("\n-----------------AFTER SORTED-----------------------------\n");
    for(i=0;i < virtualNum*frameSize;i++){
        printf("%d\n",arr[i]);
    }
    printf("\n");
}

/*Filling memories at the start.*/
void fill(char* fileName){
    int i,j;
    int random = rand() % virtualNum*frameSize; 
    for(i=0;i < virtualNum*frameSize; i++){
        if(random == virtualNum*frameSize){
            random=0;
        }
        set(i,random,"fill");//setting virtual memory with random numbers.
        random=random+1;
    }
    int count=0;
    /*Update statistics.*/
    numWrBubble++;
    numWrIndex++;
    numWrMerge++;
    numWrQuick;
    count=0;

    /*Filling physical memory at start.*/
    for(i=0;i < physicalNum*frameSize; i++){
        physicalMemory[i] = rand();
        fprintf(fp,"%-11d\n",physicalMemory[i]);
        count++;
    }
    /*Adding disk. */
    for(i=count;i<virtualNum*frameSize;i++){
        int rnd=rand();
        fprintf(fp,"%-11d\n",rnd);
    }
    count=0;
}
/*Initializing physical and virtual memory data structures. */
void initializeMemories(){
    /*Allocating memory space.*/
    virtualMem=(struct pageTableUnit*)malloc((virtualNum)*sizeof(struct pageTableUnit));
    physicalMem=(struct pageTableUnit*)malloc((physicalNum)*sizeof(struct pageTableUnit));
    mergeTemp=(int*)malloc(sizeof(int)*(virtualNum*frameSize));
    int i,j;
    virtualMemory=(int*)malloc((virtualNum*frameSize)*sizeof(int));  
    physicalMemory=(int*)malloc((physicalNum*frameSize)*sizeof(int));   
    /*First initializing.*/
    for(i=0;i<virtualNum*frameSize;i++){
        virtualMemory[i]=-1;
    }
    for(i=0;i<physicalNum*frameSize;i++){
        physicalMemory[i]=-1;
    }
    for(i=0;i<physicalNum;i++){
        physicalMem[i].pageFrameNum=i;
    }
    for(i=0;i<physicalNum;i++){
        physicalMem[i].pageFrameNum=0;
        physicalMem[i].modified=0;
        physicalMem[i].reference=0;
        physicalMem[i].present=0;
    }
    for(i=0;i<virtualNum;i++){
        virtualMem[i].pageFrameNum=0;
        virtualMem[i].modified=0;
        virtualMem[i].reference=0;
        virtualMem[i].present=0;
    }
} 
/*Printing statistics on screen.*/
void printStatistics(){
    printf("\n~~~~~~~~~~~BUBBLE SORT STATİSTICS~~~~~~~~~~~~~\n");
    printf("Number of page reads:%d\n",numReadBubble);
    printf("Number of page writes:%d\n",numWrBubble);
    printf("Number of page misses:%d\n",pageMissBubble);
    printf("Number of page replacements:%d\n",pageReplBubble);
    printf("Number of disk page writes:%d\n",diskWrBubble);
    printf("Number of disk page reads:%d\n",diskReadBubble);

    printf("\n~~~~~~~~~~~QUICK SORT STATİSTICS~~~~~~~~~~~~~\n");
     printf("Number of page reads:%d\n",numreadQuick);
    printf("Number of page writes:%d\n",numWrQuick);
    printf("Number of page misses:%d\n",pageMissQuick);
    printf("Number of page replacements:%d\n",pageReplQuick);
    printf("Number of disk page writes:%d\n",diskWrQuick);
    printf("Number of disk page reads:%d\n",diskReadQuick);

    printf("\n~~~~~~~~~~~MERGE SORT STATİSTICS~~~~~~~~~~~~~\n");
    printf("Number of page reads:%d\n",numreadMerge);
    printf("Number of page writes:%d\n",numWrMerge);
    printf("Number of page misses:%d\n",pageMissMerge);
    printf("Number of page replacements:%d\n",pageReplMerge);
    printf("Number of disk page writes:%d\n",diskWrMerge);
    printf("Number of disk page reads:%d\n",diskReadMerge);

    printf("\n~~~~~~~~~~~INDEX SORT STATİSTICS~~~~~~~~~~~~~\n");
     printf("Number of page reads:%d\n",numreadIndex);
    printf("Number of page writes:%d\n",numWrIndex);
    printf("Number of page misses:%d\n",pageMissIndex);
    printf("Number of page replacements:%d\n",pageReplIndex);
    printf("Number of disk page writes:%d\n",diskWrIndex);
    printf("Number of disk page reads:%d\n",diskReadIndex);
}
/*Thread function.*/
static void *threadFunc(void *arg){
    int whichQuarter=*((int*)arg);
    int endPoint;
    int startPoint;
    if(whichQuarter==0){ //first quarter is bubble sort.
        endPoint = (virtualNum*frameSize)/4;
        bubbleSort(endPoint);
    }
    if(whichQuarter==1){ //second quarter is quick sort.
        startPoint= (virtualNum*frameSize)/4;
        endPoint = startPoint + ((virtualNum*frameSize)/4) -1;
        quickSort(startPoint,endPoint);
    }
    if(whichQuarter==2){ //third quarter is merge sort.
        startPoint=(virtualNum*frameSize)/2;
        endPoint = startPoint + ((virtualNum*frameSize)/4)-1;
        mergeSort(startPoint,endPoint);
    }
    if(whichQuarter==3){ //fourth quarter is index sort.
        startPoint=((virtualNum*frameSize)/4)*3;
        endPoint = virtualNum*frameSize;
        indexSort(startPoint,endPoint);
    }
}
/*Main function.*/
int main(int argc, char *argv[]) {
    if(argc!=8){
        perror("Error in commandline argument!\n");
        exit(EXIT_FAILURE);
    }
    srand(10000);
    /*Getting commandline arguments.*/
    frameSize = pow(2,atoi(argv[1])); 
    physicalNum = pow(2,atoi(argv[2]));
    virtualNum = pow(2,atoi(argv[3])); 
    pageReplacement=(char*)malloc(sizeof(char)*50);
    allocPolicy=(char*)malloc(sizeof(char)*20);
    strcpy(pageReplacement,argv[4]);
    strcpy(allocPolicy,argv[5]);
    pageTablePrintInt = atoi(argv[6]);
    char  diskName[200];
    strcpy(diskName,argv[7]);
    fp=fopen(diskName,"w+"); //Opening file system.
    if(fp == NULL)
    {
        perror("Error in creating the file\n");
        exit(EXIT_FAILURE);
    }
  
    initializeMemories();
    initializeStatistics();
    fill(diskName);
    if(strcmp(pageReplacement,"LRU")==0){
        initializeAccessTime();
    }
    /*Mutex init. */
    if(pthread_mutex_init(&mtx,NULL)!=0){
        perror("Mutex creating error in main!\n");
        exit(EXIT_FAILURE);
    }

    pthread_t threads[4];
    int save[4];
    int i,s,s2;

    /*Creating threads.*/
    for (i = 0; i < 4; i++) {
        save[i] = i;
        if ((s = pthread_create(&threads[i], NULL,threadFunc,(void*) &save[i])) != 0)
        {
            perror("Failed to pthread_create operation in main.\n");
            exit(EXIT_FAILURE);
        }
    }
   
    /*Waiting for threads by main thread.*/
    for (i = 0; i < 4; i++)
    {
        if ((s = pthread_join(threads[i], NULL)) != 0)
        {
            perror("Pthread_join error in main");
            exit(EXIT_FAILURE);
        }
    }
    /*Mutex destroy.*/
    if(pthread_mutex_destroy(&mtx)!=0){
        perror("Mutex destroy error");
        exit(EXIT_FAILURE);
    }
    lastControl=1;
    updateSystatistics("check");
   
    printStatistics();
 
 
    return 0;
}    