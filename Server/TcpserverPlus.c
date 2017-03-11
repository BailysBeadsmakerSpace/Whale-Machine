// 作者：Gavin 林智杰
// 团队：贝利珠海贼团
// 项目: 鲸鱼刷卡机
// 学校：北京理工大学珠海学院
// 邮箱：543541941@qq.com

// TCP服务端说明：
//本程序是用C语言开发的，利用socket开发技术，接收硬件传过来的数据
//因为C语言开发的时候多线程比较麻烦，所以就写了个单线程demo.
//单线程可能会遇到的情况是，可能会出现丢包情况。
//建议用java(或其他语言)写多线程应用。
//本程序是在Linux下运行的。              
#include <stdio.h>  
#include <stdlib.h>  
#include <string.h>  
#include <unistd.h>  
#include <sys/types.h>  
#include <sys/socket.h>  
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include "mysql.h"
#define  PORT 6000  //设定端口号 
#define  BACKLOG 2  
void connmysql(char *NO,char *CardNO,char *Money,char *repaly);
int main()  
{  
       int  listenfd, connectfd;  
       int i,j;
       char NO[13],CardNO[4],Money[4],repaly[6];
       struct  sockaddr_in server;  
       struct  sockaddr_in client;  
       socklen_t  addrlen;  
       int len; 
       char buf[100];  
       //初始化
       memset(buf,'\0', sizeof(buf));
       memset(repaly,'\0', sizeof(repaly));
       memset(NO,'\0', sizeof(NO));
       memset(CardNO,'\0', sizeof(CardNO));
       memset(Money,'\0', sizeof(Money));
       if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)  
       {  
              perror("Creating  socket failed.");  
              exit(1);  
       }  
       int opt =SO_REUSEADDR;  
       setsockopt(listenfd,SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));  
       bzero(&server,sizeof(server));  
       server.sin_family=AF_INET;  
       server.sin_port=htons(PORT);  
       server.sin_addr.s_addr= htonl (INADDR_ANY);  
       if(bind(listenfd, (struct sockaddr *)&server, sizeof(server)) == -1) {  
              perror("Binderror.");  
              exit(1);  
       }     
       if(listen(listenfd,BACKLOG)== -1){  /* calls listen() */  
              perror("listen()error\n");  
              exit(1);  
       }  
       addrlen =sizeof(client);  
       while(1)
       {
              if((connectfd = accept(listenfd,(struct sockaddr*)&client,&addrlen))==-1) {  
                     perror("accept()error\n");  
                     exit(1);  
              } 
              while(strcmp(buf,"bye"))
               {
                      if ( (len =  recv( connectfd, buf, 100, 0 )) > 0  )
                      {
                          buf[len] = '\0';
                          for(i=0;i<12;i++)
                          {
                              NO[i]=buf[i];
                          }
                          for(j=0;j<3;j++)
                          {
                              CardNO[j]=buf[i];
                              i++;
                          }
                          for (j = 0; j<3; j++)
                          {
                              Money[j]=buf[i];
                              i++;
                          }
                          connmysql(NO,CardNO,Money,repaly);
                      }else if( len == 0 ){
                          printf(" Remote client close\n");
                          break;
                      }
                     send(connectfd,repaly,sizeof(repaly),0);
               }  
              close(connectfd);  
       }
       close(listenfd);  
       return 0;  
 }  
void connmysql(char *NO,char *CardNO,char *Money,char *repaly)
{
    int i,j;
     char sql[100];
     char State_ser[2]={'\0'};
     char money_ser[4]={'\0'};
     memset(sql,'\0', sizeof(sql));
     int t;
     sprintf(sql,"select state,money from us where cardid=%s",NO);
     MYSQL *conn=mysql_init(NULL);
     MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
     MYSQL_ROW row; //一个行数据的类型安全(type-safe)的表示
     if(!mysql_real_connect(conn,"填入主机地址","数据库账户","数据库密码","US", 0, NULL, 0) )
     {
            printf("连接数据库失败！\n%s\n",mysql_error(conn));
     }
     t=mysql_real_query(conn,sql,(unsigned int)strlen(sql));
     if(t)
     {
            printf("执行出现异常:%s\n",mysql_error(conn));
     }
     res=mysql_store_result(conn);
     while(row=mysql_fetch_row(res))
     {
         strcpy(State_ser,row[0]);
         strcpy(money_ser,row[1]);
     }
     if(!atoi(State_ser))
     {
        memset(sql,'\0', sizeof(sql));
        int m=atoi(money_ser)-atoi(Money);
        sprintf(sql,"update us set money=%d,cardno=%s where cardid=%s",m,CardNO,NO);
         t=mysql_real_query(conn,sql,(unsigned int)strlen(sql));
         if(t)
         {
                printf("执行出现异常:%s\n",mysql_error(conn));
         }
         sprintf(repaly,"%d",m);
      }else{
        memset(sql,'\0', sizeof(sql));
        sprintf(sql,"update us set state=%d where cardid=%s",1,NO);
         t=mysql_real_query(conn,sql,(unsigned int)strlen(sql));
         if(t)
         {
                printf("执行出现异常:%s\n",mysql_error(conn));
         }
         strcpy(repaly,"9999");
    }
     mysql_free_result(res);//释放结果集使用的内存。
     mysql_close(conn);
}

