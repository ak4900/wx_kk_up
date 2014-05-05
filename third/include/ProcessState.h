//  ProcessState.h

#ifndef PROCESS_STAT_H
#define PROCESS_STAT_H

#ifdef __cplusplus
extern "C" {
#endif

	typedef long long int64_t;
	typedef unsigned long long uint64_t;

	/// ��ȡ��ǰ���̵�cpuʹ���ʣ�����-1ʧ��
	int get_cpu_usage();

	/// ��ȡ��ǰ�����ڴ�������ڴ�ʹ����������-1ʧ�ܣ�0�ɹ�
	int get_memory_usage(uint64_t* mem, uint64_t* vmem);

	/// ��ȡ��ǰ�����ܹ�����д��IO�ֽ���������-1ʧ�ܣ�0�ɹ�
	int get_io_bytes(uint64_t* read_bytes, uint64_t* write_bytes);

	char* SizeFormat(uint64_t size, char *temp);

#ifdef __cplusplus
}
#endif

#endif/*PROCESS_STAT_H*/
