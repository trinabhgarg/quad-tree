#include<iostream>

#include<opencv2/opencv.hpp>

#include<stdint.h>

#include<typeinfo>

using namespace cv;

using namespace std;



//defining the structure of node of quadtree

typedef struct qnode{

    bool isleaf;

    int top;

    int bottom;

    int left;

    int right;

    struct qnode* child[4];//Array of child contains(NW,NE,SW,SE)

    int gsvalue;//gray scale value

    int *p;//Pixel of arrays

    int rows;//No. of rows of image

    int cols;//No. of cols of image

    

}qnode;



//Creating a quadtree node by allocating memory and setting the initial values

qnode *newnode(){

    qnode* node=(qnode*)malloc(sizeof(qnode));

    node->isleaf=false;

    node->top=0;

    node->bottom=0;

    node->left=0;

    node->right=0;

    node->child[0]=NULL;

    node->child[1]=NULL;

    node->child[2]=NULL;

    node->child[3]=NULL;

    node->gsvalue=0;

    return node;

}



//Adding pixel array and no. of rows and no. of cols to the node

void setqnode(qnode *node, int nrows, int ncols, int *p){

    node->p=p;

    node->rows=nrows;

    node->cols=ncols;

    node->top=0;

    node->bottom=nrows-1;

    node->left=0;

    node->right=ncols-1;

}



//Splitting quadtree node into 4 child with corresponding values of rows and columns

void splitqnode(qnode *node){

    int r1=node->top;//row 1

    int r2=node->bottom;//row 2

    int c1=node->left;//column 1

    int c2=node->right;//column 2



    node->child[0]=newnode();

    node->child[0]->p=node->p;

    node->child[0]->top=r1;

    node->child[0]->bottom=(r1+r2)/2;

    node->child[0]->left=c1;

    node->child[0]->right=(c1+c2)/2;

    node->child[0]->rows=node->rows;

    node->child[0]->cols=node->cols;



    if(c1!=c2){

        node->child[1]=newnode();

        node->child[1]->p=node->p;

        node->child[1]->top=r1;

        node->child[1]->bottom=(r1+r2)/2;

        node->child[1]->left=(c1+c2)/2+1;

        node->child[1]->right=c2;

        node->child[1]->rows=node->rows;

        node->child[1]->cols=node->cols;

    }

    if(r1!=r2){

        node->child[2]=newnode();

        node->child[2]->p=node->p;

        node->child[2]->top=(r1+r2)/2+1;

        node->child[2]->bottom=r2;

        node->child[2]->left=c1;

        node->child[2]->right=(c1+c2)/2;

        node->child[2]->rows=node->rows;

        node->child[2]->cols=node->cols;

    }

    if((c1!=c2)&&(r1!=r2)){

        node->child[3]=newnode();

        node->child[3]->p=node->p;

        node->child[3]->top=(r1+r2)/2+1;

        node->child[3]->bottom=r2;

        node->child[3]->left=(c1+c2)/2+1;

        node->child[3]->right=c2;

        node->child[3]->rows=node->rows;

        node->child[3]->cols=node->cols;

    }



}



//Calculating the average pixel value

float avg_pixel_value(qnode* node){

    float total=0;

    int top=node->top;

    int bottom=node->bottom;

    int left=node->left;

    int right=node->right;

    int area=(bottom-top+1)*(right-left+1);

    int cols=node->cols;

    for(int i=top;i<bottom+1;i++){

        for(int j=left;j<right+1;j++){

            total=total+node->p[i*cols+j];

        }

    }

    float average=float(total)/float(area);

    return average;

}





//Calculating the pixel variance

float pixel_variance(qnode* node){

    float total_variance=0;

    int top=node->top;

    int bottom=node->bottom;

    int left=node->left;

    int right=node->right;

    int area=(bottom-top+1)*(right-left+1);

    int cols=node->cols;

    float average=avg_pixel_value(node);

    for(int i=top;i<bottom+1;i++){

        for(int j=left;j<right+1;j++){

            total_variance=total_variance+(average-node->p[i*cols+j])*(average-node->p[i*cols+j]);

        }

    }

    float variance=total_variance/area;

    return variance;

}



//Function to build quadtree

void build_qdtree(qnode* node, int threshold){

    if(node==NULL){

        return;

    }

    float variance=pixel_variance(node);

    //Splitting the node if variance is greater than threshold value

    if(variance>threshold){

        splitqnode(node);

        build_qdtree(node->child[0], threshold);

        build_qdtree(node->child[1], threshold);

        build_qdtree(node->child[2], threshold);

        build_qdtree(node->child[3], threshold);

    }else{

        float average=avg_pixel_value(node);

        node->gsvalue=average;

        node->isleaf=true;

    }

}



//Calculating the number of nodes present in quadtree

int num_tree_nodes(qnode* node){

    if(node==NULL){

        return 0;

    }else if(node->isleaf==true){

        return 1;

    }else{

        return num_tree_nodes(node->child[0])+num_tree_nodes(node->child[1])+num_tree_nodes(node->child[2])+num_tree_nodes(node->child[3]);

    }

}



//Calculating the total number of pixels in the quadtree formed

int check_pixel_num(qnode* node){

    if(node==NULL){

        return 0;

    }else if(node->isleaf==true){

        int size=((node->bottom)-(node->top)+1)*((node->right)-(node->left)+1);

        return size;

    }else{

        return check_pixel_num(node->child[0])+check_pixel_num(node->child[1])+check_pixel_num(node->child[2])+check_pixel_num(node->child[3]);

    }

}



//decode the tree into matrix

void decode_tree(qnode* node, cv::Mat& modified, int rows, int cols){

    if(node==NULL){

        return;

    }else if(node->isleaf==true){

        int top=node->top;

        int bottom=node->bottom;

        int left=node->left;

        int right=node->right;

        int rgbvalue=node->gsvalue;

        for(int i=top;i<=bottom;i++){

            for(int j=left;j<=right;j++){

                modified.at<uint8_t>(i,j)=rgbvalue;

            }

        }

    }else{

        decode_tree(node->child[0], modified, rows, cols);

        decode_tree(node->child[1], modified, rows, cols);

        decode_tree(node->child[2], modified, rows, cols);

        decode_tree(node->child[3], modified, rows, cols);

    }

}





int main(int argc,const char* argv[]){

    int default_threshold=100;//default threshold value

    int threshold=default_threshold;

    string s;

    cout<<"Please enter the path of image\n";

    cin>>s;

    Mat original=imread(s, IMREAD_GRAYSCALE);//Reading original image

    Mat modified=imread(s, IMREAD_GRAYSCALE);//Initializing an empty matrix for modified image with original image to get the same size

    while(original.empty()){

        if(original.empty()){

            cout<<"No image found at entered path, please enter the path again\n";

            cin>>s;

            original=imread(s, IMREAD_GRAYSCALE);

            modified=imread(s, IMREAD_GRAYSCALE);

        }

    }

    char x;

    cout<<"Enter Y if you want to change the threshold value or press N if you wish to continue with default threshold value\n";

    cin>>x;

    if(x=='Y'){

        cout<<"Please enter the new threshold value\n";

        cin>>threshold;

    }else if(x=='N'){

        threshold=default_threshold;

    }

    int rows=modified.rows;//count of rows in matrix

    int cols=modified.cols;//count of cols in matrix

    int total_pixels=rows*cols;

    

    int *pixels=(int*)malloc(sizeof(int)*(rows*cols));//allocating memory to the pixel array

    for(int i=0;i<rows;i++){

        for(int j=0;j<cols;j++){

            pixels[i*cols+j]=modified.at<uint8_t>(i,j);

        }

    }

    

    //declaring a root for quad tree

    qnode* node;

    node=newnode();

    //We are adding the values to quadtree node here
    setqnode(node,rows,cols,pixels);

    //building the quadtree here based on threshold value
    build_qdtree(node,threshold);

    // calculating the percentage compression 
    int e= check_pixel_num(node);
    int f= num_tree_nodes(node);
    int PercentageCompresion = ((e-f)*100)/e;
    cout<<"Original Pixel: "<<e<<endl;
    cout<<"No.of Pixel after Compression: "<<f<<endl;
    cout<<"Compresion Ratio(in percentage): "<<PercentageCompresion<<"%"<<endl;

    decode_tree(node, modified, rows, cols);
    //making matrix from the compressed image(modified)


    delete[] pixels;

    imshow("Original", original);

    imshow("Compressed", modified);

    imwrite("Modified.jpeg", modified);

    waitKey(0);



    return 0;

}
