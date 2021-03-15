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

long int NowTime[3];
long int BOOK_ID= 1;
long int BOOK_CODE= 0;
const long int notPrime_Month[12]= {31,28,31,30,31,30,31,31,30,31,30,31};
const long int Prime_Month[12]= {31,29,31,30,31,30,31,31,30,31,30,31};

typedef struct BookS {
    long int Code;
    char Name[41];
    char Author[11];
    char Publication[21];
    char Introdution[101];
    long int CanBorrow_Num;//当前在架数目
    int Appoint_Num;
    char Authority;
    long int Buy_time[3];
    int informed_Num;//表示正在发出的通知的数目
    struct BookS * next1;//图书类别的链表，最后一类书指向null
    struct Book * next2;//该图书下的图书id的链表
    struct Appointed_Record * next3;//该类图书的预约链表
}BookS;

typedef struct Book {
    long int Id;
    //char Code[10];
    int Is_Borrowed;//已借出则为1，否则为0
    struct Book * next;//图书id的链表，最后一本书指向null
}Book;

typedef struct Card {
    char Status;
    char Code[11];
    char Name[10];
    char Department[10];
    long int Max_Quantity;
    long int Max_Time;
    int Borrow_Num;//目前在借的图书数量，最大数量减去该值便是可借书籍数量
    //int Borrowed_Num;//
    int Appoint_Num;//预约书籍数量
    struct Card * next;//证件链表
}Card;

typedef struct Appointed_Record {
    char Card_Code[11];
    //int Book_Code;
    long int Appointed_Time[3];
    int Is_Overdue;//该预约记录当前状态，未还0，已发通知1
    struct Appointed_Record * next;
}Appointed_Record;

typedef struct Borrowed_Record {
    char Card_Code[11];
    long int Book_Id;
    long int Book_Code;
    long int StartTime[3];
    long int EndTime[3];
    long int Borrowed_Days;
    struct Borrowed_Record * next;//借阅记录的链表
}Borrowed_Record;

/*
    总共有5个链表结构
    第一个是图书类别的链表，即BookS链表，表头为空
    第二个是每一个图书类别下都有的一个以每本图书为节点的链表，表头为每一个BookS
    第三个是每一个图书类别下都有的预约链表，表头为每一个BookS,表示预约记录
    第四个是单独的借阅记录链表，表头为空
    第五个是单独的证件链表，表头为空
 */

/*
    判断是否为闰年
    parameter:
        @year 表示测试的年数
    返回值为int，1表示是闰年，0表示不是
 */
int IsPrime(const long int year){
    if ((year%4==0 && year%100!=0) || (year%400==0)){
        return 1;
    }
    else{
        return 0;
    }
}

/*
    判断时间输入是否合法
    parameter:
        @Time 表示待判断的时间
    返回值为int，1表示时间合法，0表示输入非法
 */
int IsValidTime(const long int Time[3]){
    int flag= 1;
    for (int i=0; i<3; i++){
        if (Time[i]<1){
            flag= 0;
            break;
        }
    }
    if (IsPrime(Time[0])){
        if (Time[2]> Prime_Month[(Time[1]-1)]){
            flag= 0;
        }
    }
    else{
        if (Time[2]> notPrime_Month[(Time[1]-1)]){
            flag= 0;
        }
    }
    return flag;
}

/*
    计算两个日期相隔的天数
    parameter:
        @StartTime 开始时间
        @EndTime 结束时间
    返回值即为结果天数
    ps.若StartTime和EndTime为同一天返回1
 */
long int DaysBetween2Date(const long int StartTime[3], const long int EndTime[3]){
    const long int StartYear= StartTime[0];
    const long int StartMonth= StartTime[1];
    const long int StartDay= StartTime[2];
    const long int EndYear= EndTime[0];
    const long int EndMonth= EndTime[1];
    const long int EndDay= EndTime[2];
    long int Days= 0;
    //如果starttime大于endtime则返回-1
    int flag=0;
    if (StartYear>EndYear){
        flag= 1;
    }
    else if (StartYear==EndYear && StartMonth>EndMonth){
        flag= 1;
    }
    else if (StartYear==EndYear && StartMonth==EndMonth && StartDay>EndDay){
        flag= 1;
    }
    if (flag== 1){
        return -1;
    }
    //同一个月
    if (StartYear==EndYear && StartMonth==EndMonth){
        Days= (EndDay)- (StartDay);
    }
    //同一年
    else if (StartYear==EndYear){
        int isPrime= IsPrime(StartYear);
        for(long int i=(StartMonth-1); i<(EndMonth-1); i++){
            if (isPrime==1){
                Days+= Prime_Month[i];
            }
            else{
                Days+= notPrime_Month[i];
            }
        }
        Days= Days- StartDay+ EndDay;
    }
    //不同年
    else{
        for (long int i= StartYear; i<EndYear; i++){
            if (IsPrime(i)==1){
                Days+= 366;
            }
            else{
                Days+= 365;
            }
        }
        long int StartDays= 0;//开始日期是当年的第几天
        for(int i=0; i<(StartMonth-1); i++){
            if (IsPrime(StartYear)==1){
                StartDays+= Prime_Month[i];
            }
            else{
                StartDays+= notPrime_Month[i];
            }
        }
        StartDays+= StartDay;
        long int EndDays= 0;//结束日期时当年的第几天
        for(int i=0; i<(EndMonth-1); i++){
            if (IsPrime(EndYear)==1){
                EndDays+= Prime_Month[i];
            }
            else{
                EndDays+= notPrime_Month[i];
            }
        }
        EndDays+= EndDay;
        Days= Days+ EndDays- StartDays;
    }
    return (Days+1);
}

/*
    单链表排序函数
    在借阅情况详细查询时使用，使得输出按bookid递增的顺序排序
    parament:
        @list_head 需要排序的单链表
    函数运行结束后借阅记录链表按照bookid递增的顺序排序
    成功返回1，否则返回0。
 */
int BubbleSortByBookid (Borrowed_Record * list_head){
    if (list_head->next== NULL)
        return 1;
    Borrowed_Record * cur= list_head->next;
    Borrowed_Record * teil= NULL;
    while (cur!= teil){
        Borrowed_Record * pre= list_head;
        while (cur->next!= teil){
            if (cur->Book_Id> (cur->next)->Book_Id){
                Borrowed_Record * temp= cur->next;
                pre->next= temp;
                cur->next= temp->next;
                temp->next= cur;
                pre= pre->next;
            }
            else {
                pre= pre->next;
                cur= cur->next;
            }
        }
        teil= cur;
        cur= list_head->next;
    }
    return 1;
}

/*
    图书登记命令
    parameter:
        @Books_Code 登记的图书的编号
        @Books_Name 登记的图书的名字
        @Books_Author 登记的图书的作者
        @Books_Publication 登记的图书的出版社
        @Books_Introduction 登记的图书的介绍
        @Books_Authority 登记的图书的借阅权限，J表示仅教师可借，S表示所有人可借
        @Books_BuyTime 购买的图书的时间
        @quantity 购买的图书数量
        @BookS_head BookS链表的表头
    返回值为int，1表示操作成功，0表示操作失败
 */
int RegBooks(long int Books_Code, char Books_Name[41], char Books_Author[11],
             char Books_Publication[21], char Books_Introduction[101], char Books_Authority,
             const long int Books_BuyTime[3], long int quantity, BookS * BookS_head) {
    BookS * s=BookS_head;
    while(s->next1!=NULL){
        s= s->next1;
    }
    BookS * new_BOOKS= (BookS *)malloc(sizeof(BookS));
    s->next1= new_BOOKS;
    new_BOOKS->next1= NULL;
    new_BOOKS->next2= NULL;
    new_BOOKS->next3= NULL;
    new_BOOKS->Code= Books_Code;
    new_BOOKS->informed_Num= 0;
    for(int j=0; j<3; j++){
        new_BOOKS->Buy_time[j]= Books_BuyTime[j];
    }
    strcpy(new_BOOKS->Name, Books_Name);
    strcpy(new_BOOKS->Author, Books_Author);
    strcpy(new_BOOKS->Publication, Books_Publication);
    strcpy(new_BOOKS->Introdution, Books_Introduction);
    new_BOOKS->Authority= Books_Authority;
    new_BOOKS->Appoint_Num= 0;
    new_BOOKS->CanBorrow_Num= quantity;
    s= s->next1;
    Book * now= s->next2;
    for(long int i=0; i<quantity; i++){
        Book * new_book= (Book *)malloc(sizeof(Book));
        if(s->next2== NULL){
            s->next2= new_book;
            now= s->next2;
        }
        else{
            while (now->next!=NULL)
                now= now->next;
            now->next= new_book;
            now= now->next;
        }
        new_book->Id= BOOK_ID;
        BOOK_ID++;
        new_book->Is_Borrowed= 0;
        new_book->next= NULL;
    }
    BOOK_CODE= Books_Code;
    return 1;
}

/*
    图书增加命令
    parameter:
        @Books_Code 增加的图书的编号
        @quantity 购买的图书数量
        @BookS_head BookS链表的表头
    返回值为int，1表示操作成功，0表示操作失败
 */
int AddBooks (long int Books_Code, long int quantity, BookS * BookS_head) {
    BookS * s=BookS_head;
    while((s->next1!=NULL) &&((s->next1)->Code!=Books_Code)){
        s= s->next1;
    }
    if (s->next1== NULL){
        return 0;
    }
    s= s->next1;
    s->CanBorrow_Num+= quantity;
    Book * now= s->next2;
    while (now->next!=NULL){
        now= now->next;
    }
    for(long int i=0; i<quantity; i++){
        Book * new_book= (Book *)malloc(sizeof(Book));
        now->next= new_book;
        now= now->next;
        new_book->Id= BOOK_ID;
        BOOK_ID++;
        new_book->Is_Borrowed= 0;
        new_book->next= NULL;
    }
    return 1;
}

/*
    办卡命令
    parameter:
        @Status 办卡人的身份，J表示教师，S表示学生
        @Card_Code 办卡人的证件号
        @Card_Name 办卡人姓名
        @Card_Department 办卡人所在学院
        @Max_Quantity 办卡人允许同时借书的最大数量
        @Max_time 办卡人最大借阅时间
            @Card_head 卡的链表
    返回值为int，1表示办卡成功，0表示失败
 */
int RegCards(char Status, char Card_Code[11], char Card_Name[10], char Card_Department[10],
             long int Max_Quantity, long int Max_time, Card * Card_head) {
    Card * s= Card_head;
    while((s->next!=NULL) &&(strcmp((s->next)->Code, Card_Code)!=0)){
        s= s->next;
    }
    if (s->next!=NULL){
        return 0;
    }
    Card * new_Card= (Card *)malloc(sizeof(Card));
    s->next= new_Card;
    new_Card->Status= Status;
    strcpy(new_Card->Code, Card_Code);
    strcpy(new_Card->Name, Card_Name);
    strcpy(new_Card->Department, Card_Department);
    new_Card->Max_Quantity= Max_Quantity;
    new_Card->Max_Time= Max_time;
    new_Card->Borrow_Num= 0;
    new_Card->Appoint_Num= 0;
    new_Card->next= NULL;
    return 1;
}

/*
    销卡命令
    parameter:
        @Card_Code 需要注销的卡的卡号
        @Card_head 卡的链表头
    返回值为int类型，1表示销卡成功，0表示失败
 */
int Destory_Card(char Card_Code[11], Card * Card_head) {
    Card * s= Card_head->next;
    Card * pre= Card_head;
    while ((s!=NULL) && (strcmp(s->Code, Card_Code)!=0)){
        pre= pre->next;
        s= s->next;
    }
    if (s == NULL){
        return 0;
    }
    else{
        pre->next= s->next;
        free(s);
        return 1;
    }
}

/*
    预约处理命令
    遍历所有图书链表中的图书中的预约链表，将时间进行比较，进行相应的通知或取消操作
    该命令需要逐日处理
    paramter:
        @Time 表示处理的事这一天的预约
        @Books_head 图书的链表头
    无返回值
 */
void Appoint_Solve(long int Time[3], BookS * Books_head, Card * Card_head) {
    long int Days= DaysBetween2Date(NowTime, Time)-1;
    //更新当前日期，并且逐日推移时间并处理预约
    for (int count=0; count<Days; count++){
        BookS * s= Books_head->next1;
        while (s!=NULL){
            Appointed_Record * t= s->next3;
            //处理节点时如果出现删除操作则一定是删除最靠近s的预约节点
            while (t!=NULL){
                long int days= DaysBetween2Date(t->Appointed_Time, NowTime);
                //超过10天未收到通知，删除节点并继续对下一节点进行更新
                if (days>10 && t->Is_Overdue==0) {
                    printf ("cancelappoint %ld %ld %ld %s %ld\n", NowTime[0], NowTime[1], NowTime[2], t->Card_Code, s->Code);
                    Card *u= Card_head->next;
                    while (strcmp(u->Code, t->Card_Code)!= 0){
                        u= u->next;
                    }
                    u->Appoint_Num--;
                    u->Borrow_Num--;
                    s->Appoint_Num--;
                    if (s->next3== t){
                        Appointed_Record * temp= t;
                        s->next3= temp->next;
                        t= s->next3;
                        free(temp);
                    }
                    else {
                        Appointed_Record * pre= s->next3;
                        while (pre->next!= t){
                            pre= pre->next;
                        }
                        Appointed_Record *temp= t;
                        pre->next= temp->next;
                        t= pre->next;
                        free(temp);
                    }
                }
                //通知时间超过3天仍未取书，删除节点并继续对下一节点进行更新
                else if (days>3 && t->Is_Overdue==1) {
                    printf ("cancelappoint %ld %ld %ld %s %ld\n", NowTime[0], NowTime[1], NowTime[2], t->Card_Code, s->Code);
                    Card *u= Card_head->next;
                    while (strcmp(u->Code, t->Card_Code)!= 0){
                        u= u->next;
                    }
                    u->Appoint_Num--;
                    u->Borrow_Num--;
                    s->informed_Num--;
                    s->Appoint_Num--;
                    if (s->next3== t){
                        Appointed_Record * temp= t;
                        s->next3= temp->next;
                        t= s->next3;
                        free(temp);
                    }
                    else {
                        Appointed_Record * pre= s->next3;
                        while (pre->next!= t){
                            pre= pre->next;
                        }
                        Appointed_Record *temp= t;
                        pre->next= temp->next;
                        t= pre->next;
                        free(temp);
                    }
                }
                else {
                    //已收到通知但时间未超过3天，继续对下一节点进行更新
                    if (t->Is_Overdue==1){
                        t= t->next;
                    }
                    //未收到通知但是当前已发通知数少于在架数目，对该记录发出通知
                    else{
                        if ((s->informed_Num)< (s->CanBorrow_Num)){
                            for (int i=0; i<3; i++){
                                t->Appointed_Time[i]= NowTime[i];
                            }
                            printf ("bookreturn %ld %ld %ld %s %ld\n", t->Appointed_Time[0], t->Appointed_Time[1], t->Appointed_Time[2], t->Card_Code, s->Code);
                            t->Is_Overdue= 1;
                            s->informed_Num++;
                            t= t->next;
                        }
                        else
                            t= t->next;
                    }
                }
            }
            s= s->next1;
        }
        if (count != Days){
            NowTime[2]+= 1;
            if (NowTime[2]>28){
                switch (NowTime[1]) {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                        if (NowTime[2]>31){
                            NowTime[2]= 1;
                            NowTime[1]+= 1;
                        }
                        break;
                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        if (NowTime[2]>30){
                            NowTime[2]= 1;
                            NowTime[1]+= 1;
                        }
                        break;
                    case 2:
                        if (IsPrime(NowTime[0])){
                            if (NowTime[2]>29){
                                NowTime[2]= 1;
                                NowTime[1]= 3;
                            }
                        }
                        else{
                            if (NowTime[2]>28){
                                NowTime[2]= 1;
                                NowTime[1]= 3;
                            }
                        }
                        break;
                    case 12:
                        if (NowTime[2]>31){
                            NowTime[2]= 1;
                            NowTime[1]= 1;
                            NowTime[0]+= 1;
                        }
                    default:
                        break;
                }
            }
        }
    }
}

/*
    借书命令
    parameter:
        @Card_Code 借书人的卡号
        @Books_Id 所借的书的id
        @Time 借书的时间
        @Borrowed_head 借阅记录链表的表头
        @Card_head 卡链表的表头
        @Books_head 图书链表的表头
    返回值为int，1表示借书成功，否则返回0
 */
int Borrow(char Card_Code[11], long int Book_Id, long int Time[3],
           Borrowed_Record * Borrowed_head, Card * Card_head, BookS * Books_head) {
    Appoint_Solve(Time, Books_head, Card_head);
    //查询Card_Code所借书是否已超过最大数:调用Card中的Borrow_Num
    Card * s= Card_head;
    while((s->next!=NULL) &&(strcmp((s->next)->Code, Card_Code)!=0)){
        s= s->next;
    }
    if (s->next==NULL){
        return 0;
    }
    s= s->next;
    //查询该Book_Id是否已被预约，先找到Book_Code，再调用BookS中的Appointed_Num;
    //查询该Book_Id是否已借出
    BookS * T= Books_head->next1;
    Book * t= T->next2;
    while (t->Id != Book_Id){
        if (t->next != NULL){
            t= t->next;
        }
        else {
            T= T->next1;
            t= T->next2;
        }
    }
    //无法借出有2种情况：该书已被借，没有该人的预约记录或预约了但未发通知，没有借阅权限
    int flag=0;
    //若已被借出为0，该card未收到通知为1,无借阅权限为2，无人预约为3，该card收到通知为4，预约数少于在架数目为5
    if (t->Is_Borrowed==1){
        return 0;
    }
    if (T->Appoint_Num==0){
        flag=3;
    }
    if (T->Appoint_Num!=0){
        Appointed_Record * u= T->next3;
        for(int i=0; i<T->Appoint_Num; i++){
            if (strcmp(u->Card_Code, Card_Code)==0 && u->Is_Overdue==1){
                flag= 4;
                break;
            }
            u= u->next;
        }
        if (flag!=4){
            flag= 1;
        }
    }
    if (T->Appoint_Num<T->CanBorrow_Num){
        flag= 5;
    }
    if (T->Authority=='J' && s->Status=='S'){
        flag= 2;
    }
    if (flag==1 || flag==2){
        return 0;
    }
    if (s->Borrow_Num+s->Appoint_Num >= s->Max_Quantity && flag!= 4){
        return 0;
    }
    //借阅成功同时增添一条借阅记录，End_Time设为全0；Card_Code中的Borrow_Num+1
    Borrowed_Record * v= Borrowed_head;
    while (v->next!=NULL){
        v= v->next;
    }
    Borrowed_Record * new_Borrowed_Record = (Borrowed_Record *)malloc(sizeof(Borrowed_Record));
    v->next= new_Borrowed_Record;
    new_Borrowed_Record->next= NULL;
    strcpy(new_Borrowed_Record->Card_Code, Card_Code);
    new_Borrowed_Record->Book_Code= T->Code;
    new_Borrowed_Record->Book_Id= Book_Id;
    new_Borrowed_Record->Borrowed_Days= 0;
    for(int i=0; i<3; i++){
        new_Borrowed_Record->StartTime[i]= Time[i];
        new_Borrowed_Record->EndTime[i]= 0;
    }
    //bookcode的canborrow_num-1;bookid的isborrowed改为1
    s->Borrow_Num++;
    T->CanBorrow_Num--;
    t->Is_Borrowed= 1;
    //若flag为4，即该人是收到通知后来借书，则此时应删除对应的预约记录
    if (flag==4){
        s->Appoint_Num--;
        T->Appoint_Num--;
        T->informed_Num--;
        Appointed_Record * u= T->next3;
        if (strcmp(u->Card_Code, Card_Code)==0){
            T->next3= u->next;
            free(u);
        }
        else{
            Appointed_Record * pre= T->next3;
            u= u->next;
            while (u!=NULL && strcmp(u->Card_Code, Card_Code)!=0){
                pre= pre->next;
                u= u->next;
            }
            pre->next= u->next;
            free(u);
        }
    }
    return 1;
}

/*
 *  还书命令
 *  parameter:
 *      @Card_Code 还书人的卡号
 *      @Book_Id 还的书的ID
 *      @Time 还书时间
 *      @Borrowed_head 借阅记录链表表头
 *      @Card_head 借阅卡链表表头
 *      @Books_head 书籍链表表头
 *  无逾期返回0，逾期返回罚款数0.2/天，失败返回-1
 */
double Return(char Card_Code[11], long int Book_Id, long int Time[3],
              Borrowed_Record * Borrowed_head, Card * Card_head, BookS * Books_head) {
    Appoint_Solve(Time, Books_head, Card_head);
    Borrowed_Record * s= Borrowed_head->next;
    while((s!=NULL) && ((s->Book_Id!=Book_Id) || (s->Borrowed_Days!=0))){
        s= s->next;
    }
    if (s==NULL){
        return -1;
    }
    for(int i=0; i<3; i++){
        s->EndTime[i]= Time[i];
    }
    //调用借阅记录中的Start_Time来计算借阅时长
    s->Borrowed_Days= DaysBetween2Date(s->StartTime, s->EndTime);
    //Card_Code中的Borrow_Num-1
    Card * t= Card_head->next;
    while (t!=NULL && strcmp(t->Code, Card_Code)!=0){
        t= t->next;
    }
    if(t==NULL){
        return -1;
    }
    (t->Borrow_Num)--;
    //Books_Code中的CanBorrowed_Num+1
    BookS * U= Books_head->next1;
    while (U!=NULL && U->Code!=s->Book_Code){
        U= U->next1;
    }
    if (U==NULL){
        return -1;
    }
    U->CanBorrow_Num++;
    //book中的isborrow改成0
    Book * u= U->next2;
    while (u!=NULL && u->Id!=Book_Id){
        u= u->next;
    }
    if (u==NULL){
        return -1;
    }
    u->Is_Borrowed= 0;
    int a= (int)(s->Borrowed_Days - t->Max_Time);
    if (s->Borrowed_Days >= t->Max_Time){
        return (double)(0.2*a);
    }
    else{
        return 0;
    }
}

/*
 *  预约命令
 *  parameter:
 *      @Card_Code 预约人的卡号
 *      @Book_Code 预约的书的code
 *      @Time 预约的时间
 *      @Card_head 借阅卡链表的表头
 *      @Books_head 图书链表的表头
 *  成功预约返回1，预约失败返回0
 */
int Appoint(char Card_Code[11], long int Book_Code, long int Time[3],
            Card * Card_head, BookS * Books_head) {
    Appoint_Solve(Time, Books_head, Card_head);
    //调用Card_Code中的Borrow_Num查看是否可以进行预约
    Card *t= Card_head->next;
    while (t!=NULL && strcmp(t->Code, Card_Code)!=0){
        t= t->next;
    }
    if (t==NULL){
        return 0;//失败的输入
    }
    if (t->Borrow_Num == t->Max_Quantity){
        return 0;//无法预约
    }
    BookS *s= Books_head->next1;
    while (s!=NULL && s->Code!=Book_Code){
        s= s->next1;
    }
    if (s==NULL){
        return 0;
    }
    if (s->CanBorrow_Num > s->Appoint_Num){
        return 0;
    }
    int flag= 1;
    Appointed_Record * search= s->next3;
    while (search!= NULL){
        if (strcmp(search->Card_Code, Card_Code)==0){
            flag= 0;
        }
        search= search->next;
    }
    if (flag== 0){
        return 0;
    }
    //填写新的预约记录
    Appointed_Record * now= s->next3;
    Appointed_Record * new_Appointed= (Appointed_Record *)malloc(sizeof(Appointed_Record));
    if(now== NULL){
        s->next3= new_Appointed;
    }
    else{
        while (now->next!=NULL){
            now= now->next;
        }
        now->next= new_Appointed;
    }
    strcpy(new_Appointed->Card_Code, Card_Code);
    for (int i=0; i<3; i++){
        new_Appointed->Appointed_Time[i]= Time[i];
    }
    new_Appointed->Is_Overdue= 0;
    new_Appointed->next= NULL;
    //更改cardcode及bookcode节点的动态变量
    s->Appoint_Num++;
    t->Appoint_Num++;
    return 1;
}

/*
 *  取消预约命令
 *  parameter:
 *      @Card_Code 取消预约的人的卡号
 *      @Book_Code 取消预约的书的code
 *      @Books_head 图书链表的表头
 *      @Card_head 借阅卡链表的表头
 *  操作成功返回1，否则返回0
 */
int Appoint_Cancel(long int Time[3], char Card_Code[11], long int Book_Code,
                   BookS * Books_head, Card * Card_head) {
    Appoint_Solve(Time, Books_head, Card_head);
    //找到该卡对应的节点s
    Card * s= Card_head->next;
    while (s!=NULL && strcmp(s->Code, Card_Code)!=0){
        s= s->next;
    }
    if (s==NULL){
        return 0;
    }
    //找到该bookcode对应的节点T
    BookS * T= Books_head->next1;
    while (T!=NULL && T->Code!=Book_Code){
        T= T->next1;
    }
    if (T==NULL){
        return 0;
    }
    //找到对应的预约记录u,并执行删除操作
    Appointed_Record * u= T->next3;
    if (strcmp(u->Card_Code, Card_Code)==0){
        if (u->Is_Overdue== 1){
            T->Appoint_Num--;
            T->informed_Num--;
        }
        else{
            T->Appoint_Num--;
        }
        T->next3= u->next;
        free(u);
    }
    else{
        Appointed_Record * pre= T->next3;
        u= u->next;
        while (u!=NULL && strcmp(u->Card_Code, Card_Code)!=0){
            pre= pre->next;
            u= u->next;
        }
        if (u== NULL){
            return 0;
        }
        else{
            if (u->Is_Overdue== 1){
                T->Appoint_Num--;
                T->informed_Num--;
            }
            else{
                T->Appoint_Num--;
            }
            pre->next= u->next;
            free(u);
        }
    }
    //相应的更改card code节点的动态变量
    s->Appoint_Num--;
    s->Borrow_Num--;
    return 1;
}

/*
 *  书籍借阅平均次数统计命令
 *  parameter:
 *      @Book_Code 统计的书籍的code
 *      @StartTime 统计时间段的开始时间
 *      @EndTime 统计时间段的结束时间
 *      @Books_head 图书链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *  成功则返回该类书籍在这段时间内每本被借阅的平均次数，失败返回-1
 */
double BookTimestat(long int Book_Code, long int StartTime[3],
                    long int EndTime[3],
                    BookS * Books_head, Borrowed_Record * Borrowed_head) {
    //先比较输入是否合法：统计时间的合法性以及Book_Code的合法性
    if (DaysBetween2Date(StartTime, EndTime)<0){
        return  -1;
    }
    BookS * T= Books_head->next1;
    while (T!=NULL && T->Code!=Book_Code){
        T= T->next1;
    }
    if (T==NULL){
        return -1;
    }
    //计算该bookcode下共有多少本书
    int Books_nums= 0;
    Book * t= T->next2;
    while (t!=NULL){
        Books_nums++;
        t= t->next;
    }
    //以Time和Book_Code为key遍历借阅记录链表
    //查找归还时间晚于StartTime以及借阅时间早于EndTime，并且Book_Code符合的记录，每找到一个符合要求的记录则计数器+1
    double count= 0;
    Borrowed_Record * s= Borrowed_head->next;
    while (s!=NULL){
        if (s->Book_Code==Book_Code) {
            if (s->Borrowed_Days == 0) {
                if (DaysBetween2Date(s->StartTime, EndTime) >= 0){
                    count++;
                }
            }
            else if (DaysBetween2Date(StartTime, s->EndTime) >= 0 ||
                     (DaysBetween2Date(s->StartTime, EndTime) >= 0 && DaysBetween2Date(StartTime, s->StartTime) >= 0)){
                count++;
            }
        }
        s= s->next;
    }
    return (double)(count/Books_nums);
}

/*
 *  书籍借阅平均效率统计命令
 *  parameter:
 *      @Book_Code 统计的书籍的code
 *      @StartTime 统计时间段的开始时间
 *      @EndTime 统计时间段的结束时间
 *      @Books_head 书籍链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *  成功则返回某类书籍在这段时间内每本被借阅的平均时长除以总时长，失败返回-1
 */
double BookEffistat(long int Book_Code, long int StartTime[3], long int EndTime[3],
                 BookS * Books_head, Borrowed_Record * Borrowed_head) {
    //先比较输入是否合法统计时间的合法性以及Book_Code的合法性
    if (DaysBetween2Date(StartTime, EndTime)<0){
        return  -1;
    }
    BookS * T= Books_head->next1;
    while (T!=NULL && T->Code!=Book_Code){
        T= T->next1;
    }
    if (T==NULL){
        return -1;
    }
    //计算该bookcode下共有多少本书
    int Books_nums= 0;
    Book * t= T->next2;
    while (t!=NULL){
        Books_nums++;
        t= t->next;
    }
    //以Time和Book_Code为key遍历借阅记录链表
    //查找借阅时间在统计区间内并且Book_Code符合的记录，并将相应的时间累加至计数器上
    long int Total_Days= 0;
    Borrowed_Record * s= Borrowed_head->next;
    while (s!=NULL){
        if (s->Book_Code==Book_Code)
            if (DaysBetween2Date(StartTime, s->EndTime)>=0 || DaysBetween2Date(s->StartTime, EndTime)>=0){
                //借了但未还的情况
                if (s->Borrowed_Days==0) {
                    if (DaysBetween2Date(s->StartTime, EndTime) >= 0) {
                        if (DaysBetween2Date(s->StartTime, StartTime) >= 0){
                            Total_Days += DaysBetween2Date(StartTime, EndTime);
                        }
                        else{
                            Total_Days += DaysBetween2Date(s->StartTime, EndTime);
                        }
                    }
                }
                //跨过starttime的情况
                else if (DaysBetween2Date(s->StartTime, StartTime)>=0 &&
                         DaysBetween2Date(s->EndTime, EndTime)>=0){
                    Total_Days+= DaysBetween2Date(StartTime, s->EndTime);
                }
                //恰好在中间的情况
                else if (DaysBetween2Date(StartTime, s->StartTime)>=0 &&
                         DaysBetween2Date(s->EndTime, EndTime)>=0){
                    Total_Days+= DaysBetween2Date(s->StartTime, s->EndTime);
                }
                //跨过endtime的情况
                else if (DaysBetween2Date(s->StartTime, EndTime)>=0 &&
                         DaysBetween2Date(EndTime, s->EndTime)>=0){
                    Total_Days+= DaysBetween2Date(s->StartTime, EndTime);
                }
                //同时跨过starttime与endtime的情况
                else if (DaysBetween2Date(s->StartTime, StartTime)>=0 &&
                         DaysBetween2Date(EndTime, s->EndTime)>=0){
                    Total_Days+= DaysBetween2Date(StartTime, EndTime);
                }
            }
        s= s->next;
    }//这个情况分类需要再一次检查，等第一次检查的时候记得画图检查!!
    //对总时间进行处理，计算出借阅平均效率
    return ((double)Total_Days/ (double)(DaysBetween2Date(StartTime, EndTime)*Books_nums) );
}

/*
 *  人员借阅频率统计命令
 *  parameter:
 *      @Card_Code 需要统计的人的借阅卡号
 *      @StartTime 统计的时间段的开始时间
 *      @EndTime 统计的时间段的结束时间
 *      @Card_head 借阅卡链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *  成功则返回该人在这段时间内借阅图书的次数，失败返回-1
 */
int PersonTimestat(char Card_Code[11], long int StartTime[3], long int EndTime[3],
                   Card * Card_head, Borrowed_Record * Borrow_head) {
    //先比较输入是否合法：统计时间的合法性以及Card_Code的合法性
    if (DaysBetween2Date(StartTime, EndTime)<0){
        return -1;
    }
    Card * t= Card_head->next;
    while (t!=NULL && strcmp(t->Code, Card_Code)!=0){
        t= t->next;
    }
    if (t==NULL){
        return -1;
    }
    //以Time和Card_Code为key遍历借阅记录链表
    //查找归还时间晚于StartTime以及借阅时间早于EndTime，并且借书人为该Card_Code的记录，计数器随之变化
    int count= 0;
    Borrowed_Record * s= Borrow_head->next;
    while (s){
        if (strcmp(s->Card_Code, Card_Code)== 0){
            if (s->Borrowed_Days==0) {
                if (DaysBetween2Date(s->StartTime, EndTime) >= 0){
                    count++;
                }
            }
            else if (DaysBetween2Date(StartTime, s->EndTime)>=0 ||
                     (DaysBetween2Date(s->StartTime, EndTime)>=0&&DaysBetween2Date(StartTime, s->StartTime)>=0)){
                count++;
            }
        }
        s= s->next;
    }
    return count;
}

/*
 *  人员借阅效率统计命令
 *  parameter:
 *      @Card_Code 需要统计的人的借阅卡号
 *      @StartTime 统计的时间段的开始时间
 *      @EndTime 统计的时间段的结束时间
 *      @Card_head 借阅卡链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *  成功则返回某人在这段时间内平均借阅几本图书，失败返回-1
 */
double PersonEffistat(char Card_Code[11], long int StartTime[3], long int EndTime[3],
                      Card * Card_head, Borrowed_Record * Borrowed_head) {
    //先比较输入是否合法：统计时间的合法性以及Card_Code的合法性
    if (DaysBetween2Date(StartTime, EndTime)<0){
        return -1;
    }
    Card * t= Card_head->next;
    while (t!=NULL && strcmp(t->Code, Card_Code)!=0){
        t= t->next;
    }
    if (t==NULL){
        return -1;
    }
    //以Time和Card_Code为key遍历借阅记录链表
    //查找满足条件的记录，并且将相应的时间累加至计数器
    long int Total_Days= 0;
    Borrowed_Record * s= Borrowed_head->next;
    while (s!=NULL){
        if (strcmp(s->Card_Code, Card_Code)==0){
            if (DaysBetween2Date(StartTime, s->EndTime)>=0 || DaysBetween2Date(s->StartTime, EndTime)>=0){
                if (s->Borrowed_Days==0) {
                    if (DaysBetween2Date(s->StartTime, EndTime) >= 0) {
                        if (DaysBetween2Date(s->StartTime, StartTime) >= 0){
                            Total_Days += DaysBetween2Date(StartTime, EndTime);
                        }
                        else{
                            Total_Days += DaysBetween2Date(s->StartTime, EndTime);
                        }
                    }
                }
                    //跨过starttime的情况
                else if (DaysBetween2Date(s->StartTime, StartTime)>=0 &&
                         DaysBetween2Date(s->EndTime, EndTime)>=0){
                    Total_Days+= DaysBetween2Date(StartTime, s->EndTime);
                }
                    //恰好在中间的情况
                else if (DaysBetween2Date(StartTime, s->StartTime)>=0 &&
                         DaysBetween2Date(s->EndTime, EndTime)>=0){
                    Total_Days+= DaysBetween2Date(s->StartTime, s->EndTime);
                }
                    //跨过endtime的情况
                else if (DaysBetween2Date(s->StartTime, EndTime)>=0 &&
                         DaysBetween2Date(EndTime, s->EndTime)>=0){
                    Total_Days+= DaysBetween2Date(s->StartTime, EndTime);
                }
                    //同时跨过starttime与endtime的情况
                else if (DaysBetween2Date(s->StartTime, StartTime)>=0 &&
                         DaysBetween2Date(EndTime, s->EndTime)>=0){
                    Total_Days+= DaysBetween2Date(StartTime, EndTime);
                }
            }
    }
        s= s->next;
    }
    //对计数器进行处理，计算出借阅效率
    return (double)Total_Days/(double)DaysBetween2Date(StartTime, EndTime);
}

/*
 *  图书类目查询命令
 *  parameter:
 *      @Book_Code 查询的图书的code
 *      @Books_head 图书链表的表头
 *  成功则返回可借出图书数量，失败返回-1
 */
long int QueryBookCode(long int Book_Code, BookS * Books_head) {
    //检查输入是否合法：Book_Code是否合法
    BookS * T= Books_head->next1;
    while (T!=NULL && T->Code!=Book_Code){
        T= T->next1;
    }
    if (T==NULL){
        return -1;
    }
    if (T->Appoint_Num <= T->CanBorrow_Num){
        return (T->CanBorrow_Num- T->Appoint_Num);
    }
    else{
        return -1;
    }
}

/*
 *  图书查询命令
 *  parameter:
 *      @Book_Id 查询的图书的id
 *      @Books_head 图书链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *  成功则返回借阅者的CardCode或者字符0（表示未借出），失败返回字符-1
 */
char* QueryBookId(long int Book_Id, BookS * Books_head, Borrowed_Record * Borrowed_head) {
    //检查输入是否合法：Book_Id是否合法
    BookS * T= Books_head->next1;
    Book * t= T->next2;
    while (t->Id != Book_Id && T!=NULL){
        if (t->next != NULL){
            t= t->next;
        }
        else {
            T= T->next1;
            t= T->next2;
        }
    }
    if (T==NULL){
        return "-1";
    }
    //若最后没有找到对应的记录则说明该书未借出，否则返回记录中的Card_Id
    int flag= 0;
    Borrowed_Record * s= Borrowed_head->next;
    while (s){
        if (s->Borrowed_Days==0 && s->Book_Id==Book_Id){
            flag= 1;
            break;
        }
        s= s->next;
    }
    if (flag==1){
        return s->Card_Code;
    }
    return "0";
}

/*
 *  借阅情况概要查询命令
 *  parameter:
 *      @Card_Code 查询的卡号
 *      @Card_head 借阅卡链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *      @Books_head 图书链表的表头
 *  成功则依照一定顺序输出该Card下借阅的书籍总数，可借书籍数量，预约书籍数量，以及过期图书数量，并返回1；操作失败则返回0
 */
int QueryCard(char Card_Code[11],
              Card * Card_head, Borrowed_Record * Borrowed_head) {
    Card * s= Card_head->next;
    while (s!=NULL && strcmp(Card_Code, s->Code)!=0){
        s= s->next;
    }
    if (s==NULL){
        return 0;
    }
    //计算该卡下逾期记录条数
    Borrowed_Record * t= Borrowed_head->next;
    int count= 0;
    while (t){
        if (strcmp(t->Card_Code, Card_Code)== 0 &&
            t->Borrowed_Days== 0 &&
            DaysBetween2Date(t->StartTime, NowTime)>(s->Max_Time)){
            count++;
        }
        t= t->next;
    }
    //直接调用Card_Code中的4个变量，赋给数组后返回该数组
    printf("%d %ld %d %d\n", (s->Borrow_Num+s->Appoint_Num), (s->Max_Quantity-s->Appoint_Num-s->Borrow_Num),
           s->Appoint_Num, count);
    return 1;
}

/*
 *  借阅情况详细查询命令
 *  parameter:
 *      @Card_Code 查询的卡号
 *      @Card_head 借阅卡链表的表头
 *      @Borrowed_head 借阅记录链表的表头
 *      @Books_head 图书链表的表头
 *  成功则按照一定格式输出该Card下当前借阅的书籍情况并返回1，否则返回0
 */
int QueryDtCard(char Card_Code[11],
                Card * Card_head, Borrowed_Record * Borrowed_head, BookS * Books_head) {
    //检查输入的合法性
    Card * s= Card_head->next;
    while (s!=NULL && strcmp(Card_Code, s->Code)!=0){
        s= s->next;
    }
    if (s==NULL){
        return 0;
    }
    //遍历借阅记录查找满足条件的记录，并且按格式打印
    Borrowed_Record * u= Borrowed_head->next;
    Borrowed_Record * result_head= (Borrowed_Record *)malloc(sizeof(Borrowed_Record));
    result_head->next= NULL;
    while (u){
        if (strcmp(u->Card_Code, Card_Code)==0 && u->Borrowed_Days==0){
            //找到bookcode对应的节点以输出书名
            Borrowed_Record * result= result_head;
            while (result->next!=NULL){
                result= result->next;
            }
            Borrowed_Record * new_node= (Borrowed_Record *)malloc(sizeof(Borrowed_Record));
            new_node->Book_Code= u->Book_Code;
            new_node->Book_Id= u->Book_Id;
            strcpy(new_node->Card_Code, u->Card_Code);
            for (int i=0; i<3; i++){
                new_node->StartTime[i]= u->StartTime[i];
            }
            new_node->next= NULL;
            result->next= new_node;
            //result= result->next;
        }
        u= u->next;
    }
    BubbleSortByBookid(result_head);
    Borrowed_Record * v= result_head->next;
    while (v){
        BookS * T= Books_head->next1;
        while (T!=NULL && v->Book_Code!=T->Code){
            T= T->next1;
        }
        if (T==NULL){
            return 0;
        }
        //计算到期日
        long int DeadDays[3]= {0};
        for (int i=0; i<3; i++){
            DeadDays[i]= v->StartTime[i];
        }
        DeadDays[2]+= (s->Max_Time-1);
        int flag= IsPrime(DeadDays[0]);
        if (DeadDays[1]==2 && DeadDays[2]>28){
            if (flag== 1){
                if (DeadDays[2]>29){
                    DeadDays[2]-= 29;
                    DeadDays[1]= 3;
                }
                else{
                    DeadDays[2]-= 28;
                    DeadDays[1]= 3;
                }
            }
        }
        else {
            while (DeadDays[2] > notPrime_Month[ DeadDays[1]-1 ]){
                switch (DeadDays[1]) {
                    case 1:
                    case 3:
                    case 5:
                    case 7:
                    case 8:
                    case 10:
                        if (DeadDays[2]>31){
                            DeadDays[2]-= 31;
                            DeadDays[1]+= 1;
                        }
                        break;
                    case 4:
                    case 6:
                    case 9:
                    case 11:
                        if (DeadDays[2]>30){
                            DeadDays[2]-= 30;
                            DeadDays[1]+= 1;
                        }
                        break;
                    case 12:
                        if (DeadDays[2]>31){
                            DeadDays[2]-= 31;
                            DeadDays[1]= 1;
                            DeadDays[0]+= 1;
                        }
                    default:
                        break;
                }
            }
        }
        //计算过期时长
        long int Overdue_days;
        if (DaysBetween2Date(v->StartTime, NowTime)>s->Max_Time){
            Overdue_days= DaysBetween2Date(v->StartTime, NowTime)- s->Max_Time;
        }
        else{
            Overdue_days= 0;
        }
        //按格式输出：图书编号，书名，到期日，过期时长
        printf("%ld %s %ld %ld %ld %ld\n", v->Book_Id, T->Name,
               DeadDays[0], DeadDays[1], DeadDays[2], Overdue_days);
        v= v->next;
    }
    return 1;
}

/*
 *  初始化时间命令
 *  parameter:
 *      @year 初始化时间的年份
 *      @month 初始时间的月份
 *      @day 初始时间的日期
 *  成功则返回1，否则返回0
 */
long int init_time(long int year, long int month, long int day){
    if (month>12 || day>31){
        return 0;
    }
    NowTime[0]= year;
    NowTime[1]= month;
    NowTime[2]= day;
    return 1;
}

/*
long int scanf_long(){
    char buffer[10];
    char *stop;
    scanf("%s", buffer);
    long int x= strtol(buffer, &stop, 10);
    if (x==0)
        return -1;
    else
        return x;
}

int main()
{

    BookS * BookS_head= (BookS *)malloc(sizeof(BookS));
    BookS_head->next1= NULL;
    Borrowed_Record * Borrowed_head= (Borrowed_Record *)malloc(sizeof(Borrowed_Record));
    Borrowed_head->next= NULL;
    Card * Card_head= (Card*)malloc(sizeof(Card));
    Card_head->next= NULL;

    init_time(2019, 01, 01);

    char commandset[16][21]={"regbooks", "regcard", "delcard", "borrow", "return", "appoint", "cancelappoint", "booktimestat", "bookeffistat", "persontimestat", "personeffistat", "querybook", "querybookid", "querycard", "querydtcard", "quit"};
    long int command= -1;
    int exit= 0;
    while (exit==0){
        char input[300];
        gets(input);
        char commands[21];
        scanf("%s",commands);
        for(int i=0;i<16;i++)
            {
                if(strcmp(commands,commandset[i])==0){
                    command=i+1;
                    break;
                }
                else
                    command=-1;
            }
        switch (command) {
            case 1:{
                //图书登记命令
                int flag=1;
                long int code;
                code= scanf_long();
                if (code==-1){
                    flag= 0;
                    printf ("FAIL!\n");
                }
                else if (code <= BOOK_CODE){
                    long int quantity= scanf_long();
                    if (quantity==-1)
                        flag= 0;
                    if (AddBooks(code, quantity, BookS_head) && flag==1)
                        printf ("OK!\n");
                    else
                        printf ("FAIL!\n");
                }
                else{
                    char name[41];
                    scanf("%s", name);
                    char author[11];
                    scanf("%s", author);
                    char publication[21];
                    scanf("%s", publication);
                    char introduction[101];
                    scanf("%s", introduction);
                    long int buytime[3];
                    for (int i=0; i<3; i++){
                        buytime[i]= scanf_long();
                        if (buytime[i]== -1)
                            flag= 0;
                    }
                    if (DaysBetween2Date(NowTime, buytime)>=0)
                        flag= 0;
                    long int quantity= scanf_long();
                    char authority;
                    getchar();
                    scanf("%c", &authority);
                    if (flag==1){
                        if (RegBooks(code, name, author, publication, introduction, authority, buytime, quantity, BookS_head))
                            printf ("OK!\n");
                    }
                    else
                        printf("FAIL！\n");
                }
                break;
            }
            case 2:{
                //办卡命令
                int flag= 1;
                getchar();
                char Status;
                scanf("%c", &Status);
                getchar();
                char Code[11];
                scanf("%s", Code);
                if (strlen(Code)!= 10)
                    flag= 0;
                char Name[10];
                scanf("%s", Name);
                char Deapartment[10];
                scanf("%s", Deapartment);
                long int max_quantity= scanf_long();
                if (max_quantity==-1)
                    flag= 0;
                long int max_time= scanf_long();
                if (max_time==-1)
                    flag= 0;
                if (flag==1 &&
                    RegCards(Status, Code, Name, Deapartment, max_quantity, max_time, Card_head))
                    printf("OK！\n");
                else
                    printf("FAIL!\n");
                break;
            }
            case 3:{
                //销卡命令
                //printf("Enter the Code:");
                char Code[10];
                scanf("%s", Code);
                if (strlen(Code)!=10)
                    printf("FAIL!\n");
                else {
                    if (Destory_Card(Code, Card_head))
                        printf("OK!\n");
                    else
                        printf("FAIL!\n");
                }
                break;
            }
            case 4:{
                //借书
                int flag= 1;
                long int Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= scanf_long();
                    if (Time[i]==-1)
                        flag= 0;
                }
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!=10)
                    flag= 0;
                long int Book_ID= scanf_long();
                if (Book_ID== -1)
                    flag= 0;
                if (flag== 1 &&
                    Borrow(Card_Code, Book_ID, Time, Borrowed_head, Card_head, BookS_head)==1)
                    printf("OK!\n");
                else
                    printf("FAIL!\n");
                break;
            }
            case 5:{
                //还书
                int flag= 1;
                int long Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= scanf_long();
                    if (Time[i]==-1)
                        flag= 0;
                }
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!=10)
                    flag= 0;
                int Book_ID= (int)scanf_long();
                if (Book_ID==-1)
                    flag= 0;
                if (flag==1){
                    double fine= Return(Card_Code, Book_ID, Time, Borrowed_head, Card_head, BookS_head);
                    if (fine == 0)
                        printf ("0\n");
                    else if (fine > 0)
                        printf ("%.2f\n", fine);
                    else
                        printf ("-1\n");
                }
                break;
            }
            case 6:{
                //预约
                int flag= 1;
                long int Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= scanf_long();
                    if (Time[i]== -1)
                        flag= 0;
                }
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!=10)
                    flag= 0;
                long int Book_Code= scanf_long();
                if (Book_Code== -1)
                    flag= 0;
                if (flag==1 && Appoint(Card_Code, Book_Code, Time, Card_head, BookS_head))
                    printf("OK!\n");
                else
                    printf("FAIL!\n");
                break;
            }
            case 7:{
                //预约取消
                int flag= 1;
                long int Time[3];
                for (int i=0; i<3; i++){
                    Time[i]= scanf_long();
                    if (Time[i]== -1)
                        flag= 0;
                }
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!=10)
                    flag= 0;
                long int Book_Code= scanf_long();
                if (Book_Code==-1)
                    flag= 0;
                if (Appoint_Cancel(Time, Card_Code, Book_Code, BookS_head, Card_head))
                    printf("OK!\n");
                else
                    printf("FAIL!\n");
                break;
            }
            case 8:{
                //书籍借阅平均次数统计
                int flag= 1;
                long int Book_Code= scanf_long();
                if (Book_Code== -1)
                    flag= 0;
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= scanf_long();
                    if (StartTime[i]== -1)
                        flag= 0;
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= scanf_long();
                    if (EndTime[i]==-1)
                        flag= 0;
                }
                if (flag== 1){
                    double result= BookTimestat(Book_Code, StartTime, EndTime, BookS_head, Borrowed_head);
                    if (result==-1)
                        printf("FAIL\n");
                    else
                        printf("%.1f\n", result);
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 9:{
                //书籍借阅平均效率统计
                int flag= 1;
                long int Book_Code= scanf_long();
                if (Book_Code== -1)
                    flag= 0;
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= scanf_long();
                    if (StartTime[i]==-1)
                        flag= 0;
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= scanf_long();
                    if (EndTime[i]==-1)
                        flag= 0;
                }
                if (flag== 1){
                    double result= BookEffistat(Book_Code, StartTime, EndTime, BookS_head, Borrowed_head);
                    if (result==-1)
                        printf("FAIL\n");
                    else
                        printf("%.1f\n", result);
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 10:{
                //人员借阅频率统计
                int flag= 1;
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!=10)
                    flag= 0;
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= scanf_long();
                    if (StartTime[i]== -1)
                        flag= 0;
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= scanf_long();
                    if (EndTime[i]== -1)
                        flag= 0;
                }
                if (flag== 1){
                    int result= PersonTimestat(Card_Code, StartTime, EndTime, Card_head, Borrowed_head);
                    if (result==-1)
                        printf("FAIL\n");
                    else
                        printf("%d\n", result);
                }
                else
                    printf("FAIL\n");
                break;
            }
            case 11:{
                //人员借阅效率统计
                int flag= 1;
                char Card_Code[10];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!= 10)
                    flag= 0;
                int long StartTime[3];
                for (int i=0; i<3; i++){
                    StartTime[i]= scanf_long();
                    if (StartTime[i]== -1)
                        flag= 0;
                }
                int long EndTime[3];
                for (int i=0; i<3; i++){
                    EndTime[i]= scanf_long();
                    if (EndTime[i]== -1)
                        flag= 0;
                }
                if (flag== 1){
                    double result= PersonEffistat(Card_Code, StartTime, EndTime, Card_head, Borrowed_head);
                    if (result==-1)
                        printf("FAIL\n");
                    else
                        printf("%.1f\n", result);
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 12:{
                //图书类目查询
                int flag= 1;
                long int Book_Code= scanf_long();
                if (Book_Code== -1)
                    flag= 0;
                if (flag== 1){
                    long int result= QueryBookCode(Book_Code, BookS_head);
                    if (result==-1)
                        printf("-1\n");
                    else
                        printf("%ld\n", result);
                }
                else
                    printf ("-1\n");
                break;
            }
            case 13:{
                //图书查询
                int flag= 1;
                long int Book_Id= scanf_long();
                if (Book_Id== -1)
                    flag= 0;
                if (flag== 1){
                    char result[11];
                    strcpy(result, QueryBookId(Book_Id, BookS_head, Borrowed_head));
                    if (strcmp(result, "-1")==0)
                        printf("-1\n");//不确定
                    else if(strcmp(result, "0")==0)
                        printf("0\n");
                    else
                        printf("%s\n", result);
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 14:{
                //借阅情况概要查询
                int flag= 1;
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!= 10)
                    flag= 0;
                if (flag== 1){
                    if (QueryCard(Card_Code, Card_head, Borrowed_head, BookS_head))
                        ;
                    else
                        printf("FAIL\n");
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 15:{
                //借阅情况详细查询
                int flag= 1;
                char Card_Code[11];
                scanf("%s", Card_Code);
                if (strlen(Card_Code)!= 10)
                    flag= 0;
                if (flag== 1){
                    if (QueryDtCard(Card_Code, Card_head, Borrowed_head, BookS_head))
                        ;
                    else
                        printf("FAIL\n");
                }
                else
                    printf ("FAIL\n");
                break;
            }
            case 16:{
                //退出
                exit= 1;
                break;
            }
            case -1:{
                //错误的命令
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
  */

void Menu(void)
{
    int i;
    printf("★");
    for(i=0; i<36; i++)
        printf("-");
    printf("★");
    printf("\n");
    printf("图书管理系统\n");
    printf("1.图书登记              \n");
    printf("2.办卡                 3.销卡\n");
    printf("4.借书                 5.还书\n");
    printf("6.预约                 7.取消预约\n");
    printf("8.书籍借阅平均次数统计    9.书籍借阅平均效率统计\n");
    printf("10.人员借阅频率统计      11.人员借阅效率统计\n");
    printf("12.图书类目查询         13.图书查询\n");
    printf("14.借阅情况概要查询      15.借阅情况详细查询\n");
    printf("16.退出\n");
    printf("★");
    for(i=0; i<36; i++)
        printf("-");
    printf("★");
    printf("\n");
}

/*
    处理输入的函数
    按行读取输入
    parameter:
        @commandline 表示读取的输入行
        @command 表示读取的输入的命令的标号
        @commandvarieties 将读取的输入行以空格为分隔符分成若干个字符串
 */
char commandvarieties[11][101];
long int command;//
const char commandset[16][21]={"regbooks", "regcard", "delcard", "borrow", "return", "appoint", "cancelappoint", "booktimestat", "bookeffistat", "persontimestat", "personeffistat", "querybook", "querybookid", "querycard", "querydtcard", "quit"};
int getcommandline(){
    command= -1;
    for (int i=0; i<11; i++){
        commandvarieties[i][0]= '\0';
    }
    char commandline[300];
    int i=0, j=0, k=0;
    gets(commandline);
    while (commandline[0]== '\0')
        gets(commandline);
    while(commandline[i]!='\0'){
        if(!isspace(commandline[i])){
            commandvarieties[j][k]= commandline[i];
            k++;
            i++;
        }
        else{
            commandvarieties[j][k]= '\0';
            j++;
            k=0;
            while (isspace(commandline[i]))
                i++;
        }
    }
    commandvarieties[j][k]= '\0';
    i=0;
    while(strcmp(commandvarieties[0],commandset[i])!= 0 && i<16){
        i++;
    }
    if(i!=16){
        command=i+1;
        return 0;
    }
    else{
        command=-1;
    }
    return 1;
}

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

