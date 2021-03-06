/* 
 * Original author:  UNKNOWN
 *
 * Modified:         Kai Shen (January 2010)
 *                   Ben Cribb (Febuary 2014)
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>

/* #define DEBUG */

#define SWAP(a,b)       {double tmp; tmp = a; a = b; b = tmp;}

/* Solve the equation:
 *   matrix * X = R
 */

double **matrix, *X, *R;

/* Pre-set solution. */

double *X__;
int task_num; //define some way to customize this later if there's time
/* Initialize pthread stuff */
int strategy = 1; //which matrix factorization strategy to use, 0 = rows
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void barrier (int expect)
{
    static int arrived = 0;

    pthread_mutex_lock (&mut);  //lock

    arrived++;
    if (arrived < expect)
        pthread_cond_wait (&cond, &mut);
    else {
        arrived = 0;            // reset the barrier before broadcast is important
        pthread_cond_broadcast (&cond);
    }

    pthread_mutex_unlock (&mut);        //unlock
}


/* Initialize the matirx. */

int initMatrix(const char *fname)
{
    FILE *file;
    int l1, l2, l3;
    double d;
    int nsize; //the number of columns
    int i, j;
    double *tmp;
    char buffer[1024];

    if ((file = fopen(fname, "r")) == NULL) {
	fprintf(stderr, "The matrix file open error\n");
        exit(-1);
    }
    
    /* Parse the first line to get the matrix size. */
    fgets(buffer, 1024, file);
    sscanf(buffer, "%d %d %d", &l1, &l2, &l3);
    nsize = l1;
#ifdef DEBUG
    fprintf(stdout, "matrix size is %d\n", nsize);
#endif

    /* Initialize the space and set all elements to zero. */
    matrix = (double**)malloc(nsize*sizeof(double*));
    assert(matrix != NULL);
    tmp = (double*)malloc(nsize*nsize*sizeof(double));
    assert(tmp != NULL);    
    for (i = 0; i < nsize; i++) {
        matrix[i] = tmp;
        tmp = tmp + nsize;
    }
    for (i = 0; i < nsize; i++) {
        for (j = 0; j < nsize; j++) {
            matrix[i][j] = 0.0; //matrix[i] is column, matrix[i][j] is row
        }
    }

    /* Parse the rest of the input file to fill the matrix. */
    for (;;) {
	fgets(buffer, 1024, file);
	sscanf(buffer, "%d %d %lf", &l1, &l2, &d);
	if (l1 == 0) break;

	matrix[l1-1][l2-1] = d;
#ifdef DEBUG
	fprintf(stdout, "row %d column %d of matrix is %e\n", l1-1, l2-1, matrix[l1-1][l2-1]);
#endif
    }

    fclose(file);
    return nsize;
}

/* Initialize the right-hand-side following the pre-set solution. 
this is the vector we multiply the matrix by.  We don't need to worry about it much*/

void initRHS(int nsize)
{
    int i, j;

    X__ = (double*)malloc(nsize * sizeof(double));
    assert(X__ != NULL);
    for (i = 0; i < nsize; i++) {
	X__[i] = i+1;
    }

    R = (double*)malloc(nsize * sizeof(double));
    assert(R != NULL);
    for (i = 0; i < nsize; i++) {
	R[i] = 0.0;
	for (j = 0; j < nsize; j++) {
	    R[i] += matrix[i][j] * X__[j];
	}
    }
}

/* Initialize the results. this array is the vector that will take the solution ultimately, ignore mostly*/

void initResult(int nsize)
{
    int i;

    X = (double*)malloc(nsize * sizeof(double));
    assert(X != NULL);
    for (i = 0; i < nsize; i++) {
	X[i] = 0.0;
    }
}

/* Get the pivot - the element on column with largest absolute value. 
Parallelization strategy:  Give chunks of the column to each thread, have them
compute the largest element in thier chunk, then merge the results.*/

void getPivot(int nsize, int currow, int task_id)
{
    int i, pivotrow;

    pivotrow = currow;
    for (i = currow+1; i < nsize; i++) {
	if (fabs(matrix[i][currow]) > fabs(matrix[pivotrow][currow])) {
	    pivotrow = i;
	}
    }

    if (fabs(matrix[pivotrow][currow]) == 0.0) {
        fprintf(stderr, "The matrix is singular\n");
        exit(-1);
    }
    
    if (pivotrow != currow) {
#ifdef DEBUG
	fprintf(stdout, "pivot row at step %5d is %5d\n", currow, pivotrow);
#endif
        for (i = currow; i < nsize; i++) {
            SWAP(matrix[pivotrow][i],matrix[currow][i]);
        }
        SWAP(R[pivotrow],R[currow]);
    }
}

struct stupidargs {
    int task_id;
    int nsize;
};


/* For all the rows, get the pivot and eliminate all rows and columns
 * for that particular pivot row. EDIT THIS FUNTION*/

void *ComputeGauss(void *arguments)
{
    double *nstart;
    double *nend;
    double *mstart;
    double *mend;
    nstart = (double *) malloc (sizeof (double) * task_num);
    nend = (double *) malloc (sizeof (double) * task_num);
    mstart = (double *) malloc (sizeof (double) * task_num);
    mend = (double *) malloc (sizeof (double) * task_num);
    struct stupidargs *args = arguments;
    int task_id = args->task_id;
    int nsize = args->nsize;
    int i, j, k;
    
    double pivotval;
    //for each column, do the gausian thing to zero out the value of each row except the pivot
    //this part cannot be parallelized because the value of one loop effects the value of the
    //next loop.
    for (i = 0; i < nsize; i++) {

        //for each column, do the gausian thing to zero out the value of each row except the pivot
        //this part cannot be parallelized because the value of one loop effects the value of the
        //next loop.
        if(task_id==0) { 
            //add a multithreaded thing here
            getPivot(nsize,i,0);
        }
        barrier (task_num);

        if(task_id==0) {
            //wait until all threads are finished and pick the best pivot of the results

            //for each element in the column, multiply by scaling factor, test parallelzing
            //such that chunks of the column are handled by different threads (low priority)
            /* Scale the main row. */
            pivotval = matrix[i][i];
            if (pivotval != 1.0) {
                matrix[i][i] = 1.0;
                for (j = i + 1; j < nsize; j++) {
            	matrix[i][j] /= pivotval;
                }
                R[i] /= pivotval;
            }
        }
        barrier(task_num);
        if (strategy == 0) //row strategy
        {
            //for every row, add/subtract row1 such that element 1 of that column equals zero
            //assign chunks of work to threads        
            /* Factorize the rest of the matrix. */
            double dogs = ceil(1.22312);
            nstart[task_id] = ceil((double)(((nsize - i)/task_num)*task_id))+i+1;
            nend[task_id] = ceil((double) ( ((nsize - i)/(task_num))*(task_id+1) ) )+i;
            /*
            Something is off in my math here and it's creating a lot of error...  if you have time could you look at it elvis?
            */
            for (j = (int)nstart[task_id]; j < (int)nend[task_id]; j++) {
                pivotval = matrix[j][i];
                matrix[j][i] = 0.0;
                for (k = i + 1; k < nsize; k++) {
                    matrix[j][k] -= pivotval * matrix[i][k];
                }
                R[j] -= pivotval * R[i];
            }
        }
        if (strategy == 1)//column strategy
        {
            //for every row, add/subtract row1 such that element 1 of that column equals zero
            //assign chunks of work to threads        
            /* Factorize the rest of the matrix. */
            double dogs = ceil(1.22312);
            nstart[task_id] = ceil((double)(((nsize - i)/task_num)*task_id))+i+1;
            nend[task_id] = ceil((double) ( ((nsize - i)/(task_num))*(task_id+1) ) )+i;
            /*
            Something is off in my math here and it's creating a lot of error...  if you have time could you look at it elvis?
            */
            for (j = i+1; j < nsize; j++) {
                pivotval = matrix[j][i];
                matrix[j][i] = 0.0;
                for (k = (int)nstart[task_id]; k < (int)nend[task_id]; k++) {
                    matrix[j][k] -= pivotval * matrix[i][k];
                }
                R[j] -= pivotval * R[i];
            }
        }

        barrier (task_num);
    }

    if ((int)task_id != 0)
        pthread_exit(NULL);
    
}

/* Solve the equation. DO NOT EDIT HERE*/

void solveGauss(int nsize)
{
    int i, j;

    X[nsize-1] = R[nsize-1];
    for (i = nsize - 2; i >= 0; i --) {
        X[i] = R[i];
        for (j = nsize - 1; j > i; j--) {
            X[i] -= matrix[i][j] * X[j];
        }
    }

#ifdef DEBUG
    fprintf(stdout, "X = [");
    for (i = 0; i < nsize; i++) {
        fprintf(stdout, "%.6f ", X[i]);
    }
    fprintf(stdout, "];\n");
#endif
}


int main(int argc, char *argv[])
{

    int i;
    struct timeval start, finish;
    int nsize = 0;
    double error;
    task_num = (int)(argv[2][0] - '0');
    
    if (argc != 3) {
	fprintf(stderr, "usage: %s <matrixfile> <# of threads>\n", argv[0]);
	exit(-1);
    }

    nsize = initMatrix(argv[1]);
    initRHS(nsize);
    initResult(nsize);

    gettimeofday(&start, 0);

    int *id;
    struct stupidargs *args;
    pthread_attr_t attr;
    pthread_t *tid;
    // create threads
    id = (int *) malloc (sizeof (int) * task_num);
    args = (struct stupidargs *) malloc (sizeof (struct stupidargs) * task_num);
    tid = (pthread_t *) malloc (sizeof (pthread_t) * task_num);
    if (!id || !tid)
        {
        fprintf(stderr, "out of memory\n");
        exit(-1);
        }

    pthread_attr_init (&attr);
    pthread_attr_setscope (&attr, PTHREAD_SCOPE_SYSTEM);
    for (i = 1; i < task_num; i++) {
        id[i] = i;
        args[i].task_id = id[i];
        args[i].nsize = nsize;
        pthread_create (&tid[i], &attr, ComputeGauss, (void *)&args[i]);
    }
    
    id[0] = 0;
    args[0].task_id = id[0];
    args[0].nsize = nsize;

    ComputeGauss(&args[0]);
    for (i = 1; i<task_num; i++)
        pthread_join(tid[i], NULL);

    gettimeofday(&finish, 0);

    solveGauss(nsize);
    
    fprintf(stdout, "Time:  %f seconds\n", (finish.tv_sec - start.tv_sec) + (finish.tv_usec - start.tv_usec)*0.000001);

    error = 0.0;
    for (i = 0; i < nsize; i++) {
	double error__ = (X__[i]==0.0) ? 1.0 : fabs((X[i]-X__[i])/X__[i]);
	if (error < error__) {
	    error = error__;
	}
    }
    fprintf(stdout, "Error: %e\n", error);

    return 0;
}
