#define USER_EXEC(PORT,USERNUMBER)  \
\
    int sockfd = 0,forClientSockfd = 0;\
    sockfd = socket(AF_INET , SOCK_STREAM , 0);\
\
    if (sockfd == -1){\
        printf("Fail to create a socket.");\
    }\
    struct sockaddr_in serverInfo,clientInfo;\
    int addrlen = sizeof(clientInfo);\
    bzero(&serverInfo,sizeof(serverInfo));\
\
    serverInfo.sin_family = PF_INET;\
    serverInfo.sin_addr.s_addr = INADDR_ANY;\
    serverInfo.sin_port = htons(PORT);\
    bind(sockfd,(struct sockaddr *)&serverInfo,sizeof(serverInfo));\
    listen(sockfd,5);\
    while(1){\
	char inputBuffer[256] = {};\
	if(forClientSockfd==0)\
	 {\
	 	forClientSockfd = accept(sockfd,(struct sockaddr*) &clientInfo, &addrlen);\
	 }\
	 recv(forClientSockfd,inputBuffer,sizeof(inputBuffer),0);\
       	 if(inputBuffer[strlen(inputBuffer) - 1] == '\n'){\
          inputBuffer[strlen(inputBuffer) - 1] = '\0';\
         }\
       	 if(strcmp("exit", inputBuffer) == 0){\
           	return 0;\
         }\
	 char* behavior = strtok(inputBuffer, " ");\
	 char* file_name = strtok(NULL, " ");\
	 if(strcmp(behavior, "create")==0){\
		 char* rightstring = strtok(NULL, " ");\
		 char* boss = users[USERNUMBER].username;\
		 char* ggroup = users[USERNUMBER].group;\
		 printf("rightstring : %s\n",rightstring);\
		 printf("創檔人 : %s\n",boss);\
		 printf("創檔人所屬的group : %s\n\n",ggroup);\
		 if(rightstring==NULL)puts("Right set error.");\
		 else{\
			 for(int b=0;b<6;b++){\
				if(strcmp(users[b].username, boss)==0){\
					 if(rightstring[0]=='r' && rightstring[1]=='w'){\
					  users[b] = insert(users[b], file_name, boss,1,1);		 \
					 }\
					 else if (rightstring[0]=='r' && rightstring[1]=='-'){\
					  users[b] = insert(users[b], file_name, boss,1,0);\
					 }\
					 else if (rightstring[0]=='-' && rightstring[1]=='w'){\
					  users[b] = insert(users[b], file_name, boss,0,1);\
					 }\
			 	}\
			 	 else if(strcmp(users[b].group,ggroup)==0 && strcmp(users[b].username, boss)!=0){\
			 	 	 if(rightstring[2]=='r' && rightstring[3]=='w'){\
					  users[b] = insert(users[b], file_name, boss,1,1);		 \
					 }\
					 else if (rightstring[2]=='r' && rightstring[3]=='-'){\
					  users[b] = insert(users[b], file_name, boss,1,0);\
					 }\
					 else if (rightstring[2]=='-' && rightstring[3]=='w'){\
					  users[b] = insert(users[b], file_name, boss,0,1);\
					 }\
			 	}\
			 	else if(strcmp(users[b].group, ggroup)!=0){\
			 		 if(rightstring[4]=='r' && rightstring[5]=='w'){\
					  users[b] = insert(users[b], file_name, boss,1,1);		 \
					 }\
					 else if (rightstring[4]=='r' && rightstring[5]=='-'){\
					  users[b] = insert(users[b], file_name, boss,1,0);\
					 }\
					 else if (rightstring[4]=='-' && rightstring[5]=='w'){\
					  users[b] = insert(users[b], file_name, boss,0,1);\
					 }\
			 	}\
			}\
		 }\
		 FILE* file = fopen(file_name, "w");\
		 fclose(file);	\
		 show(users[0]); printf("\n\n");\
	 	 show(users[1]); printf("\n\n");\
		 show(users[2]); printf("\n\n");\
		 show(users[3]); printf("\n\n");\
		 show(users[4]); printf("\n\n");\
		 show(users[5]); printf("\n\n\n\n");	\
	 }\
	 else if(strcmp(behavior, "read")==0){\
		 char buff[256] = {};\
		 FILE* file = NULL;\
		 file = fopen(file_name, "rb");\
		 puts("Reading!");\
		 sleep(1);\
		 if(file==NULL)\
			{	puts("The file doesn't exist!!!!");\
				send(forClientSockfd, "The file doesn't exist!!!!",26,0);\
			}\
		 else{\
			 struct fileobject* temp = NULL;\
			 temp = users[USERNUMBER].first_object;\
			 while(temp!=NULL){\
				  if(strcmp(temp->filename, file_name)==0){\
				  	break;\
				   }\
			   temp = temp->next;\
			   \
			  }\
			  if(temp ==NULL){\
				  puts("You have no right.(1)");\
				  char message[] = {"You have no right."};\
				  send(forClientSockfd, message, sizeof(message),0);\
				  fclose(file);\
			  }\
			  else if(temp->read_right!=1){\
				  puts("You have no right.(2)");		 \
				  char message[] = {"You have no right."};\
				  send(forClientSockfd, message, sizeof(message),0);\
				  fclose(file);\
			  }\
			  else\
			  {\
				 int Size = fread(buff, 1 , 256, file);\
				 puts("Sending");\
				 sleep(0.5);\
				 if(Size==0){			 	 \
				 	 char message[] = {"empty\n"};\
				  	 send(forClientSockfd, message, sizeof(message),0);\
					 puts("Sending Success.(1)");\
					 fclose(file);\
				 }\
				 else{\
					 send(forClientSockfd,buff,Size,0);\
					 puts("Sending Success.(2)");\
					 fclose(file);\
				 }\
			  }\
			}\
	}\
		else if(strcmp(behavior, "write")==0){\
		char* oa = strtok(NULL, " ");\
		char receiveMessage[1024] = {};\
		int size = recv(forClientSockfd, receiveMessage, sizeof(receiveMessage), 0);\
		if(strcmp(receiveMessage, "The file doesn't exist!!!!\n")==0){\
			printf("The client doesn't have the file.\n\n");\
		}\
		else{\
			 struct fileobject* temp = users[USERNUMBER].first_object;\
			 while(temp!=NULL){\
				  if(strcmp(temp->filename, file_name)==0){\
				  	break;\
				  }\
				  temp = temp->next;\
			 }\
			 if(temp==NULL){\
				 printf("You have no right.\n");\
				 char message[] = {"You have no right.\n\n"};\
				 send(forClientSockfd, message, sizeof(message),0);\
			 }\
			 else if(temp->write_right!=1){\
				 printf("You have no right.\n");\
				 char message[] = {"You have no right.\n\n"};\
				 send(forClientSockfd, message, sizeof(message),0);\
			 }\
			 else{\
				 char message[] = {"You can write.\n"};\
				 send(forClientSockfd, message, sizeof(message),0);\
				 size = recv(forClientSockfd, receiveMessage, sizeof(receiveMessage), 0);\
				 FILE* file = NULL;\
				 	if(receiveMessage[0]=='e'&&receiveMessage[1]=='m'&&receiveMessage[2]=='p'){\
				 		file = fopen(file_name, "w");\
						printf("0 byte file\n");\
					 	fclose(file);\
				 	}\
				 	else{\
						if(strcmp(oa, "o")==0){\
							file = fopen(file_name, "wb");\
							fwrite(receiveMessage, sizeof(char), size, file);\
						}\
						else{\
							file = fopen(file_name, "ab");\
							fwrite(receiveMessage, sizeof(char), size, file);\
						}\
						fclose(file);					\
			 		}\
		 	}\
		}\
	}\
	else if(strcmp(behavior, "changemode")==0){\
		char* rightstring = strtok(NULL, " ");\
		struct fileobject* temp;\
		for(int i = 0;i<6;i++){	 \
			 temp = users[i].first_object;\
			 temp = find_node(users[i], file_name);\
			 if(temp != NULL){break;}\
		}\
		char* boss = temp->owner;\
		printf("欲更改檔所有者 : %s\n",boss);\
		if(strcmp(users[USERNUMBER].username, boss)!=0){\
			printf("You are not the owner of the file.\n");\
			continue;\
		}\
		char* ggroup = NULL;\
		for(int i=0;i<6;i++){\
			if(strcmp(boss,users[i].username)==0){\
			ggroup = users[i].group;\
			break;}\
		}\
		printf("所有者的group : %s\n",ggroup);\
		\
		for(int j=0; j<6; j++){\
			struct fileobject* file_in_user = find_node(users[j], file_name);\
			if(strcmp(users[j].group, ggroup)!=0){\
				if(rightstring[4]=='-'&&rightstring[5]=='-'){\
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					\
				}\
				else if(rightstring[4]=='-'&&rightstring[5]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}\
					else{users[j] = insert(users[j], file_name, boss,0,1);}\
				}\
				else if(rightstring[4]=='r'&&rightstring[5]=='-'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}\
					else{users[j] = insert(users[j], file_name, boss,1,0);}\
				}\
				else if(rightstring[4]=='r'&&rightstring[5]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}\
					else{users[j] = insert(users[j], file_name, boss,1,1);}\
				}\
				\
			}\
			else if(strcmp(boss,users[j].username)!=0 && strcmp(users[j].group, ggroup)==0){\
				if(rightstring[2]=='-'&&rightstring[3]=='-'){\
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					\
				}\
				else if(rightstring[2]=='-'&&rightstring[3]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}\
					else{users[j] = insert(users[j], file_name, boss,0,1);}\
				}\
				else if(rightstring[2]=='r'&&rightstring[3]=='-'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}\
					else{users[j] = insert(users[j], file_name, boss,1,0);}\
				}\
				else if(rightstring[2]=='r'&&rightstring[3]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}\
					else{users[j] = insert(users[j], file_name, boss,1,1);}\
				}\
			\
			}\
			else{	\
				if(rightstring[0]=='-'&&rightstring[1]=='-'){\
					if(file_in_user!=NULL){users[j] = del_node(users[j], file_name);}					\
				}\
				else if(rightstring[0]=='-'&&rightstring[1]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 0,1);}\
					else{users[j] = insert(users[j], file_name, boss,0,1);}\
				}\
				else if(rightstring[0]=='r'&&rightstring[1]=='-'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,0);}\
					else{users[j] = insert(users[j], file_name, boss,1,0);}\
				}\
				else if(rightstring[0]=='r'&&rightstring[1]=='w'){\
					if(file_in_user!=NULL){setnode_right(file_in_user, 1,1);}\
					else{users[j] = insert(users[j], file_name, boss,1,1);}\
				}\
			\
			}\
		}\
	 	show(users[0]); printf("\n");\
	 	show(users[1]); printf("\n");\
		show(users[2]); printf("\n");\
		show(users[3]); printf("\n");\
		show(users[4]); printf("\n");\
		show(users[5]); printf("\n\n\n");\
	}\
	else if(strcmp(behavior, "show")==0){\
		show(users[0]); printf("\n");\
	 	show(users[1]); printf("\n");\
		show(users[2]); printf("\n");\
		show(users[3]); printf("\n");\
		show(users[4]); printf("\n");\
		show(users[5]); printf("\n\n\n");	\
	}	\
    }\
