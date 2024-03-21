//
//  17011901.c
//  hw1
//
//  Created by Muhammed Johar on 4.11.2022.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define GRAY_MIN 0
#define GRAY_MAX 255

typedef struct pgm {
      int numRows;
      int numColumes;
      int grayMax;
      char * signature;
      char * comment;

      int ** pixData;
    } pgm;

typedef struct filter{
    int size;
    long double coefficient;
    long int ** kernal;
}filter;

pgm *zeros_pgm(int rows,int columes,int gray_max){
    int i;
    pgm * img = malloc(sizeof(pgm));
    img->comment = malloc(300*sizeof(char));
    img->signature = malloc(4*sizeof(char));
    strcpy(img->comment, "# Muhammed Johar hw1\n");
    strcpy(img->signature, "P2\n");
    img->numRows = rows;
    img->numColumes = columes;
    img->grayMax = gray_max;
    img->pixData = malloc(img->numRows*sizeof(int *));
    for (i=0; i<img->numRows; i++) {
        img->pixData[i] = calloc(img->numColumes, sizeof(int));
    }
return img;
}
void free_pgm(pgm* img){
    int i;
    if (img==NULL) {
        return;
    }else{
        free(img->comment);
        free(img->signature);
        for (i=0; i<img->numRows; i++) {
            free(img->pixData[i]);
        }
        free(img->pixData);
        free(img);
    }
}
void free_filter(filter * ftr){
    int i;
    if (ftr==NULL) {
        return;
    }else{
        for (i=0; i<ftr->size; i++) {
            free(ftr->kernal[i]);
        }
        free(ftr->kernal);
        free(ftr);
    }
}

pgm* readPgm(char * filename){
    int i,j;

    pgm* img = malloc(sizeof(pgm));
    img->comment = (char*)malloc(300*sizeof(char));
    img->signature = (char*)malloc(4*sizeof(char));
    unsigned char c;
    char * line = NULL;
    size_t len = 0;
    FILE *fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Error: Unable to open file %s.\n\n", filename);
        exit(2);
    }

    getline(&line, &len, fp);
    sscanf(line,"%s\n", img->signature);
    getline(&img->comment, &len, fp);
    getline(&line, &len, fp);
    sscanf(line,"%d %d", &img->numColumes, &img->numRows );
    getline(&line, &len, fp);
    sscanf(line,"%d", &img->grayMax );
    img->pixData = (int **)malloc(img->numRows*sizeof(int*));
    for(i=0; i < img->numRows; i++){
        img->pixData[i] = (int *)malloc(img->numColumes*sizeof(int));
    }
    if(!strcmp(img->signature,"P2")){
        i=0;
        j=0;
        while (!feof(fp))
        {
            fscanf(fp, "%d", &img->pixData[i][j]);
            if(j < img->numColumes-1){
                j++;
            }else if(i<img->numRows-1){
                j=0;
                i++;
            }
        }
    }else if(!strcmp(img->signature,"P5")){
        if(img->grayMax<=GRAY_MAX){
            i=0;
            j=0;
            while (!feof(fp))
            {
                c = fgetc(fp);
                img->pixData[i][j] = (int)c;
                if(j < img->numColumes-1){
                    j++;
                }else if(i<img->numRows-1){
                    j=0;
                    i++;
                }
            }
            
        }
    }else{
        printf("Error: Unknown Signature!\n");
        exit(3);
    }

    fclose(fp);
    return img;
}

void writePgm(pgm* img, char* filename){

    int i,j,counter=0;

    FILE *fp = fopen(filename, "w");
    fprintf(fp,"%s","P2\n");
    fprintf(fp,"%s","# Muhammed Johar HW1\n");
    fprintf(fp,"%d %d\n",img->numColumes,img->numRows);
    fprintf(fp,"%d\n",img->grayMax);
    for(i=0; i<img->numRows; i++){
        for(j=0; j<img->numColumes; j++){
            fprintf(fp,"%d",img->pixData[i][j]);
            if(counter%50==0){
                fprintf(fp," \n");
            }else{
                fprintf(fp,"  ");
            }
            counter++;

        }
    }


    fclose(fp);
}

pgm * img_copy(pgm * orginal){
    pgm * copy_img = malloc(sizeof(pgm));
    copy_img->numRows = orginal->numRows;
    copy_img->numColumes = orginal->numColumes;
    copy_img->grayMax = orginal->grayMax;
    copy_img->comment = (char*)malloc(300*sizeof(char));
    copy_img->signature = (char*)malloc(4*sizeof(char));
    strcpy(copy_img->signature,orginal->signature);
    strcpy(copy_img->comment, orginal->comment);
    copy_img->pixData = (int **)malloc(copy_img->numRows*sizeof(int*));

    int i,j;


    for(i=0; i<copy_img->numRows; i++){
        copy_img->pixData[i] = (int *)malloc(copy_img->numColumes*sizeof(int));
        for ( j = 0; j < copy_img->numColumes; j++){
            copy_img->pixData[i][j] = orginal->pixData[i][j];
        }
        
    }

    

    return copy_img;
}

float gus_fun(int x,int y,float s){
    float result,exp;
    exp = (x*x + y*y)/(-2.0*s*s);
    result = 1.0/(2*M_PI*s*s);
    result = result*pow(M_E, exp);
    
    return result;
}

filter * gaussian_filter_gen(int size, float sigma){
    int i,j;
    int x,y;
    long int sum=0;
    double val,min;
    if(size%2!=1 || size <= 0){
        printf("Error: filter's size must be a  positive odd number.\n");
        exit(4);
    }
    filter *ftr = malloc(sizeof(filter));

    ftr->kernal = malloc(size*sizeof(long int *));
    for(i=0; i<size; i++){
        ftr->kernal[i] = malloc(size*sizeof(long int));
    }

    min = gus_fun(- size/2,- size/2,sigma);
    for (i = 0; i < size; i++){
        x = i - size/2;
        for(j = 0; j < size; j++){
            y = j - size/2;
            val = gus_fun(x,y,sigma);
            sum += (long int)(val/min);
            ftr->kernal[i][j] = (long int)(val/min);
        }
    }
    for (i=0; i<size; i++) {
        for (j=0; j<size; j++) {
            printf("%5ld\t",ftr->kernal[i][j]);
        }
        printf("\n");
    }
    
    printf("sum = %ld  \n",sum);
    ftr->coefficient = 1.0/sum;
    printf("coeff = %.15Lf  \n",ftr->coefficient);
    ftr->size = size;

    return ftr;
}
filter * sobel_filter_gen(char direction){
    int gradient[] = {1,2,1};
    int i,j;
    filter * ftr = malloc(sizeof(filter));
    ftr->size = 3;
    ftr->coefficient = 1;
    ftr->kernal = malloc(ftr->size*sizeof(long int *));
    for(i=0; i<ftr->size; i++){
        ftr->kernal[i] = malloc(ftr->size*sizeof(long int));
        for (j=0; j<ftr->size; j++) {
            if(direction=='h'){
                ftr->kernal[i][j] = (ftr->size/2 -i)*gradient[j];
                
            }else if(direction=='v'){
                ftr->kernal[i][j] = (ftr->size/2 -j)*gradient[i];
            }else{
                printf("Error: sobel filter, invalid direction.\n");
                exit(6);
            }
        }
        
    }
    printf("\n\ndirection = %c\n",direction);
    for (i=0; i<ftr->size; i++) {
        for (j=0; j<ftr->size; j++) {
            printf("%ld\t",ftr->kernal[i][j]);
        }
        printf("\n");
    }
    
    return ftr;
}
pgm * sobel_both_direction(pgm *h, pgm *v){
    pgm *result;
    int i,j;
    result = zeros_pgm(h->numRows, h->numColumes, h->grayMax);
    for (i=0; i<result->numRows; i++) {
        for (j=0; j<result->numColumes; j++) {
            result->pixData[i][j] = sqrt(h->pixData[i][j]*h->pixData[i][j] + v->pixData[i][j]*v->pixData[i][j]);
        }
    }
    return result;
}

filter * laplacian_filter_gen(long int k[3][3]){
    int i,j;
    filter * ftr= malloc(sizeof(filter));
    ftr->size = 3;
    ftr->coefficient = 1.0;
    ftr->kernal = malloc(ftr->size*sizeof(long int *));
    for (i=0; i<ftr->size; i++) {
        ftr->kernal[i] = malloc(ftr->size*sizeof(long int));
        for (j=0; j<ftr->size; j++) {
            ftr->kernal[i][j] = k[i][j];
        }
    }
    return  ftr;
}


pgm * apply_filter(pgm * img, filter* ftr){
    int i,j,x,y,disp = (ftr->size)/2;
    double val;
    pgm * cpy;
    cpy = img_copy(img);
    for(i=disp; i< img->numRows-disp;i++){
        for(j=disp; j<img->numColumes-disp;j++){
            val = 0.0;
            for(x=0; x< ftr->size; x++){
                for(y=0; y<ftr->size; y++){
                    val += ftr->kernal[x][y] * img->pixData[i+x-disp][j+y-disp];
                }
            }
            cpy->pixData[i][j] = (int)(val*ftr->coefficient);
        }
    }
    return cpy;
}

pgm * normaization(pgm* img, int new_min, int new_max){
    pgm* res = zeros_pgm(img->numRows, img->numColumes, img->grayMax);
    int i,j,old_min=GRAY_MIN,old_max=GRAY_MAX;
    
    for (i=0; i<img->numRows; i++) {
        for (j=0; j<img->numColumes; j++) {
            if(img->pixData[i][j]< old_min){
                old_min = img->pixData[i][j];
            }
            if(img->pixData[i][j]> old_max){
                old_max = img->pixData[i][j];
            }
        }
    }
    for (i=0; i<res->numRows; i++) {
        for (j=0; j<res->numColumes; j++) {
            res->pixData[i][j] = (img->pixData[i][j] - old_min)*(new_max - new_min)/(old_max - old_min) + new_min;
        }
    }
    
    return res;
}
char* path_gen(char* dir_path,char * str,char* fname, int kernal_size,int sigma ){
    char * prefix = calloc(50, sizeof(char));;
    char * p = calloc(50, sizeof(char));;
    strcat(p, dir_path);
    sprintf(prefix,"%s_%d_%d_", str,kernal_size,sigma);
    strcat(p, prefix);
    strcat(p, fname);
    
    return p;
}


int main(){
    char *dir_path = calloc(50, sizeof(char));
    char *p= calloc(50, sizeof(char));;
    int gauss_size,sigma;
    pgm *img, *res_gaus, *res_sobel_vertical, *res_sobel_horizontal, *res_sobel_both , * res_normaized, *res_lap1, *res_lap2;
    long int lap_kernal_1[3][3]={{0, -1, 0},{-1, 4, -1},{0, -1, 0}};
    long int lap_kernal_2[3][3]={{-1, -1, -1},{-1, 8, -1},{-1, -1, -1}};
    filter *gaus_fil,*sobel_horizontal,*sobel_vertical, *laplacian_fil1, *laplacian_fil2;
    
    strcpy(dir_path, "/Users/mjohar/Desktop/hw1/results/");
    char* fname = "fractal_tree.ascii.pgm";
    gauss_size = 9;
    sigma = 2;
    
    strcat(p, dir_path);
    strcat(p, fname);
    
    
    gaus_fil = gaussian_filter_gen(gauss_size,sigma);
    img = readPgm(p);
    res_gaus = apply_filter(img,gaus_fil);
    res_normaized = normaization(res_gaus, GRAY_MIN, GRAY_MAX);
    res_gaus = img_copy(res_normaized);
    writePgm(res_gaus,path_gen(dir_path, "res_gauss", fname,gauss_size,sigma));
    sobel_vertical = sobel_filter_gen('v');
    sobel_horizontal = sobel_filter_gen('h');

    
    res_sobel_vertical = apply_filter(img,sobel_vertical);
    res_sobel_horizontal = apply_filter(img,sobel_horizontal);
    res_sobel_both = sobel_both_direction(res_sobel_horizontal ,res_sobel_vertical );
    
    res_normaized = normaization(res_sobel_horizontal, GRAY_MIN, GRAY_MAX);
    writePgm(res_normaized,path_gen(dir_path, "res_sobel_hori", fname,gauss_size,sigma));

    
    res_normaized = normaization(res_sobel_horizontal, GRAY_MIN, GRAY_MAX);
    writePgm(res_normaized,path_gen(dir_path, "res_sobel_veri", fname,gauss_size,sigma));

    
    res_normaized = normaization(res_sobel_both, GRAY_MIN, GRAY_MAX);
    writePgm(res_normaized, path_gen(dir_path, "res_sobel_both", fname,gauss_size,sigma));

    
    laplacian_fil1 = laplacian_filter_gen(lap_kernal_1);
    res_lap1 = apply_filter(img, laplacian_fil1);
    res_normaized = normaization(res_lap1, GRAY_MIN, GRAY_MAX);
    writePgm(res_normaized, path_gen(dir_path, "res_lablacian_kernal1", fname,gauss_size,sigma));

    
    laplacian_fil2 = laplacian_filter_gen(lap_kernal_2);
    res_lap2 = apply_filter(img, laplacian_fil2);
    res_normaized = normaization(res_lap2, GRAY_MIN, GRAY_MAX);
    writePgm(res_normaized, path_gen(dir_path, "res_lablacian_kernal2", fname,gauss_size,sigma));
    
    
    
    
    free_pgm(res_normaized);
    free_pgm(img);
    free_pgm(res_gaus);
    free_pgm(res_sobel_vertical);
    free_pgm(res_sobel_horizontal);
    free_pgm(res_sobel_both);
    
    free_filter(gaus_fil);
    free_filter(sobel_horizontal);
    free_filter(sobel_vertical);
    
    
    return 0;
}
