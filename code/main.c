//
//  main.c
//  图书管理系统
//
//  Created by LanXKay on 2019/9/11.
//  Copyright © 2019 LanXKay. All rights reserved.

//  Edit by LanXKay on 2019/10/27
//  根据老师的要求更改了时间的流逝方式
//  主要是增加了nowtime以及更改了appoint_solve函数

//  Edit by LanXKay on 2019/10/30
//  根据10.29课程设计题目的要求
//  更改bookcode的类型
//  book中添加权限变量
//  增加初始化时间函数
//  尝试添加注释

//  Edit by LanXKay on 2019/11/5
//  将通过外界输入的int类型变量全部改成了long int
//  通过手写输入函数，解决了scanf中的%d的warning的问题
//  粗略进行了检查，run successd

//  Edit by LanXKay on 2019/11/7
//  统计时需要考虑还未结束的借书记录的情况
//  对统计部分的函数做了修改

//  Edit by LanXKay on 2019/11/25
//  测试了老师给出的用例，基本没有问题
//  主要测试了1-5以及13

//  Edit by LanXKay on 2019/11/29
//  测试了老师给出的4个用例，全部一致

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "struct.h"
#include "function.h"
#include "menu.h"


long int NowTime[3];
long int BOOK_ID= 1;
long int BOOK_CODE= 0;
const long int notPrime_Month[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
const long int Prime_Month[12]= {31,29,31,30,31,30,31,31,30,31,30,31};

int main()
{
    
    BookS * BookS_head= (BookS *)malloc(sizeof(BookS));
    BookS_head->next1= NULL;
    Borrowed_Record * Borrowed_head= (Borrowed_Record *)malloc(sizeof(Borrowed_Record));
    Borrowed_head->next= NULL;
    Card * Card_head= (Card*)malloc(sizeof(Card));
    Card_head->next= NULL;
    
    init_time(2019, 01, 01);
    
    int exit= 1;
    while (exit){
        getcommandline();
        switch (command){
            case 1:{
                //regbooks
                int flag=1;
                long int code= strtol(commandvarieties[1], NULL, 10);
                            //atol(commandvarieties[1]);
                if (code<1){
                    printf ("FAIL\n");
                }
                else if (code <=BOOK_CODE){
                    long int quantity= strtol(commandvarieties[2], NULL, 10);
                            //atol(commandvarieties[2]);
                    if (quantity<=0)
                        flag= 0;
                    if (flag==1&& AddBooks(code, quantity, BookS_head))
                        printf ("OK\n");
                    else
                        printf ("FAIL\n");
                }
                else{
                    char name[41],author[11],publication[21],introduction[101];
                    if((strlen(commandvarieties[2])<=41)&& (strlen(commandvarieties[3])<=11)&& (strlen(commandvarieties[4])<=21)&& (strlen(commandvarieties[5])<=101)){
                        strcpy(name, commandvarieties[2]);
                        strcpy(author, commandvarieties[3]);
                        strcpy(publication, commandvarieties[4]);
                        strcpy(introduction, commandvarieties[5]);
                        long int quantity= strtol(commandvarieties[9], NULL, 10);
                                //atol(commandvarieties[9]);
                        char authority= commandvarieties[10][0];
                        long int buytime[3];
                        for (int i=0; i<3; i++){
                            buytime[i]= strtol(commandvarieties[6+i], NULL, 10);
                                    //atol(commandvarieties[6+i]);
                        }
                        if (IsValidTime(buytime)== 0){
                            flag= 0;
                        }
                        if(((authority!='S')&&(authority!='J'))||(quantity<0)){
                               flag=0;
                        }
                        if (DaysBetween2Date(NowTime, buytime)>=0){
                            flag= 0;
                        }
                        if (flag==1){
                            if (RegBooks(code, name, author, publication, introduction, authority, buytime, quantity, BookS_head))
                                printf ("OK\n");
                        }
                        else
                            printf("FAIL\n");
                    }
                    else
                        printf("FAIL\n");
                }
                break;
            }
            case 2:{
                //regcard
                int flag= 1;
                if((strlen(commandvarieties[1])==1)&& (strlen(commandvarieties[2])==10)&& (strlen(commandvarieties[3])<=10)&& ((strlen(commandvarieties[4])<=10))){
                    char Status, Code[11], Name[11], Deapartment[11];
                    Status= commandvarieties[1][0];
                    strcpy(Code, commandvarieties[2]);
                    strcpy(Name, commandvarieties[3]);
                    strcpy(Deapartment, commandvarieties[4]);
                    long int max_quantity= strtol(commandvarieties[5], NULL, 10);
                            //atol(commandvarieties[5]);
                    long int max_time= strtol(commandvarieties[6], NULL, 10);
                            //atol(commandvarieties[6]);
                    if ((max_quantity<0)||(max_time<=0)){
                        flag= 0;
                    }
                    if (flag==1 &&
                        RegCards(Status, Code, Name, Deapartment, max_quantity, max_time, Card_head)){
                        printf("OK\n");
                    }
                    else{
                        printf("FAIL\n");
                    }
                 }
                else{
                    printf("FAIL\n");
                }
                break;
            }
            case 3:{
                //delcard
                if(strlen(commandvarieties[1])==10){
                    char Code[11];
                    strcpy(Code,commandvarieties[1]);
                    if (Destory_Card(Code, Card_head)){
                        printf("OK\n");
                    }
                    else{
                        printf("FAIL\n");
                    }
                }
                else{
                    printf("FAIL\n");
                }
                break;
            }
            case 4:{
                //borrow
                int flag= 1;
                long int Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= strtol(commandvarieties[i+1], NULL, 10);
                            //atol(commandvarieties[i+1]);
                }
                if (IsValidTime(Time)== 0){
                    flag= 0;
                }
                if((flag==1)&& (strlen(commandvarieties[4])==10)){
                    char Card_Code[11];
                    strcpy(Card_Code, commandvarieties[4]);
                    long int Book_ID= strtol(commandvarieties[5], NULL, 10);
                            //atol(commandvarieties[5]);
                    if((Book_ID<0)|| (Book_ID>=BOOK_ID)){
                          flag=0;
                    }
                    if (flag==1&& Borrow(Card_Code, Book_ID, Time, Borrowed_head, Card_head, BookS_head)){
                        printf("OK\n");
                    }
                    else{
                        printf("FAIL\n");
                    }
                }
                else{
                    printf("FAIL\n");
                }
                break;
            }
            case 5:{
                //return
                int flag= 1;
                int long Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= strtol(commandvarieties[i+1], NULL, 10);
                            //atol(commandvarieties[i+1]);
                }
                if (IsValidTime(Time)== 0){
                    flag= 0;
                }
                if((flag==1)&& (strlen(commandvarieties[4])==10)){
                    char Card_Code[11];
                    strcpy(Card_Code, commandvarieties[4]);
                    long int Book_ID= strtol(commandvarieties[5], NULL, 10);
                            //atol(commandvarieties[5]);
                    if((Book_ID<0)|| (Book_ID>=BOOK_ID)){
                        flag=0;
                    }
                    if (flag==1){
                        double fine= Return(Card_Code, Book_ID, Time, Borrowed_head, Card_head, BookS_head);
                        if (fine==0){
                            printf ("0\n");
                        }
                        else if (fine>0){
                            printf ("%.1f\n", fine);
                        }
                        else{
                            printf ("-1\n");
                        }
                    }
                }
                else{
                    printf("FAIL\n");
                }
                break;
            }
            case 6:{
                //appoint
                int flag= 1;
                int long Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= strtol(commandvarieties[i+1], NULL, 10);
                            //atol(commandvarieties[i+1]);
                }
                if (IsValidTime(Time)== 0){
                    flag= 0;
                }
                if(flag==1&& (strlen(commandvarieties[4])==10))
                  {
                      char Card_Code[11];
                      strcpy(Card_Code, commandvarieties[4]);
                      long int Book_Code= strtol(commandvarieties[5], NULL, 10);
                              //atol(commandvarieties[5]);
                      if((Book_Code<0)){
                          flag=0;
                      }
                      if (flag==1 && Appoint(Card_Code, Book_Code, Time, Card_head, BookS_head)){
                          printf("OK\n");
                      }
                      else{
                        printf("FAIL\n");
                      }
                  }
                else{
                        printf("FAIL\n");
                }
                break;
            }
            case 7:{
                //cancelappoint
                int flag= 1;
                int long Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= strtol(commandvarieties[i+1], NULL, 10);
                            //atol(commandvarieties[i+1]);
                }
                if (IsValidTime(Time)== 0){
                    flag= 0;
                }
                if(flag==1&& (strlen(commandvarieties[4])==10)){
                    char Card_Code[11];
                    strcpy(Card_Code, commandvarieties[4]);
                    long int Book_Code= strtol(commandvarieties[5], NULL, 10);
                            //atol(commandvarieties[5]);
                    if(Book_Code<0){
                      flag=0;
                    }
                    if (flag==1 && Appoint_Cancel(Time, Card_Code, Book_Code, BookS_head, Card_head)){
                        printf("OK\n");
                    }
                    else{
                        printf("FAIL\n");
                    }
                }
                else{
                    printf("FAIL!\n");
                }
                break;
            }
            case 8:{
                // booktimestat
                int flag= 1;
                long int Book_Code= strtol(commandvarieties[1], NULL, 10);
                        //atol(commandvarieties[1]);
                if (Book_Code<0){
                    flag= 0;
                }
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= strtol(commandvarieties[i+2], NULL, 10);
                            //atol(commandvarieties[i+2]);
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= strtol(commandvarieties[i+5], NULL, 10);
                            //atol(commandvarieties[i+5]);
                }
                if (IsValidTime(StartTime)==0|| IsValidTime(EndTime)==0){
                    flag= 0;
                }
                if(flag){
                    double result= BookTimestat(Book_Code, StartTime, EndTime, BookS_head, Borrowed_head);
                    if (result==-1){
                        printf("FAIL\n");
                    }
                    else{
                        printf("%.1f\n", result);
                    }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 9:{
                //bookeffistat
                int flag= 1;
                long int Book_Code= strtol(commandvarieties[1], NULL, 10);
                        //atol(commandvarieties[1]);
                if (Book_Code<0){
                    flag= 0;
                }
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= strtol(commandvarieties[i+2], NULL, 10);
                            //atol(commandvarieties[i+2]);
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= strtol(commandvarieties[i+5], NULL, 10);
                            //atol(commandvarieties[i+5]);
                }
                if (IsValidTime(StartTime)==0|| IsValidTime(EndTime)==0){
                    flag= 0;
                }
                if (flag){
                    double result= BookEffistat(Book_Code, StartTime, EndTime, BookS_head, Borrowed_head);
                     if (result==-1){
                        printf("FAIL\n");
                     }
                     else{
                        printf("%.1f\n", result);
                     }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 10:{
                //persontimestat
                int flag= 1;
                char Card_Code[11];
                if(strlen(commandvarieties[1])==10){
                    strcpy(Card_Code,commandvarieties[1]);
                }
                else{
                    flag=0;
                }
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= strtol(commandvarieties[i+2], NULL, 10);
                            //atol(commandvarieties[i+2]);
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= strtol(commandvarieties[i+5], NULL, 10);
                            //atol(commandvarieties[i+5]);
                }
                if (IsValidTime(StartTime)==0|| IsValidTime(EndTime)==0){
                    flag= 0;
                }
                if(flag){
                    int result= PersonTimestat(Card_Code, StartTime, EndTime, Card_head, Borrowed_head);
                    if (result==-1){
                        printf("FAIL\n");
                    }
                    else{
                        printf("%d\n", result);
                    }
                }
                else{
                    printf("FAIL\n");
                }
                break;
            }
            case 11:{
                //personeffistat
                int flag= 1;
                char Card_Code[11];
                if(strlen(commandvarieties[1])==10){
                    strcpy(Card_Code,commandvarieties[1]);
                }
                else{
                    flag=0;
                }
            
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= strtol(commandvarieties[i+2], NULL, 10);
                            //atol(commandvarieties[i+2]);
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= strtol(commandvarieties[i+5], NULL, 10);
                            //atol(commandvarieties[i+5]);
                }
                if (IsValidTime(StartTime)==0|| IsValidTime(EndTime)==0){
                    flag= 0;
                }
                if(flag){
                    double result= PersonEffistat(Card_Code, StartTime, EndTime, Card_head, Borrowed_head);
                    if (result==-1){
                        printf("FAIL\n");
                    }
                    else{
                        printf("%.1f\n", result);
                    }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 12:{
                //querybook
                int flag= 1;
                long int Book_Code= strtol(commandvarieties[1], NULL, 10);
                        //atol(commandvarieties[1]);
                if (Book_Code<0){
                    flag= 0;
                }
                if (flag== 1){
                    long int result= QueryBookCode(Book_Code, BookS_head);
                    if (result==-1){
                        printf("-1\n");
                    }
                    else{
                        printf("%ld\n", result);
                    }
                }
                else{
                    printf ("-1\n");
                }
                break;
            }
            case 13:{
                //querybookid
                int flag= 1;
                long int Book_Id= strtol(commandvarieties[1], NULL, 10);
                        //atol(commandvarieties[1]);
                if (Book_Id<0||Book_Id>BOOK_ID){
                    flag= 0;
                }
                if (flag== 1){
                    char result[11];
                    strcpy(result, QueryBookId(Book_Id, BookS_head, Borrowed_head));
                    if (strcmp(result, "-1")==0){
                        printf("-1\n");
                    }
                    else if (strcmp(result, "0")==0){
                        printf("0\n");
                    }
                    else{
                        printf("%s\n", result);
                    }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 14:{
                //querycard
                int flag= 1;
                char Card_Code[11];
                if(strlen(commandvarieties[1])==10){
                    strcpy(Card_Code,commandvarieties[1]);
                }
                else{
                    flag=0;
                }
                if (flag){
                    if (QueryCard(Card_Code, Card_head, Borrowed_head)){
                         ;
                    }
                    else{
                        printf("FAIL\n");
                    }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 15:{
                //querydtcard
                int flag= 1;
                char Card_Code[11];
                if(strlen(commandvarieties[1])==10){
                    strcpy(Card_Code,commandvarieties[1]);
                }
                else{
                    flag=0;
                }
                if (flag){
                    if (QueryDtCard(Card_Code, Card_head, Borrowed_head, BookS_head)){
                        ;
                    }
                    else{
                        printf("FAIL\n");
                    }
                }
                else{
                    printf ("FAIL\n");
                }
                break;
            }
            case 16:{
                //quit
                exit= 0;
                break;
            }
           case -1:{
                //invalid input
                printf ("WRONG COMMAND\n");
                break;
            }
            default:{
                break;
            }
        }
    }
    return 0;
}

