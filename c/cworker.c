/*
 * Gearman C API Work Demo
 * Copyright (C) 2014 Jiang Bian, borderj@gmail.com 
 * All rights reserved.
 * 
 * Use and distribution licensed under the BSD license.  See
 * the COPYING file in this directory for full text.
 */ 

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgearman/gearman.h>


void *doing_image_work(gearman_job_st *job, void *context, size_t *result_size, gearman_return_t *ret_ptr){

    int workloadsize;
    char *retstr = "doing_image_work return";

    /* contect is unused */
    context = context;
    workloadsize = gearman_job_workload_size(job); //how big is the workload?

    //Copy the workload into a usable string
    char *workload = malloc(workloadsize + 1);
    memset(workload, '\0', workloadsize + 1);

    strncpy(workload, (const char*)gearman_job_workload(job), workloadsize);

    //Do your work here
    printf("%s\n", workload);

    //Not passing any result information back at the moment
    *result_size = strlen(retstr);

    /* Should do some checks to monitor for fails
     * all we're doing here is just printing the workload
     */
    *ret_ptr = GEARMAN_SUCCESS;
    free(workload);

    return strdup(retstr);
}

static void *reverse(gearman_job_st *job, void *context,
                     size_t *result_size, gearman_return_t *ret_ptr)
{

    int workloadsize;
    char *retstr = "doing_reverse_work return";

    /* contect is unused */
    context = context;
    workloadsize = gearman_job_workload_size(job); //how big is the workload?

    //Copy the workload into a usable string
    const char *workload= (const char *)gearman_job_workload(job);

    printf("%s\n", workload);

    char *result = malloc(workloadsize + 1);
    memset(result, '\0', workloadsize + 1);

    for (size_t y= 0, x= workloadsize; x; x--, y++) {
        result[y]= ((uint8_t *)workload)[x - 1];
    }

    printf("%s\n", result);

    for (size_t y= 0, x= workloadsize; x; x--, y++) {
        if (gearman_failed(gearman_job_send_data(job, &result[y], 1))) {
            printf("Job send data error\n");
            return NULL;
        }

        if (gearman_failed(gearman_job_send_status(job, (uint32_t)y, (uint32_t)workloadsize)))
        {
            printf("Job send status error\n");
            return NULL;
        }
        sleep(1);
    }

    //gearman_job_handle(job);

    //gearman_job_unique(job);

    //Not passing any result information back at the moment
    *result_size = strlen(retstr);

    /* Should do some checks to monitor for fails
     * all we're doing here is just printing the workload
     */
    *ret_ptr = GEARMAN_SUCCESS;

    return strdup(retstr);
}


int main(void){
    //Create the worker
    gearman_worker_st *worker = gearman_worker_create(NULL);

    //Connect the worker to the server and chech it worked
    gearman_return_t ret= gearman_worker_add_server(worker, "localhost", 4730);
    if (gearman_failed(ret)){
        printf("Connection Server Error %d\n", ret);
        return EXIT_FAILURE;
    }

    //register function, (worker, job_queue_name, timeout, function_to_do_work, context)
    printf("Register 'image' function\n");
    ret = gearman_worker_add_function(worker,"image",0, doing_image_work,NULL);
    if(gearman_failed(ret)){
        return EXIT_FAILURE;
    }

    printf("Register 'reverse' function\n");
    ret = gearman_worker_add_function(worker,"reverse",0, reverse, NULL);
    if(gearman_failed(ret)){
        return EXIT_FAILURE;
    }

    //Infinite loop to keep watching
    while(1){
        //Does the work
        ret = gearman_worker_work(worker);
        if ( ret != GEARMAN_SUCCESS ) {
            fprintf(stderr,"worker error: %s\n", gearman_worker_error(worker));
            gearman_job_free_all(worker);
            gearman_worker_free(worker);
            /* sleep on error to avoid cpu intensive infinite loops */
            sleep(5);
        }
    }
    return EXIT_SUCCESS;
}
