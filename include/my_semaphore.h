#pragma once

extern void print_err(int flg, char *str);
extern int creat_or_get_sem(int nsems);
extern void init_sem(int semid, int semnum, int val);
extern void del_sem(int semid);
extern void p_sem(int semid, int semnum_buf[], size_t nsops);
extern void v_sem(int semid, int semnum_buf[], size_t nsops);
extern void p_sem_s(int semid, int semnum);
extern void v_sem_s(int semid, int semnum);
