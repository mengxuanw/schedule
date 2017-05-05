//获得各进程pid
void GetPid()
{
	char buffer[100] = {0};
	FILE *fp = popen("pidof mcf_base.amd64-m64-gcc42-nn", "r");//打开管道，执行shell 命令
	fgets(buffer, 10, fp);
	pid[0] = atoi(buffer);
	pclose(fp);
	char perf0[100] = "gnome-terminal -x bash -c \"perf stat -o stat0.txt -p ";
	strcat(perf0,buffer);
	strcat(perf0,"\"");
	FILE *fp1 = popen(perf0,"r");
	pclose(fp1);

	fp = popen("pidof cactusADM_base.amd64-m64-gcc42-nn", "r");//打开管道，执行shell 命令
	fgets(buffer, 10, fp);
	pid[1] = atoi(buffer);
	pclose(fp);
	char perf1[100] = "gnome-terminal -x bash -c \"perf stat -o stat1.txt -p ";
	strcat(perf1,buffer);
	strcat(perf1,"\"");
	fp1 = popen(perf1,"r");
	pclose(fp1);

	fp = popen("pidof namd_base.amd64-m64-gcc42-nn", "r");//打开管道，执行shell 命令
	fgets(buffer, 10, fp);
	pid[2] = atoi(buffer);
	pclose(fp);
	char perf2[100] = "gnome-terminal -x bash -c \"perf stat -o stat2.txt -p ";
	strcat(perf2,buffer);
	strcat(perf2,"\"");
	fp1 = popen(perf2,"r");
	pclose(fp1);

	fp = popen("pidof sjeng_base.amd64-m64-gcc42-nn", "r");//打开管道，执行shell 命令
	fgets(buffer, 10, fp);
	pid[3] = atoi(buffer);
	pclose(fp);
	char perf3[100] = "gnome-terminal -x bash -c \"perf stat -o stat3.txt -p ";
	strcat(perf3,buffer);
	strcat(perf3,"\"");
	fp1 = popen(perf3,"r");
	pclose(fp1);
}

//根据pid得到IPC
float GetIPC(int pid[], int cpuid[])
{
	int i;
	float ipc[4] = {0};
	float IPC = 0;
	unsigned long long cycles[4], instructions[4];
	size_t res[4];
	setup_counters(pid, cpuid);
	start_counters();
	/*do something  在这段时间内的IPC,应该更准确一点，可以使用睡眠函数*/
	usleep(100000);
	stop_counters();
	for(i = 0; i < 4; i ++)
	{
		res[i] = read(cycles_fd[i], &cycles[i], sizeof(unsigned long long));
		assert(res[i] == sizeof(unsigned long long));

		res[i] = read(instructions_fd[i], &instructions[i], sizeof(unsigned long long));
		assert(res[i] == sizeof(unsigned long long));

		ipc[i] = (float)instructions[i]/cycles[i];
		IPC += ipc[i];
	}
	return IPC;
}

//按照个体基因绑定进程和cpu
void Bind(int g[])
{
	unsigned long mask = 0;
	for( int i = 0; i < 4; i ++)
	{
		if(g[i] == 0)
			mask = 1;
		else if(g[i] == 1)
			mask = 2;
		else if(g[i] == 2)
			mask = 4;
		else if(g[i] == 3)
			mask = 8;
		if(sched_setaffinity(pid[i], sizeof(mask), &mask) < 0)
		{
			perror("sched_setaffinity");
		}
	}
}

void test()
{
	//总共有24种方案，实在是没有必要用遗传算法，迭代24次找到最优方案
	//生成24中方案
	int P[24][4];
	int cur = 0;
	GetPid();
	int best = 0;
	float ipc = 0;
	float bestipc = 0;
	for(int i = 0; i < 24; i ++)
	{
		plan(P[i],cur);//生成一种方案，例如0 1 2 3
		Bind(P[i]);//按照生成的方案绑定进程和内核
		ipc =  GetIPC(pid,P[i]);//获得总IPC
		if(ipc > bestipc)
		{
			bestipc = ipc;
			best = i;
		}
	}
	//选择最好的调度方案
	Bind(P[best]);
}
