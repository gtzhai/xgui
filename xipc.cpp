#include "xgui.h"

#ifdef _WIN64 || _WIN32
int ShareMemGet(HANDLE  *psec, int flag, int fsize, LPCWSTR fname)
{
	*psec = ::CreateFileMapping((HANDLE)0xFFFFFFFF,
				(LPSECURITY_ATTRIBUTES)NULL,
				flag,
				0,
				fsize,
				fname);
	return (*psec != NULL);
}

int ShareMemOpen(HANDLE  *psec, int flag, LPCWSTR fname)
{
	*psec = OpenFileMapping(flag,
				FALSE,
				fname);

	return (*psec != NULL);
}

int ShareMemAttach(void** ppaddr, HANDLE  sec, int flag)
{
	*ppaddr = ::MapViewOfFile(sec,
		flag,
		0,
		0,
		0);
	return (*ppaddr != NULL);
}

void ShareMemDettach(void* paddr)
{
	UnmapViewOfFile(paddr);
}

void ShareMemDelete(HANDLE  sec)
{
	CloseHandle(sec);
}

LRESULT xSendMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return SendMessage(wnd, message, wParam, lParam);
}

#else
//OS_UNIX
#define IPC_PATH "/"
int make_shmem_id(char* fname)
{
	char* p = (char*)fname;
	uint nr = 1;
	uint nr2 = 4;
	while(*p)
	{
		nr ^= ( ( (nr & 63) + nr2 ) * (*p)) + (nr << 8);
		nr2+= (uint)(*p);

		p++;
	}
	return nr;
}

//
//int shm_check_exist(int fsize, char* fname)
//{
//	uint_t idnr;
//	if(shmids(0, 0, &idnr) == 0)
//	{
//		int* ids = new int[idnr];
//		if(shmids(ids, idnr, &idnr) == 0)
//		{
//			int n;
//			for(n=0;n < idnr; n ++)
//			{
//				shmid_ds ds;
//				if(shmctl(ids[n], IPC_STAT, &ds) == 0)
//				{
//					if(ds)
//				}
//			}
//		}
//	}
//}

int ShareMemGet(HANDLE *psec, int flag, int fsize, char* fname, int nid)
{
    #if 0
	if(fsize == 0)//size 0 is not allowed
		fsize = sizeof(int);

	key_t key = ftok(IPC_PATH, make_shmem_id(fname)) + nid;
	if(ShareMemOpen(psec, flag, fname, nid) != 1)
	{
		*psec = shmget(key, fsize, IPC_CREAT | flag);

		if(*psec == -1)
		{
			printf("shmget failed! - id:%d name: %s, key: %x, size: %d\n", *psec, fname, key, fsize);
			*psec = 0;
			return 0;
		}
		else
			printf("shmget - id:%d name: %s, key: %x, size: %d\n", *psec, fname, key, fsize);
	}
	else
		printf("shmget exist already - id:%d name: %s, key: %x, size: %d\n", *psec, fname, key, fsize);

	struct shmid_ds ds;
	if(shmctl(*psec, IPC_STAT, &ds) == 0)
	{
		if(ds.shm_segsz < fsize)//need enlarge size
		{
			if(ds.shm_nattch == 0 && shmctl(*psec, IPC_RMID, 0) == 0)
			{
				return ShareMemGet(psec, flag, fsize, fname, nid);
			}
			else
			{
				printf("shmget can not enlarge size - id:%d name: %s, key: %x, size: %d\n", *psec, fname, key, fsize);
				return 0;
			}
		}
	}
	else
	{
		printf("shmget can not stat - id:%d name: %s, key: %x, size: %d\n", *psec, fname, key, fsize);
		return 0;
	}

	return 1;
	#endif
	return 0;
}

int ShareMemOpen(HANDLE *psec, int flag, char* fname, int nid)
{
    #if 0
	key_t key = ftok(IPC_PATH, make_shmem_id(fname)) + nid;

	*psec = shmget(key, 0, flag);

	if(*psec == -1)
	{
		*psec = 0;
		return 0;
	}

	return 1;
	#endif
	return 0;
}

int ShareMemAttach(void** ppaddr, HANDLE sec, int flag)
{
    #if 0
	*ppaddr = shmat(sec, 0, flag);

	return (*ppaddr != NULL);
	#endif
	return 0;
}

void ShareMemDettach(void* paddr)
{
    #if 0
	shmdt((char*)paddr);
	#endif
}

void ShareMemDelete(HANDLE sec)
{
    #if 0
	struct shmid_ds ds;
	if(shmctl(sec, IPC_STAT, &ds) == 0 &&
		ds.shm_nattch == 0)
	{
		shmctl(sec, IPC_RMID, 0);
	}
	#endif

}

LRESULT xSendMessage(HWND wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//WJFTODO
	return -1;
}

#endif
