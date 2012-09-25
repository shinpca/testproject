#include<stdio.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>

int main(int argc,char *argv[]) {
  int sock,size=0,p_hand,t;
  char buf[64];
  char address;

  sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
  struct sockaddr_in addr;

  if(argv[1] == NULL)
    {
      printf("入力が不正です。\n");
    }


  printf("あなたの手を入力してください(グー:1 チョキ:2 パー:3)");
  scanf("%d",&p_hand);
  if(p_hand < 1 || p_hand >3)
    {
      printf("入力が不正です。\n");
      return 1;
    }
  sprintf(buf, "%d", p_hand);
  puts(buf);
  memset(&addr, 0 ,sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(argv[1]);
  addr.sin_port = htons(4200);

  if(connect(sock, (struct sockaddr*)&addr, sizeof(addr)) > -1)
    {
      send(sock,buf,100,0);
      while((size=recv(sock,buf,64,0))>0)
	{
	  fwrite(buf,size,1,stdout); 
	}
      close(sock);
    }
  else printf("NetWork Error\n");
}
