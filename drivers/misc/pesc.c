#include <linux/module.h>
#include <linux/init_task.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>	/* for copy_*_user */
#include <asm/perf_event.h>

#define MAX_SIZE	1024

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

static int __init pesc_module_init(void) {

	on_each_cpu(enable_cpu_counters, NULL, 1); 
	return 0;
}

static void __exit pesc_module_exit(void) {
    return;
}

MODULE_LICENSE("GPL");
module_init(pesc_module_init);
module_exit(pesc_module_exit);
