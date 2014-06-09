/************************************************************************/
/*									*/
/*  Map Font Encodings ECMA: MAC ROMAN <-> ISO 8859-1			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

const unsigned char docMAC_ROMAN_to_ISO1[256]=
    {
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    ISO1_space,			/*  MAC_ROMAN_space		 32	*/
    ISO1_exclam,		/*  MAC_ROMAN_exclam		 33	*/
    ISO1_quotedbl,		/*  MAC_ROMAN_quotedbl		 34	*/
    ISO1_numbersign,		/*  MAC_ROMAN_numbersign	 35	*/
    ISO1_dollar,		/*  MAC_ROMAN_dollar		 36	*/
    ISO1_percent,		/*  MAC_ROMAN_percent		 37	*/
    ISO1_ampersand,		/*  MAC_ROMAN_ampersand		 38	*/
    ISO1_quotesingle,		/*  MAC_ROMAN_quotesingle	 39	*/
    ISO1_parenleft,		/*  MAC_ROMAN_parenleft		 40	*/
    ISO1_parenright,		/*  MAC_ROMAN_parenright	 41	*/
    ISO1_asterisk,		/*  MAC_ROMAN_asterisk		 42	*/
    ISO1_plus,			/*  MAC_ROMAN_plus		 43	*/
    ISO1_comma,			/*  MAC_ROMAN_comma		 44	*/
    ISO1_minus,			/*  MAC_ROMAN_minus		 45	*/
    ISO1_period,		/*  MAC_ROMAN_period		 46	*/
    ISO1_slash,			/*  MAC_ROMAN_slash		 47	*/
    ISO1_zero,			/*  MAC_ROMAN_zero		 48	*/
    ISO1_one,			/*  MAC_ROMAN_one		 49	*/
    ISO1_two,			/*  MAC_ROMAN_two		 50	*/
    ISO1_three,			/*  MAC_ROMAN_three		 51	*/
    ISO1_four,			/*  MAC_ROMAN_four		 52	*/
    ISO1_five,			/*  MAC_ROMAN_five		 53	*/
    ISO1_six,			/*  MAC_ROMAN_six		 54	*/
    ISO1_seven,			/*  MAC_ROMAN_seven		 55	*/
    ISO1_eight,			/*  MAC_ROMAN_eight		 56	*/
    ISO1_nine,			/*  MAC_ROMAN_nine		 57	*/
    ISO1_colon,			/*  MAC_ROMAN_colon		 58	*/
    ISO1_semicolon,		/*  MAC_ROMAN_semicolon		 59	*/
    ISO1_less,			/*  MAC_ROMAN_less		 60	*/
    ISO1_equal,			/*  MAC_ROMAN_equal		 61	*/
    ISO1_greater,		/*  MAC_ROMAN_greater		 62	*/
    ISO1_question,		/*  MAC_ROMAN_question		 63	*/
    ISO1_at,			/*  MAC_ROMAN_at		 64	*/
    ISO1_A,			/*  MAC_ROMAN_A			 65	*/
    ISO1_B,			/*  MAC_ROMAN_B			 66	*/
    ISO1_C,			/*  MAC_ROMAN_C			 67	*/
    ISO1_D,			/*  MAC_ROMAN_D			 68	*/
    ISO1_E,			/*  MAC_ROMAN_E			 69	*/
    ISO1_F,			/*  MAC_ROMAN_F			 70	*/
    ISO1_G,			/*  MAC_ROMAN_G			 71	*/
    ISO1_H,			/*  MAC_ROMAN_H			 72	*/
    ISO1_I,			/*  MAC_ROMAN_I			 73	*/
    ISO1_J,			/*  MAC_ROMAN_J			 74	*/
    ISO1_K,			/*  MAC_ROMAN_K			 75	*/
    ISO1_L,			/*  MAC_ROMAN_L			 76	*/
    ISO1_M,			/*  MAC_ROMAN_M			 77	*/
    ISO1_N,			/*  MAC_ROMAN_N			 78	*/
    ISO1_O,			/*  MAC_ROMAN_O			 79	*/
    ISO1_P,			/*  MAC_ROMAN_P			 80	*/
    ISO1_Q,			/*  MAC_ROMAN_Q			 81	*/
    ISO1_R,			/*  MAC_ROMAN_R			 82	*/
    ISO1_S,			/*  MAC_ROMAN_S			 83	*/
    ISO1_T,			/*  MAC_ROMAN_T			 84	*/
    ISO1_U,			/*  MAC_ROMAN_U			 85	*/
    ISO1_V,			/*  MAC_ROMAN_V			 86	*/
    ISO1_W,			/*  MAC_ROMAN_W			 87	*/
    ISO1_X,			/*  MAC_ROMAN_X			 88	*/
    ISO1_Y,			/*  MAC_ROMAN_Y			 89	*/
    ISO1_Z,			/*  MAC_ROMAN_Z			 90	*/
    ISO1_bracketleft,		/*  MAC_ROMAN_bracketleft	 91	*/
    ISO1_backslash,		/*  MAC_ROMAN_backslash		 92	*/
    ISO1_bracketright,		/*  MAC_ROMAN_bracketright	 93	*/
    ISO1_circumflex,		/*  MAC_ROMAN_asciicircum	 94	*/
    ISO1_underscore,		/*  MAC_ROMAN_underscore	 95	*/
    ISO1_grave,			/*  MAC_ROMAN_grave		 96	*/
    ISO1_a,			/*  MAC_ROMAN_a			 97	*/
    ISO1_b,			/*  MAC_ROMAN_b			 98	*/
    ISO1_c,			/*  MAC_ROMAN_c			 99	*/
    ISO1_d,			/*  MAC_ROMAN_d			100	*/
    ISO1_e,			/*  MAC_ROMAN_e			101	*/
    ISO1_f,			/*  MAC_ROMAN_f			102	*/
    ISO1_g,			/*  MAC_ROMAN_g			103	*/
    ISO1_h,			/*  MAC_ROMAN_h			104	*/
    ISO1_i,			/*  MAC_ROMAN_i			105	*/
    ISO1_j,			/*  MAC_ROMAN_j			106	*/
    ISO1_k,			/*  MAC_ROMAN_k			107	*/
    ISO1_l,			/*  MAC_ROMAN_l			108	*/
    ISO1_m,			/*  MAC_ROMAN_m			109	*/
    ISO1_n,			/*  MAC_ROMAN_n			110	*/
    ISO1_o,			/*  MAC_ROMAN_o			111	*/
    ISO1_p,			/*  MAC_ROMAN_p			112	*/
    ISO1_q,			/*  MAC_ROMAN_q			113	*/
    ISO1_r,			/*  MAC_ROMAN_r			114	*/
    ISO1_s,			/*  MAC_ROMAN_s			115	*/
    ISO1_t,			/*  MAC_ROMAN_t			116	*/
    ISO1_u,			/*  MAC_ROMAN_u			117	*/
    ISO1_v,			/*  MAC_ROMAN_v			118	*/
    ISO1_w,			/*  MAC_ROMAN_w			119	*/
    ISO1_x,			/*  MAC_ROMAN_x			120	*/
    ISO1_y,			/*  MAC_ROMAN_y			121	*/
    ISO1_z,			/*  MAC_ROMAN_z			122	*/
    ISO1_braceleft,		/*  MAC_ROMAN_braceleft		123	*/
    ISO1_bar,			/*  MAC_ROMAN_bar		124	*/
    ISO1_braceright,		/*  MAC_ROMAN_braceright	125	*/
    ISO1_tilde,			/*  MAC_ROMAN_asciitilde	126	*/
    0x7f,
    ISO1_Adieresis,		/*  MAC_ROMAN_Adieresis		128	*/
    ISO1_Aring,			/*  MAC_ROMAN_Aring		129	*/
    ISO1_Ccedilla,		/*  MAC_ROMAN_Ccedilla		130	*/
    ISO1_Eacute,		/*  MAC_ROMAN_Eacute		131	*/
    ISO1_Ntilde,		/*  MAC_ROMAN_Ntilde		132	*/
    ISO1_Odieresis,		/*  MAC_ROMAN_Odieresis		133	*/
    ISO1_Udieresis,		/*  MAC_ROMAN_Udieresis		134	*/
    ISO1_aacute,		/*  MAC_ROMAN_aacute		135	*/
    ISO1_agrave,		/*  MAC_ROMAN_agrave		136	*/
    ISO1_acircumflex,		/*  MAC_ROMAN_acircumflex	137	*/
    ISO1_adieresis,		/*  MAC_ROMAN_adieresis		138	*/
    ISO1_atilde,		/*  MAC_ROMAN_atilde		139	*/
    ISO1_aring,			/*  MAC_ROMAN_aring		140	*/
    ISO1_ccedilla,		/*  MAC_ROMAN_ccedilla		141	*/
    ISO1_eacute,		/*  MAC_ROMAN_eacute		142	*/
    ISO1_egrave,		/*  MAC_ROMAN_egrave		143	*/
    ISO1_ecircumflex,		/*  MAC_ROMAN_ecircumflex	144	*/
    ISO1_edieresis,		/*  MAC_ROMAN_edieresis		145	*/
    ISO1_iacute,		/*  MAC_ROMAN_iacute		146	*/
    ISO1_igrave,		/*  MAC_ROMAN_igrave		147	*/
    ISO1_icircumflex,		/*  MAC_ROMAN_icircumflex	148	*/
    ISO1_idieresis,		/*  MAC_ROMAN_idieresis		149	*/
    ISO1_ntilde,		/*  MAC_ROMAN_ntilde		150	*/
    ISO1_oacute,		/*  MAC_ROMAN_oacute		151	*/
    ISO1_ograve,		/*  MAC_ROMAN_ograve		152	*/
    ISO1_ocircumflex,		/*  MAC_ROMAN_ocircumflex	153	*/
    ISO1_odieresis,		/*  MAC_ROMAN_odieresis		154	*/
    ISO1_otilde,		/*  MAC_ROMAN_otilde		155	*/
    ISO1_uacute,		/*  MAC_ROMAN_uacute		156	*/
    ISO1_ugrave,		/*  MAC_ROMAN_ugrave		157	*/
    ISO1_ucircumflex,		/*  MAC_ROMAN_ucircumflex	158	*/
    ISO1_udieresis,		/*  MAC_ROMAN_udieresis		159	*/
    ISO1_currency,		/*  MAC_ROMAN_dagger		160	*//**/
    ISO1_degree,		/*  MAC_ROMAN_degree		161	*/
    ISO1_cent,			/*  MAC_ROMAN_cent		162	*/
    ISO1_sterling,		/*  MAC_ROMAN_sterling		163	*/
    ISO1_section,		/*  MAC_ROMAN_section		164	*/
    ISO1_currency,		/*  MAC_ROMAN_bullet		165	*//**/
    ISO1_paragraph,		/*  MAC_ROMAN_paragraph		166	*/
    ISO1_germandbls,		/*  MAC_ROMAN_germandbls	167	*/
    ISO1_registered,		/*  MAC_ROMAN_registered	168	*/
    ISO1_copyright,		/*  MAC_ROMAN_copyright		169	*/
    ISO1_currency,		/*  MAC_ROMAN_trademark		170	*//**/
    ISO1_acute,			/*  MAC_ROMAN_acute		171	*/
    ISO1_dieresis,		/*  MAC_ROMAN_dieresis		172	*/
    ISO1_currency,		/*  MAC_ROMAN_notequal		173	*//**/
    ISO1_AE,			/*  MAC_ROMAN_AE		174	*/
    ISO1_Oslash,		/*  MAC_ROMAN_Oslash		175	*/
    ISO1_currency,		/*  MAC_ROMAN_infinity		176	*//**/
    ISO1_plusminus,		/*  MAC_ROMAN_plusminus		177	*/
    ISO1_currency,		/*  MAC_ROMAN_lessequal		178	*//**/
    ISO1_currency,		/*  MAC_ROMAN_greaterequal	179	*//**/
    ISO1_yen,			/*  MAC_ROMAN_yen		180	*/
    ISO1_mu,			/*  MAC_ROMAN_mu		181	*/
    ISO1_currency,		/*  MAC_ROMAN_partialdiff	182	*//**/
    ISO1_currency,		/*  MAC_ROMAN_summation		183	*//**/
    ISO1_currency,		/*  MAC_ROMAN_product		184	*//**/
    ISO1_currency,		/*  MAC_ROMAN_pi		185	*//**/
    ISO1_currency,		/*  MAC_ROMAN_integral		186	*//**/
    ISO1_ordfeminine,		/*  MAC_ROMAN_ordfeminine	187	*/
    ISO1_ordmasculine,		/*  MAC_ROMAN_ordmasculine	188	*/
    ISO1_currency,		/*  MAC_ROMAN__0xBD		189	*//**/
    ISO1_ae,			/*  MAC_ROMAN_ae		190	*/
    ISO1_oslash,		/*  MAC_ROMAN_oslash		191	*/
    ISO1_questiondown,		/*  MAC_ROMAN_questiondown	192	*/
    ISO1_exclamdown,		/*  MAC_ROMAN_exclamdown	193	*/
    ISO1_logicalnot,		/*  MAC_ROMAN_logicalnot	194	*/
    ISO1_currency,		/*  MAC_ROMAN_radical		195	*//**/
    ISO1_currency,		/*  MAC_ROMAN_florin		196	*//**/
    ISO1_currency,		/*  MAC_ROMAN_approxequal	197	*//**/
    ISO1_currency,		/*  MAC_ROMAN_Delta		198	*//**/
    ISO1_guillemotleft,		/*  MAC_ROMAN_guillemotleft	199	*/
    ISO1_guillemotright,	/*  MAC_ROMAN_guillemotright	200	*/
    ISO1_currency,		/*  MAC_ROMAN_ellipsis		201	*//**/
    ISO1_nobreakspace,		/*  MAC_ROMAN_nobreakspace	202	*/
    ISO1_Agrave,		/*  MAC_ROMAN_Agrave		203	*/
    ISO1_Atilde,		/*  MAC_ROMAN_Atilde		204	*/
    ISO1_Otilde,		/*  MAC_ROMAN_Otilde		205	*/
    ISO1_currency,		/*  MAC_ROMAN_OE		206	*//**/
    ISO1_currency,		/*  MAC_ROMAN_oe		207	*//**/
    ISO1_hyphen,		/*  MAC_ROMAN_endash		208	*//**/
    ISO1_hyphen,		/*  MAC_ROMAN_emdash		209	*//**/
    ISO1_quotedbl,		/*  MAC_ROMAN_quotedblleft	210	*//**/
    ISO1_quotedbl,		/*  MAC_ROMAN_quotedblright	211	*//**/
    ISO1_quoteleft,		/*  MAC_ROMAN_quoteleft		212	*/
    ISO1_quotesingle,		/*  MAC_ROMAN_quoteright	213	*//**/
    ISO1_divide,		/*  MAC_ROMAN_divide		214	*/
    ISO1_currency,		/*  MAC_ROMAN_lozenge		215	*//**/
    ISO1_ydieresis,		/*  MAC_ROMAN_ydieresis		216	*/
    ISO1_Y,			/*  MAC_ROMAN_Ydieresis		217	*//**/
    ISO1_slash,			/*  MAC_ROMAN_fraction		218	*//**/
    ISO1_currency,		/*  MAC_ROMAN_Euro		219	*//**/
    ISO1_guillemotleft,		/*  MAC_ROMAN_guilsinglleft	220	*//**/
    ISO1_guillemotright,	/*  MAC_ROMAN_guilsinglright	221	*//**/
    ISO1_currency,		/*  MAC_ROMAN__0xDE		222	*//**/
    ISO1_currency,		/*  MAC_ROMAN__0xDF		223	*//**/
    ISO1_currency,		/*  MAC_ROMAN_daggerdbl		224	*/
    ISO1_periodcentered,	/*  MAC_ROMAN_periodcentered	225	*/
    ISO1_quoteleft,		/*  MAC_ROMAN_quotesinglbase	226	*//**/
    ISO1_quotedbl,		/*  MAC_ROMAN_quotedblbase	227	*//**/
    ISO1_currency,		/*  MAC_ROMAN_perthousand	228	*//**/
    ISO1_Acircumflex,		/*  MAC_ROMAN_Acircumflex	229	*/
    ISO1_Ecircumflex,		/*  MAC_ROMAN_Ecircumflex	230	*/
    ISO1_Aacute,		/*  MAC_ROMAN_Aacute		231	*/
    ISO1_Edieresis,		/*  MAC_ROMAN_Edieresis		232	*/
    ISO1_Egrave,		/*  MAC_ROMAN_Egrave		233	*/
    ISO1_Iacute,		/*  MAC_ROMAN_Iacute		234	*/
    ISO1_Icircumflex,		/*  MAC_ROMAN_Icircumflex	235	*/
    ISO1_Idieresis,		/*  MAC_ROMAN_Idieresis		236	*/
    ISO1_Igrave,		/*  MAC_ROMAN_Igrave		237	*/
    ISO1_Oacute,		/*  MAC_ROMAN_Oacute		238	*/
    ISO1_Ocircumflex,		/*  MAC_ROMAN_Ocircumflex	239	*/
    ISO1_currency,		/*  MAC_ROMAN__0xF0		240	*//**/
    ISO1_Ograve,		/*  MAC_ROMAN_Ograve		241	*/
    ISO1_Uacute,		/*  MAC_ROMAN_Uacute		242	*/
    ISO1_Ucircumflex,		/*  MAC_ROMAN_Ucircumflex	243	*/
    ISO1_Ugrave,		/*  MAC_ROMAN_Ugrave		244	*/
    ISO1_dotlessi,		/*  MAC_ROMAN_dotlessi		245	*/
    ISO1_circumflex,		/*  MAC_ROMAN_circumflex	246	*/
    ISO1_tilde,			/*  MAC_ROMAN_tilde		247	*/
    ISO1_macron,		/*  MAC_ROMAN_macron		248	*/
    ISO1_breve,			/*  MAC_ROMAN_breve		249	*/
    ISO1_dotaccent,		/*  MAC_ROMAN_dotaccent		250	*/
    ISO1_ring,			/*  MAC_ROMAN_ring		251	*/
    ISO1_cedilla,		/*  MAC_ROMAN_cedilla		252	*/
    ISO1_hungarumlaut,		/*  MAC_ROMAN_hungarumlaut	253	*/
    ISO1_ogonek,		/*  MAC_ROMAN_ogonek		254	*/
    ISO1_caron,			/*  MAC_ROMAN_caron		255	*/
    };

const unsigned char docISO1_to_MAC_ROMAN[256]=
    {
    0x00,
    0x01,
    0x02,
    0x03,
    0x04,
    0x05,
    0x06,
    0x07,
    0x08,
    0x09,
    0x0a,
    0x0b,
    0x0c,
    0x0d,
    0x0e,
    0x0f,
    0x10,
    0x11,
    0x12,
    0x13,
    0x14,
    0x15,
    0x16,
    0x17,
    0x18,
    0x19,
    0x1a,
    0x1b,
    0x1c,
    0x1d,
    0x1e,
    0x1f,
    MAC_ROMAN_space,		/*  ISO1_space		 32	*/
    MAC_ROMAN_exclam,		/*  ISO1_exclam		 33	*/
    MAC_ROMAN_quotedbl,		/*  ISO1_quotedbl	 34	*/
    MAC_ROMAN_numbersign,	/*  ISO1_numbersign	 35	*/
    MAC_ROMAN_dollar,		/*  ISO1_dollar		 36	*/
    MAC_ROMAN_percent,		/*  ISO1_percent	 37	*/
    MAC_ROMAN_ampersand,	/*  ISO1_ampersand	 38	*/
    MAC_ROMAN_quotesingle,	/*  ISO1_quotesingle	 39	*/
    MAC_ROMAN_parenleft,	/*  ISO1_parenleft	 40	*/
    MAC_ROMAN_parenright,	/*  ISO1_parenright	 41	*/
    MAC_ROMAN_asterisk,		/*  ISO1_asterisk	 42	*/
    MAC_ROMAN_plus,		/*  ISO1_plus		 43	*/
    MAC_ROMAN_comma,		/*  ISO1_comma		 44	*/
    MAC_ROMAN_minus,		/*  ISO1_minus		 45	*/
    MAC_ROMAN_period,		/*  ISO1_period		 46	*/
    MAC_ROMAN_slash,		/*  ISO1_slash		 47	*/
    MAC_ROMAN_zero,		/*  ISO1_zero		 48	*/
    MAC_ROMAN_one,		/*  ISO1_one		 49	*/
    MAC_ROMAN_two,		/*  ISO1_two		 50	*/
    MAC_ROMAN_three,		/*  ISO1_three		 51	*/
    MAC_ROMAN_four,		/*  ISO1_four		 52	*/
    MAC_ROMAN_five,		/*  ISO1_five		 53	*/
    MAC_ROMAN_six,		/*  ISO1_six		 54	*/
    MAC_ROMAN_seven,		/*  ISO1_seven		 55	*/
    MAC_ROMAN_eight,		/*  ISO1_eight		 56	*/
    MAC_ROMAN_nine,		/*  ISO1_nine		 57	*/
    MAC_ROMAN_colon,		/*  ISO1_colon		 58	*/
    MAC_ROMAN_semicolon,	/*  ISO1_semicolon	 59	*/
    MAC_ROMAN_less,		/*  ISO1_less		 60	*/
    MAC_ROMAN_equal,		/*  ISO1_equal		 61	*/
    MAC_ROMAN_greater,		/*  ISO1_greater	 62	*/
    MAC_ROMAN_question,		/*  ISO1_question	 63	*/
    MAC_ROMAN_at,		/*  ISO1_at		 64	*/
    MAC_ROMAN_A,		/*  ISO1_A		 65	*/
    MAC_ROMAN_B,		/*  ISO1_B		 66	*/
    MAC_ROMAN_C,		/*  ISO1_C		 67	*/
    MAC_ROMAN_D,		/*  ISO1_D		 68	*/
    MAC_ROMAN_E,		/*  ISO1_E		 69	*/
    MAC_ROMAN_F,		/*  ISO1_F		 70	*/
    MAC_ROMAN_G,		/*  ISO1_G		 71	*/
    MAC_ROMAN_H,		/*  ISO1_H		 72	*/
    MAC_ROMAN_I,		/*  ISO1_I		 73	*/
    MAC_ROMAN_J,		/*  ISO1_J		 74	*/
    MAC_ROMAN_K,		/*  ISO1_K		 75	*/
    MAC_ROMAN_L,		/*  ISO1_L		 76	*/
    MAC_ROMAN_M,		/*  ISO1_M		 77	*/
    MAC_ROMAN_N,		/*  ISO1_N		 78	*/
    MAC_ROMAN_O,		/*  ISO1_O		 79	*/
    MAC_ROMAN_P,		/*  ISO1_P		 80	*/
    MAC_ROMAN_Q,		/*  ISO1_Q		 81	*/
    MAC_ROMAN_R,		/*  ISO1_R		 82	*/
    MAC_ROMAN_S,		/*  ISO1_S		 83	*/
    MAC_ROMAN_T,		/*  ISO1_T		 84	*/
    MAC_ROMAN_U,		/*  ISO1_U		 85	*/
    MAC_ROMAN_V,		/*  ISO1_V		 86	*/
    MAC_ROMAN_W,		/*  ISO1_W		 87	*/
    MAC_ROMAN_X,		/*  ISO1_X		 88	*/
    MAC_ROMAN_Y,		/*  ISO1_Y		 89	*/
    MAC_ROMAN_Z,		/*  ISO1_Z		 90	*/
    MAC_ROMAN_bracketleft,	/*  ISO1_bracketleft	 91	*/
    MAC_ROMAN_backslash,	/*  ISO1_backslash	 92	*/
    MAC_ROMAN_bracketright,	/*  ISO1_bracketright	 93	*/
    MAC_ROMAN_asciicircum,	/*  ISO1_asciicircum	 94	*/
    MAC_ROMAN_underscore,	/*  ISO1_underscore	 95	*/
    MAC_ROMAN_grave,		/*  ISO1_grave		 96	*/
    MAC_ROMAN_a,		/*  ISO1_a		 97	*/
    MAC_ROMAN_b,		/*  ISO1_b		 98	*/
    MAC_ROMAN_c,		/*  ISO1_c		 99	*/
    MAC_ROMAN_d,		/*  ISO1_d		100	*/
    MAC_ROMAN_e,		/*  ISO1_e		101	*/
    MAC_ROMAN_f,		/*  ISO1_f		102	*/
    MAC_ROMAN_g,		/*  ISO1_g		103	*/
    MAC_ROMAN_h,		/*  ISO1_h		104	*/
    MAC_ROMAN_i,		/*  ISO1_i		105	*/
    MAC_ROMAN_j,		/*  ISO1_j		106	*/
    MAC_ROMAN_k,		/*  ISO1_k		107	*/
    MAC_ROMAN_l,		/*  ISO1_l		108	*/
    MAC_ROMAN_m,		/*  ISO1_m		109	*/
    MAC_ROMAN_n,		/*  ISO1_n		110	*/
    MAC_ROMAN_o,		/*  ISO1_o		111	*/
    MAC_ROMAN_p,		/*  ISO1_p		112	*/
    MAC_ROMAN_q,		/*  ISO1_q		113	*/
    MAC_ROMAN_r,		/*  ISO1_r		114	*/
    MAC_ROMAN_s,		/*  ISO1_s		115	*/
    MAC_ROMAN_t,		/*  ISO1_t		116	*/
    MAC_ROMAN_u,		/*  ISO1_u		117	*/
    MAC_ROMAN_v,		/*  ISO1_v		118	*/
    MAC_ROMAN_w,		/*  ISO1_w		119	*/
    MAC_ROMAN_x,		/*  ISO1_x		120	*/
    MAC_ROMAN_y,		/*  ISO1_y		121	*/
    MAC_ROMAN_z,		/*  ISO1_z		122	*/
    MAC_ROMAN_braceleft,	/*  ISO1_braceleft	123	*/
    MAC_ROMAN_bar,		/*  ISO1_bar		124	*/
    MAC_ROMAN_braceright,	/*  ISO1_braceright	125	*/
    MAC_ROMAN_asciitilde,	/*  ISO1_asciitilde	126	*/
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    0xf0,
    MAC_ROMAN_nobreakspace,	/*  ISO1_nobreakspace	160	*/
    MAC_ROMAN_exclamdown,	/*  ISO1_exclamdown	161	*/
    MAC_ROMAN_cent,		/*  ISO1_cent		162	*/
    MAC_ROMAN_sterling,		/*  ISO1_sterling	163	*/
    0xf0,			/*  ISO1_currency	164	*//**/
    MAC_ROMAN_yen,		/*  ISO1_yen		165	*/
    MAC_ROMAN_bar,		/*  ISO1_brokenbar	166	*//**/
    MAC_ROMAN_section,		/*  ISO1_section	167	*/
    MAC_ROMAN_dieresis,		/*  ISO1_dieresis	168	*/
    MAC_ROMAN_copyright,	/*  ISO1_copyright	169	*/
    MAC_ROMAN_ordfeminine,	/*  ISO1_ordfeminine	170	*/
    MAC_ROMAN_guillemotleft,	/*  ISO1_guillemotleft	171	*/
    MAC_ROMAN_logicalnot,	/*  ISO1_logicalnot	172	*/
    0xf0,			/*  ISO1__0xAD		173	*//**/
    MAC_ROMAN_registered,	/*  ISO1_registered	174	*/
    MAC_ROMAN_macron,		/*  ISO1_macron		175	*/
    MAC_ROMAN_degree,		/*  ISO1_degree		176	*/
    MAC_ROMAN_plusminus,	/*  ISO1_plusminus	177	*/
    MAC_ROMAN_two,		/*  ISO1_twosuperior	178	*//**/
    MAC_ROMAN_three,		/*  ISO1_threesuperior	179	*//**/
    MAC_ROMAN_acute,		/*  ISO1_acute		180	*/
    MAC_ROMAN_mu,		/*  ISO1_mu		181	*/
    MAC_ROMAN_paragraph,	/*  ISO1_paragraph	182	*/
    MAC_ROMAN_periodcentered,	/*  ISO1_periodcentered	183	*/
    MAC_ROMAN_cedilla,		/*  ISO1_cedilla	184	*/
    MAC_ROMAN_one,		/*  ISO1_onesuperior	185	*//**/
    MAC_ROMAN_ordmasculine,	/*  ISO1_ordmasculine	186	*/
    MAC_ROMAN_guillemotright,	/*  ISO1_guillemotright	187	*/
    0xf0,			/*  ISO1_onequarter	188	*//**/
    0xf0,			/*  ISO1_onehalf	189	*//**/
    0xf0,			/*  ISO1_threequarters	190	*//**/
    MAC_ROMAN_questiondown,	/*  ISO1_questiondown	191	*/
    MAC_ROMAN_Agrave,		/*  ISO1_Agrave		192	*/
    MAC_ROMAN_Aacute,		/*  ISO1_Aacute		193	*/
    MAC_ROMAN_Acircumflex,	/*  ISO1_Acircumflex	194	*/
    MAC_ROMAN_Atilde,		/*  ISO1_Atilde		195	*/
    MAC_ROMAN_Adieresis,	/*  ISO1_Adieresis	196	*/
    MAC_ROMAN_Aring,		/*  ISO1_Aring		197	*/
    MAC_ROMAN_AE,		/*  ISO1_AE		198	*/
    MAC_ROMAN_Ccedilla,		/*  ISO1_Ccedilla	199	*/
    MAC_ROMAN_Egrave,		/*  ISO1_Egrave		200	*/
    MAC_ROMAN_Eacute,		/*  ISO1_Eacute		201	*/
    MAC_ROMAN_Ecircumflex,	/*  ISO1_Ecircumflex	202	*/
    MAC_ROMAN_Edieresis,	/*  ISO1_Edieresis	203	*/
    MAC_ROMAN_Igrave,		/*  ISO1_Igrave		204	*/
    MAC_ROMAN_Iacute,		/*  ISO1_Iacute		205	*/
    MAC_ROMAN_Icircumflex,	/*  ISO1_Icircumflex	206	*/
    MAC_ROMAN_Idieresis,	/*  ISO1_Idieresis	207	*/
    MAC_ROMAN_D,		/*  ISO1_Eth		208	*//**/
    MAC_ROMAN_Ntilde,		/*  ISO1_Ntilde		209	*/
    MAC_ROMAN_Ograve,		/*  ISO1_Ograve		210	*/
    MAC_ROMAN_Oacute,		/*  ISO1_Oacute		211	*/
    MAC_ROMAN_Ocircumflex,	/*  ISO1_Ocircumflex	212	*/
    MAC_ROMAN_Otilde,		/*  ISO1_Otilde		213	*/
    MAC_ROMAN_Odieresis,	/*  ISO1_Odieresis	214	*/
    MAC_ROMAN_asterisk,		/*  ISO1_multiply	215	*//**/
    MAC_ROMAN_Oslash,		/*  ISO1_Oslash		216	*/
    MAC_ROMAN_Ugrave,		/*  ISO1_Ugrave		217	*/
    MAC_ROMAN_Uacute,		/*  ISO1_Uacute		218	*/
    MAC_ROMAN_Ucircumflex,	/*  ISO1_Ucircumflex	219	*/
    MAC_ROMAN_Udieresis,	/*  ISO1_Udieresis	220	*/
    MAC_ROMAN_Y,		/*  ISO1_Yacute		221	*//**/
    MAC_ROMAN_T,		/*  ISO1_Thorn		222	*//**/
    MAC_ROMAN_germandbls,	/*  ISO1_germandbls	223	*/
    MAC_ROMAN_agrave,		/*  ISO1_agrave		224	*/
    MAC_ROMAN_aacute,		/*  ISO1_aacute		225	*/
    MAC_ROMAN_acircumflex,	/*  ISO1_acircumflex	226	*/
    MAC_ROMAN_atilde,		/*  ISO1_atilde		227	*/
    MAC_ROMAN_adieresis,	/*  ISO1_adieresis	228	*/
    MAC_ROMAN_aring,		/*  ISO1_aring		229	*/
    MAC_ROMAN_ae,		/*  ISO1_ae		230	*/
    MAC_ROMAN_ccedilla,		/*  ISO1_ccedilla	231	*/
    MAC_ROMAN_egrave,		/*  ISO1_egrave		232	*/
    MAC_ROMAN_eacute,		/*  ISO1_eacute		233	*/
    MAC_ROMAN_ecircumflex,	/*  ISO1_ecircumflex	234	*/
    MAC_ROMAN_edieresis,	/*  ISO1_edieresis	235	*/
    MAC_ROMAN_igrave,		/*  ISO1_igrave		236	*/
    MAC_ROMAN_iacute,		/*  ISO1_iacute		237	*/
    MAC_ROMAN_icircumflex,	/*  ISO1_icircumflex	238	*/
    MAC_ROMAN_idieresis,	/*  ISO1_idieresis	239	*/
    MAC_ROMAN_d,		/*  ISO1_eth		240	*//**/
    MAC_ROMAN_ntilde,		/*  ISO1_ntilde		241	*/
    MAC_ROMAN_ograve,		/*  ISO1_ograve		242	*/
    MAC_ROMAN_oacute,		/*  ISO1_oacute		243	*/
    MAC_ROMAN_ocircumflex,	/*  ISO1_ocircumflex	244	*/
    MAC_ROMAN_otilde,		/*  ISO1_otilde		245	*/
    MAC_ROMAN_odieresis,	/*  ISO1_odieresis	246	*/
    MAC_ROMAN_divide,		/*  ISO1_divide		247	*/
    MAC_ROMAN_oslash,		/*  ISO1_oslash		248	*/
    MAC_ROMAN_ugrave,		/*  ISO1_ugrave		249	*/
    MAC_ROMAN_uacute,		/*  ISO1_uacute		250	*/
    MAC_ROMAN_ucircumflex,	/*  ISO1_ucircumflex	251	*/
    MAC_ROMAN_udieresis,	/*  ISO1_udieresis	252	*/
    MAC_ROMAN_y,		/*  ISO1_yacute		253	*//**/
    MAC_ROMAN_t,		/*  ISO1_thorn		254	*//**/
    MAC_ROMAN_ydieresis,	/*  ISO1_ydieresis	255	*/
    };

