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

void get_head(FILE** file , Encode* encodes, char** file_name , long* last_bits){
    if(file == nullptr || *file == nullptr || encodes == nullptr || file_name == nullptr || last_bits == nullptr)error("get head:argument error");
    
    unsigned char* buffer = new unsigned char[sizeof(long)*2];
    if(buffer == nullptr)error("get head: new buffer error");
    if(fread(buffer,sizeof(long),2,*file)!=2)error("get head: read two long error");
    long len_head = *((long*)buffer);
    long len_table = *((long*)(buffer + sizeof(long)));
    if(buffer != nullptr){
        delete[]  buffer;
        buffer = nullptr;
    }
    buffer = new unsigned char[len_table];
    if(buffer == nullptr)error("get head: new buffer error");
    fseek(*file, 0L, SEEK_SET);
    if(fread(buffer, sizeof(unsigned char), len_table, *file) != len_table)error("get head:fread error");
    unsigned char *pt1 = buffer;
    unsigned char *pt2 = buffer + len_head;
    unsigned char *pt2_end = buffer + len_table;
    unsigned char *pt;
    pt1 += sizeof(long)*2;
    long len_author = *(pt1) , len_soft = *(pt1+1) , len_suf = *(pt1+2) , len_file_name = *(pt1+3),i;
    *last_bits = *(pt1+4);
    pt1 += 5;
    log("\ncompress author:",false);
    for(i=1,pt = pt1; i<=len_author;i++,pt++)printf("%c",*pt);
    log("\ncompress soft:",false);
    for(i=1; i<=len_soft;i++,pt++)printf("%c",*pt);
    log("\ncompress author:",false);
    for(i=1; i<len_suf;i++,pt++)printf("%c",*pt);
    log("");
    (*file_name) = new char[len_file_name+1];
    if(file_name == nullptr)error("get head:new filename error");
    memcpy((*file_name), pt, len_file_name);
    (*file_name)[len_file_name]='\0';
    log("original file name:",false);
    for(i=0;i<len_file_name;i++,pt++)printf("%c",*pt);
    log("");
    pt = pt2;
    for (i=0; pt<pt2_end;i++) {
        if(*pt == 0x00)pt++;
        else{
            encodes[i].code = new char[*pt];
            if(encodes[i].code == nullptr)error("get head:new encodes[i].code");
            encodes[i].len = *pt;
            memcpy(encodes[i].code, pt+1, *pt);
            pt = pt + 1 + encodes[i].len;
        }
    }
    if(buffer != nullptr){
        delete[] buffer;
        buffer = nullptr;
    }
}

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

void encode(Node** nodes,long len,Encode* encodes){
    if(nodes == nullptr || len < 1 || len > 256)error("encode:argument error");
    
    if(len == 1)error("encode:node len == 1 , too small");
    //对原Node数据进行拷贝
    Node** backup = new Node*[len+1];
    if(backup == nullptr)error("encode:new backup error");
    long i , j, start;
    char tmp[260];
    for(i=1;i<=len;i++)backup[i]=nodes[i];
    Huffman huffman=*new Huffman(nodes,len);
    for(i=1;i<=len;i++){
        start=259;
        Node* leaf = backup[i];
        while (leaf->parent != nullptr) {
            Node* parent = leaf->parent;
            if(parent->left == leaf)tmp[start--]='0';
            else if(parent->right == leaf)tmp[start--]='1';
            else error("huffman tree we reference seems not correct");
            leaf = leaf->parent;
        }
        j =backup[i]->data;
        encodes[j].code = new char[259-start];
        if(encodes[j].code == nullptr)error("encode:new code failed");
        encodes[j].len = 259 - start;
        memcpy(encodes[j].code,tmp+start+1,259-start);
    }
    if(backup != nullptr){
        delete[] backup;
        backup = nullptr;
    }

}
bool decode_node(Node** root , bool f){
    if(root == nullptr  || *root == nullptr)error("decode node: argument error");
    if(f){
        (*root) = (*root)->right;
        if((*root)->left == nullptr && (*root)->right == nullptr)return true;
        return false;
    }else{
        (*root) = (*root)->left;
        if((*root)->left == nullptr && (*root)->right == nullptr)return true;
        return false;
    }
}
void decode(Node* root,Node** last,EncodeBuffer* en_buffer,DecodeBuffer* de_buffer){
    if(last == nullptr || en_buffer == nullptr || de_buffer == nullptr)error("decode:argument error");
    
    long last_bits = 0 , i;
    bool f1=false,f2;
    if(en_buffer->last_bytes > 0){
        f1 = true;
        last_bits = en_buffer->last_bytes;
    }
    if(*last == nullptr)*last = root;
    if(de_buffer->len != 0)de_buffer->len = 0;
    long len_buffer = en_buffer->len;
    
    unsigned char *pt = en_buffer->buffer , *pt_end = en_buffer->buffer + len_buffer -1,tmpc,bit_base;
    for(;pt <= pt_end;pt++){
        tmpc = *pt;
        for(i = 0 ;i<8;i++){
            bit_base = 0x80>>i;
            if(bit_base & tmpc)f2=true;
            else f2 = false;
            if(decode_node(last, f2)==1){
                de_buffer->buffer[de_buffer->len] = (*last)->data;
                de_buffer->len += 1;
                (*last) = root;
            }
        }
    }
    if (f1) {
        tmpc = en_buffer->buffer[en_buffer->len];
        for(i=0;i<last_bits;i++){
            bit_base = 0x80 >> i;
            if(bit_base & tmpc)f2=true;
            else f2 = false;
            if(decode_node(last, f2)==1){
                de_buffer->buffer[de_buffer->len] = (*last)->data;
                de_buffer->len += 1;
                *last = root;
            }
        }
    }
}

#endif /* tools_h */
