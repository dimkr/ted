/************************************************************************/
/*									*/
/*  Map Font Encodings Baltic: WIN1257 <-> ISO 8859-13			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

const unsigned char docWIN1257_to_ISO13[256]=
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
    ISO13_space,		/*  WIN1257_space	 32	*/
    ISO13_exclam,		/*  WIN1257_exclam	 33	*/
    ISO13_quotedbl,		/*  WIN1257_quotedbl	 34	*/
    ISO13_numbersign,		/*  WIN1257_numbersign	 35	*/
    ISO13_dollar,		/*  WIN1257_dollar	 36	*/
    ISO13_percent,		/*  WIN1257_percent	 37	*/
    ISO13_ampersand,		/*  WIN1257_ampersand	 38	*/
    ISO13_quotesingle,		/*  WIN1257_quotesingle	 39	*/
    ISO13_parenleft,		/*  WIN1257_parenleft	 40	*/
    ISO13_parenright,		/*  WIN1257_parenright	 41	*/
    ISO13_asterisk,		/*  WIN1257_asterisk	 42	*/
    ISO13_plus,			/*  WIN1257_plus	 43	*/
    ISO13_comma,		/*  WIN1257_comma	 44	*/
    ISO13_minus,		/*  WIN1257_minus	 45	*/
    ISO13_period,		/*  WIN1257_period	 46	*/
    ISO13_slash,		/*  WIN1257_slash	 47	*/
    ISO13_zero,			/*  WIN1257_zero	 48	*/
    ISO13_one,			/*  WIN1257_one		 49	*/
    ISO13_two,			/*  WIN1257_two		 50	*/
    ISO13_three,		/*  WIN1257_three	 51	*/
    ISO13_four,			/*  WIN1257_four	 52	*/
    ISO13_five,			/*  WIN1257_five	 53	*/
    ISO13_six,			/*  WIN1257_six		 54	*/
    ISO13_seven,		/*  WIN1257_seven	 55	*/
    ISO13_eight,		/*  WIN1257_eight	 56	*/
    ISO13_nine,			/*  WIN1257_nine	 57	*/
    ISO13_colon,		/*  WIN1257_colon	 58	*/
    ISO13_semicolon,		/*  WIN1257_semicolon	 59	*/
    ISO13_less,			/*  WIN1257_less	 60	*/
    ISO13_equal,		/*  WIN1257_equal	 61	*/
    ISO13_greater,		/*  WIN1257_greater	 62	*/
    ISO13_question,		/*  WIN1257_question	 63	*/
    ISO13_at,			/*  WIN1257_at		 64	*/
    ISO13_A,			/*  WIN1257_A		 65	*/
    ISO13_B,			/*  WIN1257_B		 66	*/
    ISO13_C,			/*  WIN1257_C		 67	*/
    ISO13_D,			/*  WIN1257_D		 68	*/
    ISO13_E,			/*  WIN1257_E		 69	*/
    ISO13_F,			/*  WIN1257_F		 70	*/
    ISO13_G,			/*  WIN1257_G		 71	*/
    ISO13_H,			/*  WIN1257_H		 72	*/
    ISO13_I,			/*  WIN1257_I		 73	*/
    ISO13_J,			/*  WIN1257_J		 74	*/
    ISO13_K,			/*  WIN1257_K		 75	*/
    ISO13_L,			/*  WIN1257_L		 76	*/
    ISO13_M,			/*  WIN1257_M		 77	*/
    ISO13_N,			/*  WIN1257_N		 78	*/
    ISO13_O,			/*  WIN1257_O		 79	*/
    ISO13_P,			/*  WIN1257_P		 80	*/
    ISO13_Q,			/*  WIN1257_Q		 81	*/
    ISO13_R,			/*  WIN1257_R		 82	*/
    ISO13_S,			/*  WIN1257_S		 83	*/
    ISO13_T,			/*  WIN1257_T		 84	*/
    ISO13_U,			/*  WIN1257_U		 85	*/
    ISO13_V,			/*  WIN1257_V		 86	*/
    ISO13_W,			/*  WIN1257_W		 87	*/
    ISO13_X,			/*  WIN1257_X		 88	*/
    ISO13_Y,			/*  WIN1257_Y		 89	*/
    ISO13_Z,			/*  WIN1257_Z		 90	*/
    ISO13_bracketleft,		/*  WIN1257_bracketleft	 91	*/
    ISO13_backslash,		/*  WIN1257_backslash	 92	*/
    ISO13_bracketright,		/*  WIN1257_bracketright 93	*/
    ISO13_asciicircum,		/*  WIN1257_asciicircum	 94	*/
    ISO13_underscore,		/*  WIN1257_underscore	 95	*/
    ISO13_grave,		/*  WIN1257_grave	 96	*/
    ISO13_a,			/*  WIN1257_a		 97	*/
    ISO13_b,			/*  WIN1257_b		 98	*/
    ISO13_c,			/*  WIN1257_c		 99	*/
    ISO13_d,			/*  WIN1257_d		100	*/
    ISO13_e,			/*  WIN1257_e		101	*/
    ISO13_f,			/*  WIN1257_f		102	*/
    ISO13_g,			/*  WIN1257_g		103	*/
    ISO13_h,			/*  WIN1257_h		104	*/
    ISO13_i,			/*  WIN1257_i		105	*/
    ISO13_j,			/*  WIN1257_j		106	*/
    ISO13_k,			/*  WIN1257_k		107	*/
    ISO13_l,			/*  WIN1257_l		108	*/
    ISO13_m,			/*  WIN1257_m		109	*/
    ISO13_n,			/*  WIN1257_n		110	*/
    ISO13_o,			/*  WIN1257_o		111	*/
    ISO13_p,			/*  WIN1257_p		112	*/
    ISO13_q,			/*  WIN1257_q		113	*/
    ISO13_r,			/*  WIN1257_r		114	*/
    ISO13_s,			/*  WIN1257_s		115	*/
    ISO13_t,			/*  WIN1257_t		116	*/
    ISO13_u,			/*  WIN1257_u		117	*/
    ISO13_v,			/*  WIN1257_v		118	*/
    ISO13_w,			/*  WIN1257_w		119	*/
    ISO13_x,			/*  WIN1257_x		120	*/
    ISO13_y,			/*  WIN1257_y		121	*/
    ISO13_z,			/*  WIN1257_z		122	*/
    ISO13_braceleft,		/*  WIN1257_braceleft	123	*/
    ISO13_bar,			/*  WIN1257_bar		124	*/
    ISO13_braceright,		/*  WIN1257_braceright	125	*/
    ISO13_asciitilde,		/*  WIN1257_asciitilde	126	*/
    ISO13__0x7F,		/*  WIN1257__0x7F	127	*/
    ISO13__0x80,		/*  WIN1257__0x80	128	*/
    ISO13__0x81,		/*  WIN1257__0x81	129	*/
    ISO13_quotesingle,		/*  WIN1257_quotesinglbase 130	*//**/
    ISO13__0x83,		/*  WIN1257__0x83	131	*/
    ISO13_quotedblbase,		/*  WIN1257_quotedblbase 132	*/
    ISO13_currency,		/*  WIN1257_ellipsis	133	*//**/
    ISO13_currency,		/*  WIN1257_dagger	134	*//**/
    ISO13_currency,		/*  WIN1257_daggerdbl	135	*//**/
    ISO13__0x88,		/*  WIN1257__0x88	136	*/
    ISO13_currency,		/*  WIN1257_perthousand	137	*//**/
    ISO13__0x8A,		/*  WIN1257__0x8A	138	*/
    ISO13_currency,		/*  WIN1257_guilsinglleft 139	*//**/
    ISO13__0x8C,		/*  WIN1257__0x8C	140	*/
    ISO13_currency,		/*  WIN1257_dieresis	141	*//**/
    ISO13_currency,		/*  WIN1257_caron	142	*//**/
    ISO13_currency,		/*  WIN1257_cedilla	143	*//**/
    ISO13__0x90,		/*  WIN1257__0x90	144	*/
    ISO13_quotesingle,		/*  WIN1257_quotesingle	145	*//**/
    ISO13_quoteright,		/*  WIN1257_quoteright	146	*/
    ISO13_quotedblleft,		/*  WIN1257_quotedblleft 147	*/
    ISO13_quotedblright,	/*  WIN1257_quotedblright 148	*/
    ISO13_currency,		/*  WIN1257_bullet	149	*//**/
    ISO13_minus,		/*  WIN1257_endash	150	*//**/
    ISO13_minus,		/*  WIN1257_emdash	151	*//**/
    ISO13__0x98,		/*  WIN1257__0x98	152	*/
    ISO13_currency,		/*  WIN1257_trademark	153	*//**/
    ISO13__0x9A,		/*  WIN1257__0x9A	154	*/
    ISO13_currency,		/*  WIN1257_guilsinglright 155	*//**/
    ISO13__0x9C,		/*  WIN1257__0x9C	156	*/
    ISO13_currency,		/*  WIN1257_macron	157	*//**/
    ISO13_currency,		/*  WIN1257_ogonek	158	*//**/
    ISO13__0x9F,		/*  WIN1257__0x9F	159	*/
    ISO13_nobreakspace,		/*  WIN1257_space	160	*/
    ISO13_currency,		/*  WIN1257__0xA1	161	*//**/
    ISO13_cent,			/*  WIN1257_cent	162	*/
    ISO13_sterling,		/*  WIN1257_sterling	163	*/
    ISO13_currency,		/*  WIN1257_currency	164	*/
    ISO13_currency,		/*  WIN1257__0xA5	165	*//**/
    ISO13_brokenbar,		/*  WIN1257_brokenbar	166	*/
    ISO13_section,		/*  WIN1257_section	167	*/
    ISO13_Oslash,		/*  WIN1257_Oslash	168	*/
    ISO13_copyright,		/*  WIN1257_copyright	169	*/
    ISO13_Rcommaaccent,		/*  WIN1257_Rcommaaccent 170	*/
    ISO13_guillemotleft,	/*  WIN1257_guillemotleft 171	*/
    ISO13_logicalnot,		/*  WIN1257_logicalnot	172	*/
    ISO13_hyphen,		/*  WIN1257_hyphen	173	*/
    ISO13_registered,		/*  WIN1257_registered	174	*/
    ISO13_AE,			/*  WIN1257_AE		175	*/
    ISO13_degree,		/*  WIN1257_degree	176	*/
    ISO13_plusminus,		/*  WIN1257_plusminus	177	*/
    ISO13_twosuperior,		/*  WIN1257_twosuperior	178	*/
    ISO13_threesuperior,	/*  WIN1257_threesuperior 179	*/
    ISO13_currency,		/*  WIN1257_acute	180	*//**/
    ISO13_mu,			/*  WIN1257_mu		181	*/
    ISO13_paragraph,		/*  WIN1257_paragraph	182	*/
    ISO13_periodcentered,	/*  WIN1257_periodcentered 183	*/
    ISO13_oslash,		/*  WIN1257_oslash	184	*/
    ISO13_onesuperior,		/*  WIN1257_onesuperior	185	*/
    ISO13_rcommaaccent,		/*  WIN1257_rcommaaccent 186	*/
    ISO13_guillemotright,	/*  WIN1257_guillemotright 187	*/
    ISO13_onequarter,		/*  WIN1257_onequarter	188	*/
    ISO13_onehalf,		/*  WIN1257_onehalf	189	*/
    ISO13_threequarters,	/*  WIN1257_threequarters 190	*/
    ISO13_ae,			/*  WIN1257_ae		191	*/
    ISO13_Aogonek,		/*  WIN1257_Aogonek	192	*/
    ISO13_Iogonek,		/*  WIN1257_Iogonek	193	*/
    ISO13_Amacron,		/*  WIN1257_Amacron	194	*/
    ISO13_Cacute,		/*  WIN1257_Cacute	195	*/
    ISO13_Adieresis,		/*  WIN1257_Adieresis	196	*/
    ISO13_Aring,		/*  WIN1257_Aring	197	*/
    ISO13_Eogonek,		/*  WIN1257_Eogonek	198	*/
    ISO13_Emacron,		/*  WIN1257_Emacron	199	*/
    ISO13_Ccaron,		/*  WIN1257_Ccaron	200	*/
    ISO13_Eacute,		/*  WIN1257_Eacute	201	*/
    ISO13_Zacute,		/*  WIN1257_Zacute	202	*/
    ISO13_Edotaccent,		/*  WIN1257_Edotaccent	203	*/
    ISO13_Gcommaaccent,		/*  WIN1257_Gcommaaccent 204	*/
    ISO13_Kcommaaccent,		/*  WIN1257_Kcommaaccent 205	*/
    ISO13_Imacron,		/*  WIN1257_Imacron	206	*/
    ISO13_Lcommaaccent,		/*  WIN1257_Lcommaaccent 207	*/
    ISO13_Scaron,		/*  WIN1257_Scaron	208	*/
    ISO13_Nacute,		/*  WIN1257_Nacute	209	*/
    ISO13_Ncommaaccent,		/*  WIN1257_Ncommaaccent 210	*/
    ISO13_Oacute,		/*  WIN1257_Oacute	211	*/
    ISO13_Omacron,		/*  WIN1257_Omacron	212	*/
    ISO13_Otilde,		/*  WIN1257_Otilde	213	*/
    ISO13_Odieresis,		/*  WIN1257_Odieresis	214	*/
    ISO13_multiply,		/*  WIN1257_multiply	215	*/
    ISO13_Uogonek,		/*  WIN1257_Uogonek	216	*/
    ISO13_Lslash,		/*  WIN1257_Lslash	217	*/
    ISO13_Sacute,		/*  WIN1257_Sacute	218	*/
    ISO13_Umacron,		/*  WIN1257_Umacron	219	*/
    ISO13_Udieresis,		/*  WIN1257_Udieresis	220	*/
    ISO13_Zdotaccent,		/*  WIN1257_Zdotaccent	221	*/
    ISO13_Zcaron,		/*  WIN1257_Zcaron	222	*/
    ISO13_germandbls,		/*  WIN1257_germandbls	223	*/
    ISO13_aogonek,		/*  WIN1257_aogonek	224	*/
    ISO13_iogonek,		/*  WIN1257_iogonek	225	*/
    ISO13_amacron,		/*  WIN1257_amacron	226	*/
    ISO13_cacute,		/*  WIN1257_cacute	227	*/
    ISO13_adieresis,		/*  WIN1257_adieresis	228	*/
    ISO13_aring,		/*  WIN1257_aring	229	*/
    ISO13_eogonek,		/*  WIN1257_eogonek	230	*/
    ISO13_emacron,		/*  WIN1257_emacron	231	*/
    ISO13_ccaron,		/*  WIN1257_ccaron	232	*/
    ISO13_eacute,		/*  WIN1257_eacute	233	*/
    ISO13_zacute,		/*  WIN1257_zacute	234	*/
    ISO13_edotaccent,		/*  WIN1257_edotaccent	235	*/
    ISO13_gcommaaccent,		/*  WIN1257_gcommaaccent 236	*/
    ISO13_kcommaaccent,		/*  WIN1257_kcommaaccent 237	*/
    ISO13_imacron,		/*  WIN1257_imacron	238	*/
    ISO13_lcommaaccent,		/*  WIN1257_lcommaaccent 239	*/
    ISO13_scaron,		/*  WIN1257_scaron	240	*/
    ISO13_nacute,		/*  WIN1257_nacute	241	*/
    ISO13_ncommaaccent,		/*  WIN1257_ncommaaccent 242	*/
    ISO13_oacute,		/*  WIN1257_oacute	243	*/
    ISO13_omacron,		/*  WIN1257_omacron	244	*/
    ISO13_otilde,		/*  WIN1257_otilde	245	*/
    ISO13_odieresis,		/*  WIN1257_odieresis	246	*/
    ISO13_divide,		/*  WIN1257_divide	247	*/
    ISO13_uogonek,		/*  WIN1257_uogonek	248	*/
    ISO13_lslash,		/*  WIN1257_lslash	249	*/
    ISO13_sacute,		/*  WIN1257_sacute	250	*/
    ISO13_umacron,		/*  WIN1257_umacron	251	*/
    ISO13_udieresis,		/*  WIN1257_udieresis	252	*/
    ISO13_zdotaccent,		/*  WIN1257_zdotaccent	253	*/
    ISO13_zcaron,		/*  WIN1257_zcaron	254	*/
    ISO13_currency,		/*  WIN1257_dotaccent	255	*//**/
    };

const unsigned char docISO13_to_WIN1257[256]=
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
    WIN1257_space,		/*  ISO13_space		 32	*/
    WIN1257_exclam,		/*  ISO13_exclam	 33	*/
    WIN1257_quotedbl,		/*  ISO13_quotedbl	 34	*/
    WIN1257_numbersign,		/*  ISO13_numbersign	 35	*/
    WIN1257_dollar,		/*  ISO13_dollar	 36	*/
    WIN1257_percent,		/*  ISO13_percent	 37	*/
    WIN1257_ampersand,		/*  ISO13_ampersand	 38	*/
    WIN1257_quotesingle,	/*  ISO13_quotesingle	 39	*/
    WIN1257_parenleft,		/*  ISO13_parenleft	 40	*/
    WIN1257_parenright,		/*  ISO13_parenright	 41	*/
    WIN1257_asterisk,		/*  ISO13_asterisk	 42	*/
    WIN1257_plus,		/*  ISO13_plus		 43	*/
    WIN1257_comma,		/*  ISO13_comma		 44	*/
    WIN1257_minus,		/*  ISO13_minus		 45	*/
    WIN1257_period,		/*  ISO13_period	 46	*/
    WIN1257_slash,		/*  ISO13_slash		 47	*/
    WIN1257_zero,		/*  ISO13_zero		 48	*/
    WIN1257_one,		/*  ISO13_one		 49	*/
    WIN1257_two,		/*  ISO13_two		 50	*/
    WIN1257_three,		/*  ISO13_three		 51	*/
    WIN1257_four,		/*  ISO13_four		 52	*/
    WIN1257_five,		/*  ISO13_five		 53	*/
    WIN1257_six,		/*  ISO13_six		 54	*/
    WIN1257_seven,		/*  ISO13_seven		 55	*/
    WIN1257_eight,		/*  ISO13_eight		 56	*/
    WIN1257_nine,		/*  ISO13_nine		 57	*/
    WIN1257_colon,		/*  ISO13_colon		 58	*/
    WIN1257_semicolon,		/*  ISO13_semicolon	 59	*/
    WIN1257_less,		/*  ISO13_less		 60	*/
    WIN1257_equal,		/*  ISO13_equal		 61	*/
    WIN1257_greater,		/*  ISO13_greater	 62	*/
    WIN1257_question,		/*  ISO13_question	 63	*/
    WIN1257_at,			/*  ISO13_at		 64	*/
    WIN1257_A,			/*  ISO13_A		 65	*/
    WIN1257_B,			/*  ISO13_B		 66	*/
    WIN1257_C,			/*  ISO13_C		 67	*/
    WIN1257_D,			/*  ISO13_D		 68	*/
    WIN1257_E,			/*  ISO13_E		 69	*/
    WIN1257_F,			/*  ISO13_F		 70	*/
    WIN1257_G,			/*  ISO13_G		 71	*/
    WIN1257_H,			/*  ISO13_H		 72	*/
    WIN1257_I,			/*  ISO13_I		 73	*/
    WIN1257_J,			/*  ISO13_J		 74	*/
    WIN1257_K,			/*  ISO13_K		 75	*/
    WIN1257_L,			/*  ISO13_L		 76	*/
    WIN1257_M,			/*  ISO13_M		 77	*/
    WIN1257_N,			/*  ISO13_N		 78	*/
    WIN1257_O,			/*  ISO13_O		 79	*/
    WIN1257_P,			/*  ISO13_P		 80	*/
    WIN1257_Q,			/*  ISO13_Q		 81	*/
    WIN1257_R,			/*  ISO13_R		 82	*/
    WIN1257_S,			/*  ISO13_S		 83	*/
    WIN1257_T,			/*  ISO13_T		 84	*/
    WIN1257_U,			/*  ISO13_U		 85	*/
    WIN1257_V,			/*  ISO13_V		 86	*/
    WIN1257_W,			/*  ISO13_W		 87	*/
    WIN1257_X,			/*  ISO13_X		 88	*/
    WIN1257_Y,			/*  ISO13_Y		 89	*/
    WIN1257_Z,			/*  ISO13_Z		 90	*/
    WIN1257_bracketleft,	/*  ISO13_bracketleft	 91	*/
    WIN1257_backslash,		/*  ISO13_backslash	 92	*/
    WIN1257_bracketright,	/*  ISO13_bracketright	 93	*/
    WIN1257_asciicircum,	/*  ISO13_asciicircum	 94	*/
    WIN1257_underscore,		/*  ISO13_underscore	 95	*/
    WIN1257_quoteleft,		/*  ISO13_quoteleft	 96	*/
    WIN1257_a,			/*  ISO13_a		 97	*/
    WIN1257_b,			/*  ISO13_b		 98	*/
    WIN1257_c,			/*  ISO13_c		 99	*/
    WIN1257_d,			/*  ISO13_d		100	*/
    WIN1257_e,			/*  ISO13_e		101	*/
    WIN1257_f,			/*  ISO13_f		102	*/
    WIN1257_g,			/*  ISO13_g		103	*/
    WIN1257_h,			/*  ISO13_h		104	*/
    WIN1257_i,			/*  ISO13_i		105	*/
    WIN1257_j,			/*  ISO13_j		106	*/
    WIN1257_k,			/*  ISO13_k		107	*/
    WIN1257_l,			/*  ISO13_l		108	*/
    WIN1257_m,			/*  ISO13_m		109	*/
    WIN1257_n,			/*  ISO13_n		110	*/
    WIN1257_o,			/*  ISO13_o		111	*/
    WIN1257_p,			/*  ISO13_p		112	*/
    WIN1257_q,			/*  ISO13_q		113	*/
    WIN1257_r,			/*  ISO13_r		114	*/
    WIN1257_s,			/*  ISO13_s		115	*/
    WIN1257_t,			/*  ISO13_t		116	*/
    WIN1257_u,			/*  ISO13_u		117	*/
    WIN1257_v,			/*  ISO13_v		118	*/
    WIN1257_w,			/*  ISO13_w		119	*/
    WIN1257_x,			/*  ISO13_x		120	*/
    WIN1257_y,			/*  ISO13_y		121	*/
    WIN1257_z,			/*  ISO13_z		122	*/
    WIN1257_braceleft,		/*  ISO13_braceleft	123	*/
    WIN1257_bar,		/*  ISO13_bar		124	*/
    WIN1257_braceright,		/*  ISO13_braceright	125	*/
    WIN1257_asciitilde,		/*  ISO13_asciitilde	126	*/
    WIN1257__0x7F,		/*  ISO13__0x7F		127	*/
    WIN1257__0x80,		/*  ISO13__0x80		128	*/
    WIN1257__0x81,		/*  ISO13__0x81		129	*/
    WIN1257_asterisk,		/*  ISO13__0x82		130	*//**/
    WIN1257__0x83,		/*  ISO13__0x83		131	*/
    WIN1257_asterisk,		/*  ISO13__0x84		132	*//**/
    WIN1257_asterisk,		/*  ISO13__0x85		133	*//**/
    WIN1257_asterisk,		/*  ISO13__0x86		134	*//**/
    WIN1257_asterisk,		/*  ISO13__0x87		135	*//**/
    WIN1257__0x88,		/*  ISO13__0x88		136	*/
    WIN1257_asterisk,		/*  ISO13__0x89		137	*//**/
    WIN1257__0x8A,		/*  ISO13__0x8A		138	*/
    WIN1257_asterisk,		/*  ISO13__0x8B		139	*//**/
    WIN1257__0x8C,		/*  ISO13__0x8C		140	*/
    WIN1257_asterisk,		/*  ISO13__0x8D		141	*//**/
    WIN1257_asterisk,		/*  ISO13__0x8E		142	*//**/
    WIN1257_asterisk,		/*  ISO13__0x8F		143	*//**/
    WIN1257__0x90,		/*  ISO13__0x90		144	*/
    WIN1257_asterisk,		/*  ISO13__0x91		145	*//**/
    WIN1257_asterisk,		/*  ISO13__0x92		146	*//**/
    WIN1257_asterisk,		/*  ISO13__0x93		147	*//**/
    WIN1257_asterisk,		/*  ISO13__0x94		148	*//**/
    WIN1257_asterisk,		/*  ISO13__0x95		149	*//**/
    WIN1257_asterisk,		/*  ISO13__0x96		150	*//**/
    WIN1257_asterisk,		/*  ISO13__0x97		151	*//**/
    WIN1257__0x98,		/*  ISO13__0x98		152	*/
    WIN1257_asterisk,		/*  ISO13__0x99		153	*//**/
    WIN1257__0x9A,		/*  ISO13__0x9A		154	*/
    WIN1257_asterisk,		/*  ISO13__0x9B		155	*//**/
    WIN1257__0x9C,		/*  ISO13__0x9C		156	*/
    WIN1257_asterisk,		/*  ISO13__0x9D		157	*//**/
    WIN1257_asterisk,		/*  ISO13__0x9E		158	*//**/
    WIN1257__0x9F,		/*  ISO13__0x9F		159	*/
    WIN1257_nobreakspace,	/*  ISO13_space		160	*/
    WIN1257_quotedblright,	/*  ISO13_quotedblright	161	*/
    WIN1257_cent,		/*  ISO13_cent		162	*/
    WIN1257_sterling,		/*  ISO13_sterling	163	*/
    WIN1257_currency,		/*  ISO13_currency	164	*/
    WIN1257_quotedblbase,	/*  ISO13_quotedblbase	165	*/
    WIN1257_brokenbar,		/*  ISO13_brokenbar	166	*/
    WIN1257_section,		/*  ISO13_section	167	*/
    WIN1257_Oslash,		/*  ISO13_Oslash	168	*/
    WIN1257_copyright,		/*  ISO13_copyright	169	*/
    WIN1257_Rcommaaccent,	/*  ISO13_Rcommaaccent	170	*/
    WIN1257_guillemotleft,	/*  ISO13_guillemotleft	171	*/
    WIN1257_logicalnot,		/*  ISO13_logicalnot	172	*/
    WIN1257_hyphen,		/*  ISO13_hyphen	173	*/
    WIN1257_registered,		/*  ISO13_registered	174	*/
    WIN1257_AE,			/*  ISO13_AE		175	*/
    WIN1257_degree,		/*  ISO13_degree	176	*/
    WIN1257_plusminus,		/*  ISO13_plusminus	177	*/
    WIN1257_twosuperior,	/*  ISO13_twosuperior	178	*/
    WIN1257_threesuperior,	/*  ISO13_threesuperior	179	*/
    WIN1257_quotedblleft,	/*  ISO13_quotedblleft	180	*/
    WIN1257_mu,			/*  ISO13_mu		181	*/
    WIN1257_paragraph,		/*  ISO13_paragraph	182	*/
    WIN1257_periodcentered,	/*  ISO13_periodcentered 183	*/
    WIN1257_oslash,		/*  ISO13_oslash	184	*/
    WIN1257_onesuperior,	/*  ISO13_onesuperior	185	*/
    WIN1257_rcommaaccent,	/*  ISO13_rcommaaccent	186	*/
    WIN1257_guillemotright,	/*  ISO13_guillemotright 187	*/
    WIN1257_onequarter,		/*  ISO13_onequarter	188	*/
    WIN1257_onehalf,		/*  ISO13_onehalf	189	*/
    WIN1257_threequarters,	/*  ISO13_threequarters	190	*/
    WIN1257_ae,			/*  ISO13_ae		191	*/
    WIN1257_Aogonek,		/*  ISO13_Aogonek	192	*/
    WIN1257_Iogonek,		/*  ISO13_Iogonek	193	*/
    WIN1257_Amacron,		/*  ISO13_Amacron	194	*/
    WIN1257_Cacute,		/*  ISO13_Cacute	195	*/
    WIN1257_Adieresis,		/*  ISO13_Adieresis	196	*/
    WIN1257_Aring,		/*  ISO13_Aring		197	*/
    WIN1257_Eogonek,		/*  ISO13_Eogonek	198	*/
    WIN1257_Emacron,		/*  ISO13_Emacron	199	*/
    WIN1257_Ccaron,		/*  ISO13_Ccaron	200	*/
    WIN1257_Eacute,		/*  ISO13_Eacute	201	*/
    WIN1257_Zacute,		/*  ISO13_Zacute	202	*/
    WIN1257_Edotaccent,		/*  ISO13_Edotaccent	203	*/
    WIN1257_Gcommaaccent,	/*  ISO13_Gcommaaccent	204	*/
    WIN1257_Kcommaaccent,	/*  ISO13_Kcommaaccent	205	*/
    WIN1257_Imacron,		/*  ISO13_Imacron	206	*/
    WIN1257_Lcommaaccent,	/*  ISO13_Lcommaaccent	207	*/
    WIN1257_Scaron,		/*  ISO13_Scaron	208	*/
    WIN1257_Nacute,		/*  ISO13_Nacute	209	*/
    WIN1257_Ncommaaccent,	/*  ISO13_Ncommaaccent	210	*/
    WIN1257_Oacute,		/*  ISO13_Oacute	211	*/
    WIN1257_Omacron,		/*  ISO13_Omacron	212	*/
    WIN1257_Otilde,		/*  ISO13_Otilde	213	*/
    WIN1257_Odieresis,		/*  ISO13_Odieresis	214	*/
    WIN1257_multiply,		/*  ISO13_multiply	215	*/
    WIN1257_Uogonek,		/*  ISO13_Uogonek	216	*/
    WIN1257_Lslash,		/*  ISO13_Lslash	217	*/
    WIN1257_Sacute,		/*  ISO13_Sacute	218	*/
    WIN1257_Umacron,		/*  ISO13_Umacron	219	*/
    WIN1257_Udieresis,		/*  ISO13_Udieresis	220	*/
    WIN1257_Zdotaccent,		/*  ISO13_Zdotaccent	221	*/
    WIN1257_Zcaron,		/*  ISO13_Zcaron	222	*/
    WIN1257_germandbls,		/*  ISO13_germandbls	223	*/
    WIN1257_aogonek,		/*  ISO13_aogonek	224	*/
    WIN1257_iogonek,		/*  ISO13_iogonek	225	*/
    WIN1257_amacron,		/*  ISO13_amacron	226	*/
    WIN1257_cacute,		/*  ISO13_cacute	227	*/
    WIN1257_adieresis,		/*  ISO13_adieresis	228	*/
    WIN1257_aring,		/*  ISO13_aring		229	*/
    WIN1257_eogonek,		/*  ISO13_eogonek	230	*/
    WIN1257_emacron,		/*  ISO13_emacron	231	*/
    WIN1257_ccaron,		/*  ISO13_ccaron	232	*/
    WIN1257_eacute,		/*  ISO13_eacute	233	*/
    WIN1257_zacute,		/*  ISO13_zacute	234	*/
    WIN1257_edotaccent,		/*  ISO13_edotaccent	235	*/
    WIN1257_gcommaaccent,	/*  ISO13_gcommaaccent	236	*/
    WIN1257_kcommaaccent,	/*  ISO13_kcommaaccent	237	*/
    WIN1257_imacron,		/*  ISO13_imacron	238	*/
    WIN1257_lcommaaccent,	/*  ISO13_lcommaaccent	239	*/
    WIN1257_scaron,		/*  ISO13_scaron	240	*/
    WIN1257_nacute,		/*  ISO13_nacute	241	*/
    WIN1257_ncommaaccent,	/*  ISO13_ncommaaccent	242	*/
    WIN1257_oacute,		/*  ISO13_oacute	243	*/
    WIN1257_omacron,		/*  ISO13_omacron	244	*/
    WIN1257_otilde,		/*  ISO13_otilde	245	*/
    WIN1257_odieresis,		/*  ISO13_odieresis	246	*/
    WIN1257_divide,		/*  ISO13_divide	247	*/
    WIN1257_uogonek,		/*  ISO13_uogonek	248	*/
    WIN1257_lslash,		/*  ISO13_lslash	249	*/
    WIN1257_sacute,		/*  ISO13_sacute	250	*/
    WIN1257_umacron,		/*  ISO13_umacron	251	*/
    WIN1257_udieresis,		/*  ISO13_udieresis	252	*/
    WIN1257_zdotaccent,		/*  ISO13_zdotaccent	253	*/
    WIN1257_zcaron,		/*  ISO13_zcaron	254	*/
    WIN1257_quoteright,		/*  ISO13_quoteright	255	*/
    };

