#ifndef PARSER_H
#define PARSER_H

int factor(void);//phân tích nhân tử
int term(void);//phân tích số hạng
int expression(void); // phân tích biểu thức
void condition(void); // phân tích điều kiện
void statement(void); // phân tích câu lệnh
void block(int num_arg); // phân tích các khối câu lệnh
void program(void); //Phân tích chương trình

#endif
