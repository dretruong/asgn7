#include "io.h"

static uint8_t bitbuf[4096];
static uint16_t bitindex = 0;
static uint8_t symbuf[4096];
static uint16_t symindex = 0;
float compress_stat = 0;
float uncompressed_stat = 0;
int read_bytes(int infile, uint8_t *buf, int to_read){
    int rbytes = 0;
    int total = 0;
    do{
        rbytes = read(infile,buf + total, to_read-total);
        if(rbytes == -1){
            return -1;
        }
        total += rbytes;
    }while((rbytes >0) && (total != to_read));
    uncompressed_stat += total;
    return total;
}

int write_bytes(int outfile, uint8_t *buf, int to_write){
    int rbytes = 0;
    int total = 0;
    do{
        rbytes = write(outfile,buf + total, to_write - total);
        if(rbytes == -1){
            return -1;
        }
        total += rbytes;
    }while((rbytes >0) && (total != to_write));
    compress_stat += total;   
    return total;
}

void read_header(int infile, FileHeader *header){
    read_bytes(infile,(uint8_t*)header, sizeof(FileHeader));
    return;
}

void write_header(int outfile, FileHeader *header){
    write_bytes(outfile,(uint8_t*)header, sizeof(FileHeader));
    return;
}

bool read_sym(int infile, uint8_t *byte){
    static int end = 0;
    if(symindex == 0){
        end = read_bytes(infile,symbuf,4096);
    }
    *byte = symbuf[symindex++];
    if(symindex == 4096){
        symindex = 0;
    }
    if(end == 4096){
        return true;
    }
    else{
        if(symindex == end + 1){
            return false;
        }
        else{
            return true;
        }
    }
}

void buffer_pair(int outfile, uint16_t code, uint8_t sym, uint8_t bit_len){
    for(int bit =0; bit <bit_len; bit+= 1){
        if(code & 1){ //gets bit 
            bitbuf[bitindex / 8] |= (1 << (bitindex % 8)); // sets bit
        }
        else{
            bitbuf[bitindex / 8] &= ~(1 << bitindex % 8); // clear  bit
        }
        bitindex += 1;
        code >>= 1;
        if(bitindex == 4096 * 8){
            write_bytes(outfile,bitbuf,4096);
            bitindex = 0;
        }
    }
    for(int i = 0; i < 8; i += 1){
        if(sym & 1){
            bitbuf[bitindex/8] |= (1 << (bitindex % 8));
        }
        else{
            bitbuf[bitindex / 8] &= ~(1<<(bitindex % 8));
        }
        bitindex += 1;
        sym >>= 1;
        if(bitindex == 4096 * 8){
            write_bytes(outfile, bitbuf, 4096);
            bitindex = 0;
        }
    }
}

void flush_pairs(int outfile){
    int bytes;
    //int syms;
    if(bitindex !=0){
        if(bitindex % 8 == 0){
            bytes = (bitindex / 8);
        }
        else{
            bytes = ((bitindex/8) + 1);
        }
        write_bytes(outfile,bitbuf,bytes);
    }
    //if(bitindex !=0){
    //    if(bitindex % 8 == 0){
    //        syms = (bitindex / 8);
    //    }
    //    else{
    //        syms = (bitindex/8 + 1);
    //    }
    //    write_bytes(outfile,bitbuf,syms);
   // }
}


bool read_pair(int infile, uint16_t *code, uint8_t *sym, uint8_t bit_len){
    *code = 0;
    *sym = 0;
    if(bitindex == 0){
      read_bytes(infile,bitbuf,4096);
    }
    for(uint8_t bit =0; bit <bit_len; bit+= 1){
       
        if((bitbuf[bitindex/8] & (1 << bitindex % 8))){ //gets bit and checks if == 1
            *code |= (1 << (bit)); // sets bit
        }
        else{
            *code &= ~(1 << bit); // clears bit
        }
        bitindex += 1;
        if(bitindex == 4096 * 8){
            read_bytes(infile,bitbuf,4096);
            bitindex = 0;
        }
    }
    if(*code == STOP_CODE){
      return false;
    }
    for(int i = 0; i < 8; i += 1){
        if((bitbuf[bitindex/8] & (1 << bitindex % 8))){
            *sym |= (1 << (i));
        }
        else{
            *sym  &= ~(1<<(i));
        }
        bitindex += 1;
        if(bitindex == 4096 * 8){
            read_bytes(infile, bitbuf, 4096);
            bitindex = 0;
        }
    }
    return true;
}

void buffer_word(int outfile, Word *w){
    for(uint32_t i = 0; i < w->len; i += 1){
        symbuf[symindex] = w->syms[i];
        symindex++;
        if(symindex == 4096){
            write_bytes(outfile,symbuf,4096);
            symindex = 0;
        }
    }
}

void flush_words(int outfile){
    if(symindex !=0){
        write_bytes(outfile,symbuf,symindex);
        symindex = 0;
    }
}
