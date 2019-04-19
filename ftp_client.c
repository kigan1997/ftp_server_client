#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <readline/history.h>


int main(int argc, char **argv)
{
	if(argc==1) 
        printf("\nPlease give <IP Address> <Port>\n");

	int sd,file_size;
	struct stat	obj;
	char buf[100],buf1[300], *type, *filename, path[50];//, input[20];
	char* buff, *input, *buff1;
	//sprintf(buf,"GET /%s HTTP/1.1\r\n\r\nHost: %s:%s",argv[3],argv[1],argv[2]);
	struct sockaddr_in serv_addr,client_addr;
	serv_addr.sin_family = AF_INET;       
	serv_addr.sin_port = htons(atoi(argv[2]));     
	inet_aton(argv[1],&(serv_addr.sin_addr));
	bzero(&(serv_addr.sin_zero), 8);

	sd=socket(AF_INET,SOCK_STREAM,0);
	printf("Socket descriptor: %d\n",sd);
	//bind(sd,(struct sockaddr *)&client_addr, sizeof(struct sockaddr));

	
	printf("Connect:%d\n",connect(sd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr)));



	while(1){
		file_size = obj.st_size = 0;
		//printf("Enter your request:\n");
		/*memset(path,0,50);
		recv(sd,path,sizeof(path),0);
		printf("%s>> ",path);*/
		// printf("bal\n");
		input = readline("ftp>> ");
		// gets(input);
		// printf("bal\n");
		// scanf("%[\n]s",input);
		printf("%s\n",input);
		if (strlen(input) != 0) {
			add_history(input);
			// type = strsep(&input," ");
			type = strtok(input," ");
			// printf("%s\n",type);
			// filename = strsep(&input," ");
			filename = strtok(NULL," ");
			// printf("%s\n",filename);
		}
		else continue;
// printf("%s %s\n",type,filename );
		if(strstr(type,"bye") != NULL){
			sprintf(buf,"%s %s",type,type);
			send(sd,buf,sizeof(buf),0);
			break;
		}
		/*scanf("%s",filename);*/ 
		else if(strstr(type,"pwd") != NULL || strstr(type,"ls") != NULL)
			sprintf(buf,"%s %s ",type,type);
		else
			sprintf(buf,"%s %s ",type,filename);
		printf("%s\n",buf);
		printf("Send request:%ld\n",send(sd,buf,sizeof(buf),0));

		if(strstr(type,"cd") != NULL){
			memset(buf,0,50);
			recv(sd,buf,sizeof(buf),0);
			printf("%s\n",buf);
			free(input);
			continue;
		}
		else if(strstr(type,"rm") != NULL || strstr(type,"mkdir") != NULL){
			free(input);
			continue;
		}
		else if(strstr(type,"pwd") != NULL){
			memset(buf,0,50);
			recv(sd,buf,sizeof(buf),0);
			printf("%s\n",buf);
			memset(buf,0,50);
			free(input);
			continue;
		}
		else if(strstr(type,"ls") != NULL){// || strstr(type,"pwd") != NULL){
			recv(sd, &file_size, sizeof(int), 0);
			// free(buff);
			printf("%d\n",file_size );
			buff1 = malloc(file_size);
			recv(sd,buff1,file_size,0);
			// printf("%s\n",buff);
			puts(buff1);
			free(buff1);
			file_size = obj.st_size = 0;
			free(input);

			continue;
		}

		memset(buf,0,sizeof(buf));
		if(recv(sd,buf,sizeof(buf),0)>0){
			printf("%s\n",buf);
		}

		if(strstr(buf,"OK") != NULL && strstr(type,"get") != NULL){
			/*strcpy(buf,"recv_");
			char *recv_fname = buf, *fname = filename;//argv[3];
			strcat(recv_fname,fname);
			printf("%s\n",recv_fname);
			int fd = open(recv_fname, O_RDWR | O_CREAT);
			strcpy(fname,"inverted_");
			strcat(fname,recv_fname);*/
			//int fd2 = open(fname, O_WRONLY | O_CREAT);
			memset(buf,0,sizeof(buf));
			/*int fd = open(filename, O_RDWR | O_CREAT);
			while(recv(sd,buf1,sizeof(buf1),0)>0){
				printf("%s\n",buf1);
				write(fd,buf1,sizeof(buf1));
				memset(buf1,0,sizeof(buf1));
			}
			close(fd);*/
			recv(sd, &file_size, sizeof(int), 0);
			printf("%d\n",file_size);
			buff = malloc(file_size);
			FILE* fp1;
			fp1 = fopen(filename,"w");
			if(fp1 == NULL) printf("cazz\n");
			else{
				recv(sd,buff,file_size,0);
				fwrite(buff,sizeof(char),file_size,fp1);
			}
				/*while(recv(sd,buf1,sizeof(buf1),)>0){
				//write(fd,buf1,sizeof(buf1));
				printf("%s\n",buf1);
				fwrite(buf1,sizeof(char),sizeof(buf1),fp1);
				memset(buf1,0,sizeof(buf1));
				}*/
			free(buff);
			file_size = obj.st_size = 0;
			fclose(fp1);
			printf("Recieved\n");
			free(input);
			
		}
		else if(strstr(buf,"OK") != NULL && strstr(type,"put") != NULL){
			memset(buf,0,sizeof(buf));

			stat(filename, &obj);
			file_size = obj.st_size;
			send(sd,&file_size, sizeof(int), 0);

			int fd = open(filename, O_RDONLY | O_CREAT);
			//while(sendfile(sd,fd,NULL,sizeof(buf1))>0);
			sendfile(sd,fd,NULL,file_size);
			file_size = obj.st_size = 0;
			close(fd);
			/*FILE* fp1;
			fp1 = fopen(filename,"r");
			if(fp1 == NULL) printf("cazz\n");
			else
				while(!feof){
				//write(fd,buf1,sizeof(buf1));
				fscanf(fp1,"%s",buf1);
				//printf("%s\n", buf1);
				send(sd,buf1,sizeof(buf1),0);
				memset(buf1,0,sizeof(buf1));
				}
			fclose(fp1);*/
			printf("Sent\n");
			free(input);
			
		}
		else
			exit(1);

	}
	
	close(sd);
	//accept(sd, (struct sockaddr *)&client_addr,(int *) sizeof(struct sockaddr));
}