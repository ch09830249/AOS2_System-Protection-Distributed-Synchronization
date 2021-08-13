#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc , char *argv[])
{   
    int size;
    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);
    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(5002);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error");
    }
	char receiveMessage[1024] = {};
	char input[256];
	printf(">");
    	while(fgets(input, 256 , stdin) != NULL){
		//replace \n to \0
       		 if(input[strlen(input) - 1] == '\n'){
            	 	input[strlen(input) - 1] = '\0';
        	 }
		 send(sockfd,input,sizeof(input),0);
		 //將指令送出// if input is exit, leave this programe
       		 if(strcmp("exit", input) == 0){
			 sleep(0.5);
           		 return 0;
        	 }
		char* behavior = strtok(input, " ");
		char* filename = strtok(NULL, " ");
		char* rightstring;		

		//read operation		
		if(strcmp(behavior, "read")==0){
			size = recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);//接收server資訊
			if(receiveMessage[0]=='T'){//沒檔案沒得讀
				puts("The server doesn't have the file");
			}
			else if(receiveMessage[0]=='Y'){//沒權限沒得讀
				puts("You have no right.");
			}
			else{ //有權限讀
				if(receiveMessage[0]=='e'&&receiveMessage[1]=='m'&&receiveMessage[2]=='p'){
					FILE* file = fopen(filename, "w");
			 		fclose(file);
				}
				else{
					FILE* file = fopen(filename, "wb");
					fwrite(receiveMessage, sizeof(char), size, file);
					fclose(file);
				}				
			 }
		}

		//write operation
		else if(strcmp(behavior, "write")==0){
			char buff[256];
			char receive[256];
			FILE* file = fopen(filename, "rb");
			if(file==NULL)//client沒檔案沒得讀
			{	puts("The file doesn't exist!!!!\n");
				char message[] = {"The file doesn't exist!!!!\n"};
				send(sockfd, message, sizeof(message),0);//告訴server, client沒檔
			}
			else{//有檔案讀
				char message[] = {"The file exist!!!!\n"};
				send(sockfd, message, sizeof(message),0);//告訴server, client有檔
				recv(sockfd, receive, sizeof(receive),0);//server告訴client你有沒有權限寫
				if(strcmp(receive, "You have no right.\n\n")==0){//沒權寫
						printf("You have no right.\n");
						fclose(file);
				}
				else{//有權
						char buffe[256];
						int Size = fread(buffe, 1 , 256, file);
						puts("Sending");
						if(Size==0){//0 byte
							 char message[] = {"empty\n"};
						  	 send(sockfd, message, sizeof(message),0);
							 puts("Sending Success.(1)");
							 fclose(file);
						}
						else{	
							send(sockfd, buffe, Size,0);						
							puts("Sending Success.(2)");
							fclose(file);
						}
				}
							
			}
		}
	
		
		printf(">");
        }
    close(sockfd);
    return 0;
}
