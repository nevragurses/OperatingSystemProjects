#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MB 1048576 //mb as bytes.
#define MAXLENG 7 //max address length.
#define KB 1024 //kb as bytes.
#define END '\0' //End of file character.
int main(int argc, char *argv[]) {
    int i;
    if(argc!=4){
        perror("Error in commandline argument!\n");
        exit(EXIT_FAILURE);
    }
    /*Getting commandline arguments.*/
    int blockSize = atoi(argv[1]);  
    int freeInode = atoi(argv[2]);
    char fileName[50];
    strcpy(fileName,argv[3]);
    FILE *fp;
    fp=fopen(fileName,"w+"); //Opening file system.
    if(fp == NULL)
    {
        perror("Error in creating the file\n");
        exit(EXIT_FAILURE);
    }
    fseek(fp,MB-1, SEEK_SET);  //Make size of file system exactly 1 MB.
    if(fputc(END,fp)==EOF){  //Putting eof character.*/
        perror("Error in determine size of file operation\n");
        exit(EXIT_FAILURE);
    }
    int blockNum=MB / (blockSize*KB); //number of blocks.
    int inodeNum=(blockNum*15)/100; //number of i-node blocks.
    int fsmStart = blockSize*KB; //file space management start address.
    int inodeStart=2*fsmStart; //i-node blocks start address.
    int rootDirStart= inodeStart + (inodeNum*blockSize*KB); //root directory block start address.
    int fileDirStart=rootDirStart + (blockSize*KB); //files and directories blocks start address.
    int dataBlockStart=fileDirStart+(4*blockSize*KB); //Data blocks start address.
    int dataBlock=blockNum -(inodeNum+7); //number of data blocks.
    int numberOfRootDir = (blockSize*KB)/113; //number of directories in root dir.
    int numberOfDir=(4*blockSize*KB)/458; //number of directories in files and directories part.

    /*Superblock section*/
    fseek(fp,0,SEEK_SET);
    fprintf(fp,"~~SUPERBLOCK~~\n");
    fprintf(fp,"Magic number:0x%d\n",12345);
    fprintf(fp,"Size of file:%d MB\n",1);
    fprintf(fp,"Block number:%d\n",blockNum);
    fprintf(fp,"Block size:%d\n",blockSize*KB);
    fprintf(fp,"Start address of free space management:%d\n",fsmStart);
    fprintf(fp,"I-node block number:%d\n",inodeNum);
    fprintf(fp,"Free i-nodes number:%d\n",freeInode);
    fprintf(fp,"Start address of i-node blocks:%d\n",2*fsmStart);
    fprintf(fp,"Start address of root directory:%d\n",rootDirStart);
    fprintf(fp,"Number of directories in root dir:%d\n",numberOfRootDir);
    fprintf(fp,"Start address of files and directories:%d\n",fileDirStart);
    fprintf(fp,"Number of directories:%d\n",numberOfDir);
    fprintf(fp,"Number of Files:%d\n",freeInode);
    fprintf(fp,"Data block number:%d\n",dataBlock);
    fprintf(fp,"Start address of data blocks:%d\n",dataBlockStart);


    /*Free space management block section*/
    fseek(fp,fsmStart,SEEK_SET);
    fprintf(fp,"\n~~FREE SPACE MGMT~~");
    fprintf(fp,"\nFree i-nodes number:%d\n",freeInode);
    fprintf(fp,"Free data block number:%d\n",dataBlock);
    fprintf(fp,"Free data block address:%d      ",dataBlockStart);
    fprintf(fp,"\nFree root dir index:%d      ",1);
    fprintf(fp,"\nFree root file index:%d      ",1);
    fprintf(fp,"\nFree dir index:%d      ",1);
    fprintf(fp,"\nFree i-node index:%d      ",1);
    fprintf(fp,"\n");

    /*I-node blocks section*/
    fseek(fp,2*fsmStart,SEEK_SET);
    fprintf(fp,"\n~~I-NODES~~");
    for(i=0;i<freeInode;i++){
        fprintf(fp,"\nI-node:%d\n",i+1);
        fprintf(fp,"Name of file:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nLast modification date and time:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nSize of file:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-0 Address:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-1 Address:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-2 Address:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-3 Address:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-4 Address:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDisk-5 Address:");
        fprintf(fp,"       "); 
        fprintf(fp,"\nSingle Indirect Block:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDouble Indirect Block:");
        fprintf(fp,"       ");
        fprintf(fp,"\nTriple Indirect Block:");
        fprintf(fp,"       ");
    }
    fprintf(fp,"\n");

    /*Root Directory Section.*/
    int rootLoop=(blockSize*KB)/113;
    fseek(fp,rootDirStart,SEEK_SET);
    fprintf(fp,"\n~~ROOT DIR~~");
    for(i=0;i<rootLoop;i++){
        fprintf(fp,"\nR-Dir-%d Name:",i+1);
        fprintf(fp,"                              ");
        fprintf(fp,"\nR-File-%d Name:",i+1);
        fprintf(fp,"                              ");
        fprintf(fp,"\nR-I-node:");
        fprintf(fp,"       ");
    }
    fprintf(fp,"\n");

    /*Files and Directories Section*/
    int sizeLoop=(4*blockSize*KB)/458;
    fseek(fp,fileDirStart,SEEK_SET);
    fprintf(fp,"\n~~FILES AND DIRECTORIES~~");
    for(i=0;i<sizeLoop;i++){
        fprintf(fp,"\nDirectory%d-Name:",i+1);
        fprintf(fp,"                              ");
        fprintf(fp,"\nFile1-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nI-node:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDir1-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nDir-Index:");
        fprintf(fp,"       ");
        fprintf(fp,"\nFile2-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nI-node:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDir2-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nDir-Index:");
        fprintf(fp,"       ");
        fprintf(fp,"\nFile3-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nI-node:");
        fprintf(fp,"       ");
        fprintf(fp,"\nDir3-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nDir-Index:");
        fprintf(fp,"       ");
        fprintf(fp,"\nFile4-Name:");
        fprintf(fp,"                              ");
        fprintf(fp,"\nI-node:");
        fprintf(fp,"       ");
    }    
    fprintf(fp,"\n");

    /*Data Blocks Section*/
    fseek(fp,dataBlockStart,SEEK_SET);
    fprintf(fp,"\n~~Data Block~~\n");
    return 0;
    
}    