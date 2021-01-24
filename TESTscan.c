#include <stdio.h>
#include <ctype.h>
#include<string.h>
//下面定义保留，为简化程序，使用字符指针数组保存所有保留字。
//如果想增加保留字，可继续添加，并修改保留字数目
#define keywordSum  8
#define HASHSIZE 23
typedef struct
{
	int elem;
	char* value;
}HashTable;

HashTable hashArr[HASHSIZE];
char* keyword[keywordSum] = { "if","else","for","while","do","int","read","write" };
//下面定义纯单分界符，如需要可添加
char singleword[50] = "+-*%(){};,:\"\"";//   /是否需要加到单分界集   加则注释部分需要基于单分界符判断 
//下面定义双分界符的首字符
char doubleword[10] = "><=!";
extern char Scanin[300], Scanout[300]; //用于接收输入输出文件名,在TEST_main.c中定义
extern FILE* fin, * fout; //用于指向输入输出文件的指针,在TEST_main.c中定义

int InitHashTable(HashTable H[])
{
	int i;
	for (i = 0; i < 23; i++)
	{
		H[i].elem = 0;
		H[i].value = " ";
	}
	return 1;
}

int Hash(int key) {
	return key % 23;
}

int toAscii(char* key) {
	int ascValue = 0;
	for (int i = 0; i < strlen(key); i++) {
		ascValue += key[i];
	}
	return ascValue;
}

int step = 0;//标记保留字存入哈希
void InsertHash(HashTable H[], int key)
{
	int addr = Hash(key);
	while (H[addr].value == NULL)
	{
		addr = (addr + 1) % 23;
	}
	H[addr].elem = key;
	H[addr].value = keyword[step];
	step++;
}

int SearchHash(HashTable H[], int key, char s[])
{

	int addr = Hash(key);
	if (H[addr].elem == key) {
		if (strcmp(H[addr].value, s) == 0)
		{
			return addr;
		}
	}
	return -1;
}


int TESTscan()//词法分析函数
{
	InitHashTable(hashArr);
	for (int i = 0; i < 8; i++)
	{
		InsertHash(hashArr, toAscii(keyword[i]));
	}
	char ch, token[60]; //ch为每次读入的字符，token用于保存识别出的单词  original 40 too short   expend to 60
	int es = 0, j, n; //es错误代码，0表示没有错误。j,n为临时变量，控制组合单词时的下标等
	printf("请输入源程序文件名（包括路径）：");
	scanf("%s", Scanin);
	printf("请输入词法分析输出文件名（包括路径）：");
	scanf("%s", Scanout);
	if ((fin = fopen(Scanin, "r")) == NULL) //判断输入文件名是否正确
	{
		printf("\n打开词法分析输入文件出错!\n");
		return(1);//输入文件出错返回错误代码1
	}
	if ((fout = fopen(Scanout, "w")) == NULL) //判断输出文件名是否正确
	{
		printf("\n创建词法分析输出文件出错!\n");
		return(2); //输出文件出错返回错误代码2
	}
	ch = getc(fin);
	while (ch != EOF)
	{
		while (ch == ' ' || ch == '\n' || ch == '\t') ch = getc(fin);
		if (ch == EOF) break;
		if (isalpha(ch))   //如果是字母，则进行标识符处理
		{
			token[0] = ch; j = 1;
			ch = getc(fin);
			while (isalnum(ch) && ch != ',')  //如果是字母数字则组合标识符；如果不是则标识符组合结束
			{
				token[j++] = ch;  //组合的标识符保存在token中
				ch = getc(fin);  //读下一个字符
			}
			token[j] = '\0';  //标识符组合结束	 
			//查保留字
			int item;
			item = SearchHash(hashArr, toAscii(token), token);
			/* n=0;
			 while ((n<keywordSum) && strcmp(token,keyword[n])) n++;*/
			if (/*n>=keywordSum*/item == -1)  //不是保留字，输出标识符
				fprintf(fout, "%s\t%s\n", "ID", token);  //输出标识符符号
			else//是保留字，输出保留字
				fprintf(fout, "%s\t%s\n", token, token);  //输出保留字符号
		}
		else if (isdigit(ch))//数字处理
		{
			token[0] = ch; j = 1;
			ch = getc(fin);  //读下一个字符
			while (isdigit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
			{
				token[j++] = ch;  //组合整数保存在token中
				ch = getc(fin);  //读下一个字符
			}
			token[j] = '\0';  //整数组合结束		 
			fprintf(fout, "%s\t%s\n", "NUM", token);  //输出整数符号
		}
		else if (strchr(singleword, ch) > 0)  //单分符处理
		{
			token[0] = ch; token[1] = '\0';
			fprintf(fout, "%s\t%s\n", token, token);  //输出单分界符符号
			if (ch == '"') {
				char ch2;
				int k = 0;
				ch2 = getc(fin);
				while (ch2 != '"' && ch2 != EOF) {
					ch = ch2;
					token[k] = ch;
					k++;
					ch2 = getc(fin);
				}
				token[k] = '\0';
				fprintf(fout, "%s\t%s\n", "STR", token);
				char token1[50];
				token1[0] = ch2; token1[1] = '\0';
				fprintf(fout, "%s\t%s\n", token1, token1);  //输出单分界符符号

			}
			ch = getc(fin);//读下一个符号以便识别下一个单词
		}
		else if (strchr(doubleword, ch) > 0)  //双分界符处理
		{
			token[0] = ch;
			ch = getc(fin);  //读下一个字符判断是否为双分界符
			if (ch == '=')  //如果是=，组合双分界符
			{
				token[1] = ch; token[2] = '\0';  //组合双分界符结束
				ch = getc(fin);  //读下一个符号以便识别下一个单词
			}
			else//不是=则为单分界符
				token[1] = '\0';
			fprintf(fout, "%s\t%s\n", token, token);  //输出单或双分界符符号
		}
		else if (ch == '/')  //注释处理
		{
			ch = getc(fin);  //读下一个字符
			if (ch == '*')  //如果是*，则开始处理注释
			{
				char ch1;
				ch1 = getc(fin);  //读下一个字符
				do
				{
					ch = ch1; ch1 = getc(fin);
				}  //删除注释
				while ((ch != '*' || ch1 != '/') && ch1 != EOF);  //直到遇到注释结束符*/或文件尾
				ch = getc(fin);//读下一个符号以便识别下一个单词
			}
			else  //不是*则处理单分界符/
			{
				token[0] = '/'; token[1] = '\0';
				fprintf(fout, "%s\t%s\n", token, token);  //输出单分界符/
			}
		}
		else if (ch == '"') {
			char ch2;
			int k = 0;
			ch2 = getc(fin);
			while (ch2 != '"' && ch2 != EOF) {
				ch = ch2;
				token[k] = ch;
				k++;
				ch2 = getc(fin);
			}
			token[k] = '\0';
			fprintf(fout, "%s\t%s\n", "STR", token);
		}
		else//错误处理
		{
			token[0] = ch; token[1] = '\0';
			ch = getc(fin);  //读下一个符号以便识别下一个单词
			es = 3;  //设置错误代码
			fprintf(fout, "%s\t%s\n", "ERROR", token);  //输出错误符号
			printf("分析出现非法字符: %s\n", token);
		}
	}
	fprintf(fout, "%s\t%s\n", "#", "#");
	fclose(fin);//关闭输入输出文件
	fclose(fout);
	return(es);  //返回主程序
}


//#include <stdio.h>
//#include <ctype.h>
////下面定义保留，为简化程序，使用字符指针数组保存所有保留字。
////如果想增加保留字，可继续添加，并修改保留字数目
//#define keywordSum  8
//char *keyword[keywordSum]={ "if","else","for","while","do","int","read","write"};
////下面定义纯单分界符，如需要可添加
//char singleword[50]="+-*(){};,:";
////下面定义双分界符的首字符
//char doubleword[10]="><=!";
//extern char Scanin[300], Scanout[300]; //用于接收输入输出文件名,在TEST_main.c中定义
//extern FILE *fin,*fout; //用于指向输入输出文件的指针,在TEST_main.c中定义
//int TESTscan()//词法分析函数
//{
//   char ch,token[80]; //ch为每次读入的字符，token用于保存识别出的单词
//   int es=0,j,n; //es错误代码，0表示没有错误。j,n为临时变量，控制组合单词时的下标等
//   printf("请输入源程序文件名（包括路径）：");
//   scanf("%s",Scanin);
//   printf("请输入词法分析输出文件名（包括路径）：");
//   scanf("%s",Scanout);
//   if ((fin=fopen(Scanin,"r"))==NULL) //判断输入文件名是否正确
//   {
//      printf("\n打开词法分析输入文件出错!\n");
//      return(1);//输入文件出错返回错误代码1
//   }
//   if ((fout=fopen(Scanout,"w"))==NULL) //判断输出文件名是否正确
//   {
//      printf("\n创建词法分析输出文件出错!\n");
//      return(2); //输出文件出错返回错误代码2
//   }
//   ch=getc(fin);
//   while(ch!=EOF)
//   {
//      while (ch==' '||ch=='\n'||ch=='\t') ch=getc(fin);
//      if (isalpha(ch))   //如果是字母，则进行标识符处理
//      {
//		 token[0]=ch; j=1;
//		 ch=getc(fin);
//		 while(isalnum(ch))  //如果是字母数字则组合标识符；如果不是则标识符组合结束
//		 {
//			token[j++]=ch;  //组合的标识符保存在token中
//			ch=getc(fin);  //读下一个字符
//		 }
//		 token[j]='\0';  //标识符组合结束	 
//		 //查保留字
//		 n=0;
//		 while ((n<keywordSum) && strcmp(token,keyword[n])) n++;
//		 if (n>=keywordSum)  //不是保留字，输出标识符
//			fprintf(fout,"%s\t%s\n","ID",token);  //输出标识符符号
//		else//是保留字，输出保留字
//			fprintf(fout,"%s\t%s\n",token,token);  //输出保留字符号
//	  } else if (isdigit(ch))//数字处理
//      {
//		 token[0]=ch; j=1;
//		 ch=getc(fin);  //读下一个字符
//		 while (isdigit(ch))  //如果是数字则组合整数；如果不是则整数组合结束
//		 {
//			token[j++]=ch;  //组合整数保存在token中
//			ch=getc(fin);  //读下一个字符
//		 }
//		 token[j]='\0';  //整数组合结束		 
//		 fprintf(fout,"%s\t%s\n","NUM",token);  //输出整数符号
//      } else if (strchr(singleword,ch)>0)  //单分符处理
//      {
//		 token[0]=ch; token[1]='\0';
//		 ch=getc(fin);//读下一个符号以便识别下一个单词
//		 fprintf(fout,"%s\t%s\n",token,token);  //输出单分界符符号
//      }else if (strchr(doubleword,ch)>0)  //双分界符处理
//      {
//		 token[0]=ch;
//		 ch=getc(fin);  //读下一个字符判断是否为双分界符
//		 if (ch=='=')  //如果是=，组合双分界符
//		 {
//			token[1]=ch;token[2]='\0';  //组合双分界符结束
//		   	ch=getc(fin);  //读下一个符号以便识别下一个单词
//		 } else//不是=则为单分界符
//			token[1]='\0';
//		 fprintf(fout,"%s\t%s\n",token,token);  //输出单或双分界符符号
//      } else if (ch=='/')  //注释处理
//      {
//		 ch=getc(fin);  //读下一个字符
//		 if (ch=='*')  //如果是*，则开始处理注释
//		 {  char ch1;
//			ch1=getc(fin);  //读下一个字符
//			do
//			{  ch=ch1;ch1=getc(fin);}  //删除注释
//			while ((ch!='*' || ch1!='/')&&ch1!=EOF);  //直到遇到注释结束符*/或文件尾
//			ch=getc(fin);//读下一个符号以便识别下一个单词
//		 } else  //不是*则处理单分界符/
//		 {
//			 token[0]='/'; token[1]='\0';
//			 fprintf(fout,"%s\t%s\n",token,token);  //输出单分界符/
//		 }
//	} else//错误处理
//	  {
//		 token[0]=ch;token[1]='\0';
//		 ch=getc(fin);  //读下一个符号以便识别下一个单词
//		 es=3;  //设置错误代码
//		 fprintf(fout,"%s\t%s\n","ERROR",token);  //输出错误符号
//      }
//   }
//   fclose(fin);//关闭输入输出文件
//   fclose(fout);
//   return(es);  //返回主程序
//}
//
