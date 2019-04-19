#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
/*#include <readline/readline.h>
#include <readline/history.h>*/


void response(char * fname,int new_sd){
	char buf[100];
	memset(buf,0,sizeof(buf));
	if(access(fname,F_OK) != 0){
		sprintf(buf,"Not Found");
		send(new_sd,buf,sizeof(buf),0);
		memset(buf,0,sizeof(buf));
		// printf("%s\n", buf);
		//exit(1);
	}
	else if(access(fname,R_OK) != 0){
		sprintf(buf,"Forbideen");
		send(new_sd,buf,sizeof(buf),0);
		memset(buf,0,sizeof(buf));
		// printf("%s\n", buf);
		//exit(1);
	}
	else{
		sprintf(buf,"OK");
		send(new_sd,buf,sizeof(buf),0);
		memset(buf,0,sizeof(buf));
		// printf("%s\n", buf);
	}
}


void chn_dir(char * fname,int new_sd){
	char buf[100];
	memset(buf,0,sizeof(buf));
	if(access(fname,F_OK) != 0){
		sprintf(buf,"Folder Not Found");
		send(new_sd,buf,sizeof(buf),0);
		memset(buf,0,sizeof(buf));
		// printf("%s\n", buf);
		//exit(1);
	}
	else{
		sprintf(buf,"Folder Changed");
		send(new_sd,buf,sizeof(buf),0);
		chdir(fname);
		memset(buf,0,sizeof(buf));
		// printf("%s\n", buf);
	}
}

int main(int argc, char **argv)
{
	if(argc==1) 
        printf("\nPlease give <PORT NO>\n");
    char path[100];
	

	int sd,fd,file_size;
	struct stat	obj;
	char buf[100],buf1[300],buf2[300];
	char * buff;
	char *get,*fname,fname1[20];
	struct sockaddr_in serv_addr,client_addr2,client_addr;
	serv_addr.sin_family = AF_INET;       
	serv_addr.sin_port = htons(atoi(argv[1]));
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	bzero(&(serv_addr.sin_zero), 8);


	sd=socket(AF_INET,SOCK_STREAM,0);
	if(sd >= 0)
		printf("Socket descriptor: %d\n",sd);
	else perror("Failed\n");
	
	;
	printf("bind:%d\n",bind(sd,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr)));
	printf("listen:%d\n",listen(sd,4));

	int len = sizeof(struct sockaddr_in);
	int new_sd = accept(sd, (struct sockaddr *)&client_addr,&len);

	while(1){
		file_size = obj.st_size = 0;
		/*memset(path,0,100);
		getcwd(path,sizeof(path));
		send(new_sd,path,sizeof(path),0);*/
		printf("Ping me..\n");
		if(recv(new_sd,buf,sizeof(buf),0)>0){
			printf("Request:%s\n", buf);
		}

		get = strtok(buf," ");
		//if(strcmp(get, "get") != 0 || strcmp(get, "put") != 0 || strcmp(get, "bye") == 0) exit(1);
		int status;
		fname = strtok(NULL," ");
		strcpy(fname1,fname);
		//if(fname[0] == '/') fname++;
		printf("Request for:%s\n", fname);

		if(strstr(get,"bye") != NULL)
			break;
		else if(strstr(get,"cd") != NULL){
			// printf("%s\n",fname);
			chn_dir(fname,new_sd);
			// printf("%s\n",fname);strstr(get,"pwd") != NULL || 			
			continue;
		}
		else if(strstr(get,"rm") != NULL || strstr(get,"mkdir") != NULL){
			strcat(get," ");
			strcat(get,fname1);
			puts(get);
			system(get);			
			continue;
		}
		else if(strstr(get,"pwd") != NULL){
			memset(path,0,100);
			getcwd(path,sizeof(path));
			//path = malloc(50);
			// path = get_current_dir_name();
			printf("b:%s\n",path);
			path[strlen(path)] = '\0';
			printf("a:%s\n",path);
			// path[strlen(path)] = '\0';
			send(new_sd,path,50,0);
			// free(path);
			//strcat(get," > pwd.txt");
			/*system("pwd > pwd.txt");
			// execv(get,NULL);
			FILE* fp1;
			fp1 = fopen("pwd.txt","r");
			fscanf(fp1,"%s",buf1);
			buf1[strlen(buf1)] = '\0';
			send(new_sd,buf1,sizeof(buf1),0);
			fclose(fp1);
			system("rm pwd.txt");*/
			continue;
		}
		else if(strstr(get,"ls") != NULL){// || strstr(get,"pwd") != NULL){
			strcat(get," > ls.txt");
			system(get);
			// execv(get,NULL);
			stat("ls.txt", &obj);
			file_size = obj.st_size;
			send(new_sd,&file_size, sizeof(int), 0);

			int fd = open("ls.txt", O_RDONLY | O_CREAT);
			sendfile(new_sd,fd,NULL,file_size);
			close(fd);
			system("rm ls.txt");
			continue;
		}

		if(strstr(get,"get") != NULL){
			response(fname,new_sd);
			printf("Sending..\n");


			stat(fname, &obj);
			file_size = obj.st_size;
			send(new_sd,&file_size, sizeof(int), 0);

			int fd = open(fname, O_RDONLY | O_CREAT);
			//while(sendfile(new_sd,fd,NULL,sizeof(buf1))>0);
			sendfile(new_sd,fd,NULL,file_size);
			close(fd);
			/*FILE* fp1;
			fp1 = fopen(fname,"r");
			if(fp1 == NULL) printf("cazz\n");
			else
				while(!feof){
					//write(fd,buf1,sizeof(buf1));
					fscanf(fp1,"%s",buf1);
					send(new_sd,buf1,sizeof(buf1),0);
					memset(buf1,0,sizeof(buf1));
				}
			fclose(fp1);*/
			printf("Sent\n");
		}
		else if(strstr(get,"put") != NULL){
			sprintf(buf,"OK");
			send(new_sd,buf,sizeof(buf),0);
			memset(buf,0,sizeof(buf));
			/*strcpy(buf,"recv_");
			char *recv_fname = buf, *filename = fname;
			printf("%s-%s\n",filename,fname);
			strcat(recv_fname,filename);*/
			printf("Receiving..%s\n",fname1);
			/*int fd = open(fname, O_RDWR | O_CREAT);
			while(recv(new_sd,buf1,sizeof(buf1),0)>0){
				printf("%s\n",buf1);
				write(fd,buf1,sizeof(buf1));
				memset(buf1,0,sizeof(buf1));
			}*/
			recv(new_sd, &file_size, sizeof(int), 0);
			buff = malloc(file_size*sizeof(char));

			/*int fd = open(fname, O_RDWR | O_CREAT);
			recv(new_sd,buff,file_size,0);
			write(fd,buff,sizeof(buff));
			printf("Buffer:%s\n",buff);
			close(fd);*/

			FILE* fp1;
			printf("%s\n", fname1);
			fp1 = fopen(fname1,"w");
			if(fp1 == NULL) printf("cazz\n");
			else{
				recv(new_sd,buff,file_size,0);
				fwrite(buff,sizeof(char),file_size,fp1);
			}
			fclose(fp1);
			printf("Received\n");
		}

		
		
	}
	close(new_sd);
	close(sd);
}
