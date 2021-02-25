#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define MB 1048576
#define KB 1024
/*File system operations.*/
void mkdirComm(FILE *fd,char* path,int fsmStart,int numberOfDir,int numberOfRootDir);
void writeComm(FILE *fd,char* path,char* fileName,int fsmStart,int blockSize,int numberOfDir,int numberOfRootDir,int numberOfInode);
int readComm(FILE *fd,char* path,char* fileName,int numberOfDir,int numberOfRootDir);
void listComm(FILE* fd,char* path,int numberOfDir,int numberOfRootDir);
void dumpe2fsComm(FILE* fd);
/*Functions that are using as helper functions for file system operations.*/
int findString(FILE *fd,char*string,int start,int end);
char* getString(FILE *fd,int byte);
void change(FILE *fd,int byte,int size,char* changeStr);
int searchInDir(FILE* fd,int amount,char* dirName);
int searchInFile(FILE* fd,int amount,char*rootFile);
void controlSameNameFile(FILE* fd,char* fileName,int start);
void controlSameNameDir(FILE* fd,char* dirName,int start);
void findFirstEmpty(FILE* fd,char pathArr[][100],int i,int numberOfDir);
void findFirstEmptyFile(FILE* fd,char pathArr[][100],int i,int val,int numberOfDir);
void helperControlPath(FILE*fd,char* dirName,char* innerDirName,int numberOfDir);
void isValidPath(FILE* fd,char arrPath[][100],int num,int numberOfDir,int numberOfRootDir);
void createDir(FILE *fd,char *dir,int fsmStart,int numberOfDir);
void rootFileNameControl(FILE *fd,int amount,char* rootFile);
void rootDirNameControl(FILE *fd,int amount,char* rootDir);
void rootDirControl(FILE *fd,int amount,char* rootDir);
char* readFile(FILE* fp,int length);
void updateDataInFSM(FILE *fd,int fsmStart);
void updateNodeInFSM(FILE* fd,int fsmStart);
void listRootDir(FILE*fd);


int main(int argc, char *argv[]) {
    FILE *fin;
    if((fin = fopen(argv[1], "r+")) == NULL) { /*Opening file system.*/
		return(EXIT_FAILURE);
	}
    char operation[20];
    char path[200];
    char fileName[100];
    if(argc < 3){
        perror("Error in commandline argument!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(operation,argv[2]);
    /*Getting some informations about file system,that are using in function parameters.*/
    int fsmStart = findString(fin,"~~FREE SPACE MGMT~~",0,MB);  
    int addressBlock= findString(fin,"Block size:",0,MB);
    int numberOfDir = atoi(getString(fin,findString(fin,"Number of directories:",0,MB))); 
    int numberOfRootDir = atoi(getString(fin,findString(fin,"Number of directories in root dir:",0,MB)));
    int blockSize=atoi(getString(fin,addressBlock));
    int numberOfInode= atoi(getString(fin,findString(fin,"Free i-nodes number:",0,MB)));
    /*mkdir file system operation.*/
    if (strcmp(operation,"mkdir") == 0){
        if(argc!=4 ){
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);
        }
        strcpy(path,argv[3]);
        mkdirComm(fin,path,fsmStart,numberOfDir,numberOfRootDir);
    }
    /*write file system operation.*/
    else if (strcmp(operation,"write") == 0){
        if(argc!=5){
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);
        }
        strcpy(path,argv[3]);
        strcpy(fileName,argv[4]);
        writeComm(fin,path,fileName,fsmStart,blockSize,numberOfDir,numberOfRootDir,numberOfInode);
    }
    /*list file system operation.*/
    else if (strcmp(operation,"list") == 0){
         if(argc!=4){
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);
        }
        strcpy(path,argv[3]);
       
        listComm(fin,path,numberOfDir,numberOfRootDir);
    }
    /*dumpe2fs file system operation.*/
    else if(strcmp(operation,"dumpe2fs") == 0){
        if(argc!=3){
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);
        }
        dumpe2fsComm(fin);
    } 
    /*read file system operation.*/
    else if(strcmp(operation,"read") == 0){
        if(argc!=5 ){
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);
        }
        strcpy(path,argv[3]);
        strcpy(fileName,argv[4]);
      
        readComm(fin,path,fileName,numberOfDir,numberOfRootDir);
    }
    /*False operations.*/
    else{
            perror("Error in commandline argument.");
            exit(EXIT_FAILURE);

    } 
   
    fclose(fin);
    return 0;
}
/*mkdir operation.Makes a new directory under the directory given path if possible.*/
void mkdirComm(FILE *fd,char* path,int fsmStart,int numberOfDir,int numberOfRootDir){
    char pathArr[10][100];
    int i=0;
    int val,startPoint,changePoint;
    char searched[100];
    char *getStr=(char*)malloc(100);
    char update[20];
    char* tok = strtok(path,"/"); 
    while( tok != NULL ) { /*Getting path.*/
        strcpy(pathArr[i],tok);
        i++;
        tok = strtok(NULL,"/");
    } 
    isValidPath(fd,pathArr,i-2,numberOfDir,numberOfRootDir); /*Controls path is valid or not.*/
    if(i==1){ /*If root directory is creating.*/
        rootDirNameControl(fd,numberOfRootDir,pathArr[0]);/*Controls whether root dir name is valid or not.*/
        if(strlen(pathArr[0])>30){/*Directory name length can be at most 30 */
            perror("Error!Directory name length can be at most 30.\n");
            exit(EXIT_FAILURE);
        }
        startPoint=findString(fd,"Free root dir index:",fsmStart,MB);
        strcpy(getStr, getString(fd,startPoint));
        val=atoi(getStr);
        if(val>numberOfRootDir){
            printf("Error!There can be at most %d directory root dir of system.",numberOfRootDir);
            exit(EXIT_FAILURE);
        }
        sprintf(searched,"R-Dir-%d Name:",val);
        changePoint=findString(fd,searched,0,MB);
        change(fd,changePoint,30,pathArr[0]);
        fseek(fd,startPoint,SEEK_SET); 
        sprintf(update,"%d",val+1);
        change(fd,startPoint,7,update);
        fseek(fd,0,SEEK_SET);
        createDir(fd,pathArr[0],fsmStart,numberOfDir);
    }    
    if(i>=2){/*If a directory is creating in given path.*/
        if(strlen(pathArr[i-1])>30){/*Directory name length can be at most 30 */
            perror("Error!Directory name length can be at most 30.\n");
            exit(EXIT_FAILURE);
        }
        findFirstEmpty(fd,pathArr,i,numberOfDir); /*Finds first empyty dir-name place in directory.*/
        createDir(fd,pathArr[i-1],fsmStart,numberOfDir); /*Creates directory in directories section in file system.*/
    }
    free(getStr);
    
}
/*Write file system operation.Creates a file named file under given path in file system, then copies the contents of the Linux file into the new file.*/
void writeComm(FILE *fd,char* path,char* fileName,int fsmStart,int blockSize,int numberOfDir,int numberOfRootDir,int numberOfInode){
    FILE *fp ;
    if((fp=fopen(fileName, "r"))==NULL){ /*Opens file.*/
        perror("Error! Failed open input file");
        exit(EXIT_FAILURE);
    }
    char keepPath[10][100];
    int i=0;
    char* tok = strtok(path,"/"); 
     while( tok != NULL ) { /*Getting path.*/
        strcpy(keepPath[i],tok); 
        i++;
        tok = strtok(NULL,"/");
    
    }  
    if(strlen(keepPath[i-1])>30){ /*File name length must be smaller than 30.*/
        perror("Error!File name length must be smaller than 30");
        exit(EXIT_FAILURE);
    }
    char *getStr=(char*)malloc(100);
    char searched[100];
    int changePoint;
    int val;
    int startPoint;
    char* increase=malloc(10);
    int valRoot;
    int addr;
    char* rInode=malloc(200);
    if(i==0){
        perror("Error! You must write a file name!\n");
        exit(EXIT_FAILURE);
    }
    if(i==1){  /*File is creating in root directory.*/
        rootFileNameControl(fd,numberOfRootDir,keepPath[0]);/*Controls whether file name is valid or not.*/
        int rootPoint=findString(fd,"Free root file index:",fsmStart,MB);/*Finds free root directory index to write file*/
        char* rootFile=malloc(200);
        strcpy(rootFile, getString(fd,rootPoint));
        valRoot=atoi(rootFile);
        if(valRoot>numberOfRootDir){ /*Controls whether spaces is  full to record file in root dir or not.*/
            printf("Error!There can be at most %d file in  system.",numberOfDir);
            exit(EXIT_FAILURE);
        }
        char* searchRootFile=malloc(200);
        sprintf(searchRootFile,"R-File-%d Name:",valRoot);
        addr=findString(fd,searchRootFile,0,MB);
        change(fd,addr,30,keepPath[0]); 
        fseek(fd,rootPoint,SEEK_SET); 
        char* updateFreeFile=malloc(100);
        sprintf(updateFreeFile,"%d",valRoot+1);
        change(fd,rootPoint,7,updateFreeFile);
        free(rootFile);
        free(searchRootFile);
        free(updateFreeFile);
    }
    else{ /*If there is path,control whether this path is valid or not.*/
        isValidPath(fd,keepPath,i-2,numberOfDir,numberOfRootDir);
    }

    startPoint=findString(fd,"Free i-node index:",fsmStart,MB); /*finds free i-node address to record file in i-node.*/
    strcpy(getStr, getString(fd,startPoint));
    val=atoi(getStr);
    if(val>numberOfInode){ /*Controls whether all i-nodes are full or not.*/
        printf("Error!There can be at most %d i-node in system.",numberOfInode);
    }
    int dataBlockAddr=findString(fd,"Free data block number:",fsmStart,MB);
    int controlNum=atoi(getString(fd,dataBlockAddr));
    if(controlNum==0){ /*Controls whether all data block are full or not.*/
         printf("Error!Data block is full.No more file!");
    }
    updateNodeInFSM(fd,fsmStart); /*Update free i-node number in file system management section.*/
    if(i==1){    
        change(fd,findString(fd,"R-I-node:",addr,MB),7,getString(fd,startPoint)); /*Records i-node number of file in root directory.*/
    }
    sprintf(searched,"I-node:%d",val);
    changePoint=findString(fd,searched,0,MB);
    sprintf(increase,"%d",val+1);
    change(fd,startPoint,7,increase);

    if(i>1){
        findFirstEmptyFile(fd,keepPath,i,val,numberOfDir); /*Records file in given directory.*/
    }

    /*Updates i-node attributes according to given file*/
    time_t rawtime; /*For getting current time*/
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    char time[40];
    strcpy(time,asctime (timeinfo));
    time[strlen(time)-1]='\0';

     change(fd,findString(fd,"Name of file:",changePoint,MB),30,keepPath[i-1]); 
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    char lengthArr[length+1];
    sprintf(lengthArr,"%d",length);
    change(fd,findString(fd,"Last modification date and time:",changePoint,MB),7,time);
    change(fd,findString(fd,"Size of file:",changePoint,MB),7,lengthArr);
    char *contents=malloc(length+1);
    strcpy(contents,readFile(fp,length));

    /*Updates data block section in i-node according to find first free data block address.*/
    int freeData=findString(fd,"Free data block address:",0,MB);
    char* freeAdr=malloc(20);
    strcpy(freeAdr, getString(fd,freeData));
    int val2=atoi(freeAdr)+16;
    fseek(fd,val2,SEEK_SET);
    fprintf(fd,"%s",contents);
    char upload[20];
    sprintf(upload,"%d",val2+length);
    change(fd,freeData,7,upload);

    char* freeDataAddress=(char*)malloc(10);
    sprintf(freeDataAddress,"%d",val2);
    change(fd,findString(fd,"Disk-0 Address:",changePoint,MB),7,freeDataAddress);
    updateDataInFSM(fd,fsmStart);
    /*If size of file is bigger than 1 block size,updates other disk adresses in i-node.*/
    if(length>blockSize){
        int divide=length/blockSize;
        if(divide>5){
            perror("ERROR! FILE SIZE IS TOO BIG");
            exit(EXIT_FAILURE);
        }
        char* big=malloc(100);
        char *diskNum=malloc(100);
        sprintf(big,"%d",divide);
        int z=1;
        for(z=1;z<=divide;z++){
            sprintf(freeDataAddress,"%d",(val2+(z*blockSize)));    
            sprintf(diskNum,"Disk-%d Address:",z);
            change(fd,findString(fd,diskNum,changePoint,MB),7,freeDataAddress);
            updateDataInFSM(fd,fsmStart); 
        } 
        free(big);
        free(diskNum); 

    }
    else{
        sprintf(upload,"%d",val2+blockSize);
        change(fd,freeData,7,upload); 
        //updateDataInFSM(fd,fsmStart); 
    }
    free(getStr);
    free(rInode);
    free(freeAdr);
    free(freeDataAddress);
    free(contents);
    free(increase);
      

}
/*Read file system operation.Reads the file named file under given path in file system, then writes this data to the Linux file.*/
int readComm(FILE *fd,char* path,char* fileName,int numberOfDir,int numberOfRootDir){
    char keepPath[10][100];
    int i=0,a;
    char* tok = strtok(path,"/"); 
    while( tok != NULL ) {
        strcpy(keepPath[i],tok);
        i++;
        tok = strtok(NULL,"/");
    } 
    int getInodeNum=-1;
    if(i==0){
        perror("Error! You must write a file name!\n");
        exit(EXIT_FAILURE);
    }
    if(i==1){ /*Reads file in root directory*/
        int fileAddr=searchInFile(fd,numberOfRootDir,keepPath[0]);
        getInodeNum=atoi(getString(fd,findString(fd,"R-I-node:",fileAddr,MB))); /*Gets i-node number of given file.*/

    }
    if(i>1){ /*Reads file in given path.*/
        isValidPath(fd,keepPath,i-2,numberOfDir,numberOfRootDir); /*Controls path is valid or not.*/
        
        char* apply=malloc(200);
        strcpy(apply,keepPath[i-2]);  
        char ch = ' '; 
        for(a=strlen(keepPath[i-2]);a < 30;a++){
            strncat(apply,&ch , 1); 
        }
        ch = '\n'; 
        strncat(apply,&ch , 1);
        int start=searchInDir(fd,numberOfDir,apply);
    

        char* searchFile=malloc(200);
        strcpy(searchFile,keepPath[i-1]); 
        char ch2 = ' ';  
        for(a=strlen(keepPath[i-1]);a < 30;a++){
            strncat(searchFile,&ch2 , 1); 
        }
        ch2 = '\n'; 
        strncat(searchFile,&ch2 , 1);
    
        int z=1;
        char* temp=malloc(200);
       
        for(z=1;z<5;z++){  /*Finds file in directory.*/
            sprintf(temp,"File%d-Name",z);
            if(strcmp(getString(fd,findString(fd,temp,start,MB)),searchFile)==0){
                int searchFileStart=findString(fd,temp,start,MB);
                int inodeStart=findString(fd,"I-node:",searchFileStart,MB);
                getInodeNum=atoi(getString(fd,inodeStart));

            }
        }
        if(getInodeNum==-1){ /*If there is no file that has given name,prints error messahe.*/
            printf("Error!No that name file in directory to read,name is->%s",searchFile);
            exit(EXIT_FAILURE);
        }
        free(apply);
        free(searchFile);
        free(temp);
    }     
    char* stringInode=malloc(100);
    sprintf(stringInode,"I-node:%d",getInodeNum); /*Gets i-node number of given file.*/
    int inodeBlockAddr=findString(fd,stringInode,0,MB);
    int address=atoi(getString(fd,findString(fd,"Disk-0 Address:",inodeBlockAddr,MB))); /*Finds data block number of file.*/
    /*Opens linux file.*/
    FILE* out;
    if((out = fopen(fileName, "w+")) == NULL) {
        perror("ERROR! Failed open output file\n");  
        return(EXIT_FAILURE);
    }
    fseek(fd,address,SEEK_SET);
    char readChar;
    /*Writes contents of file in given path in linux file*/
    do{
        readChar=fgetc(fd);
        if(readChar=='\0')
            break;
        fputc(readChar,out);
    }while(readChar!='\0');  
    free(stringInode);
    return 0;

}
/*List file system operation.*/
void listComm(FILE* fd,char* path,int numberOfDir,int numberOfRootDir){
    char keepPath[10][100];
    int i=0,a=0;
    char* tok = strtok(path,"/"); 
     while( tok != NULL ) {
        strcpy(keepPath[i],tok);
        i++;
        tok = strtok(NULL,"/");
    
    }
    if(i==0){
        listRootDir(fd); /*Lists files and directories in root directory.*/
    } 
    else{ /*Lists files and directories in given path.*/
        isValidPath(fd,keepPath,i-1,numberOfDir,numberOfRootDir);
        char* apply=malloc(200);
        strcpy(apply,keepPath[i-1]);  
        char ch = ' '; 
        for(a=strlen(keepPath[i-1]);a < 30;a++){
            strncat(apply,&ch , 1); 
        }
        ch = '\n'; 
        strncat(apply,&ch , 1);
        int start=searchInDir(fd,numberOfDir,apply);

        int z=1;
        char* control=malloc(10);
        *control='\0';
        char* temp=malloc(200);

        for(z=1;z<5;z++){
            sprintf(temp,"File%d-Name",z);
            if(strcmp(getString(fd,findString(fd,temp,start,MB)),control)!=0){
                printf("%s\n",getString(fd,findString(fd,temp,start,MB)));
            }
        }
        for(z=1;z<4;z++){
            sprintf(temp,"Dir%d-Name",z);
            if(strcmp(getString(fd,findString(fd,temp,start,MB)),control)!=0){
                printf("%s\n",getString(fd,findString(fd,temp,start,MB)));
            }
        }
        free(temp);
        free(control);
        free(apply);
    }    
}
/*Dumpe2fs file system operation.*/
void dumpe2fsComm(FILE* fd){
    printf("Block Size:%s",getString(fd,findString(fd,"Block size:",0,MB)));
    printf("Block Count:%s",getString(fd,findString(fd,"Block number:",0,MB)));
    printf("I-node Block Count:%s",getString(fd,findString(fd,"I-node block number:",0,MB)));
    printf("I-node Count:%s",getString(fd,findString(fd,"Free i-nodes number:",0,MB)));
    printf("Number of Directories:%s",getString(fd,findString(fd,"Number of directories",0,MB)));
    printf("Number of Files:%s",getString(fd,findString(fd,"Free i-nodes number:",0,MB)));
    int inodeStart=findString(fd,"~~I-NODES~~",0,MB);
    char* control=malloc(10);
    *control='\0';
    char* temp=malloc(200);
    printf("~~~Occupied I-Node Blocks and File Names In That Blocks~~~\n");
    int i=1;
    for(i=1;i<400;i++){
        sprintf(temp,"I-node:%d",i);
        int findNode=findString(fd,temp,inodeStart,MB);

        if(strcmp(getString(fd,findString(fd,"Name of file:",strlen(temp)+findNode+1,MB)),control)!=0){
            printf("%s\n",temp);
            printf("%s",getString(fd,findString(fd,"Name of file:",strlen(temp)+findNode+1,MB)));
        }   
    }
    free(control);
    free(temp);
}

/*****************BELOW FUNCTIONS USING AS HELPER FUNCTIONS FOR FILE SYSTEM OPERATIONS*****************************/

/*Finds address of given string.*/
int findString(FILE *fd,char*string,int start,int end){
    char * line = (char*)malloc(2000);
    size_t len = 0;
    int findedByte;
    size_t read;
    fseek(fd,start,SEEK_SET);
    while(ftell(fd)<=end ){
        if(((int)(read = getline(&line, &len, fd))) != -1){ /*If given string is finded,return its address.*/
            if (strstr(line, string) ){
                int loc=ftell(fd);
                findedByte = loc-read;
                break;
            }
        }        
    }
    free(line);
    return findedByte;  
}
/*Gets string after two dots punctuation.*/
char* getString(FILE *fd,int byte){
    char * line = (char*)malloc(2000);
    char *get=(char*)malloc(200);
    size_t len = 0;
    int i=0;
    fseek(fd,byte,SEEK_SET);
    getline(&line, &len, fd);
    char* token = strtok(line,":"); 
    i++;
    while( token != NULL ) {
        if(i==2){
            strcpy(get,token); /*Copy string after two dots punctuation.*/
        }    
        token = strtok(NULL,":");
        i++;
    }     
    free(line);
    char blank=' ';
    if(get[0]==blank && get[1] == blank ){
        *get='\0';
    }

    return get;  
}
/*Change string after two dots punctuation according to given adress.*/
void change(FILE *fd,int byte,int size,char* changeStr){
    char * line = (char*)malloc(2000);
    int keep=0,i=0;  
    size_t len = 0;
    fseek(fd,byte,SEEK_SET);
    getline(&line, &len, fd);/*Gets line in given address.*/
    char* token = strtok(line,":"); 
    i++;
    while( token != NULL ) {/*Changes string after two dots punctuation.*/
        if(i==2){
            keep= ftell(fd)-strlen(token);  
            fseek(fd,keep,SEEK_SET);
            fprintf(fd,"%s",changeStr);
            int deleteAmount=size-strlen(changeStr);
            for(i=0;i<deleteAmount;i++){
                fputc(' ',fd);
            }
        }    
        token = strtok(NULL,":");
        i++;
    }   
    free(line);   
}
/*Finds address of given directory name.If it does not exist,prints error message.*/ 
int searchInDir(FILE* fd,int amount,char* dirName){
    char searchDir[100];
    int finded=0;
    int i=0;
    for(i=0;i<amount-1;i++){ /*Controls all directory names in root directory.*/
        sprintf(searchDir,"Directory%d-Name:",i+1);
        int findDir=findString(fd,searchDir,0,MB);
        if(strcmp(getString(fd,findDir),dirName)==0){
            finded=1;
            return findDir;
            break;
        }
    }
    if(finded == 0){
        printf("Error!There is no such name directory in your system,name is->%s \n",dirName);
        exit(EXIT_FAILURE);
    }
    return -1;
}
/*Finds address of given file name in root directory.If it does not exist,prints error message.*/
int searchInFile(FILE* fd,int amount,char*rootFile){
    char searchFile[100];
    int finded=0;
    char *apply=malloc(100);
    strcpy(apply,rootFile);
    char ch = ' '; 
    int a=0;
    for(a=strlen(rootFile);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int i=0;
    for(i=0;i<amount-1;i++){ /*Controls all file names in root directory.*/
        sprintf(searchFile,"R-File-%d Name",i+1);
        int findFile=findString(fd,searchFile,0,MB);
        if(strcmp(getString(fd,findFile),apply)==0){
            finded=1;
            return findFile;
            break;
        }
    }
    if(finded == 0){
        printf("Error!There is no such name file in your system,name is->%s \n",rootFile);
        exit(EXIT_FAILURE);
    }
    free(apply);
    return -1;

}
/*Controls whether there is same name file in directory or not.If it exists,prints error message.*/
void controlSameNameFile(FILE* fd,char* fileName,int start){
    char searchDir[100];
    int finded=0,a=0;
    char *apply=malloc(100);
    strcpy(apply,fileName);
    char ch = ' '; 
    for( a=strlen(fileName);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int i=0;
    for(i=0;i<4;i++){ /*Controls all file names in directory.*/
        sprintf(searchDir,"File%d-Name:",i+1);
        int findDir=findString(fd,searchDir,start,MB);
        if(strcmp(getString(fd,findDir),apply)==0){
            finded=1;
        }
    }
    if(finded == 1){
        printf("Error!There is same name file in your directory,name is->%s \n",apply);
        exit(EXIT_FAILURE);
    }
    free(apply);

}
/*Controls whether there is same name directory in directory or not.If it exists,prints error message.*/
void controlSameNameDir(FILE* fd,char* dirName,int start){
    char searchDir[100];
    int finded=0,a=0;
    char *apply=malloc(100);
    strcpy(apply,dirName);
    char ch = ' '; 
    for( a=strlen(dirName);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int i=0;
    for(i=0;i<3;i++){ /*Controls all directory names in directory.*/
        sprintf(searchDir,"Dir%d-Name:",i+1);
        int findDir=findString(fd,searchDir,start,MB);
        if(strcmp(getString(fd,findDir),apply)==0){
            finded=1;
        }
    }
    if(finded == 1){
        printf("Error!There is same name directory in your directory,name is->%s \n",apply);
        exit(EXIT_FAILURE);
    }
    free(apply);

}
/*Creates inner directory in directory.Writes directory name in first appropriate place in directory.*/
void findFirstEmpty(FILE* fd,char pathArr[][100],int i,int numberOfDir){
    int j=1,empty=0,a;
    char* apply=(char*)malloc(30);
    char* temp=(char*)malloc(100);
    char* addr=(char*)malloc(100);
    char*addresPtr=(char*)malloc(20);
    char* strAddr=(char*)malloc(30);
    char* control=(char*)malloc(10);
    int inner;

    strcpy(apply,pathArr[i-2]);
    char ch = ' '; 
    for( a=strlen(pathArr[i-2]);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    inner=searchInDir(fd,numberOfDir,apply);
    controlSameNameDir(fd,pathArr[i-1],inner);
    *control='\0';
    while(j<4 && empty==0){ /*Finds first empty dir-name place in directory.*/
        sprintf(temp,"Dir%d-Name:",j);
        int loc=findString(fd,temp,inner,MB);
        if(strcmp(getString(fd,loc),control)==0){
            change(fd,loc,30,pathArr[i-1]);
            int locAddr=findString(fd,"Dir-Index",loc,MB);
            sprintf(addresPtr,"%d",j);
            change(fd,locAddr,7,addresPtr);
            empty=1;
            break;
        }
        j++;
    }
    if(empty==0){
        perror("Error!There can be at most 3 directory in a directory");
        exit(EXIT_FAILURE);
    }
    free(apply);
    free(temp);
    free(addr);
    free(addresPtr);
    free(strAddr);
    free(control);
}
/*Creates file in directory.Writes file name in first appropriate place in directory.*/
void findFirstEmptyFile(FILE* fd,char pathArr[][100],int i,int val,int numberOfDir){
    int j=1,empty=0,a;
    char* apply=(char*)malloc(30);
    char* temp=(char*)malloc(100);
    char* addr=(char*)malloc(100);
    char*addresPtr=(char*)malloc(20);
    char* strAddr=(char*)malloc(30);
    char* control=(char*)malloc(10);
    int inner;

    strcpy(apply,pathArr[i-2]);
    char ch = ' '; 
    for( a=strlen(pathArr[i-2]);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    inner=searchInDir(fd,numberOfDir,apply);
    controlSameNameFile(fd,pathArr[i-1],inner);
    *control='\0';
    while(j<5 && empty==0){ /*Finds first empty file-name place in directory.*/
        sprintf(temp,"File%d-Name:",j);
        int loc=findString(fd,temp,inner,MB);
        if(strcmp(getString(fd,loc),control)==0){
            change(fd,loc,30,pathArr[i-1]);
            char* inodeVal=malloc(10);
            sprintf(inodeVal,"%d",val);
            change(fd,findString(fd,"I-node:",loc,MB),7,inodeVal);
            empty=1;
            break;
        }
        j++;
    }
    if(empty==0){
        perror("Error!There can be at most 4 file in a directory");
        exit(EXIT_FAILURE);
    }
    free(apply);
    free(temp);
    free(addr);
    free(addresPtr);
    free(strAddr);
    free(control);
}

/*Creates directory.It writes directory name and address in directory section.*/
void createDir(FILE *fd,char *dir,int fsmStart,int numberOfDir){
    int secondStart,changePoint2,val2=0;
    char update2[20];
    char getStr2[100],searched2[100];
    fseek(fd,0,SEEK_SET);
    secondStart= findString(fd,"Free dir index:",fsmStart,MB);
    strcpy(getStr2, getString(fd,secondStart));
    val2=atoi(getStr2);
    if(val2>numberOfDir){
        printf("Error!There can be at most %d directory in  system.",numberOfDir);
        exit(EXIT_FAILURE);
    }
    sprintf(searched2,"Directory%d-Name",val2);
    changePoint2=findString(fd,searched2,0,MB);
    change(fd,changePoint2,30,dir);
    fseek(fd,secondStart,SEEK_SET);
    sprintf(update2,"%d",val2+1);
    change(fd,secondStart,7,update2);
}
/*Controls whether there is same name file name in root directory or not.If it exists,prints error message.*/
void rootFileNameControl(FILE *fd,int amount,char* rootFile){
    char searchDir[100];
    char* apply=malloc(200);
    strcpy(apply,rootFile);  
    char ch = ' '; 
    int a=0;
    for(a=strlen(rootFile);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int finded=0;
    int i=0;
    for(i=0;i<amount;i++){ /*Controls all directory names in root directory.*/
        sprintf(searchDir,"R-File-%d Name:",i+1);
        int findDir=findString(fd,searchDir,0,MB);
        if(strcmp(getString(fd,findDir),apply)==0){
            finded=1;
        }
    }
    if(finded == 1){
        printf("Error!There is same name root directory in your system,name is->%s \n",rootFile);
        exit(EXIT_FAILURE);
    }
    free(apply);
}
/*Controls whether there is same name directory name in root directory or not.If it exists,prints error message.*/
void rootDirNameControl(FILE *fd,int amount,char* rootDir){
    char searchDir[100];
    char* apply=malloc(200);
    strcpy(apply,rootDir);  
    char ch = ' '; 
    int a=0;
    for(a=strlen(rootDir);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int finded=0;
    int i=0;
    for(i=0;i<amount;i++){ /*Controls all directory names in root directory.*/
        sprintf(searchDir,"R-Dir-%d",i+1);
        int findDir=findString(fd,searchDir,0,MB);
        if(strcmp(getString(fd,findDir),apply)==0){
            finded=1;
        }
    }
    if(finded == 1){
        printf("Error!There is same name root directory in your system,name is->%s \n",rootDir);
        exit(EXIT_FAILURE);
    }
    free(apply);
}

/*Controls whether given root directory name exists or not.If it does not exist,prints error message.*/
void rootDirControl(FILE *fd,int amount,char* rootDir){
    char searchDir[100];
    char* apply=malloc(200);
    strcpy(apply,rootDir);  
    char ch = ' '; 
    int a=0;
    for(a=strlen(rootDir);a < 30;a++){
        strncat(apply,&ch , 1); 
    }
    ch = '\n'; 
    strncat(apply,&ch , 1);
    int finded=0;
    int i=0;
    for(i=0;i<amount;i++){ /*Controls all directory names in root directory.*/
        sprintf(searchDir,"R-Dir-%d",i+1);
        int findDir=findString(fd,searchDir,0,MB);
        if(strcmp(getString(fd,findDir),apply)==0){
            finded=1;
        }
    }
    if(finded == 0){
        printf("Error!No such directory in rootDir->%s \n",rootDir);
        exit(EXIT_FAILURE);
    }
    free(apply);
}
/*Helper function to control path name valid or not.Controls inner directory names in a directory.*/
void helperControlPath(FILE*fd,char* dirName,char* innerDirName,int numberOfDir){
    char searchDir[100];
    int finded=0,i=0;
    int inner=searchInDir(fd,numberOfDir,dirName);
    for(i=0;i<3;i++){ /*Controls all inner directory names in given directory.*/
        sprintf(searchDir,"Dir%d-Name:",i+1);
        int findDir=findString(fd,searchDir,inner,MB);
        if(strcmp(getString(fd,findDir),innerDirName)==0){
            finded=1;
        }
    }
    if(finded == 0){
        printf("Error! Invalid path name!No such directory->%s \n",innerDirName);
        exit(EXIT_FAILURE);
    }
}
/*Controls whether path name is valid or not.It uses helperControlPath function to control inner directories in a directory.*/
void isValidPath(FILE* fd,char arrPath[][100],int num,int numberOfDir,int numberOfRootDir){
    if(num==0){
        rootDirControl(fd,numberOfRootDir,arrPath[0]);
    }
    int i=1,a=0;
    for(i=1;i<=num;i++){
        char* apply=malloc(200);
        char* innerDir=malloc(200);
        strcpy(apply,arrPath[i-1]);  
        strcpy(innerDir,arrPath[i]);  
        char ch = ' '; 
        for(a=strlen(arrPath[i-1]);a < 30;a++){
            strncat(apply,&ch , 1); 
        }
        for(a=strlen(arrPath[i]);a < 30;a++){
            strncat(innerDir,&ch , 1); 
        }
        ch = '\n'; 
        strncat(apply,&ch , 1);
        strncat(innerDir,&ch , 1); 
        helperControlPath(fd,apply,innerDir,numberOfDir);
        free(innerDir);
        free(apply);
    }

}

/*Reads all file and records in buffer.*/
char* readFile(FILE* fp,int length){
    fseek(fp, 0, SEEK_SET); 
    char *string = malloc(length + 1);
    fread(string, 1, length, fp);
    fclose(fp);
    return string;
}
/*Updates free data block section in free space management.*/
void updateDataInFSM(FILE *fd,int fsmStart){
    int addr=findString(fd,"Free data block number:",fsmStart,MB);
    int num=atoi(getString(fd,addr));
    char* update=malloc(20);
    sprintf(update,"%d",num-1);
    change(fd,addr,3,update);
    free(update);
}
/*Updates free i-nodes section in free space management.*/
void updateNodeInFSM(FILE* fd,int fsmStart){
    int addr=findString(fd,"Free i-nodes number:",fsmStart,MB);
    int num=atoi(getString(fd,addr));
    char* update=malloc(20);
    sprintf(update,"%d",num-1);
    change(fd,addr,3,update);
    free(update);
}
/*Lists files and directories in root directory*/
void listRootDir(FILE*fd){
    char* dir=malloc(200);
    char*file=malloc(200);
    char* control=malloc(10);
        *control='\0';
    int i=1;
    for(i=1;i<20;i++){
        sprintf(dir,"R-Dir-%d Name",i);
        sprintf(file,"R-File-%d Name:",i);
        if(strcmp(getString(fd,findString(fd,dir,0,MB)),control)!=0){
            printf("%s\n",getString(fd,findString(fd,dir,0,MB)));
        }
        if(strcmp(getString(fd,findString(fd,file,0,MB)),control)!=0){
            printf("%s\n",getString(fd,findString(fd,file,0,MB)));
        }
    }
    free(dir);
    free(file);
    free(control);
}



