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
