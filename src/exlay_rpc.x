program EXLAYPROG {
	version EXLAYVERS {
		int EX_CREATE_STACK(unsigned int) = 1;
		int EX_SET_BINDING(int, unsigned int, string, string, string) = 2;
		int EX_BIND_STACK(int) = 3;
		int EX_SET_REMOTE(int, int, string) = 4;
		int EX_DIAL_STACK(int, uint32_t) = 5;
		int EX_LISTEN_STACK(int) = 6;
		int EX_CLOSE_STACK(int) = 7;
	} = 1;
} = 0x20001001;
