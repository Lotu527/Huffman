//
//  tools.h
//  Huffman
//
//  Created by lotu on 2018/3/23.
//  Copyright © 2018年 lotu. All rights reserved.
//

#ifndef tools_h
#define tools_h
#include <stdio.h>
#include <unistd.h>
#include "huffman.h"

void bit_set(unsigned char* ch , long i ,long f){
    if (f == 1) {
        (*ch) |= (0x80>>i);
    }else if (f == 0) {
        (*ch) &= ~(0x80>>i);
    } else {
        error("bit set:flag not correct");
    }
}
FILE* open_file(const char* name){
    if(name == nullptr)error("open file:argument error");
    if(access(name,0)!=0)error("open file:file not exist");
    FILE* file = fopen(name,"rb");
    if(file == nullptr)error("open file:error");
    return file;
}
FILE* create_file(const char* name){
    if(name == nullptr)error("create file:argument error");
    if(access(name,0)==0){//文件存在
        if(remove(name) != 0)//删除文件失败
            error("create file:delete file error");
    }
    FILE* file = fopen(name,"wb");
    if(file == nullptr)error("create file:cann`t open file");//创建文件失败
    return file;
}
FILE* create_file_uncompress(const char* name){
    if(name == nullptr)error("create file uncompress:argument error");
    if(access(name,0)==0){//解压出的文件存在，创建一个新命名文件
        long tmp = strlen(name);
        char* new_name = new char[tmp+2];
        memcpy(new_name,name,tmp);
        long i=0,j=0,k=0;
        for(;i<tmp;i++){
            if(name[i] == '.' && k==0){
                k++;
                new_name[j++]='1';
                new_name[j++]='.';
            }else
                new_name[j++]=name[i];
        }
        new_name[j]='\0';
        name=new_name;
    }
    FILE* file = fopen(name,"wb");
    if(file == nullptr)error("create file uncompress:cann`t open file");//创建文件失败
    return file;
}

void get_weight(FILE* file,Node** nodes,long* len){
    if(file == nullptr || nodes == nullptr || len == nullptr)error("get weight:argument error");
    *len = 0;
    const long len_buffer = 40*1024*1024;
    unsigned char* buffer = new unsigned char[len_buffer];
    if(buffer == nullptr)error("get weight:new buffer error");
    
    Node* array = new Node[257];
    if(array == nullptr)error("get weight:new array error");
    
    //读取文件，统计每种字符出现的次数
    long read = 0,i;
    unsigned char *pt,*pt_end;
    while ((read = fread(buffer,sizeof(unsigned char),len_buffer,file))!=0) {
        pt=buffer;
        pt_end=buffer+read;
        for(;pt!=pt_end;pt++){
//            printf("%ld\n",long(*pt));
            array[long(*pt)].data = *pt;
            array[long(*pt)].count += 1;
        }
    }
    for(i=0;i<257;i++){
        if(array[i].count > 0){
            (*len) += 1;
            nodes[*len] = &(array[i]);
        }
    }
    if(buffer != nullptr){
        delete[] buffer;
        buffer = nullptr;
    }
}

long convert(unsigned char* data){
    for(int i=0;i<8;i++)
        printf("%c",data[i]);
    printf("\n");
    return 1;
}
void encode(Node** nodes,long len,Encode* encodes){
    if(nodes == nullptr || len < 1 || len > 256)error("encode:argument error");
    
    if(len == 1)error("encode:node len == 1 , too small");
    //对原Node数据进行拷贝
    Node** backup = new Node*[len+1];
    if(backup == nullptr)error("encode:new backup error");
    long i , start;
    char tmp[260];
    for(i=1;i<=len;i++)backup[i]=nodes[i];
    Huffman huffman=*new Huffman(nodes,len);
    huffman.test_huffman_tree();
//    huffman.in_traverse_tree(huffman.get_root());
    for(i=0;i<len;i++){
        
//        if(encodes[i].len>0)log(encodes[i].code);
//        start=259;
//        Node* leaf = backup[i];
//        while (leaf->parent != nullptr) {
//            Node* parent = leaf->parent;
//            if(parent->left == leaf)tmp[start--]='0';
//            else if(parent->right == leaf)tmp[start--]='1';
//            else error("huffman tree we reference seems not correct");
//            leaf = leaf->parent;
//        }
//        long j =backup[i]->data;
//        printf("%ld",j);
////        convert(backup[i]->data);
//        encodes[j].code = new char[259-start];
//        if(encodes[j].code == nullptr)error("encode:new code failed");
//        encodes[j].len = 259 - start;
//        memcpy(encodes[j].code,tmp+start+1,259-start);
    }
    if(backup != nullptr){
        delete[] backup;
        backup = nullptr;
    }

}



#endif /* tools_h */
