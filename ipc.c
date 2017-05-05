void setup_counters(int pid[], int cpuid[])
{
	struct perf_event_attr attr[4];
	int i;
	for ( i = 0; i < 4; i++)
		memset(&attr[i], 0, sizeof(attr[i]));
#ifdef USERSPACE_ONLY
	attr[0].exclude_kernel = 1;
	attr[0].exclude_hv = 1;
	attr[0].exclude_idle = 1;

	attr[1].exclude_kernel = 1;
	attr[1].exclude_hv = 1;
	attr[1].exclude_idle = 1;

	attr[2].exclude_kernel = 1;
	attr[2].exclude_hv = 1;
	attr[2].exclude_idle = 1;

	attr[3].exclude_kernel = 1;
	attr[3].exclude_hv = 1;
	attr[3].exclude_idle = 1;
#endif
	for(i = 0; i < 4; i ++)
	{
		attr[i].disabled = 1;
		attr[i].type = PERF_TYPE_HARDWARE;
		attr[i].config = PERF_COUNT_HW_CPU_CYCLES;
		cycles_fd[i] = sys_perf_event_open(&attr[i], pid[i], -1, -1, 0);
		if(cycles_fd[i] < 0)
		{
			perror("sys_perf_event_open");
			exit(1);
		}

		attr[i].disabled = 0;
		attr[i].type = PERF_TYPE_HARDWARE;
		attr[i].config = PERF_COUNT_HW_INSTRUCTIONS;
		instructions_fd[i] = sys_perf_event_open(&attr[i], pid[i], -1, cycles_fd[i], 0);
		if(instructions_fd[i] < 0)
		{
			perror("sys_perf_event_open");
			exit(1);
		}
	}
}
void start_counters(void)
{
	for(int i = 0; i < 4; i ++)
		ioctl(cycles_fd[i], PERF_EVENT_IOC_ENABLE);
}
void stop_counters(void)
{
	for(int i = 0; i < 4; i ++)
		ioctl(cycles_fd[i], PERF_EVENT_IOC_DISABLE);
}
