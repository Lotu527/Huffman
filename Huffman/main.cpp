//
//  main.cpp
//  Huffman
//
//  Created by lotu on 2018/3/18.
//  Copyright © 2018年 lotu. All rights reserved.
//
#include <iostream>
#include "struct.h"
#include "tools.h"
#include "heap.h"
#include "huffman.h"
void compress(const char* name);
void uncompress(const char* name);

int main(int argc, const char * argv[]) {
    
//    char model = *(argv[1]);
//    const char* file = argv[2];
    char model = '0';
    const char* file = "/Users/lotu/Desktop/文档.txt";
    if (model=='0') {
        compress(file);
    }
    else if(model=='1')
    {
        uncompress(file);
    }
    else
    {
        printf("Unsuporrted operate, try 0 or 1\n");
    }
    
    return 0;
}
void compress(const char* name){
    //传入要压缩文件路径，打开文件，并创建压缩文件
    //读取文件信息，统计字符频率，创建编码表，创建文件头
    //加载文件信息，并对应编码，写入缓冲区
    //写入压缩文件
    if(name == nullptr)error("compress:argument error");
    log("=============compress begin============");
    log("source file name:",false);
    log(name);
    //文件头信息
    Header header;
    header.len_file_name = strlen(name);
    header.file_name = new char[header.len_file_name+1];
    memcpy(header.file_name,name,header.len_file_name);
    header.len_header += header.len_file_name;
    //打开源文件，创建压缩文件
    char* compress_name = new char[header.len_file_name+header.len_suf+1];
    memcpy(compress_name,name,header.len_file_name);
    memcpy(compress_name+header.len_file_name,header.suf,header.len_suf);
    compress_name[header.len_file_name+header.len_suf]='\0';
    log("compress to file name:",false);
    log(compress_name);
    FILE *file=open_file(name);
    FILE *compress_file=create_file(compress_name);
    //统计文件中字符频率
    Node** nodes = new Node*[257];
    if(nodes == nullptr)error("compress:new nodes error");
    long node_num = 0;//出现字符种类
    get_weight(file,nodes,&node_num);
    //对字符进行重新编码
//    for(int i=1;i<=node_num;i++)
//    {
//        std::cout<<nodes[i]->count<<" "<<nodes[i]->data<<std::endl;
//    }
    Encode* en_array = new Encode[257];
    if(en_array==nullptr)error("compress:new en_array error");
    encode(nodes, node_num, en_array);
    
    //    Huffman* huffman=new Huffman();
    //    huffman->test_huffman_tree();
    //    Encode* encodes = huffman->encode(nodes,node_num);
    //    for(int i=0;i<257;i++){
    //        if(encodes[i].len > 0){
    //            for(int j=0;j<encodes[i].len;j++)
    //                std::cout<<encodes[i].code[j];
    //        }
    //        std::cout<<std::endl;
    //    }
    //    log("hello");
    //    if(encodes == nullptr)error("compress:new encodes error");
    
    
}
void uncompress(const char* name){
    
}
