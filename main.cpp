#include"headers.h"
void CreateSignalProcess(struct Data *data)
{
	data->count = 0;
#if PLATFORM == PLATFORM_WIN
	HANDLE Event = CreateEvent(NULL, FALSE, FALSE, "Event");

	if (Event != NULL)
	{
		Sleep(1000);
		SetEvent(Event);
	}
	else
		printf("CreateEvent() error: %d\n", GetLastError());
#else
	data->key = ftok("/home/draokweil/lab2", 0);
	data->semid = semget(data->key, 1, 0666 | IPC_CREAT);
	semctl(data->semid, 0, SETVAL, (int)0);
	data->mybuff.sem_num = 0;

	data->mybuff1.sem_num = 0;
	data->mybuff1.sem_op = 1;
#endif
}

void CreateNewProcess(char *path, struct Data *data, struct Stack **stack)
{
#if PLATFORM == PLATFORM_WIN
	char *numberCh;
	numberCh = (char*)malloc(10);
	int c = data->count;
	data->count = ++c;
	sprintf(numberCh, "%d", data->count);

	char *str = NULL;
	str = (char*)malloc(sizeof(char) * 100);
	strcpy(str, path);

	strcat(str, " ");
	strcat(str, numberCh);

	STARTUPINFO stInfo;
	PROCESS_INFORMATION prInfo;

	ZeroMemory(&stInfo, sizeof(STARTUPINFO));
	stInfo.cb = sizeof(STARTUPINFO);

	if (!CreateProcess(NULL, str, NULL, NULL, FALSE, 0, NULL, NULL, &stInfo, &prInfo))
	{
		data->count = --c;
		exit(0);
	}


	struct Data data_pr;
	data_pr.prInfo = prInfo;

	push(*&stack, data_pr);
#else
	switch (data->pid = fork())
	{
	case -1:
		perror("fork error...");
		exit(1);
	case 0:
	{
		int c = 0;
		c = data->count;
		data->count = ++c;

		int i = 1, k = data->count / 10;

		for (i = 1; k != 0; i++)
			k = k / 10;

		char *params;
		params = (char *)malloc((i + 1) * sizeof(char));
		params[i] = '\0';

		for (int num = i, number = data->count; num > 0; num--, number /= 10)
			params[num - 1] = number % 10 + '0';

		char *cmd[3] = { "/home/draokweil/lab2", params, 0 };
		execv("/home/draokweil/lab2", cmd);
	}
	default:
	{
		int m = data->count;
		data->count = ++m;
		push(&*stack, *data);
	}
	break;
	}
#endif
}

void CloseLastProcess(struct Stack **stack, struct Data *data)
{
#if PLATFORM == PLATFORM_WIN
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");

	if (Event == NULL)
		exit(0);

	WaitForSingleObject(Event, INFINITE);

	TerminateProcess((*stack)->hand, 0);
	pop(*&stack);
	int c = data->count;
	data->count = --c;

	SetEvent(Event);
#else
	semop(data->semid, &(data->mybuff), 1);
	kill((*stack)->pid, SIGKILL);
	pop(&*stack);
	int c = data->count;
	data->count = --c;
#endif
}

void CloseAllProcesses(struct Stack **stack, struct Data *data)
{
#if PLATFORM == PLATFORM_WIN
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");

	if (Event == NULL)
		exit(0);

	WaitForSingleObject(Event, INFINITE);

	while (size(*stack) != 0)
	{
		TerminateProcess((*stack)->hand, 0);
		pop(*&stack);
		int c = data->count;
		data->count = --c;
	}
#else
	semop(data->semid, &(data->mybuff), 1);
	semop(data->semid, &(data->mybuff1), 1);

	while (size(*stack) != 0)
	{
		kill((*stack)->pid, SIGKILL);
		pop(&*stack);
		int c = data->count;
		data->count = --c;
	}
	return;
#endif
}

void PrintProcesses(char **argv, struct Data *data)
{
#if PLATFORM == PLATFORM_WIN
	HANDLE Event = OpenEvent(EVENT_ALL_ACCESS, FALSE, "Event");
	while (1)
	{
		WaitForSingleObject(Event, INFINITE);

		printf("Number of process %s\n\n", argv[1]);

		SetEvent(Event);
		Sleep(1000);
	}
#else
	while (1)
	{
		semop(data->semid, &(data->mybuff), 1);
		semop(data->semid, &(data->mybuff1), 1);
		usleep(100000);
		printf("Process number %s\n", argv[1]);
		data->mybuff1.sem_op = -1;
		semop(data->semid, &(data->mybuff1), 1);
		data->mybuff1.sem_op = 1;
	}
#endif
}



int main(int argc, char *argv[])
{
	struct Stack *stack = NULL;
	struct Data data;

	if (argc == 1)
	{
		CreateSignalProcess(&data);

		while (1)
		{
			switch (_getch())
			{
			case '+':
				CreateNewProcess(argv[0], &data, &stack);
				break;
			case '-':
				if (size(stack) != 0)
					CloseLastProcess(&stack, &data);
				break;
			case 'q':
				CloseAllProcesses(&stack, &data);
				return 0;
				break;
			}
		}
	}
	else
		PrintProcesses(argv, &data);

	return 0;
}
