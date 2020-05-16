#include <linux/module.h>
#include <linux/init_task.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>	/* for copy_*_user */
#include <asm/perf_event.h>
#include "pesc.h"


#define MAX_SIZE	1024

#define ARRAYLEN 5005000

u64 canary_value[ARRAYLEN];
int canaryproc_pid[ARRAYLEN];
int canary_count = 0;
int testprocpid = -1;

static char msg[MAX_SIZE];

/*static int hello_proc_show(struct seq_file *m, void *v) {*/
/*seq_printf(m, "Hello proc!\n");*/
/*return 0;*/
/*}*/

/*static int hello_proc_open(struct inode *inode, struct  file *file) {*/
/*return single_open(file, hello_proc_show, NULL);*/
/*}*/


static void traversal_thread_group(struct task_struct * tsk){
	struct task_struct * curr_thread = NULL;
	unsigned long tg_offset = offsetof(struct task_struct, thread_group);

	curr_thread = (struct task_struct *) (((unsigned long)tsk->thread_group.next) - tg_offset);
	/*if (curr_thread == tsk){*/
	/*printk("Thread Group is empty!");*/
	/*return;*/
	/*}*/
	while (curr_thread != tsk){
		printk("THREAD PID=%d\tCANARY=%lx\n", curr_thread->pid, curr_thread->stack_canary);
		curr_thread = (struct task_struct *) (((unsigned long)curr_thread->thread_group.next) - tg_offset);
	}
}

static void traversal_process(void) {
	struct task_struct * tsk = NULL;

	traversal_thread_group(&init_task);
	for_each_process(tsk){
		printk("PROC PID=%d\tCANARY=%lx\n", tsk->pid, tsk->stack_canary);
		traversal_thread_group(tsk);
	}
}

static noinline void canary_test_stub(u64 * a1, u64 *a2) {
	printk("ADDR: %llx %llx\n", (u64)a1, (u64)a2);
}

//extern unsigned long __stack_chk_guard;
//void canary_test(void) {
//	u64 array[4] = {0x1111, 0x2222, 0x3333, 0x4444};
//	u64 berry[4] = {0xaaaa, 0xbbbb, 0xcccc, 0xdddd};

//	printk("PID %d CANARY %lx %lx\n", current->pid, __stack_chk_guard, current->stack_canary);

//	canary_test_stub(array, berry);
//}

static inline u32 armv8pmu_pmcr_read(void)
{
	return read_sysreg(pmcr_el0);
}

static inline void armv8pmu_pmcr_write(u32 val)
{
	val &= ARMV8_PMU_PMCR_MASK;
	isb();
	write_sysreg(val, pmcr_el0);
}

static void enable_cpu_counters(void* data) {
	/* 
	 * Disable any use space access by setting PMUSERENR_EL0.EN to 0:
	 * While at EL0, PMUSERENR_EL0 is always RO. Accesses to the other Performance
	 * Monitors registers at EL0 are trapped to EL1, unless overridden by one of
	 * PMUSERENR_EL0.{ER, CR, SW}.
	 */
	asm volatile("msr PMUSERENR_EL0, %0" : : "r"(0x0)); 

	/*
	 * Performance Monitors Count Enable Set register
	 * PMCNTENSET_EL0.C, bit [31], when 1, enables the cycle counter.
	 */
	asm volatile("msr PMCNTENSET_EL0, %0" :: "r" ((u32)(1<<31)));

	armv8pmu_pmcr_write(armv8pmu_pmcr_read() | (ARMV8_PMU_PMCR_E|ARMV8_PMU_PMCR_LC));   
	printk("CPU:%d PMCR%x\n", smp_processor_id(), armv8pmu_pmcr_read());
}


ssize_t proc_read(struct file *filp,char __user *buf,size_t count,loff_t *offp ) 
{
	/*sprintf(msg, "%s", "hello proc is read");*/

	int i;
	int tmpcount = 0;
	u64 tmpcanary[10];
	printk("proc_read:%s\n", msg);
	/*canary_test();*/

	traversal_process();
	for(i = 0; i < ARRAYLEN; i++)
	{

		if(canaryproc_pid[i] == 0) break;
		if(canaryproc_pid[i] == testprocpid)
		{
			tmpcanary[(tmpcount++)%10] = canary_value[i];
			if(tmpcount % 10 == 0)
				printk("%dCanary: %llx %llx %llx %llx %llx %llx %llx %llx %llx %llx", tmpcount,tmpcanary[0], tmpcanary[1], tmpcanary[2], tmpcanary[3], tmpcanary[4],tmpcanary[5],tmpcanary[6],tmpcanary[7],tmpcanary[8],tmpcanary[9]);
		}
	}
	printk("i: %d,  count:%d", i, tmpcount);
	return 0;
}

ssize_t proc_write(struct file *filp,const char *buf,size_t count,loff_t *offp)
{
	int ret = 0;
	if (count > MAX_SIZE){
		count =  MAX_SIZE;
	}

	/*ret = copy_from_user(msg,buf,count);*/
	ret = copy_from_user(msg,buf,count);
	printk("proc_write: copy_from_user return =%d\n", ret);
	msg[count] = '\0';
	sscanf(msg, "%d", &testprocpid);
	printk("testprocpid: %d", testprocpid);
	return count;
}

static const struct file_operations hello_proc_fops = {
	/*.owner = THIS_MODULE,*/
	/*.open = hello_proc_open,*/
	.read = proc_read,
	.write = proc_write,
	/*.llseek = seq_lseek,*/
	/*.release = single_release,*/
};

static int __init hello_proc_init(void) {
	proc_create("hello_proc", 0, NULL, &hello_proc_fops);

	// Enable PMU cycle counter
	on_each_cpu(enable_cpu_counters, NULL, 1); 
	/*enable_armv8pmu();*/
	return 0;
}

static void __exit hello_proc_exit(void) {
	remove_proc_entry("hello_proc", NULL);
}

MODULE_LICENSE("GPL");
module_init(hello_proc_init);
module_exit(hello_proc_exit);
