#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include<float.h>
#include<stdbool.h>

// points

struct point {
    float x;
    float y;
};

typedef struct point point;

point* newpoint(float x,float y){
    // creates a new point
    point* p = (point*)malloc(sizeof(point));
    p->x = x;
    p->y = y;
    return p;
}

// chech if two points are equal

int equal(point* p1,point* p2){
    // checks if two points are equal
    return (p1->x == p2->x && p1->y == p2->y);
}

// region 
// we are moving in clock wise direction always for region


struct region {
    point* p1; // top left
    point* p2; // top right
    point* p3; // bottom right
    point* p4;  // bottom left
};

typedef struct region region;

region* newregion(point* p1,point* p2,point* p3,point* p4){
    // creates new region
    region* r = (region*)malloc(sizeof(region));
    r->p1 = p1;
    r->p2 = p2;
    r->p3 = p3;
    r->p4 = p4;
    return r;
}

//find if point in region or not

int ispointinregion(point* p,region* r){
    if(p->x >= r->p1->x && p->x <= r->p2->x && p->y <= r->p1->y && p->y >= r->p4->y){
        
        return 1;
    }
    //printf("r2\n");
    return 0;
}

// find which region point is in

int findregion(point*p, region* r){
    // 1 for top left N/W
    // 2 for top right N/E
    // 3 for bottom right S/E
    // 4 for bottom left S/W
    // tells in which sub region the point should go
    float xmid = (r->p1->x + r->p2->x)/2;
    float ymid = (r->p1->y + r->p4->y)/2;
    if(p->x <= xmid && p->y >= ymid){
        return 1;
    }
    else if(p->x >= xmid && p->y >= ymid){
        return 2;
    }
    else if(p->x >= xmid && p->y <= ymid){
        return 3;
    }
    else if(p->x <= xmid && p->y <= ymid){
        return 4;
    }
    return 0;
}

int box_intersect_check(region* b1,region* b2)
{
    // checks if two region intersect, if one region is another
    if(b2->p1->x > b1->p1->x && b2->p2->x < b1->p2->x && b2->p1->y < b1->p1->y && b2->p3->y > b1->p3->y)
        return 1;
    else if(b2->p1->x < b1->p1->x && b2->p2->x > b1->p2->x && b2->p1->y > b1->p1->y && b2->p3->y < b1->p3->y)
        return 2;
    else if(b1->p2->x >= b2->p1->x && b1->p2->x <= b2->p2->x)
    {
        if(b1->p3->y <= b2->p1->y && b1->p3->y >= b2->p3->y)
            return 1;
        else if(b1->p1->y <= b2->p1->y && b1->p1->y >= b2->p3->y)
            return 1;
    }
    else if(b1->p1->x <= b2->p2->x && b1->p1->x >= b2->p1->x)
    {
        if(b1->p1->y <= b2->p1->y && b1->p1->y >= b2->p3->y)
            return 1;
        else if(b1->p3->y <= b2->p1->y && b1->p3->y >= b2->p3->y)
            return 1;
    }
    return 0;
}

// quad tree

struct quadtree{
    region* r;
    point* p;
    struct quadtree* nw;
    struct quadtree* ne;
    struct quadtree* se;
    struct quadtree* sw;
};

typedef struct quadtree quadtree;

// new quad tree 

quadtree* newquadtree(point* p1,point* p2,point* p3,point* p4){
    // creates an empty quad tree whose region top left point is p1 and bottom right point is p3
    quadtree* qt = (quadtree*)malloc(sizeof(quadtree));
    qt->p = NULL;
    qt->nw = NULL;
    qt->ne = NULL;
    qt->se = NULL;
    qt->sw = NULL;
    qt->r = newregion(p1,p2,p3,p4);
    return qt;
}

// insert in quad tree

void insertquadtree(quadtree* qt,point* p){
    if (qt == NULL){
        return;
    }
    else if (ispointinregion(p,qt->r)==1){
        if (qt->p == NULL){
            // base case for recursion
            // if there is no point then we insert the point in the tree
            //by creating new quad trees as its children.
            qt->p = p;
            point* tophalf = newpoint((qt->r->p1->x + qt->r->p2->x)/2,qt->r->p1->y);
            point* bottomhalf = newpoint((qt->r->p1->x + qt->r->p2->x)/2,qt->r->p4->y);
            point* lefthalf = newpoint(qt->r->p1->x,(qt->r->p1->y + qt->r->p4->y)/2);
            point* righthalf = newpoint(qt->r->p2->x,(qt->r->p1->y + qt->r->p4->y)/2);
            point* center = newpoint((qt->r->p1->x + qt->r->p2->x)/2,(qt->r->p1->y + qt->r->p4->y)/2);
            qt->nw= newquadtree(qt->r->p1,tophalf,center,lefthalf);
            qt->ne= newquadtree(tophalf,qt->r->p2,righthalf,center);
            qt->se= newquadtree(center,righthalf,qt->r->p3,bottomhalf);
            qt->sw= newquadtree(lefthalf,center,bottomhalf,qt->r->p4);
            printf("Point inserted \n");
        }
        else {
            // if there is a point in the tree then we insert the point in the appropriate child
            if (ispointinregion(p,qt->nw->r)){
                insertquadtree(qt->nw,p);
            }
            else if (ispointinregion(p,qt->ne->r)){
                insertquadtree(qt->ne,p);
            }
            else if (ispointinregion(p,qt->se->r)){
                insertquadtree(qt->se,p);
            }
            else if (ispointinregion(p,qt->sw->r)){
                insertquadtree(qt->sw,p);
            }
        }
    }
    else {
        printf("point not in region\n");
    }
}

// searching in quad tree

int searchquadtree(quadtree* qt,point* p){
    if (qt->p == NULL){
        // base case for recursion
        return 0;
    }
    else{
        if (equal(qt->p,p)){
            // also base case for recursion
            return 1; // the point is found
        }
        else {
            // determines in which sub region the point is and goes into that region 
            int region = findregion(p,qt->r);
            if (region == 1){
                return searchquadtree(qt->nw,p);
            }
            else if (region == 2){
                return searchquadtree(qt->ne,p);
            }
            else if (region == 3){
                return searchquadtree(qt->se,p);
            }
            else if (region == 4){
                return searchquadtree(qt->sw,p);
            }
        }

    }
}

void range_query(quadtree* qt,region* re)
{
    // prints all the points that are present in the region
    if(qt==NULL || qt->p==NULL)
        return;
    if(box_intersect_check(qt->r,re)==0)
    {
        return;
    }
    if(box_intersect_check(qt->r,re)==2)
    {
        re = qt->r;
    }
    if(ispointinregion(qt->p, re)==1)
    {
        printf("%f %f\n",qt->p->x,qt->p->y);
    }
    range_query(qt->nw,re);
    range_query(qt->ne,re);
    range_query(qt->se,re);
    range_query(qt->sw,re);
}

int main(){
    // choices 
    // 1 for new quad tree
    // 2 for insert point 
    // 3 for search a point
    // 4 for range query
    // 5 Exit
    int choice = 1;
    quadtree *qt = NULL;
    while (1)
    {
        if (choice == 1)
        {
            /* code */
            printf("To make the quad tree please enter all the four points of parallelogram in clockwise direction\n");
            float x1, y1, x2, y2, x3, y3, x4, y4;
            printf("Enter first point: ");
            scanf("%f", &x1);
            scanf("%f", &y1);  
            printf("Enter second point: ");
            scanf("%f", &x2);
            scanf("%f", &y2);
            printf("Enter third point: ");
            scanf("%f", &x3);
            scanf("%f", &y3);
            printf("Enter fourth point: ");
            scanf("%f", &x4);
            scanf("%f", &y4);
            point *p1 = newpoint(x1, y1);
            point *p2 = newpoint(x2, y2);
            point *p3 = newpoint(x3, y3);
            point *p4 = newpoint(x4, y4);
            qt = newquadtree(p1, p2, p3, p4);
        }
        else if (choice == 2)
        {
            /* code */
            printf("Enter the point to be inserted: ");
            float x, y;
            scanf("%f", &x);
            scanf("%f", &y);
            point *p = newpoint(x, y);
            insertquadtree(qt, p);
            
        }
        else if (choice == 3)
        {
            /* code */
            printf("Enter the point to be searched: ");
            float x, y;
            scanf("%f", &x);
            scanf("%f", &y);
            point *p = newpoint(x, y);
            int result = searchquadtree(qt, p);
            if (result == 1)
            {
                printf("Point found\n");
            }
            else
            {
                printf("Point not found\n");
            }
        }
        else if (choice ==4)
        {
            /* code */
            printf("Enter the range of points to be searched in clockwise direction\n");
            float x1, y1, x2, y2, x3, y3, x4, y4;
            printf("Enter first point: ");
            scanf("%f", &x1);
            scanf("%f", &y1);
            printf("Enter second point: ");
            scanf("%f", &x2);
            scanf("%f", &y2);
            printf("Enter third point: ");
            scanf("%f", &x3);
            scanf("%f", &y3);
            printf("Enter fourth point: ");
            scanf("%f", &x4);
            scanf("%f", &y4);
            point *p1 = newpoint(x1, y1);
            point *p2 = newpoint(x2, y2);
            point *p3 = newpoint(x3, y3);
            point *p4 = newpoint(x4, y4);
            region *r = newregion(p1, p2, p3, p4);
            printf("Points in the range are: \n");
            range_query(qt, r);
        }
        else if (choice == 5)
        {
            /* code */
            printf("Thank you for using the program");
            break;
        }
    


        printf("\nEnter the number corresponding to the operation you want to perform\n1. To create a new quad tree\n2. To insert an element into it\n");
        printf("3. Search an element\n4. Perform range query\n5. To EXIT\n");
        printf("Enter your choice :");
        scanf("%d", &choice);
    }
    

    return 0;
}
