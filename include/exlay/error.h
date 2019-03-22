#ifndef __EXLAY_ERROR__
#define __EXLAY_ERROR__

#define print_err(CODE) \
	do { \
		switch ((-(CODE))) {\
			case CODE_DUP: \
				fprintf(stderr, "Dupulicated protocol.\n"); \
				break;\
			case CODE_NMEM: \
				fprintf(stderr, "No memory left.\n");\
				break;\
			case CODE_NEXIST: \
			case CODE_NFND: \
				fprintf(stderr, "No such protocol exists.\n");\
				break;\
			case CODE_NEXEP: \
				fprintf(stderr, "No such exlay endpoint.\n");\
				break;\
			case CODE_NLYR: \
				fprintf(stderr, "Invalid layer number.\n");\
				break;\
			case CODE_NPRTLIB: \
				fprintf(stderr, "No such protocol library.\n");\
				break;\
			case CODE_EDLOPEN: \
				fprintf(stderr, "Failed to load protocol library.\n");\
				break;\
			case CODE_EDLSHM: \
				fprintf(stderr, "Invalid protocol library.\n");\
				break;\
			case CODE_EDIFFBS: \
				fprintf(stderr, "Different binding sizes are specified.\n");\
				break;\
			case CODE_EBIND: \
				fprintf(stderr, "All bindings of this exlay stack are already used.\n");\
				break;\
			case CODE_OK:\
				break;\
		} } while (0)

#endif
