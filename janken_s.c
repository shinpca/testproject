/*4I 15番 齋藤新之助*/
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<netdb.h>
#include<sys/types.h>
#include<signal.h>
#include<pthread.h>

#define BUF_SIZE 64                   /*バッファのサイズ*/
int ssock=0;
/*終了時にsocketをcloseする。*/
void sigFunction(int sigNumber)
{
  if(sigNumber == 2)
    {
      puts("A program is exited...");
      close(ssock);
      exit(1);
    }
  else
    exit(1);
}


int main() {


  int temp,read_size,csock,p_hand,c_hand,check=0;
  char buf[64];
  char result[5];
  char draw[5] = {'D','r','a','w','\0'};
  char win[5] = {'W','i','n','!','\0'};
  char lose[5] = {'L','o','s','e','\0'};
  struct sockaddr_in saddr;
  struct sockaddr_in caddr;
  struct sigaction sigInthandl;
  int adlen = sizeof(caddr);
  sigInthandl.sa_handler = sigFunction; /*ハンドラ関数として指定*/
  sigInthandl.sa_flags = 0; /*フラグなし*/
  /*全シグナルをマスクするマスクを作成*/
  if(sigfillset(&sigInthandl.sa_mask) < 0) {
    printf("Error:sigfillset is failure.\n");
    exit(1);
  }
  /*割り込みシグナルに対する処理を設定*/
  if(sigaction(SIGINT,&sigInthandl,0) < 0) {
    printf("Error:sigaction is failure.\n");
    exit(1);
  }


  ssock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);   /*ソケットを生成*/

  memset(&saddr, 0 ,sizeof(saddr));                  /*初期化*/
  saddr.sin_family = AF_INET;                        /*Inetnetを指定*/
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);         /*アドレス設定*/
  saddr.sin_port = htons(4200);                      /*ポート番号指定*/
  /*コネクションを受けつけるIPアドレス、ポート番号とソケットとを対応づけ*/
  temp  =  bind(ssock, (struct sockaddr*) &saddr, sizeof(saddr));
  /*ネットワークの障害を検知*/
  if(temp == -1)
    {
      printf("Network Error");
      exit(1);
    }
  listen(ssock,10);
  while(1) {
    csock = accept(ssock, (struct sockaddr *)&caddr, &adlen);
    check = recv(csock, buf, 15, 0);
    printf("a");
    p_hand = atoi(buf);
    c_hand = rand() % 3 + 1;
    
    if(c_hand == p_hand){
      strcpy(result, draw);
    } else if(p_hand == 1 && c_hand == 2){
      strcpy(result, win);
    } else if(p_hand == 2 && c_hand == 3){
      strcpy(result, win);
    } else if(p_hand == 3 && c_hand == 1){
      strcpy(result, win);
    } else{
      strcpy(result, lose);

}
    if(check != -1)
      send(csock,result,strlen(result),0);
    else;
    close(csock);
  }
  close(ssock);
  exit(EXIT_SUCCESS);
}
