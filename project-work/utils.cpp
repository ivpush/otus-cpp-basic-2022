// 
// remove blanks from src string
//    src - initial/source string
//    len - max length

void remove_blanks (
	char * src
  , int len
)
{
	char * dest;

    for (dest  = src; *src != 0 && *src != '\n' && *src != '\r' && len > 0; len--, src++)
	{
        if ((*src == ' ')
		 || (*src == '\t')) 
			continue;
			
        *dest = *src;
        dest++;
    }

    *dest = 0;
}

// 
// remove blanks from src string
//    src - initial/source string

void remove_blanks (
	char * src
)
{
	char * dest;

    for (dest = src; *src != 0 && *src != '\n' && *src != '\r'; src++)
	{
        if ((*src == ' ')
		 || (*src == '\t')) 
			continue;
			
        *dest = *src;
        dest++;
    }

    *dest = 0;
}

// ------------------------------------
// cp2utf
// convert source string from cp-1251 into destination string in utf-8
//     srcStr  - initial/source string
//     destStr - destination string, should be allocated outside by caller
//
void cp2utf ( 
	unsigned char * srcStr
  , unsigned char * destStr 
) 
{
	static const long utf[ 256 ] = {
//      	  X0    X1    X2    X3    X4    X5    X6    X7    X8    X9    Xa    Xb    Xc    Xd    Xe    Xf		
/* 0X */	   0,    1,    2,    3,    4,    5,    6,    7,    8,    9,   10,   11,   12,   13,   14,   15
/* 1X */  ,   16,   17,   18,   19,   20,   21,   22,   23,   24,   25,   26,   27,   28,   29,   30,   31
/* 2X */  ,   32,   33,   34,   35,   36,   37,   38,   39,   40,   41,   42,   43,   44,   45,   46,   47
/* 3X */  ,   48,   49,   50,   51,   52,   53,   54,   55,   56,   57,   58,   59,   60,   61,   62,   63
/* 4X */  ,   64,   65,   66,   67,   68,   69,   70,   71,   72,   73,   74,   75,   76,   77,   78,   79
/* 5X */  ,   80,   81,   82,   83,   84,   85,   86,   87,   88,   89,   90,   91,   92,   93,   94,   95
/* 6X */  ,   96,   97,   98,   99,  100,  101,  102,  103,  104,  105,  106,  107,  108,  109,  110,  111
/* 7X */  ,  112,  113,  114,  115,  116,  117,  118,  119,  120,  121,  122,  123,  124,  125,  126,  127
/* 8X */  , 1026, 1027, 8218, 1107, 8222, 8230, 8224, 8225, 8364, 8240, 1033, 8249, 1034, 1036, 1035, 1039
/* 9X */  , 1106, 8216, 8217, 8220, 8221, 8226, 8211, 8212, 8250, 8482, 1113, 8250, 1114, 1116, 1115, 1119
/* aX */  ,  160, 1038, 1118, 1032,  164, 1168,  166,  167, 1025,  169, 1028,  171,  172,  173,  174, 1031
/* bX */  ,  176,  177, 1030, 1110, 1169,  181,  182,  183, 1105, 8470, 1108,  187, 1112, 1029, 1109, 1111
/* cX */  , 1040, 1041, 1042, 1043, 1044, 1045, 1046, 1047, 1048, 1049, 1050, 1051, 1052, 1053, 1054, 1055
/* dX */  , 1056, 1057, 1058, 1059, 1060, 1061, 1062, 1063, 1064, 1065, 1066, 1067, 1068, 1069, 1070, 1071
/* eX */  , 1072, 1073, 1074, 1075, 1076, 1077, 1078, 1079, 1080, 1081, 1082, 1083, 1084, 1085, 1086, 1087
/* fX */  , 1088, 1089, 1090, 1091, 1092, 1093, 1094, 1095, 1096, 1097, 1098, 1099, 1100, 1101, 1102, 1103
	};

	int j = 0;

	while (*srcStr) 
	{
		long c = utf[ *srcStr ];
		if( c < 0x80 ) {
			destStr[ j++ ] = c;
		}
		else if( c < 0x800 ) {
			destStr[ j++ ] = c >> 6 | 0xc0;
			destStr[ j++ ] = c & 0x3f | 0x80;
		} 
		else if( c < 0x10000 ) {
			destStr[ j++ ] = c >> 12 | 0xe0;
			destStr[ j++ ] = c >> 6 & 0x3f | 0x80;
			destStr[ j++ ] = c & 0x3f | 0x80;
		} 
		else if( c < 0x200000 ) {
			destStr[ j++ ] = c >> 18 | 0xf0;
			destStr[ j++ ] = c >> 12 & 0x3f | 0x80;
			destStr[ j++ ] = c >> 6 & 0x3f | 0x80;
			destStr[ j++ ] = c & 0x3f | 0x80;
		}
		srcStr++;
	}
	destStr[ j ] = '\0';
}

