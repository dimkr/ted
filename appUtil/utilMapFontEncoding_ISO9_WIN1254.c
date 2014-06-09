/************************************************************************/
/*									*/
/*  Map Font Encodings Turkish: WIN1254 <-> ISO 8859-9			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

const unsigned char docWIN1254_to_ISO9[256]=
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
    ISO9_space,			/*  WIN1254_space		 32	*/
    ISO9_exclam,		/*  WIN1254_exclam		 33	*/
    ISO9_quotedbl,		/*  WIN1254_quotedbl		 34	*/
    ISO9_numbersign,		/*  WIN1254_numbersign		 35	*/
    ISO9_dollar,		/*  WIN1254_dollar		 36	*/
    ISO9_percent,		/*  WIN1254_percent		 37	*/
    ISO9_ampersand,		/*  WIN1254_ampersand		 38	*/
    ISO9_quotesingle,		/*  WIN1254_quotesingle		 39	*/
    ISO9_parenleft,		/*  WIN1254_parenleft		 40	*/
    ISO9_parenright,		/*  WIN1254_parenright		 41	*/
    ISO9_asterisk,		/*  WIN1254_asterisk		 42	*/
    ISO9_plus,			/*  WIN1254_plus		 43	*/
    ISO9_comma,			/*  WIN1254_comma		 44	*/
    ISO9_minus,			/*  WIN1254_minus		 45	*/
    ISO9_period,		/*  WIN1254_period		 46	*/
    ISO9_slash,			/*  WIN1254_slash		 47	*/
    ISO9_zero,			/*  WIN1254_zero		 48	*/
    ISO9_one,			/*  WIN1254_one			 49	*/
    ISO9_two,			/*  WIN1254_two			 50	*/
    ISO9_three,			/*  WIN1254_three		 51	*/
    ISO9_four,			/*  WIN1254_four		 52	*/
    ISO9_five,			/*  WIN1254_five		 53	*/
    ISO9_six,			/*  WIN1254_six			 54	*/
    ISO9_seven,			/*  WIN1254_seven		 55	*/
    ISO9_eight,			/*  WIN1254_eight		 56	*/
    ISO9_nine,			/*  WIN1254_nine		 57	*/
    ISO9_colon,			/*  WIN1254_colon		 58	*/
    ISO9_semicolon,		/*  WIN1254_semicolon		 59	*/
    ISO9_less,			/*  WIN1254_less		 60	*/
    ISO9_equal,			/*  WIN1254_equal		 61	*/
    ISO9_greater,		/*  WIN1254_greater		 62	*/
    ISO9_question,		/*  WIN1254_question		 63	*/
    ISO9_at,			/*  WIN1254_at			 64	*/
    ISO9_A,			/*  WIN1254_A			 65	*/
    ISO9_B,			/*  WIN1254_B			 66	*/
    ISO9_C,			/*  WIN1254_C			 67	*/
    ISO9_D,			/*  WIN1254_D			 68	*/
    ISO9_E,			/*  WIN1254_E			 69	*/
    ISO9_F,			/*  WIN1254_F			 70	*/
    ISO9_G,			/*  WIN1254_G			 71	*/
    ISO9_H,			/*  WIN1254_H			 72	*/
    ISO9_I,			/*  WIN1254_I			 73	*/
    ISO9_J,			/*  WIN1254_J			 74	*/
    ISO9_K,			/*  WIN1254_K			 75	*/
    ISO9_L,			/*  WIN1254_L			 76	*/
    ISO9_M,			/*  WIN1254_M			 77	*/
    ISO9_N,			/*  WIN1254_N			 78	*/
    ISO9_O,			/*  WIN1254_O			 79	*/
    ISO9_P,			/*  WIN1254_P			 80	*/
    ISO9_Q,			/*  WIN1254_Q			 81	*/
    ISO9_R,			/*  WIN1254_R			 82	*/
    ISO9_S,			/*  WIN1254_S			 83	*/
    ISO9_T,			/*  WIN1254_T			 84	*/
    ISO9_U,			/*  WIN1254_U			 85	*/
    ISO9_V,			/*  WIN1254_V			 86	*/
    ISO9_W,			/*  WIN1254_W			 87	*/
    ISO9_X,			/*  WIN1254_X			 88	*/
    ISO9_Y,			/*  WIN1254_Y			 89	*/
    ISO9_Z,			/*  WIN1254_Z			 90	*/
    ISO9_bracketleft,		/*  WIN1254_bracketleft		 91	*/
    ISO9_backslash,		/*  WIN1254_backslash		 92	*/
    ISO9_bracketright,		/*  WIN1254_bracketright	 93	*/
    ISO9_asciicircum,		/*  WIN1254_asciicircum		 94	*/
    ISO9_underscore,		/*  WIN1254_underscore		 95	*/
    ISO9_grave,			/*  WIN1254_grave		 96	*/
    ISO9_a,			/*  WIN1254_a			 97	*/
    ISO9_b,			/*  WIN1254_b			 98	*/
    ISO9_c,			/*  WIN1254_c			 99	*/
    ISO9_d,			/*  WIN1254_d			100	*/
    ISO9_e,			/*  WIN1254_e			101	*/
    ISO9_f,			/*  WIN1254_f			102	*/
    ISO9_g,			/*  WIN1254_g			103	*/
    ISO9_h,			/*  WIN1254_h			104	*/
    ISO9_i,			/*  WIN1254_i			105	*/
    ISO9_j,			/*  WIN1254_j			106	*/
    ISO9_k,			/*  WIN1254_k			107	*/
    ISO9_l,			/*  WIN1254_l			108	*/
    ISO9_m,			/*  WIN1254_m			109	*/
    ISO9_n,			/*  WIN1254_n			110	*/
    ISO9_o,			/*  WIN1254_o			111	*/
    ISO9_p,			/*  WIN1254_p			112	*/
    ISO9_q,			/*  WIN1254_q			113	*/
    ISO9_r,			/*  WIN1254_r			114	*/
    ISO9_s,			/*  WIN1254_s			115	*/
    ISO9_t,			/*  WIN1254_t			116	*/
    ISO9_u,			/*  WIN1254_u			117	*/
    ISO9_v,			/*  WIN1254_v			118	*/
    ISO9_w,			/*  WIN1254_w			119	*/
    ISO9_x,			/*  WIN1254_x			120	*/
    ISO9_y,			/*  WIN1254_y			121	*/
    ISO9_z,			/*  WIN1254_z			122	*/
    ISO9_braceleft,		/*  WIN1254_braceleft		123	*/
    ISO9_bar,			/*  WIN1254_bar			124	*/
    ISO9_braceright,		/*  WIN1254_braceright		125	*/
    ISO9_asciitilde,		/*  WIN1254_asciitilde		126	*/
    0x7F,			/*  WIN1254__0x7F		127	*//**/
    0x80,			/*  WIN1254__0x80		128	*//**/
    0x81,			/*  WIN1254__0x81		129	*//**/
    ISO9_quotesingle,		/*  WIN1254_quotesinglbase	130	*//**/
    ISO9_currency,		/*  WIN1254_florin		131	*//**/
    ISO9_quotedbl,		/*  WIN1254_quotedblbase	132	*//**/
    ISO9_currency,		/*  WIN1254_ellipsis		133	*//**/
    ISO9_currency,		/*  WIN1254_dagger		134	*//**/
    ISO9_currency,		/*  WIN1254_daggerdbl		135	*//**/
    ISO9_asciicircum,		/*  WIN1254_circumflex		136	*//**/
    ISO9_currency,		/*  WIN1254_perthousand		137	*//**/
    ISO9_currency,		/*  WIN1254_Scaron		138	*//**/
    ISO9_currency,		/*  WIN1254_guilsinglleft	139	*//**/
    ISO9_currency,		/*  WIN1254_OE			140	*//**/
    0x8D,			/*  WIN1254__0x8D		141	*//**/
    0x8E,			/*  WIN1254__0x8E		142	*//**/
    0x8F,			/*  WIN1254__0x8F		143	*//**/
    0x90,			/*  WIN1254__0x90		144	*//**/
    ISO9_quotesingle,		/*  WIN1254_quotesingle		145	*//**/
    ISO9_quotesingle,		/*  WIN1254_quoteright		146	*//**/
    ISO9_quotedbl,		/*  WIN1254_quotedbllefti	147	*//**/
    ISO9_quotedbl,		/*  WIN1254_quotedblright	148	*//**/
    ISO9_currency,		/*  WIN1254_bullet		149	*//**/
    ISO9_minus,			/*  WIN1254_endash		150	*//**/
    ISO9_minus,			/*  WIN1254_emdash		151	*//**/
    ISO9_asciitilde,		/*  WIN1254_tilde		152	*//**/
    ISO9_currency,		/*  WIN1254_trademark		153	*//**/
    ISO9_currency,		/*  WIN1254_scaron		154	*//**/
    ISO9_currency,		/*  WIN1254_guilsinglright	155	*//**/
    ISO9_currency,		/*  WIN1254_oe			156	*//**/
    0x9D,			/*  WIN1254__0x9D		157	*//**/
    0x9E,			/*  WIN1254__0x9E		158	*//**/
    ISO9_ydieresis,		/*  WIN1254_Ydieresis		159	*//**/
    ISO9_nobreakspace,		/*  WIN1254_space		160	*/
    ISO9_exclamdown,		/*  WIN1254_exclamdown		161	*/
    ISO9_cent,			/*  WIN1254_cent		162	*/
    ISO9_sterling,		/*  WIN1254_sterling		163	*/
    ISO9_currency,		/*  WIN1254_currency		164	*/
    ISO9_yen,			/*  WIN1254_yen			165	*/
    ISO9_brokenbar,		/*  WIN1254_brokenbar		166	*/
    ISO9_section,		/*  WIN1254_section		167	*/
    ISO9_dieresis,		/*  WIN1254_dieresis		168	*/
    ISO9_copyright,		/*  WIN1254_copyright		169	*/
    ISO9_ordfeminine,		/*  WIN1254_ordfeminine		170	*/
    ISO9_guillemotleft,		/*  WIN1254_guillemotleft	171	*/
    ISO9_logicalnot,		/*  WIN1254_logicalnot		172	*/
    ISO9_hyphen,		/*  WIN1254_hyphen		173	*/
    ISO9_registered,		/*  WIN1254_registered		174	*/
    ISO9_macron,		/*  WIN1254_macron		175	*/
    ISO9_degree,		/*  WIN1254_degree		176	*/
    ISO9_plusminus,		/*  WIN1254_plusminus		177	*/
    ISO9_twosuperior,		/*  WIN1254_twosuperior		178	*/
    ISO9_threesuperior,		/*  WIN1254_threesuperior	179	*/
    ISO9_acute,			/*  WIN1254_acute		180	*/
    ISO9_mu,			/*  WIN1254_mu			181	*/
    ISO9_paragraph,		/*  WIN1254_paragraph		182	*/
    ISO9_periodcentered,	/*  WIN1254_periodcentered	183	*/
    ISO9_cedilla,		/*  WIN1254_cedilla		184	*/
    ISO9_onesuperior,		/*  WIN1254_onesuperior		185	*/
    ISO9_ordmasculine,		/*  WIN1254_ordmasculine	186	*/
    ISO9_guillemotright,	/*  WIN1254_guillemotright	187	*/
    ISO9_onequarter,		/*  WIN1254_onequarter		188	*/
    ISO9_onehalf,		/*  WIN1254_onehalf		189	*/
    ISO9_threequarters,		/*  WIN1254_threequarters	190	*/
    ISO9_questiondown,		/*  WIN1254_questiondown	191	*/
    ISO9_Agrave,		/*  WIN1254_Agrave		192	*/
    ISO9_Aacute,		/*  WIN1254_Aacute		193	*/
    ISO9_Acircumflex,		/*  WIN1254_Acircumflex		194	*/
    ISO9_Atilde,		/*  WIN1254_Atilde		195	*/
    ISO9_Adieresis,		/*  WIN1254_Adieresis		196	*/
    ISO9_Aring,			/*  WIN1254_Aring		197	*/
    ISO9_AE,			/*  WIN1254_AE			198	*/
    ISO9_Ccedilla,		/*  WIN1254_Ccedilla		199	*/
    ISO9_Egrave,		/*  WIN1254_Egrave		200	*/
    ISO9_Eacute,		/*  WIN1254_Eacute		201	*/
    ISO9_Ecircumflex,		/*  WIN1254_Ecircumflex		202	*/
    ISO9_Edieresis,		/*  WIN1254_Edieresis		203	*/
    ISO9_Igrave,		/*  WIN1254_Igrave		204	*/
    ISO9_Iacute,		/*  WIN1254_Iacute		205	*/
    ISO9_Icircumflex,		/*  WIN1254_Icircumflex		206	*/
    ISO9_Idieresis,		/*  WIN1254_Idieresis		207	*/
    ISO9_Gbreve,		/*  WIN1254_Gbreve		208	*/
    ISO9_Ntilde,		/*  WIN1254_Ntilde		209	*/
    ISO9_Ograve,		/*  WIN1254_Ograve		210	*/
    ISO9_Oacute,		/*  WIN1254_Oacute		211	*/
    ISO9_Ocircumflex,		/*  WIN1254_Ocircumflex		212	*/
    ISO9_Otilde,		/*  WIN1254_Otilde		213	*/
    ISO9_Odieresis,		/*  WIN1254_Odieresis		214	*/
    ISO9_multiply,		/*  WIN1254_multiply		215	*/
    ISO9_Oslash,		/*  WIN1254_Oslash		216	*/
    ISO9_Ugrave,		/*  WIN1254_Ugrave		217	*/
    ISO9_Uacute,		/*  WIN1254_Uacute		218	*/
    ISO9_Ucircumflex,		/*  WIN1254_Ucircumflex		219	*/
    ISO9_Udieresis,		/*  WIN1254_Udieresis		220	*/
    ISO9_Idotaccent,		/*  WIN1254_Idotaccent		221	*/
    ISO9_Scedilla,		/*  WIN1254_Scedilla		222	*/
    ISO9_germandbls,		/*  WIN1254_germandbls		223	*/
    ISO9_agrave,		/*  WIN1254_agrave		224	*/
    ISO9_aacute,		/*  WIN1254_aacute		225	*/
    ISO9_acircumflex,		/*  WIN1254_acircumflex		226	*/
    ISO9_atilde,		/*  WIN1254_atilde		227	*/
    ISO9_adieresis,		/*  WIN1254_adieresis		228	*/
    ISO9_aring,			/*  WIN1254_aring		229	*/
    ISO9_ae,			/*  WIN1254_ae			230	*/
    ISO9_ccedilla,		/*  WIN1254_ccedilla		231	*/
    ISO9_egrave,		/*  WIN1254_egrave		232	*/
    ISO9_eacute,		/*  WIN1254_eacute		233	*/
    ISO9_ecircumflex,		/*  WIN1254_ecircumflex		234	*/
    ISO9_edieresis,		/*  WIN1254_edieresis		235	*/
    ISO9_igrave,		/*  WIN1254_igrave		236	*/
    ISO9_iacute,		/*  WIN1254_iacute		237	*/
    ISO9_icircumflex,		/*  WIN1254_icircumflex		238	*/
    ISO9_idieresis,		/*  WIN1254_idieresis		239	*/
    ISO9_gbreve,		/*  WIN1254_gbreve		240	*/
    ISO9_ntilde,		/*  WIN1254_ntilde		241	*/
    ISO9_ograve,		/*  WIN1254_ograve		242	*/
    ISO9_oacute,		/*  WIN1254_oacute		243	*/
    ISO9_ocircumflex,		/*  WIN1254_ocircumflex		244	*/
    ISO9_otilde,		/*  WIN1254_otilde		245	*/
    ISO9_odieresis,		/*  WIN1254_odieresis		246	*/
    ISO9_divide,		/*  WIN1254_divide		247	*/
    ISO9_oslash,		/*  WIN1254_oslash		248	*/
    ISO9_ugrave,		/*  WIN1254_ugrave		249	*/
    ISO9_uacute,		/*  WIN1254_uacute		250	*/
    ISO9_ucircumflex,		/*  WIN1254_ucircumflex		251	*/
    ISO9_udieresis,		/*  WIN1254_udieresis		252	*/
    ISO9_dotlessi,		/*  WIN1254_dotlessi		253	*/
    ISO9_scedilla,		/*  WIN1254_scedilla		254	*/
    ISO9_ydieresis,		/*  WIN1254_ydieresis		255	*/
    };

const unsigned char docISO9_to_WIN1254[256]=
    {
    WIN1254_currency,		/*  .NOTDEF_0		  0	*//**/
    WIN1254_currency,		/*  .NOTDEF_1		  1	*//**/
    WIN1254_currency,		/*  .NOTDEF_2		  2	*//**/
    WIN1254_currency,		/*  .NOTDEF_3		  3	*//**/
    WIN1254_currency,		/*  .NOTDEF_4		  4	*//**/
    WIN1254_currency,		/*  .NOTDEF_5		  5	*//**/
    WIN1254_currency,		/*  .NOTDEF_6		  6	*//**/
    WIN1254_currency,		/*  .NOTDEF_7		  7	*//**/
    WIN1254_currency,		/*  .NOTDEF_8		  8	*//**/
    WIN1254_currency,		/*  .NOTDEF_9		  9	*//**/
    WIN1254_currency,		/*  .NOTDEF_10		 10	*//**/
    WIN1254_currency,		/*  .NOTDEF_11		 11	*//**/
    WIN1254_currency,		/*  .NOTDEF_12		 12	*//**/
    WIN1254_currency,		/*  .NOTDEF_13		 13	*//**/
    WIN1254_currency,		/*  .NOTDEF_14		 14	*//**/
    WIN1254_currency,		/*  .NOTDEF_15		 15	*//**/
    WIN1254_currency,		/*  .NOTDEF_16		 16	*//**/
    WIN1254_currency,		/*  .NOTDEF_17		 17	*//**/
    WIN1254_currency,		/*  .NOTDEF_18		 18	*//**/
    WIN1254_currency,		/*  .NOTDEF_19		 19	*//**/
    WIN1254_currency,		/*  .NOTDEF_20		 20	*//**/
    WIN1254_currency,		/*  .NOTDEF_21		 21	*//**/
    WIN1254_currency,		/*  .NOTDEF_22		 22	*//**/
    WIN1254_currency,		/*  .NOTDEF_23		 23	*//**/
    WIN1254_currency,		/*  .NOTDEF_24		 24	*//**/
    WIN1254_currency,		/*  .NOTDEF_25		 25	*//**/
    WIN1254_currency,		/*  .NOTDEF_26		 26	*//**/
    WIN1254_currency,		/*  .NOTDEF_27		 27	*//**/
    WIN1254_currency,		/*  .NOTDEF_28		 28	*//**/
    WIN1254_currency,		/*  .NOTDEF_29		 29	*//**/
    WIN1254_currency,		/*  .NOTDEF_30		 30	*//**/
    WIN1254_currency,		/*  .NOTDEF_31		 31	*//**/
    WIN1254_space,		/*  ISO9_space		 32	*/
    WIN1254_exclam,		/*  ISO9_exclam		 33	*/
    WIN1254_quotedbl,		/*  ISO9_quotedbl	 34	*/
    WIN1254_numbersign,		/*  ISO9_numbersign	 35	*/
    WIN1254_dollar,		/*  ISO9_dollar		 36	*/
    WIN1254_percent,		/*  ISO9_percent	 37	*/
    WIN1254_ampersand,		/*  ISO9_ampersand	 38	*/
    WIN1254_quotesingle,	/*  ISO9_quotesingle	 39	*/
    WIN1254_parenleft,		/*  ISO9_parenleft	 40	*/
    WIN1254_parenright,		/*  ISO9_parenright	 41	*/
    WIN1254_asterisk,		/*  ISO9_asterisk	 42	*/
    WIN1254_plus,		/*  ISO9_plus		 43	*/
    WIN1254_comma,		/*  ISO9_comma		 44	*/
    WIN1254_minus,		/*  ISO9_minus		 45	*/
    WIN1254_period,		/*  ISO9_period		 46	*/
    WIN1254_slash,		/*  ISO9_slash		 47	*/
    WIN1254_zero,		/*  ISO9_zero		 48	*/
    WIN1254_one,		/*  ISO9_one		 49	*/
    WIN1254_two,		/*  ISO9_two		 50	*/
    WIN1254_three,		/*  ISO9_three		 51	*/
    WIN1254_four,		/*  ISO9_four		 52	*/
    WIN1254_five,		/*  ISO9_five		 53	*/
    WIN1254_six,		/*  ISO9_six		 54	*/
    WIN1254_seven,		/*  ISO9_seven		 55	*/
    WIN1254_eight,		/*  ISO9_eight		 56	*/
    WIN1254_nine,		/*  ISO9_nine		 57	*/
    WIN1254_colon,		/*  ISO9_colon		 58	*/
    WIN1254_semicolon,		/*  ISO9_semicolon	 59	*/
    WIN1254_less,		/*  ISO9_less		 60	*/
    WIN1254_equal,		/*  ISO9_equal		 61	*/
    WIN1254_greater,		/*  ISO9_greater	 62	*/
    WIN1254_question,		/*  ISO9_question	 63	*/
    WIN1254_at,			/*  ISO9_at		 64	*/
    WIN1254_A,			/*  ISO9_A		 65	*/
    WIN1254_B,			/*  ISO9_B		 66	*/
    WIN1254_C,			/*  ISO9_C		 67	*/
    WIN1254_D,			/*  ISO9_D		 68	*/
    WIN1254_E,			/*  ISO9_E		 69	*/
    WIN1254_F,			/*  ISO9_F		 70	*/
    WIN1254_G,			/*  ISO9_G		 71	*/
    WIN1254_H,			/*  ISO9_H		 72	*/
    WIN1254_I,			/*  ISO9_I		 73	*/
    WIN1254_J,			/*  ISO9_J		 74	*/
    WIN1254_K,			/*  ISO9_K		 75	*/
    WIN1254_L,			/*  ISO9_L		 76	*/
    WIN1254_M,			/*  ISO9_M		 77	*/
    WIN1254_N,			/*  ISO9_N		 78	*/
    WIN1254_O,			/*  ISO9_O		 79	*/
    WIN1254_P,			/*  ISO9_P		 80	*/
    WIN1254_Q,			/*  ISO9_Q		 81	*/
    WIN1254_R,			/*  ISO9_R		 82	*/
    WIN1254_S,			/*  ISO9_S		 83	*/
    WIN1254_T,			/*  ISO9_T		 84	*/
    WIN1254_U,			/*  ISO9_U		 85	*/
    WIN1254_V,			/*  ISO9_V		 86	*/
    WIN1254_W,			/*  ISO9_W		 87	*/
    WIN1254_X,			/*  ISO9_X		 88	*/
    WIN1254_Y,			/*  ISO9_Y		 89	*/
    WIN1254_Z,			/*  ISO9_Z		 90	*/
    WIN1254_bracketleft,	/*  ISO9_bracketleft	 91	*/
    WIN1254_backslash,		/*  ISO9_backslash	 92	*/
    WIN1254_bracketright,	/*  ISO9_bracketright	 93	*/
    WIN1254_asciicircum,	/*  ISO9_asciicircum	 94	*/
    WIN1254_underscore,		/*  ISO9_underscore	 95	*/
    WIN1254_quoteleft,		/*  ISO9_quoteleft	 96	*/
    WIN1254_a,			/*  ISO9_a		 97	*/
    WIN1254_b,			/*  ISO9_b		 98	*/
    WIN1254_c,			/*  ISO9_c		 99	*/
    WIN1254_d,			/*  ISO9_d		100	*/
    WIN1254_e,			/*  ISO9_e		101	*/
    WIN1254_f,			/*  ISO9_f		102	*/
    WIN1254_g,			/*  ISO9_g		103	*/
    WIN1254_h,			/*  ISO9_h		104	*/
    WIN1254_i,			/*  ISO9_i		105	*/
    WIN1254_j,			/*  ISO9_j		106	*/
    WIN1254_k,			/*  ISO9_k		107	*/
    WIN1254_l,			/*  ISO9_l		108	*/
    WIN1254_m,			/*  ISO9_m		109	*/
    WIN1254_n,			/*  ISO9_n		110	*/
    WIN1254_o,			/*  ISO9_o		111	*/
    WIN1254_p,			/*  ISO9_p		112	*/
    WIN1254_q,			/*  ISO9_q		113	*/
    WIN1254_r,			/*  ISO9_r		114	*/
    WIN1254_s,			/*  ISO9_s		115	*/
    WIN1254_t,			/*  ISO9_t		116	*/
    WIN1254_u,			/*  ISO9_u		117	*/
    WIN1254_v,			/*  ISO9_v		118	*/
    WIN1254_w,			/*  ISO9_w		119	*/
    WIN1254_x,			/*  ISO9_x		120	*/
    WIN1254_y,			/*  ISO9_y		121	*/
    WIN1254_z,			/*  ISO9_z		122	*/
    WIN1254_braceleft,		/*  ISO9_braceleft	123	*/
    WIN1254_bar,		/*  ISO9_bar		124	*/
    WIN1254_braceright,		/*  ISO9_braceright	125	*/
    WIN1254_asciitilde,		/*  ISO9_asciitilde	126	*/
    WIN1254_currency,		/*  .NOTDEF_127		127	*//**/
    WIN1254_currency,		/*  .NOTDEF_128		128	*//**/
    WIN1254_currency,		/*  .NOTDEF_129		129	*//**/
    WIN1254_currency,		/*  .NOTDEF_130		130	*//**/
    WIN1254_currency,		/*  .NOTDEF_131		131	*//**/
    WIN1254_currency,		/*  .NOTDEF_132		132	*//**/
    WIN1254_currency,		/*  .NOTDEF_133		133	*//**/
    WIN1254_currency,		/*  .NOTDEF_134		134	*//**/
    WIN1254_currency,		/*  .NOTDEF_135		135	*//**/
    WIN1254_currency,		/*  .NOTDEF_136		136	*//**/
    WIN1254_currency,		/*  .NOTDEF_137		137	*//**/
    WIN1254_currency,		/*  .NOTDEF_138		138	*//**/
    WIN1254_currency,		/*  .NOTDEF_139		139	*//**/
    WIN1254_currency,		/*  .NOTDEF_140		140	*//**/
    WIN1254_currency,		/*  .NOTDEF_141		141	*//**/
    WIN1254_currency,		/*  .NOTDEF_142		142	*//**/
    WIN1254_currency,		/*  .NOTDEF_143		143	*//**/
    WIN1254_currency,		/*  .NOTDEF_144		144	*//**/
    WIN1254_currency,		/*  .NOTDEF_145		145	*//**/
    WIN1254_currency,		/*  .NOTDEF_146		146	*//**/
    WIN1254_currency,		/*  .NOTDEF_147		147	*//**/
    WIN1254_currency,		/*  .NOTDEF_148		148	*//**/
    WIN1254_currency,		/*  .NOTDEF_149		149	*//**/
    WIN1254_currency,		/*  .NOTDEF_150		150	*//**/
    WIN1254_currency,		/*  .NOTDEF_151		151	*//**/
    WIN1254_currency,		/*  .NOTDEF_152		152	*//**/
    WIN1254_currency,		/*  .NOTDEF_153		153	*//**/
    WIN1254_currency,		/*  .NOTDEF_154		154	*//**/
    WIN1254_currency,		/*  .NOTDEF_155		155	*//**/
    WIN1254_currency,		/*  .NOTDEF_156		156	*//**/
    WIN1254_currency,		/*  .NOTDEF_157		157	*//**/
    WIN1254_currency,		/*  .NOTDEF_158		158	*//**/
    WIN1254_currency,		/*  .NOTDEF_159		159	*//**/
    WIN1254_nobreakspace,	/*  ISO9_space		160	*/
    WIN1254_exclamdown,		/*  ISO9_exclamdown	161	*/
    WIN1254_cent,		/*  ISO9_cent		162	*/
    WIN1254_sterling,		/*  ISO9_sterling	163	*/
    WIN1254_currency,		/*  ISO9_currency	164	*/
    WIN1254_yen,		/*  ISO9_yen		165	*/
    WIN1254_brokenbar,		/*  ISO9_brokenbar	166	*/
    WIN1254_section,		/*  ISO9_section	167	*/
    WIN1254_dieresis,		/*  ISO9_dieresis	168	*/
    WIN1254_copyright,		/*  ISO9_copyright	169	*/
    WIN1254_ordfeminine,	/*  ISO9_ordfeminine	170	*/
    WIN1254_guillemotleft,	/*  ISO9_guillemotleft	171	*/
    WIN1254_logicalnot,		/*  ISO9_logicalnot	172	*/
    WIN1254_hyphen,		/*  ISO9_hyphen		173	*/
    WIN1254_registered,		/*  ISO9_registered	174	*/
    WIN1254_macron,		/*  ISO9_macron		175	*/
    WIN1254_degree,		/*  ISO9_degree		176	*/
    WIN1254_plusminus,		/*  ISO9_plusminus	177	*/
    WIN1254_twosuperior,	/*  ISO9_twosuperior	178	*/
    WIN1254_threesuperior,	/*  ISO9_threesuperior	179	*/
    WIN1254_acute,		/*  ISO9_acute		180	*/
    WIN1254_mu,			/*  ISO9_mu		181	*/
    WIN1254_paragraph,		/*  ISO9_paragraph	182	*/
    WIN1254_periodcentered,	/*  ISO9_periodcentered	183	*/
    WIN1254_cedilla,		/*  ISO9_cedilla	184	*/
    WIN1254_onesuperior,	/*  ISO9_onesuperior	185	*/
    WIN1254_ordmasculine,	/*  ISO9_ordmasculine	186	*/
    WIN1254_guillemotright,	/*  ISO9_guillemotright	187	*/
    WIN1254_onequarter,		/*  ISO9_onequarter	188	*/
    WIN1254_onehalf,		/*  ISO9_onehalf	189	*/
    WIN1254_threequarters,	/*  ISO9_threequarters	190	*/
    WIN1254_questiondown,	/*  ISO9_questiondown	191	*/
    WIN1254_Agrave,		/*  ISO9_Agrave		192	*/
    WIN1254_Aacute,		/*  ISO9_Aacute		193	*/
    WIN1254_Acircumflex,	/*  ISO9_Acircumflex	194	*/
    WIN1254_Atilde,		/*  ISO9_Atilde		195	*/
    WIN1254_Adieresis,		/*  ISO9_Adieresis	196	*/
    WIN1254_Aring,		/*  ISO9_Aring		197	*/
    WIN1254_AE,			/*  ISO9_AE		198	*/
    WIN1254_Ccedilla,		/*  ISO9_Ccedilla	199	*/
    WIN1254_Egrave,		/*  ISO9_Egrave		200	*/
    WIN1254_Eacute,		/*  ISO9_Eacute		201	*/
    WIN1254_Ecircumflex,	/*  ISO9_Ecircumflex	202	*/
    WIN1254_Edieresis,		/*  ISO9_Edieresis	203	*/
    WIN1254_Igrave,		/*  ISO9_Igrave		204	*/
    WIN1254_Iacute,		/*  ISO9_Iacute		205	*/
    WIN1254_Icircumflex,	/*  ISO9_Icircumflex	206	*/
    WIN1254_Idieresis,		/*  ISO9_Idieresis	207	*/
    WIN1254_Gbreve,		/*  ISO9_Gbreve		208	*/
    WIN1254_Ntilde,		/*  ISO9_Ntilde		209	*/
    WIN1254_Ograve,		/*  ISO9_Ograve		210	*/
    WIN1254_Oacute,		/*  ISO9_Oacute		211	*/
    WIN1254_Ocircumflex,	/*  ISO9_Ocircumflex	212	*/
    WIN1254_Otilde,		/*  ISO9_Otilde		213	*/
    WIN1254_Odieresis,		/*  ISO9_Odieresis	214	*/
    WIN1254_multiply,		/*  ISO9_multiply	215	*/
    WIN1254_Oslash,		/*  ISO9_Oslash		216	*/
    WIN1254_Ugrave,		/*  ISO9_Ugrave		217	*/
    WIN1254_Uacute,		/*  ISO9_Uacute		218	*/
    WIN1254_Ucircumflex,	/*  ISO9_Ucircumflex	219	*/
    WIN1254_Udieresis,		/*  ISO9_Udieresis	220	*/
    WIN1254_Idotaccent,		/*  ISO9_Idotaccent	221	*/
    WIN1254_Scedilla,		/*  ISO9_Scedilla	222	*/
    WIN1254_germandbls,		/*  ISO9_germandbls	223	*/
    WIN1254_agrave,		/*  ISO9_agrave		224	*/
    WIN1254_aacute,		/*  ISO9_aacute		225	*/
    WIN1254_acircumflex,	/*  ISO9_acircumflex	226	*/
    WIN1254_atilde,		/*  ISO9_atilde		227	*/
    WIN1254_adieresis,		/*  ISO9_adieresis	228	*/
    WIN1254_aring,		/*  ISO9_aring		229	*/
    WIN1254_ae,			/*  ISO9_ae		230	*/
    WIN1254_ccedilla,		/*  ISO9_ccedilla	231	*/
    WIN1254_egrave,		/*  ISO9_egrave		232	*/
    WIN1254_eacute,		/*  ISO9_eacute		233	*/
    WIN1254_ecircumflex,	/*  ISO9_ecircumflex	234	*/
    WIN1254_edieresis,		/*  ISO9_edieresis	235	*/
    WIN1254_igrave,		/*  ISO9_igrave		236	*/
    WIN1254_iacute,		/*  ISO9_iacute		237	*/
    WIN1254_icircumflex,	/*  ISO9_icircumflex	238	*/
    WIN1254_idieresis,		/*  ISO9_idieresis	239	*/
    WIN1254_gbreve,		/*  ISO9_gbreve		240	*/
    WIN1254_ntilde,		/*  ISO9_ntilde		241	*/
    WIN1254_ograve,		/*  ISO9_ograve		242	*/
    WIN1254_oacute,		/*  ISO9_oacute		243	*/
    WIN1254_ocircumflex,	/*  ISO9_ocircumflex	244	*/
    WIN1254_otilde,		/*  ISO9_otilde		245	*/
    WIN1254_odieresis,		/*  ISO9_odieresis	246	*/
    WIN1254_divide,		/*  ISO9_divide		247	*/
    WIN1254_oslash,		/*  ISO9_oslash		248	*/
    WIN1254_ugrave,		/*  ISO9_ugrave		249	*/
    WIN1254_uacute,		/*  ISO9_uacute		250	*/
    WIN1254_ucircumflex,	/*  ISO9_ucircumflex	251	*/
    WIN1254_udieresis,		/*  ISO9_udieresis	252	*/
    WIN1254_dotlessi,		/*  ISO9_dotlessi	253	*/
    WIN1254_scedilla,		/*  ISO9_scedilla	254	*/
    WIN1254_ydieresis,		/*  ISO9_ydieresis	255	*/
    };
