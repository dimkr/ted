/************************************************************************/
/*									*/
/*  Map Font Encodings							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

/************************************************************************/
/*									*/
/*  Character set mapping tables.					*/
/*									*/
/************************************************************************/

const unsigned char	docDOS437_to_ISO1[256]=
    {
    0x00,		/*  			  0	*/
    0x01,		/*  			  1	*/
    0x02,		/*  			  2	*/
    0x03,		/*  			  3	*/
    0x04,		/*  			  4	*/
    0x05,		/*  			  5	*/
    0x06,		/*  			  6	*/
    0x07,		/*  			  7	*/
    0x08,		/*  			  8	*/
    0x09,		/*  			  9	*/
    0x0a,		/*  			 10	*/
    0x0b,		/*  			 11	*/
    0x0c,		/*  			 12	*/
    0x0d,		/*  			 13	*/
    0x0e,		/*  			 14	*/
    0x0f,		/*  			 15	*/
    0x10,		/*  			 16	*/
    0x11,		/*  			 17	*/
    0x12,		/*  			 18	*/
    0x13,		/*  			 19	*/
    0x14,		/*  			 20	*/
    0x15,		/*  			 21	*/
    0x16,		/*  			 22	*/
    0x17,		/*  			 23	*/
    0x18,		/*  			 24	*/
    0x19,		/*  			 25	*/
    0x1a,		/*  			 26	*/
    0x1b,		/*  			 27	*/
    0x1c,		/*  			 28	*/
    0x1d,		/*  			 29	*/
    0x1e,		/*  			 30	*/
    0x1f,		/*  			 31	*/
    ISO1_space,		/*  DOS437_space	 32	*/
    ISO1_exclam,	/*  DOS437_exclam	 33	*/
    ISO1_quotedbl,	/*  DOS437_quotedbl	 34	*/
    ISO1_numbersign,	/*  DOS437_numbersign	 35	*/
    ISO1_dollar,	/*  DOS437_dollar	 36	*/
    ISO1_percent,	/*  DOS437_percent	 37	*/
    ISO1_ampersand,	/*  DOS437_ampersand	 38	*/
    ISO1_quotesingle,	/*  DOS437_quoteright	 39	*/
    ISO1_parenleft,	/*  DOS437_parenleft	 40	*/
    ISO1_parenright,	/*  DOS437_parenright	 41	*/
    ISO1_asterisk,	/*  DOS437_asterisk	 42	*/
    ISO1_plus,		/*  DOS437_plus		 43	*/
    ISO1_comma,		/*  DOS437_comma	 44	*/
    ISO1_minus,		/*  DOS437_minus	 45	*/
    ISO1_period,	/*  DOS437_period	 46	*/
    ISO1_slash,		/*  DOS437_slash	 47	*/
    ISO1_zero,		/*  DOS437_zero		 48	*/
    ISO1_one,		/*  DOS437_one		 49	*/
    ISO1_two,		/*  DOS437_two		 50	*/
    ISO1_three,		/*  DOS437_three	 51	*/
    ISO1_four,		/*  DOS437_four		 52	*/
    ISO1_five,		/*  DOS437_five		 53	*/
    ISO1_six,		/*  DOS437_six		 54	*/
    ISO1_seven,		/*  DOS437_seven	 55	*/
    ISO1_eight,		/*  DOS437_eight	 56	*/
    ISO1_nine,		/*  DOS437_nine		 57	*/
    ISO1_colon,		/*  DOS437_colon	 58	*/
    ISO1_semicolon,	/*  DOS437_semicolon	 59	*/
    ISO1_less,		/*  DOS437_less		 60	*/
    ISO1_equal,		/*  DOS437_equal	 61	*/
    ISO1_greater,	/*  DOS437_greater	 62	*/
    ISO1_question,	/*  DOS437_question	 63	*/
    ISO1_at,		/*  DOS437_at		 64	*/
    ISO1_A,		/*  DOS437_A		 65	*/
    ISO1_B,		/*  DOS437_B		 66	*/
    ISO1_C,		/*  DOS437_C		 67	*/
    ISO1_D,		/*  DOS437_D		 68	*/
    ISO1_E,		/*  DOS437_E		 69	*/
    ISO1_F,		/*  DOS437_F		 70	*/
    ISO1_G,		/*  DOS437_G		 71	*/
    ISO1_H,		/*  DOS437_H		 72	*/
    ISO1_I,		/*  DOS437_I		 73	*/
    ISO1_J,		/*  DOS437_J		 74	*/
    ISO1_K,		/*  DOS437_K		 75	*/
    ISO1_L,		/*  DOS437_L		 76	*/
    ISO1_M,		/*  DOS437_M		 77	*/
    ISO1_N,		/*  DOS437_N		 78	*/
    ISO1_O,		/*  DOS437_O		 79	*/
    ISO1_P,		/*  DOS437_P		 80	*/
    ISO1_Q,		/*  DOS437_Q		 81	*/
    ISO1_R,		/*  DOS437_R		 82	*/
    ISO1_S,		/*  DOS437_S		 83	*/
    ISO1_T,		/*  DOS437_T		 84	*/
    ISO1_U,		/*  DOS437_U		 85	*/
    ISO1_V,		/*  DOS437_V		 86	*/
    ISO1_W,		/*  DOS437_W		 87	*/
    ISO1_X,		/*  DOS437_X		 88	*/
    ISO1_Y,		/*  DOS437_Y		 89	*/
    ISO1_Z,		/*  DOS437_Z		 90	*/
    ISO1_bracketleft,	/*  DOS437_bracketleft	 91	*/
    ISO1_backslash,	/*  DOS437_backslash	 92	*/
    ISO1_bracketright,	/*  DOS437_bracketright	 93	*/
    ISO1_circumflex,	/*  DOS437_circumflex	 94	*/
    ISO1_underscore,	/*  DOS437_underscore	 95	*/
    ISO1_quoteleft,	/*  DOS437_quoteleft	 96	*/
    ISO1_a,		/*  DOS437_a		 97	*/
    ISO1_b,		/*  DOS437_b		 98	*/
    ISO1_c,		/*  DOS437_c		 99	*/
    ISO1_d,		/*  DOS437_d		100	*/
    ISO1_e,		/*  DOS437_e		101	*/
    ISO1_f,		/*  DOS437_f		102	*/
    ISO1_g,		/*  DOS437_g		103	*/
    ISO1_h,		/*  DOS437_h		104	*/
    ISO1_i,		/*  DOS437_i		105	*/
    ISO1_j,		/*  DOS437_j		106	*/
    ISO1_k,		/*  DOS437_k		107	*/
    ISO1_l,		/*  DOS437_l		108	*/
    ISO1_m,		/*  DOS437_m		109	*/
    ISO1_n,		/*  DOS437_n		110	*/
    ISO1_o,		/*  DOS437_o		111	*/
    ISO1_p,		/*  DOS437_p		112	*/
    ISO1_q,		/*  DOS437_q		113	*/
    ISO1_r,		/*  DOS437_r		114	*/
    ISO1_s,		/*  DOS437_s		115	*/
    ISO1_t,		/*  DOS437_t		116	*/
    ISO1_u,		/*  DOS437_u		117	*/
    ISO1_v,		/*  DOS437_v		118	*/
    ISO1_w,		/*  DOS437_w		119	*/
    ISO1_x,		/*  DOS437_x		120	*/
    ISO1_y,		/*  DOS437_y		121	*/
    ISO1_z,		/*  DOS437_z		122	*/
    ISO1_braceleft,	/*  DOS437_braceleft	123	*/
    ISO1_bar,		/*  DOS437_bar		124	*/
    ISO1_braceright,	/*  DOS437_braceright	125	*/
    ISO1_tilde,		/*  DOS437_tilde	126	*/
    0x7f,		/*  0x7f		127	*/
    ISO1_Ccedilla,	/*  DOS437_Ccedilla	128	*/
    ISO1_udieresis,	/*  DOS437_udieresis	129	*/
    ISO1_eacute,	/*  DOS437_eacute	130	*/
    ISO1_acircumflex,	/*  DOS437_acircumflex	131	*/
    ISO1_adieresis,	/*  DOS437_adieresis	132	*/
    ISO1_agrave,	/*  DOS437_agrave	133	*/
    ISO1_aring,		/*  DOS437_aring	134	*/
    ISO1_ccedilla,	/*  DOS437_ccedilla	135	*/
    ISO1_ecircumflex,	/*  DOS437_ecircumflex	136	*/
    ISO1_edieresis,	/*  DOS437_edieresis	137	*/
    ISO1_egrave,	/*  DOS437_egrave	138	*/
    ISO1_idieresis,	/*  DOS437_idieresis	139	*/
    ISO1_icircumflex,	/*  DOS437_icircumflex	140	*/
    ISO1_igrave,	/*  DOS437_igrave	141	*/
    ISO1_Adieresis,	/*  DOS437_Adieresis	142	*/
    ISO1_Aring,		/*  DOS437_Aring	143	*/
    ISO1_Eacute,	/*  DOS437_Eacute	144	*/
    ISO1_ae,		/*  DOS437_ae		145	*/
    ISO1_AE,		/*  DOS437_AE		146	*/
    ISO1_ocircumflex,	/*  DOS437_ocircumflex	147	*/
    ISO1_odieresis,	/*  DOS437_odieresis	148	*/
    ISO1_ograve,	/*  DOS437_ograve	149	*/
    ISO1_ucircumflex,	/*  DOS437_ucircumflex	150	*/
    ISO1_ugrave,	/*  DOS437_ugrave	151	*/
    ISO1_ydieresis,	/*  DOS437_ydieresis	152	*/
    ISO1_Odieresis,	/*  DOS437_Odieresis	153	*/
    ISO1_Udieresis,	/*  DOS437_Udieresis	154	*/
    ISO1_cent,		/*  DOS437_cent		155	*/
    ISO1_sterling,	/*  DOS437_sterling	156	*/
    ISO1_yen,		/*  DOS437_yen		157	*/
    ISO1_currency,	/*  DOS437_peseta	158	*//**/
    ISO1_currency,	/*  DOS437_florin	159	*//**/
    ISO1_aacute,	/*  DOS437_aacute	160	*/
    ISO1_iacute,	/*  DOS437_iacute	161	*/
    ISO1_oacute,	/*  DOS437_oacute	162	*/
    ISO1_uacute,	/*  DOS437_uacute	163	*/
    ISO1_ntilde,	/*  DOS437_ntilde	164	*/
    ISO1_Ntilde,	/*  DOS437_Ntilde	165	*/
    ISO1_ordfeminine,	/*  DOS437_ordfeminine	166	*/
    ISO1_ordmasculine,	/*  DOS437_ordmasculine	167	*/
    ISO1_questiondown,	/*  DOS437_questiondown	168	*/
    ISO1_currency,	/*  DOS437_revlogicalnot 169	*//**/
    ISO1_logicalnot,	/*  DOS437_logicalnot	170	*/
    ISO1_onehalf,	/*  DOS437_onehalf	171	*/
    ISO1_onequarter,	/*  DOS437_onequarter	172	*/
    ISO1_exclamdown,	/*  DOS437_exclamdown	173	*/
    ISO1_guillemotleft,	/*  DOS437_guillemotleft 174	*/
    ISO1_guillemotright,/*  DOS437_guillemotright 175	*/
    ISO1_currency,	/*  DOS437_ltshade	176	*//**/
    ISO1_currency,	/*  DOS437_shade	177	*//**/
    ISO1_currency,	/*  DOS437_dkshade	178	*//**/
    ISO1_currency,	/*  DOS437_SF110000	179	*//**/
    ISO1_currency,	/*  DOS437_SF090000	180	*//**/
    ISO1_currency,	/*  DOS437_SF190000	181	*//**/
    ISO1_currency,	/*  DOS437_SF200000	182	*//**/
    ISO1_currency,	/*  DOS437_SF210000	183	*//**/
    ISO1_currency,	/*  DOS437_SF220000	184	*//**/
    ISO1_currency,	/*  DOS437_SF230000	185	*//**/
    ISO1_currency,	/*  DOS437_SF240000	186	*//**/
    ISO1_currency,	/*  DOS437_SF250000	187	*//**/
    ISO1_currency,	/*  DOS437_SF260000	188	*//**/
    ISO1_currency,	/*  DOS437_SF270000	189	*//**/
    ISO1_currency,	/*  DOS437_SF280000	190	*//**/
    ISO1_currency,	/*  DOS437_SF030000	191	*//**/
    ISO1_currency,	/*  DOS437_SF020000	192	*//**/
    ISO1_currency,	/*  DOS437_SF070000	193	*//**/
    ISO1_currency,	/*  DOS437_SF060000	194	*//**/
    ISO1_currency,	/*  DOS437_SF080000	195	*//**/
    ISO1_currency,	/*  DOS437_SF100000	196	*//**/
    ISO1_currency,	/*  DOS437_SF050000	197	*//**/
    ISO1_currency,	/*  DOS437_SF360000	198	*//**/
    ISO1_currency,	/*  DOS437_SF370000	199	*//**/
    ISO1_currency,	/*  DOS437_SF380000	200	*//**/
    ISO1_currency,	/*  DOS437_SF390000	201	*//**/
    ISO1_currency,	/*  DOS437_SF400000	202	*//**/
    ISO1_currency,	/*  DOS437_SF410000	203	*//**/
    ISO1_currency,	/*  DOS437_SF420000	204	*//**/
    ISO1_currency,	/*  DOS437_SF430000	205	*//**/
    ISO1_currency,	/*  DOS437_SF440000	206	*//**/
    ISO1_currency,	/*  DOS437_SF450000	207	*//**/
    ISO1_currency,	/*  DOS437_SF460000	208	*//**/
    ISO1_currency,	/*  DOS437_SF470000	209	*//**/
    ISO1_currency,	/*  DOS437_SF480000	210	*//**/
    ISO1_currency,	/*  DOS437_SF490000	211	*//**/
    ISO1_currency,	/*  DOS437_SF500000	212	*//**/
    ISO1_currency,	/*  DOS437_SF510000	213	*//**/
    ISO1_currency,	/*  DOS437_SF520000	214	*//**/
    ISO1_currency,	/*  DOS437_SF530000	215	*//**/
    ISO1_currency,	/*  DOS437_SF540000	216	*//**/
    ISO1_currency,	/*  DOS437_SF040000	217	*//**/
    ISO1_currency,	/*  DOS437_SF010000	218	*//**/
    ISO1_currency,	/*  DOS437_block	219	*//**/
    ISO1_currency,	/*  DOS437_dnblock	220	*//**/
    ISO1_currency,	/*  DOS437_lfblock	221	*//**/
    ISO1_currency,	/*  DOS437_rtblock	222	*//**/
    ISO1_currency,	/*  DOS437_upblock	223	*//**/
    ISO1_currency,	/*  DOS437_alpha	224	*//**/
    ISO1_germandbls,	/*  DOS437_germandbls	225	*/
    ISO1_currency,	/*  DOS437_Gamma	226	*//**/
    ISO1_currency,	/*  DOS437_pi		227	*//**/
    ISO1_currency,	/*  DOS437_Sigma	228	*//**/
    ISO1_currency,	/*  DOS437_sigma	229	*//**/
    ISO1_mu,		/*  DOS437_mu		230	*/
    ISO1_currency,	/*  DOS437_tau		231	*//**/
    ISO1_currency,	/*  DOS437_Phi		232	*//**/
    ISO1_currency,	/*  DOS437_Theta	233	*//**/
    ISO1_currency,	/*  DOS437_Omega	234	*//**/
    ISO1_currency,	/*  DOS437_delta	235	*//**/
    ISO1_currency,	/*  DOS437_infinity	236	*//**/
    ISO1_currency,	/*  DOS437_phi		237	*//**/
    ISO1_currency,	/*  DOS437_epsilon	238	*//**/
    ISO1_currency,	/*  DOS437_intersection	239	*//**/
    ISO1_currency,	/*  DOS437_equivalence	240	*//**/
    ISO1_plusminus,	/*  DOS437_plusminus	241	*/
    ISO1_currency,	/*  DOS437_greaterequal	242	*//**/
    ISO1_currency,	/*  DOS437_lessequal	243	*//**/
    ISO1_currency,	/*  DOS437_integraltp	244	*//**/
    ISO1_currency,	/*  DOS437_integralbt	245	*//**/
    ISO1_divide,	/*  DOS437_divide	246	*/
    ISO1_currency,	/*  DOS437_approxequal	247	*/
    ISO1_degree,	/*  DOS437_degree	248	*/
    ISO1_periodcentered,/*  DOS437_bullet	249	*//**/
    ISO1_periodcentered,/*  DOS437_periodcentered 250	*/
    ISO1_currency,	/*  DOS437_radical	251	*//**/
    ISO1_currency,	/*  DOS437_nsuperior	252	*//**/
    ISO1_twosuperior,	/*  DOS437_twosuperior	253	*/
    ISO1_currency,	/*  DOS437_filledbox	254	*//**/
    ISO1_nobreakspace,	/*  DOS437_nobreakspace	255	*/
    };

const unsigned char	docDOS850_to_ISO1[256]=
    {
    0x00,		/*  			  0	*/
    0x01,		/*  			  1	*/
    0x02,		/*  			  2	*/
    0x03,		/*  			  3	*/
    0x04,		/*  			  4	*/
    0x05,		/*  			  5	*/
    0x06,		/*  			  6	*/
    0x07,		/*  			  7	*/
    0x08,		/*  			  8	*/
    0x09,		/*  			  9	*/
    0x0a,		/*  			 10	*/
    0x0b,		/*  			 11	*/
    0x0c,		/*  			 12	*/
    0x0d,		/*  			 13	*/
    0x0e,		/*  			 14	*/
    0x0f,		/*  			 15	*/
    0x10,		/*  			 16	*/
    0x11,		/*  			 17	*/
    0x12,		/*  			 18	*/
    0x13,		/*  			 19	*/
    0x14,		/*  			 20	*/
    0x15,		/*  			 21	*/
    0x16,		/*  			 22	*/
    0x17,		/*  			 23	*/
    0x18,		/*  			 24	*/
    0x19,		/*  			 25	*/
    0x1a,		/*  			 26	*/
    0x1b,		/*  			 27	*/
    0x1c,		/*  			 28	*/
    0x1d,		/*  			 29	*/
    0x1e,		/*  			 30	*/
    0x1f,		/*  			 31	*/
    ISO1_space,		/*  DOS850_space	 32	*/
    ISO1_exclam,	/*  DOS850_exclam	 33	*/
    ISO1_quotedbl,	/*  DOS850_quotedbl	 34	*/
    ISO1_numbersign,	/*  DOS850_numbersign	 35	*/
    ISO1_dollar,	/*  DOS850_dollar	 36	*/
    ISO1_percent,	/*  DOS850_percent	 37	*/
    ISO1_ampersand,	/*  DOS850_ampersand	 38	*/
    ISO1_quotesingle,	/*  DOS850_quoteright	 39	*/
    ISO1_parenleft,	/*  DOS850_parenleft	 40	*/
    ISO1_parenright,	/*  DOS850_parenright	 41	*/
    ISO1_asterisk,	/*  DOS850_asterisk	 42	*/
    ISO1_plus,		/*  DOS850_plus		 43	*/
    ISO1_comma,		/*  DOS850_comma	 44	*/
    ISO1_minus,		/*  DOS850_minus	 45	*/
    ISO1_period,	/*  DOS850_period	 46	*/
    ISO1_slash,		/*  DOS850_slash	 47	*/
    ISO1_zero,		/*  DOS850_zero		 48	*/
    ISO1_one,		/*  DOS850_one		 49	*/
    ISO1_two,		/*  DOS850_two		 50	*/
    ISO1_three,		/*  DOS850_three	 51	*/
    ISO1_four,		/*  DOS850_four		 52	*/
    ISO1_five,		/*  DOS850_five		 53	*/
    ISO1_six,		/*  DOS850_six		 54	*/
    ISO1_seven,		/*  DOS850_seven	 55	*/
    ISO1_eight,		/*  DOS850_eight	 56	*/
    ISO1_nine,		/*  DOS850_nine		 57	*/
    ISO1_colon,		/*  DOS850_colon	 58	*/
    ISO1_semicolon,	/*  DOS850_semicolon	 59	*/
    ISO1_less,		/*  DOS850_less		 60	*/
    ISO1_equal,		/*  DOS850_equal	 61	*/
    ISO1_greater,	/*  DOS850_greater	 62	*/
    ISO1_question,	/*  DOS850_question	 63	*/
    ISO1_at,		/*  DOS850_at		 64	*/
    ISO1_A,		/*  DOS850_A		 65	*/
    ISO1_B,		/*  DOS850_B		 66	*/
    ISO1_C,		/*  DOS850_C		 67	*/
    ISO1_D,		/*  DOS850_D		 68	*/
    ISO1_E,		/*  DOS850_E		 69	*/
    ISO1_F,		/*  DOS850_F		 70	*/
    ISO1_G,		/*  DOS850_G		 71	*/
    ISO1_H,		/*  DOS850_H		 72	*/
    ISO1_I,		/*  DOS850_I		 73	*/
    ISO1_J,		/*  DOS850_J		 74	*/
    ISO1_K,		/*  DOS850_K		 75	*/
    ISO1_L,		/*  DOS850_L		 76	*/
    ISO1_M,		/*  DOS850_M		 77	*/
    ISO1_N,		/*  DOS850_N		 78	*/
    ISO1_O,		/*  DOS850_O		 79	*/
    ISO1_P,		/*  DOS850_P		 80	*/
    ISO1_Q,		/*  DOS850_Q		 81	*/
    ISO1_R,		/*  DOS850_R		 82	*/
    ISO1_S,		/*  DOS850_S		 83	*/
    ISO1_T,		/*  DOS850_T		 84	*/
    ISO1_U,		/*  DOS850_U		 85	*/
    ISO1_V,		/*  DOS850_V		 86	*/
    ISO1_W,		/*  DOS850_W		 87	*/
    ISO1_X,		/*  DOS850_X		 88	*/
    ISO1_Y,		/*  DOS850_Y		 89	*/
    ISO1_Z,		/*  DOS850_Z		 90	*/
    ISO1_bracketleft,	/*  DOS850_bracketleft	 91	*/
    ISO1_backslash,	/*  DOS850_backslash	 92	*/
    ISO1_bracketright,	/*  DOS850_bracketright	 93	*/
    ISO1_circumflex,	/*  DOS850_circumflex	 94	*/
    ISO1_underscore,	/*  DOS850_underscore	 95	*/
    ISO1_quoteleft,	/*  DOS850_quoteleft	 96	*/
    ISO1_a,		/*  DOS850_a		 97	*/
    ISO1_b,		/*  DOS850_b		 98	*/
    ISO1_c,		/*  DOS850_c		 99	*/
    ISO1_d,		/*  DOS850_d		100	*/
    ISO1_e,		/*  DOS850_e		101	*/
    ISO1_f,		/*  DOS850_f		102	*/
    ISO1_g,		/*  DOS850_g		103	*/
    ISO1_h,		/*  DOS850_h		104	*/
    ISO1_i,		/*  DOS850_i		105	*/
    ISO1_j,		/*  DOS850_j		106	*/
    ISO1_k,		/*  DOS850_k		107	*/
    ISO1_l,		/*  DOS850_l		108	*/
    ISO1_m,		/*  DOS850_m		109	*/
    ISO1_n,		/*  DOS850_n		110	*/
    ISO1_o,		/*  DOS850_o		111	*/
    ISO1_p,		/*  DOS850_p		112	*/
    ISO1_q,		/*  DOS850_q		113	*/
    ISO1_r,		/*  DOS850_r		114	*/
    ISO1_s,		/*  DOS850_s		115	*/
    ISO1_t,		/*  DOS850_t		116	*/
    ISO1_u,		/*  DOS850_u		117	*/
    ISO1_v,		/*  DOS850_v		118	*/
    ISO1_w,		/*  DOS850_w		119	*/
    ISO1_x,		/*  DOS850_x		120	*/
    ISO1_y,		/*  DOS850_y		121	*/
    ISO1_z,		/*  DOS850_z		122	*/
    ISO1_braceleft,	/*  DOS850_braceleft	123	*/
    ISO1_bar,		/*  DOS850_bar		124	*/
    ISO1_braceright,	/*  DOS850_braceright	125	*/
    ISO1_tilde,		/*  DOS850_tilde	126	*/
    0x7f,		/*  0x7f		127	*/
    ISO1_Ccedilla,	/*  DOS850_Ccedilla	128	*/
    ISO1_udieresis,	/*  DOS850_udieresis	129	*/
    ISO1_eacute,	/*  DOS850_eacute	130	*/
    ISO1_acircumflex,	/*  DOS850_acircumflex	131	*/
    ISO1_adieresis,	/*  DOS850_adieresis	132	*/
    ISO1_agrave,	/*  DOS850_agrave	133	*/
    ISO1_aring,		/*  DOS850_aring	134	*/
    ISO1_ccedilla,	/*  DOS850_ccedilla	135	*/
    ISO1_ecircumflex,	/*  DOS850_ecircumflex	136	*/
    ISO1_edieresis,	/*  DOS850_edieresis	137	*/
    ISO1_egrave,	/*  DOS850_egrave	138	*/
    ISO1_idieresis,	/*  DOS850_idieresis	139	*/
    ISO1_icircumflex,	/*  DOS850_icircumflex	140	*/
    ISO1_igrave,	/*  DOS850_igrave	141	*/
    ISO1_Adieresis,	/*  DOS850_Adieresis	142	*/
    ISO1_Aring,		/*  DOS850_Aring	143	*/
    ISO1_Eacute,	/*  DOS850_Eacute	144	*/
    ISO1_ae,		/*  DOS850_ae		145	*/
    ISO1_AE,		/*  DOS850_AE		146	*/
    ISO1_ocircumflex,	/*  DOS850_ocircumflex	147	*/
    ISO1_odieresis,	/*  DOS850_odieresis	148	*/
    ISO1_ograve,	/*  DOS850_ograve	149	*/
    ISO1_ucircumflex,	/*  DOS850_ucircumflex	150	*/
    ISO1_ugrave,	/*  DOS850_ugrave	151	*/
    ISO1_ydieresis,	/*  DOS850_ydieresis	152	*/
    ISO1_Odieresis,	/*  DOS850_Odieresis	153	*/
    ISO1_Udieresis,	/*  DOS850_Udieresis	154	*/
    ISO1_oslash,	/*  DOS850_oslash	155	*/
    ISO1_sterling,	/*  DOS850_sterling	156	*/
    ISO1_Oslash,	/*  DOS850_Oslash	157	*/
    ISO1_multiply,	/*  DOS850_multiply	158	*/
    ISO1_currency,	/*  DOS850_florin	159	*//**/
    ISO1_aacute,	/*  DOS850_aacute	160	*/
    ISO1_iacute,	/*  DOS850_iacute	161	*/
    ISO1_oacute,	/*  DOS850_oacute	162	*/
    ISO1_uacute,	/*  DOS850_uacute	163	*/
    ISO1_ntilde,	/*  DOS850_ntilde	164	*/
    ISO1_Ntilde,	/*  DOS850_Ntilde	165	*/
    ISO1_ordfeminine,	/*  DOS850_ordfeminine	166	*/
    ISO1_ordmasculine,	/*  DOS850_ordmasculine	167	*/
    ISO1_questiondown,	/*  DOS850_questiondown	168	*/
    ISO1_registered,	/*  DOS850_registered	169	*/
    ISO1_logicalnot,	/*  DOS850_logicalnot	170	*/
    ISO1_onehalf,	/*  DOS850_onehalf	171	*/
    ISO1_onequarter,	/*  DOS850_onequarter	172	*/
    ISO1_exclamdown,	/*  DOS850_exclamdown	173	*/
    ISO1_guillemotleft,	/*  DOS850_guillemotleft 174	*/
    ISO1_guillemotright,/*  DOS850_guillemotright 175	*/
    ISO1_currency,	/*  DOS850_ltshade	176	*//**/
    ISO1_currency,	/*  DOS850_shade	177	*//**/
    ISO1_currency,	/*  DOS850_dkshade	178	*//**/
    ISO1_currency,	/*  DOS850_SF110000	179	*//**/
    ISO1_currency,	/*  DOS850_SF090000	180	*//**/
    ISO1_Aacute,	/*  DOS850_Aacute	181	*/
    ISO1_Acircumflex,	/*  DOS850_Acircumflex	182	*/
    ISO1_Agrave,	/*  DOS850_Agrave	183	*/
    ISO1_copyright,	/*  DOS850_copyright	184	*/
    ISO1_currency,	/*  DOS850_SF230000	185	*//**/
    ISO1_currency,	/*  DOS850_SF240000	186	*//**/
    ISO1_currency,	/*  DOS850_SF250000	187	*//**/
    ISO1_currency,	/*  DOS850_SF260000	188	*//**/
    ISO1_cent,		/*  DOS850_cent		189	*/
    ISO1_yen,		/*  DOS850_yen		190	*/
    ISO1_currency,	/*  DOS850_SF030000	191	*//**/
    ISO1_currency,	/*  DOS850_SF020000	192	*//**/
    ISO1_currency,	/*  DOS850_SF070000	193	*//**/
    ISO1_currency,	/*  DOS850_SF060000	194	*//**/
    ISO1_currency,	/*  DOS850_SF080000	195	*//**/
    ISO1_currency,	/*  DOS850_SF100000	196	*//**/
    ISO1_currency,	/*  DOS850_SF050000	197	*//**/
    ISO1_atilde,	/*  DOS850_atilde	198	*/
    ISO1_Atilde,	/*  DOS850_Atilde	199	*/
    ISO1_currency,	/*  DOS850_SF380000	200	*//**/
    ISO1_currency,	/*  DOS850_SF390000	201	*//**/
    ISO1_currency,	/*  DOS850_SF400000	202	*//**/
    ISO1_currency,	/*  DOS850_SF410000	203	*//**/
    ISO1_currency,	/*  DOS850_SF420000	204	*//**/
    ISO1_currency,	/*  DOS850_SF430000	205	*//**/
    ISO1_currency,	/*  DOS850_SF440000	206	*//**/
    ISO1_currency,	/*  DOS850_currency	207	*/
    ISO1_eth,		/*  DOS850_eth		208	*/
    ISO1_Eth,		/*  DOS850_Eth		209	*/
    ISO1_Ecircumflex,	/*  DOS850_Ecircumflex	210	*/
    ISO1_Edieresis,	/*  DOS850_Edieresis	211	*/
    ISO1_Egrave,	/*  DOS850_Egrave	212	*/
    ISO1_dotlessi,	/*  DOS850_dotlessi	213	*/
    ISO1_Iacute,	/*  DOS850_Iacute	214	*/
    ISO1_Icircumflex,	/*  DOS850_Icircumflex	215	*/
    ISO1_Idieresis,	/*  DOS850_Idieresis	216	*/
    ISO1_currency,	/*  DOS850_SF040000	217	*//**/
    ISO1_currency,	/*  DOS850_SF010000	218	*//**/
    ISO1_currency,	/*  DOS850_block	219	*//**/
    ISO1_currency,	/*  DOS850_dnblock	220	*/
    ISO1_brokenbar,	/*  DOS850_brokenbar	221	*/
    ISO1_Igrave,	/*  DOS850_Igrave	222	*/
    ISO1_currency,	/*  DOS850_upblock	223	*//**/
    ISO1_Oacute,	/*  DOS850_Oacute	224	*/
    ISO1_germandbls,	/*  DOS850_germandbls	225	*/
    ISO1_Ocircumflex,	/*  DOS850_Ocircumflex	226	*/
    ISO1_Ograve,	/*  DOS850_Ograve	227	*/
    ISO1_otilde,	/*  DOS850_otilde	228	*/
    ISO1_Otilde,	/*  DOS850_Otilde	229	*/
    ISO1_mu,		/*  DOS850_mu		230	*/
    ISO1_thorn,		/*  DOS850_thorn	231	*/
    ISO1_Thorn,		/*  DOS850_Thorn	232	*/
    ISO1_Uacute,	/*  DOS850_Uacute	233	*/
    ISO1_Ucircumflex,	/*  DOS850_Ucircumflex	234	*/
    ISO1_Ugrave,	/*  DOS850_Ugrave	235	*/
    ISO1_yacute,	/*  DOS850_yacute	236	*/
    ISO1_Yacute,	/*  DOS850_Yacute	237	*/
    ISO1_macron,	/*  DOS850_macron	238	*/
    ISO1_acute,		/*  DOS850_acute	239	*/
    ISO1_hyphen,	/*  DOS850_hyphen	240	*/
    ISO1_plusminus,	/*  DOS850_plusminus	241	*/
    ISO1_underscore,	/*  DOS850_underscoredbl 242	*//**/
    ISO1_threequarters,	/*  DOS850_threequarters 243	*/
    ISO1_paragraph,	/*  DOS850_paragraph	244	*/
    ISO1_section,	/*  DOS850_section	245	*/
    ISO1_divide,	/*  DOS850_divide	246	*/
    ISO1_cedilla,	/*  DOS850_cedilla	247	*/
    ISO1_degree,	/*  DOS850_degree	248	*/
    ISO1_dieresis,	/*  DOS850_dieresis	249	*/
    ISO1_periodcentered,/*  DOS850_periodcentered 250	*/
    ISO1_onesuperior,	/*  DOS850_onesuperior	251	*/
    ISO1_threesuperior,	/*  DOS850_threesuperior 252	*/
    ISO1_twosuperior,	/*  DOS850_twosuperior	253	*/
    ISO1_currency,	/*  DOS850_filledbox	254	*//**/
    ISO1_nobreakspace,	/*  DOS850_nobreakspace	255	*/
    };

