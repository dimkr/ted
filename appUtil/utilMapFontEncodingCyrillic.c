/************************************************************************/
/*									*/
/*  Map Font Encodings: Cyrillic: WIN1251 <-> ISO 8859-5 <-> KOI8R	*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

const unsigned char docKOI8R_to_WIN1251[256]=
    {
    0x00,			/*  			  0	*/
    0x01,			/*  			  1	*/
    0x02,			/*  			  2	*/
    0x03,			/*  			  3	*/
    0x04,			/*  			  4	*/
    0x05,			/*  			  5	*/
    0x06,			/*  			  6	*/
    0x07,			/*  			  7	*/
    0x08,			/*  			  8	*/
    0x09,			/*  			  9	*/
    0x0A,			/*  			 10	*/
    0x0B,			/*  			 11	*/
    0x0C,			/*  			 12	*/
    0x0D,			/*  			 13	*/
    0x0E,			/*  			 14	*/
    0x0F,			/*  			 15	*/
    0x10,			/*  			 16	*/
    0x11,			/*  			 17	*/
    0x12,			/*  			 18	*/
    0x13,			/*  			 19	*/
    0x14,			/*  			 20	*/
    0x15,			/*  			 21	*/
    0x16,			/*  			 22	*/
    0x17,			/*  			 23	*/
    0x18,			/*  			 24	*/
    0x19,			/*  			 25	*/
    0x1A,			/*  			 26	*/
    0x1B,			/*  			 27	*/
    0x1C,			/*  			 28	*/
    0x1D,			/*  			 29	*/
    0x1E,			/*  			 30	*/
    0x1F,			/*  			 31	*/
    WIN1251_space,		/*  KOI8R_space		 32	*/
    WIN1251_exclam,		/*  KOI8R_exclam	 33	*/
    WIN1251_quotedbl,		/*  KOI8R_quotedbl	 34	*/
    WIN1251_numbersign,		/*  KOI8R_numbersign	 35	*/
    WIN1251_dollar,		/*  KOI8R_dollar	 36	*/
    WIN1251_percent,		/*  KOI8R_percent	 37	*/
    WIN1251_ampersand,		/*  KOI8R_ampersand	 38	*/
    WIN1251_quotesingle,	/*  KOI8R_quotesingle	 39	*/
    WIN1251_parenleft,		/*  KOI8R_parenleft	 40	*/
    WIN1251_parenright,		/*  KOI8R_parenright	 41	*/
    WIN1251_asterisk,		/*  KOI8R_asterisk	 42	*/
    WIN1251_plus,		/*  KOI8R_plus		 43	*/
    WIN1251_comma,		/*  KOI8R_comma		 44	*/
    WIN1251_minus,		/*  KOI8R_minus		 45	*/
    WIN1251_period,		/*  KOI8R_period	 46	*/
    WIN1251_slash,		/*  KOI8R_slash		 47	*/
    WIN1251_zero,		/*  KOI8R_zero		 48	*/
    WIN1251_one,		/*  KOI8R_one		 49	*/
    WIN1251_two,		/*  KOI8R_two		 50	*/
    WIN1251_three,		/*  KOI8R_three		 51	*/
    WIN1251_four,		/*  KOI8R_four		 52	*/
    WIN1251_five,		/*  KOI8R_five		 53	*/
    WIN1251_six,		/*  KOI8R_six		 54	*/
    WIN1251_seven,		/*  KOI8R_seven		 55	*/
    WIN1251_eight,		/*  KOI8R_eight		 56	*/
    WIN1251_nine,		/*  KOI8R_nine		 57	*/
    WIN1251_colon,		/*  KOI8R_colon		 58	*/
    WIN1251_semicolon,		/*  KOI8R_semicolon	 59	*/
    WIN1251_less,		/*  KOI8R_less		 60	*/
    WIN1251_equal,		/*  KOI8R_equal		 61	*/
    WIN1251_greater,		/*  KOI8R_greater	 62	*/
    WIN1251_question,		/*  KOI8R_question	 63	*/
    WIN1251_at,			/*  KOI8R_at		 64	*/
    WIN1251_A,			/*  KOI8R_A		 65	*/
    WIN1251_B,			/*  KOI8R_B		 66	*/
    WIN1251_C,			/*  KOI8R_C		 67	*/
    WIN1251_D,			/*  KOI8R_D		 68	*/
    WIN1251_E,			/*  KOI8R_E		 69	*/
    WIN1251_F,			/*  KOI8R_F		 70	*/
    WIN1251_G,			/*  KOI8R_G		 71	*/
    WIN1251_H,			/*  KOI8R_H		 72	*/
    WIN1251_I,			/*  KOI8R_I		 73	*/
    WIN1251_J,			/*  KOI8R_J		 74	*/
    WIN1251_K,			/*  KOI8R_K		 75	*/
    WIN1251_L,			/*  KOI8R_L		 76	*/
    WIN1251_M,			/*  KOI8R_M		 77	*/
    WIN1251_N,			/*  KOI8R_N		 78	*/
    WIN1251_O,			/*  KOI8R_O		 79	*/
    WIN1251_P,			/*  KOI8R_P		 80	*/
    WIN1251_Q,			/*  KOI8R_Q		 81	*/
    WIN1251_R,			/*  KOI8R_R		 82	*/
    WIN1251_S,			/*  KOI8R_S		 83	*/
    WIN1251_T,			/*  KOI8R_T		 84	*/
    WIN1251_U,			/*  KOI8R_U		 85	*/
    WIN1251_V,			/*  KOI8R_V		 86	*/
    WIN1251_W,			/*  KOI8R_W		 87	*/
    WIN1251_X,			/*  KOI8R_X		 88	*/
    WIN1251_Y,			/*  KOI8R_Y		 89	*/
    WIN1251_Z,			/*  KOI8R_Z		 90	*/
    WIN1251_bracketleft,	/*  KOI8R_bracketleft	 91	*/
    WIN1251_backslash,		/*  KOI8R_backslash	 92	*/
    WIN1251_bracketright,	/*  KOI8R_bracketright	 93	*/
    WIN1251_asciicircum,	/*  KOI8R_asciicircum	 94	*/
    WIN1251_underscore,		/*  KOI8R_underscore	 95	*/
    WIN1251_quoteleft,		/*  KOI8R_quoteleft	 96	*/
    WIN1251_a,			/*  KOI8R_a		 97	*/
    WIN1251_b,			/*  KOI8R_b		 98	*/
    WIN1251_c,			/*  KOI8R_c		 99	*/
    WIN1251_d,			/*  KOI8R_d		100	*/
    WIN1251_e,			/*  KOI8R_e		101	*/
    WIN1251_f,			/*  KOI8R_f		102	*/
    WIN1251_g,			/*  KOI8R_g		103	*/
    WIN1251_h,			/*  KOI8R_h		104	*/
    WIN1251_i,			/*  KOI8R_i		105	*/
    WIN1251_j,			/*  KOI8R_j		106	*/
    WIN1251_k,			/*  KOI8R_k		107	*/
    WIN1251_l,			/*  KOI8R_l		108	*/
    WIN1251_m,			/*  KOI8R_m		109	*/
    WIN1251_n,			/*  KOI8R_n		110	*/
    WIN1251_o,			/*  KOI8R_o		111	*/
    WIN1251_p,			/*  KOI8R_p		112	*/
    WIN1251_q,			/*  KOI8R_q		113	*/
    WIN1251_r,			/*  KOI8R_r		114	*/
    WIN1251_s,			/*  KOI8R_s		115	*/
    WIN1251_t,			/*  KOI8R_t		116	*/
    WIN1251_u,			/*  KOI8R_u		117	*/
    WIN1251_v,			/*  KOI8R_v		118	*/
    WIN1251_w,			/*  KOI8R_w		119	*/
    WIN1251_x,			/*  KOI8R_x		120	*/
    WIN1251_y,			/*  KOI8R_y		121	*/
    WIN1251_z,			/*  KOI8R_z		122	*/
    WIN1251_braceleft,		/*  KOI8R_braceleft	123	*/
    WIN1251_bar,		/*  KOI8R_bar		124	*/
    WIN1251_braceright,		/*  KOI8R_braceright	125	*/
    WIN1251_asciitilde,		/*  KOI8R_asciitilde	126	*/
    WIN1251__0x7F,		/*  KOI8R__0x7F		127	*/
    WIN1251_currency,		/*  KOI8R_SF100000	128	*//**/
    WIN1251_currency,		/*  KOI8R_SF110000	129	*//**/
    WIN1251_currency,		/*  KOI8R_SF010000	130	*//**/
    WIN1251_currency,		/*  KOI8R_SF030000	131	*//**/
    WIN1251_currency,		/*  KOI8R_SF020000	132	*//**/
    WIN1251_currency,		/*  KOI8R_SF040000	133	*//**/
    WIN1251_currency,		/*  KOI8R_SF080000	134	*//**/
    WIN1251_currency,		/*  KOI8R_SF090000	135	*//**/
    WIN1251_currency,		/*  KOI8R_SF060000	136	*//**/
    WIN1251_currency,		/*  KOI8R_SF070000	137	*//**/
    WIN1251_currency,		/*  KOI8R_SF050000	138	*//**/
    WIN1251_currency,		/*  KOI8R_upblock	139	*//**/
    WIN1251_currency,		/*  KOI8R_dnblock	140	*//**/
    WIN1251_currency,		/*  KOI8R_block		141	*//**/
    WIN1251_currency,		/*  KOI8R_lfblock	142	*//**/
    WIN1251_currency,		/*  KOI8R_rtblock	143	*//**/
    WIN1251_currency,		/*  KOI8R_ltshade	144	*//**/
    WIN1251_currency,		/*  KOI8R_shade		145	*//**/
    WIN1251_currency,		/*  KOI8R_dkshade	146	*//**/
    WIN1251_currency,		/*  KOI8R_integraltp	147	*//**/
    WIN1251_currency,		/*  KOI8R_filledbox	148	*//**/
    WIN1251_bullet,		/*  KOI8R_bullet 	149	*/
    WIN1251_currency,		/*  KOI8R_radical	150	*//**/
    WIN1251_currency,		/*  KOI8R_approxequal	151	*//**/
    WIN1251_currency,		/*  KOI8R_lessequal	152	*//**/
    WIN1251_currency,		/*  KOI8R_greaterequal	153	*//**/
    WIN1251_nobreakspace,	/*  KOI8R_nobreakspace	154	*/
    WIN1251_currency,		/*  KOI8R_integralbt	155	*//**/
    WIN1251_degree,		/*  KOI8R_degree	156	*/
    WIN1251_currency,		/*  KOI8R_twosuperior	157	*//**/
    WIN1251_periodcentered,	/*  KOI8R_periodcentered 158	*/
    WIN1251_currency,		/*  KOI8R_divide	159	*//**/
    WIN1251_currency,		/*  KOI8R_SF430000	160	*//**/
    WIN1251_currency,		/*  KOI8R_SF240000	161	*//**/
    WIN1251_currency,		/*  KOI8R_SF510000	162	*//**/
    WIN1251_afii10071,		/*  KOI8R_afii10071	163	*/
    WIN1251_currency,		/*  KOI8R_SF520000	164	*//**/
    WIN1251_currency,		/*  KOI8R_SF390000	165	*//**/
    WIN1251_currency,		/*  KOI8R_SF220000	166	*//**/
    WIN1251_currency,		/*  KOI8R_SF210000	167	*//**/
    WIN1251_currency,		/*  KOI8R_SF250000	168	*//**/
    WIN1251_currency,		/*  KOI8R_SF500000	169	*//**/
    WIN1251_currency,		/*  KOI8R_SF490000	170	*//**/
    WIN1251_currency,		/*  KOI8R_SF380000	171	*//**/
    WIN1251_currency,		/*  KOI8R_SF280000	172	*//**/
    WIN1251_currency,		/*  KOI8R_SF270000	173	*//**/
    WIN1251_currency,		/*  KOI8R_SF260000	174	*//**/
    WIN1251_currency,		/*  KOI8R_SF360000	175	*//**/
    WIN1251_currency,		/*  KOI8R_SF370000	176	*//**/
    WIN1251_currency,		/*  KOI8R_SF420000	177	*//**/
    WIN1251_currency,		/*  KOI8R_SF190000	178	*//**/
    WIN1251_afii10023,		/*  KOI8R_afii10023	179	*/
    WIN1251_currency,		/*  KOI8R_SF200000	180	*//**/
    WIN1251_currency,		/*  KOI8R_SF230000	181	*//**/
    WIN1251_currency,		/*  KOI8R_SF470000	182	*//**/
    WIN1251_currency,		/*  KOI8R_SF480000	183	*//**/
    WIN1251_currency,		/*  KOI8R_SF410000	184	*//**/
    WIN1251_currency,		/*  KOI8R_SF450000	185	*//**/
    WIN1251_currency,		/*  KOI8R_SF460000	186	*//**/
    WIN1251_currency,		/*  KOI8R_SF400000	187	*//**/
    WIN1251_currency,		/*  KOI8R_SF540000	188	*//**/
    WIN1251_currency,		/*  KOI8R_SF530000	189	*//**/
    WIN1251_currency,		/*  KOI8R_SF440000	190	*//**/
    WIN1251_copyright,		/*  KOI8R_copyright	191	*/
    WIN1251_afii10096,		/*  KOI8R_afii10096	192	*/
    WIN1251_afii10065,		/*  KOI8R_afii10065	193	*/
    WIN1251_afii10066,		/*  KOI8R_afii10066	194	*/
    WIN1251_afii10088,		/*  KOI8R_afii10088	195	*/
    WIN1251_afii10069,		/*  KOI8R_afii10069	196	*/
    WIN1251_afii10070,		/*  KOI8R_afii10070	197	*/
    WIN1251_afii10086,		/*  KOI8R_afii10086	198	*/
    WIN1251_afii10068,		/*  KOI8R_afii10068	199	*/
    WIN1251_afii10087,		/*  KOI8R_afii10087	200	*/
    WIN1251_afii10074,		/*  KOI8R_afii10074	201	*/
    WIN1251_afii10075,		/*  KOI8R_afii10075	202	*/
    WIN1251_afii10076,		/*  KOI8R_afii10076	203	*/
    WIN1251_afii10077,		/*  KOI8R_afii10077	204	*/
    WIN1251_afii10078,		/*  KOI8R_afii10078	205	*/
    WIN1251_afii10079,		/*  KOI8R_afii10079	206	*/
    WIN1251_afii10080,		/*  KOI8R_afii10080	207	*/
    WIN1251_afii10081,		/*  KOI8R_afii10081	208	*/
    WIN1251_afii10097,		/*  KOI8R_afii10097	209	*/
    WIN1251_afii10082,		/*  KOI8R_afii10082	210	*/
    WIN1251_afii10083,		/*  KOI8R_afii10083	211	*/
    WIN1251_afii10084,		/*  KOI8R_afii10084	212	*/
    WIN1251_afii10085,		/*  KOI8R_afii10085	213	*/
    WIN1251_afii10072,		/*  KOI8R_afii10072	214	*/
    WIN1251_afii10067,		/*  KOI8R_afii10067	215	*/
    WIN1251_afii10094,		/*  KOI8R_afii10094	216	*/
    WIN1251_afii10093,		/*  KOI8R_afii10093	217	*/
    WIN1251_afii10073,		/*  KOI8R_afii10073	218	*/
    WIN1251_afii10090,		/*  KOI8R_afii10090	219	*/
    WIN1251_afii10095,		/*  KOI8R_afii10095	220	*/
    WIN1251_afii10091,		/*  KOI8R_afii10091	221	*/
    WIN1251_afii10089,		/*  KOI8R_afii10089	222	*/
    WIN1251_afii10092,		/*  KOI8R_afii10092	223	*/
    WIN1251_afii10048,		/*  KOI8R_afii10048	224	*/
    WIN1251_afii10017,		/*  KOI8R_afii10017	225	*/
    WIN1251_afii10018,		/*  KOI8R_afii10018	226	*/
    WIN1251_afii10040,		/*  KOI8R_afii10040	227	*/
    WIN1251_afii10021,		/*  KOI8R_afii10021	228	*/
    WIN1251_afii10022,		/*  KOI8R_afii10022	229	*/
    WIN1251_afii10038,		/*  KOI8R_afii10038	230	*/
    WIN1251_afii10020,		/*  KOI8R_afii10020	231	*/
    WIN1251_afii10039,		/*  KOI8R_afii10039	232	*/
    WIN1251_afii10026,		/*  KOI8R_afii10026	233	*/
    WIN1251_afii10027,		/*  KOI8R_afii10027	234	*/
    WIN1251_afii10028,		/*  KOI8R_afii10028	235	*/
    WIN1251_afii10029,		/*  KOI8R_afii10029	236	*/
    WIN1251_afii10030,		/*  KOI8R_afii10030	237	*/
    WIN1251_afii10031,		/*  KOI8R_afii10031	238	*/
    WIN1251_afii10032,		/*  KOI8R_afii10032	239	*/
    WIN1251_afii10033,		/*  KOI8R_afii10033	240	*/
    WIN1251_afii10049,		/*  KOI8R_afii10049	241	*/
    WIN1251_afii10034,		/*  KOI8R_afii10034	242	*/
    WIN1251_afii10035,		/*  KOI8R_afii10035	243	*/
    WIN1251_afii10036,		/*  KOI8R_afii10036	244	*/
    WIN1251_afii10037,		/*  KOI8R_afii10037	245	*/
    WIN1251_afii10024,		/*  KOI8R_afii10024	246	*/
    WIN1251_afii10019,		/*  KOI8R_afii10019	247	*/
    WIN1251_afii10046,		/*  KOI8R_afii10046	248	*/
    WIN1251_afii10045,		/*  KOI8R_afii10045	249	*/
    WIN1251_afii10025,		/*  KOI8R_afii10025	250	*/
    WIN1251_afii10042,		/*  KOI8R_afii10042	251	*/
    WIN1251_afii10047,		/*  KOI8R_afii10047	252	*/
    WIN1251_afii10043,		/*  KOI8R_afii10043	253	*/
    WIN1251_afii10041,		/*  KOI8R_afii10041	254	*/
    WIN1251_afii10044,		/*  KOI8R_afii10044	255	*/
    };

const unsigned char docWIN1251_to_KOI8R[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    KOI8R_space,		/*  WIN1251_space		 32	*/
    KOI8R_exclam,		/*  WIN1251_exclam		 33	*/
    KOI8R_quotedbl,		/*  WIN1251_quotedbl		 34	*/
    KOI8R_numbersign,		/*  WIN1251_numbersign		 35	*/
    KOI8R_dollar,		/*  WIN1251_dollar		 36	*/
    KOI8R_percent,		/*  WIN1251_percent		 37	*/
    KOI8R_ampersand,		/*  WIN1251_ampersand		 38	*/
    KOI8R_quotesingle,		/*  WIN1251_quotesingle		 39	*/
    KOI8R_parenleft,		/*  WIN1251_parenleft		 40	*/
    KOI8R_parenright,		/*  WIN1251_parenright		 41	*/
    KOI8R_asterisk,		/*  WIN1251_asterisk		 42	*/
    KOI8R_plus,			/*  WIN1251_plus		 43	*/
    KOI8R_comma,		/*  WIN1251_comma		 44	*/
    KOI8R_minus,		/*  WIN1251_minus		 45	*/
    KOI8R_period,		/*  WIN1251_period		 46	*/
    KOI8R_slash,		/*  WIN1251_slash		 47	*/
    KOI8R_zero,			/*  WIN1251_zero		 48	*/
    KOI8R_one,			/*  WIN1251_one			 49	*/
    KOI8R_two,			/*  WIN1251_two			 50	*/
    KOI8R_three,		/*  WIN1251_three		 51	*/
    KOI8R_four,			/*  WIN1251_four		 52	*/
    KOI8R_five,			/*  WIN1251_five		 53	*/
    KOI8R_six,			/*  WIN1251_six			 54	*/
    KOI8R_seven,		/*  WIN1251_seven		 55	*/
    KOI8R_eight,		/*  WIN1251_eight		 56	*/
    KOI8R_nine,			/*  WIN1251_nine		 57	*/
    KOI8R_colon,		/*  WIN1251_colon		 58	*/
    KOI8R_semicolon,		/*  WIN1251_semicolon		 59	*/
    KOI8R_less,			/*  WIN1251_less		 60	*/
    KOI8R_equal,		/*  WIN1251_equal		 61	*/
    KOI8R_greater,		/*  WIN1251_greater		 62	*/
    KOI8R_question,		/*  WIN1251_question		 63	*/
    KOI8R_at,			/*  WIN1251_at			 64	*/
    KOI8R_A,			/*  WIN1251_A			 65	*/
    KOI8R_B,			/*  WIN1251_B			 66	*/
    KOI8R_C,			/*  WIN1251_C			 67	*/
    KOI8R_D,			/*  WIN1251_D			 68	*/
    KOI8R_E,			/*  WIN1251_E			 69	*/
    KOI8R_F,			/*  WIN1251_F			 70	*/
    KOI8R_G,			/*  WIN1251_G			 71	*/
    KOI8R_H,			/*  WIN1251_H			 72	*/
    KOI8R_I,			/*  WIN1251_I			 73	*/
    KOI8R_J,			/*  WIN1251_J			 74	*/
    KOI8R_K,			/*  WIN1251_K			 75	*/
    KOI8R_L,			/*  WIN1251_L			 76	*/
    KOI8R_M,			/*  WIN1251_M			 77	*/
    KOI8R_N,			/*  WIN1251_N			 78	*/
    KOI8R_O,			/*  WIN1251_O			 79	*/
    KOI8R_P,			/*  WIN1251_P			 80	*/
    KOI8R_Q,			/*  WIN1251_Q			 81	*/
    KOI8R_R,			/*  WIN1251_R			 82	*/
    KOI8R_S,			/*  WIN1251_S			 83	*/
    KOI8R_T,			/*  WIN1251_T			 84	*/
    KOI8R_U,			/*  WIN1251_U			 85	*/
    KOI8R_V,			/*  WIN1251_V			 86	*/
    KOI8R_W,			/*  WIN1251_W			 87	*/
    KOI8R_X,			/*  WIN1251_X			 88	*/
    KOI8R_Y,			/*  WIN1251_Y			 89	*/
    KOI8R_Z,			/*  WIN1251_Z			 90	*/
    KOI8R_bracketleft,		/*  WIN1251_bracketleft		 91	*/
    KOI8R_backslash,		/*  WIN1251_backslash		 92	*/
    KOI8R_bracketright,		/*  WIN1251_bracketright	 93	*/
    KOI8R_asciicircum,		/*  WIN1251_asciicircum		 94	*/
    KOI8R_underscore,		/*  WIN1251_underscore		 95	*/
    KOI8R_quoteleft,		/*  WIN1251_quoteleft		 96	*/
    KOI8R_a,			/*  WIN1251_a			 97	*/
    KOI8R_b,			/*  WIN1251_b			 98	*/
    KOI8R_c,			/*  WIN1251_c			 99	*/
    KOI8R_d,			/*  WIN1251_d			100	*/
    KOI8R_e,			/*  WIN1251_e			101	*/
    KOI8R_f,			/*  WIN1251_f			102	*/
    KOI8R_g,			/*  WIN1251_g			103	*/
    KOI8R_h,			/*  WIN1251_h			104	*/
    KOI8R_i,			/*  WIN1251_i			105	*/
    KOI8R_j,			/*  WIN1251_j			106	*/
    KOI8R_k,			/*  WIN1251_k			107	*/
    KOI8R_l,			/*  WIN1251_l			108	*/
    KOI8R_m,			/*  WIN1251_m			109	*/
    KOI8R_n,			/*  WIN1251_n			110	*/
    KOI8R_o,			/*  WIN1251_o			111	*/
    KOI8R_p,			/*  WIN1251_p			112	*/
    KOI8R_q,			/*  WIN1251_q			113	*/
    KOI8R_r,			/*  WIN1251_r			114	*/
    KOI8R_s,			/*  WIN1251_s			115	*/
    KOI8R_t,			/*  WIN1251_t			116	*/
    KOI8R_u,			/*  WIN1251_u			117	*/
    KOI8R_v,			/*  WIN1251_v			118	*/
    KOI8R_w,			/*  WIN1251_w			119	*/
    KOI8R_x,			/*  WIN1251_x			120	*/
    KOI8R_y,			/*  WIN1251_y			121	*/
    KOI8R_z,			/*  WIN1251_z			122	*/
    KOI8R_braceleft,		/*  WIN1251_braceleft		123	*/
    KOI8R_bar,			/*  WIN1251_bar			124	*/
    KOI8R_braceright,		/*  WIN1251_braceright		125	*/
    KOI8R_asciitilde,		/*  WIN1251_asciitilde		126	*/
    KOI8R__0x7F,		/*  WIN1251__0x7F		127	*/
    KOI8R_copyright,		/*  WIN1251_afii10051		128	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10052		129	*//**/
    KOI8R_quoteleft,		/*  WIN1251_quotesinglbase	130	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10100		131	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblbase	132	*//**/
    KOI8R_copyright,		/*  WIN1251_ellipsis		133	*//**/
    KOI8R_copyright,		/*  WIN1251_dagger		134	*//**/
    KOI8R_copyright,		/*  WIN1251_daggerdbl		135	*//**/
    KOI8R_copyright,		/*  WIN1251__0x88		136	*//**/
    KOI8R_copyright,		/*  WIN1251_perthousand		137	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10058		138	*//**/
    KOI8R_copyright,		/*  WIN1251_guilsinglleft	139	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10059		140	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10061		141	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10060		142	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10145		143	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10099		144	*//**/
    KOI8R_quotesingle,		/*  WIN1251_quotesingle		145	*//**/
    KOI8R_quotesingle,		/*  WIN1251_quoteright		146	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblleft	147	*//**/
    KOI8R_quotedbl,		/*  WIN1251_quotedblright	148	*//**/
    KOI8R_bullet,		/*  WIN1251_bullet		149	*/
    KOI8R_minus,		/*  WIN1251_endash		150	*//**/
    KOI8R_minus,		/*  WIN1251_emdash		151	*//**/
    KOI8R_copyright,		/*  WIN1251__0x98		152	*//**/
    KOI8R_copyright,		/*  WIN1251_trademark		153	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10106		154	*//**/
    KOI8R_copyright,		/*  WIN1251_guilsinglright	155	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10107		156	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10109		157	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10108		158	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10193		159	*//**/
    KOI8R_nobreakspace,		/*  WIN1251_nobreakspace	160	*/
    KOI8R_copyright,		/*  WIN1251_afii10062		161	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10110		162	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10057		163	*//**/
    KOI8R_copyright,		/*  WIN1251_currency		164	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10050		165	*//**/
    KOI8R_bar,			/*  WIN1251_brokenbar		166	*//**/
    KOI8R_copyright,		/*  WIN1251_section		167	*//**/
    KOI8R_afii10023,		/*  WIN1251_afii10023		168	*/
    KOI8R_copyright,		/*  WIN1251_copyright		169	*/
    KOI8R_copyright,		/*  WIN1251_afii10053		170	*//**/
    KOI8R_copyright,		/*  WIN1251_guillemotleft	171	*//**/
    KOI8R_copyright,		/*  WIN1251_logicalnot		172	*//**/
    KOI8R_minus,		/*  WIN1251_hyphen		173	*//**/
    KOI8R_copyright,		/*  WIN1251_registered		174	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10056		175	*//**/
    KOI8R_degree,		/*  WIN1251_degree		176	*/
    KOI8R_copyright,		/*  WIN1251_plusminus		177	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10055		178	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10103		179	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10098		180	*//**/
    KOI8R_copyright,		/*  WIN1251_mu			181	*//**/
    KOI8R_copyright,		/*  WIN1251_paragraph		182	*//**/
    KOI8R_periodcentered,	/*  WIN1251_periodcentered	183	*/
    KOI8R_afii10071,		/*  WIN1251_afii10071		184	*/
    KOI8R_copyright,		/*  WIN1251_afii61352		185	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10101		186	*//**/
    KOI8R_copyright,		/*  WIN1251_guillemotright	187	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10105		188	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10054		189	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10102		190	*//**/
    KOI8R_copyright,		/*  WIN1251_afii10104		191	*//**/
    KOI8R_afii10017,		/*  WIN1251_afii10017		192	*/
    KOI8R_afii10018,		/*  WIN1251_afii10018		193	*/
    KOI8R_afii10019,		/*  WIN1251_afii10019		194	*/
    KOI8R_afii10020,		/*  WIN1251_afii10020		195	*/
    KOI8R_afii10021,		/*  WIN1251_afii10021		196	*/
    KOI8R_afii10022,		/*  WIN1251_afii10022		197	*/
    KOI8R_afii10024,		/*  WIN1251_afii10024		198	*/
    KOI8R_afii10025,		/*  WIN1251_afii10025		199	*/
    KOI8R_afii10026,		/*  WIN1251_afii10026		200	*/
    KOI8R_afii10027,		/*  WIN1251_afii10027		201	*/
    KOI8R_afii10028,		/*  WIN1251_afii10028		202	*/
    KOI8R_afii10029,		/*  WIN1251_afii10029		203	*/
    KOI8R_afii10030,		/*  WIN1251_afii10030		204	*/
    KOI8R_afii10031,		/*  WIN1251_afii10031		205	*/
    KOI8R_afii10032,		/*  WIN1251_afii10032		206	*/
    KOI8R_afii10033,		/*  WIN1251_afii10033		207	*/
    KOI8R_afii10034,		/*  WIN1251_afii10034		208	*/
    KOI8R_afii10035,		/*  WIN1251_afii10035		209	*/
    KOI8R_afii10036,		/*  WIN1251_afii10036		210	*/
    KOI8R_afii10037,		/*  WIN1251_afii10037		211	*/
    KOI8R_afii10038,		/*  WIN1251_afii10038		212	*/
    KOI8R_afii10039,		/*  WIN1251_afii10039		213	*/
    KOI8R_afii10040,		/*  WIN1251_afii10040		214	*/
    KOI8R_afii10041,		/*  WIN1251_afii10041		215	*/
    KOI8R_afii10042,		/*  WIN1251_afii10042		216	*/
    KOI8R_afii10043,		/*  WIN1251_afii10043		217	*/
    KOI8R_afii10044,		/*  WIN1251_afii10044		218	*/
    KOI8R_afii10045,		/*  WIN1251_afii10045		219	*/
    KOI8R_afii10046,		/*  WIN1251_afii10046		220	*/
    KOI8R_afii10047,		/*  WIN1251_afii10047		221	*/
    KOI8R_afii10048,		/*  WIN1251_afii10048		222	*/
    KOI8R_afii10049,		/*  WIN1251_afii10049		223	*/
    KOI8R_afii10065,		/*  WIN1251_afii10065		224	*/
    KOI8R_afii10066,		/*  WIN1251_afii10066		225	*/
    KOI8R_afii10067,		/*  WIN1251_afii10067		226	*/
    KOI8R_afii10068,		/*  WIN1251_afii10068		227	*/
    KOI8R_afii10069,		/*  WIN1251_afii10069		228	*/
    KOI8R_afii10070,		/*  WIN1251_afii10070		229	*/
    KOI8R_afii10072,		/*  WIN1251_afii10072		230	*/
    KOI8R_afii10073,		/*  WIN1251_afii10073		231	*/
    KOI8R_afii10074,		/*  WIN1251_afii10074		232	*/
    KOI8R_afii10075,		/*  WIN1251_afii10075		233	*/
    KOI8R_afii10076,		/*  WIN1251_afii10076		234	*/
    KOI8R_afii10077,		/*  WIN1251_afii10077		235	*/
    KOI8R_afii10078,		/*  WIN1251_afii10078		236	*/
    KOI8R_afii10079,		/*  WIN1251_afii10079		237	*/
    KOI8R_afii10080,		/*  WIN1251_afii10080		238	*/
    KOI8R_afii10081,		/*  WIN1251_afii10081		239	*/
    KOI8R_afii10082,		/*  WIN1251_afii10082		240	*/
    KOI8R_afii10083,		/*  WIN1251_afii10083		241	*/
    KOI8R_afii10084,		/*  WIN1251_afii10084		242	*/
    KOI8R_afii10085,		/*  WIN1251_afii10085		243	*/
    KOI8R_afii10086,		/*  WIN1251_afii10086		244	*/
    KOI8R_afii10087,		/*  WIN1251_afii10087		245	*/
    KOI8R_afii10088,		/*  WIN1251_afii10088		246	*/
    KOI8R_afii10089,		/*  WIN1251_afii10089		247	*/
    KOI8R_afii10090,		/*  WIN1251_afii10090		248	*/
    KOI8R_afii10091,		/*  WIN1251_afii10091		249	*/
    KOI8R_afii10092,		/*  WIN1251_afii10092		250	*/
    KOI8R_afii10093,		/*  WIN1251_afii10093		251	*/
    KOI8R_afii10094,		/*  WIN1251_afii10094		252	*/
    KOI8R_afii10095,		/*  WIN1251_afii10095		253	*/
    KOI8R_afii10096,		/*  WIN1251_afii10096		254	*/
    KOI8R_afii10097,		/*  WIN1251_afii10097		255	*/
    };

const unsigned char docWIN1251_to_ISO5[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    ISO5_space,			/*  WIN1251_space		 32	*/
    ISO5_exclam,		/*  WIN1251_exclam		 33	*/
    ISO5_quotedbl,		/*  WIN1251_quotedbl		 34	*/
    ISO5_numbersign,		/*  WIN1251_numbersign		 35	*/
    ISO5_dollar,		/*  WIN1251_dollar		 36	*/
    ISO5_percent,		/*  WIN1251_percent		 37	*/
    ISO5_ampersand,		/*  WIN1251_ampersand		 38	*/
    ISO5_quotesingle,		/*  WIN1251_quotesingle		 39	*/
    ISO5_parenleft,		/*  WIN1251_parenleft		 40	*/
    ISO5_parenright,		/*  WIN1251_parenright		 41	*/
    ISO5_asterisk,		/*  WIN1251_asterisk		 42	*/
    ISO5_plus,			/*  WIN1251_plus		 43	*/
    ISO5_comma,			/*  WIN1251_comma		 44	*/
    ISO5_minus,			/*  WIN1251_minus		 45	*/
    ISO5_period,		/*  WIN1251_period		 46	*/
    ISO5_slash,			/*  WIN1251_slash		 47	*/
    ISO5_zero,			/*  WIN1251_zero		 48	*/
    ISO5_one,			/*  WIN1251_one			 49	*/
    ISO5_two,			/*  WIN1251_two			 50	*/
    ISO5_three,			/*  WIN1251_three		 51	*/
    ISO5_four,			/*  WIN1251_four		 52	*/
    ISO5_five,			/*  WIN1251_five		 53	*/
    ISO5_six,			/*  WIN1251_six			 54	*/
    ISO5_seven,			/*  WIN1251_seven		 55	*/
    ISO5_eight,			/*  WIN1251_eight		 56	*/
    ISO5_nine,			/*  WIN1251_nine		 57	*/
    ISO5_colon,			/*  WIN1251_colon		 58	*/
    ISO5_semicolon,		/*  WIN1251_semicolon		 59	*/
    ISO5_less,			/*  WIN1251_less		 60	*/
    ISO5_equal,			/*  WIN1251_equal		 61	*/
    ISO5_greater,		/*  WIN1251_greater		 62	*/
    ISO5_question,		/*  WIN1251_question		 63	*/
    ISO5_at,			/*  WIN1251_at			 64	*/
    ISO5_A,			/*  WIN1251_A			 65	*/
    ISO5_B,			/*  WIN1251_B			 66	*/
    ISO5_C,			/*  WIN1251_C			 67	*/
    ISO5_D,			/*  WIN1251_D			 68	*/
    ISO5_E,			/*  WIN1251_E			 69	*/
    ISO5_F,			/*  WIN1251_F			 70	*/
    ISO5_G,			/*  WIN1251_G			 71	*/
    ISO5_H,			/*  WIN1251_H			 72	*/
    ISO5_I,			/*  WIN1251_I			 73	*/
    ISO5_J,			/*  WIN1251_J			 74	*/
    ISO5_K,			/*  WIN1251_K			 75	*/
    ISO5_L,			/*  WIN1251_L			 76	*/
    ISO5_M,			/*  WIN1251_M			 77	*/
    ISO5_N,			/*  WIN1251_N			 78	*/
    ISO5_O,			/*  WIN1251_O			 79	*/
    ISO5_P,			/*  WIN1251_P			 80	*/
    ISO5_Q,			/*  WIN1251_Q			 81	*/
    ISO5_R,			/*  WIN1251_R			 82	*/
    ISO5_S,			/*  WIN1251_S			 83	*/
    ISO5_T,			/*  WIN1251_T			 84	*/
    ISO5_U,			/*  WIN1251_U			 85	*/
    ISO5_V,			/*  WIN1251_V			 86	*/
    ISO5_W,			/*  WIN1251_W			 87	*/
    ISO5_X,			/*  WIN1251_X			 88	*/
    ISO5_Y,			/*  WIN1251_Y			 89	*/
    ISO5_Z,			/*  WIN1251_Z			 90	*/
    ISO5_bracketleft,		/*  WIN1251_bracketleft		 91	*/
    ISO5_backslash,		/*  WIN1251_backslash		 92	*/
    ISO5_bracketright,		/*  WIN1251_bracketright	 93	*/
    ISO5_asciicircum,		/*  WIN1251_asciicircum		 94	*/
    ISO5_underscore,		/*  WIN1251_underscore		 95	*/
    ISO5_quoteleft,		/*  WIN1251_quoteleft		 96	*/
    ISO5_a,			/*  WIN1251_a			 97	*/
    ISO5_b,			/*  WIN1251_b			 98	*/
    ISO5_c,			/*  WIN1251_c			 99	*/
    ISO5_d,			/*  WIN1251_d			100	*/
    ISO5_e,			/*  WIN1251_e			101	*/
    ISO5_f,			/*  WIN1251_f			102	*/
    ISO5_g,			/*  WIN1251_g			103	*/
    ISO5_h,			/*  WIN1251_h			104	*/
    ISO5_i,			/*  WIN1251_i			105	*/
    ISO5_j,			/*  WIN1251_j			106	*/
    ISO5_k,			/*  WIN1251_k			107	*/
    ISO5_l,			/*  WIN1251_l			108	*/
    ISO5_m,			/*  WIN1251_m			109	*/
    ISO5_n,			/*  WIN1251_n			110	*/
    ISO5_o,			/*  WIN1251_o			111	*/
    ISO5_p,			/*  WIN1251_p			112	*/
    ISO5_q,			/*  WIN1251_q			113	*/
    ISO5_r,			/*  WIN1251_r			114	*/
    ISO5_s,			/*  WIN1251_s			115	*/
    ISO5_t,			/*  WIN1251_t			116	*/
    ISO5_u,			/*  WIN1251_u			117	*/
    ISO5_v,			/*  WIN1251_v			118	*/
    ISO5_w,			/*  WIN1251_w			119	*/
    ISO5_x,			/*  WIN1251_x			120	*/
    ISO5_y,			/*  WIN1251_y			121	*/
    ISO5_z,			/*  WIN1251_z			122	*/
    ISO5_braceleft,		/*  WIN1251_braceleft		123	*/
    ISO5_bar,			/*  WIN1251_bar			124	*/
    ISO5_braceright,		/*  WIN1251_braceright		125	*/
    ISO5_asciitilde,		/*  WIN1251_asciitilde		126	*/
    ISO5_at,			/*  WIN1251__0x7F		127	*//**/
    ISO5_afii10051,		/*  WIN1251_afii10051		128	*/
    ISO5_afii10052,		/*  WIN1251_afii10052		129	*/
    ISO5_quoteleft,		/*  WIN1251_quotesinglbase	130	*//**/
    ISO5_afii10100,		/*  WIN1251_afii10100		131	*/
    ISO5_quotedbl,		/*  WIN1251_quotedblbase	132	*//**/
    ISO5_at,			/*  WIN1251_ellipsis		133	*//**/
    ISO5_at,			/*  WIN1251_dagger		134	*//**/
    ISO5_at,			/*  WIN1251_daggerdbl		135	*//**/
    ISO5_at,			/*  WIN1251__0x88		136	*//**/
    ISO5_at,			/*  WIN1251_perthousand		137	*//**/
    ISO5_afii10058,		/*  WIN1251_afii10058		138	*/
    ISO5_at,			/*  WIN1251_guilsinglleft	139	*//**/
    ISO5_afii10059,		/*  WIN1251_afii10059		140	*/
    ISO5_afii10061,		/*  WIN1251_afii10061		141	*/
    ISO5_afii10060,		/*  WIN1251_afii10060		142	*/
    ISO5_afii10145,		/*  WIN1251_afii10145		143	*/
    ISO5_afii10099,		/*  WIN1251_afii10099		144	*/
    ISO5_quotesingle,		/*  WIN1251_quotesingle		145	*/
    ISO5_quoteleft,		/*  WIN1251_quoteright		146	*//**/
    ISO5_quotedbl,		/*  WIN1251_quotedblleft	147	*//**/
    ISO5_quotedbl,		/*  WIN1251_quotedblright	148	*//**/
    ISO5_at,			/*  WIN1251_bullet		149	*//**/
    ISO5_minus,			/*  WIN1251_endash		150	*//**/
    ISO5_minus,			/*  WIN1251_emdash		151	*//**/
    ISO5_at,			/*  WIN1251__0x98		152	*//**/
    ISO5_at,			/*  WIN1251_trademark		153	*//**/
    ISO5_afii10106,		/*  WIN1251_afii10106		154	*/
    ISO5_at,			/*  WIN1251_guilsinglright	155	*//**/
    ISO5_afii10107,		/*  WIN1251_afii10107		156	*/
    ISO5_afii10109,		/*  WIN1251_afii10109		157	*/
    ISO5_afii10108,		/*  WIN1251_afii10108		158	*/
    ISO5_afii10193,		/*  WIN1251_afii10193		159	*/
    ISO5_nobreakspace,		/*  WIN1251_nobreakspace	160	*/
    ISO5_afii10062,		/*  WIN1251_afii10062		161	*/
    ISO5_afii10110,		/*  WIN1251_afii10110		162	*/
    ISO5_afii10057,		/*  WIN1251_afii10057		163	*/
    ISO5_at,			/*  WIN1251_currency		164	*//**/
    ISO5_at,			/*  WIN1251_afii10050		165	*//**/
    ISO5_bar,			/*  WIN1251_brokenbar		166	*//**/
    ISO5_section,		/*  WIN1251_section		167	*/
    ISO5_afii10023,		/*  WIN1251_afii10023		168	*/
    ISO5_at,			/*  WIN1251_copyright		169	*//**/
    ISO5_afii10053,		/*  WIN1251_afii10053		170	*/
    ISO5_at,			/*  WIN1251_guillemotleft	171	*//**/
    ISO5_at,			/*  WIN1251_logicalnot		172	*//**/
    ISO5_hyphen,		/*  WIN1251_hyphen		173	*/
    ISO5_at,			/*  WIN1251_registered		174	*//**/
    ISO5_afii10056,		/*  WIN1251_afii10056		175	*/
    ISO5_at,			/*  WIN1251_degree		176	*//**/
    ISO5_at,			/*  WIN1251_plusminus		177	*//**/
    ISO5_afii10055,		/*  WIN1251_afii10055		178	*/
    ISO5_afii10103,		/*  WIN1251_afii10103		179	*/
    ISO5_at,			/*  WIN1251_afii10098		180	*//**/
    ISO5_at,			/*  WIN1251_mu			181	*//**/
    ISO5_at,			/*  WIN1251_paragraph		182	*//**/
    ISO5_at,			/*  WIN1251_periodcentered	183	*//**/
    ISO5_afii10071,		/*  WIN1251_afii10071		184	*/
    ISO5_afii61352,		/*  WIN1251_afii61352		185	*/
    ISO5_afii10101,		/*  WIN1251_afii10101		186	*/
    ISO5_at,			/*  WIN1251_guillemotright	187	*//**/
    ISO5_afii10105,		/*  WIN1251_afii10105		188	*/
    ISO5_afii10054,		/*  WIN1251_afii10054		189	*/
    ISO5_afii10102,		/*  WIN1251_afii10102		190	*/
    ISO5_afii10104,		/*  WIN1251_afii10104		191	*/
    ISO5_afii10017,		/*  WIN1251_afii10017		192	*/
    ISO5_afii10018,		/*  WIN1251_afii10018		193	*/
    ISO5_afii10019,		/*  WIN1251_afii10019		194	*/
    ISO5_afii10020,		/*  WIN1251_afii10020		195	*/
    ISO5_afii10021,		/*  WIN1251_afii10021		196	*/
    ISO5_afii10022,		/*  WIN1251_afii10022		197	*/
    ISO5_afii10024,		/*  WIN1251_afii10024		198	*/
    ISO5_afii10025,		/*  WIN1251_afii10025		199	*/
    ISO5_afii10026,		/*  WIN1251_afii10026		200	*/
    ISO5_afii10027,		/*  WIN1251_afii10027		201	*/
    ISO5_afii10028,		/*  WIN1251_afii10028		202	*/
    ISO5_afii10029,		/*  WIN1251_afii10029		203	*/
    ISO5_afii10030,		/*  WIN1251_afii10030		204	*/
    ISO5_afii10031,		/*  WIN1251_afii10031		205	*/
    ISO5_afii10032,		/*  WIN1251_afii10032		206	*/
    ISO5_afii10033,		/*  WIN1251_afii10033		207	*/
    ISO5_afii10034,		/*  WIN1251_afii10034		208	*/
    ISO5_afii10035,		/*  WIN1251_afii10035		209	*/
    ISO5_afii10036,		/*  WIN1251_afii10036		210	*/
    ISO5_afii10037,		/*  WIN1251_afii10037		211	*/
    ISO5_afii10038,		/*  WIN1251_afii10038		212	*/
    ISO5_afii10039,		/*  WIN1251_afii10039		213	*/
    ISO5_afii10040,		/*  WIN1251_afii10040		214	*/
    ISO5_afii10041,		/*  WIN1251_afii10041		215	*/
    ISO5_afii10042,		/*  WIN1251_afii10042		216	*/
    ISO5_afii10043,		/*  WIN1251_afii10043		217	*/
    ISO5_afii10044,		/*  WIN1251_afii10044		218	*/
    ISO5_afii10045,		/*  WIN1251_afii10045		219	*/
    ISO5_afii10046,		/*  WIN1251_afii10046		220	*/
    ISO5_afii10047,		/*  WIN1251_afii10047		221	*/
    ISO5_afii10048,		/*  WIN1251_afii10048		222	*/
    ISO5_afii10049,		/*  WIN1251_afii10049		223	*/
    ISO5_afii10065,		/*  WIN1251_afii10065		224	*/
    ISO5_afii10066,		/*  WIN1251_afii10066		225	*/
    ISO5_afii10067,		/*  WIN1251_afii10067		226	*/
    ISO5_afii10068,		/*  WIN1251_afii10068		227	*/
    ISO5_afii10069,		/*  WIN1251_afii10069		228	*/
    ISO5_afii10070,		/*  WIN1251_afii10070		229	*/
    ISO5_afii10072,		/*  WIN1251_afii10072		230	*/
    ISO5_afii10073,		/*  WIN1251_afii10073		231	*/
    ISO5_afii10074,		/*  WIN1251_afii10074		232	*/
    ISO5_afii10075,		/*  WIN1251_afii10075		233	*/
    ISO5_afii10076,		/*  WIN1251_afii10076		234	*/
    ISO5_afii10077,		/*  WIN1251_afii10077		235	*/
    ISO5_afii10078,		/*  WIN1251_afii10078		236	*/
    ISO5_afii10079,		/*  WIN1251_afii10079		237	*/
    ISO5_afii10080,		/*  WIN1251_afii10080		238	*/
    ISO5_afii10081,		/*  WIN1251_afii10081		239	*/
    ISO5_afii10082,		/*  WIN1251_afii10082		240	*/
    ISO5_afii10083,		/*  WIN1251_afii10083		241	*/
    ISO5_afii10084,		/*  WIN1251_afii10084		242	*/
    ISO5_afii10085,		/*  WIN1251_afii10085		243	*/
    ISO5_afii10086,		/*  WIN1251_afii10086		244	*/
    ISO5_afii10087,		/*  WIN1251_afii10087		245	*/
    ISO5_afii10088,		/*  WIN1251_afii10088		246	*/
    ISO5_afii10089,		/*  WIN1251_afii10089		247	*/
    ISO5_afii10090,		/*  WIN1251_afii10090		248	*/
    ISO5_afii10091,		/*  WIN1251_afii10091		249	*/
    ISO5_afii10092,		/*  WIN1251_afii10092		250	*/
    ISO5_afii10093,		/*  WIN1251_afii10093		251	*/
    ISO5_afii10094,		/*  WIN1251_afii10094		252	*/
    ISO5_afii10095,		/*  WIN1251_afii10095		253	*/
    ISO5_afii10096,		/*  WIN1251_afii10096		254	*/
    ISO5_afii10097,		/*  WIN1251_afii10097		255	*/
    };

const unsigned char docISO5_to_WIN1251[256]=
    {
    0x00,			/*  				  0	*/
    0x01,			/*  				  1	*/
    0x02,			/*  				  2	*/
    0x03,			/*  				  3	*/
    0x04,			/*  				  4	*/
    0x05,			/*  				  5	*/
    0x06,			/*  				  6	*/
    0x07,			/*  				  7	*/
    0x08,			/*  				  8	*/
    0x09,			/*  				  9	*/
    0x0A,			/*  				 10	*/
    0x0B,			/*  				 11	*/
    0x0C,			/*  				 12	*/
    0x0D,			/*  				 13	*/
    0x0E,			/*  				 14	*/
    0x0F,			/*  				 15	*/
    0x10,			/*  				 16	*/
    0x11,			/*  				 17	*/
    0x12,			/*  				 18	*/
    0x13,			/*  				 19	*/
    0x14,			/*  				 20	*/
    0x15,			/*  				 21	*/
    0x16,			/*  				 22	*/
    0x17,			/*  				 23	*/
    0x18,			/*  				 24	*/
    0x19,			/*  				 25	*/
    0x1A,			/*  				 26	*/
    0x1B,			/*  				 27	*/
    0x1C,			/*  				 28	*/
    0x1D,			/*  				 29	*/
    0x1E,			/*  				 30	*/
    0x1F,			/*  				 31	*/
    WIN1251_space,		/*  ISO5_space			 32	*/
    WIN1251_exclam,		/*  ISO5_exclam			 33	*/
    WIN1251_quotedbl,		/*  ISO5_quotedbl		 34	*/
    WIN1251_numbersign,		/*  ISO5_numbersign		 35	*/
    WIN1251_dollar,		/*  ISO5_dollar			 36	*/
    WIN1251_percent,		/*  ISO5_percent		 37	*/
    WIN1251_ampersand,		/*  ISO5_ampersand		 38	*/
    WIN1251_quotesingle,	/*  ISO5_quotesingle		 39	*/
    WIN1251_parenleft,		/*  ISO5_parenleft		 40	*/
    WIN1251_parenright,		/*  ISO5_parenright		 41	*/
    WIN1251_asterisk,		/*  ISO5_asterisk		 42	*/
    WIN1251_plus,		/*  ISO5_plus			 43	*/
    WIN1251_comma,		/*  ISO5_comma			 44	*/
    WIN1251_minus,		/*  ISO5_minus			 45	*/
    WIN1251_period,		/*  ISO5_period			 46	*/
    WIN1251_slash,		/*  ISO5_slash			 47	*/
    WIN1251_zero,		/*  ISO5_zero			 48	*/
    WIN1251_one,		/*  ISO5_one			 49	*/
    WIN1251_two,		/*  ISO5_two			 50	*/
    WIN1251_three,		/*  ISO5_three			 51	*/
    WIN1251_four,		/*  ISO5_four			 52	*/
    WIN1251_five,		/*  ISO5_five			 53	*/
    WIN1251_six,		/*  ISO5_six			 54	*/
    WIN1251_seven,		/*  ISO5_seven			 55	*/
    WIN1251_eight,		/*  ISO5_eight			 56	*/
    WIN1251_nine,		/*  ISO5_nine			 57	*/
    WIN1251_colon,		/*  ISO5_colon			 58	*/
    WIN1251_semicolon,		/*  ISO5_semicolon		 59	*/
    WIN1251_less,		/*  ISO5_less			 60	*/
    WIN1251_equal,		/*  ISO5_equal			 61	*/
    WIN1251_greater,		/*  ISO5_greater		 62	*/
    WIN1251_question,		/*  ISO5_question		 63	*/
    WIN1251_at,			/*  ISO5_at			 64	*/
    WIN1251_A,			/*  ISO5_A			 65	*/
    WIN1251_B,			/*  ISO5_B			 66	*/
    WIN1251_C,			/*  ISO5_C			 67	*/
    WIN1251_D,			/*  ISO5_D			 68	*/
    WIN1251_E,			/*  ISO5_E			 69	*/
    WIN1251_F,			/*  ISO5_F			 70	*/
    WIN1251_G,			/*  ISO5_G			 71	*/
    WIN1251_H,			/*  ISO5_H			 72	*/
    WIN1251_I,			/*  ISO5_I			 73	*/
    WIN1251_J,			/*  ISO5_J			 74	*/
    WIN1251_K,			/*  ISO5_K			 75	*/
    WIN1251_L,			/*  ISO5_L			 76	*/
    WIN1251_M,			/*  ISO5_M			 77	*/
    WIN1251_N,			/*  ISO5_N			 78	*/
    WIN1251_O,			/*  ISO5_O			 79	*/
    WIN1251_P,			/*  ISO5_P			 80	*/
    WIN1251_Q,			/*  ISO5_Q			 81	*/
    WIN1251_R,			/*  ISO5_R			 82	*/
    WIN1251_S,			/*  ISO5_S			 83	*/
    WIN1251_T,			/*  ISO5_T			 84	*/
    WIN1251_U,			/*  ISO5_U			 85	*/
    WIN1251_V,			/*  ISO5_V			 86	*/
    WIN1251_W,			/*  ISO5_W			 87	*/
    WIN1251_X,			/*  ISO5_X			 88	*/
    WIN1251_Y,			/*  ISO5_Y			 89	*/
    WIN1251_Z,			/*  ISO5_Z			 90	*/
    WIN1251_bracketleft,	/*  ISO5_bracketleft		 91	*/
    WIN1251_backslash,		/*  ISO5_backslash		 92	*/
    WIN1251_bracketright,	/*  ISO5_bracketright		 93	*/
    WIN1251_asciicircum,	/*  ISO5_asciicircum		 94	*/
    WIN1251_underscore,		/*  ISO5_underscore		 95	*/
    WIN1251_quoteleft,		/*  ISO5_quoteleft		 96	*/
    WIN1251_a,			/*  ISO5_a			 97	*/
    WIN1251_b,			/*  ISO5_b			 98	*/
    WIN1251_c,			/*  ISO5_c			 99	*/
    WIN1251_d,			/*  ISO5_d			100	*/
    WIN1251_e,			/*  ISO5_e			101	*/
    WIN1251_f,			/*  ISO5_f			102	*/
    WIN1251_g,			/*  ISO5_g			103	*/
    WIN1251_h,			/*  ISO5_h			104	*/
    WIN1251_i,			/*  ISO5_i			105	*/
    WIN1251_j,			/*  ISO5_j			106	*/
    WIN1251_k,			/*  ISO5_k			107	*/
    WIN1251_l,			/*  ISO5_l			108	*/
    WIN1251_m,			/*  ISO5_m			109	*/
    WIN1251_n,			/*  ISO5_n			110	*/
    WIN1251_o,			/*  ISO5_o			111	*/
    WIN1251_p,			/*  ISO5_p			112	*/
    WIN1251_q,			/*  ISO5_q			113	*/
    WIN1251_r,			/*  ISO5_r			114	*/
    WIN1251_s,			/*  ISO5_s			115	*/
    WIN1251_t,			/*  ISO5_t			116	*/
    WIN1251_u,			/*  ISO5_u			117	*/
    WIN1251_v,			/*  ISO5_v			118	*/
    WIN1251_w,			/*  ISO5_w			119	*/
    WIN1251_x,			/*  ISO5_x			120	*/
    WIN1251_y,			/*  ISO5_y			121	*/
    WIN1251_z,			/*  ISO5_z			122	*/
    WIN1251_braceleft,		/*  ISO5_braceleft		123	*/
    WIN1251_bar,		/*  ISO5_bar			124	*/
    WIN1251_braceright,		/*  ISO5_braceright		125	*/
    WIN1251_asciitilde,		/*  ISO5_asciitilde		126	*/
    WIN1251_currency,		/*  .NOTDEF_127			127	*//**/
    WIN1251_currency,		/*  .NOTDEF_128			128	*//**/
    WIN1251_currency,		/*  .NOTDEF_129			129	*//**/
    WIN1251_currency,		/*  .NOTDEF_130			130	*//**/
    WIN1251_currency,		/*  .NOTDEF_131			131	*//**/
    WIN1251_currency,		/*  .NOTDEF_132			132	*//**/
    WIN1251_currency,		/*  .NOTDEF_133			133	*//**/
    WIN1251_currency,		/*  .NOTDEF_134			134	*//**/
    WIN1251_currency,		/*  .NOTDEF_135			135	*//**/
    WIN1251_currency,		/*  .NOTDEF_136			136	*//**/
    WIN1251_currency,		/*  .NOTDEF_137			137	*//**/
    WIN1251_currency,		/*  .NOTDEF_138			138	*//**/
    WIN1251_currency,		/*  .NOTDEF_139			139	*//**/
    WIN1251_currency,		/*  .NOTDEF_140			140	*//**/
    WIN1251_currency,		/*  .NOTDEF_141			141	*//**/
    WIN1251_currency,		/*  .NOTDEF_142			142	*//**/
    WIN1251_currency,		/*  .NOTDEF_143			143	*//**/
    WIN1251_currency,		/*  .NOTDEF_144			144	*//**/
    WIN1251_currency,		/*  .NOTDEF_145			145	*//**/
    WIN1251_currency,		/*  .NOTDEF_146			146	*//**/
    WIN1251_currency,		/*  .NOTDEF_147			147	*//**/
    WIN1251_currency,		/*  .NOTDEF_148			148	*//**/
    WIN1251_currency,		/*  .NOTDEF_149			149	*//**/
    WIN1251_currency,		/*  .NOTDEF_150			150	*//**/
    WIN1251_currency,		/*  .NOTDEF_151			151	*//**/
    WIN1251_currency,		/*  .NOTDEF_152			152	*//**/
    WIN1251_currency,		/*  .NOTDEF_153			153	*//**/
    WIN1251_currency,		/*  .NOTDEF_154			154	*//**/
    WIN1251_currency,		/*  .NOTDEF_155			155	*//**/
    WIN1251_currency,		/*  .NOTDEF_156			156	*//**/
    WIN1251_currency,		/*  .NOTDEF_157			157	*//**/
    WIN1251_currency,		/*  .NOTDEF_158			158	*//**/
    WIN1251_currency,		/*  .NOTDEF_159			159	*//**/
    WIN1251_nobreakspace,	/*  ISO5_nobreakspace		160	*/
    WIN1251_afii10023,		/*  ISO5_afii10023		161	*/
    WIN1251_afii10051,		/*  ISO5_afii10051		162	*/
    WIN1251_afii10052,		/*  ISO5_afii10052		163	*/
    WIN1251_afii10053,		/*  ISO5_afii10053		164	*/
    WIN1251_afii10054,		/*  ISO5_afii10054		165	*/
    WIN1251_afii10055,		/*  ISO5_afii10055		166	*/
    WIN1251_afii10056,		/*  ISO5_afii10056		167	*/
    WIN1251_afii10057,		/*  ISO5_afii10057		168	*/
    WIN1251_afii10058,		/*  ISO5_afii10058		169	*/
    WIN1251_afii10059,		/*  ISO5_afii10059		170	*/
    WIN1251_afii10060,		/*  ISO5_afii10060		171	*/
    WIN1251_afii10061,		/*  ISO5_afii10061		172	*/
    WIN1251_hyphen,		/*  ISO5_hyphen			173	*/
    WIN1251_afii10062,		/*  ISO5_afii10062		174	*/
    WIN1251_afii10145,		/*  ISO5_afii10145		175	*/
    WIN1251_afii10017,		/*  ISO5_afii10017		176	*/
    WIN1251_afii10018,		/*  ISO5_afii10018		177	*/
    WIN1251_afii10019,		/*  ISO5_afii10019		178	*/
    WIN1251_afii10020,		/*  ISO5_afii10020		179	*/
    WIN1251_afii10021,		/*  ISO5_afii10021		180	*/
    WIN1251_afii10022,		/*  ISO5_afii10022		181	*/
    WIN1251_afii10024,		/*  ISO5_afii10024		182	*/
    WIN1251_afii10025,		/*  ISO5_afii10025		183	*/
    WIN1251_afii10026,		/*  ISO5_afii10026		184	*/
    WIN1251_afii10027,		/*  ISO5_afii10027		185	*/
    WIN1251_afii10028,		/*  ISO5_afii10028		186	*/
    WIN1251_afii10029,		/*  ISO5_afii10029		187	*/
    WIN1251_afii10030,		/*  ISO5_afii10030		188	*/
    WIN1251_afii10031,		/*  ISO5_afii10031		189	*/
    WIN1251_afii10032,		/*  ISO5_afii10032		190	*/
    WIN1251_afii10033,		/*  ISO5_afii10033		191	*/
    WIN1251_afii10034,		/*  ISO5_afii10034		192	*/
    WIN1251_afii10035,		/*  ISO5_afii10035		193	*/
    WIN1251_afii10036,		/*  ISO5_afii10036		194	*/
    WIN1251_afii10037,		/*  ISO5_afii10037		195	*/
    WIN1251_afii10038,		/*  ISO5_afii10038		196	*/
    WIN1251_afii10039,		/*  ISO5_afii10039		197	*/
    WIN1251_afii10040,		/*  ISO5_afii10040		198	*/
    WIN1251_afii10041,		/*  ISO5_afii10041		199	*/
    WIN1251_afii10042,		/*  ISO5_afii10042		200	*/
    WIN1251_afii10043,		/*  ISO5_afii10043		201	*/
    WIN1251_afii10044,		/*  ISO5_afii10044		202	*/
    WIN1251_afii10045,		/*  ISO5_afii10045		203	*/
    WIN1251_afii10046,		/*  ISO5_afii10046		204	*/
    WIN1251_afii10047,		/*  ISO5_afii10047		205	*/
    WIN1251_afii10048,		/*  ISO5_afii10048		206	*/
    WIN1251_afii10049,		/*  ISO5_afii10049		207	*/
    WIN1251_afii10065,		/*  ISO5_afii10065		208	*/
    WIN1251_afii10066,		/*  ISO5_afii10066		209	*/
    WIN1251_afii10067,		/*  ISO5_afii10067		210	*/
    WIN1251_afii10068,		/*  ISO5_afii10068		211	*/
    WIN1251_afii10069,		/*  ISO5_afii10069		212	*/
    WIN1251_afii10070,		/*  ISO5_afii10070		213	*/
    WIN1251_afii10072,		/*  ISO5_afii10072		214	*/
    WIN1251_afii10073,		/*  ISO5_afii10073		215	*/
    WIN1251_afii10074,		/*  ISO5_afii10074		216	*/
    WIN1251_afii10075,		/*  ISO5_afii10075		217	*/
    WIN1251_afii10076,		/*  ISO5_afii10076		218	*/
    WIN1251_afii10077,		/*  ISO5_afii10077		219	*/
    WIN1251_afii10078,		/*  ISO5_afii10078		220	*/
    WIN1251_afii10079,		/*  ISO5_afii10079		221	*/
    WIN1251_afii10080,		/*  ISO5_afii10080		222	*/
    WIN1251_afii10081,		/*  ISO5_afii10081		223	*/
    WIN1251_afii10082,		/*  ISO5_afii10082		224	*/
    WIN1251_afii10083,		/*  ISO5_afii10083		225	*/
    WIN1251_afii10084,		/*  ISO5_afii10084		226	*/
    WIN1251_afii10085,		/*  ISO5_afii10085		227	*/
    WIN1251_afii10086,		/*  ISO5_afii10086		228	*/
    WIN1251_afii10087,		/*  ISO5_afii10087		229	*/
    WIN1251_afii10088,		/*  ISO5_afii10088		230	*/
    WIN1251_afii10089,		/*  ISO5_afii10089		231	*/
    WIN1251_afii10090,		/*  ISO5_afii10090		232	*/
    WIN1251_afii10091,		/*  ISO5_afii10091		233	*/
    WIN1251_afii10092,		/*  ISO5_afii10092		234	*/
    WIN1251_afii10093,		/*  ISO5_afii10093		235	*/
    WIN1251_afii10094,		/*  ISO5_afii10094		236	*/
    WIN1251_afii10095,		/*  ISO5_afii10095		237	*/
    WIN1251_afii10096,		/*  ISO5_afii10096		238	*/
    WIN1251_afii10097,		/*  ISO5_afii10097		239	*/
    WIN1251_afii61352,		/*  ISO5_afii61352		240	*/
    WIN1251_afii10071,		/*  ISO5_afii10071		241	*/
    WIN1251_afii10099,		/*  ISO5_afii10099		242	*/
    WIN1251_afii10100,		/*  ISO5_afii10100		243	*/
    WIN1251_afii10101,		/*  ISO5_afii10101		244	*/
    WIN1251_afii10102,		/*  ISO5_afii10102		245	*/
    WIN1251_afii10103,		/*  ISO5_afii10103		246	*/
    WIN1251_afii10104,		/*  ISO5_afii10104		247	*/
    WIN1251_afii10105,		/*  ISO5_afii10105		248	*/
    WIN1251_afii10106,		/*  ISO5_afii10106		249	*/
    WIN1251_afii10107,		/*  ISO5_afii10107		250	*/
    WIN1251_afii10108,		/*  ISO5_afii10108		251	*/
    WIN1251_afii10109,		/*  ISO5_afii10109		252	*/
    WIN1251_section,		/*  ISO5_section		253	*/
    WIN1251_afii10110,		/*  ISO5_afii10110		254	*/
    WIN1251_afii10193,		/*  ISO5_afii10193		255	*/
    };

