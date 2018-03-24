//
//  struct.h
//  Huffman
//
//  Created by lotu on 2018/3/23.
//  Copyright © 2018年 lotu. All rights reserved.
//

#ifndef struct_h
#define struct_h

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct Node
{
    unsigned char data;
    unsigned long long count;
    Node* parent;
    Node* left;
    Node* right;
    Node(){
        data=0x00;
        count=0;
        parent=nullptr;
        left=nullptr;
        right=nullptr;
    }
//    ~Node(){
//        if(parent!=nullptr){
//            delete parent;
//            parent=nullptr;
//        }
//        if(left!=nullptr){
//            delete left;
//            left=nullptr;
//        }
//        if(right!=nullptr){
//            delete right;
//            right=nullptr;
//        }
//    }
};

struct Encode
{
    char* code;
    long len;
    Encode() {
        code=nullptr;
        len=0;
    }
    ~Encode(){
        if(code!=nullptr){
            delete code;
            code=nullptr;
        }
    }
};

struct EncodeBuffer
{
    unsigned char* buffer;
    long len;
    long last_bytes;
    EncodeBuffer() {
        buffer=nullptr;
        len=0;
        last_bytes=0;
    }
    ~EncodeBuffer(){
        if(buffer!=nullptr){
            delete buffer;
            buffer=nullptr;
        }
    }
};

struct DecodeBuffer
{
    unsigned char* buffer;
    long len;
    DecodeBuffer() {
        buffer=nullptr;
        len=0;
    }
    ~DecodeBuffer(){
        if(buffer!=nullptr){
            delete buffer;
            buffer=nullptr;
        }
    }
};

struct Header
{
    
    long len_header=0;
    long len_table=0;
    long len_author=0;
    long len_soft=0;
    long len_suf=0;
    long len_file_name=0;
    long len_lastByte=0;
    
    char* author=nullptr;
    char* soft=nullptr;
    char* suf=nullptr;
    char* file_name=nullptr;
    Header(const char* suf =".henc" , const char* author="lotu" ,  const char* soft="CMP"){
        len_author=strlen(author);
        len_suf=strlen(suf);
        len_soft=strlen(soft);
        this->author = new char[len_author+1];
        memcpy(this->author,author,len_author);
        this->soft = new char[len_soft+1];
        memcpy(this->soft,soft,len_soft);
        this->suf = new char[len_suf+1];
        memcpy(this->suf,suf,len_suf);
        this->len_header=len_author+len_soft+len_suf+3+1+2*sizeof(long);
    }
    ~Header(){
        if(author!=nullptr){
            delete[] author;
            author=nullptr;
        }
        if(suf!=nullptr){
            delete[] suf;
            suf=nullptr;
        }
        if(file_name!=nullptr){
            delete[] file_name;
            file_name=nullptr;
        }
        if(soft!=nullptr){
            delete[] soft;
            soft=nullptr;
        }
    }
};

void log(const char* info,bool endl=true){
    if(endl)
        printf("%s\n",info);
    else
        printf("%s", info);
}

void error(const char* info,int code=0){
    log(info);
    exit(code);
}


#endif /* struct_h */
