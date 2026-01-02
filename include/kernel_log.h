#ifndef KERNEL_LOG
#define KERNEL_LOG

void diag_init(void);
void diag_tick(void);
void diag_print(void);

void diag_file_created(void);
void diag_mem_used(int bytes);

#endif