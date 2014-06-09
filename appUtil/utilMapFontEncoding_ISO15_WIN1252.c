/************************************************************************/
/*									*/
/*  Map Font Encodings ECMA: WIN1252 <-> ISO 8859-15			*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	"psFont.h"
#   include	<charnames.h>

const unsigned char docWIN1252_to_ISO15[256]=
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
    ISO1_space,			/*  WIN1252_space		 32	*/
    ISO1_exclam,		/*  WIN1252_exclam		 33	*/
    ISO1_quotedbl,		/*  WIN1252_quotedbl		 34	*/
    ISO1_numbersign,		/*  WIN1252_numbersign		 35	*/
    ISO1_dollar,		/*  WIN1252_dollar		 36	*/
    ISO1_percent,		/*  WIN1252_percent		 37	*/
    ISO1_ampersand,		/*  WIN1252_ampersand		 38	*/
    ISO1_quotesingle,		/*  WIN1252_quotesingle		 39	*/
    ISO1_parenleft,		/*  WIN1252_parenleft		 40	*/
    ISO1_parenright,		/*  WIN1252_parenright		 41	*/
    ISO1_asterisk,		/*  WIN1252_asterisk		 42	*/
    ISO1_plus,			/*  WIN1252_plus		 43	*/
    ISO1_comma,			/*  WIN1252_comma		 44	*/
    ISO1_minus,			/*  WIN1252_minus		 45	*/
    ISO1_period,		/*  WIN1252_period		 46	*/
    ISO1_slash,			/*  WIN1252_slash		 47	*/
    ISO1_zero,			/*  WIN1252_zero		 48	*/
    ISO1_one,			/*  WIN1252_one			 49	*/
    ISO1_two,			/*  WIN1252_two			 50	*/
    ISO1_three,			/*  WIN1252_three		 51	*/
    ISO1_four,			/*  WIN1252_four		 52	*/
    ISO1_five,			/*  WIN1252_five		 53	*/
    ISO1_six,			/*  WIN1252_six			 54	*/
    ISO1_seven,			/*  WIN1252_seven		 55	*/
    ISO1_eight,			/*  WIN1252_eight		 56	*/
    ISO1_nine,			/*  WIN1252_nine		 57	*/
    ISO1_colon,			/*  WIN1252_colon		 58	*/
    ISO1_semicolon,		/*  WIN1252_semicolon		 59	*/
    ISO1_less,			/*  WIN1252_less		 60	*/
    ISO1_equal,			/*  WIN1252_equal		 61	*/
    ISO1_greater,		/*  WIN1252_greater		 62	*/
    ISO1_question,		/*  WIN1252_question		 63	*/
    ISO1_at,			/*  WIN1252_at			 64	*/
    ISO1_A,			/*  WIN1252_A			 65	*/
    ISO1_B,			/*  WIN1252_B			 66	*/
    ISO1_C,			/*  WIN1252_C			 67	*/
    ISO1_D,			/*  WIN1252_D			 68	*/
    ISO1_E,			/*  WIN1252_E			 69	*/
    ISO1_F,			/*  WIN1252_F			 70	*/
    ISO1_G,			/*  WIN1252_G			 71	*/
    ISO1_H,			/*  WIN1252_H			 72	*/
    ISO1_I,			/*  WIN1252_I			 73	*/
    ISO1_J,			/*  WIN1252_J			 74	*/
    ISO1_K,			/*  WIN1252_K			 75	*/
    ISO1_L,			/*  WIN1252_L			 76	*/
    ISO1_M,			/*  WIN1252_M			 77	*/
    ISO1_N,			/*  WIN1252_N			 78	*/
    ISO1_O,			/*  WIN1252_O			 79	*/
    ISO1_P,			/*  WIN1252_P			 80	*/
    ISO1_Q,			/*  WIN1252_Q			 81	*/
    ISO1_R,			/*  WIN1252_R			 82	*/
    ISO1_S,			/*  WIN1252_S			 83	*/
    ISO1_T,			/*  WIN1252_T			 84	*/
    ISO1_U,			/*  WIN1252_U			 85	*/
    ISO1_V,			/*  WIN1252_V			 86	*/
    ISO1_W,			/*  WIN1252_W			 87	*/
    ISO1_X,			/*  WIN1252_X			 88	*/
    ISO1_Y,			/*  WIN1252_Y			 89	*/
    ISO1_Z,			/*  WIN1252_Z			 90	*/
    ISO1_bracketleft,		/*  WIN1252_bracketleft		 91	*/
    ISO1_backslash,		/*  WIN1252_backslash		 92	*/
    ISO1_bracketright,		/*  WIN1252_bracketright	 93	*/
    ISO1_circumflex,		/*  WIN1252_asciicircum		 94	*/
    ISO1_underscore,		/*  WIN1252_underscore		 95	*/
    ISO1_quoteleft,		/*  WIN1252_quoteleft		 96	*/
    ISO1_a,			/*  WIN1252_a			 97	*/
    ISO1_b,			/*  WIN1252_b			 98	*/
    ISO1_c,			/*  WIN1252_c			 99	*/
    ISO1_d,			/*  WIN1252_d			100	*/
    ISO1_e,			/*  WIN1252_e			101	*/
    ISO1_f,			/*  WIN1252_f			102	*/
    ISO1_g,			/*  WIN1252_g			103	*/
    ISO1_h,			/*  WIN1252_h			104	*/
    ISO1_i,			/*  WIN1252_i			105	*/
    ISO1_j,			/*  WIN1252_j			106	*/
    ISO1_k,			/*  WIN1252_k			107	*/
    ISO1_l,			/*  WIN1252_l			108	*/
    ISO1_m,			/*  WIN1252_m			109	*/
    ISO1_n,			/*  WIN1252_n			110	*/
    ISO1_o,			/*  WIN1252_o			111	*/
    ISO1_p,			/*  WIN1252_p			112	*/
    ISO1_q,			/*  WIN1252_q			113	*/
    ISO1_r,			/*  WIN1252_r			114	*/
    ISO1_s,			/*  WIN1252_s			115	*/
    ISO1_t,			/*  WIN1252_t			116	*/
    ISO1_u,			/*  WIN1252_u			117	*/
    ISO1_v,			/*  WIN1252_v			118	*/
    ISO1_w,			/*  WIN1252_w			119	*/
    ISO1_x,			/*  WIN1252_x			120	*/
    ISO1_y,			/*  WIN1252_y			121	*/
    ISO1_z,			/*  WIN1252_z			122	*/
    ISO1_braceleft,		/*  WIN1252_braceleft		123	*/
    ISO1_bar,			/*  WIN1252_bar			124	*/
    ISO1_braceright,		/*  WIN1252_braceright		125	*/
    ISO1_tilde,			/*  WIN1252_asciitilde		126	*/
    0x7F,			/*  WIN1252__0x7F		127	*//**/
    ISO15_Euro,			/*  WIN1252_Euro		128	*/
    0x81,			/*  WIN1252__0x81		129	*//**/
    ISO1_quoteleft,		/*  WIN1252_quotesinglbase	130	*//**/
    ISO1_multiply,		/*  WIN1252_florin		131	*//**/
    ISO1_quotedbl,		/*  WIN1252_quotedblbase	132	*//**/
    ISO1_multiply,		/*  WIN1252_ellipsis		133	*//**/
    ISO1_multiply,		/*  WIN1252_dagger		134	*//**/
    ISO1_multiply,		/*  WIN1252_daggerdbl		135	*//**/
    ISO1_circumflex,		/*  WIN1252_circumflex		136	*/
    ISO1_multiply,		/*  WIN1252_perthousand		137	*//**/
    ISO15_Scaron,		/*  WIN1252_Scaron		138	*/
    ISO1_guillemotleft,		/*  WIN1252_guilsinglleft	139	*//**/
    ISO15_OE,			/*  WIN1252_OE			140	*/
    0x8D,			/*  WIN1252__0x8D		141	*//**/
    0x8E,			/*  WIN1252__0x8E		142	*//**/
    0x8F,			/*  WIN1252__0x8F		143	*//**/
    0x90,			/*  WIN1252__0x90		144	*//**/
    ISO1_grave,			/*  WIN1252_quotesingle		145	*/
    ISO1_quotesingle,		/*  WIN1252_quoteright		146	*/
    ISO1_quotedbl,		/*  WIN1252_quotedblleft	147	*//**/
    ISO1_quotedbl,		/*  WIN1252_quotedblright	148	*//**/
    ISO1_periodcentered,	/*  WIN1252_bullet		149	*//**/
    ISO1_minus,			/*  WIN1252_endash		150	*//**/
    ISO1_minus,			/*  WIN1252_emdash		151	*//**/
    ISO1_tilde,			/*  WIN1252_tilde		152	*/
    ISO1_multiply,		/*  WIN1252_trademark		153	*//**/
    ISO15_scaron,		/*  WIN1252_scaron		154	*/
    ISO1_guillemotright,	/*  WIN1252_guilsinglright	155	*//**/
    ISO15_oe,			/*  WIN1252_oe			156	*/
    0x9D,			/*  WIN1252__0x9D		157	*//**/
    0x9E,			/*  WIN1252__0x9E		158	*//**/
    ISO15_Ydieresis,		/*  WIN1252_Ydieresis		159	*/
    ISO1_nobreakspace,		/*  WIN1252_nobreakspace	160	*/
    ISO1_exclamdown,		/*  WIN1252_exclamdown		161	*/
    ISO1_cent,			/*  WIN1252_cent		162	*/
    ISO1_sterling,		/*  WIN1252_sterling		163	*/
    ISO1_multiply,		/*  WIN1252_currency		164	*//**/
    ISO1_yen,			/*  WIN1252_yen			165	*/
    ISO1_brokenbar,		/*  WIN1252_brokenbar		166	*/
    ISO1_section,		/*  WIN1252_section		167	*/
    ISO1_dieresis,		/*  WIN1252_dieresis		168	*/
    ISO1_copyright,		/*  WIN1252_copyright		169	*/
    ISO1_ordfeminine,		/*  WIN1252_ordfeminine		170	*/
    ISO1_guillemotleft,		/*  WIN1252_guillemotleft	171	*/
    ISO1_logicalnot,		/*  WIN1252_logicalnot		172	*/
    0xAD,			/*  WIN1252__0xAD		173	*//**/
    ISO1_registered,		/*  WIN1252_registered		174	*/
    ISO1_macron,		/*  WIN1252_macron		175	*/
    ISO1_degree,		/*  WIN1252_degree		176	*/
    ISO1_plusminus,		/*  WIN1252_plusminus		177	*/
    ISO1_twosuperior,		/*  WIN1252_twosuperior		178	*/
    ISO1_threesuperior,		/*  WIN1252_threesuperior	179	*/
    ISO1_acute,			/*  WIN1252_acute		180	*/
    ISO1_mu,			/*  WIN1252_mu			181	*/
    ISO1_paragraph,		/*  WIN1252_paragraph		182	*/
    ISO1_periodcentered,	/*  WIN1252_periodcentered	183	*/
    ISO1_cedilla,		/*  WIN1252_cedilla		184	*/
    ISO1_onesuperior,		/*  WIN1252_onesuperior		185	*/
    ISO1_ordmasculine,		/*  WIN1252_ordmasculine	186	*/
    ISO1_guillemotright,	/*  WIN1252_guillemotright	187	*/
    ISO1_onequarter,		/*  WIN1252_onequarter		188	*/
    ISO1_onehalf,		/*  WIN1252_onehalf		189	*/
    ISO1_threequarters,		/*  WIN1252_threequarters	190	*/
    ISO1_questiondown,		/*  WIN1252_questiondown	191	*/
    ISO1_Agrave,		/*  WIN1252_Agrave		192	*/
    ISO1_Aacute,		/*  WIN1252_Aacute		193	*/
    ISO1_Acircumflex,		/*  WIN1252_Acircumflex		194	*/
    ISO1_Atilde,		/*  WIN1252_Atilde		195	*/
    ISO1_Adieresis,		/*  WIN1252_Adieresis		196	*/
    ISO1_Aring,			/*  WIN1252_Aring		197	*/
    ISO1_AE,			/*  WIN1252_AE			198	*/
    ISO1_Ccedilla,		/*  WIN1252_Ccedilla		199	*/
    ISO1_Egrave,		/*  WIN1252_Egrave		200	*/
    ISO1_Eacute,		/*  WIN1252_Eacute		201	*/
    ISO1_Ecircumflex,		/*  WIN1252_Ecircumflex		202	*/
    ISO1_Edieresis,		/*  WIN1252_Edieresis		203	*/
    ISO1_Igrave,		/*  WIN1252_Igrave		204	*/
    ISO1_Iacute,		/*  WIN1252_Iacute		205	*/
    ISO1_Icircumflex,		/*  WIN1252_Icircumflex		206	*/
    ISO1_Idieresis,		/*  WIN1252_Idieresis		207	*/
    ISO1_Eth,			/*  WIN1252_Eth			208	*/
    ISO1_Ntilde,		/*  WIN1252_Ntilde		209	*/
    ISO1_Ograve,		/*  WIN1252_Ograve		210	*/
    ISO1_Oacute,		/*  WIN1252_Oacute		211	*/
    ISO1_Ocircumflex,		/*  WIN1252_Ocircumflex		212	*/
    ISO1_Otilde,		/*  WIN1252_Otilde		213	*/
    ISO1_Odieresis,		/*  WIN1252_Odieresis		214	*/
    ISO1_multiply,		/*  WIN1252_multiply		215	*/
    ISO1_Oslash,		/*  WIN1252_Oslash		216	*/
    ISO1_Ugrave,		/*  WIN1252_Ugrave		217	*/
    ISO1_Uacute,		/*  WIN1252_Uacute		218	*/
    ISO1_Ucircumflex,		/*  WIN1252_Ucircumflex		219	*/
    ISO1_Udieresis,		/*  WIN1252_Udieresis		220	*/
    ISO1_Yacute,		/*  WIN1252_Yacute		221	*/
    ISO1_Thorn,			/*  WIN1252_Thorn		222	*/
    ISO1_germandbls,		/*  WIN1252_germandbls		223	*/
    ISO1_agrave,		/*  WIN1252_agrave		224	*/
    ISO1_aacute,		/*  WIN1252_aacute		225	*/
    ISO1_acircumflex,		/*  WIN1252_acircumflex		226	*/
    ISO1_atilde,		/*  WIN1252_atilde		227	*/
    ISO1_adieresis,		/*  WIN1252_adieresis		228	*/
    ISO1_aring,			/*  WIN1252_aring		229	*/
    ISO1_ae,			/*  WIN1252_ae			230	*/
    ISO1_ccedilla,		/*  WIN1252_ccedilla		231	*/
    ISO1_egrave,		/*  WIN1252_egrave		232	*/
    ISO1_eacute,		/*  WIN1252_eacute		233	*/
    ISO1_ecircumflex,		/*  WIN1252_ecircumflex		234	*/
    ISO1_edieresis,		/*  WIN1252_edieresis		235	*/
    ISO1_igrave,		/*  WIN1252_igrave		236	*/
    ISO1_iacute,		/*  WIN1252_iacute		237	*/
    ISO1_icircumflex,		/*  WIN1252_icircumflex		238	*/
    ISO1_idieresis,		/*  WIN1252_idieresis		239	*/
    ISO1_eth,			/*  WIN1252_eth			240	*/
    ISO1_ntilde,		/*  WIN1252_ntilde		241	*/
    ISO1_ograve,		/*  WIN1252_ograve		242	*/
    ISO1_oacute,		/*  WIN1252_oacute		243	*/
    ISO1_ocircumflex,		/*  WIN1252_ocircumflex		244	*/
    ISO1_otilde,		/*  WIN1252_otilde		245	*/
    ISO1_odieresis,		/*  WIN1252_odieresis		246	*/
    ISO1_divide,		/*  WIN1252_divide		247	*/
    ISO1_oslash,		/*  WIN1252_oslash		248	*/
    ISO1_ugrave,		/*  WIN1252_ugrave		249	*/
    ISO1_uacute,		/*  WIN1252_uacute		250	*/
    ISO1_ucircumflex,		/*  WIN1252_ucircumflex		251	*/
    ISO1_udieresis,		/*  WIN1252_udieresis		252	*/
    ISO1_yacute,		/*  WIN1252_yacute		253	*/
    ISO1_thorn,			/*  WIN1252_thorn		254	*/
    ISO1_ydieresis,		/*  WIN1252_ydieresis		255	*/
    };

const unsigned char docISO15_to_WIN1252[256]=
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
    WIN1252_space,		/*  ISO15_space			 32	*/
    WIN1252_exclam,		/*  ISO15_exclam		 33	*/
    WIN1252_quotedbl,		/*  ISO15_quotedbl		 34	*/
    WIN1252_numbersign,		/*  ISO15_numbersign		 35	*/
    WIN1252_dollar,		/*  ISO15_dollar		 36	*/
    WIN1252_percent,		/*  ISO15_percent		 37	*/
    WIN1252_ampersand,		/*  ISO15_ampersand		 38	*/
    WIN1252_quotesingle,	/*  ISO15_quotesingle		 39	*/
    WIN1252_parenleft,		/*  ISO15_parenleft		 40	*/
    WIN1252_parenright,		/*  ISO15_parenright		 41	*/
    WIN1252_asterisk,		/*  ISO15_asterisk		 42	*/
    WIN1252_plus,		/*  ISO15_plus			 43	*/
    WIN1252_comma,		/*  ISO15_comma			 44	*/
    WIN1252_minus,		/*  ISO15_hyphen		 45	*/
    WIN1252_period,		/*  ISO15_period		 46	*/
    WIN1252_slash,		/*  ISO15_slash			 47	*/
    WIN1252_zero,		/*  ISO15_zero			 48	*/
    WIN1252_one,		/*  ISO15_one			 49	*/
    WIN1252_two,		/*  ISO15_two			 50	*/
    WIN1252_three,		/*  ISO15_three			 51	*/
    WIN1252_four,		/*  ISO15_four			 52	*/
    WIN1252_five,		/*  ISO15_five			 53	*/
    WIN1252_six,		/*  ISO15_six			 54	*/
    WIN1252_seven,		/*  ISO15_seven			 55	*/
    WIN1252_eight,		/*  ISO15_eight			 56	*/
    WIN1252_nine,		/*  ISO15_nine			 57	*/
    WIN1252_colon,		/*  ISO15_colon			 58	*/
    WIN1252_semicolon,		/*  ISO15_semicolon		 59	*/
    WIN1252_less,		/*  ISO15_less			 60	*/
    WIN1252_equal,		/*  ISO15_equal			 61	*/
    WIN1252_greater,		/*  ISO15_greater		 62	*/
    WIN1252_question,		/*  ISO15_question		 63	*/
    WIN1252_at,			/*  ISO15_at			 64	*/
    WIN1252_A,			/*  ISO15_A			 65	*/
    WIN1252_B,			/*  ISO15_B			 66	*/
    WIN1252_C,			/*  ISO15_C			 67	*/
    WIN1252_D,			/*  ISO15_D			 68	*/
    WIN1252_E,			/*  ISO15_E			 69	*/
    WIN1252_F,			/*  ISO15_F			 70	*/
    WIN1252_G,			/*  ISO15_G			 71	*/
    WIN1252_H,			/*  ISO15_H			 72	*/
    WIN1252_I,			/*  ISO15_I			 73	*/
    WIN1252_J,			/*  ISO15_J			 74	*/
    WIN1252_K,			/*  ISO15_K			 75	*/
    WIN1252_L,			/*  ISO15_L			 76	*/
    WIN1252_M,			/*  ISO15_M			 77	*/
    WIN1252_N,			/*  ISO15_N			 78	*/
    WIN1252_O,			/*  ISO15_O			 79	*/
    WIN1252_P,			/*  ISO15_P			 80	*/
    WIN1252_Q,			/*  ISO15_Q			 81	*/
    WIN1252_R,			/*  ISO15_R			 82	*/
    WIN1252_S,			/*  ISO15_S			 83	*/
    WIN1252_T,			/*  ISO15_T			 84	*/
    WIN1252_U,			/*  ISO15_U			 85	*/
    WIN1252_V,			/*  ISO15_V			 86	*/
    WIN1252_W,			/*  ISO15_W			 87	*/
    WIN1252_X,			/*  ISO15_X			 88	*/
    WIN1252_Y,			/*  ISO15_Y			 89	*/
    WIN1252_Z,			/*  ISO15_Z			 90	*/
    WIN1252_bracketleft,	/*  ISO15_bracketleft		 91	*/
    WIN1252_backslash,		/*  ISO15_backslash		 92	*/
    WIN1252_bracketright,	/*  ISO15_bracketright		 93	*/
    WIN1252_asciicircum,	/*  ISO15_asciicircum		 94	*/
    WIN1252_underscore,		/*  ISO15_underscore		 95	*/
    WIN1252_quoteleft,		/*  ISO15_quoteleft		 96	*/
    WIN1252_a,			/*  ISO15_a			 97	*/
    WIN1252_b,			/*  ISO15_b			 98	*/
    WIN1252_c,			/*  ISO15_c			 99	*/
    WIN1252_d,			/*  ISO15_d			100	*/
    WIN1252_e,			/*  ISO15_e			101	*/
    WIN1252_f,			/*  ISO15_f			102	*/
    WIN1252_g,			/*  ISO15_g			103	*/
    WIN1252_h,			/*  ISO15_h			104	*/
    WIN1252_i,			/*  ISO15_i			105	*/
    WIN1252_j,			/*  ISO15_j			106	*/
    WIN1252_k,			/*  ISO15_k			107	*/
    WIN1252_l,			/*  ISO15_l			108	*/
    WIN1252_m,			/*  ISO15_m			109	*/
    WIN1252_n,			/*  ISO15_n			110	*/
    WIN1252_o,			/*  ISO15_o			111	*/
    WIN1252_p,			/*  ISO15_p			112	*/
    WIN1252_q,			/*  ISO15_q			113	*/
    WIN1252_r,			/*  ISO15_r			114	*/
    WIN1252_s,			/*  ISO15_s			115	*/
    WIN1252_t,			/*  ISO15_t			116	*/
    WIN1252_u,			/*  ISO15_u			117	*/
    WIN1252_v,			/*  ISO15_v			118	*/
    WIN1252_w,			/*  ISO15_w			119	*/
    WIN1252_x,			/*  ISO15_x			120	*/
    WIN1252_y,			/*  ISO15_y			121	*/
    WIN1252_z,			/*  ISO15_z			122	*/
    WIN1252_braceleft,		/*  ISO15_braceleft		123	*/
    WIN1252_bar,		/*  ISO15_bar			124	*/
    WIN1252_braceright,		/*  ISO15_braceright		125	*/
    WIN1252_asciitilde,		/*  ISO15_asciitilde		126	*/
    0x7F,			/*  ISO15__0x7F			127	*/
    WIN1252_Euro,		/*  ISO15_Euro			128	*/
    0x81,			/*  ISO15__0x81			129	*/
    0x82,			/*  ISO15__0x82			130	*//**/
    0x83,			/*  ISO15__0x83			131	*//**/
    0x84,			/*  ISO15__0x84			132	*//**/
    0x85,			/*  ISO15__0x85			133	*//**/
    0x86,			/*  ISO15__0x86			134	*//**/
    0x87,			/*  ISO15__0x87			135	*//**/
    0x88,			/*  ISO15__0x88			136	*//**/
    0x89,			/*  ISO15__0x89			137	*//**/
    0x8A,			/*  ISO15__0x8A			138	*//**/
    0x8B,			/*  ISO15__0x8B			139	*//**/
    0x8C,			/*  ISO15__0x8C			140	*//**/
    0x8D,			/*  ISO15__0x8D			141	*/
    0x8E,			/*  ISO15__0x8E			142	*/
    0x8F,			/*  ISO15__0x8F			143	*/
    0x90,			/*  ISO15__0x90			144	*/
    0x91,			/*  ISO15__0x91			145	*//**/
    0x92,			/*  ISO15__0x92			146	*//**/
    0x93,			/*  ISO15__0x93			147	*//**/
    0x94,			/*  ISO15__0x94			148	*//**/
    0x95,			/*  ISO15__0x95			149	*//**/
    0x96,			/*  ISO15__0x96			150	*//**/
    0x97,			/*  ISO15__0x97			151	*//**/
    0x98,			/*  ISO15__0x98			152	*//**/
    0x99,			/*  ISO15__0x99			153	*//**/
    0x9A,			/*  ISO15__0x9A			154	*//**/
    0x9B,			/*  ISO15__0x9B			155	*//**/
    0x9C,			/*  ISO15__0x9C			156	*//**/
    0x9D,			/*  ISO15__0x9D			157	*/
    0x9E,			/*  ISO15__0x9E			158	*/
    0x9F,			/*  ISO15__0x9F			159	*//**/
    WIN1252_nobreakspace,	/*  ISO15_nobreakspace		160	*/
    WIN1252_exclamdown,		/*  ISO15_exclamdown		161	*/
    WIN1252_cent,		/*  ISO15_cent			162	*/
    WIN1252_sterling,		/*  ISO15_sterling		163	*/
    WIN1252_Euro,		/*  ISO15_Euro			164	*/
    WIN1252_yen,		/*  ISO15_yen			165	*/
    WIN1252_Scaron,		/*  ISO15_Scaron		166	*/
    WIN1252_section,		/*  ISO15_section		167	*/
    WIN1252_scaron,		/*  ISO15_scaron		168	*/
    WIN1252_copyright,		/*  ISO15_copyright		169	*/
    WIN1252_ordfeminine,	/*  ISO15_ordfeminine		170	*/
    WIN1252_guillemotleft,	/*  ISO15_guillemotleft		171	*/
    WIN1252_logicalnot,		/*  ISO15_logicalnot		172	*/
    0xAD,			/*  ISO15__0xAD			173	*/
    WIN1252_registered,		/*  ISO15_registered		174	*/
    WIN1252_macron,		/*  ISO15_macron		175	*/
    WIN1252_degree,		/*  ISO15_degree		176	*/
    WIN1252_plusminus,		/*  ISO15_plusminus		177	*/
    WIN1252_twosuperior,	/*  ISO15_twosuperior		178	*/
    WIN1252_threesuperior,	/*  ISO15_threesuperior		179	*/
    WIN1252_Z,			/*  ISO15_Zcaron		180	*//**/
    WIN1252_mu,			/*  ISO15_mu			181	*/
    WIN1252_paragraph,		/*  ISO15_paragraph		182	*/
    WIN1252_periodcentered,	/*  ISO15_periodcentered	183	*/
    WIN1252_z,			/*  ISO15_zcaron		184	*//**/
    WIN1252_onesuperior,	/*  ISO15_onesuperior		185	*/
    WIN1252_ordmasculine,	/*  ISO15_ordmasculine		186	*/
    WIN1252_guillemotright,	/*  ISO15_guillemotright	187	*/
    WIN1252_OE,			/*  ISO15_OE			188	*/
    WIN1252_oe,			/*  ISO15_oe			189	*/
    WIN1252_Ydieresis,		/*  ISO15_Ydieresis		190	*/
    WIN1252_questiondown,	/*  ISO15_questiondown		191	*/
    WIN1252_Agrave,		/*  ISO15_Agrave		192	*/
    WIN1252_Aacute,		/*  ISO15_Aacute		193	*/
    WIN1252_Acircumflex,	/*  ISO15_Acircumflex		194	*/
    WIN1252_Atilde,		/*  ISO15_Atilde		195	*/
    WIN1252_Adieresis,		/*  ISO15_Adieresis		196	*/
    WIN1252_Aring,		/*  ISO15_Aring			197	*/
    WIN1252_AE,			/*  ISO15_AE			198	*/
    WIN1252_Ccedilla,		/*  ISO15_Ccedilla		199	*/
    WIN1252_Egrave,		/*  ISO15_Egrave		200	*/
    WIN1252_Eacute,		/*  ISO15_Eacute		201	*/
    WIN1252_Ecircumflex,	/*  ISO15_Ecircumflex		202	*/
    WIN1252_Edieresis,		/*  ISO15_Edieresis		203	*/
    WIN1252_Igrave,		/*  ISO15_Igrave		204	*/
    WIN1252_Iacute,		/*  ISO15_Iacute		205	*/
    WIN1252_Icircumflex,	/*  ISO15_Icircumflex		206	*/
    WIN1252_Idieresis,		/*  ISO15_Idieresis		207	*/
    WIN1252_Eth,		/*  ISO15_Eth			208	*/
    WIN1252_Ntilde,		/*  ISO15_Ntilde		209	*/
    WIN1252_Ograve,		/*  ISO15_Ograve		210	*/
    WIN1252_Oacute,		/*  ISO15_Oacute		211	*/
    WIN1252_Ocircumflex,	/*  ISO15_Ocircumflex		212	*/
    WIN1252_Otilde,		/*  ISO15_Otilde		213	*/
    WIN1252_Odieresis,		/*  ISO15_Odieresis		214	*/
    WIN1252_multiply,		/*  ISO15_multiply		215	*/
    WIN1252_Oslash,		/*  ISO15_Oslash		216	*/
    WIN1252_Ugrave,		/*  ISO15_Ugrave		217	*/
    WIN1252_Uacute,		/*  ISO15_Uacute		218	*/
    WIN1252_Ucircumflex,	/*  ISO15_Ucircumflex		219	*/
    WIN1252_Udieresis,		/*  ISO15_Udieresis		220	*/
    WIN1252_Yacute,		/*  ISO15_Yacute		221	*/
    WIN1252_Thorn,		/*  ISO15_Thorn			222	*/
    WIN1252_germandbls,		/*  ISO15_germandbls		223	*/
    WIN1252_agrave,		/*  ISO15_agrave		224	*/
    WIN1252_aacute,		/*  ISO15_aacute		225	*/
    WIN1252_acircumflex,	/*  ISO15_acircumflex		226	*/
    WIN1252_atilde,		/*  ISO15_atilde		227	*/
    WIN1252_adieresis,		/*  ISO15_adieresis		228	*/
    WIN1252_aring,		/*  ISO15_aring			229	*/
    WIN1252_ae,			/*  ISO15_ae			230	*/
    WIN1252_ccedilla,		/*  ISO15_ccedilla		231	*/
    WIN1252_egrave,		/*  ISO15_egrave		232	*/
    WIN1252_eacute,		/*  ISO15_eacute		233	*/
    WIN1252_ecircumflex,	/*  ISO15_ecircumflex		234	*/
    WIN1252_edieresis,		/*  ISO15_edieresis		235	*/
    WIN1252_igrave,		/*  ISO15_igrave		236	*/
    WIN1252_iacute,		/*  ISO15_iacute		237	*/
    WIN1252_icircumflex,	/*  ISO15_icircumflex		238	*/
    WIN1252_idieresis,		/*  ISO15_idieresis		239	*/
    WIN1252_eth,		/*  ISO15_eth			240	*/
    WIN1252_ntilde,		/*  ISO15_ntilde		241	*/
    WIN1252_ograve,		/*  ISO15_ograve		242	*/
    WIN1252_oacute,		/*  ISO15_oacute		243	*/
    WIN1252_ocircumflex,	/*  ISO15_ocircumflex		244	*/
    WIN1252_otilde,		/*  ISO15_otilde		245	*/
    WIN1252_odieresis,		/*  ISO15_odieresis		246	*/
    WIN1252_divide,		/*  ISO15_divide		247	*/
    WIN1252_oslash,		/*  ISO15_oslash		248	*/
    WIN1252_ugrave,		/*  ISO15_ugrave		249	*/
    WIN1252_uacute,		/*  ISO15_uacute		250	*/
    WIN1252_ucircumflex,	/*  ISO15_ucircumflex		251	*/
    WIN1252_udieresis,		/*  ISO15_udieresis		252	*/
    WIN1252_yacute,		/*  ISO15_yacute		253	*/
    WIN1252_thorn,		/*  ISO15_thorn			254	*/
    WIN1252_ydieresis,		/*  ISO15_ydieresis		255	*/
    };

