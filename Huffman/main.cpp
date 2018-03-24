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
    if(argc!=3){
        printf("Useage: program-name num filename\n");
        printf("program-name is the app you run\n");
        printf("num means the operate you want, 0 means compress, 1 means Uncompress\n");
        printf("filename is the file you want to compress or Uncompress\n");
        exit(0);
    }
    char model = *(argv[1]);
    const char* file = argv[2];
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
    long node_num = 0, i;//出现字符种类
    get_weight(file,nodes,&node_num);
    //对字符进行重新编码
    Encode* en_array = new Encode[257];
    if(en_array==nullptr)error("compress:new en_array error");
    encode(nodes, node_num, en_array);
    //处理文件头
    header.len_table = header.len_header;
    for (i=0; i<256; i++) {
        if(en_array[i].len == 0)header.len_table ++ ;
        else if(en_array[i].len > 0)header.len_table += 1 + en_array[i].len;
    }
    unsigned char* buffer_header = new unsigned char[header.len_table];
    if(buffer_header== nullptr)error("compress:new buffer header error");
    unsigned char* pt = buffer_header;
    *((long*)pt) = header.len_header;
    *((long *)(pt+sizeof(long)))=header.len_table;
    pt+=2*sizeof(long);
    *(pt++)=(unsigned char)header.len_author;
    *(pt++)=(unsigned char)header.len_soft;
    *(pt++)=(unsigned char)header.len_suf;
    *(pt++)=(unsigned char)header.len_file_name;
    *(pt++)=(unsigned char)header.len_lastByte;
    const char* ch = "lotuCMPhenc";
    memcpy(pt,ch,11);
    pt+=11;
    memcpy(pt,name,header.len_file_name);
    pt+=header.len_file_name;
    for (i=0; i<256; i++) {
        if(en_array[i].len == 0){
            *pt = 0x00;
            pt += 1;
        }else{
            *pt = en_array[i].len;
            pt += 1;
            memcpy(pt, en_array[i].code, en_array[i].len);
            pt += en_array[i].len;
        }
    }
    if(fwrite(buffer_header,header.len_table,1,compress_file)!=1)error("compress: fwrite header error");
    //对文件内容编码
    EncodeBuffer* en_buffer = new EncodeBuffer;
    if(en_buffer == nullptr)error("compress: new en_buffer error");
    const long fread_size = 4*1024*1024;
    const long len_en_buffer = 32 * fread_size;
    en_buffer->buffer = new unsigned char[len_en_buffer];
    if(en_buffer->buffer == nullptr)error("compress: new en_buffer->buffer error");
    unsigned char* fread_buffer =new unsigned char[fread_size];
    if(fread_buffer == nullptr)error("compress: new fread buffer error");
    
    i = 0;
    fseek(file, 0L, SEEK_SET);
    long long num_write_bits = -1;
    long num_per_write_bits = -1, j , k , byte_i , bit_i , flag;
    pt = nullptr;
    unsigned char* pt_end = nullptr;
    while ((i = fread(fread_buffer, sizeof(unsigned char), fread_size, file)) != 0) {
        num_per_write_bits = -1;
        if( num_write_bits >= 0)num_per_write_bits += (num_write_bits + 1)% 8;
        pt = fread_buffer;
        pt_end = fread_buffer + i;
        while (pt!=pt_end) {
            j = *pt;
            if(en_array[j].len <= 0)error("compress: en_array error");
            for(k=0;k<en_array[j].len;k++){
                num_write_bits ++;
                num_per_write_bits ++;
                byte_i = num_per_write_bits / 8;
                bit_i = num_write_bits % 8;
                flag = (long)(en_array[j].code[k] - '0');
                bit_set((en_buffer->buffer) + byte_i, bit_i, flag);
            }
            pt ++;
        }
        en_buffer->len = (num_per_write_bits + 1)/8;
        en_buffer->last_bytes = (num_write_bits + 1)%8;
        if(fwrite(en_buffer->buffer, sizeof(unsigned char), en_buffer->len, compress_file) != en_buffer->len)error("compress: fwrite error");
        if(en_buffer->last_bytes > 0) *(en_buffer->buffer) = *(en_buffer->buffer + en_buffer->len);
    }
    if((num_write_bits + 1)%8 != 0){
        if(fwrite(en_buffer->buffer, sizeof(unsigned char), 1, compress_file) != 1)error("compress:fwrite error");
        header.len_lastByte = (num_write_bits + 1)% 8;
    }
    *(buffer_header + 2*sizeof(long)+4) = header.len_lastByte;
    fseek(compress_file, 0L, SEEK_SET);
    if(fwrite(buffer_header, 1, 2*sizeof(long)+5, compress_file)!=(2*sizeof(long)+5))error("compress :fwrite error");
    
    //释放内存
    if(compress_name != nullptr){
        delete[] compress_name;
        compress_name = nullptr;
    }
    if(nodes!=nullptr){
        delete[] nodes;
        nodes = nullptr;
    }
    if(en_array != nullptr){
        delete[] en_array;
        en_array = nullptr;
    }
    if(buffer_header!=nullptr){
        delete[] buffer_header;
        buffer_header=nullptr;
    }
    
    
    if(en_buffer->buffer!=nullptr){
        delete [] en_buffer->buffer;
        en_buffer->buffer=nullptr;
    }
    if(en_buffer!=nullptr){
        delete en_buffer;
        en_buffer=nullptr;
    }
    if(fread_buffer!=nullptr){
        delete[] fread_buffer;
        fread_buffer=nullptr;
    }
    
    if(fclose(file)!=0){
        log("Jcompress: fclose src_fp error");
    }
    if(fclose(compress_file)!=0){
        log("Jcompress: fclose src_fp error\n");
    }
    log("Jcompress: succeed\n");
}
void uncompress(const char* name){
    if (access(name, 0)!=0) error("file not exit");
    log("====================uncompress file====================");
    log("compress file:",false);
    log(name);
    FILE* file = open_file(name);
    if(file == nullptr)error("uncompress:open file error");
    fseek(file, 0L, SEEK_END);
    long file_len = ftell(file),last_bits=0;
    if(file_len == -1)log("compress file too big to its len(bigger than 2.1G)");
    else printf("compress file size: %ld bytes\n",file_len);
    
    fseek(file, 0L, SEEK_SET);
    Encode* encodes = new Encode[256];
    if(encodes == nullptr)error("uncompress:new encodes error");
    char* uncompress_name = nullptr;
    //读取文件头获取基本信息和编码表
    get_head(&file,encodes,&uncompress_name,&last_bits);
    Huffman huffman=*new Huffman(encodes);
    //创建解压文件
    FILE* uncompress_file = create_file(uncompress_name);
    //解码文件
    EncodeBuffer* en_buffer = new EncodeBuffer();
    DecodeBuffer* de_buffer = new DecodeBuffer();
    if(en_buffer == nullptr || de_buffer == nullptr)error("uncompress: new en_buffer/de_buffer error");
    const long len_en_buffer = 8 * 1024 *1024;
    const long len_de_buffer = 10 * len_en_buffer;
    en_buffer->buffer = new unsigned char[len_en_buffer];
    de_buffer->buffer = new unsigned char[len_de_buffer];
    if(en_buffer->buffer == nullptr || de_buffer->buffer == nullptr)error("uncompress: new en_buffer/de_buffer->buffer error");
    
    long long read_num = 0;
    long read_bytes = 0;
    Node* last = nullptr;
    while ((read_bytes = fread(en_buffer->buffer, sizeof(unsigned char), len_en_buffer, file))!=0) {
        read_num += read_bytes;
        if(read_num == file_len && last_bits > 0){
            en_buffer->last_bytes = last_bits;
            en_buffer->len = read_bytes -1;
        }else{
            en_buffer->last_bytes = 0;
            en_buffer->len = read_bytes;
        }
        decode(huffman.get_root(), &last, en_buffer, de_buffer);
        if(fwrite(de_buffer->buffer, sizeof(unsigned char),de_buffer->len, uncompress_file) != de_buffer->len)error("uncompress:fwrite error");
    }
    log("uncompress file succeed");
    if(encodes!=nullptr){
        delete [] encodes;
        encodes = nullptr;
    }
    if(en_buffer != nullptr){
        delete en_buffer;
        en_buffer = nullptr;
    }
    if(de_buffer != nullptr){
        delete de_buffer;
        de_buffer = nullptr;
    }
    if(fclose(uncompress_file)!=0)error("uncompress:fclose error");
}
