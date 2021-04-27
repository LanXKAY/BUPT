/*
    总共有5个链表结构
    第一个是图书类别的链表，即BookS链表，表头为空
    第二个是每一个图书类别下都有的一个以每本图书为节点的链表，表头为每一个BookS
    第三个是每一个图书类别下都有的预约链表，表头为每一个BookS,表示预约记录
    第四个是单独的借阅记录链表，表头为空
    第五个是单独的证件链表，表头为空
 */

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

