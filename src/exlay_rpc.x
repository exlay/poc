program EXLAYPROG {
	version EXLAYVERS {
		int EX_CREATE_STACK(unsigned int) = 1;
		int EX_SET_BINDING(int, unsigned int, string, string, string) = 2;
		int EX_BIND_STACK(int) = 3;
		int EX_SET_REMOTE(int, int, string) = 4;
		int EX_DIAL_STACK(int) = 5;
		int EX_LISTEN_STACK(int) = 6;
		int EX_CLOSE_STACK(int) = 7;
		string EXLAY_LIST(void) = 8;
		int EXLAY_ADD(string, string) = 9;
		int EXLAY_DEL(string) = 10;
		string EXLAY_INFO(string) = 11;
		int EXLAY_UPDATE(string, string) = 12;
	} = 1;
} = 0x20001001;
