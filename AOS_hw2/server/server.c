#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
#include "socket.h"
/*struct filelist{
	char* name;
	sem_t rsem;
	sem_t wsem;
	sem_t x;
	sem_t y;
	sem_t z;
	int readcnt;
        int wrtcnt;
}filearray[60];

int filenum = 0;*/
struct user{
	char* username;
	char* group;
	struct fileobject* first_object;
}users[6];

struct fileobject{
	char* filename;
	int read_right;
	int write_right;
	char* owner;
	struct fileobjec* next;
};

struct user insert(struct user user1, char* name, char*fileowner, int a, int b){//檔名name,只有filename放入node
    struct fileobject* job = (struct fileobject*)malloc(sizeof(struct fileobject));
    job->filename = strdup(name);
    job->read_right = a;
    job->write_right = b;
    job->owner = strdup(fileowner);
    job->next = NULL;
	if(user1.first_object == NULL){
		user1.first_object = job;
	}
	else{
		job->next = user1.first_object;
		user1.first_object = job;
	}
	return user1;
}


void show(struct user user1){
    printf("username : %s\n", user1.username);
    printf("group : %s\n\n\n", user1.group);
    struct fileobject* temp = user1.first_object;
    while(temp!=NULL){
        printf("filename : %s\n", temp->filename);
        printf("owner : %s\n", temp->owner);
        printf("read : %d\n", temp->read_right);
        printf("write : %d\n", temp->write_right);
        printf("\n\n");
        temp = temp->next;
    }
}

struct user del_node(struct user user1, char* name){ //將user1底下file為name的node刪掉
	struct fileobject* current = user1.first_object;
	struct fileobject* previous = NULL;
	while(current != NULL && (strcmp(name, current->filename)!=0)){
        previous = current;
        current = current->next;
	}
	if(current == NULL) {
        printf("There is no this file");
         return user1;
    }
    else if (current == user1.first_object) {
        user1.first_object = current->next;
        free(current);
        current = NULL;
         return user1;
    }
    else {
        previous->next = current->next;
        free(current);
        current = NULL;
        return user1;
    }
}


struct fileobject* find_node(struct user user1, char* name){ //找user1底下有沒有此檔案權限(node)
    struct fileobject* temp = user1.first_object;
    while(temp!=NULL){
        if(strcmp(temp->filename, name)==0){
            return temp;
        }
        temp = temp->next;
    } return NULL;
}


void setnode_right(struct fileobject* node, int a, int b){//將read設為a  將write設為b
    node->read_right = a;
    node->write_right = b;
}


void* Suser2() {
  USER_EXEC(5001,1);
  pthread_exit(NULL); // 離開子執行緒
}
void* Suser3() {
  USER_EXEC(5002,2);
  pthread_exit(NULL); // 離開子執行緒
}
void* Suser4() {
  USER_EXEC(5003,3);
  pthread_exit(NULL); // 離開子執行緒
}
void* Suser5() {
  USER_EXEC(5004,4);
  pthread_exit(NULL); // 離開子執行緒
}
void* Suser6() {
  USER_EXEC(5005,5);
  pthread_exit(NULL); // 離開子執行緒
}



int main(int argc , char *argv[])
{  


    for(int i=0;i<6;i++){
        users[i].username = NULL;
        users[i].group = NULL;
        users[i].first_object = NULL;
    }
    users[0].username = "Kenny";  users[0].group = "AOS";
    users[1].username = "Danny";  users[1].group = "AOS";
    users[2].username = "Anna";   users[2].group = "AOS";
    users[3].username = "Johnny"; users[3].group = "CSE";
    users[4].username = "Cindy";  users[4].group = "CSE";
    users[5].username = "Amy";    users[5].group = "CSE";

    pthread_t user2,user3,user4,user5,user6;// 宣告 pthread 變數
    pthread_create(&user2, NULL, Suser2, NULL); // 建立子執行緒
    pthread_create(&user3, NULL, Suser3, NULL); // 建立子執行緒
    pthread_create(&user4, NULL, Suser4, NULL); // 建立子執行緒
    pthread_create(&user5, NULL, Suser5, NULL); // 建立子執行緒
    pthread_create(&user6, NULL, Suser6, NULL); // 建立子執行緒  
    
    
    //socket的建立
    
    int sockfd = 0,forClientSockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線
    struct sockaddr_in serverInfo,clientInfo;
    int addrlen = sizeof(clientInfo);
    bzero(&serverInfo,sizeof(serverInfo));

    serverInfo.sin_family = PF_INET;
    serverInfo.sin_addr.s_addr = INADDR_ANY;
    serverInfo.sin_port = htons(5000);
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));
    listen(sockfd,5);

    while(1){
	char inputBuffer[256] = {};//接收來自client的指令
	if(forClientSockfd==0)
	 {
	 	forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);
	 }
	 recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);
	 //replace \n to \0
       	 if(inputBuffer[strlen(inputBuffer) - 1] == '\n'){
          inputBuffer[strlen(inputBuffer) - 1] = '\0';
         }
	 // if input is exit, leave this programe
       	 if(strcmp("exit", inputBuffer) == 0){
           	return 0;
         }
	 char* behavior = strtok(inputBuffer, " ");//先切出做什麼operation
	 char* file_name = strtok(NULL, " ");//再切出檔案的名稱

	 //create operation
	 if(strcmp(behavior, "create")==0){
		 char* rightstring = strtok(NULL, " ");//將rightstring切出來
		 char* boss = users[0].username;//目前要創檔的是Kenny
		 char* ggroup = users[0].group;
		 /*filearray[filenum].name = strdup(boss);
		 sem_init(&filearray[filenum].wsem,0,1);
		 sem_init(&filearray[filenum].rsem,0,1);
		 sem_init(&filearray[filenum].x,0,1);
		 sem_init(&filearray[filenum].y,0,1);
		 sem_init(&filearray[filenum].z,0,1);
		 filearray[filenum].wrtcnt = 0;
		 filearray[filenum].readcnt = 0;
		 filenum++;	*/	 
		 printf("rightstring : %s\n",rightstring);
		 printf("創檔人 : %s\n",boss);
		 printf("創檔人所屬的group : %s\n\n",ggroup);
		 if(rightstring==NULL)puts("Right set error.");
		 else{
			 for(int b=0;b<6;b++){
				if(strcmp(users[b].username, boss)==0){
					 if(rightstring[0]=='r' && rightstring[1]=='w'){
					  users[b] = insert(users[b], file_name, boss,1,1);		 
					 }
					 else if (rightstring[0]=='r' && rightstring[1]=='-'){
					  users[b] = insert(users[b], file_name, boss,1,0);
					 }
					 else if (rightstring[0]=='-' && rightstring[1]=='w'){
					  users[b] = insert(users[b], file_name, boss,0,1);
					 }
			 	}
			 	 else if(strcmp(users[b].group,ggroup)==0 && strcmp(users[b].username, boss)!=0){
			 	 	 if(rightstring[2]=='r' && rightstring[3]=='w'){
					  users[b] = insert(users[b], file_name, boss,1,1);		 
					 }
					 else if (rightstring[2]=='r' && rightstring[3]=='-'){
					  users[b] = insert(users[b], file_name, boss,1,0);
					 }
					 else if (rightstring[2]=='-' && rightstring[3]=='w'){
					  users[b] = insert(users[b], file_name, boss,0,1);
					 }
			 	}
			 	else if(strcmp(users[b].group, ggroup)!=0){
			 		 if(rightstring[4]=='r' && rightstring[5]=='w'){
					  users[b] = insert(users[b], file_name, boss,1,1);		 
					 }
					 else if (rightstring[4]=='r' && rightstring[5]=='-'){
					  users[b] = insert(users[b], file_name, boss,1,0);
					 }
					 else if (rightstring[4]=='-' && rightstring[5]=='w'){
					  users[b] = insert(users[b], file_name, boss,0,1);
					 }
			 	}
			}
		 }
		 FILE* file = fopen(file_name, "w");
		 fclose(file);	
		 show(users[0]); printf("\n\n");
	 	 show(users[1]); printf("\n\n");
		 show(users[2]); printf("\n\n");
		 show(users[3]); printf("\n\n");
		 show(users[4]); printf("\n\n");
		 show(users[5]); printf("\n\n\n\n");	
	 }
	
	 //read operation
	 else if(strcmp(behavior, "read")==0){
		 char buff[256] = {};
		 FILE* file = NULL;
		 file = fopen(file_name, "rb");
		 puts("Reading!");
		 sleep(1);
		 if(file==NULL)//若server無此檔案
			{	puts("The file doesn't exist!!!!");
				send(forClientSockfd, "The file doesn't exist!!!!",26,0);
			}
		 else{	 printf("2");
			 struct fileobject* temp = NULL;
			 temp = users[0].first_object;//檢查是否有權限
			 while(temp!=NULL){
				  if(strcmp(temp->filename, file_name)==0){
				  	break;
				   }
			   temp = temp->next;
			   
			  }
			  if(temp ==NULL){
				  puts("You have no right.(1)");
				  char message[] = {"You have no right."};
				  send(forClientSockfd, message, sizeof(message),0);
				  fclose(file);
			  }
			  else if(temp->read_right!=1){
				  puts("You have no right.(2)");		 
				  char message[] = {"You have no right."};
				  send(forClientSockfd, message, sizeof(message),0);
				  fclose(file);
			  }
			  else
			  {	 /*printf("Start reading,please waiting");
				 從filearray找file
			  	 int i = 0;
			  	 for(i=0;i<60;i++){
			  	 	if(strcmp(filearray[i].name, file_name)==0){
			  	 		break;
			  	 	}
			  	 }
				 printf("第幾個檔案 : %d",i);
			  	 互斥 read
			  	 sem_wait(&filearray[i].z);
			  	 sem_wait(&filearray[i].rsem);
			  	 sem_wait(&filearray[i].x);
			  	 filearray[i].readcnt = filearray[i].readcnt + 1;
			  	 if(filearray[i].readcnt==1){sem_wait(&filearray[i].wsem);}
			  	 sem_post(&filearray[i].x);
			  	 sem_post(&filearray[i].rsem);
			  	 sem_post(&filearray[i].z);*/		  	 
				 int Size = fread(buff, 1 , 256, file);
				 puts("Sending");
				 sleep(0.5);
				 if(Size==0){			 	 
				 	 char message[] = {"empty\n"};
				  	 send(forClientSockfd, message, sizeof(message),0);
					 puts("Sending Success.(1)");
					 fclose(file);
				 }
				 else{
					 send(forClientSockfd,buff,Size,0);
					 puts("Sending Success.(2)");
					 fclose(file);
				 }
				 /*sem_wait(&filearray[i].x);
				 filearray[i].readcnt = filearray[i].readcnt -1;
				 if(filearray[i].readcnt==0){sem_post(&filearray[i].wsem);}
				 sem_post(&filearray[i].x);
				 char message[] = {};
				 send(forClientSockfd, message, sizeof(message),0);
			 	 printf("Finish reading");*/
			  }
		}
		 
	}

	 //write operation		
	else if(strcmp(behavior, "write")==0){
		char* oa = strtok(NULL, " ");//切出oa
		char receiveMessage[1024] = {};
		int size = recv(forClientSockfd, receiveMessage, sizeof(receiveMessage), 0);//接收來自client的資訊（有沒有檔）
		if(strcmp(receiveMessage, "The file doesn't exist!!!!\n")==0){//client沒有此file, 告訴server不用做
			printf("The client doesn't have the file.\n\n");
		}
		else{//有此file
			 struct fileobject* temp = users[0].first_object;//檢查cap_list權限
			 while(temp!=NULL){
				  if(strcmp(temp->filename, file_name)==0){
				  	break;
				  }
				  temp = temp->next;
			 }
			 if(temp==NULL){
				 printf("You have no right.\n");
				 char message[] = {"You have no right.\n\n"};
				 send(forClientSockfd, message, sizeof(message),0);//告訴client你沒有權限寫
			 }
			 else if(temp->write_right!=1){//若沒有此node或沒權限
				 printf("You have no right.\n");
				 char message[] = {"You have no right.\n\n"};
				 send(forClientSockfd, message, sizeof(message),0);//告訴client你沒有權限寫
			 }
			 else{	 /*printf("Start writing");
			 	 //從filearray找file
				  	 int i = 0;
				  	 for(i=0;i<filenum;i++){
				  	 	if(strcmp(filearray[i].name, file_name)==0){
				  	 		break;
				  	 	}
				  	 }
				 printf("第幾個file : %d",i);
				 sem_wait(&filearray[i].y);
				 filearray[i].wrtcnt = filearray[i].wrtcnt + 1;
				 if(filearray[i].wrtcnt==1){sem_wait(&filearray[i].rsem);}
				 sem_post(&filearray[i].y);
				 sem_wait(&filearray[i].wsem);
				 sleep(10);*/
				 char message[] = {"You can write.\n"};
				 send(forClientSockfd, message, sizeof(message),0);//告訴client你有權限可以寫
				 size = recv(forClientSockfd, receiveMessage, sizeof(receiveMessage), 0);//接收檔案資訊
				 FILE* file = NULL;
				 	if(receiveMessage[0]=='e'&&receiveMessage[1]=='m'&&receiveMessage[2]=='p'){
				 		file = fopen(file_name, "w");
						printf("0 byte file\n");
					 	fclose(file);
				 	}
				 	else{
						if(strcmp(oa, "o")==0){
							file = fopen(file_name, "wb");
							fwrite(receiveMessage, sizeof(char), size, file);
						}
						else{
							file = fopen(file_name, "ab");
							fwrite(receiveMessage, sizeof(char), size, file);
						}
						fclose(file);
					
			 		}
			 		/*sem_wait(&filearray[i].y);
			 		filearray[i].wrtcnt = filearray[i].wrtcnt -1;
			 		if(filearray[i].wrtcnt==0){sem_post(&filearray[i].rsem);}
			 		sem_post(&filearray[i].wsem);
			 		sem_post(&filearray[i].y);
			 		send(forClientSockfd, message, sizeof(message),0);
			 		printf("Finish writing");*/
		 	}		
		}
	}
	
	//changemode
	else if(strcmp(behavior, "changemode")==0){//current user Kenny
		char* rightstring = strtok(NULL, " ");//將rightstring切出來
		struct fileobject* temp;//指向我要的node
		for(int i = 0;i<6;i++){	 
			 temp = users[i].first_object;
			 temp = find_node(users[i], file_name);
			 if(temp != NULL){break;}
		}
		char* boss = temp->owner;//找出owner名字放入boss
		printf("欲更改檔所有者 : %s\n",boss);
		if(strcmp(users[0].username, boss)!=0){ //看看Kenny是不是owner, 不是不准改
			printf("You are not the owner of the file.\n");
			continue;
		}
		char* ggroup = NULL;//找出owner屬於哪個group放入ggroup
		for(int i=0;i<6;i++){
			if(strcmp(boss,users[i].username)==0){
			ggroup = users[i].group;
			break;}
		}
		printf("所有者的group : %s\n",ggroup);
		
		for(int j=0; j<6; j++){
			struct fileobject* file_in_user = find_node(users[j], file_name);//在user下找此file的node
			if(strcmp(users[j].group, ggroup)!=0){
				if(rightstring[4]=='-'&&rightstring[5]=='-'){
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					
				}
				else if(rightstring[4]=='-'&&rightstring[5]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}
					else{users[j] = insert(users[j], file_name, boss,0,1);}
				}
				else if(rightstring[4]=='r'&&rightstring[5]=='-'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}
					else{users[j] = insert(users[j], file_name, boss,1,0);}
				}
				else if(rightstring[4]=='r'&&rightstring[5]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}
					else{users[j] = insert(users[j], file_name, boss,1,1);}
				}
				
			}
			else if(strcmp(boss,users[j].username)!=0 && strcmp(users[j].group, ggroup)==0){
				if(rightstring[2]=='-'&&rightstring[3]=='-'){
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					
				}
				else if(rightstring[2]=='-'&&rightstring[3]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}
					else{users[j] = insert(users[j], file_name, boss,0,1);}
				}
				else if(rightstring[2]=='r'&&rightstring[3]=='-'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}
					else{users[j] = insert(users[j], file_name, boss,1,0);}
				}
				else if(rightstring[2]=='r'&&rightstring[3]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}
					else{users[j] = insert(users[j], file_name, boss,1,1);}
				}
			
			}
			else{	
				if(rightstring[0]=='-'&&rightstring[1]=='-'){
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					
				}
				else if(rightstring[0]=='-'&&rightstring[1]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}
					else{users[j] = insert(users[j], file_name, boss,0,1);}
				}
				else if(rightstring[0]=='r'&&rightstring[1]=='-'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}
					else{users[j] = insert(users[j], file_name, boss,1,0);}
				}
				else if(rightstring[0]=='r'&&rightstring[1]=='w'){
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}
					else{users[j] = insert(users[j], file_name, boss,1,1);}
				}
			
			}
		}
		
	 	show(users[0]); printf("\n");
	 	show(users[1]); printf("\n");
		show(users[2]); printf("\n");
		show(users[3]); printf("\n");
		show(users[4]); printf("\n");
		show(users[5]); printf("\n\n\n");
	}
	
	//show
	else if(strcmp(behavior, "show")==0){
		show(users[0]); printf("\n");
	 	show(users[1]); printf("\n");
		show(users[2]); printf("\n");
		show(users[3]); printf("\n");
		show(users[4]); printf("\n");
		show(users[5]); printf("\n\n\n");	
	}

	
	
    }
    return 0;
}


