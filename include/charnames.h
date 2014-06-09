/************************************************************************/
/*									*/
/*  ASCII, Names are the Postscript Glyph names, For the Control	*/
/*	characters, the names in /usr/pub/ascii are used.		*/
/*									*/
/*  ECMA 94 Latin 1 character Set Chart (= ISO 8859-1).			*/
/*  ASCII+ ISOLatin1Encoding.						*/
/*  Spelling follows the Postscipt Glyph Name, (prefixed with 'ISO1_'.)	*/
/*  Glyph names are from appendix E.5 (p. 495) of:			*/
/*  Adobe Systems: Postscript Language Reference Manual, Addison &	*/
/*	Wesley, Reading MA., 1990, ISBN 0-201-18127-4			*/
/*									*/
/*  Idem for Standard Encoding of Postscript.				*/
/*									*/
/*  Idem for NeXT Encoding of Postscript.				*/
/*									*/
/*  Idem for Apple Macintosh ROM character set.				*/
/*									*/
/*  Idem for EBCDIC for ibm 3270.					*/
/*	This is more or less guessed. Consulted are:			*/
/*	1)  Character map in x3270 source.				*/
/*	2)  Unicode Standard, Appendix 6.2, PC EBCDIC Code Page (p. 605)*/
/*	    Unicode Consortium: Unicode Standard, Version 1.0, Volume 1,*/
/*		Addidon & Wesley, Reading MA., 1991, ISBN 0-201-56788-1	*/
/*									*/
/*  Attempt to standardize naming of quotes:				*/
/*	quotesingle= u:0027= 39 (ascii quote)				*/
/*	quotedbl= u:0022= 34 (ascii double quote)			*/
/*									*/
/************************************************************************/

#   define	CHARisUPPER	0x1
#   define	CHARisLOWER	0x2
#   define	CHARisDIGIT	0x4
#   define	CHARisALPHA	(CHARisUPPER|CHARisLOWER)
#   define	CHARisALNUM	(CHARisALPHA|CHARisDIGIT)

/************************************************************************/
/*									*/
/*  ASCII								*/
/*									*/
/************************************************************************/

#   define	ASCII__nul		0x00	/*  0			*/
#   define	ASCII__soh		0x01	/*  1			*/
#   define	ASCII__stx		0x02	/*  2			*/
#   define	ASCII__etx		0x03	/*  3			*/
#   define	ASCII__eot		0x04	/*  4			*/
#   define	ASCII__enq		0x05	/*  5			*/
#   define	ASCII__ack		0x06	/*  6			*/
#   define	ASCII__bel		0x07	/*  7			*/
#   define	ASCII__bs		0x08	/*  8			*/
#   define	ASCII__ht		0x09	/*  9			*/
#   define	ASCII__nl		0x0a	/*  10			*/
#   define	ASCII__vt		0x0b	/*  11			*/
#   define	ASCII__np		0x0c	/*  12			*/
#   define	ASCII__cr		0x0d	/*  13			*/
#   define	ASCII__so		0x0e	/*  14			*/
#   define	ASCII__si		0x0f	/*  15			*/
#   define	ASCII__dle		0x10	/*  16			*/
#   define	ASCII__dc1		0x11	/*  17			*/
#   define	ASCII__dc2		0x12	/*  18			*/
#   define	ASCII__dc3		0x13	/*  19			*/
#   define	ASCII__dc4		0x14	/*  20			*/
#   define	ASCII__nak		0x15	/*  21			*/
#   define	ASCII__syn		0x16	/*  22			*/
#   define	ASCII__etb		0x17	/*  23			*/
#   define	ASCII__can		0x18	/*  24			*/
#   define	ASCII__em		0x19	/*  25			*/
#   define	ASCII__sub		0x1a	/*  26			*/
#   define	ASCII__esc		0x1b	/*  27			*/
#   define	ASCII__fs		0x1c	/*  28			*/
#   define	ASCII__gs		0x1d	/*  29			*/
#   define	ASCII__rs		0x1e	/*  30			*/
#   define	ASCII__us		0x1f	/*  31			*/

#   define	ASCII_space		0x20	/*  32			*/
#   define	ASCII_exclam		0x21	/*  33			*/
#   define	ASCII_quotedbl		0x22	/*  34			*/
#   define	ASCII_numbersign	0x23	/*  35			*/
#   define	ASCII_dollar		0x24	/*  36			*/
#   define	ASCII_percent		0x25	/*  37			*/
#   define	ASCII_ampersand		0x26	/*  38			*/
#   define	ASCII_quotesingle	0x27	/*  39			*/
#   define	ASCII_parenleft		0x28	/*  40			*/
#   define	ASCII_parenright	0x29	/*  41			*/
#   define	ASCII_asterisk		0x2a	/*  42			*/
#   define	ASCII_plus		0x2b	/*  43			*/
#   define	ASCII_comma		0x2c	/*  44			*/
#   define	ASCII_minus		0x2d	/*  45			*/
#   define	ASCII_period		0x2e	/*  46			*/
#   define	ASCII_slash		0x2f	/*  47			*/
#   define	ASCII_zero		0x30	/*  48			*/
#   define	ASCII_one		0x31	/*  49			*/
#   define	ASCII_two		0x32	/*  50			*/
#   define	ASCII_three		0x33	/*  51			*/
#   define	ASCII_four		0x34	/*  52			*/
#   define	ASCII_five		0x35	/*  53			*/
#   define	ASCII_six		0x36	/*  54			*/
#   define	ASCII_seven		0x37	/*  55			*/
#   define	ASCII_eight		0x38	/*  56			*/
#   define	ASCII_nine		0x39	/*  57			*/
#   define	ASCII_colon		0x3a	/*  58			*/
#   define	ASCII_semicolon		0x3b	/*  59			*/
#   define	ASCII_less		0x3c	/*  60			*/
#   define	ASCII_equal		0x3d	/*  61			*/
#   define	ASCII_greater		0x3e	/*  62			*/
#   define	ASCII_question		0x3f	/*  63			*/

#   define	ASCII_at		0x40	/*  64			*/
#   define	ASCII_A			0x41	/*  65			*/
#   define	ASCII_B			0x42	/*  66			*/
#   define	ASCII_C			0x43	/*  67			*/
#   define	ASCII_D			0x44	/*  68			*/
#   define	ASCII_E			0x45	/*  69			*/
#   define	ASCII_F			0x46	/*  70			*/
#   define	ASCII_G			0x47	/*  71			*/
#   define	ASCII_H			0x48	/*  72			*/
#   define	ASCII_I			0x49	/*  73			*/
#   define	ASCII_J			0x4a	/*  74			*/
#   define	ASCII_K			0x4b	/*  75			*/
#   define	ASCII_L			0x4c	/*  76			*/
#   define	ASCII_M			0x4d	/*  77			*/
#   define	ASCII_N			0x4e	/*  78			*/
#   define	ASCII_O			0x4f	/*  79			*/
#   define	ASCII_P			0x50	/*  80			*/
#   define	ASCII_Q			0x51	/*  81			*/
#   define	ASCII_R			0x52	/*  82			*/
#   define	ASCII_S			0x53	/*  83			*/
#   define	ASCII_T			0x54	/*  84			*/
#   define	ASCII_U			0x55	/*  85			*/
#   define	ASCII_V			0x56	/*  86			*/
#   define	ASCII_W			0x57	/*  87			*/
#   define	ASCII_X			0x58	/*  88			*/
#   define	ASCII_Y			0x59	/*  89			*/
#   define	ASCII_Z			0x5a	/*  90			*/
#   define	ASCII_bracketleft	0x5b	/*  91			*/
#   define	ASCII_backslash		0x5c	/*  92			*/
#   define	ASCII_bracketright	0x5d	/*  93			*/
#   define	ASCII_circumflex	0x5e	/*  94			*/
#   define	ASCII_underscore	0x5f	/*  95			*/

#   define	ASCII_quoteleft		0x60	/*  96			*/
#   define	ASCII_a			0x61	/*  97			*/
#   define	ASCII_b			0x62	/*  98			*/
#   define	ASCII_c			0x63	/*  99			*/
#   define	ASCII_d			0x64	/*  100			*/
#   define	ASCII_e			0x65	/*  101			*/
#   define	ASCII_f			0x66	/*  102			*/
#   define	ASCII_g			0x67	/*  103			*/
#   define	ASCII_h			0x68	/*  104			*/
#   define	ASCII_i			0x69	/*  105			*/
#   define	ASCII_j			0x6a	/*  106			*/
#   define	ASCII_k			0x6b	/*  107			*/
#   define	ASCII_l			0x6c	/*  108			*/
#   define	ASCII_m			0x6d	/*  109			*/
#   define	ASCII_n			0x6e	/*  110			*/
#   define	ASCII_o			0x6f	/*  111			*/
#   define	ASCII_p			0x70	/*  112			*/
#   define	ASCII_q			0x71	/*  113			*/
#   define	ASCII_r			0x72	/*  114			*/
#   define	ASCII_s			0x73	/*  115			*/
#   define	ASCII_t			0x74	/*  116			*/
#   define	ASCII_u			0x75	/*  117			*/
#   define	ASCII_v			0x76	/*  118			*/
#   define	ASCII_w			0x77	/*  119			*/
#   define	ASCII_x			0x78	/*  120			*/
#   define	ASCII_y			0x79	/*  121			*/
#   define	ASCII_z			0x7a	/*  122			*/
#   define	ASCII_braceleft		0x7b	/*  123			*/
#   define	ASCII_bar		0x7c	/*  124			*/
#   define	ASCII_braceright	0x7d	/*  125			*/
#   define	ASCII_tilde		0x7e	/*  126			*/
#   define	ASCII__del		0x7f	/*  127			*/

#   define	ASCII_isupper( c )	((c)>=ASCII_A&&(c)<=ASCII_Z)
#   define	ASCII_islower( c )	((c)>=ASCII_a&&(c)<=ASCII_z)
#   define	ASCII_isdigit( c )	((c)>=ASCII_zero&&(c)<=ASCII_nine)
#   define	ASCII_isalpha( c )	(ASCII_isupper((c))||ASCII_islower((c)))
#   define	ASCII_tolower( c )	(ASCII_isupper((c))?(c)+0x20:(c))
#   define	ASCII_toupper( c )	(ASCII_islower((c))?(c)-0x20:(c))

/************************************************************************/
/*									*/
/*  ISO Latin1 (ISO 8859-1)						*/
/*									*/
/************************************************************************/

#   define ISO1__nul		ASCII__nul	/*  0		*/
#   define ISO1__soh		ASCII__soh	/*  1		*/
#   define ISO1__stx		ASCII__stx	/*  2		*/
#   define ISO1__etx		ASCII__etx	/*  3		*/
#   define ISO1__eot		ASCII__eot	/*  4		*/
#   define ISO1__enq		ASCII__enq	/*  5		*/
#   define ISO1__ack		ASCII__ack	/*  6		*/
#   define ISO1__bel		ASCII__bel	/*  7		*/
#   define ISO1__bs		ASCII__bs	/*  8		*/
#   define ISO1__ht		ASCII__ht	/*  9		*/
#   define ISO1__nl		ASCII__nl	/*  10		*/
#   define ISO1__vt		ASCII__vt	/*  11		*/
#   define ISO1__np		ASCII__np	/*  12		*/
#   define ISO1__cr		ASCII__cr	/*  13		*/
#   define ISO1__so		ASCII__so	/*  14		*/
#   define ISO1__si		ASCII__si	/*  15		*/
#   define ISO1__dle		ASCII__dle	/*  16		*/
#   define ISO1__dc1		ASCII__dc1	/*  17		*/
#   define ISO1__dc2		ASCII__dc2	/*  18		*/
#   define ISO1__dc3		ASCII__dc3	/*  19		*/
#   define ISO1__dc4		ASCII__dc4	/*  20		*/
#   define ISO1__nak		ASCII__nak	/*  21		*/
#   define ISO1__syn		ASCII__syn	/*  22		*/
#   define ISO1__etb		ASCII__etb	/*  23		*/
#   define ISO1__can		ASCII__can	/*  24		*/
#   define ISO1__em		ASCII__em	/*  25		*/
#   define ISO1__sub		ASCII__sub	/*  26		*/
#   define ISO1__esc		ASCII__esc	/*  27		*/
#   define ISO1__fs		ASCII__fs	/*  28		*/
#   define ISO1__gs		ASCII__gs	/*  29		*/
#   define ISO1__rs		ASCII__rs	/*  30		*/
#   define ISO1__us		ASCII__us	/*  31		*/

#   define ISO1_space		ASCII_space	/*  32		*/
#   define ISO1_exclam		ASCII_exclam	/*  33		*/
#   define ISO1_quotedbl	ASCII_quotedbl	/*  34		*/
#   define ISO1_numbersign	ASCII_numbersign /*  35		*/
#   define ISO1_dollar		ASCII_dollar	/*  36		*/
#   define ISO1_percent		ASCII_percent	/*  37		*/
#   define ISO1_ampersand	ASCII_ampersand	/*  38		*/
#   define ISO1_quotesingle	ASCII_quotesingle /*  39		*/
#   define ISO1_parenleft	ASCII_parenleft	/*  40		*/
#   define ISO1_parenright	ASCII_parenright /*  41		*/
#   define ISO1_asterisk	ASCII_asterisk	/*  42		*/
#   define ISO1_plus		ASCII_plus	/*  43		*/
#   define ISO1_comma		ASCII_comma	/*  44		*/
#   define ISO1_minus		ASCII_minus	/*  45		*/
#   define ISO1_period		ASCII_period	/*  46		*/
#   define ISO1_slash		ASCII_slash	/*  47		*/
#   define ISO1_zero		ASCII_zero	/*  48		*/
#   define ISO1_one		ASCII_one	/*  49		*/
#   define ISO1_two		ASCII_two	/*  50		*/
#   define ISO1_three		ASCII_three	/*  51		*/
#   define ISO1_four		ASCII_four	/*  52		*/
#   define ISO1_five		ASCII_five	/*  53		*/
#   define ISO1_six		ASCII_six	/*  54		*/
#   define ISO1_seven		ASCII_seven	/*  55		*/
#   define ISO1_eight		ASCII_eight	/*  56		*/
#   define ISO1_nine		ASCII_nine	/*  57		*/
#   define ISO1_colon		ASCII_colon	/*  58		*/
#   define ISO1_semicolon	ASCII_semicolon	/*  59		*/
#   define ISO1_less		ASCII_less	/*  60		*/
#   define ISO1_equal		ASCII_equal	/*  61		*/
#   define ISO1_greater		ASCII_greater	/*  62		*/
#   define ISO1_question	ASCII_question	/*  63		*/

#   define ISO1_at		ASCII_at	/*  64		*/
#   define ISO1_A		ASCII_A		/*  65		*/
#   define ISO1_B		ASCII_B		/*  66		*/
#   define ISO1_C		ASCII_C		/*  67		*/
#   define ISO1_D		ASCII_D		/*  68		*/
#   define ISO1_E		ASCII_E		/*  69		*/
#   define ISO1_F		ASCII_F		/*  70		*/
#   define ISO1_G		ASCII_G		/*  71		*/
#   define ISO1_H		ASCII_H		/*  72		*/
#   define ISO1_I		ASCII_I		/*  73		*/
#   define ISO1_J		ASCII_J		/*  74		*/
#   define ISO1_K		ASCII_K		/*  75		*/
#   define ISO1_L		ASCII_L		/*  76		*/
#   define ISO1_M		ASCII_M		/*  77		*/
#   define ISO1_N		ASCII_N		/*  78		*/
#   define ISO1_O		ASCII_O		/*  79		*/
#   define ISO1_P		ASCII_P		/*  80		*/
#   define ISO1_Q		ASCII_Q		/*  81		*/
#   define ISO1_R		ASCII_R		/*  82		*/
#   define ISO1_S		ASCII_S		/*  83		*/
#   define ISO1_T		ASCII_T		/*  84		*/
#   define ISO1_U		ASCII_U		/*  85		*/
#   define ISO1_V		ASCII_V		/*  86		*/
#   define ISO1_W		ASCII_W		/*  87		*/
#   define ISO1_X		ASCII_X		/*  88		*/
#   define ISO1_Y		ASCII_Y		/*  89		*/
#   define ISO1_Z		ASCII_Z		/*  90		*/
#   define ISO1_bracketleft	ASCII_bracketleft /* 91		*/
#   define ISO1_backslash	ASCII_backslash	/*  92		*/
#   define ISO1_bracketright	ASCII_bracketright /* 93	*/
#   define ISO1_circumflex	ASCII_circumflex /*  94		*/
#   define ISO1_underscore	ASCII_underscore /*  95		*/

#   define ISO1_quoteleft	ASCII_quoteleft	/*  96		*/
#   define ISO1_a		ASCII_a		/*  97		*/
#   define ISO1_b		ASCII_b		/*  98		*/
#   define ISO1_c		ASCII_c		/*  99		*/
#   define ISO1_d		ASCII_d		/*  100		*/
#   define ISO1_e		ASCII_e		/*  101		*/
#   define ISO1_f		ASCII_f		/*  102		*/
#   define ISO1_g		ASCII_g		/*  103		*/
#   define ISO1_h		ASCII_h		/*  104		*/
#   define ISO1_i		ASCII_i		/*  105		*/
#   define ISO1_j		ASCII_j		/*  106		*/
#   define ISO1_k		ASCII_k		/*  107		*/
#   define ISO1_l		ASCII_l		/*  108		*/
#   define ISO1_m		ASCII_m		/*  109		*/
#   define ISO1_n		ASCII_n		/*  110		*/
#   define ISO1_o		ASCII_o		/*  111		*/
#   define ISO1_p		ASCII_p		/*  112		*/
#   define ISO1_q		ASCII_q		/*  113		*/
#   define ISO1_r		ASCII_r		/*  114		*/
#   define ISO1_s		ASCII_s		/*  115		*/
#   define ISO1_t		ASCII_t		/*  116		*/
#   define ISO1_u		ASCII_u		/*  117		*/
#   define ISO1_v		ASCII_v		/*  118		*/
#   define ISO1_w		ASCII_w		/*  119		*/
#   define ISO1_x		ASCII_x		/*  120		*/
#   define ISO1_y		ASCII_y		/*  121		*/
#   define ISO1_z		ASCII_z		/*  122		*/
#   define ISO1_braceleft	ASCII_braceleft	/*  123		*/
#   define ISO1_bar		ASCII_bar	/*  124		*/
#   define ISO1_braceright	ASCII_braceright /* 125		*/
#   define ISO1_tilde		ASCII_tilde	/*  126		*/
#   define ISO1__del		ASCII__del	/*  127		*/

#   define	ISO1__NOTDEF_128	128	/*  0x80	*/
#   define	ISO1__NOTDEF_129	129
#   define	ISO1__NOTDEF_130	130
#   define	ISO1__NOTDEF_131	131
#   define	ISO1__NOTDEF_132	132
#   define	ISO1__NOTDEF_133	133
#   define	ISO1__NOTDEF_134	134
#   define	ISO1__NOTDEF_135	135

#   define	ISO1__NOTDEF_136	136	/*  0x88	*/
#   define	ISO1__NOTDEF_137	137
#   define	ISO1__NOTDEF_138	138
#   define	ISO1__NOTDEF_139	139
#   define	ISO1__NOTDEF_140	140
#   define	ISO1__NOTDEF_141	141
#   define	ISO1__NOTDEF_142	142
#   define	ISO1__NOTDEF_143	143

#   define	ISO1_dotlessi		144	/*  0x90	*/
#   define	ISO1_grave		145
#   define	ISO1__acute_146		146
#   define	ISO1__circumflex_147	147
#   define	ISO1__tilde_148		148
#   define	ISO1__macron_149	149
#   define	ISO1_breve		150
#   define	ISO1_dotaccent		151

#   define	ISO1__dieresis_152	152
#   define	ISO1__NOTDEF_153	153
#   define	ISO1_ring		154
#   define	ISO1__cedilla_155	155
#   define	ISO1__NOTDEF_156	156
#   define	ISO1_hungarumlaut	157
#   define	ISO1_ogonek		158
#   define	ISO1_caron		159

#   define	ISO1_nobreakspace	160	/*  0xa0	*/
#   define	ISO1_exclamdown		161
#   define	ISO1_cent		162
#   define	ISO1_sterling		163
#   define	ISO1_currency		164	/*  15 -> Euro		*/
#   define	ISO1_yen		165
#   define	ISO1_brokenbar		166	/*  15 -> Scaron	*/
#   define	ISO1_section		167

#   define	ISO1_dieresis		168	/*  15 -> scaron	*/
#   define	ISO1_copyright		169
#   define	ISO1_ordfeminine	170
#   define	ISO1_guillemotleft	171
#   define	ISO1_logicalnot		172
#   define	ISO1_hyphen		173
#   define	ISO1_registered		174
#   define	ISO1_macron		175

#   define	ISO1_degree		176	/*  0xb0	*/
#   define	ISO1_plusminus		177
#   define	ISO1_twosuperior	178
#   define	ISO1_threesuperior	179
#   define	ISO1_acute		180	/*  15 -> Zcaron	*/
#   define	ISO1_mu			181
#   define	ISO1_paragraph		182
#   define	ISO1_periodcentered	183

#   define	ISO1_cedilla		184	/*  15 -> zcaron	*/
#   define	ISO1_onesuperior	185
#   define	ISO1_ordmasculine	186
#   define	ISO1_guillemotright	187
#   define	ISO1_onequarter		188	/*  15 -> OE		*/
#   define	ISO1_onehalf		189	/*  15 -> oe		*/
#   define	ISO1_threequarters	190	/*  15 -> Ydieresis	*/
#   define	ISO1_questiondown	191

#   define	ISO1_Agrave		192	/*  0xc0	*/
#   define	ISO1_Aacute		193
#   define	ISO1_Acircumflex	194
#   define	ISO1_Atilde		195
#   define	ISO1_Adieresis		196
#   define	ISO1_Aring		197
#   define	ISO1_AE			198
#   define	ISO1_Ccedilla		199

#   define	ISO1_Egrave		200
#   define	ISO1_Eacute		201
#   define	ISO1_Ecircumflex	202
#   define	ISO1_Edieresis		203
#   define	ISO1_Igrave		204
#   define	ISO1_Iacute		205
#   define	ISO1_Icircumflex	206
#   define	ISO1_Idieresis		207

#   define	ISO1_Eth		208	/*  0xd0	*/
#   define	ISO1_Ntilde		209
#   define	ISO1_Ograve		210
#   define	ISO1_Oacute		211
#   define	ISO1_Ocircumflex	212
#   define	ISO1_Otilde		213
#   define	ISO1_Odieresis		214
#   define	ISO1_multiply		215

#   define	ISO1_Oslash		216
#   define	ISO1_Ugrave		217
#   define	ISO1_Uacute		218
#   define	ISO1_Ucircumflex	219
#   define	ISO1_Udieresis		220
#   define	ISO1_Yacute		221
#   define	ISO1_Thorn		222
#   define	ISO1_germandbls		223

#   define	ISO1_agrave		224	/*  0xe0	*/
#   define	ISO1_aacute		225
#   define	ISO1_acircumflex	226
#   define	ISO1_atilde		227
#   define	ISO1_adieresis		228
#   define	ISO1_aring		229
#   define	ISO1_ae			230
#   define	ISO1_ccedilla		231

#   define	ISO1_egrave		232
#   define	ISO1_eacute		233
#   define	ISO1_ecircumflex	234
#   define	ISO1_edieresis		235
#   define	ISO1_igrave		236
#   define	ISO1_iacute		237
#   define	ISO1_icircumflex	238
#   define	ISO1_idieresis		239

#   define	ISO1_eth		240	/*  0xf0	*/
#   define	ISO1_ntilde		241
#   define	ISO1_ograve		242
#   define	ISO1_oacute		243
#   define	ISO1_ocircumflex	244
#   define	ISO1_otilde		245
#   define	ISO1_odieresis		246
#   define	ISO1_divide		247

#   define	ISO1_oslash		248
#   define	ISO1_ugrave		249
#   define	ISO1_uacute		250
#   define	ISO1_ucircumflex	251
#   define	ISO1_udieresis		252
#   define	ISO1_yacute		253
#   define	ISO1_thorn		254
#   define	ISO1_ydieresis		255	/*  0xff	*/

#   define	ISO1_isupper(c)	((ASCII_isupper((c)))||\
				  ((c)>=ISO1_Agrave&&(c)<=ISO1_Odieresis)||\
				  ((c)>=ISO1_Oslash&&(c)<=ISO1_germandbls))
#   define	ISO1_islower(c)	((ASCII_islower((c)))||\
				  ((c)>=ISO1_germandbls&&(c)<=ISO1_odieresis)||\
				  ((c)>=ISO1_oslash&&(c)<=ISO1_ydieresis))
#   define	ISO1_isalpha(c)	(ISO1_isupper((c))||ISO1_islower((c)))
#   define	ISO1_isdigit(c)	(ASCII_isdigit((c)))
#   define	ISO1_isalnum(c)	(ISO1_isupper((c))||ISO1_islower((c))||\
				  ISO1_isdigit((c)))
#   define	ISO1_tolower(c)	(((ISO1_isupper(c)&&(c)!=ISO1_germandbls))?\
				    ((c)+0x20):(c))
#   define	ISO1_toupper(c)	(((ISO1_islower(c)&&(c)!=ISO1_germandbls))?\
				    ((c)-0x20):(c))

/*15*/
#   define	ISO15_Euro		164	/*  1 -> currency	*/
#   define	ISO15_Scaron		166	/*  1 -> brokenbar	*/
#   define	ISO15_scaron		168	/*  1 -> dieresis	*/
#   define	ISO15_Zcaron		180	/*  1 -> acute		*/
#   define	ISO15_zcaron		184	/*  1 -> cedilla	*/
#   define	ISO15_OE		188	/*  1 -> onequarter	*/
#   define	ISO15_oe		189	/*  1 -> onehalf	*/
#   define	ISO15_Ydieresis		190	/*  1 -> threequarters	*/

#   define	ISO15_isupper(c) ((ASCII_isupper((c)))||\
				  ((c)>=ISO1_Agrave&&(c)<=ISO1_Odieresis)||\
				  ((c)>=ISO1_Oslash&&(c)<=ISO1_germandbls)||\
				  ((c)==ISO15_Scaron)||\
				  ((c)==ISO15_Zcaron)||\
				  ((c)==ISO15_OE)||\
				  ((c)==ISO15_Ydieresis))
#   define	ISO15_islower(c) ((ASCII_islower((c)))||\
				  ((c)>=ISO1_germandbls&&(c)<=ISO1_odieresis)||\
				  ((c)>=ISO1_oslash&&(c)<=ISO1_ydieresis)||\
				  ((c)==ISO15_scaron)||\
				  ((c)==ISO15_zcaron)||\
				  ((c)==ISO15_oe))

#   define	ISO15_isalpha(c) (ISO15_isupper((c))||ISO15_islower((c)))
#   define	ISO15_isdigit(c) (ASCII_isdigit((c)))
#   define	ISO15_isalnum(c) (ISO15_isupper((c))||ISO15_islower((c))||\
				  ISO15_isdigit((c)))

#   define	ISO15_tolower(c) ((!ISO15_isupper(c)?(c):\
				    ((c)==ISO1_germandbls?(ISO1_germandbls):\
				    ((c)==ISO15_Scaron?(ISO15_scaron):\
				    ((c)==ISO15_Zcaron?(ISO15_zcaron):\
				    ((c)==ISO15_OE?(ISO15_oe):\
				    ((c)==ISO15_Ydieresis?(ISO1_ydieresis):\
				    ((c)+0x20))))))))

#   define	ISO15_toupper(c) ((!ISO15_islower(c)?(c):\
				    ((c)==ISO1_germandbls?(ISO1_germandbls):\
				    ((c)==ISO15_scaron?(ISO15_Scaron):\
				    ((c)==ISO15_zcaron?(ISO15_Zcaron):\
				    ((c)==ISO15_oe?(ISO15_OE):\
				    ((c)==ISO1_ydieresis?(ISO15_Ydieresis):\
				    ((c)-0x20))))))))

/************************************************************************/
/*									*/
/*  ISO Latin2 (ISO 8859-2)						*/
/*									*/
/************************************************************************/

#   define ISO2__nul		ASCII__nul	/*  0		*/
#   define ISO2__soh		ASCII__soh	/*  1		*/
#   define ISO2__stx		ASCII__stx	/*  2		*/
#   define ISO2__etx		ASCII__etx	/*  3		*/
#   define ISO2__eot		ASCII__eot	/*  4		*/
#   define ISO2__enq		ASCII__enq	/*  5		*/
#   define ISO2__ack		ASCII__ack	/*  6		*/
#   define ISO2__bel		ASCII__bel	/*  7		*/
#   define ISO2__bs		ASCII__bs	/*  8		*/
#   define ISO2__ht		ASCII__ht	/*  9		*/
#   define ISO2__nl		ASCII__nl	/*  10		*/
#   define ISO2__vt		ASCII__vt	/*  11		*/
#   define ISO2__np		ASCII__np	/*  12		*/
#   define ISO2__cr		ASCII__cr	/*  13		*/
#   define ISO2__so		ASCII__so	/*  14		*/
#   define ISO2__si		ASCII__si	/*  15		*/
#   define ISO2__dle		ASCII__dle	/*  16		*/
#   define ISO2__dc1		ASCII__dc1	/*  17		*/
#   define ISO2__dc2		ASCII__dc2	/*  18		*/
#   define ISO2__dc3		ASCII__dc3	/*  19		*/
#   define ISO2__dc4		ASCII__dc4	/*  20		*/
#   define ISO2__nak		ASCII__nak	/*  21		*/
#   define ISO2__syn		ASCII__syn	/*  22		*/
#   define ISO2__etb		ASCII__etb	/*  23		*/
#   define ISO2__can		ASCII__can	/*  24		*/
#   define ISO2__em		ASCII__em	/*  25		*/
#   define ISO2__sub		ASCII__sub	/*  26		*/
#   define ISO2__esc		ASCII__esc	/*  27		*/
#   define ISO2__fs		ASCII__fs	/*  28		*/
#   define ISO2__gs		ASCII__gs	/*  29		*/
#   define ISO2__rs		ASCII__rs	/*  30		*/
#   define ISO2__us		ASCII__us	/*  31		*/

#   define ISO2_space		ASCII_space	/*  32		*/
#   define ISO2_exclam		ASCII_exclam	/*  33		*/
#   define ISO2_quotedbl	ASCII_quotedbl	/*  34		*/
#   define ISO2_numbersign	ASCII_numbersign /*  35		*/
#   define ISO2_dollar		ASCII_dollar	/*  36		*/
#   define ISO2_percent		ASCII_percent	/*  37		*/
#   define ISO2_ampersand	ASCII_ampersand	/*  38		*/
#   define ISO2_quotesingle	ASCII_quotesingle /*  39		*/
#   define ISO2_parenleft	ASCII_parenleft	/*  40		*/
#   define ISO2_parenright	ASCII_parenright /*  41		*/
#   define ISO2_asterisk	ASCII_asterisk	/*  42		*/
#   define ISO2_plus		ASCII_plus	/*  43		*/
#   define ISO2_comma		ASCII_comma	/*  44		*/
#   define ISO2_minus		ASCII_minus	/*  45		*/
#   define ISO2_period		ASCII_period	/*  46		*/
#   define ISO2_slash		ASCII_slash	/*  47		*/
#   define ISO2_zero		ASCII_zero	/*  48		*/
#   define ISO2_one		ASCII_one	/*  49		*/
#   define ISO2_two		ASCII_two	/*  50		*/
#   define ISO2_three		ASCII_three	/*  51		*/
#   define ISO2_four		ASCII_four	/*  52		*/
#   define ISO2_five		ASCII_five	/*  53		*/
#   define ISO2_six		ASCII_six	/*  54		*/
#   define ISO2_seven		ASCII_seven	/*  55		*/
#   define ISO2_eight		ASCII_eight	/*  56		*/
#   define ISO2_nine		ASCII_nine	/*  57		*/
#   define ISO2_colon		ASCII_colon	/*  58		*/
#   define ISO2_semicolon	ASCII_semicolon	/*  59		*/
#   define ISO2_less		ASCII_less	/*  60		*/
#   define ISO2_equal		ASCII_equal	/*  61		*/
#   define ISO2_greater		ASCII_greater	/*  62		*/
#   define ISO2_question	ASCII_question	/*  63		*/

#   define ISO2_at		ASCII_at	/*  64		*/
#   define ISO2_A		ASCII_A		/*  65		*/
#   define ISO2_B		ASCII_B		/*  66		*/
#   define ISO2_C		ASCII_C		/*  67		*/
#   define ISO2_D		ASCII_D		/*  68		*/
#   define ISO2_E		ASCII_E		/*  69		*/
#   define ISO2_F		ASCII_F		/*  70		*/
#   define ISO2_G		ASCII_G		/*  71		*/
#   define ISO2_H		ASCII_H		/*  72		*/
#   define ISO2_I		ASCII_I		/*  73		*/
#   define ISO2_J		ASCII_J		/*  74		*/
#   define ISO2_K		ASCII_K		/*  75		*/
#   define ISO2_L		ASCII_L		/*  76		*/
#   define ISO2_M		ASCII_M		/*  77		*/
#   define ISO2_N		ASCII_N		/*  78		*/
#   define ISO2_O		ASCII_O		/*  79		*/
#   define ISO2_P		ASCII_P		/*  80		*/
#   define ISO2_Q		ASCII_Q		/*  81		*/
#   define ISO2_R		ASCII_R		/*  82		*/
#   define ISO2_S		ASCII_S		/*  83		*/
#   define ISO2_T		ASCII_T		/*  84		*/
#   define ISO2_U		ASCII_U		/*  85		*/
#   define ISO2_V		ASCII_V		/*  86		*/
#   define ISO2_W		ASCII_W		/*  87		*/
#   define ISO2_X		ASCII_X		/*  88		*/
#   define ISO2_Y		ASCII_Y		/*  89		*/
#   define ISO2_Z		ASCII_Z		/*  90		*/
#   define ISO2_bracketleft	ASCII_bracketleft /* 91		*/
#   define ISO2_backslash	ASCII_backslash	/*  92		*/
#   define ISO2_bracketright	ASCII_bracketright /* 93	*/
#   define ISO2_circumflex	ASCII_circumflex /*  94		*/
#   define ISO2_underscore	ASCII_underscore /*  95		*/

#   define ISO2_quoteleft	ASCII_quoteleft	/*  96		*/
#   define ISO2_a		ASCII_a		/*  97		*/
#   define ISO2_b		ASCII_b		/*  98		*/
#   define ISO2_c		ASCII_c		/*  99		*/
#   define ISO2_d		ASCII_d		/*  100		*/
#   define ISO2_e		ASCII_e		/*  101		*/
#   define ISO2_f		ASCII_f		/*  102		*/
#   define ISO2_g		ASCII_g		/*  103		*/
#   define ISO2_h		ASCII_h		/*  104		*/
#   define ISO2_i		ASCII_i		/*  105		*/
#   define ISO2_j		ASCII_j		/*  106		*/
#   define ISO2_k		ASCII_k		/*  107		*/
#   define ISO2_l		ASCII_l		/*  108		*/
#   define ISO2_m		ASCII_m		/*  109		*/
#   define ISO2_n		ASCII_n		/*  110		*/
#   define ISO2_o		ASCII_o		/*  111		*/
#   define ISO2_p		ASCII_p		/*  112		*/
#   define ISO2_q		ASCII_q		/*  113		*/
#   define ISO2_r		ASCII_r		/*  114		*/
#   define ISO2_s		ASCII_s		/*  115		*/
#   define ISO2_t		ASCII_t		/*  116		*/
#   define ISO2_u		ASCII_u		/*  117		*/
#   define ISO2_v		ASCII_v		/*  118		*/
#   define ISO2_w		ASCII_w		/*  119		*/
#   define ISO2_x		ASCII_x		/*  120		*/
#   define ISO2_y		ASCII_y		/*  121		*/
#   define ISO2_z		ASCII_z		/*  122		*/
#   define ISO2_braceleft	ASCII_braceleft	/*  123		*/
#   define ISO2_bar		ASCII_bar	/*  124		*/
#   define ISO2_braceright	ASCII_braceright /* 125		*/
#   define ISO2_tilde		ASCII_tilde	/*  126		*/
#   define ISO2__del		ASCII__del	/*  127		*/

#   define ISO2__NOTDEF_128	0x80		/*  128		*/
#   define ISO2__NOTDEF_129	0x81		/*  129		*/
#   define ISO2__NOTDEF_130	0x82		/*  130		*/
#   define ISO2__NOTDEF_131	0x83		/*  131		*/
#   define ISO2__NOTDEF_132	0x84		/*  132		*/
#   define ISO2__NOTDEF_133	0x85		/*  133		*/
#   define ISO2__NOTDEF_134	0x86		/*  134		*/
#   define ISO2__NOTDEF_135	0x85		/*  135		*/

#   define ISO2__NOTDEF_136	0x88		/*  136		*/
#   define ISO2__NOTDEF_137	0x89		/*  137		*/
#   define ISO2__NOTDEF_138	0x8a		/*  138		*/
#   define ISO2__NOTDEF_139	0x8b		/*  139		*/
#   define ISO2__NOTDEF_140	0x8c		/*  140		*/
#   define ISO2__NOTDEF_141	0x8d		/*  141		*/
#   define ISO2__NOTDEF_142	0x8e		/*  142		*/
#   define ISO2__NOTDEF_143	0x8f		/*  143		*/

#   define ISO2_dotlessi	0x90		/*  144		*/
#   define ISO2_grave		0x91		/*  145		*/
#   define ISO2__acute_146	0x92		/*  146		*/
#   define ISO2__circumflex_147	0x93		/*  147		*/
#   define ISO2__tilde_148	0x94		/*  148		*/
#   define ISO2__macron_149	0x95		/*  149		*/
#   define ISO2__breve_150	0x96		/*  150		*/
#   define ISO2__dotaccent_151	0x97		/*  151		*/

#   define ISO2__dieresis_152	0x98		/*  152		*/
#   define ISO2__NOTDEF_153	0x99		/*  153		*/
#   define ISO2_ring		0x9a		/*  154		*/
#   define ISO2__cedilla_155	0x9b		/*  155		*/
#   define ISO2__NOTDEF_156	0x9c		/*  156		*/
#   define ISO2__hungarumlaut_157 0x9d		/*  157		*/
#   define ISO2__ogonek_158	0x9e		/*  158		*/
#   define ISO2__caron_159	0x9f		/*  159		*/

#   define ISO2_nobreakspace	0xa0		/*  160		*/
#   define ISO2_Aogonek		0xa1		/*  161		*/
#   define ISO2_breve		0xa2		/*  162		*/
#   define ISO2_Lslash		0xa3		/*  163		*/
#   define ISO2_currency	0xa4		/*  164		*/
#   define ISO2_Lcaron		0xa5		/*  165		*/
#   define ISO2_Sacute		0xa6		/*  166		*/
#   define ISO2_section		0xa7		/*  167		*/

#   define ISO2_dieresis	0xa8		/*  168		*/
#   define ISO2_Scaron		0xa9		/*  169		*/
#   define ISO2_Scedilla	0xaa		/*  170		*/
#   define ISO2_Tcaron		0xab		/*  171		*/
#   define ISO2_Zacute		0xac		/*  172		*/
#   define ISO2_hyphen		0xad		/*  173		*/
#   define ISO2_Zcaron		0xae		/*  174		*/
#   define ISO2_Zdotaccent	0xaf		/*  175		*/

#   define ISO2_degree		0xb0		/*  176		*/
#   define ISO2_aogonek		0xb1		/*  177		*/
#   define ISO2_ogonek		0xb2		/*  178		*/
#   define ISO2_lslash		0xb3		/*  179		*/
#   define ISO2_acute		0xb4		/*  180		*/
#   define ISO2_lcaron		0xb5		/*  181		*/
#   define ISO2_sacute		0xb6		/*  182		*/
#   define ISO2_caron		0xb7		/*  183		*/

#   define ISO2_cedilla		0xb8		/*  184		*/
#   define ISO2_scaron		0xb9		/*  185		*/
#   define ISO2_scedilla	0xba		/*  186		*/
#   define ISO2_tcaron		0xbb		/*  187		*/
#   define ISO2_zacute		0xbc		/*  188		*/
#   define ISO2_hungarumlaut	0xbd		/*  189		*/
#   define ISO2_zcaron		0xbe		/*  190		*/
#   define ISO2_zdotaccent	0xbf		/*  191		*/

#   define ISO2_Racute		0xc0		/*  192		*/
#   define ISO2_Aacute		0xc1		/*  193		*/
#   define ISO2_Acircumflex	0xc2		/*  194		*/
#   define ISO2_Abreve		0xc3		/*  195		*/
#   define ISO2_Adieresis	0xc4		/*  196		*/
#   define ISO2_Lacute		0xc5		/*  197		*/
#   define ISO2_Cacute		0xc6		/*  198		*/
#   define ISO2_Ccedilla	0xc7		/*  199		*/

#   define ISO2_Ccaron		0xc8		/*  200		*/
#   define ISO2_Eacute		0xc9		/*  201		*/
#   define ISO2_Eogonek		0xca		/*  202		*/
#   define ISO2_Edieresis	0xcb		/*  203		*/
#   define ISO2_Ecaron		0xcc		/*  204		*/
#   define ISO2_Iacute		0xcd		/*  205		*/
#   define ISO2_Icircumflex	0xce		/*  206		*/
#   define ISO2_Dcaron		0xcf		/*  207		*/

#   define ISO2_Dcroat		0xd0		/*  208		*/
#   define ISO2_Nacute		0xd1		/*  209		*/
#   define ISO2_Ncaron		0xd2		/*  210		*/
#   define ISO2_Oacute		0xd3		/*  211		*/
#   define ISO2_Ocircumflex	0xd4		/*  212		*/
#   define ISO2_Ohungarumlaut	0xd5		/*  213		*/
#   define ISO2_Odieresis	0xd6		/*  214		*/
#   define ISO2_multiply	0xd7		/*  215		*/

#   define ISO2_Rcaron		0xd8		/*  216		*/
#   define ISO2_Uring		0xd9		/*  217		*/
#   define ISO2_Uacute		0xda		/*  218		*/
#   define ISO2_Uhungarumlaut	0xdb		/*  219		*/
#   define ISO2_Udieresis	0xdc		/*  220		*/
#   define ISO2_Yacute		0xdd		/*  221		*/
#   define ISO2_Tcedilla	0xde		/*  222		*/
#   define ISO2_germandbls	0xdf		/*  223		*/

#   define ISO2_racute		0xe0		/*  224		*/
#   define ISO2_aacute		0xe1		/*  225		*/
#   define ISO2_acircumflex	0xe2		/*  226		*/
#   define ISO2_abreve		0xe3		/*  227		*/
#   define ISO2_adieresis	0xe4		/*  228		*/
#   define ISO2_lacute		0xe5		/*  229		*/
#   define ISO2_cacute		0xe6		/*  230		*/
#   define ISO2_ccedilla	0xe7		/*  231		*/

#   define ISO2_ccaron		0xe8		/*  232		*/
#   define ISO2_eacute		0xe9		/*  233		*/
#   define ISO2_eogonek		0xea		/*  234		*/
#   define ISO2_edieresis	0xeb		/*  235		*/
#   define ISO2_ecaron		0xec		/*  236		*/
#   define ISO2_iacute		0xed		/*  237		*/
#   define ISO2_icircumflex	0xee		/*  238		*/
#   define ISO2_dcaron		0xef		/*  239		*/

#   define ISO2_dcroat		0xf0		/*  240		*/
#   define ISO2_nacute		0xf1		/*  241		*/
#   define ISO2_ncaron		0xf2		/*  242		*/
#   define ISO2_oacute		0xf3		/*  243		*/
#   define ISO2_ocircumflex	0xf4		/*  244		*/
#   define ISO2_ohungarumlaut	0xf5		/*  245		*/
#   define ISO2_odieresis	0xf6		/*  246		*/
#   define ISO2_divide		0xf7		/*  247		*/

#   define ISO2_rcaron		0xf8		/*  248		*/
#   define ISO2_uring		0xf9		/*  249		*/
#   define ISO2_uacute		0xfa		/*  250		*/
#   define ISO2_uhungarumlaut	0xfb		/*  251		*/
#   define ISO2_udieresis	0xfc		/*  252		*/
#   define ISO2_yacute		0xfd		/*  253		*/
#   define ISO2_tcedilla	0xfe		/*  254		*/
#   define ISO2_dotaccent	0xff		/*  255		*/

#   define	ISO2_isupper(c)	((ASCII_isupper((c)))||\
				  ((c)==ISO2_Aogonek)||\
				  ((c)==ISO2_Lslash)||\
				  ((c)==ISO2_Lcaron)||\
				  ((c)==ISO2_Sacute)||\
				  ((c)==ISO2_Scaron)||\
				  ((c)==ISO2_Scedilla)||\
				  ((c)==ISO2_Tcaron)||\
				  ((c)==ISO2_Zacute)||\
				  ((c)==ISO2_Zcaron)||\
				  ((c)==ISO2_Zdotaccent)||\
				  ((c)>=ISO2_Racute&&(c)<=ISO2_Odieresis)||\
				  ((c)>=ISO2_Rcaron&&(c)<=ISO2_germandbls))

#   define	ISO2_islower(c)	((ASCII_islower((c)))||\
				  ((c)==ISO2_aogonek)||\
				  ((c)==ISO2_lslash)||\
				  ((c)==ISO2_lcaron)||\
				  ((c)==ISO2_sacute)||\
				  ((c)==ISO2_scaron)||\
				  ((c)==ISO2_scedilla)||\
				  ((c)==ISO2_tcaron)||\
				  ((c)==ISO2_zacute)||\
				  ((c)==ISO2_zcaron)||\
				  ((c)==ISO2_zdotaccent)||\
				  ((c)>=ISO2_germandbls&&(c)<=ISO2_odieresis)||\
				  ((c)>=ISO2_rcaron&&(c)<=ISO2_tcedilla))

#   define	ISO2_isalpha(c)	(ISO2_isupper((c))||ISO2_islower((c)))
#   define	ISO2_isdigit(c)	(ASCII_isdigit((c)))
#   define	ISO2_isalnum(c)	(ISO2_isupper((c))||ISO2_islower((c))||\
				  ISO2_isdigit((c)))
#   define	ISO2_tolower(c)	(((ISO2_isupper(c)&&(c)!=ISO2_germandbls))?\
			(((c)>=0xa0&&(c)<=0xaf)?((c)+0x10):((c)+0x20)):(c))
#   define	ISO2_toupper(c)	(((ISO2_islower(c)&&(c)!=ISO2_germandbls))?\
			(((c)>=0xb0&&(c)<=0xbf)?((c)-0x10):((c)-0x20)):(c))

/************************************************************************/
/*									*/
/*  Modern Greek (ISO 8859-7)						*/
/*									*/
/************************************************************************/

#   define	ISO7_space		0x20	/*   32		*/
#   define	ISO7_exclam		0x21	/*   33		*/
#   define	ISO7_quotedbl		0x22	/*   34		*/
#   define	ISO7_numbersign		0x23	/*   35		*/
#   define	ISO7_dollar		0x24	/*   36		*/
#   define	ISO7_percent		0x25	/*   37		*/
#   define	ISO7_ampersand		0x26	/*   38		*/
#   define	ISO7_quotesingle	0x27	/*   39		*/
#   define	ISO7_parenleft		0x28	/*   40		*/
#   define	ISO7_parenright		0x29	/*   41		*/
#   define	ISO7_asterisk		0x2a	/*   42		*/
#   define	ISO7_plus		0x2b	/*   43		*/
#   define	ISO7_comma		0x2c	/*   44		*/
#   define	ISO7_minus		0x2d	/*   45		*/
#   define	ISO7_period		0x2e	/*   46		*/
#   define	ISO7_slash		0x2f	/*   47		*/
#   define	ISO7_zero		0x30	/*   48		*/
#   define	ISO7_one		0x31	/*   49		*/
#   define	ISO7_two		0x32	/*   50		*/
#   define	ISO7_three		0x33	/*   51		*/
#   define	ISO7_four		0x34	/*   52		*/
#   define	ISO7_five		0x35	/*   53		*/
#   define	ISO7_six		0x36	/*   54		*/
#   define	ISO7_seven		0x37	/*   55		*/
#   define	ISO7_eight		0x38	/*   56		*/
#   define	ISO7_nine		0x39	/*   57		*/
#   define	ISO7_colon		0x3a	/*   58		*/
#   define	ISO7_semicolon		0x3b	/*   59		*/
#   define	ISO7_less		0x3c	/*   60		*/
#   define	ISO7_equal		0x3d	/*   61		*/
#   define	ISO7_greater		0x3e	/*   62		*/
#   define	ISO7_question		0x3f	/*   63		*/
#   define	ISO7_at			0x40	/*   64		*/
#   define	ISO7_A			0x41	/*   65		*/
#   define	ISO7_B			0x42	/*   66		*/
#   define	ISO7_C			0x43	/*   67		*/
#   define	ISO7_D			0x44	/*   68		*/
#   define	ISO7_E			0x45	/*   69		*/
#   define	ISO7_F			0x46	/*   70		*/
#   define	ISO7_G			0x47	/*   71		*/
#   define	ISO7_H			0x48	/*   72		*/
#   define	ISO7_I			0x49	/*   73		*/
#   define	ISO7_J			0x4a	/*   74		*/
#   define	ISO7_K			0x4b	/*   75		*/
#   define	ISO7_L			0x4c	/*   76		*/
#   define	ISO7_M			0x4d	/*   77		*/
#   define	ISO7_N			0x4e	/*   78		*/
#   define	ISO7_O			0x4f	/*   79		*/
#   define	ISO7_P			0x50	/*   80		*/
#   define	ISO7_Q			0x51	/*   81		*/
#   define	ISO7_R			0x52	/*   82		*/
#   define	ISO7_S			0x53	/*   83		*/
#   define	ISO7_T			0x54	/*   84		*/
#   define	ISO7_U			0x55	/*   85		*/
#   define	ISO7_V			0x56	/*   86		*/
#   define	ISO7_W			0x57	/*   87		*/
#   define	ISO7_X			0x58	/*   88		*/
#   define	ISO7_Y			0x59	/*   89		*/
#   define	ISO7_Z			0x5a	/*   90		*/
#   define	ISO7_bracketleft	0x5b	/*   91		*/
#   define	ISO7_backslash		0x5c	/*   92		*/
#   define	ISO7_bracketright	0x5d	/*   93		*/
#   define	ISO7_asciicircum	0x5e	/*   94		*/
#   define	ISO7_underscore		0x5f	/*   95		*/
#   define	ISO7_grave		0x60	/*   96		*/
#   define	ISO7_a			0x61	/*   97		*/
#   define	ISO7_b			0x62	/*   98		*/
#   define	ISO7_c			0x63	/*   99		*/
#   define	ISO7_d			0x64	/*  100		*/
#   define	ISO7_e			0x65	/*  101		*/
#   define	ISO7_f			0x66	/*  102		*/
#   define	ISO7_g			0x67	/*  103		*/
#   define	ISO7_h			0x68	/*  104		*/
#   define	ISO7_i			0x69	/*  105		*/
#   define	ISO7_j			0x6a	/*  106		*/
#   define	ISO7_k			0x6b	/*  107		*/
#   define	ISO7_l			0x6c	/*  108		*/
#   define	ISO7_m			0x6d	/*  109		*/
#   define	ISO7_n			0x6e	/*  110		*/
#   define	ISO7_o			0x6f	/*  111		*/
#   define	ISO7_p			0x70	/*  112		*/
#   define	ISO7_q			0x71	/*  113		*/
#   define	ISO7_r			0x72	/*  114		*/
#   define	ISO7_s			0x73	/*  115		*/
#   define	ISO7_t			0x74	/*  116		*/
#   define	ISO7_u			0x75	/*  117		*/
#   define	ISO7_v			0x76	/*  118		*/
#   define	ISO7_w			0x77	/*  119		*/
#   define	ISO7_x			0x78	/*  120		*/
#   define	ISO7_y			0x79	/*  121		*/
#   define	ISO7_z			0x7a	/*  122		*/
#   define	ISO7_braceleft		0x7b	/*  123		*/
#   define	ISO7_bar		0x7c	/*  124		*/
#   define	ISO7_braceright		0x7d	/*  125		*/
#   define	ISO7_asciitilde		0x7e	/*  126		*/
#   define	ISO7_nobreakspace	0xa0	/*  160		*/
#   define	ISO7_afii64937		0xa1	/*  161		*/
#   define	ISO7_afii57929		0xa2	/*  162		*/
#   define	ISO7_sterling		0xa3	/*  163		*/
#   define	ISO7_brokenbar		0xa6	/*  166		*/
#   define	ISO7_section		0xa7	/*  167		*/
#   define	ISO7_dieresis		0xa8	/*  168		*/
#   define	ISO7_copyright		0xa9	/*  169		*/
#   define	ISO7_guillemotleft	0xab	/*  171		*/
#   define	ISO7_logicalnot		0xac	/*  172		*/
#   define	ISO7_hyphen		0xad	/*  173		*/
#   define	ISO7_afii00208		0xaf	/*  175		*/
#   define	ISO7_degree		0xb0	/*  176		*/
#   define	ISO7_plusminus		0xb1	/*  177		*/
#   define	ISO7_twosuperior	0xb2	/*  178		*/
#   define	ISO7_threesuperior	0xb3	/*  179		*/
#   define	ISO7_tonos		0xb4	/*  180		*/
#   define	ISO7_dieresistonos	0xb5	/*  181		*/
#   define	ISO7_Alphatonos		0xb6	/*  182		*/
#   define	ISO7_periodcentered	0xb7	/*  183		*/
#   define	ISO7_Epsilontonos	0xb8	/*  184		*/
#   define	ISO7_Etatonos		0xb9	/*  185		*/
#   define	ISO7_Iotatonos		0xba	/*  186		*/
#   define	ISO7_guillemotright	0xbb	/*  187		*/
#   define	ISO7_Omicrontonos	0xbc	/*  188		*/
#   define	ISO7_onehalf		0xbd	/*  189		*/
#   define	ISO7_Upsilontonos	0xbe	/*  190		*/
#   define	ISO7_Omegatonos		0xbf	/*  191		*/
#   define	ISO7_iotadieresistonos	0xc0	/*  192		*/
#   define	ISO7_Alpha		0xc1	/*  193		*/
#   define	ISO7_Beta		0xc2	/*  194		*/
#   define	ISO7_Gamma		0xc3	/*  195		*/
#   define	ISO7_Delta		0xc4	/*  196		*/
#   define	ISO7_Epsilon		0xc5	/*  197		*/
#   define	ISO7_Zeta		0xc6	/*  198		*/
#   define	ISO7_Eta		0xc7	/*  199		*/
#   define	ISO7_Theta		0xc8	/*  200		*/
#   define	ISO7_Iota		0xc9	/*  201		*/
#   define	ISO7_Kappa		0xca	/*  202		*/
#   define	ISO7_Lambda		0xcb	/*  203		*/
#   define	ISO7_Mu			0xcc	/*  204		*/
#   define	ISO7_Nu			0xcd	/*  205		*/
#   define	ISO7_Xi			0xce	/*  206		*/
#   define	ISO7_Omicron		0xcf	/*  207		*/
#   define	ISO7_Pi			0xd0	/*  208		*/
#   define	ISO7_Rho		0xd1	/*  209		*/
#   define	ISO7_Sigma		0xd3	/*  211		*/
#   define	ISO7_Tau		0xd4	/*  212		*/
#   define	ISO7_Upsilon		0xd5	/*  213		*/
#   define	ISO7_Phi		0xd6	/*  214		*/
#   define	ISO7_Chi		0xd7	/*  215		*/
#   define	ISO7_Psi		0xd8	/*  216		*/
#   define	ISO7_Omega		0xd9	/*  217		*/
#   define	ISO7_Iotadieresis	0xda	/*  218		*/
#   define	ISO7_Upsilondieresis	0xdb	/*  219		*/
#   define	ISO7_alphatonos		0xdc	/*  220		*/
#   define	ISO7_epsilontonos	0xdd	/*  221		*/
#   define	ISO7_etatonos		0xde	/*  222		*/
#   define	ISO7_iotatonos		0xdf	/*  223		*/
#   define	ISO7_upsilondieresistonos 0xe0	/*  224		*/
#   define	ISO7_alpha		0xe1	/*  225		*/
#   define	ISO7_beta		0xe2	/*  226		*/
#   define	ISO7_gamma		0xe3	/*  227		*/
#   define	ISO7_delta		0xe4	/*  228		*/
#   define	ISO7_epsilon		0xe5	/*  229		*/
#   define	ISO7_zeta		0xe6	/*  230		*/
#   define	ISO7_eta		0xe7	/*  231		*/
#   define	ISO7_theta		0xe8	/*  232		*/
#   define	ISO7_iota		0xe9	/*  233		*/
#   define	ISO7_kappa		0xea	/*  234		*/
#   define	ISO7_lambda		0xeb	/*  235		*/
#   define	ISO7_mu			0xec	/*  236		*/
#   define	ISO7_nu			0xed	/*  237		*/
#   define	ISO7_xi			0xee	/*  238		*/
#   define	ISO7_omicron		0xef	/*  239		*/
#   define	ISO7_pi			0xf0	/*  240		*/
#   define	ISO7_rho		0xf1	/*  241		*/
#   define	ISO7_sigma1		0xf2	/*  242		*/
#   define	ISO7_sigma		0xf3	/*  243		*/
#   define	ISO7_tau		0xf4	/*  244		*/
#   define	ISO7_upsilon		0xf5	/*  245		*/
#   define	ISO7_phi		0xf6	/*  246		*/
#   define	ISO7_chi		0xf7	/*  247		*/
#   define	ISO7_psi		0xf8	/*  248		*/
#   define	ISO7_omega		0xf9	/*  249		*/
#   define	ISO7_iotadieresis	0xfa	/*  250		*/
#   define	ISO7_upsilondieresis	0xfb	/*  251		*/
#   define	ISO7_omicrontonos	0xfc	/*  252		*/
#   define	ISO7_upsilontonos	0xfd	/*  253		*/
#   define	ISO7_omegatonos		0xfe	/*  254		*/

#   define	ISO7_isupper(c)	((ASCII_isupper((c)))||\
				  ((c)==ISO7_Alphatonos)||\
				  ((c)==ISO7_Epsilontonos)||\
				  ((c)==ISO7_Etatonos)||\
				  ((c)==ISO7_Iotatonos)||\
				  ((c)==ISO7_Omicrontonos)||\
				  ((c)==ISO7_Upsilontonos)||\
				  ((c)==ISO7_Omegatonos)||\
				  ((c)>=ISO7_Alpha&&(c)<=ISO7_Rho)||\
				  ((c)>=ISO7_Sigma&&(c)<=ISO7_Upsilondieresis))

#   define	ISO7_islower(c)	((ASCII_islower((c)))||\
				  ((c)>=ISO7_alphatonos&&(c)<=ISO7_omegatonos))

#   define	ISO7_isalpha(c)	(ISO7_isupper((c))||ISO7_islower((c)))

#   define	ISO7_isdigit(c)	(ASCII_isdigit((c)))

#   define	ISO7_isalnum(c)	(ISO7_isupper((c))||ISO7_islower((c))||\
				  ISO7_isdigit((c)))

#   define	ISO7_tolower(c)	(\
		ASCII_isupper(c)?ASCII_tolower(c): (\
		((c)>=ISO7_Alpha&&(c)<=ISO7_Rho)?((c)+0x20): (\
		((c)>=ISO7_Sigma&&(c)<=ISO7_Upsilondieresis)?((c)+0x20): (\
		((c)==ISO7_Alphatonos)?ISO7_alphatonos: (\
		((c)==ISO7_Epsilontonos)?ISO7_epsilontonos: (\
		((c)==ISO7_Etatonos)?ISO7_etatonos: (\
		((c)==ISO7_Iotatonos)?ISO7_iotatonos: (\
		((c)==ISO7_Omicrontonos)?ISO7_omicrontonos: (\
		((c)==ISO7_Upsilontonos)?ISO7_upsilontonos: (\
		((c)==ISO7_Omegatonos)?ISO7_omegatonos: (\
		(c) )))))))))))


#   define	ISO7_toupper(c)	(\
		ASCII_islower(c)?ASCII_toupper(c): (\
		((c)>=ISO7_alpha&&(c)<=ISO7_rho)?((c)-0x20): (\
		((c)>=ISO7_sigma&&(c)<=ISO7_upsilondieresis)?((c)-0x20): (\
		((c)==ISO7_alphatonos)?ISO7_Alphatonos: (\
		((c)==ISO7_epsilontonos)?ISO7_Epsilontonos: (\
		((c)==ISO7_etatonos)?ISO7_Etatonos: (\
		((c)==ISO7_iotatonos)?ISO7_Iotatonos: (\
		((c)==ISO7_omicrontonos)?ISO7_Omicrontonos: (\
		((c)==ISO7_upsilontonos)?ISO7_Upsilontonos: (\
		((c)==ISO7_omegatonos)?ISO7_Omegatonos: (\
		((c)==ISO7_sigma1)?ISO7_Sigma: (\
		(c) ))))))))))))

/************************************************************************/
/*									*/
/*  Adobe Postscript Standard encoding.					*/
/*									*/
/************************************************************************/

#   define	PS_space		ASCII_space
#   define	PS_exclam		ASCII_exclam
#   define	PS_quotedbl		ASCII_quotedbl
#   define	PS_numbersign		ASCII_numbersign
#   define	PS_dollar		ASCII_dollar
#   define	PS_percent		ASCII_percent
#   define	PS_ampersand		ASCII_ampersand
#   define	PS_quoteright		ASCII_quotesingle

#   define	PS_parenleft		ASCII_parenleft
#   define	PS_parenright		ASCII_parenright
#   define	PS_asterisk		ASCII_asterisk
#   define	PS_plus			ASCII_plus
#   define	PS_comma		ASCII_comma
#   define	PS_minus		ASCII_minus
#   define	PS_period		ASCII_period
#   define	PS_slash		ASCII_slash

#   define	PS_zero			ASCII_zero
#   define	PS_one			ASCII_one
#   define	PS_two			ASCII_two
#   define	PS_three		ASCII_three
#   define	PS_four			ASCII_four
#   define	PS_five			ASCII_five
#   define	PS_six			ASCII_six
#   define	PS_seven		ASCII_seven

#   define	PS_eight		ASCII_eight
#   define	PS_nine			ASCII_nine
#   define	PS_colon		ASCII_colon
#   define	PS_semicolon		ASCII_semicolon
#   define	PS_less			ASCII_less
#   define	PS_equal		ASCII_equal
#   define	PS_greater		ASCII_greater
#   define	PS_question		ASCII_question

#   define	PS_at			ASCII_at
#   define	PS_A			ASCII_A
#   define	PS_B			ASCII_B
#   define	PS_C			ASCII_C
#   define	PS_D			ASCII_D
#   define	PS_E			ASCII_E
#   define	PS_F			ASCII_F
#   define	PS_G			ASCII_G

#   define	PS_H			ASCII_H
#   define	PS_I			ASCII_I
#   define	PS_J			ASCII_J
#   define	PS_K			ASCII_K
#   define	PS_L			ASCII_L
#   define	PS_M			ASCII_M
#   define	PS_N			ASCII_N
#   define	PS_O			ASCII_O

#   define	PS_P			ASCII_P
#   define	PS_Q			ASCII_Q
#   define	PS_R			ASCII_R
#   define	PS_S			ASCII_S
#   define	PS_T			ASCII_T
#   define	PS_U			ASCII_U
#   define	PS_V			ASCII_V
#   define	PS_W			ASCII_W

#   define	PS_X			ASCII_X
#   define	PS_Y			ASCII_Y
#   define	PS_Z			ASCII_Z
#   define	PS_bracketleft		ASCII_bracketleft
#   define	PS_backslash		ASCII_backslash
#   define	PS_bracketright		ASCII_bracketright
#   define	PS_circumflex		ASCII_circumflex
#   define	PS_underscore		ASCII_underscore

#   define	PS_quoteleft		ASCII_quoteleft
#   define	PS_a			ASCII_a
#   define	PS_b			ASCII_b
#   define	PS_c			ASCII_c
#   define	PS_d			ASCII_d
#   define	PS_e			ASCII_e
#   define	PS_f			ASCII_f
#   define	PS_g			ASCII_g

#   define	PS_h			ASCII_h
#   define	PS_i			ASCII_i
#   define	PS_j			ASCII_j
#   define	PS_k			ASCII_k
#   define	PS_l			ASCII_l
#   define	PS_m			ASCII_m
#   define	PS_n			ASCII_n
#   define	PS_o			ASCII_o

#   define	PS_p			ASCII_p
#   define	PS_q			ASCII_q
#   define	PS_r			ASCII_r
#   define	PS_s			ASCII_s
#   define	PS_t			ASCII_t
#   define	PS_u			ASCII_u
#   define	PS_v			ASCII_v
#   define	PS_w			ASCII_w

#   define	PS_x			ASCII_x
#   define	PS_y			ASCII_y
#   define	PS_z			ASCII_z
#   define	PS_braceleft		ASCII_braceleft
#   define	PS_bar			ASCII_bar
#   define	PS_braceright		ASCII_braceright
#   define	PS_asciitilde		ASCII_tilde
/*0x7f*/

/*0200*/
/*0201*/
/*0202*/
/*0203*/
/*0204*/
/*0205*/
/*0206*/
/*0207*/

/*0210*/
/*0211*/
/*0212*/
/*0213*/
/*0214*/
/*0215*/
/*0216*/
/*0217*/

/*0220*/
/*0221*/
/*0222*/
/*0223*/
/*0224*/
/*0225*/
/*0226*/
/*0227*/

/*0230*/
/*0231*/
/*0232*/
/*0233*/
/*0234*/
/*0235*/
/*0236*/
/*0237*/

/*0240*/
#   define	PS_exclamdown		0xa1
#   define	PS_cent			0xa2
#   define	PS_sterling		0xa3
#   define	PS_fraction		0xa4
#   define	PS_yen			0xa5
#   define	PS_florin		0xa6
#   define	PS_section		0xa7

#   define	PS_currency		0xa8
#   define	PS_quotesingle		0xa9
#   define	PS_quotedblleft		0xaa
#   define	PS_guillemotleft	0xab
#   define	PS_guillsinglleft	0xac
#   define	PS_guillsinglright	0xad
#   define	PS_fi			0xae
#   define	PS_fl			0xaf

/*0260*/
#   define	PS_endash		0xb1
#   define	PS_dagger		0xb2
#   define	PS_daggerdbl		0xb3
#   define	PS_periodcentered	0xb4
/*0265*/
#   define	PS_paragraph		0xb6
#   define	PS_bullet		0xb7

#   define	PS_quotesinglbase	0xb8
#   define	PS_quotedblbase		0xb9
#   define	PS_quotedblright	0xba
#   define	PS_guillemotright	0xbb
#   define	PS_ellipsis		0xbc
#   define	PS_perthousand		0xbd
/*0276*/
#   define	PS_questiondown		0xbf

/*0300*/
#   define	PS_grave		0xc1
#   define	PS_acute		0xc2
#   define	PS_circumflex_0xc3	0xc3
#   define	PS_tilde		0xc4
#   define	PS_macron		0xc5
#   define	PS_breve		0xc6
#   define	PS_dotaccent		0xc7

#   define	PS_dieresis		0xc8
/*0311*/
#   define	PS_ring			0xca
#   define	PS_cedilla		0xcb
/*0314*/
#   define	PS_hungarumlaut		0xcd
#   define	PS_ogonek		0xce
#   define	PS_caron		0xcf

#   define	PS_emdash		0xd0
/*0xd1*/
/*0xd2*/
/*0xd3*/
/*0xd4*/
/*0xd5*/
/*0xd6*/
/*0xd7*/

/*0330*/
/*0331*/
/*0332*/
/*0333*/
/*0334*/
/*0335*/
/*0336*/
/*0337*/

/*0xe0=0340*/
#   define	PS_AE			0xe1
/*0xe2*/
#   define	PS_ordfeminine		0xe3
/*0xe*/
/*0xe5*/
/*0xe6*/
/*0xe7*/

#   define	PS_Lslash		0xc8
#   define	PS_Oslash		0xe9
#   define	PS_OE			0xea
#   define	PS_ordmasculine		0xeb
/*0354*/
/*0355*/
/*0356*/
/*0357*/

/*0360*/
#   define	PS_ae			0xf1
/*0362*/
/*0363*/
/*0364*/
#   define	PS_dotlessi		0xf5
/*0366*/
/*0367*/

#   define	PS_lslash		0xf8
#   define	PS_oslash		0xf9
#   define	PS_oe			0xfa
#   define	PS_germandbls		0xfb
/*0374*/
/*0375*/
/*0376*/
/*0377*/

/************************************************************************/
/*									*/
/*  NeXT PostScript encoding.						*/
/*									*/
/************************************************************************/

#   define	NX_nul		ASCII_nul
#   define	NX_soh		ASCII_soh
#   define	NX_stx		ASCII_stx
#   define	NX_etx		ASCII_etx
#   define	NX_eot		ASCII_eot
#   define	NX_enq		ASCII_enq
#   define	NX_ack		ASCII_ack
#   define	NX_bel		ASCII_bel
#   define	NX_bs		ASCII_bs
#   define	NX_ht		ASCII_ht
#   define	NX_nl		ASCII_nl
#   define	NX_vt		ASCII_vt
#   define	NX_np		ASCII_np
#   define	NX_cr		ASCII_cr
#   define	NX_so		ASCII_so
#   define	NX_si		ASCII_si
#   define	NX_dle		ASCII_dle
#   define	NX_dc1		ASCII_dc1
#   define	NX_dc2		ASCII_dc2
#   define	NX_dc3		ASCII_dc3
#   define	NX_dc4		ASCII_dc4
#   define	NX_nak		ASCII_nak
#   define	NX_syn		ASCII_syn
#   define	NX_etb		ASCII_etb
#   define	NX_can		ASCII_can
#   define	NX_em		ASCII_em
#   define	NX_sub		ASCII_sub
#   define	NX_esc		ASCII_esc
#   define	NX_fs		ASCII_fs
#   define	NX_gs		ASCII_gs
#   define	NX_rs		ASCII_rs
#   define	NX_us		ASCII_us

#   define	NX_space	PS_space
#   define	NX_exclam	PS_exclam
#   define	NX_quotedbl	PS_quotedbl
#   define	NX_numbersign	PS_numbersign
#   define	NX_dollar	PS_dollar
#   define	NX_percent	PS_percent
#   define	NX_ampersand	PS_ampersand
#   define	NX_quoteright	PS_quoteright

#   define	NX_parenleft	PS_parenleft
#   define	NX_parenright	PS_parenright
#   define	NX_asterisk	PS_asterisk
#   define	NX_plus		PS_plus
#   define	NX_comma	PS_comma
#   define	NX_minus	PS_minus
#   define	NX_period	PS_period
#   define	NX_slash	PS_slash

#   define	NX_zero		PS_zero
#   define	NX_one		PS_one
#   define	NX_two		PS_two
#   define	NX_three	PS_three
#   define	NX_four		PS_four
#   define	NX_five		PS_five
#   define	NX_six		PS_six
#   define	NX_seven	PS_seven

#   define	NX_eight	PS_eight
#   define	NX_nine		PS_nine
#   define	NX_colon	PS_colon
#   define	NX_semicolon	PS_semicolon
#   define	NX_less		PS_less
#   define	NX_equal	PS_equal
#   define	NX_greater	PS_greater
#   define	NX_question	PS_question

#   define	NX_at		PS_at
#   define	NX_A		PS_A
#   define	NX_B		PS_B
#   define	NX_C		PS_C
#   define	NX_D		PS_D
#   define	NX_E		PS_E
#   define	NX_F		PS_F
#   define	NX_G		PS_G

#   define	NX_H		PS_H
#   define	NX_I		PS_I
#   define	NX_J		PS_J
#   define	NX_K		PS_K
#   define	NX_L		PS_L
#   define	NX_M		PS_M
#   define	NX_N		PS_N
#   define	NX_O		PS_O

#   define	NX_P		PS_P
#   define	NX_Q		PS_Q
#   define	NX_R		PS_R
#   define	NX_S		PS_S
#   define	NX_T		PS_T
#   define	NX_U		PS_U
#   define	NX_V		PS_V
#   define	NX_W		PS_W

#   define	NX_X		PS_X
#   define	NX_Y		PS_Y
#   define	NX_Z		PS_Z
#   define	NX_bracketleft	PS_bracketleft
#   define	NX_backslash	PS_backslash
#   define	NX_bracketright	PS_bracketright
#   define	NX_circumflex	PS_circumflex
#   define	NX_underscore	PS_underscore

#   define	NX_quoteleft	PS_quoteleft
#   define	NX_a		PS_a
#   define	NX_b		PS_b
#   define	NX_c		PS_c
#   define	NX_d		PS_d
#   define	NX_e		PS_e
#   define	NX_f		PS_f
#   define	NX_g		PS_g

#   define	NX_h		PS_h
#   define	NX_i		PS_i
#   define	NX_j		PS_j
#   define	NX_k		PS_k
#   define	NX_l		PS_l
#   define	NX_m		PS_m
#   define	NX_n		PS_n
#   define	NX_o		PS_o

#   define	NX_p		PS_p
#   define	NX_q		PS_q
#   define	NX_r		PS_r
#   define	NX_s		PS_s
#   define	NX_t		PS_t
#   define	NX_u		PS_u
#   define	NX_v		PS_v
#   define	NX_w		PS_w

#   define	NX_x		PS_x
#   define	NX_y		PS_y
#   define	NX_z		PS_z
#   define	NX_braceleft	PS_braceleft
#   define	NX_bar		PS_bar
#   define	NX_braceright	PS_braceright
#   define	NX_asciitilde	PS_asciitilde
#   define	NX_del		ASCII_del

/*0200*/
#   define	NX_Agrave		0x81
#   define	NX_Aacute		0x82
#   define	NX_Acircumflex		0x83
#   define	NX_Atilde		0x84
#   define	NX_Adieresis		0x85
#   define	NX_Aring		0x86
#   define	NX_Ccedilla		0x87

#   define	NX_Egrave		0x88
#   define	NX_Eacute		0x89
#   define	NX_Ecircumflex		0x8a
#   define	NX_Edieresis		0x8b
#   define	NX_Igrave		0x8c
#   define	NX_Iacute		0x8d
#   define	NX_Icircumflex		0x8e
#   define	NX_Idieresis		0x8f

#   define	NX_Eth			0x90
#   define	NX_Ntilde		0x91
#   define	NX_Ograve		0x92
#   define	NX_Oacute		0x93
#   define	NX_Ocircumflex		0x94
#   define	NX_Otilde		0x95
#   define	NX_Odieresis		0x96
#   define	NX_Ugrave		0x97

#   define	NX_Uacute		0x98
#   define	NX_Ucircumflex		0x99
#   define	NX_Udieresis		0x9a
#   define	NX_Yacute		0x9b
#   define	NX_Thorn		0x9c
#   define	NX_mu			0x9d
#   define	NX_multiply		0x9e
#   define	NX_divide		0x9f

#   define	NX_copyright		0xa0
#   define	NX_exclamdown		PS_exclamdown
#   define	NX_cent			PS_cent
#   define	NX_sterling		PS_sterling
#   define	NX_fraction		PS_fraction
#   define	NX_yen			PS_yen
#   define	NX_florin		PS_florin
#   define	NX_section		PS_section

#   define	NX_currency		PS_currency
#   define	NX_quotesingle		PS_quotesingle
#   define	NX_quotedblleft		PS_quotednlleft
#   define	NX_guillemotleft	PS_guillemotleft
#   define	NX_guillsinglleft	PS_guillsinglleft
#   define	NX_guillsinglright	PS_guillsinglright
#   define	NX_fi			PS_fi
#   define	NX_fl			PS_fl

#   define	NX_registered		0xb0
#   define	NX_endash		PS_endash
#   define	NX_dagger		PS_dagger
#   define	NX_daggerdbl		PS_daggerdbl
#   define	NX_periodcentered	PS_periodcentered
#   define	NX_brokenbar		0xb5
#   define	NX_paragraph		PS_paragraph
#   define	NX_bullet		PS_bullet

#   define	NX_quotesinglbase	PS_quotesinglbase
#   define	NX_quotedblbase		PS_quotedblbase
#   define	NX_quotedblright	PS_quotedblright
#   define	NX_guillemotright	PS_guillemotright
#   define	NX_ellipsis		PS_ellipsis
#   define	NX_perthousand		PS_perthousand
#   define	NX_logicalnot		0xbe
#   define	NX_questiondown		PS_questiondown

#   define	NX_onesuperior		0xc0
#   define	NX_grave		PS_grave
#   define	NX_acute		PS_acute
#   define	NX_circumflex		PS_circumflex
#   define	NX_tilde		PS_tilde
#   define	NX_macron		PS_macron
#   define	NX_breve		PS_breve
#   define	NX_dotaccent		PS_dotaccent

#   define	NX_dieresis		PS_dieresis
#   define	NX_twosuperior		0311
#   define	NX_ring			PS_ring
#   define	NX_cedilla		PS_cedilla
#   define	NX_threesuperior	0314
#   define	NX_hungarumlaut		PS_hungarumlaut
#   define	NX_ogonek		PS_ogonek
#   define	NX_caron		PS_caron

#   define	NX_emdash		PS_emdash
#   define	NX_plusminus		0xd1
#   define	NX_onequarter		0xd2
#   define	NX_onehalf		0xd3
#   define	NX_threequarters	0xd4
#   define	NX_agrave		0xd5
#   define	NX_aacute		0xd6
#   define	NX_acircumflex		0xd7

#   define	NX_atilde		0xd8
#   define	NX_adieresis		0xd9
#   define	NX_aring		0xda
#   define	NX_ccedilla		0xdb
#   define	NX_egrave		0xdc
#   define	NX_eacute		0xdd
#   define	NX_ecircumflex		0xde
#   define	NX_edieresis		0xdf

#   define	NX_igrave		0xe0
#   define	NX_AE			PS_AE
#   define	NX_iacute		0xe2
#   define	NX_ordfeminine		PS_ordfeminine
#   define	NX_icircumflex		0xe4
#   define	NX_idieresis		0xe5
#   define	NX_eth			0xe6
#   define	NX_ntilde		0xe7

#   define	NX_Lslash		PS_Lslash
#   define	NX_Oslash		PS_Oslash
#   define	NX_OE			PS_OE
#   define	NX_ordmasculine		PS_ordmasculine
#   define	NX_ograve		0xec
#   define	NX_oacute		0xed
#   define	NX_ocircumflex		0xee
#   define	NX_otilde		0xef

#   define	NX_odieresis		0xf0
#   define	NX_ae			PS_ae
#   define	NX_ugrave		0xf2
#   define	NX_uacute		0xf3
#   define	NX_ucircumflex		0xf4
#   define	NX_dotlessi		PS_dotlessi
#   define	NX_udieresis		0xf6
#   define	NX_yacute		0xf7

#   define	NX_lslash		PS_lslash
#   define	NX_oslash		PS_oslash
#   define	NX_oe			PS_oe
#   define	NX_germandbls		PS_germandbls
#   define	NX_thorn		0xfc
#   define	NX_ydieresis		0xfd
/*0376*/
/*0377*/

/************************************************************************/
/*									*/
/*  EBCDIC as used by the IBM 3270 series terminals.			*/
/*									*/
/************************************************************************/

#   define	EBCDIC_space		0x40
#   define	EBCDIC_0x41		0x41	/*  Non breaking space	*/
#   define	EBCDIC_acircumflex	0x42
#   define	EBCDIC_adieresis	0x43
#   define	EBCDIC_agrave		0x44
#   define	EBCDIC_aacute		0x45
#   define	EBCDIC_atilde		0x46
#   define	EBCDIC_a_XXX		0x47
#   define	EBCDIC_c_XXX		0x48
#   define	EBCDIC_ntilde		0x49
#   define	EBCDIC_cent		0x4a
#   define	EBCDIC_period		0x4b
#   define	EBCDIC_less		0x4c
#   define	EBCDIC_parenleft	0x4d
#   define	EBCDIC_plus		0x4e
#   define	EBCDIC_bar		0x4f
#   define	EBCDIC_ampersand	0x50
#   define	EBCDIC_eacute		0x51
#   define	EBCDIC_ecircumflex	0x52
#   define	EBCDIC_edieresis	0x53
#   define	EBCDIC_egrave		0x54
#   define	EBCDIC_iacute		0x55
#   define	EBCDIC_icircumflex	0x56
#   define	EBCDIC_idieresis	0x57
#   define	EBCDIC_igrave		0x58
#   define	EBCDIC_germandbls	0x59
#   define	EBCDIC_exclam		0x5a
#   define	EBCDIC_dollar		0x5b
#   define	EBCDIC_asterisk		0x5c
#   define	EBCDIC_parenright	0x5d
#   define	EBCDIC_semicolon	0x5e
#   define	EBCDIC_logicalnot	0x5f
#   define	EBCDIC_minus		0x60
#   define	EBCDIC_slash		0x61
#   define	EBCDIC_Acircumflex	0x62
#   define	EBCDIC_Adieresis	0x63
#   define	EBCDIC_Agrave		0x64
#   define	EBCDIC_Aacute		0x65
#   define	EBCDIC_Atilde		0x66
#   define	EBCDIC_Aring		0x67
#   define	EBCDIC_Ccedilla		0x68
#   define	EBCDIC_Ntilde		0x69
#   define	EBCDIC_brokenbar	0x6a
#   define	EBCDIC_comma		0x6b
#   define	EBCDIC_percent		0x6c
#   define	EBCDIC_underscore	0x6d
#   define	EBCDIC_greater		0x6e
#   define	EBCDIC_question		0x6f
#   define	EBCDIC_oslash		0x70
#   define	EBCDIC_Eacute		0x71
#   define	EBCDIC_Ecircumflex	0x72
#   define	EBCDIC_Edieresis	0x73
#   define	EBCDIC_Egrave		0x74
#   define	EBCDIC_Iacute		0x75
#   define	EBCDIC_Icircumflex	0x76
#   define	EBCDIC_Idieresis	0x77
#   define	EBCDIC_Igrave		0x78
#   define	EBCDIC_grave		0x79
#   define	EBCDIC_colon		0x7a
#   define	EBCDIC_numbersign	0x7b
#   define	EBCDIC_at		0x7c
#   define	EBCDIC_quotesingle	0x7d
#   define	EBCDIC_equal		0x7e
#   define	EBCDIC_quoteddbl	0x7f
#   define	EBCDIC_Oslash		0x80
#   define	EBCDIC_a		0x81
#   define	EBCDIC_b		0x82
#   define	EBCDIC_c		0x83
#   define	EBCDIC_d		0x84
#   define	EBCDIC_e		0x85
#   define	EBCDIC_f		0x86
#   define	EBCDIC_g		0x87
#   define	EBCDIC_h		0x88
#   define	EBCDIC_i		0x89
#   define	EBCDIC_guillemotleft	0x8a
#   define	EBCDIC_guillemotright	0x8b
#   define	EBCDIC_eth		0x8c
#   define	EBCDIC_yacute		0x8d
#   define	EBCDIC_thorn		0x8e
#   define	EBCDIC_plusminus	0x8f
#   define	EBCDIC_degree		0x90
#   define	EBCDIC_j		0x91
#   define	EBCDIC_k		0x92
#   define	EBCDIC_l		0x93
#   define	EBCDIC_m		0x94
#   define	EBCDIC_n		0x95
#   define	EBCDIC_o		0x96
#   define	EBCDIC_p		0x97
#   define	EBCDIC_q		0x98
#   define	EBCDIC_r		0x99
#   define	EBCDIC_ordfeminine	0x9a
#   define	EBCDIC_ordmasculine	0x9b
#   define	EBCDIC_ae		0x9c
#   define	EBCDIC_cedilla		0x9d
#   define	EBCDIC_AE		0x9e
#   define	EBCDIC_currency		0x9f
#   define	EBCDIC_mu		0xa0
#   define	EBCDIC_tilde		0xa1
#   define	EBCDIC_s		0xa2
#   define	EBCDIC_t		0xa3
#   define	EBCDIC_u		0xa4
#   define	EBCDIC_v		0xa5
#   define	EBCDIC_w		0xa6
#   define	EBCDIC_x		0xa7
#   define	EBCDIC_y		0xa8
#   define	EBCDIC_z		0xa9
#   define	EBCDIC_exclamdown	0xaa
#   define	EBCDIC_questiondown	0xab
#   define	EBCDIC_Eth		0xac
#   define	EBCDIC_Yacute		0xad
#   define	EBCDIC_Thorn		0xae
#   define	EBCDIC_registered	0xaf
#   define	EBCDIC_circumflex	0xb0
#   define	EBCDIC_sterling		0xb1
#   define	EBCDIC_yen		0xb2
#   define	EBCDIC_dotcentered	0xb3
#   define	EBCDIC_copyright	0xb4
#   define	EBCDIC_section		0xb5
#   define	EBCDIC_paragraph	0xb6
#   define	EBCDIC_onequarter	0xb7
#   define	EBCDIC_onehalf		0xb8
#   define	EBCDIC_threequarters	0xb9
#   define	EBCDIC_bracketleft	0xba
#   define	EBCDIC_bracketright	0xbb
#   define	EBCDIC_overscore	0xbc /**/
#   define	EBCDIC_dieresis		0xbd
#   define	EBCDIC_acute		0xbe
#   define	EBCDIC_multiply		0xbf
#   define	EBCDIC_braceleft	0xc0
#   define	EBCDIC_A		0xc1
#   define	EBCDIC_B		0xc2
#   define	EBCDIC_C		0xc3
#   define	EBCDIC_D		0xc4
#   define	EBCDIC_E		0xc5
#   define	EBCDIC_F		0xc6
#   define	EBCDIC_G		0xc7
#   define	EBCDIC_H		0xc8
#   define	EBCDIC_I		0xc9
#   define	EBCDIC_hyphen		0xca
#   define	EBCDIC_ocircumflex	0xcb
#   define	EBCDIC_odieresis	0xcc
#   define	EBCDIC_ograve		0xcd
#   define	EBCDIC_oacute		0xce
#   define	EBCDIC_otilde		0xcf
#   define	EBCDIC_braceright	0xd0
#   define	EBCDIC_J		0xd1
#   define	EBCDIC_K		0xd2
#   define	EBCDIC_L		0xd3
#   define	EBCDIC_M		0xd4
#   define	EBCDIC_N		0xd5
#   define	EBCDIC_O		0xd6
#   define	EBCDIC_P		0xd7
#   define	EBCDIC_Q		0xd8
#   define	EBCDIC_R		0xd9
#   define	EBCDIC_onesuperior	0xda
#   define	EBCDIC_ucircumflex	0xdb
#   define	EBCDIC_udieresis	0xdc
#   define	EBCDIC_ugrave		0xdd
#   define	EBCDIC_uacute		0xde
#   define	EBCDIC_ydieresis	0xdf
#   define	EBCDIC_backslash	0xe0
#   define	EBCDIC_divide		0xe1
#   define	EBCDIC_S		0xe2
#   define	EBCDIC_T		0xe3
#   define	EBCDIC_U		0xe4
#   define	EBCDIC_V		0xe5
#   define	EBCDIC_W		0xe6
#   define	EBCDIC_X		0xe7
#   define	EBCDIC_Y		0xe8
#   define	EBCDIC_Z		0xe9
#   define	EBCDIC_twosuperior	0xea
#   define	EBCDIC_Ocircumflex	0xeb
#   define	EBCDIC_Odieresis	0xec
#   define	EBCDIC_Ograve		0xed
#   define	EBCDIC_Oacute		0xee
#   define	EBCDIC_Otilde		0xef
#   define	EBCDIC_zero		0xf0
#   define	EBCDIC_one		0xf1
#   define	EBCDIC_two		0xf2
#   define	EBCDIC_three		0xf3
#   define	EBCDIC_four		0xf4
#   define	EBCDIC_five		0xf5
#   define	EBCDIC_six		0xf6
#   define	EBCDIC_seven		0xf7
#   define	EBCDIC_eight		0xf8
#   define	EBCDIC_nine		0xf9
#   define	EBCDIC_threesuperior	0xfa
#   define	EBCDIC_Ucircumflex	0xfb
#   define	EBCDIC_Udieresis	0xfc
#   define	EBCDIC_Ugrave		0xfd
#   define	EBCDIC_Uacute		0xfe
#   define	EBCDIC_OE		0xff

/************************************************************************/
/*									*/
/*  MS-DOS code page 437						*/
/*									*/
/************************************************************************/

#   define	DOS437__0x00		0x00	/*    0		*/
#   define	DOS437__0x01		0x01	/*    1		*/
#   define	DOS437__0x02		0x02	/*    2		*/
#   define	DOS437__0x03		0x03	/*    3		*/
#   define	DOS437__0x04		0x04	/*    4		*/
#   define	DOS437__0x05		0x05	/*    5		*/
#   define	DOS437__0x06		0x06	/*    6		*/
#   define	DOS437__0x07		0x07	/*    7		*/
#   define	DOS437__0x08		0x08	/*    8		*/
#   define	DOS437__0x09		0x09	/*    9		*/
#   define	DOS437__0x0a		0x0a	/*   10		*/
#   define	DOS437__0x0b		0x0b	/*   11		*/
#   define	DOS437__0x0c		0x0c	/*   12		*/
#   define	DOS437__0x0d		0x0d	/*   13		*/
#   define	DOS437__0x0e		0x0e	/*   14		*/
#   define	DOS437__0x0f		0x0f	/*   15		*/
#   define	DOS437__0x10		0x10	/*   16		*/
#   define	DOS437__0x11		0x11	/*   17		*/
#   define	DOS437__0x12		0x12	/*   18		*/
#   define	DOS437__0x13		0x13	/*   19		*/
#   define	DOS437__0x14		0x14	/*   20		*/
#   define	DOS437__0x15		0x15	/*   21		*/
#   define	DOS437__0x16		0x16	/*   22		*/
#   define	DOS437__0x17		0x17	/*   23		*/
#   define	DOS437__0x18		0x18	/*   24		*/
#   define	DOS437__0x19		0x19	/*   25		*/
#   define	DOS437__0x1a		0x1a	/*   26		*/
#   define	DOS437__0x1b		0x1b	/*   27		*/
#   define	DOS437__0x1c		0x1c	/*   28		*/
#   define	DOS437__0x1d		0x1d	/*   29		*/
#   define	DOS437__0x1e		0x1e	/*   30		*/
#   define	DOS437__0x1f		0x1f	/*   31		*/
#   define	DOS437_space		0x20	/*   32		*/
#   define	DOS437_exclam		0x21	/*   33		*/
#   define	DOS437_quotedbl		0x22	/*   34		*/
#   define	DOS437_numbersign	0x23	/*   35		*/
#   define	DOS437_dollar		0x24	/*   36		*/
#   define	DOS437_percent		0x25	/*   37		*/
#   define	DOS437_ampersand	0x26	/*   38		*/
#   define	DOS437_quotesingle	0x27	/*   39		*/
#   define	DOS437_parenleft	0x28	/*   40		*/
#   define	DOS437_parenright	0x29	/*   41		*/
#   define	DOS437_asterisk		0x2a	/*   42		*/
#   define	DOS437_plus		0x2b	/*   43		*/
#   define	DOS437_comma		0x2c	/*   44		*/
#   define	DOS437_minus		0x2d	/*   45		*/
#   define	DOS437_period		0x2e	/*   46		*/
#   define	DOS437_slash		0x2f	/*   47		*/
#   define	DOS437_zero		0x30	/*   48		*/
#   define	DOS437_one		0x31	/*   49		*/
#   define	DOS437_two		0x32	/*   50		*/
#   define	DOS437_three		0x33	/*   51		*/
#   define	DOS437_four		0x34	/*   52		*/
#   define	DOS437_five		0x35	/*   53		*/
#   define	DOS437_six		0x36	/*   54		*/
#   define	DOS437_seven		0x37	/*   55		*/
#   define	DOS437_eight		0x38	/*   56		*/
#   define	DOS437_nine		0x39	/*   57		*/
#   define	DOS437_colon		0x3a	/*   58		*/
#   define	DOS437_semicolon	0x3b	/*   59		*/
#   define	DOS437_less		0x3c	/*   60		*/
#   define	DOS437_equal		0x3d	/*   61		*/
#   define	DOS437_greater		0x3e	/*   62		*/
#   define	DOS437_question		0x3f	/*   63		*/
#   define	DOS437_at		0x40	/*   64		*/
#   define	DOS437_A		0x41	/*   65		*/
#   define	DOS437_B		0x42	/*   66		*/
#   define	DOS437_C		0x43	/*   67		*/
#   define	DOS437_D		0x44	/*   68		*/
#   define	DOS437_E		0x45	/*   69		*/
#   define	DOS437_F		0x46	/*   70		*/
#   define	DOS437_G		0x47	/*   71		*/
#   define	DOS437_H		0x48	/*   72		*/
#   define	DOS437_I		0x49	/*   73		*/
#   define	DOS437_J		0x4a	/*   74		*/
#   define	DOS437_K		0x4b	/*   75		*/
#   define	DOS437_L		0x4c	/*   76		*/
#   define	DOS437_M		0x4d	/*   77		*/
#   define	DOS437_N		0x4e	/*   78		*/
#   define	DOS437_O		0x4f	/*   79		*/
#   define	DOS437_P		0x50	/*   80		*/
#   define	DOS437_Q		0x51	/*   81		*/
#   define	DOS437_R		0x52	/*   82		*/
#   define	DOS437_S		0x53	/*   83		*/
#   define	DOS437_T		0x54	/*   84		*/
#   define	DOS437_U		0x55	/*   85		*/
#   define	DOS437_V		0x56	/*   86		*/
#   define	DOS437_W		0x57	/*   87		*/
#   define	DOS437_X		0x58	/*   88		*/
#   define	DOS437_Y		0x59	/*   89		*/
#   define	DOS437_Z		0x5a	/*   90		*/
#   define	DOS437_bracketleft	0x5b	/*   91		*/
#   define	DOS437_backslash	0x5c	/*   92		*/
#   define	DOS437_bracketright	0x5d	/*   93		*/
#   define	DOS437_circumflex	0x5e	/*   94		*/
#   define	DOS437_underscore	0x5f	/*   95		*/
#   define	DOS437_quoteleft	0x60	/*   96		*/
#   define	DOS437_a		0x61	/*   97		*/
#   define	DOS437_b		0x62	/*   98		*/
#   define	DOS437_c		0x63	/*   99		*/
#   define	DOS437_d		0x64	/*  100		*/
#   define	DOS437_e		0x65	/*  101		*/
#   define	DOS437_f		0x66	/*  102		*/
#   define	DOS437_g		0x67	/*  103		*/
#   define	DOS437_h		0x68	/*  104		*/
#   define	DOS437_i		0x69	/*  105		*/
#   define	DOS437_j		0x6a	/*  106		*/
#   define	DOS437_k		0x6b	/*  107		*/
#   define	DOS437_l		0x6c	/*  108		*/
#   define	DOS437_m		0x6d	/*  109		*/
#   define	DOS437_n		0x6e	/*  110		*/
#   define	DOS437_o		0x6f	/*  111		*/
#   define	DOS437_p		0x70	/*  112		*/
#   define	DOS437_q		0x71	/*  113		*/
#   define	DOS437_r		0x72	/*  114		*/
#   define	DOS437_s		0x73	/*  115		*/
#   define	DOS437_t		0x74	/*  116		*/
#   define	DOS437_u		0x75	/*  117		*/
#   define	DOS437_v		0x76	/*  118		*/
#   define	DOS437_w		0x77	/*  119		*/
#   define	DOS437_x		0x78	/*  120		*/
#   define	DOS437_y		0x79	/*  121		*/
#   define	DOS437_z		0x7a	/*  122		*/
#   define	DOS437_braceleft	0x7b	/*  123		*/
#   define	DOS437_bar		0x7c	/*  124		*/
#   define	DOS437_braceright	0x7d	/*  125		*/
#   define	DOS437_tilde		0x7e	/*  126		*/
#   define	DOS437__0x7f		0x7f	/*  127		*/
#   define	DOS437_Ccedilla		0x80	/*  128		*/
#   define	DOS437_udieresis	0x81	/*  129		*/
#   define	DOS437_eacute		0x82	/*  130		*/
#   define	DOS437_acircumflex	0x83	/*  131		*/
#   define	DOS437_adieresis	0x84	/*  132		*/
#   define	DOS437_agrave		0x85	/*  133		*/
#   define	DOS437_aring		0x86	/*  134		*/
#   define	DOS437_ccedilla		0x87	/*  135		*/
#   define	DOS437_ecircumflex	0x88	/*  136		*/
#   define	DOS437_edieresis	0x89	/*  137		*/
#   define	DOS437_egrave		0x8a	/*  138		*/
#   define	DOS437_idieresis	0x8b	/*  139		*/
#   define	DOS437_icircumflex	0x8c	/*  140		*/
#   define	DOS437_igrave		0x8d	/*  141		*/
#   define	DOS437_Adieresis	0x8e	/*  142		*/
#   define	DOS437_Aring		0x8f	/*  143		*/
#   define	DOS437_Eacute		0x90	/*  144		*/
#   define	DOS437_ae		0x91	/*  145		*/
#   define	DOS437_AE		0x92	/*  146		*/
#   define	DOS437_ocircumflex	0x93	/*  147		*/
#   define	DOS437_odieresis	0x94	/*  148		*/
#   define	DOS437_ograve		0x95	/*  149		*/
#   define	DOS437_ucircumflex	0x96	/*  150		*/
#   define	DOS437_ugrave		0x97	/*  151		*/
#   define	DOS437_ydieresis	0x98	/*  152		*/
#   define	DOS437_Odieresis	0x99	/*  153		*/
#   define	DOS437_Udieresis	0x9a	/*  154		*/
#   define	DOS437_cent		0x9b	/*  155		*/
#   define	DOS437_sterling		0x9c	/*  156		*/
#   define	DOS437_yen		0x9d	/*  157		*/
#   define	DOS437_peseta		0x9e	/*  158		*/
#   define	DOS437_florin		0x9f	/*  159		*/
#   define	DOS437_aacute		0xa0	/*  160		*/
#   define	DOS437_iacute		0xa1	/*  161		*/
#   define	DOS437_oacute		0xa2	/*  162		*/
#   define	DOS437_uacute		0xa3	/*  163		*/
#   define	DOS437_ntilde		0xa4	/*  164		*/
#   define	DOS437_Ntilde		0xa5	/*  165		*/
#   define	DOS437_ordfeminine	0xa6	/*  166		*/
#   define	DOS437_ordmasculine	0xa7	/*  167		*/
#   define	DOS437_questiondown	0xa8	/*  168		*/
#   define	DOS437_revlogicalnot	0xa9	/*  169		*/
#   define	DOS437_logicalnot	0xaa	/*  170		*/
#   define	DOS437_onehalf		0xab	/*  171		*/
#   define	DOS437_onequarter	0xac	/*  172		*/
#   define	DOS437_exclamdown	0xad	/*  173		*/
#   define	DOS437_guillemotleft	0xae	/*  174		*/
#   define	DOS437_guillemotright	0xaf	/*  175		*/
#   define	DOS437_ltshade		0xb0	/*  176		*/
#   define	DOS437_shade		0xb1	/*  177		*/
#   define	DOS437_dkshade		0xb2	/*  178		*/
#   define	DOS437_SF110000		0xb3	/*  179		*/
#   define	DOS437_SF090000		0xb4	/*  180		*/
#   define	DOS437_SF190000		0xb5	/*  181		*/
#   define	DOS437_SF200000		0xb6	/*  182		*/
#   define	DOS437_SF210000		0xb7	/*  183		*/
#   define	DOS437_SF220000		0xb8	/*  184		*/
#   define	DOS437_SF230000		0xb9	/*  185		*/
#   define	DOS437_SF240000		0xba	/*  186		*/
#   define	DOS437_SF250000		0xbb	/*  187		*/
#   define	DOS437_SF260000		0xbc	/*  188		*/
#   define	DOS437_SF270000		0xbd	/*  189		*/
#   define	DOS437_SF280000		0xbe	/*  190		*/
#   define	DOS437_SF030000		0xbf	/*  191		*/
#   define	DOS437_SF020000		0xc0	/*  192		*/
#   define	DOS437_SF070000		0xc1	/*  193		*/
#   define	DOS437_SF060000		0xc2	/*  194		*/
#   define	DOS437_SF080000		0xc3	/*  195		*/
#   define	DOS437_SF100000		0xc4	/*  196		*/
#   define	DOS437_SF050000		0xc5	/*  197		*/
#   define	DOS437_SF360000		0xc6	/*  198		*/
#   define	DOS437_SF370000		0xc7	/*  199		*/
#   define	DOS437_SF380000		0xc8	/*  200		*/
#   define	DOS437_SF390000		0xc9	/*  201		*/
#   define	DOS437_SF400000		0xca	/*  202		*/
#   define	DOS437_SF410000		0xcb	/*  203		*/
#   define	DOS437_SF420000		0xcc	/*  204		*/
#   define	DOS437_SF430000		0xcd	/*  205		*/
#   define	DOS437_SF440000		0xce	/*  206		*/
#   define	DOS437_SF450000		0xcf	/*  207		*/
#   define	DOS437_SF460000		0xd0	/*  208		*/
#   define	DOS437_SF470000		0xd1	/*  209		*/
#   define	DOS437_SF480000		0xd2	/*  210		*/
#   define	DOS437_SF490000		0xd3	/*  211		*/
#   define	DOS437_SF500000		0xd4	/*  212		*/
#   define	DOS437_SF510000		0xd5	/*  213		*/
#   define	DOS437_SF520000		0xd6	/*  214		*/
#   define	DOS437_SF530000		0xd7	/*  215		*/
#   define	DOS437_SF540000		0xd8	/*  216		*/
#   define	DOS437_SF040000		0xd9	/*  217		*/
#   define	DOS437_SF010000		0xda	/*  218		*/
#   define	DOS437_block		0xdb	/*  219		*/
#   define	DOS437_dnblock		0xdc	/*  220		*/
#   define	DOS437_lfblock		0xdd	/*  221		*/
#   define	DOS437_rtblock		0xde	/*  222		*/
#   define	DOS437_upblock		0xdf	/*  223		*/
#   define	DOS437_alpha		0xe0	/*  224		*/
#   define	DOS437_germandbls	0xe1	/*  225		*/
#   define	DOS437_Gamma		0xe2	/*  226		*/
#   define	DOS437_pi		0xe3	/*  227		*/
#   define	DOS437_Sigma		0xe4	/*  228		*/
#   define	DOS437_sigma		0xe5	/*  229		*/
#   define	DOS437_mu		0xe6	/*  230		*/
#   define	DOS437_tau		0xe7	/*  231		*/
#   define	DOS437_Phi		0xe8	/*  232		*/
#   define	DOS437_Theta		0xe9	/*  233		*/
#   define	DOS437_Omega		0xea	/*  234		*/
#   define	DOS437_delta		0xeb	/*  235		*/
#   define	DOS437_infinity		0xec	/*  236		*/
#   define	DOS437_phi		0xed	/*  237		*/
#   define	DOS437_epsilon		0xee	/*  238		*/
#   define	DOS437_intersection	0xef	/*  239		*/
#   define	DOS437_equivalence	0xf0	/*  240		*/
#   define	DOS437_plusminus	0xf1	/*  241		*/
#   define	DOS437_greaterequal	0xf2	/*  242		*/
#   define	DOS437_lessequal	0xf3	/*  243		*/
#   define	DOS437_integraltp	0xf4	/*  244		*/
#   define	DOS437_integralbt	0xf5	/*  245		*/
#   define	DOS437_divide		0xf6	/*  246		*/
#   define	DOS437_approxequal	0xf7	/*  247		*/
#   define	DOS437_degree		0xf8	/*  248		*/
#   define	DOS437_bullet		0xf9	/*  249		*/
#   define	DOS437_periodcentered	0xfa	/*  250		*/
#   define	DOS437_radical		0xfb	/*  251		*/
#   define	DOS437_nsuperior	0xfc	/*  252		*/
#   define	DOS437_twosuperior	0xfd	/*  253		*/
#   define	DOS437_filledbox	0xfe	/*  254		*/
#   define	DOS437_nobreakspace	0xff	/*  255		*/

/************************************************************************/
/*									*/
/*  MS-DOS code page 850						*/
/*									*/
/************************************************************************/

#   define	DOS850__0x00		0x00	/*    0		*/
#   define	DOS850__0x01		0x01	/*    1		*/
#   define	DOS850__0x02		0x02	/*    2		*/
#   define	DOS850__0x03		0x03	/*    3		*/
#   define	DOS850__0x04		0x04	/*    4		*/
#   define	DOS850__0x05		0x05	/*    5		*/
#   define	DOS850__0x06		0x06	/*    6		*/
#   define	DOS850__0x07		0x07	/*    7		*/
#   define	DOS850__0x08		0x08	/*    8		*/
#   define	DOS850__0x09		0x09	/*    9		*/
#   define	DOS850__0x0a		0x0a	/*   10		*/
#   define	DOS850__0x0b		0x0b	/*   11		*/
#   define	DOS850__0x0c		0x0c	/*   12		*/
#   define	DOS850__0x0d		0x0d	/*   13		*/
#   define	DOS850__0x0e		0x0e	/*   14		*/
#   define	DOS850__0x0f		0x0f	/*   15		*/
#   define	DOS850__0x10		0x10	/*   16		*/
#   define	DOS850__0x11		0x11	/*   17		*/
#   define	DOS850__0x12		0x12	/*   18		*/
#   define	DOS850__0x13		0x13	/*   19		*/
#   define	DOS850__0x14		0x14	/*   20		*/
#   define	DOS850__0x15		0x15	/*   21		*/
#   define	DOS850__0x16		0x16	/*   22		*/
#   define	DOS850__0x17		0x17	/*   23		*/
#   define	DOS850__0x18		0x18	/*   24		*/
#   define	DOS850__0x19		0x19	/*   25		*/
#   define	DOS850__0x1a		0x1a	/*   26		*/
#   define	DOS850__0x1b		0x1b	/*   27		*/
#   define	DOS850__0x1c		0x1c	/*   28		*/
#   define	DOS850__0x1d		0x1d	/*   29		*/
#   define	DOS850__0x1e		0x1e	/*   30		*/
#   define	DOS850__0x1f		0x1f	/*   31		*/
#   define	DOS850_space		0x20	/*   32		*/
#   define	DOS850_exclam		0x21	/*   33		*/
#   define	DOS850_quotedbl		0x22	/*   34		*/
#   define	DOS850_numbersign	0x23	/*   35		*/
#   define	DOS850_dollar		0x24	/*   36		*/
#   define	DOS850_percent		0x25	/*   37		*/
#   define	DOS850_ampersand	0x26	/*   38		*/
#   define	DOS850_quotesingle	0x27	/*   39		*/
#   define	DOS850_parenleft	0x28	/*   40		*/
#   define	DOS850_parenright	0x29	/*   41		*/
#   define	DOS850_asterisk		0x2a	/*   42		*/
#   define	DOS850_plus		0x2b	/*   43		*/
#   define	DOS850_comma		0x2c	/*   44		*/
#   define	DOS850_minus		0x2d	/*   45		*/
#   define	DOS850_period		0x2e	/*   46		*/
#   define	DOS850_slash		0x2f	/*   47		*/
#   define	DOS850_zero		0x30	/*   48		*/
#   define	DOS850_one		0x31	/*   49		*/
#   define	DOS850_two		0x32	/*   50		*/
#   define	DOS850_three		0x33	/*   51		*/
#   define	DOS850_four		0x34	/*   52		*/
#   define	DOS850_five		0x35	/*   53		*/
#   define	DOS850_six		0x36	/*   54		*/
#   define	DOS850_seven		0x37	/*   55		*/
#   define	DOS850_eight		0x38	/*   56		*/
#   define	DOS850_nine		0x39	/*   57		*/
#   define	DOS850_colon		0x3a	/*   58		*/
#   define	DOS850_semicolon	0x3b	/*   59		*/
#   define	DOS850_less		0x3c	/*   60		*/
#   define	DOS850_equal		0x3d	/*   61		*/
#   define	DOS850_greater		0x3e	/*   62		*/
#   define	DOS850_question		0x3f	/*   63		*/
#   define	DOS850_at		0x40	/*   64		*/
#   define	DOS850_A		0x41	/*   65		*/
#   define	DOS850_B		0x42	/*   66		*/
#   define	DOS850_C		0x43	/*   67		*/
#   define	DOS850_D		0x44	/*   68		*/
#   define	DOS850_E		0x45	/*   69		*/
#   define	DOS850_F		0x46	/*   70		*/
#   define	DOS850_G		0x47	/*   71		*/
#   define	DOS850_H		0x48	/*   72		*/
#   define	DOS850_I		0x49	/*   73		*/
#   define	DOS850_J		0x4a	/*   74		*/
#   define	DOS850_K		0x4b	/*   75		*/
#   define	DOS850_L		0x4c	/*   76		*/
#   define	DOS850_M		0x4d	/*   77		*/
#   define	DOS850_N		0x4e	/*   78		*/
#   define	DOS850_O		0x4f	/*   79		*/
#   define	DOS850_P		0x50	/*   80		*/
#   define	DOS850_Q		0x51	/*   81		*/
#   define	DOS850_R		0x52	/*   82		*/
#   define	DOS850_S		0x53	/*   83		*/
#   define	DOS850_T		0x54	/*   84		*/
#   define	DOS850_U		0x55	/*   85		*/
#   define	DOS850_V		0x56	/*   86		*/
#   define	DOS850_W		0x57	/*   87		*/
#   define	DOS850_X		0x58	/*   88		*/
#   define	DOS850_Y		0x59	/*   89		*/
#   define	DOS850_Z		0x5a	/*   90		*/
#   define	DOS850_bracketleft	0x5b	/*   91		*/
#   define	DOS850_backslash	0x5c	/*   92		*/
#   define	DOS850_bracketright	0x5d	/*   93		*/
#   define	DOS850_circumflex	0x5e	/*   94		*/
#   define	DOS850_underscore	0x5f	/*   95		*/
#   define	DOS850_quoteleft	0x60	/*   96		*/
#   define	DOS850_a		0x61	/*   97		*/
#   define	DOS850_b		0x62	/*   98		*/
#   define	DOS850_c		0x63	/*   99		*/
#   define	DOS850_d		0x64	/*  100		*/
#   define	DOS850_e		0x65	/*  101		*/
#   define	DOS850_f		0x66	/*  102		*/
#   define	DOS850_g		0x67	/*  103		*/
#   define	DOS850_h		0x68	/*  104		*/
#   define	DOS850_i		0x69	/*  105		*/
#   define	DOS850_j		0x6a	/*  106		*/
#   define	DOS850_k		0x6b	/*  107		*/
#   define	DOS850_l		0x6c	/*  108		*/
#   define	DOS850_m		0x6d	/*  109		*/
#   define	DOS850_n		0x6e	/*  110		*/
#   define	DOS850_o		0x6f	/*  111		*/
#   define	DOS850_p		0x70	/*  112		*/
#   define	DOS850_q		0x71	/*  113		*/
#   define	DOS850_r		0x72	/*  114		*/
#   define	DOS850_s		0x73	/*  115		*/
#   define	DOS850_t		0x74	/*  116		*/
#   define	DOS850_u		0x75	/*  117		*/
#   define	DOS850_v		0x76	/*  118		*/
#   define	DOS850_w		0x77	/*  119		*/
#   define	DOS850_x		0x78	/*  120		*/
#   define	DOS850_y		0x79	/*  121		*/
#   define	DOS850_z		0x7a	/*  122		*/
#   define	DOS850_braceleft	0x7b	/*  123		*/
#   define	DOS850_bar		0x7c	/*  124		*/
#   define	DOS850_braceright	0x7d	/*  125		*/
#   define	DOS850_tilde		0x7e	/*  126		*/
#   define	DOS850__0x7f		0x7f	/*  127		*/
#   define	DOS850_Ccedilla		0x80	/*  128		*/
#   define	DOS850_udieresis	0x81	/*  129		*/
#   define	DOS850_eacute		0x82	/*  130		*/
#   define	DOS850_acircumflex	0x83	/*  131		*/
#   define	DOS850_adieresis	0x84	/*  132		*/
#   define	DOS850_agrave		0x85	/*  133		*/
#   define	DOS850_aring		0x86	/*  134		*/
#   define	DOS850_ccedilla		0x87	/*  135		*/
#   define	DOS850_ecircumflex	0x88	/*  136		*/
#   define	DOS850_edieresis	0x89	/*  137		*/
#   define	DOS850_egrave		0x8a	/*  138		*/
#   define	DOS850_idieresis	0x8b	/*  139		*/
#   define	DOS850_icircumflex	0x8c	/*  140		*/
#   define	DOS850_igrave		0x8d	/*  141		*/
#   define	DOS850_Adieresis	0x8e	/*  142		*/
#   define	DOS850_Aring		0x8f	/*  143		*/
#   define	DOS850_Eacute		0x90	/*  144		*/
#   define	DOS850_ae		0x91	/*  145		*/
#   define	DOS850_AE		0x92	/*  146		*/
#   define	DOS850_ocircumflex	0x93	/*  147		*/
#   define	DOS850_odieresis	0x94	/*  148		*/
#   define	DOS850_ograve		0x95	/*  149		*/
#   define	DOS850_ucircumflex	0x96	/*  150		*/
#   define	DOS850_ugrave		0x97	/*  151		*/
#   define	DOS850_ydieresis	0x98	/*  152		*/
#   define	DOS850_Odieresis	0x99	/*  153		*/
#   define	DOS850_Udieresis	0x9a	/*  154		*/
#   define	DOS850_oslash		0x9b	/*  155		*/
#   define	DOS850_sterling		0x9c	/*  156		*/
#   define	DOS850_Oslash		0x9d	/*  157		*/
#   define	DOS850_multiply		0x9e	/*  158		*/
#   define	DOS850_florin		0x9f	/*  159		*/
#   define	DOS850_aacute		0xa0	/*  160		*/
#   define	DOS850_iacute		0xa1	/*  161		*/
#   define	DOS850_oacute		0xa2	/*  162		*/
#   define	DOS850_uacute		0xa3	/*  163		*/
#   define	DOS850_ntilde		0xa4	/*  164		*/
#   define	DOS850_Ntilde		0xa5	/*  165		*/
#   define	DOS850_ordfeminine	0xa6	/*  166		*/
#   define	DOS850_ordmasculine	0xa7	/*  167		*/
#   define	DOS850_questiondown	0xa8	/*  168		*/
#   define	DOS850_registered	0xa9	/*  169		*/
#   define	DOS850_logicalnot	0xaa	/*  170		*/
#   define	DOS850_onehalf		0xab	/*  171		*/
#   define	DOS850_onequarter	0xac	/*  172		*/
#   define	DOS850_exclamdown	0xad	/*  173		*/
#   define	DOS850_guillemotleft	0xae	/*  174		*/
#   define	DOS850_guillemotright	0xaf	/*  175		*/
#   define	DOS850_ltshade		0xb0	/*  176		*/
#   define	DOS850_shade		0xb1	/*  177		*/
#   define	DOS850_dkshade		0xb2	/*  178		*/
#   define	DOS850_SF110000		0xb3	/*  179		*/
#   define	DOS850_SF090000		0xb4	/*  180		*/
#   define	DOS850_Aacute		0xb5	/*  181		*/
#   define	DOS850_Acircumflex	0xb6	/*  182		*/
#   define	DOS850_Agrave		0xb7	/*  183		*/
#   define	DOS850_copyright	0xb8	/*  184		*/
#   define	DOS850_SF230000		0xb9	/*  185		*/
#   define	DOS850_SF240000		0xba	/*  186		*/
#   define	DOS850_SF250000		0xbb	/*  187		*/
#   define	DOS850_SF260000		0xbc	/*  188		*/
#   define	DOS850_cent		0xbd	/*  189		*/
#   define	DOS850_yen		0xbe	/*  190		*/
#   define	DOS850_SF030000		0xbf	/*  191		*/
#   define	DOS850_SF020000		0xc0	/*  192		*/
#   define	DOS850_SF070000		0xc1	/*  193		*/
#   define	DOS850_SF060000		0xc2	/*  194		*/
#   define	DOS850_SF080000		0xc3	/*  195		*/
#   define	DOS850_SF100000		0xc4	/*  196		*/
#   define	DOS850_SF050000		0xc5	/*  197		*/
#   define	DOS850_atilde		0xc6	/*  198		*/
#   define	DOS850_Atilde		0xc7	/*  199		*/
#   define	DOS850_SF380000		0xc8	/*  200		*/
#   define	DOS850_SF390000		0xc9	/*  201		*/
#   define	DOS850_SF400000		0xca	/*  202		*/
#   define	DOS850_SF410000		0xcb	/*  203		*/
#   define	DOS850_SF420000		0xcc	/*  204		*/
#   define	DOS850_SF430000		0xcd	/*  205		*/
#   define	DOS850_SF440000		0xce	/*  206		*/
#   define	DOS850_currency		0xcf	/*  207		*/
#   define	DOS850_eth		0xd0	/*  208		*/
#   define	DOS850_Eth		0xd1	/*  209		*/
#   define	DOS850_Ecircumflex	0xd2	/*  210		*/
#   define	DOS850_Edieresis	0xd3	/*  211		*/
#   define	DOS850_Egrave		0xd4	/*  212		*/
#   define	DOS850_dotlessi		0xd5	/*  213		*/
#   define	DOS850_Iacute		0xd6	/*  214		*/
#   define	DOS850_Icircumflex	0xd7	/*  215		*/
#   define	DOS850_Idieresis	0xd8	/*  216		*/
#   define	DOS850_SF040000		0xd9	/*  217		*/
#   define	DOS850_SF010000		0xda	/*  218		*/
#   define	DOS850_block		0xdb	/*  219		*/
#   define	DOS850_dnblock		0xdc	/*  220		*/
#   define	DOS850_brokenbar	0xdd	/*  221		*/
#   define	DOS850_Igrave		0xde	/*  222		*/
#   define	DOS850_upblock		0xdf	/*  223		*/
#   define	DOS850_Oacute		0xe0	/*  224		*/
#   define	DOS850_germandbls	0xe1	/*  225		*/
#   define	DOS850_Ocircumflex	0xe2	/*  226		*/
#   define	DOS850_Ograve		0xe3	/*  227		*/
#   define	DOS850_otilde		0xe4	/*  228		*/
#   define	DOS850_Otilde		0xe5	/*  229		*/
#   define	DOS850_mu		0xe6	/*  230		*/
#   define	DOS850_thorn		0xe7	/*  231		*/
#   define	DOS850_Thorn		0xe8	/*  232		*/
#   define	DOS850_Uacute		0xe9	/*  233		*/
#   define	DOS850_Ucircumflex	0xea	/*  234		*/
#   define	DOS850_Ugrave		0xeb	/*  235		*/
#   define	DOS850_yacute		0xec	/*  236		*/
#   define	DOS850_Yacute		0xed	/*  237		*/
#   define	DOS850_macron		0xee	/*  238		*/
#   define	DOS850_acute		0xef	/*  239		*/
#   define	DOS850_hyphen		0xf0	/*  240		*/
#   define	DOS850_plusminus	0xf1	/*  241		*/
#   define	DOS850_underscoredbl	0xf2	/*  242		*/
#   define	DOS850_threequarters	0xf3	/*  243		*/
#   define	DOS850_paragraph	0xf4	/*  244		*/
#   define	DOS850_section		0xf5	/*  245		*/
#   define	DOS850_divide		0xf6	/*  246		*/
#   define	DOS850_cedilla		0xf7	/*  247		*/
#   define	DOS850_degree		0xf8	/*  248		*/
#   define	DOS850_dieresis		0xf9	/*  249		*/
#   define	DOS850_periodcentered	0xfa	/*  250		*/
#   define	DOS850_onesuperior	0xfb	/*  251		*/
#   define	DOS850_threesuperior	0xfc	/*  252		*/
#   define	DOS850_twosuperior	0xfd	/*  253		*/
#   define	DOS850_filledbox	0xfe	/*  254		*/
#   define	DOS850_nobreakspace	0xff	/*  255		*/

/************************************************************************/
/*									*/
/*  The Apple MACINTOSH roman character set.				*/
/*									*/
/************************************************************************/

#   define	MAC_ROMAN_space			0x20	/*   32		*/
#   define	MAC_ROMAN_exclam		0x21	/*   33		*/
#   define	MAC_ROMAN_quotedbl		0x22	/*   34		*/
#   define	MAC_ROMAN_numbersign		0x23	/*   35		*/
#   define	MAC_ROMAN_dollar		0x24	/*   36		*/
#   define	MAC_ROMAN_percent		0x25	/*   37		*/
#   define	MAC_ROMAN_ampersand		0x26	/*   38		*/
#   define	MAC_ROMAN_quotesingle		0x27	/*   39		*/
#   define	MAC_ROMAN_parenleft		0x28	/*   40		*/
#   define	MAC_ROMAN_parenright		0x29	/*   41		*/
#   define	MAC_ROMAN_asterisk		0x2a	/*   42		*/
#   define	MAC_ROMAN_plus			0x2b	/*   43		*/
#   define	MAC_ROMAN_comma			0x2c	/*   44		*/
#   define	MAC_ROMAN_minus			0x2d	/*   45		*/
#   define	MAC_ROMAN_period		0x2e	/*   46		*/
#   define	MAC_ROMAN_slash			0x2f	/*   47		*/
#   define	MAC_ROMAN_zero			0x30	/*   48		*/
#   define	MAC_ROMAN_one			0x31	/*   49		*/
#   define	MAC_ROMAN_two			0x32	/*   50		*/
#   define	MAC_ROMAN_three			0x33	/*   51		*/
#   define	MAC_ROMAN_four			0x34	/*   52		*/
#   define	MAC_ROMAN_five			0x35	/*   53		*/
#   define	MAC_ROMAN_six			0x36	/*   54		*/
#   define	MAC_ROMAN_seven			0x37	/*   55		*/
#   define	MAC_ROMAN_eight			0x38	/*   56		*/
#   define	MAC_ROMAN_nine			0x39	/*   57		*/
#   define	MAC_ROMAN_colon			0x3a	/*   58		*/
#   define	MAC_ROMAN_semicolon		0x3b	/*   59		*/
#   define	MAC_ROMAN_less			0x3c	/*   60		*/
#   define	MAC_ROMAN_equal			0x3d	/*   61		*/
#   define	MAC_ROMAN_greater		0x3e	/*   62		*/
#   define	MAC_ROMAN_question		0x3f	/*   63		*/
#   define	MAC_ROMAN_at			0x40	/*   64		*/
#   define	MAC_ROMAN_A			0x41	/*   65		*/
#   define	MAC_ROMAN_B			0x42	/*   66		*/
#   define	MAC_ROMAN_C			0x43	/*   67		*/
#   define	MAC_ROMAN_D			0x44	/*   68		*/
#   define	MAC_ROMAN_E			0x45	/*   69		*/
#   define	MAC_ROMAN_F			0x46	/*   70		*/
#   define	MAC_ROMAN_G			0x47	/*   71		*/
#   define	MAC_ROMAN_H			0x48	/*   72		*/
#   define	MAC_ROMAN_I			0x49	/*   73		*/
#   define	MAC_ROMAN_J			0x4a	/*   74		*/
#   define	MAC_ROMAN_K			0x4b	/*   75		*/
#   define	MAC_ROMAN_L			0x4c	/*   76		*/
#   define	MAC_ROMAN_M			0x4d	/*   77		*/
#   define	MAC_ROMAN_N			0x4e	/*   78		*/
#   define	MAC_ROMAN_O			0x4f	/*   79		*/
#   define	MAC_ROMAN_P			0x50	/*   80		*/
#   define	MAC_ROMAN_Q			0x51	/*   81		*/
#   define	MAC_ROMAN_R			0x52	/*   82		*/
#   define	MAC_ROMAN_S			0x53	/*   83		*/
#   define	MAC_ROMAN_T			0x54	/*   84		*/
#   define	MAC_ROMAN_U			0x55	/*   85		*/
#   define	MAC_ROMAN_V			0x56	/*   86		*/
#   define	MAC_ROMAN_W			0x57	/*   87		*/
#   define	MAC_ROMAN_X			0x58	/*   88		*/
#   define	MAC_ROMAN_Y			0x59	/*   89		*/
#   define	MAC_ROMAN_Z			0x5a	/*   90		*/
#   define	MAC_ROMAN_bracketleft		0x5b	/*   91		*/
#   define	MAC_ROMAN_backslash		0x5c	/*   92		*/
#   define	MAC_ROMAN_bracketright		0x5d	/*   93		*/
#   define	MAC_ROMAN_asciicircum		0x5e	/*   94		*/
#   define	MAC_ROMAN_underscore		0x5f	/*   95		*/
#   define	MAC_ROMAN_grave			0x60	/*   96		*/
#   define	MAC_ROMAN_a			0x61	/*   97		*/
#   define	MAC_ROMAN_b			0x62	/*   98		*/
#   define	MAC_ROMAN_c			0x63	/*   99		*/
#   define	MAC_ROMAN_d			0x64	/*  100		*/
#   define	MAC_ROMAN_e			0x65	/*  101		*/
#   define	MAC_ROMAN_f			0x66	/*  102		*/
#   define	MAC_ROMAN_g			0x67	/*  103		*/
#   define	MAC_ROMAN_h			0x68	/*  104		*/
#   define	MAC_ROMAN_i			0x69	/*  105		*/
#   define	MAC_ROMAN_j			0x6a	/*  106		*/
#   define	MAC_ROMAN_k			0x6b	/*  107		*/
#   define	MAC_ROMAN_l			0x6c	/*  108		*/
#   define	MAC_ROMAN_m			0x6d	/*  109		*/
#   define	MAC_ROMAN_n			0x6e	/*  110		*/
#   define	MAC_ROMAN_o			0x6f	/*  111		*/
#   define	MAC_ROMAN_p			0x70	/*  112		*/
#   define	MAC_ROMAN_q			0x71	/*  113		*/
#   define	MAC_ROMAN_r			0x72	/*  114		*/
#   define	MAC_ROMAN_s			0x73	/*  115		*/
#   define	MAC_ROMAN_t			0x74	/*  116		*/
#   define	MAC_ROMAN_u			0x75	/*  117		*/
#   define	MAC_ROMAN_v			0x76	/*  118		*/
#   define	MAC_ROMAN_w			0x77	/*  119		*/
#   define	MAC_ROMAN_x			0x78	/*  120		*/
#   define	MAC_ROMAN_y			0x79	/*  121		*/
#   define	MAC_ROMAN_z			0x7a	/*  122		*/
#   define	MAC_ROMAN_braceleft		0x7b	/*  123		*/
#   define	MAC_ROMAN_bar			0x7c	/*  124		*/
#   define	MAC_ROMAN_braceright		0x7d	/*  125		*/
#   define	MAC_ROMAN_asciitilde		0x7e	/*  126		*/
#   define	MAC_ROMAN_Adieresis		0x80	/*  128		*/
#   define	MAC_ROMAN_Aring			0x81	/*  129		*/
#   define	MAC_ROMAN_Ccedilla		0x82	/*  130		*/
#   define	MAC_ROMAN_Eacute		0x83	/*  131		*/
#   define	MAC_ROMAN_Ntilde		0x84	/*  132		*/
#   define	MAC_ROMAN_Odieresis		0x85	/*  133		*/
#   define	MAC_ROMAN_Udieresis		0x86	/*  134		*/
#   define	MAC_ROMAN_aacute		0x87	/*  135		*/
#   define	MAC_ROMAN_agrave		0x88	/*  136		*/
#   define	MAC_ROMAN_acircumflex		0x89	/*  137		*/
#   define	MAC_ROMAN_adieresis		0x8a	/*  138		*/
#   define	MAC_ROMAN_atilde		0x8b	/*  139		*/
#   define	MAC_ROMAN_aring			0x8c	/*  140		*/
#   define	MAC_ROMAN_ccedilla		0x8d	/*  141		*/
#   define	MAC_ROMAN_eacute		0x8e	/*  142		*/
#   define	MAC_ROMAN_egrave		0x8f	/*  143		*/
#   define	MAC_ROMAN_ecircumflex		0x90	/*  144		*/
#   define	MAC_ROMAN_edieresis		0x91	/*  145		*/
#   define	MAC_ROMAN_iacute		0x92	/*  146		*/
#   define	MAC_ROMAN_igrave		0x93	/*  147		*/
#   define	MAC_ROMAN_icircumflex		0x94	/*  148		*/
#   define	MAC_ROMAN_idieresis		0x95	/*  149		*/
#   define	MAC_ROMAN_ntilde		0x96	/*  150		*/
#   define	MAC_ROMAN_oacute		0x97	/*  151		*/
#   define	MAC_ROMAN_ograve		0x98	/*  152		*/
#   define	MAC_ROMAN_ocircumflex		0x99	/*  153		*/
#   define	MAC_ROMAN_odieresis		0x9a	/*  154		*/
#   define	MAC_ROMAN_otilde		0x9b	/*  155		*/
#   define	MAC_ROMAN_uacute		0x9c	/*  156		*/
#   define	MAC_ROMAN_ugrave		0x9d	/*  157		*/
#   define	MAC_ROMAN_ucircumflex		0x9e	/*  158		*/
#   define	MAC_ROMAN_udieresis		0x9f	/*  159		*/
#   define	MAC_ROMAN_dagger		0xa0	/*  160		*/
#   define	MAC_ROMAN_degree		0xa1	/*  161		*/
#   define	MAC_ROMAN_cent			0xa2	/*  162		*/
#   define	MAC_ROMAN_sterling		0xa3	/*  163		*/
#   define	MAC_ROMAN_section		0xa4	/*  164		*/
#   define	MAC_ROMAN_bullet		0xa5	/*  165		*/
#   define	MAC_ROMAN_paragraph		0xa6	/*  166		*/
#   define	MAC_ROMAN_germandbls		0xa7	/*  167		*/
#   define	MAC_ROMAN_registered		0xa8	/*  168		*/
#   define	MAC_ROMAN_copyright		0xa9	/*  169		*/
#   define	MAC_ROMAN_trademark		0xaa	/*  170		*/
#   define	MAC_ROMAN_acute			0xab	/*  171		*/
#   define	MAC_ROMAN_dieresis		0xac	/*  172		*/
#   define	MAC_ROMAN_notequal		0xad	/*  173		*/
#   define	MAC_ROMAN_AE			0xae	/*  174		*/
#   define	MAC_ROMAN_Oslash		0xaf	/*  175		*/
#   define	MAC_ROMAN_infinity		0xb0	/*  176		*/
#   define	MAC_ROMAN_plusminus		0xb1	/*  177		*/
#   define	MAC_ROMAN_lessequal		0xb2	/*  178		*/
#   define	MAC_ROMAN_greaterequal		0xb3	/*  179		*/
#   define	MAC_ROMAN_yen			0xb4	/*  180		*/
#   define	MAC_ROMAN_mu			0xb5	/*  181		*/
#   define	MAC_ROMAN_partialdiff		0xb6	/*  182		*/
#   define	MAC_ROMAN_summation		0xb7	/*  183		*/
#   define	MAC_ROMAN_product		0xb8	/*  184		*/
#   define	MAC_ROMAN_pi			0xb9	/*  185		*/
#   define	MAC_ROMAN_integral		0xba	/*  186		*/
#   define	MAC_ROMAN_ordfeminine		0xbb	/*  187		*/
#   define	MAC_ROMAN_ordmasculine		0xbc	/*  188		*/
#   define	MAC_ROMAN__Omega		0xbd	/*  189		*/
#   define	MAC_ROMAN_ae			0xbe	/*  190		*/
#   define	MAC_ROMAN_oslash		0xbf	/*  191		*/
#   define	MAC_ROMAN_questiondown		0xc0	/*  192		*/
#   define	MAC_ROMAN_exclamdown		0xc1	/*  193		*/
#   define	MAC_ROMAN_logicalnot		0xc2	/*  194		*/
#   define	MAC_ROMAN_radical		0xc3	/*  195		*/
#   define	MAC_ROMAN_florin		0xc4	/*  196		*/
#   define	MAC_ROMAN_approxequal		0xc5	/*  197		*/
#   define	MAC_ROMAN_Delta			0xc6	/*  198		*/
#   define	MAC_ROMAN_guillemotleft		0xc7	/*  199		*/
#   define	MAC_ROMAN_guillemotright	0xc8	/*  200		*/
#   define	MAC_ROMAN_ellipsis		0xc9	/*  201		*/
#   define	MAC_ROMAN_nobreakspace		0xca	/*  202		*/
#   define	MAC_ROMAN_Agrave		0xcb	/*  203		*/
#   define	MAC_ROMAN_Atilde		0xcc	/*  204		*/
#   define	MAC_ROMAN_Otilde		0xcd	/*  205		*/
#   define	MAC_ROMAN_OE			0xce	/*  206		*/
#   define	MAC_ROMAN_oe			0xcf	/*  207		*/
#   define	MAC_ROMAN_endash		0xd0	/*  208		*/
#   define	MAC_ROMAN_emdash		0xd1	/*  209		*/
#   define	MAC_ROMAN_quotedblleft		0xd2	/*  210		*/
#   define	MAC_ROMAN_quotedblright		0xd3	/*  211		*/
#   define	MAC_ROMAN_quoteleft		0xd4	/*  212		*/
#   define	MAC_ROMAN_quoteright		0xd5	/*  213		*/
#   define	MAC_ROMAN_divide		0xd6	/*  214		*/
#   define	MAC_ROMAN_lozenge		0xd7	/*  215		*/
#   define	MAC_ROMAN_ydieresis		0xd8	/*  216		*/
#   define	MAC_ROMAN_Ydieresis		0xd9	/*  217		*/
#   define	MAC_ROMAN_fraction		0xda	/*  218		*/
#   define	MAC_ROMAN_Euro			0xdb	/*  219		*/
#   define	MAC_ROMAN_guilsinglleft		0xdc	/*  220		*/
#   define	MAC_ROMAN_guilsinglright	0xdd	/*  221		*/
#   define	MAC_ROMAN__fi			0xde	/*  222		*/
#   define	MAC_ROMAN__fl			0xdf	/*  223		*/
#   define	MAC_ROMAN_daggerdbl		0xe0	/*  224		*/
#   define	MAC_ROMAN_periodcentered	0xe1	/*  225		*/
#   define	MAC_ROMAN_quotesinglbase	0xe2	/*  226		*/
#   define	MAC_ROMAN_quotedblbase		0xe3	/*  227		*/
#   define	MAC_ROMAN_perthousand		0xe4	/*  228		*/
#   define	MAC_ROMAN_Acircumflex		0xe5	/*  229		*/
#   define	MAC_ROMAN_Ecircumflex		0xe6	/*  230		*/
#   define	MAC_ROMAN_Aacute		0xe7	/*  231		*/
#   define	MAC_ROMAN_Edieresis		0xe8	/*  232		*/
#   define	MAC_ROMAN_Egrave		0xe9	/*  233		*/
#   define	MAC_ROMAN_Iacute		0xea	/*  234		*/
#   define	MAC_ROMAN_Icircumflex		0xeb	/*  235		*/
#   define	MAC_ROMAN_Idieresis		0xec	/*  236		*/
#   define	MAC_ROMAN_Igrave		0xed	/*  237		*/
#   define	MAC_ROMAN_Oacute		0xee	/*  238		*/
#   define	MAC_ROMAN_Ocircumflex		0xef	/*  239		*/
#   define	MAC_ROMAN__apple		0xf0	/*  240		*/
#   define	MAC_ROMAN_Ograve		0xf1	/*  241		*/
#   define	MAC_ROMAN_Uacute		0xf2	/*  242		*/
#   define	MAC_ROMAN_Ucircumflex		0xf3	/*  243		*/
#   define	MAC_ROMAN_Ugrave		0xf4	/*  244		*/
#   define	MAC_ROMAN_dotlessi		0xf5	/*  245		*/
#   define	MAC_ROMAN_circumflex		0xf6	/*  246		*/
#   define	MAC_ROMAN_tilde			0xf7	/*  247		*/
#   define	MAC_ROMAN_macron		0xf8	/*  248		*/
#   define	MAC_ROMAN_breve			0xf9	/*  249		*/
#   define	MAC_ROMAN_dotaccent		0xfa	/*  250		*/
#   define	MAC_ROMAN_ring			0xfb	/*  251		*/
#   define	MAC_ROMAN_cedilla		0xfc	/*  252		*/
#   define	MAC_ROMAN_hungarumlaut		0xfd	/*  253		*/
#   define	MAC_ROMAN_ogonek		0xfe	/*  254		*/
#   define	MAC_ROMAN_caron			0xff	/*  255		*/

/************************************************************************/
/*									*/
/*  PostScript symbol encoding						*/
/*									*/
/************************************************************************/

#   define	PSSYM__UNDEF0			0
#   define	PSSYM__UNDEF1			1
#   define	PSSYM__UNDEF2			2
#   define	PSSYM__UNDEF3			3
#   define	PSSYM__UNDEF4			4
#   define	PSSYM__UNDEF5			5
#   define	PSSYM__UNDEF6			6
#   define	PSSYM__UNDEF7			7
#   define	PSSYM__UNDEF8			8
#   define	PSSYM__UNDEF9			9
#   define	PSSYM__UNDEF10			10
#   define	PSSYM__UNDEF11			11
#   define	PSSYM__UNDEF12			12
#   define	PSSYM__UNDEF13			13
#   define	PSSYM__UNDEF14			14
#   define	PSSYM__UNDEF15			15
#   define	PSSYM__UNDEF16			16
#   define	PSSYM__UNDEF17			17
#   define	PSSYM__UNDEF18			18
#   define	PSSYM__UNDEF19			19
#   define	PSSYM__UNDEF20			20
#   define	PSSYM__UNDEF21			21
#   define	PSSYM__UNDEF22			22
#   define	PSSYM__UNDEF23			23
#   define	PSSYM__UNDEF24			24
#   define	PSSYM__UNDEF25			25
#   define	PSSYM__UNDEF26			26
#   define	PSSYM__UNDEF27			27
#   define	PSSYM__UNDEF28			28
#   define	PSSYM__UNDEF29			29
#   define	PSSYM__UNDEF30			30
#   define	PSSYM__UNDEF31			31
#   define	PSSYM_space			32
#   define	PSSYM_exclam			33
#   define	PSSYM_universal			34
#   define	PSSYM_numbersign		35
#   define	PSSYM_existential		36
#   define	PSSYM_percent			37
#   define	PSSYM_ampersand			38
#   define	PSSYM_suchthat			39
#   define	PSSYM_parenleft			40
#   define	PSSYM_parenright		41
#   define	PSSYM_asteriskmath		42
#   define	PSSYM_plus			43
#   define	PSSYM_comma			44
#   define	PSSYM_minus			45
#   define	PSSYM_period			46
#   define	PSSYM_slash			47
#   define	PSSYM_zero			48
#   define	PSSYM_one			49
#   define	PSSYM_two			50
#   define	PSSYM_three			51
#   define	PSSYM_four			52
#   define	PSSYM_five			53
#   define	PSSYM_six			54
#   define	PSSYM_seven			55
#   define	PSSYM_eight			56
#   define	PSSYM_nine			57
#   define	PSSYM_colon			58
#   define	PSSYM_semicolon			59
#   define	PSSYM_less			60
#   define	PSSYM_equal			61
#   define	PSSYM_greater			62
#   define	PSSYM_question			63
#   define	PSSYM_congruent			64
#   define	PSSYM_Alpha			65
#   define	PSSYM_Beta			66
#   define	PSSYM_Chi			67
#   define	PSSYM_Delta			68
#   define	PSSYM_Epsilon			69
#   define	PSSYM_Phi			70
#   define	PSSYM_Gamma			71
#   define	PSSYM_Eta			72
#   define	PSSYM_Iota			73
#   define	PSSYM_theta1			74
#   define	PSSYM_Kappa			75
#   define	PSSYM_Lambda			76
#   define	PSSYM_Mu			77
#   define	PSSYM_Nu			78
#   define	PSSYM_Omicron			79
#   define	PSSYM_Pi			80
#   define	PSSYM_Theta			81
#   define	PSSYM_Rho			82
#   define	PSSYM_Sigma			83
#   define	PSSYM_Tau			84
#   define	PSSYM_Upsilon			85
#   define	PSSYM_sigma1			86
#   define	PSSYM_Omega			87
#   define	PSSYM_Xi			88
#   define	PSSYM_Psi			89
#   define	PSSYM_Zeta			90
#   define	PSSYM_bracketleft		91
#   define	PSSYM_therefore			92
#   define	PSSYM_bracketright		93
#   define	PSSYM_perpendicular		94
#   define	PSSYM_underscore		95
#   define	PSSYM_radicalex			96
#   define	PSSYM_alpha			97
#   define	PSSYM_beta			98
#   define	PSSYM_chi			99
#   define	PSSYM_delta			100
#   define	PSSYM_epsilon			101
#   define	PSSYM_phi			102
#   define	PSSYM_gamma			103
#   define	PSSYM_eta			104
#   define	PSSYM_iota			105
#   define	PSSYM_phi1			106
#   define	PSSYM_kappa			107
#   define	PSSYM_lambda			108
#   define	PSSYM_mu			109
#   define	PSSYM_nu			110
#   define	PSSYM_omicron			111
#   define	PSSYM_pi			112
#   define	PSSYM_theta			113
#   define	PSSYM_rho			114
#   define	PSSYM_sigma			115
#   define	PSSYM_tau			116
#   define	PSSYM_upsilon			117
#   define	PSSYM_omega1			118
#   define	PSSYM_omega			119
#   define	PSSYM_xi			120
#   define	PSSYM_psi			121
#   define	PSSYM_zeta			122
#   define	PSSYM_braceleft			123
#   define	PSSYM_bar			124
#   define	PSSYM_braceright		125
#   define	PSSYM_similar			126
#   define	PSSYM__UNDEF127			127
#   define	PSSYM__UNDEF128			128
#   define	PSSYM__UNDEF129			129
#   define	PSSYM__UNDEF130			130
#   define	PSSYM__UNDEF131			131
#   define	PSSYM__UNDEF132			132
#   define	PSSYM__UNDEF133			133
#   define	PSSYM__UNDEF134			134
#   define	PSSYM__UNDEF135			135
#   define	PSSYM__UNDEF136			136
#   define	PSSYM__UNDEF137			137
#   define	PSSYM__UNDEF138			138
#   define	PSSYM__UNDEF139			139
#   define	PSSYM__UNDEF140			140
#   define	PSSYM__UNDEF141			141
#   define	PSSYM__UNDEF142			142
#   define	PSSYM__UNDEF143			143
#   define	PSSYM__UNDEF144			144
#   define	PSSYM__UNDEF145			145
#   define	PSSYM__UNDEF146			146
#   define	PSSYM__UNDEF147			147
#   define	PSSYM__UNDEF148			148
#   define	PSSYM__UNDEF149			149
#   define	PSSYM__UNDEF150			150
#   define	PSSYM__UNDEF151			151
#   define	PSSYM__UNDEF152			152
#   define	PSSYM__UNDEF153			153
#   define	PSSYM__UNDEF154			154
#   define	PSSYM__UNDEF155			155
#   define	PSSYM__UNDEF156			156
#   define	PSSYM__UNDEF157			157
#   define	PSSYM__UNDEF158			158
#   define	PSSYM__UNDEF159			159
#   define	PSSYM__UNDEF160			160
#   define	PSSYM_Upsilon1			161
#   define	PSSYM_minute			162
#   define	PSSYM_lessequal			163
#   define	PSSYM_fraction			164
#   define	PSSYM_infinity			165
#   define	PSSYM_florin			166
#   define	PSSYM_club			167
#   define	PSSYM_diamond			168
#   define	PSSYM_heart			169
#   define	PSSYM_spade			170
#   define	PSSYM_arrowboth			171
#   define	PSSYM_arrowleft			172
#   define	PSSYM_arrowup			173
#   define	PSSYM_arrowright		174
#   define	PSSYM_arrowdown			175
#   define	PSSYM_degree			176
#   define	PSSYM_plusminus			177
#   define	PSSYM_second			178
#   define	PSSYM_greaterequal		179
#   define	PSSYM_multiply			180
#   define	PSSYM_proportional		181
#   define	PSSYM_partialdiff		182
#   define	PSSYM_bullet			183
#   define	PSSYM_divide			184
#   define	PSSYM_notequal			185
#   define	PSSYM_equivalence		186
#   define	PSSYM_approxequal		187
#   define	PSSYM_ellipsis			188
#   define	PSSYM_arrowvertex		189
#   define	PSSYM_arrowhorizex		190
#   define	PSSYM_carriagereturn		191
#   define	PSSYM_aleph			192
#   define	PSSYM_Ifraktur			193
#   define	PSSYM_Rfraktur			194
#   define	PSSYM_weierstrass		195
#   define	PSSYM_circlemultiply		196
#   define	PSSYM_circleplus		197
#   define	PSSYM_emptyset			198
#   define	PSSYM_intersection		199
#   define	PSSYM_union			200
#   define	PSSYM_propersuperset		201
#   define	PSSYM_reflexsuperset		202
#   define	PSSYM_notsubset			203
#   define	PSSYM_propersubset		204
#   define	PSSYM_reflexsubset		205
#   define	PSSYM_element			206
#   define	PSSYM_notelement		207
#   define	PSSYM_angle			208
#   define	PSSYM_gradient			209
#   define	PSSYM_registerserif		210
#   define	PSSYM_copyrightserif		211
#   define	PSSYM_trademarkserif		212
#   define	PSSYM_product			213
#   define	PSSYM_radical			214
#   define	PSSYM_dotmath			215
#   define	PSSYM_logicalnot		216
#   define	PSSYM_logicaland		217
#   define	PSSYM_logicalor			218
#   define	PSSYM_arrowdblboth		219
#   define	PSSYM_arrowdblleft		220
#   define	PSSYM_arrowdblup		221
#   define	PSSYM_arrowdblright		222
#   define	PSSYM_arrowdbldown		223
#   define	PSSYM_lozenge			224
#   define	PSSYM_angleleft			225
#   define	PSSYM_registersans		226
#   define	PSSYM_copyrightsans		227
#   define	PSSYM_trademarksans		228
#   define	PSSYM_summation			229
#   define	PSSYM_parenlefttp		230
#   define	PSSYM_parenleftex		231
#   define	PSSYM_parenleftbt		232
#   define	PSSYM_bracketlefttp		233
#   define	PSSYM_bracketleftex		234
#   define	PSSYM_bracketleftbt		235
#   define	PSSYM_bracelefttp		236
#   define	PSSYM_braceleftmid		237
#   define	PSSYM_braceleftbt		238
#   define	PSSYM_braceex			239
#   define	PSSYM__UNDEF240			240
#   define	PSSYM_angleright		241
#   define	PSSYM_integral			242
#   define	PSSYM_integraltp		243
#   define	PSSYM_integralex		244
#   define	PSSYM_integralbt		245
#   define	PSSYM_parenrighttp		246
#   define	PSSYM_parenrightex		247
#   define	PSSYM_parenrightbt		248
#   define	PSSYM_bracketrighttp		249
#   define	PSSYM_bracketrightex		250
#   define	PSSYM_bracketrightbt		251
#   define	PSSYM_bracerighttp		252
#   define	PSSYM_bracerightmid		253
#   define	PSSYM_bracerightbt		254
#   define	PSSYM__UNDEF255			255

#   define	PSSYM_isalpha(c) \
			(PSSYM_isupper((c))||PSSYM_islower((c)))

#   define	PSSYM_isdigit(c) \
			(ASCII_isdigit((c)))

#   define	PSSYM_isalnum(c) \
			(PSSYM_isupper((c))||PSSYM_islower((c))||\
			PSSYM_isdigit((c)))

#   define	PSSYM_isupper(c) \
			  (((c)>=PSSYM_Alpha&&(c)<=PSSYM_Iota ) || \
			   ((c)>=PSSYM_Kappa&&(c)<=PSSYM_Upsilon ) || \
			   ((c)>=PSSYM_Omega&&(c)<=PSSYM_Zeta) || \
			   ((c)==PSSYM_Upsilon1) )

#   define	PSSYM_islower(c) \
			  (((c)>=PSSYM_alpha&&(c)<=PSSYM_zeta) || \
			   ((c)==PSSYM_theta1) || \
			   ((c)==PSSYM_sigma1) )

#   define	PSSYM_tolower(c) ( \
		      ((c)>=PSSYM_Alpha&&(c)<=PSSYM_Iota )? ((c)+0x30): (\
		      ((c)>=PSSYM_Kappa&&(c)<=PSSYM_Upsilon )? ((c)+0x30): (\
		      ((c)>=PSSYM_Omega&&(c)<=PSSYM_Zeta )? ((c)+0x30): (\
		      ((c)==PSSYM_Upsilon1)? PSSYM_upsilon: (\
		       (c) )))))

#   define	PSSYM_toupper(c) ( \
		      ((c)>=PSSYM_alpha&&(c)<=PSSYM_iota )? ((c)-0x30): (\
		      ((c)>=PSSYM_kappa&&(c)<=PSSYM_upsilon )? ((c)-0x30): (\
		      ((c)>=PSSYM_omega&&(c)<=PSSYM_zeta )? ((c)-0x30): (\
		      ((c)==PSSYM_theta1)? PSSYM_Theta: (\
		      ((c)==PSSYM_sigma1)? PSSYM_Sigma: (\
		      ((c)==PSSYM_phi1)? PSSYM_Phi: (\
		      ((c)==PSSYM_omega1)? PSSYM_Omega: (\
		       (c) ))))))))

/************************************************************************/
/*									*/
/*  Names for the characters in the Adobe Cyrillic Encoding.		*/
/*									*/
/************************************************************************/

#   define	PSCYR__0x00		0x00	/*   0  */
#   define	PSCYR__0x01		0x01	/*   1  */
#   define	PSCYR__0x02		0x02	/*   2  */
#   define	PSCYR__0x03		0x03	/*   3  */
#   define	PSCYR__0x04		0x04	/*   4  */
#   define	PSCYR__0x05		0x05	/*   5  */
#   define	PSCYR__0x06		0x06	/*   6  */
#   define	PSCYR__0x07		0x07	/*   7  */
#   define	PSCYR__0x08		0x08	/*   8  */
#   define	PSCYR__0x09		0x09	/*   9  */
#   define	PSCYR__0x0a		0x0a	/*  10  */
#   define	PSCYR__0x0b		0x0b	/*  11  */
#   define	PSCYR__0x0c		0x0c	/*  12  */
#   define	PSCYR__0x0d		0x0d	/*  13  */
#   define	PSCYR__0x0e		0x0e	/*  14  */
#   define	PSCYR__0x0f		0x0f	/*  15  */
#   define	PSCYR__0x10		0x10	/*  16  */
#   define	PSCYR__0x11		0x11	/*  17  */
#   define	PSCYR__0x12		0x12	/*  18  */
#   define	PSCYR__0x13		0x13	/*  19  */
#   define	PSCYR__0x14		0x14	/*  20  */
#   define	PSCYR__0x15		0x15	/*  21  */
#   define	PSCYR__0x16		0x16	/*  22  */
#   define	PSCYR__0x17		0x17	/*  23  */
#   define	PSCYR__0x18		0x18	/*  24  */
#   define	PSCYR__0x19		0x19	/*  25  */
#   define	PSCYR__0x1a		0x1a	/*  26  */
#   define	PSCYR__0x1b		0x1b	/*  27  */
#   define	PSCYR__0x1c		0x1c	/*  28  */
#   define	PSCYR__0x1d		0x1d	/*  29  */
#   define	PSCYR__0x1e		0x1e	/*  30  */
#   define	PSCYR__0x1f		0x1f	/*  31  */
#   define	PSCYR_space		0x20	/*  32  */
#   define	PSCYR_exclam		0x21	/*  33  */
#   define	PSCYR_quotedbl		0x22	/*  34  */
#   define	PSCYR_numbersign	0x23	/*  35  */
#   define	PSCYR_dollar		0x24	/*  36  */
#   define	PSCYR_percent		0x25	/*  37  */
#   define	PSCYR_ampersand		0x26	/*  38  */
#   define	PSCYR_quotesingle	0x27	/*  39  */
#   define	PSCYR_parenleft		0x28	/*  40  */
#   define	PSCYR_parenright	0x29	/*  41  */
#   define	PSCYR_asterisk		0x2a	/*  42  */
#   define	PSCYR_plus		0x2b	/*  43  */
#   define	PSCYR_comma		0x2c	/*  44  */
#   define	PSCYR_minus		0x2d	/*  45  */
#   define	PSCYR_period		0x2e	/*  46  */
#   define	PSCYR_slash		0x2f	/*  47  */
#   define	PSCYR_zero		0x30	/*  48  */
#   define	PSCYR_one		0x31	/*  49  */
#   define	PSCYR_two		0x32	/*  50  */
#   define	PSCYR_three		0x33	/*  51  */
#   define	PSCYR_four		0x34	/*  52  */
#   define	PSCYR_five		0x35	/*  53  */
#   define	PSCYR_six		0x36	/*  54  */
#   define	PSCYR_seven		0x37	/*  55  */
#   define	PSCYR_eight		0x38	/*  56  */
#   define	PSCYR_nine		0x39	/*  57  */
#   define	PSCYR_colon		0x3a	/*  58  */
#   define	PSCYR_semicolon		0x3b	/*  59  */
#   define	PSCYR_less		0x3c	/*  60  */
#   define	PSCYR_equal		0x3d	/*  61  */
#   define	PSCYR_greater		0x3e	/*  62  */
#   define	PSCYR_question		0x3f	/*  63  */
#   define	PSCYR_at		0x40	/*  64  */
#   define	PSCYR_A			0x41	/*  65  */
#   define	PSCYR_B			0x42	/*  66  */
#   define	PSCYR_C			0x43	/*  67  */
#   define	PSCYR_D			0x44	/*  68  */
#   define	PSCYR_E			0x45	/*  69  */
#   define	PSCYR_F			0x46	/*  70  */
#   define	PSCYR_G			0x47	/*  71  */
#   define	PSCYR_H			0x48	/*  72  */
#   define	PSCYR_I			0x49	/*  73  */
#   define	PSCYR_J			0x4a	/*  74  */
#   define	PSCYR_K			0x4b	/*  75  */
#   define	PSCYR_L			0x4c	/*  76  */
#   define	PSCYR_M			0x4d	/*  77  */
#   define	PSCYR_N			0x4e	/*  78  */
#   define	PSCYR_O			0x4f	/*  79  */
#   define	PSCYR_P			0x50	/*  80  */
#   define	PSCYR_Q			0x51	/*  81  */
#   define	PSCYR_R			0x52	/*  82  */
#   define	PSCYR_S			0x53	/*  83  */
#   define	PSCYR_T			0x54	/*  84  */
#   define	PSCYR_U			0x55	/*  85  */
#   define	PSCYR_V			0x56	/*  86  */
#   define	PSCYR_W			0x57	/*  87  */
#   define	PSCYR_X			0x58	/*  88  */
#   define	PSCYR_Y			0x59	/*  89  */
#   define	PSCYR_Z			0x5a	/*  90  */
#   define	PSCYR_bracketleft	0x5b	/*  91  */
#   define	PSCYR_backslash		0x5c	/*  92  */
#   define	PSCYR_bracketright	0x5d	/*  93  */
#   define	PSCYR_asciicircum	0x5e	/*  94  */
#   define	PSCYR_underscore	0x5f	/*  95  */
#   define	PSCYR_quoteleft		0x60	/*  96  */
#   define	PSCYR_a			0x61	/*  97  */
#   define	PSCYR_b			0x62	/*  98  */
#   define	PSCYR_c			0x63	/*  99  */
#   define	PSCYR_d			0x64	/* 100  */
#   define	PSCYR_e			0x65	/* 101  */
#   define	PSCYR_f			0x66	/* 102  */
#   define	PSCYR_g			0x67	/* 103  */
#   define	PSCYR_h			0x68	/* 104  */
#   define	PSCYR_i			0x69	/* 105  */
#   define	PSCYR_j			0x6a	/* 106  */
#   define	PSCYR_k			0x6b	/* 107  */
#   define	PSCYR_l			0x6c	/* 108  */
#   define	PSCYR_m			0x6d	/* 109  */
#   define	PSCYR_n			0x6e	/* 110  */
#   define	PSCYR_o			0x6f	/* 111  */
#   define	PSCYR_p			0x70	/* 112  */
#   define	PSCYR_q			0x71	/* 113  */
#   define	PSCYR_r			0x72	/* 114  */
#   define	PSCYR_s			0x73	/* 115  */
#   define	PSCYR_t			0x74	/* 116  */
#   define	PSCYR_u			0x75	/* 117  */
#   define	PSCYR_v			0x76	/* 118  */
#   define	PSCYR_w			0x77	/* 119  */
#   define	PSCYR_x			0x78	/* 120  */
#   define	PSCYR_y			0x79	/* 121  */
#   define	PSCYR_z			0x7a	/* 122  */
#   define	PSCYR_braceleft		0x7b	/* 123  */
#   define	PSCYR_bar		0x7c	/* 124  */
#   define	PSCYR_braceright	0x7d	/* 125  */
#   define	PSCYR_asciitilde	0x7e	/* 126  */
#   define	PSCYR__0x7f		0x7f	/* 127  */
#   define	PSCYR_afii10051		0x80	/* 128  */
#   define	PSCYR_afii10052		0x81	/* 129  */
#   define	PSCYR_quotesinglbase	0x82	/* 130  */
#   define	PSCYR_afii10100		0x83	/* 131  */
#   define	PSCYR_quotedblbase	0x84	/* 132  */
#   define	PSCYR_ellipsis		0x85	/* 133  */
#   define	PSCYR_dagger		0x86	/* 134  */
#   define	PSCYR_daggerdbl		0x87	/* 135  */
#   define	PSCYR__0x88		0x88	/* 136  */
#   define	PSCYR_perthousand	0x89	/* 137  */
#   define	PSCYR_afii10058		0x8a	/* 138  */
#   define	PSCYR_guilsinglleft	0x8b	/* 139  */
#   define	PSCYR_afii10059		0x8c	/* 140  */
#   define	PSCYR_afii10061		0x8d	/* 141  */
#   define	PSCYR_afii10060		0x8e	/* 142  */
#   define	PSCYR_afii10145		0x8f	/* 143  */
#   define	PSCYR_afii10099		0x90	/* 144  */
#   define	PSCYR_grave		0x91	/* 145  */
#   define	PSCYR_quoteright	0x92	/* 146  */
#   define	PSCYR_quotedblleft	0x93	/* 147  */
#   define	PSCYR_quotedblright	0x94	/* 148  */
#   define	PSCYR_bullet		0x95	/* 149  */
#   define	PSCYR_endash		0x96	/* 150  */
#   define	PSCYR_emdash		0x97	/* 151  */
#   define	PSCYR__0x98		0x98	/* 152  */
#   define	PSCYR_trademark		0x99	/* 153  */
#   define	PSCYR_afii10106		0x9a	/* 154  */
#   define	PSCYR_guilsinglright	0x9b	/* 155  */
#   define	PSCYR_afii10107		0x9c	/* 156  */
#   define	PSCYR_afii10109		0x9d	/* 157  */
#   define	PSCYR_afii10108		0x9e	/* 158  */
#   define	PSCYR_afii10193		0x9f	/* 159  */
#   define	PSCYR__space_0xa0	0xa0	/* 160  */
#   define	PSCYR_afii10062		0xa1	/* 161  */
#   define	PSCYR_afii10110		0xa2	/* 162  */
#   define	PSCYR_afii10057		0xa3	/* 163  */
#   define	PSCYR_currency		0xa4	/* 164  */
#   define	PSCYR_afii10050		0xa5	/* 165  */
#   define	PSCYR_brokenbar		0xa6	/* 166  */
#   define	PSCYR_section		0xa7	/* 167  */
#   define	PSCYR_afii10023		0xa8	/* 168  */
#   define	PSCYR_copyright		0xa9	/* 169  */
#   define	PSCYR_afii10053		0xaa	/* 170  */
#   define	PSCYR_guillemotleft	0xab	/* 171  */
#   define	PSCYR_logicalnot	0xac	/* 172  */
#   define	PSCYR__hyphen_0xad	0xad	/* 173  */
#   define	PSCYR_registered	0xae	/* 174  */
#   define	PSCYR_afii10056		0xaf	/* 175  */
#   define	PSCYR_degree		0xb0	/* 176  */
#   define	PSCYR_plusminus		0xb1	/* 177  */
#   define	PSCYR_afii10055		0xb2	/* 178  */
#   define	PSCYR_afii10103		0xb3	/* 179  */
#   define	PSCYR_afii10098		0xb4	/* 180  */
#   define	PSCYR_mu		0xb5	/* 181  */
#   define	PSCYR_paragraph		0xb6	/* 182  */
#   define	PSCYR_periodcentered	0xb7	/* 183  */
#   define	PSCYR_afii10071		0xb8	/* 184  */
#   define	PSCYR_afii61352		0xb9	/* 185  */
#   define	PSCYR_afii10101		0xba	/* 186  */
#   define	PSCYR_guillemotright	0xbb	/* 187  */
#   define	PSCYR_afii10105		0xbc	/* 188  */
#   define	PSCYR_afii10054		0xbd	/* 189  */
#   define	PSCYR_afii10102		0xbe	/* 190  */
#   define	PSCYR_afii10104		0xbf	/* 191  */
#   define	PSCYR_afii10017		0xc0	/* 192  */
#   define	PSCYR_afii10018		0xc1	/* 193  */
#   define	PSCYR_afii10019		0xc2	/* 194  */
#   define	PSCYR_afii10020		0xc3	/* 195  */
#   define	PSCYR_afii10021		0xc4	/* 196  */
#   define	PSCYR_afii10022		0xc5	/* 197  */
#   define	PSCYR_afii10024		0xc6	/* 198  */
#   define	PSCYR_afii10025		0xc7	/* 199  */
#   define	PSCYR_afii10026		0xc8	/* 200  */
#   define	PSCYR_afii10027		0xc9	/* 201  */
#   define	PSCYR_afii10028		0xca	/* 202  */
#   define	PSCYR_afii10029		0xcb	/* 203  */
#   define	PSCYR_afii10030		0xcc	/* 204  */
#   define	PSCYR_afii10031		0xcd	/* 205  */
#   define	PSCYR_afii10032		0xce	/* 206  */
#   define	PSCYR_afii10033		0xcf	/* 207  */
#   define	PSCYR_afii10034		0xd0	/* 208  */
#   define	PSCYR_afii10035		0xd1	/* 209  */
#   define	PSCYR_afii10036		0xd2	/* 210  */
#   define	PSCYR_afii10037		0xd3	/* 211  */
#   define	PSCYR_afii10038		0xd4	/* 212  */
#   define	PSCYR_afii10039		0xd5	/* 213  */
#   define	PSCYR_afii10040		0xd6	/* 214  */
#   define	PSCYR_afii10041		0xd7	/* 215  */
#   define	PSCYR_afii10042		0xd8	/* 216  */
#   define	PSCYR_afii10043		0xd9	/* 217  */
#   define	PSCYR_afii10044		0xda	/* 218  */
#   define	PSCYR_afii10045		0xdb	/* 219  */
#   define	PSCYR_afii10046		0xdc	/* 220  */
#   define	PSCYR_afii10047		0xdd	/* 221  */
#   define	PSCYR_afii10048		0xde	/* 222  */
#   define	PSCYR_afii10049		0xdf	/* 223  */
#   define	PSCYR_afii10065		0xe0	/* 224  */
#   define	PSCYR_afii10066		0xe1	/* 225  */
#   define	PSCYR_afii10067		0xe2	/* 226  */
#   define	PSCYR_afii10068		0xe3	/* 227  */
#   define	PSCYR_afii10069		0xe4	/* 228  */
#   define	PSCYR_afii10070		0xe5	/* 229  */
#   define	PSCYR_afii10072		0xe6	/* 230  */
#   define	PSCYR_afii10073		0xe7	/* 231  */
#   define	PSCYR_afii10074		0xe8	/* 232  */
#   define	PSCYR_afii10075		0xe9	/* 233  */
#   define	PSCYR_afii10076		0xea	/* 234  */
#   define	PSCYR_afii10077		0xeb	/* 235  */
#   define	PSCYR_afii10078		0xec	/* 236  */
#   define	PSCYR_afii10079		0xed	/* 237  */
#   define	PSCYR_afii10080		0xee	/* 238  */
#   define	PSCYR_afii10081		0xef	/* 239  */
#   define	PSCYR_afii10082		0xf0	/* 240  */
#   define	PSCYR_afii10083		0xf1	/* 241  */
#   define	PSCYR_afii10084		0xf2	/* 242  */
#   define	PSCYR_afii10085		0xf3	/* 243  */
#   define	PSCYR_afii10086		0xf4	/* 244  */
#   define	PSCYR_afii10087		0xf5	/* 245  */
#   define	PSCYR_afii10088		0xf6	/* 246  */
#   define	PSCYR_afii10089		0xf7	/* 247  */
#   define	PSCYR_afii10090		0xf8	/* 248  */
#   define	PSCYR_afii10091		0xf9	/* 249  */
#   define	PSCYR_afii10092		0xfa	/* 250  */
#   define	PSCYR_afii10093		0xfb	/* 251  */
#   define	PSCYR_afii10094		0xfc	/* 252  */
#   define	PSCYR_afii10095		0xfd	/* 253  */
#   define	PSCYR_afii10096		0xfe	/* 254  */
#   define	PSCYR_afii10097		0xff	/* 255  */

/************************************************************************/
/*									*/
/*  Names for the characters in the ISO 8859.5 (Cyrillic) Encoding.	*/
/*									*/
/************************************************************************/

#   define	ISO5_space		0x20	/*   32		*/
#   define	ISO5_exclam		0x21	/*   33		*/
#   define	ISO5_quotedbl		0x22	/*   34		*/
#   define	ISO5_numbersign		0x23	/*   35		*/
#   define	ISO5_dollar		0x24	/*   36		*/
#   define	ISO5_percent		0x25	/*   37		*/
#   define	ISO5_ampersand		0x26	/*   38		*/
#   define	ISO5_quotesingle	0x27	/*   39		*/
#   define	ISO5_parenleft		0x28	/*   40		*/
#   define	ISO5_parenright		0x29	/*   41		*/
#   define	ISO5_asterisk		0x2a	/*   42		*/
#   define	ISO5_plus		0x2b	/*   43		*/
#   define	ISO5_comma		0x2c	/*   44		*/
#   define	ISO5_minus		0x2d	/*   45		*/
#   define	ISO5_period		0x2e	/*   46		*/
#   define	ISO5_slash		0x2f	/*   47		*/
#   define	ISO5_zero		0x30	/*   48		*/
#   define	ISO5_one		0x31	/*   49		*/
#   define	ISO5_two		0x32	/*   50		*/
#   define	ISO5_three		0x33	/*   51		*/
#   define	ISO5_four		0x34	/*   52		*/
#   define	ISO5_five		0x35	/*   53		*/
#   define	ISO5_six		0x36	/*   54		*/
#   define	ISO5_seven		0x37	/*   55		*/
#   define	ISO5_eight		0x38	/*   56		*/
#   define	ISO5_nine		0x39	/*   57		*/
#   define	ISO5_colon		0x3a	/*   58		*/
#   define	ISO5_semicolon		0x3b	/*   59		*/
#   define	ISO5_less		0x3c	/*   60		*/
#   define	ISO5_equal		0x3d	/*   61		*/
#   define	ISO5_greater		0x3e	/*   62		*/
#   define	ISO5_question		0x3f	/*   63		*/
#   define	ISO5_at			0x40	/*   64		*/
#   define	ISO5_A			0x41	/*   65		*/
#   define	ISO5_B			0x42	/*   66		*/
#   define	ISO5_C			0x43	/*   67		*/
#   define	ISO5_D			0x44	/*   68		*/
#   define	ISO5_E			0x45	/*   69		*/
#   define	ISO5_F			0x46	/*   70		*/
#   define	ISO5_G			0x47	/*   71		*/
#   define	ISO5_H			0x48	/*   72		*/
#   define	ISO5_I			0x49	/*   73		*/
#   define	ISO5_J			0x4a	/*   74		*/
#   define	ISO5_K			0x4b	/*   75		*/
#   define	ISO5_L			0x4c	/*   76		*/
#   define	ISO5_M			0x4d	/*   77		*/
#   define	ISO5_N			0x4e	/*   78		*/
#   define	ISO5_O			0x4f	/*   79		*/
#   define	ISO5_P			0x50	/*   80		*/
#   define	ISO5_Q			0x51	/*   81		*/
#   define	ISO5_R			0x52	/*   82		*/
#   define	ISO5_S			0x53	/*   83		*/
#   define	ISO5_T			0x54	/*   84		*/
#   define	ISO5_U			0x55	/*   85		*/
#   define	ISO5_V			0x56	/*   86		*/
#   define	ISO5_W			0x57	/*   87		*/
#   define	ISO5_X			0x58	/*   88		*/
#   define	ISO5_Y			0x59	/*   89		*/
#   define	ISO5_Z			0x5a	/*   90		*/
#   define	ISO5_bracketleft	0x5b	/*   91		*/
#   define	ISO5_backslash		0x5c	/*   92		*/
#   define	ISO5_bracketright	0x5d	/*   93		*/
#   define	ISO5_asciicircum	0x5e	/*   94		*/
#   define	ISO5_underscore		0x5f	/*   95		*/
#   define	ISO5_quoteleft		0x60	/*   96		*/
#   define	ISO5_a			0x61	/*   97		*/
#   define	ISO5_b			0x62	/*   98		*/
#   define	ISO5_c			0x63	/*   99		*/
#   define	ISO5_d			0x64	/*  100		*/
#   define	ISO5_e			0x65	/*  101		*/
#   define	ISO5_f			0x66	/*  102		*/
#   define	ISO5_g			0x67	/*  103		*/
#   define	ISO5_h			0x68	/*  104		*/
#   define	ISO5_i			0x69	/*  105		*/
#   define	ISO5_j			0x6a	/*  106		*/
#   define	ISO5_k			0x6b	/*  107		*/
#   define	ISO5_l			0x6c	/*  108		*/
#   define	ISO5_m			0x6d	/*  109		*/
#   define	ISO5_n			0x6e	/*  110		*/
#   define	ISO5_o			0x6f	/*  111		*/
#   define	ISO5_p			0x70	/*  112		*/
#   define	ISO5_q			0x71	/*  113		*/
#   define	ISO5_r			0x72	/*  114		*/
#   define	ISO5_s			0x73	/*  115		*/
#   define	ISO5_t			0x74	/*  116		*/
#   define	ISO5_u			0x75	/*  117		*/
#   define	ISO5_v			0x76	/*  118		*/
#   define	ISO5_w			0x77	/*  119		*/
#   define	ISO5_x			0x78	/*  120		*/
#   define	ISO5_y			0x79	/*  121		*/
#   define	ISO5_z			0x7a	/*  122		*/
#   define	ISO5_braceleft		0x7b	/*  123		*/
#   define	ISO5_bar		0x7c	/*  124		*/
#   define	ISO5_braceright		0x7d	/*  125		*/
#   define	ISO5_asciitilde		0x7e	/*  126		*/
#   define	ISO5_nobreakspace	0xa0	/*  160		*/
#   define	ISO5_afii10023		0xa1	/*  161		*/
#   define	ISO5_afii10051		0xa2	/*  162		*/
#   define	ISO5_afii10052		0xa3	/*  163		*/
#   define	ISO5_afii10053		0xa4	/*  164		*/
#   define	ISO5_afii10054		0xa5	/*  165		*/
#   define	ISO5_afii10055		0xa6	/*  166		*/
#   define	ISO5_afii10056		0xa7	/*  167		*/
#   define	ISO5_afii10057		0xa8	/*  168		*/
#   define	ISO5_afii10058		0xa9	/*  169		*/
#   define	ISO5_afii10059		0xaa	/*  170		*/
#   define	ISO5_afii10060		0xab	/*  171		*/
#   define	ISO5_afii10061		0xac	/*  172		*/
#   define	ISO5_hyphen		0xad	/*  173		*/
#   define	ISO5_afii10062		0xae	/*  174		*/
#   define	ISO5_afii10145		0xaf	/*  175		*/
#   define	ISO5_afii10017		0xb0	/*  176		*/
#   define	ISO5_afii10018		0xb1	/*  177		*/
#   define	ISO5_afii10019		0xb2	/*  178		*/
#   define	ISO5_afii10020		0xb3	/*  179		*/
#   define	ISO5_afii10021		0xb4	/*  180		*/
#   define	ISO5_afii10022		0xb5	/*  181		*/
#   define	ISO5_afii10024		0xb6	/*  182		*/
#   define	ISO5_afii10025		0xb7	/*  183		*/
#   define	ISO5_afii10026		0xb8	/*  184		*/
#   define	ISO5_afii10027		0xb9	/*  185		*/
#   define	ISO5_afii10028		0xba	/*  186		*/
#   define	ISO5_afii10029		0xbb	/*  187		*/
#   define	ISO5_afii10030		0xbc	/*  188		*/
#   define	ISO5_afii10031		0xbd	/*  189		*/
#   define	ISO5_afii10032		0xbe	/*  190		*/
#   define	ISO5_afii10033		0xbf	/*  191		*/
#   define	ISO5_afii10034		0xc0	/*  192		*/
#   define	ISO5_afii10035		0xc1	/*  193		*/
#   define	ISO5_afii10036		0xc2	/*  194		*/
#   define	ISO5_afii10037		0xc3	/*  195		*/
#   define	ISO5_afii10038		0xc4	/*  196		*/
#   define	ISO5_afii10039		0xc5	/*  197		*/
#   define	ISO5_afii10040		0xc6	/*  198		*/
#   define	ISO5_afii10041		0xc7	/*  199		*/
#   define	ISO5_afii10042		0xc8	/*  200		*/
#   define	ISO5_afii10043		0xc9	/*  201		*/
#   define	ISO5_afii10044		0xca	/*  202		*/
#   define	ISO5_afii10045		0xcb	/*  203		*/
#   define	ISO5_afii10046		0xcc	/*  204		*/
#   define	ISO5_afii10047		0xcd	/*  205		*/
#   define	ISO5_afii10048		0xce	/*  206		*/
#   define	ISO5_afii10049		0xcf	/*  207		*/
#   define	ISO5_afii10065		0xd0	/*  208		*/
#   define	ISO5_afii10066		0xd1	/*  209		*/
#   define	ISO5_afii10067		0xd2	/*  210		*/
#   define	ISO5_afii10068		0xd3	/*  211		*/
#   define	ISO5_afii10069		0xd4	/*  212		*/
#   define	ISO5_afii10070		0xd5	/*  213		*/
#   define	ISO5_afii10072		0xd6	/*  214		*/
#   define	ISO5_afii10073		0xd7	/*  215		*/
#   define	ISO5_afii10074		0xd8	/*  216		*/
#   define	ISO5_afii10075		0xd9	/*  217		*/
#   define	ISO5_afii10076		0xda	/*  218		*/
#   define	ISO5_afii10077		0xdb	/*  219		*/
#   define	ISO5_afii10078		0xdc	/*  220		*/
#   define	ISO5_afii10079		0xdd	/*  221		*/
#   define	ISO5_afii10080		0xde	/*  222		*/
#   define	ISO5_afii10081		0xdf	/*  223		*/
#   define	ISO5_afii10082		0xe0	/*  224		*/
#   define	ISO5_afii10083		0xe1	/*  225		*/
#   define	ISO5_afii10084		0xe2	/*  226		*/
#   define	ISO5_afii10085		0xe3	/*  227		*/
#   define	ISO5_afii10086		0xe4	/*  228		*/
#   define	ISO5_afii10087		0xe5	/*  229		*/
#   define	ISO5_afii10088		0xe6	/*  230		*/
#   define	ISO5_afii10089		0xe7	/*  231		*/
#   define	ISO5_afii10090		0xe8	/*  232		*/
#   define	ISO5_afii10091		0xe9	/*  233		*/
#   define	ISO5_afii10092		0xea	/*  234		*/
#   define	ISO5_afii10093		0xeb	/*  235		*/
#   define	ISO5_afii10094		0xec	/*  236		*/
#   define	ISO5_afii10095		0xed	/*  237		*/
#   define	ISO5_afii10096		0xee	/*  238		*/
#   define	ISO5_afii10097		0xef	/*  239		*/
#   define	ISO5_afii61352		0xf0	/*  240		*/
#   define	ISO5_afii10071		0xf1	/*  241		*/
#   define	ISO5_afii10099		0xf2	/*  242		*/
#   define	ISO5_afii10100		0xf3	/*  243		*/
#   define	ISO5_afii10101		0xf4	/*  244		*/
#   define	ISO5_afii10102		0xf5	/*  245		*/
#   define	ISO5_afii10103		0xf6	/*  246		*/
#   define	ISO5_afii10104		0xf7	/*  247		*/
#   define	ISO5_afii10105		0xf8	/*  248		*/
#   define	ISO5_afii10106		0xf9	/*  249		*/
#   define	ISO5_afii10107		0xfa	/*  250		*/
#   define	ISO5_afii10108		0xfb	/*  251		*/
#   define	ISO5_afii10109		0xfc	/*  252		*/
#   define	ISO5_section		0xfd	/*  253		*/
#   define	ISO5_afii10110		0xfe	/*  254		*/
#   define	ISO5_afii10193		0xff	/*  255		*/

#   define	ISO5_isupper(c)	((ASCII_isupper((c)))||\
			      ((c)>=ISO5_afii10023&&(c)<=ISO5_afii10061)||\
			      ((c)>=ISO5_afii10062&&(c)<=ISO5_afii10145)||\
			      ((c)>=ISO5_afii10062&&(c)<=ISO5_afii10049))

#   define	ISO5_islower(c)	((ASCII_islower((c)))||\
			      ((c)>=ISO5_afii10065&&(c)<=ISO5_afii10097)||\
			      ((c)>=ISO5_afii10071&&(c)<=ISO5_afii10109)||\
			      ((c)>=ISO5_afii10110&&(c)<=ISO5_afii10193))

#   define	ISO5_isalpha(c)	(ISO5_isupper((c))||ISO5_islower((c)))
#   define	ISO5_isdigit(c)	(ASCII_isdigit((c)))
#   define	ISO5_isalnum(c)	(ISO5_isupper((c))||ISO5_islower((c))||\
				  ISO5_isdigit((c)))

#   define	ISO5_tolower(c) ( \
		  (ASCII_isupper(c)?ASCII_tolower(c): (\
		  ((c)>=ISO5_afii10023&&(c)<=ISO5_afii10061 )? ((c)+0x50): (\
		  ((c)>=ISO5_afii10062&&(c)<=ISO5_afii10145 )? ((c)+0x50): (\
		  ((c)>=ISO5_afii10017&&(c)<=ISO5_afii10049 )? ((c)+0x20): (\
		       (c) ))))))

#   define	ISO5_toupper(c) ( \
		  (ASCII_islower(c)?ASCII_toupper(c): (\
		  ((c)>=ISO5_afii10065&&(c)<=ISO5_afii10097 )? ((c)-0x20): (\
		  ((c)>=ISO5_afii10071&&(c)<=ISO5_afii10109 )? ((c)-0x50): (\
		  ((c)>=ISO5_afii10110&&(c)<=ISO5_afii10193 )? ((c)-0x50): (\
		       (c) ))))))

/************************************************************************/
/*									*/
/*  Names for the characters in the Windows 1251 code page.		*/
/*  (Cyrillic)								*/
/*									*/
/************************************************************************/

#   define	WIN1251__0x00		0x00	/*    0		*/
#   define	WIN1251__0x01		0x01	/*    1		*/
#   define	WIN1251__0x02		0x02	/*    2		*/
#   define	WIN1251__0x03		0x03	/*    3		*/
#   define	WIN1251__0x04		0x04	/*    4		*/
#   define	WIN1251__0x05		0x05	/*    5		*/
#   define	WIN1251__0x06		0x06	/*    6		*/
#   define	WIN1251__0x07		0x07	/*    7		*/
#   define	WIN1251__0x08		0x08	/*    8		*/
#   define	WIN1251__0x09		0x09	/*    9		*/
#   define	WIN1251__0x0A		0x0a	/*   10		*/
#   define	WIN1251__0x0B		0x0b	/*   11		*/
#   define	WIN1251__0x0C		0x0c	/*   12		*/
#   define	WIN1251__0x0D		0x0d	/*   13		*/
#   define	WIN1251__0x0E		0x0e	/*   14		*/
#   define	WIN1251__0x0F		0x0f	/*   15		*/
#   define	WIN1251__0x10		0x10	/*   16		*/
#   define	WIN1251__0x11		0x11	/*   17		*/
#   define	WIN1251__0x12		0x12	/*   18		*/
#   define	WIN1251__0x13		0x13	/*   19		*/
#   define	WIN1251__0x14		0x14	/*   20		*/
#   define	WIN1251__0x15		0x15	/*   21		*/
#   define	WIN1251__0x16		0x16	/*   22		*/
#   define	WIN1251__0x17		0x17	/*   23		*/
#   define	WIN1251__0x18		0x18	/*   24		*/
#   define	WIN1251__0x19		0x19	/*   25		*/
#   define	WIN1251__0x1A		0x1a	/*   26		*/
#   define	WIN1251__0x1B		0x1b	/*   27		*/
#   define	WIN1251__0x1C		0x1c	/*   28		*/
#   define	WIN1251__0x1D		0x1d	/*   29		*/
#   define	WIN1251__0x1E		0x1e	/*   30		*/
#   define	WIN1251__0x1F		0x1f	/*   31		*/
#   define	WIN1251_space		0x20	/*   32		*/
#   define	WIN1251_exclam		0x21	/*   33		*/
#   define	WIN1251_quotedbl	0x22	/*   34		*/
#   define	WIN1251_numbersign	0x23	/*   35		*/
#   define	WIN1251_dollar		0x24	/*   36		*/
#   define	WIN1251_percent		0x25	/*   37		*/
#   define	WIN1251_ampersand	0x26	/*   38		*/
#   define	WIN1251_quotesingle	0x27	/*   39		*/
#   define	WIN1251_parenleft	0x28	/*   40		*/
#   define	WIN1251_parenright	0x29	/*   41		*/
#   define	WIN1251_asterisk	0x2a	/*   42		*/
#   define	WIN1251_plus		0x2b	/*   43		*/
#   define	WIN1251_comma		0x2c	/*   44		*/
#   define	WIN1251_minus		0x2d	/*   45		*/
#   define	WIN1251_period		0x2e	/*   46		*/
#   define	WIN1251_slash		0x2f	/*   47		*/
#   define	WIN1251_zero		0x30	/*   48		*/
#   define	WIN1251_one		0x31	/*   49		*/
#   define	WIN1251_two		0x32	/*   50		*/
#   define	WIN1251_three		0x33	/*   51		*/
#   define	WIN1251_four		0x34	/*   52		*/
#   define	WIN1251_five		0x35	/*   53		*/
#   define	WIN1251_six		0x36	/*   54		*/
#   define	WIN1251_seven		0x37	/*   55		*/
#   define	WIN1251_eight		0x38	/*   56		*/
#   define	WIN1251_nine		0x39	/*   57		*/
#   define	WIN1251_colon		0x3a	/*   58		*/
#   define	WIN1251_semicolon	0x3b	/*   59		*/
#   define	WIN1251_less		0x3c	/*   60		*/
#   define	WIN1251_equal		0x3d	/*   61		*/
#   define	WIN1251_greater		0x3e	/*   62		*/
#   define	WIN1251_question	0x3f	/*   63		*/
#   define	WIN1251_at		0x40	/*   64		*/
#   define	WIN1251_A		0x41	/*   65		*/
#   define	WIN1251_B		0x42	/*   66		*/
#   define	WIN1251_C		0x43	/*   67		*/
#   define	WIN1251_D		0x44	/*   68		*/
#   define	WIN1251_E		0x45	/*   69		*/
#   define	WIN1251_F		0x46	/*   70		*/
#   define	WIN1251_G		0x47	/*   71		*/
#   define	WIN1251_H		0x48	/*   72		*/
#   define	WIN1251_I		0x49	/*   73		*/
#   define	WIN1251_J		0x4a	/*   74		*/
#   define	WIN1251_K		0x4b	/*   75		*/
#   define	WIN1251_L		0x4c	/*   76		*/
#   define	WIN1251_M		0x4d	/*   77		*/
#   define	WIN1251_N		0x4e	/*   78		*/
#   define	WIN1251_O		0x4f	/*   79		*/
#   define	WIN1251_P		0x50	/*   80		*/
#   define	WIN1251_Q		0x51	/*   81		*/
#   define	WIN1251_R		0x52	/*   82		*/
#   define	WIN1251_S		0x53	/*   83		*/
#   define	WIN1251_T		0x54	/*   84		*/
#   define	WIN1251_U		0x55	/*   85		*/
#   define	WIN1251_V		0x56	/*   86		*/
#   define	WIN1251_W		0x57	/*   87		*/
#   define	WIN1251_X		0x58	/*   88		*/
#   define	WIN1251_Y		0x59	/*   89		*/
#   define	WIN1251_Z		0x5a	/*   90		*/
#   define	WIN1251_bracketleft	0x5b	/*   91		*/
#   define	WIN1251_backslash	0x5c	/*   92		*/
#   define	WIN1251_bracketright	0x5d	/*   93		*/
#   define	WIN1251_asciicircum	0x5e	/*   94		*/
#   define	WIN1251_underscore	0x5f	/*   95		*/
#   define	WIN1251_quoteleft	0x60	/*   96		*/
#   define	WIN1251_a		0x61	/*   97		*/
#   define	WIN1251_b		0x62	/*   98		*/
#   define	WIN1251_c		0x63	/*   99		*/
#   define	WIN1251_d		0x64	/*  100		*/
#   define	WIN1251_e		0x65	/*  101		*/
#   define	WIN1251_f		0x66	/*  102		*/
#   define	WIN1251_g		0x67	/*  103		*/
#   define	WIN1251_h		0x68	/*  104		*/
#   define	WIN1251_i		0x69	/*  105		*/
#   define	WIN1251_j		0x6a	/*  106		*/
#   define	WIN1251_k		0x6b	/*  107		*/
#   define	WIN1251_l		0x6c	/*  108		*/
#   define	WIN1251_m		0x6d	/*  109		*/
#   define	WIN1251_n		0x6e	/*  110		*/
#   define	WIN1251_o		0x6f	/*  111		*/
#   define	WIN1251_p		0x70	/*  112		*/
#   define	WIN1251_q		0x71	/*  113		*/
#   define	WIN1251_r		0x72	/*  114		*/
#   define	WIN1251_s		0x73	/*  115		*/
#   define	WIN1251_t		0x74	/*  116		*/
#   define	WIN1251_u		0x75	/*  117		*/
#   define	WIN1251_v		0x76	/*  118		*/
#   define	WIN1251_w		0x77	/*  119		*/
#   define	WIN1251_x		0x78	/*  120		*/
#   define	WIN1251_y		0x79	/*  121		*/
#   define	WIN1251_z		0x7a	/*  122		*/
#   define	WIN1251_braceleft	0x7b	/*  123		*/
#   define	WIN1251_bar		0x7c	/*  124		*/
#   define	WIN1251_braceright	0x7d	/*  125		*/
#   define	WIN1251_asciitilde	0x7e	/*  126		*/
#   define	WIN1251__0x7F		0x7f	/*  127		*/
#   define	WIN1251_afii10051	0x80	/*  128		*/
#   define	WIN1251_afii10052	0x81	/*  129		*/
#   define	WIN1251_quotesinglbase	0x82	/*  130		*/
#   define	WIN1251_afii10100	0x83	/*  131		*/
#   define	WIN1251_quotedblbase	0x84	/*  132		*/
#   define	WIN1251_ellipsis	0x85	/*  133		*/
#   define	WIN1251_dagger		0x86	/*  134		*/
#   define	WIN1251_daggerdbl	0x87	/*  135		*/
#   define	WIN1251__0x88		0x88	/*  136		*/
#   define	WIN1251_perthousand	0x89	/*  137		*/
#   define	WIN1251_afii10058	0x8a	/*  138		*/
#   define	WIN1251_guilsinglleft	0x8b	/*  139		*/
#   define	WIN1251_afii10059	0x8c	/*  140		*/
#   define	WIN1251_afii10061	0x8d	/*  141		*/
#   define	WIN1251_afii10060	0x8e	/*  142		*/
#   define	WIN1251_afii10145	0x8f	/*  143		*/
#   define	WIN1251_afii10099	0x90	/*  144		*/
#   define	WIN1251__quoteleft	0x91	/*  145		*/
#   define	WIN1251_quoteright	0x92	/*  146		*/
#   define	WIN1251_quotedblleft	0x93	/*  147		*/
#   define	WIN1251_quotedblright	0x94	/*  148		*/
#   define	WIN1251_bullet		0x95	/*  149		*/
#   define	WIN1251_endash		0x96	/*  150		*/
#   define	WIN1251_emdash		0x97	/*  151		*/
#   define	WIN1251__0x98		0x98	/*  152		*/
#   define	WIN1251_trademark	0x99	/*  153		*/
#   define	WIN1251_afii10106	0x9a	/*  154		*/
#   define	WIN1251_guilsinglright	0x9b	/*  155		*/
#   define	WIN1251_afii10107	0x9c	/*  156		*/
#   define	WIN1251_afii10109	0x9d	/*  157		*/
#   define	WIN1251_afii10108	0x9e	/*  158		*/
#   define	WIN1251_afii10193	0x9f	/*  159		*/
#   define	WIN1251_nobreakspace	0xa0	/*  160		*/
#   define	WIN1251_afii10062	0xa1	/*  161		*/
#   define	WIN1251_afii10110	0xa2	/*  162		*/
#   define	WIN1251_afii10057	0xa3	/*  163		*/
#   define	WIN1251_currency	0xa4	/*  164		*/
#   define	WIN1251_afii10050	0xa5	/*  165		*/
#   define	WIN1251_brokenbar	0xa6	/*  166		*/
#   define	WIN1251_section		0xa7	/*  167		*/
#   define	WIN1251_afii10023	0xa8	/*  168		*/
#   define	WIN1251_copyright	0xa9	/*  169		*/
#   define	WIN1251_afii10053	0xaa	/*  170		*/
#   define	WIN1251_guillemotleft	0xab	/*  171		*/
#   define	WIN1251_logicalnot	0xac	/*  172		*/
#   define	WIN1251_hyphen		0xad	/*  173		*/
#   define	WIN1251_registered	0xae	/*  174		*/
#   define	WIN1251_afii10056	0xaf	/*  175		*/
#   define	WIN1251_degree		0xb0	/*  176		*/
#   define	WIN1251_plusminus	0xb1	/*  177		*/
#   define	WIN1251_afii10055	0xb2	/*  178		*/
#   define	WIN1251_afii10103	0xb3	/*  179		*/
#   define	WIN1251_afii10098	0xb4	/*  180		*/
#   define	WIN1251_mu		0xb5	/*  181		*/
#   define	WIN1251_paragraph	0xb6	/*  182		*/
#   define	WIN1251_periodcentered	0xb7	/*  183		*/
#   define	WIN1251_afii10071	0xb8	/*  184		*/
#   define	WIN1251_afii61352	0xb9	/*  185		*/
#   define	WIN1251_afii10101	0xba	/*  186		*/
#   define	WIN1251_guillemotright	0xbb	/*  187		*/
#   define	WIN1251_afii10105	0xbc	/*  188		*/
#   define	WIN1251_afii10054	0xbd	/*  189		*/
#   define	WIN1251_afii10102	0xbe	/*  190		*/
#   define	WIN1251_afii10104	0xbf	/*  191		*/
#   define	WIN1251_afii10017	0xc0	/*  192		*/
#   define	WIN1251_afii10018	0xc1	/*  193		*/
#   define	WIN1251_afii10019	0xc2	/*  194		*/
#   define	WIN1251_afii10020	0xc3	/*  195		*/
#   define	WIN1251_afii10021	0xc4	/*  196		*/
#   define	WIN1251_afii10022	0xc5	/*  197		*/
#   define	WIN1251_afii10024	0xc6	/*  198		*/
#   define	WIN1251_afii10025	0xc7	/*  199		*/
#   define	WIN1251_afii10026	0xc8	/*  200		*/
#   define	WIN1251_afii10027	0xc9	/*  201		*/
#   define	WIN1251_afii10028	0xca	/*  202		*/
#   define	WIN1251_afii10029	0xcb	/*  203		*/
#   define	WIN1251_afii10030	0xcc	/*  204		*/
#   define	WIN1251_afii10031	0xcd	/*  205		*/
#   define	WIN1251_afii10032	0xce	/*  206		*/
#   define	WIN1251_afii10033	0xcf	/*  207		*/
#   define	WIN1251_afii10034	0xd0	/*  208		*/
#   define	WIN1251_afii10035	0xd1	/*  209		*/
#   define	WIN1251_afii10036	0xd2	/*  210		*/
#   define	WIN1251_afii10037	0xd3	/*  211		*/
#   define	WIN1251_afii10038	0xd4	/*  212		*/
#   define	WIN1251_afii10039	0xd5	/*  213		*/
#   define	WIN1251_afii10040	0xd6	/*  214		*/
#   define	WIN1251_afii10041	0xd7	/*  215		*/
#   define	WIN1251_afii10042	0xd8	/*  216		*/
#   define	WIN1251_afii10043	0xd9	/*  217		*/
#   define	WIN1251_afii10044	0xda	/*  218		*/
#   define	WIN1251_afii10045	0xdb	/*  219		*/
#   define	WIN1251_afii10046	0xdc	/*  220		*/
#   define	WIN1251_afii10047	0xdd	/*  221		*/
#   define	WIN1251_afii10048	0xde	/*  222		*/
#   define	WIN1251_afii10049	0xdf	/*  223		*/
#   define	WIN1251_afii10065	0xe0	/*  224		*/
#   define	WIN1251_afii10066	0xe1	/*  225		*/
#   define	WIN1251_afii10067	0xe2	/*  226		*/
#   define	WIN1251_afii10068	0xe3	/*  227		*/
#   define	WIN1251_afii10069	0xe4	/*  228		*/
#   define	WIN1251_afii10070	0xe5	/*  229		*/
#   define	WIN1251_afii10072	0xe6	/*  230		*/
#   define	WIN1251_afii10073	0xe7	/*  231		*/
#   define	WIN1251_afii10074	0xe8	/*  232		*/
#   define	WIN1251_afii10075	0xe9	/*  233		*/
#   define	WIN1251_afii10076	0xea	/*  234		*/
#   define	WIN1251_afii10077	0xeb	/*  235		*/
#   define	WIN1251_afii10078	0xec	/*  236		*/
#   define	WIN1251_afii10079	0xed	/*  237		*/
#   define	WIN1251_afii10080	0xee	/*  238		*/
#   define	WIN1251_afii10081	0xef	/*  239		*/
#   define	WIN1251_afii10082	0xf0	/*  240		*/
#   define	WIN1251_afii10083	0xf1	/*  241		*/
#   define	WIN1251_afii10084	0xf2	/*  242		*/
#   define	WIN1251_afii10085	0xf3	/*  243		*/
#   define	WIN1251_afii10086	0xf4	/*  244		*/
#   define	WIN1251_afii10087	0xf5	/*  245		*/
#   define	WIN1251_afii10088	0xf6	/*  246		*/
#   define	WIN1251_afii10089	0xf7	/*  247		*/
#   define	WIN1251_afii10090	0xf8	/*  248		*/
#   define	WIN1251_afii10091	0xf9	/*  249		*/
#   define	WIN1251_afii10092	0xfa	/*  250		*/
#   define	WIN1251_afii10093	0xfb	/*  251		*/
#   define	WIN1251_afii10094	0xfc	/*  252		*/
#   define	WIN1251_afii10095	0xfd	/*  253		*/
#   define	WIN1251_afii10096	0xfe	/*  254		*/
#   define	WIN1251_afii10097	0xff	/*  255		*/

/************************************************************************/
/*									*/
/*  Names for the characters in the KOI-8 Russian Encoding.		*/
/*									*/
/************************************************************************/

#   define	KOI8R__0x00		0x00	/*    0		*/
#   define	KOI8R__0x01		0x01	/*    1		*/
#   define	KOI8R__0x02		0x02	/*    2		*/
#   define	KOI8R__0x03		0x03	/*    3		*/
#   define	KOI8R__0x04		0x04	/*    4		*/
#   define	KOI8R__0x05		0x05	/*    5		*/
#   define	KOI8R__0x06		0x06	/*    6		*/
#   define	KOI8R__0x07		0x07	/*    7		*/
#   define	KOI8R__0x08		0x08	/*    8		*/
#   define	KOI8R__0x09		0x09	/*    9		*/
#   define	KOI8R__0x0A		0x0a	/*   10		*/
#   define	KOI8R__0x0B		0x0b	/*   11		*/
#   define	KOI8R__0x0C		0x0c	/*   12		*/
#   define	KOI8R__0x0D		0x0d	/*   13		*/
#   define	KOI8R__0x0E		0x0e	/*   14		*/
#   define	KOI8R__0x0F		0x0f	/*   15		*/
#   define	KOI8R__0x10		0x10	/*   16		*/
#   define	KOI8R__0x11		0x11	/*   17		*/
#   define	KOI8R__0x12		0x12	/*   18		*/
#   define	KOI8R__0x13		0x13	/*   19		*/
#   define	KOI8R__0x14		0x14	/*   20		*/
#   define	KOI8R__0x15		0x15	/*   21		*/
#   define	KOI8R__0x16		0x16	/*   22		*/
#   define	KOI8R__0x17		0x17	/*   23		*/
#   define	KOI8R__0x18		0x18	/*   24		*/
#   define	KOI8R__0x19		0x19	/*   25		*/
#   define	KOI8R__0x1A		0x1a	/*   26		*/
#   define	KOI8R__0x1B		0x1b	/*   27		*/
#   define	KOI8R__0x1C		0x1c	/*   28		*/
#   define	KOI8R__0x1D		0x1d	/*   29		*/
#   define	KOI8R__0x1E		0x1e	/*   30		*/
#   define	KOI8R__0x1F		0x1f	/*   31		*/
#   define	KOI8R_space		0x20	/*   32		*/
#   define	KOI8R_exclam		0x21	/*   33		*/
#   define	KOI8R_quotedbl		0x22	/*   34		*/
#   define	KOI8R_numbersign	0x23	/*   35		*/
#   define	KOI8R_dollar		0x24	/*   36		*/
#   define	KOI8R_percent		0x25	/*   37		*/
#   define	KOI8R_ampersand		0x26	/*   38		*/
#   define	KOI8R_quotesingle	0x27	/*   39		*/
#   define	KOI8R_parenleft		0x28	/*   40		*/
#   define	KOI8R_parenright	0x29	/*   41		*/
#   define	KOI8R_asterisk		0x2a	/*   42		*/
#   define	KOI8R_plus		0x2b	/*   43		*/
#   define	KOI8R_comma		0x2c	/*   44		*/
#   define	KOI8R_minus		0x2d	/*   45		*/
#   define	KOI8R_period		0x2e	/*   46		*/
#   define	KOI8R_slash		0x2f	/*   47		*/
#   define	KOI8R_zero		0x30	/*   48		*/
#   define	KOI8R_one		0x31	/*   49		*/
#   define	KOI8R_two		0x32	/*   50		*/
#   define	KOI8R_three		0x33	/*   51		*/
#   define	KOI8R_four		0x34	/*   52		*/
#   define	KOI8R_five		0x35	/*   53		*/
#   define	KOI8R_six		0x36	/*   54		*/
#   define	KOI8R_seven		0x37	/*   55		*/
#   define	KOI8R_eight		0x38	/*   56		*/
#   define	KOI8R_nine		0x39	/*   57		*/
#   define	KOI8R_colon		0x3a	/*   58		*/
#   define	KOI8R_semicolon		0x3b	/*   59		*/
#   define	KOI8R_less		0x3c	/*   60		*/
#   define	KOI8R_equal		0x3d	/*   61		*/
#   define	KOI8R_greater		0x3e	/*   62		*/
#   define	KOI8R_question		0x3f	/*   63		*/
#   define	KOI8R_at		0x40	/*   64		*/
#   define	KOI8R_A			0x41	/*   65		*/
#   define	KOI8R_B			0x42	/*   66		*/
#   define	KOI8R_C			0x43	/*   67		*/
#   define	KOI8R_D			0x44	/*   68		*/
#   define	KOI8R_E			0x45	/*   69		*/
#   define	KOI8R_F			0x46	/*   70		*/
#   define	KOI8R_G			0x47	/*   71		*/
#   define	KOI8R_H			0x48	/*   72		*/
#   define	KOI8R_I			0x49	/*   73		*/
#   define	KOI8R_J			0x4a	/*   74		*/
#   define	KOI8R_K			0x4b	/*   75		*/
#   define	KOI8R_L			0x4c	/*   76		*/
#   define	KOI8R_M			0x4d	/*   77		*/
#   define	KOI8R_N			0x4e	/*   78		*/
#   define	KOI8R_O			0x4f	/*   79		*/
#   define	KOI8R_P			0x50	/*   80		*/
#   define	KOI8R_Q			0x51	/*   81		*/
#   define	KOI8R_R			0x52	/*   82		*/
#   define	KOI8R_S			0x53	/*   83		*/
#   define	KOI8R_T			0x54	/*   84		*/
#   define	KOI8R_U			0x55	/*   85		*/
#   define	KOI8R_V			0x56	/*   86		*/
#   define	KOI8R_W			0x57	/*   87		*/
#   define	KOI8R_X			0x58	/*   88		*/
#   define	KOI8R_Y			0x59	/*   89		*/
#   define	KOI8R_Z			0x5a	/*   90		*/
#   define	KOI8R_bracketleft	0x5b	/*   91		*/
#   define	KOI8R_backslash		0x5c	/*   92		*/
#   define	KOI8R_bracketright	0x5d	/*   93		*/
#   define	KOI8R_asciicircum	0x5e	/*   94		*/
#   define	KOI8R_underscore	0x5f	/*   95		*/
#   define	KOI8R_quoteleft		0x60	/*   96		*/
#   define	KOI8R_a			0x61	/*   97		*/
#   define	KOI8R_b			0x62	/*   98		*/
#   define	KOI8R_c			0x63	/*   99		*/
#   define	KOI8R_d			0x64	/*  100		*/
#   define	KOI8R_e			0x65	/*  101		*/
#   define	KOI8R_f			0x66	/*  102		*/
#   define	KOI8R_g			0x67	/*  103		*/
#   define	KOI8R_h			0x68	/*  104		*/
#   define	KOI8R_i			0x69	/*  105		*/
#   define	KOI8R_j			0x6a	/*  106		*/
#   define	KOI8R_k			0x6b	/*  107		*/
#   define	KOI8R_l			0x6c	/*  108		*/
#   define	KOI8R_m			0x6d	/*  109		*/
#   define	KOI8R_n			0x6e	/*  110		*/
#   define	KOI8R_o			0x6f	/*  111		*/
#   define	KOI8R_p			0x70	/*  112		*/
#   define	KOI8R_q			0x71	/*  113		*/
#   define	KOI8R_r			0x72	/*  114		*/
#   define	KOI8R_s			0x73	/*  115		*/
#   define	KOI8R_t			0x74	/*  116		*/
#   define	KOI8R_u			0x75	/*  117		*/
#   define	KOI8R_v			0x76	/*  118		*/
#   define	KOI8R_w			0x77	/*  119		*/
#   define	KOI8R_x			0x78	/*  120		*/
#   define	KOI8R_y			0x79	/*  121		*/
#   define	KOI8R_z			0x7a	/*  122		*/
#   define	KOI8R_braceleft		0x7b	/*  123		*/
#   define	KOI8R_bar		0x7c	/*  124		*/
#   define	KOI8R_braceright	0x7d	/*  125		*/
#   define	KOI8R_asciitilde	0x7e	/*  126		*/
#   define	KOI8R__0x7F		0x7f	/*  127		*/
#   define	KOI8R_SF100000		0x80	/*  128		*/
#   define	KOI8R_SF110000		0x81	/*  129		*/
#   define	KOI8R_SF010000		0x82	/*  130		*/
#   define	KOI8R_SF030000		0x83	/*  131		*/
#   define	KOI8R_SF020000		0x84	/*  132		*/
#   define	KOI8R_SF040000		0x85	/*  133		*/
#   define	KOI8R_SF080000		0x86	/*  134		*/
#   define	KOI8R_SF090000		0x87	/*  135		*/
#   define	KOI8R_SF060000		0x88	/*  136		*/
#   define	KOI8R_SF070000		0x89	/*  137		*/
#   define	KOI8R_SF050000		0x8a	/*  138		*/
#   define	KOI8R_upblock		0x8b	/*  139		*/
#   define	KOI8R_dnblock		0x8c	/*  140		*/
#   define	KOI8R_block		0x8d	/*  141		*/
#   define	KOI8R_lfblock		0x8e	/*  142		*/
#   define	KOI8R_rtblock		0x8f	/*  143		*/
#   define	KOI8R_ltshade		0x90	/*  144		*/
#   define	KOI8R_shade		0x91	/*  145		*/
#   define	KOI8R_dkshade		0x92	/*  146		*/
#   define	KOI8R_integraltp	0x93	/*  147		*/
#   define	KOI8R_filledbox		0x94	/*  148		*/
#   define	KOI8R_bullet		0x95	/*  149		*/
#   define	KOI8R_radical		0x96	/*  150		*/
#   define	KOI8R_approxequal	0x97	/*  151		*/
#   define	KOI8R_lessequal		0x98	/*  152		*/
#   define	KOI8R_greaterequal	0x99	/*  153		*/
#   define	KOI8R_nobreakspace	0x9a	/*  154		*/
#   define	KOI8R_integralbt	0x9b	/*  155		*/
#   define	KOI8R_degree		0x9c	/*  156		*/
#   define	KOI8R_twosuperior	0x9d	/*  157		*/
#   define	KOI8R_periodcentered	0x9e	/*  158		*/
#   define	KOI8R_divide		0x9f	/*  159		*/
#   define	KOI8R_SF430000		0xa0	/*  160		*/
#   define	KOI8R_SF240000		0xa1	/*  161		*/
#   define	KOI8R_SF510000		0xa2	/*  162		*/
#   define	KOI8R_afii10071		0xa3	/*  163 io	*/
#   define	KOI8R_SF520000		0xa4	/*  164		*/
#   define	KOI8R_SF390000		0xa5	/*  165		*/
#   define	KOI8R_SF220000		0xa6	/*  166		*/
#   define	KOI8R_SF210000		0xa7	/*  167		*/
#   define	KOI8R_SF250000		0xa8	/*  168		*/
#   define	KOI8R_SF500000		0xa9	/*  169		*/
#   define	KOI8R_SF490000		0xaa	/*  170		*/
#   define	KOI8R_SF380000		0xab	/*  171		*/
#   define	KOI8R_SF280000		0xac	/*  172		*/
#   define	KOI8R_SF270000		0xad	/*  173		*/
#   define	KOI8R_SF260000		0xae	/*  174		*/
#   define	KOI8R_SF360000		0xaf	/*  175		*/
#   define	KOI8R_SF370000		0xb0	/*  176		*/
#   define	KOI8R_SF420000		0xb1	/*  177		*/
#   define	KOI8R_SF190000		0xb2	/*  178		*/
#   define	KOI8R_afii10023		0xb3	/*  179 IO	*/
#   define	KOI8R_SF200000		0xb4	/*  180		*/
#   define	KOI8R_SF230000		0xb5	/*  181		*/
#   define	KOI8R_SF470000		0xb6	/*  182		*/
#   define	KOI8R_SF480000		0xb7	/*  183		*/
#   define	KOI8R_SF410000		0xb8	/*  184		*/
#   define	KOI8R_SF450000		0xb9	/*  185		*/
#   define	KOI8R_SF460000		0xba	/*  186		*/
#   define	KOI8R_SF400000		0xbb	/*  187		*/
#   define	KOI8R_SF540000		0xbc	/*  188		*/
#   define	KOI8R_SF530000		0xbd	/*  189		*/
#   define	KOI8R_SF440000		0xbe	/*  190		*/
#   define	KOI8R_copyright		0xbf	/*  191		*/
#   define	KOI8R_afii10096		0xc0	/*  192		*/
#   define	KOI8R_afii10065		0xc1	/*  193		*/
#   define	KOI8R_afii10066		0xc2	/*  194		*/
#   define	KOI8R_afii10088		0xc3	/*  195		*/
#   define	KOI8R_afii10069		0xc4	/*  196		*/
#   define	KOI8R_afii10070		0xc5	/*  197		*/
#   define	KOI8R_afii10086		0xc6	/*  198		*/
#   define	KOI8R_afii10068		0xc7	/*  199		*/
#   define	KOI8R_afii10087		0xc8	/*  200		*/
#   define	KOI8R_afii10074		0xc9	/*  201		*/
#   define	KOI8R_afii10075		0xca	/*  202		*/
#   define	KOI8R_afii10076		0xcb	/*  203		*/
#   define	KOI8R_afii10077		0xcc	/*  204		*/
#   define	KOI8R_afii10078		0xcd	/*  205		*/
#   define	KOI8R_afii10079		0xce	/*  206		*/
#   define	KOI8R_afii10080		0xcf	/*  207		*/
#   define	KOI8R_afii10081		0xd0	/*  208		*/
#   define	KOI8R_afii10097		0xd1	/*  209		*/
#   define	KOI8R_afii10082		0xd2	/*  210		*/
#   define	KOI8R_afii10083		0xd3	/*  211		*/
#   define	KOI8R_afii10084		0xd4	/*  212		*/
#   define	KOI8R_afii10085		0xd5	/*  213		*/
#   define	KOI8R_afii10072		0xd6	/*  214		*/
#   define	KOI8R_afii10067		0xd7	/*  215		*/
#   define	KOI8R_afii10094		0xd8	/*  216		*/
#   define	KOI8R_afii10093		0xd9	/*  217		*/
#   define	KOI8R_afii10073		0xda	/*  218		*/
#   define	KOI8R_afii10090		0xdb	/*  219		*/
#   define	KOI8R_afii10095		0xdc	/*  220		*/
#   define	KOI8R_afii10091		0xdd	/*  221		*/
#   define	KOI8R_afii10089		0xde	/*  222		*/
#   define	KOI8R_afii10092		0xdf	/*  223		*/
#   define	KOI8R_afii10048		0xe0	/*  224		*/
#   define	KOI8R_afii10017		0xe1	/*  225		*/
#   define	KOI8R_afii10018		0xe2	/*  226		*/
#   define	KOI8R_afii10040		0xe3	/*  227		*/
#   define	KOI8R_afii10021		0xe4	/*  228		*/
#   define	KOI8R_afii10022		0xe5	/*  229		*/
#   define	KOI8R_afii10038		0xe6	/*  230		*/
#   define	KOI8R_afii10020		0xe7	/*  231		*/
#   define	KOI8R_afii10039		0xe8	/*  232		*/
#   define	KOI8R_afii10026		0xe9	/*  233		*/
#   define	KOI8R_afii10027		0xea	/*  234		*/
#   define	KOI8R_afii10028		0xeb	/*  235		*/
#   define	KOI8R_afii10029		0xec	/*  236		*/
#   define	KOI8R_afii10030		0xed	/*  237		*/
#   define	KOI8R_afii10031		0xee	/*  238		*/
#   define	KOI8R_afii10032		0xef	/*  239		*/
#   define	KOI8R_afii10033		0xf0	/*  240		*/
#   define	KOI8R_afii10049		0xf1	/*  241		*/
#   define	KOI8R_afii10034		0xf2	/*  242		*/
#   define	KOI8R_afii10035		0xf3	/*  243		*/
#   define	KOI8R_afii10036		0xf4	/*  244		*/
#   define	KOI8R_afii10037		0xf5	/*  245		*/
#   define	KOI8R_afii10024		0xf6	/*  246		*/
#   define	KOI8R_afii10019		0xf7	/*  247		*/
#   define	KOI8R_afii10046		0xf8	/*  248		*/
#   define	KOI8R_afii10045		0xf9	/*  249		*/
#   define	KOI8R_afii10025		0xfa	/*  250		*/
#   define	KOI8R_afii10042		0xfb	/*  251		*/
#   define	KOI8R_afii10047		0xfc	/*  252		*/
#   define	KOI8R_afii10043		0xfd	/*  253		*/
#   define	KOI8R_afii10041		0xfe	/*  254		*/
#   define	KOI8R_afii10044		0xff	/*  255		*/

#   define	KOI8R_isupper(c)	((ASCII_isupper((c)))|| \
					  (c) == KOI8R_afii10023 || \
					  ((c)>=KOI8R_afii10048 && \
					   (c)<=KOI8R_afii10044 ) )
#   define	KOI8R_islower(c)	((ASCII_islower((c)))||\
					  (c) == KOI8R_afii10071 || \
					  ((c)>=KOI8R_afii10096 && \
					   (c)<=KOI8R_afii10092 ) )
#   define	KOI8R_isalpha(c)	(KOI8R_isupper((c))||KOI8R_islower((c)))
#   define	KOI8R_isdigit(c)	(ASCII_isdigit((c)))
#   define	KOI8R_isalnum(c)	(KOI8R_isupper((c))|| \
					 KOI8R_islower((c))|| \
					 KOI8R_isdigit((c)) )

#   define	KOI8R_tolower(c)	(((ASCII_isupper(c)))? \
							    ((c)+0x20): \
					   ((c)==KOI8R_afii10023?  \
							    ((c)-0x10): \
					   KOI8R_isupper(c)? \
							    ((c)-0x20): \
							    (c)))

#   define	KOI8R_toupper(c)	(((ASCII_islower(c)))? \
							    ((c)-0x20): \
					   ((c)==KOI8R_afii10071?  \
							    ((c)+0x10): \
					   KOI8R_islower(c)? \
							    ((c)+0x20): \
							    (c)))

/************************************************************************/
/*									*/
/*  Names for the characters in the Windows 1250 code page.		*/
/*  (Eastern Europe)							*/
/*									*/
/************************************************************************/

#   define WIN1250__nul		ASCII__nul	/*  0		*/
#   define WIN1250__soh		ASCII__soh	/*  1		*/
#   define WIN1250__stx		ASCII__stx	/*  2		*/
#   define WIN1250__etx		ASCII__etx	/*  3		*/
#   define WIN1250__eot		ASCII__eot	/*  4		*/
#   define WIN1250__enq		ASCII__enq	/*  5		*/
#   define WIN1250__ack		ASCII__ack	/*  6		*/
#   define WIN1250__bel		ASCII__bel	/*  7		*/
#   define WIN1250__bs		ASCII__bs	/*  8		*/
#   define WIN1250__ht		ASCII__ht	/*  9		*/
#   define WIN1250__nl		ASCII__nl	/*  10		*/
#   define WIN1250__vt		ASCII__vt	/*  11		*/
#   define WIN1250__np		ASCII__np	/*  12		*/
#   define WIN1250__cr		ASCII__cr	/*  13		*/
#   define WIN1250__so		ASCII__so	/*  14		*/
#   define WIN1250__si		ASCII__si	/*  15		*/
#   define WIN1250__dle		ASCII__dle	/*  16		*/
#   define WIN1250__dc1		ASCII__dc1	/*  17		*/
#   define WIN1250__dc2		ASCII__dc2	/*  18		*/
#   define WIN1250__dc3		ASCII__dc3	/*  19		*/
#   define WIN1250__dc4		ASCII__dc4	/*  20		*/
#   define WIN1250__nak		ASCII__nak	/*  21		*/
#   define WIN1250__syn		ASCII__syn	/*  22		*/
#   define WIN1250__etb		ASCII__etb	/*  23		*/
#   define WIN1250__can		ASCII__can	/*  24		*/
#   define WIN1250__em		ASCII__em	/*  25		*/
#   define WIN1250__sub		ASCII__sub	/*  26		*/
#   define WIN1250__esc		ASCII__esc	/*  27		*/
#   define WIN1250__fs		ASCII__fs	/*  28		*/
#   define WIN1250__gs		ASCII__gs	/*  29		*/
#   define WIN1250__rs		ASCII__rs	/*  30		*/
#   define WIN1250__us		ASCII__us	/*  31		*/

#   define WIN1250_space	ASCII_space	/*  32		*/
#   define WIN1250_exclam	ASCII_exclam	/*  33		*/
#   define WIN1250_quotedbl	ASCII_quotedbl	/*  34		*/
#   define WIN1250_numbersign	ASCII_numbersign /*  35		*/
#   define WIN1250_dollar	ASCII_dollar	/*  36		*/
#   define WIN1250_percent	ASCII_percent	/*  37		*/
#   define WIN1250_ampersand	ASCII_ampersand	/*  38		*/
#   define WIN1250_quotesingle	ASCII_quotesingle /*  39	*/
#   define WIN1250_parenleft	ASCII_parenleft	/*  40		*/
#   define WIN1250_parenright	ASCII_parenright /*  41		*/
#   define WIN1250_asterisk	ASCII_asterisk	/*  42		*/
#   define WIN1250_plus		ASCII_plus	/*  43		*/
#   define WIN1250_comma	ASCII_comma	/*  44		*/
#   define WIN1250_minus	ASCII_minus	/*  45		*/
#   define WIN1250_period	ASCII_period	/*  46		*/
#   define WIN1250_slash	ASCII_slash	/*  47		*/
#   define WIN1250_zero		ASCII_zero	/*  48		*/
#   define WIN1250_one		ASCII_one	/*  49		*/
#   define WIN1250_two		ASCII_two	/*  50		*/
#   define WIN1250_three	ASCII_three	/*  51		*/
#   define WIN1250_four		ASCII_four	/*  52		*/
#   define WIN1250_five		ASCII_five	/*  53		*/
#   define WIN1250_six		ASCII_six	/*  54		*/
#   define WIN1250_seven	ASCII_seven	/*  55		*/
#   define WIN1250_eight	ASCII_eight	/*  56		*/
#   define WIN1250_nine		ASCII_nine	/*  57		*/
#   define WIN1250_colon	ASCII_colon	/*  58		*/
#   define WIN1250_semicolon	ASCII_semicolon	/*  59		*/
#   define WIN1250_less		ASCII_less	/*  60		*/
#   define WIN1250_equal	ASCII_equal	/*  61		*/
#   define WIN1250_greater	ASCII_greater	/*  62		*/
#   define WIN1250_question	ASCII_question	/*  63		*/

#   define WIN1250_at		ASCII_at	/*  64		*/
#   define WIN1250_A		ASCII_A		/*  65		*/
#   define WIN1250_B		ASCII_B		/*  66		*/
#   define WIN1250_C		ASCII_C		/*  67		*/
#   define WIN1250_D		ASCII_D		/*  68		*/
#   define WIN1250_E		ASCII_E		/*  69		*/
#   define WIN1250_F		ASCII_F		/*  70		*/
#   define WIN1250_G		ASCII_G		/*  71		*/
#   define WIN1250_H		ASCII_H		/*  72		*/
#   define WIN1250_I		ASCII_I		/*  73		*/
#   define WIN1250_J		ASCII_J		/*  74		*/
#   define WIN1250_K		ASCII_K		/*  75		*/
#   define WIN1250_L		ASCII_L		/*  76		*/
#   define WIN1250_M		ASCII_M		/*  77		*/
#   define WIN1250_N		ASCII_N		/*  78		*/
#   define WIN1250_O		ASCII_O		/*  79		*/
#   define WIN1250_P		ASCII_P		/*  80		*/
#   define WIN1250_Q		ASCII_Q		/*  81		*/
#   define WIN1250_R		ASCII_R		/*  82		*/
#   define WIN1250_S		ASCII_S		/*  83		*/
#   define WIN1250_T		ASCII_T		/*  84		*/
#   define WIN1250_U		ASCII_U		/*  85		*/
#   define WIN1250_V		ASCII_V		/*  86		*/
#   define WIN1250_W		ASCII_W		/*  87		*/
#   define WIN1250_X		ASCII_X		/*  88		*/
#   define WIN1250_Y		ASCII_Y		/*  89		*/
#   define WIN1250_Z		ASCII_Z		/*  90		*/
#   define WIN1250_bracketleft	ASCII_bracketleft /* 91		*/
#   define WIN1250_backslash	ASCII_backslash	/*  92		*/
#   define WIN1250_bracketright	ASCII_bracketright /* 93	*/
#   define WIN1250_circumflex	ASCII_circumflex /*  94		*/
#   define WIN1250_underscore	ASCII_underscore /*  95		*/

#   define WIN1250_quoteleft	ASCII_quoteleft	/*  96		*/
#   define WIN1250_a		ASCII_a		/*  97		*/
#   define WIN1250_b		ASCII_b		/*  98		*/
#   define WIN1250_c		ASCII_c		/*  99		*/
#   define WIN1250_d		ASCII_d		/*  100		*/
#   define WIN1250_e		ASCII_e		/*  101		*/
#   define WIN1250_f		ASCII_f		/*  102		*/
#   define WIN1250_g		ASCII_g		/*  103		*/
#   define WIN1250_h		ASCII_h		/*  104		*/
#   define WIN1250_i		ASCII_i		/*  105		*/
#   define WIN1250_j		ASCII_j		/*  106		*/
#   define WIN1250_k		ASCII_k		/*  107		*/
#   define WIN1250_l		ASCII_l		/*  108		*/
#   define WIN1250_m		ASCII_m		/*  109		*/
#   define WIN1250_n		ASCII_n		/*  110		*/
#   define WIN1250_o		ASCII_o		/*  111		*/
#   define WIN1250_p		ASCII_p		/*  112		*/
#   define WIN1250_q		ASCII_q		/*  113		*/
#   define WIN1250_r		ASCII_r		/*  114		*/
#   define WIN1250_s		ASCII_s		/*  115		*/
#   define WIN1250_t		ASCII_t		/*  116		*/
#   define WIN1250_u		ASCII_u		/*  117		*/
#   define WIN1250_v		ASCII_v		/*  118		*/
#   define WIN1250_w		ASCII_w		/*  119		*/
#   define WIN1250_x		ASCII_x		/*  120		*/
#   define WIN1250_y		ASCII_y		/*  121		*/
#   define WIN1250_z		ASCII_z		/*  122		*/
#   define WIN1250_braceleft	ASCII_braceleft	/*  123		*/
#   define WIN1250_bar		ASCII_bar	/*  124		*/
#   define WIN1250_braceright	ASCII_braceright /* 125		*/
#   define WIN1250_tilde	ASCII_tilde	/*  126		*/
#   define WIN1250__del		ASCII__del	/*  127		*/

#   define WIN1250__0x80	0x80		/*  128		*//**/
#   define WIN1250__0x81	0x81		/*  129		*//**/
#   define WIN1250_quotesinglbase 0x82		/*  130		*/
#   define WIN1250_florin	0x83		/*  131		*/
#   define WIN1250_quotedblbase	0x84		/*  132		*/
#   define WIN1250_ellipsis	0x85		/*  133		*/
#   define WIN1250_dagger	0x86		/*  134		*/
#   define WIN1250_daggerdbl	0x87		/*  135		*/
#   define WIN1250__circumflex_136 0x88		/*  136		*/
#   define WIN1250_perthousand	0x89		/*  137		*/
#   define WIN1250_Scaron	0x8a		/*  138		*/
#   define WIN1250_guilsinglleft 0x8b		/*  139		*/
#   define WIN1250_Sacute	0x8c		/*  140		*/
#   define WIN1250_Tcaron	0x8d		/*  141		*/
#   define WIN1250_Zcaron	0x8e		/*  142		*/
#   define WIN1250_Zacute	0x8f		/*  143		*/
#   define WIN1250__0x90	0x90		/*  144		*//**/
#   define WIN1250__quoteleft_145 0x91		/*  145		*/
#   define WIN1250__quoteright_146 0x92		/*  146		*/
#   define WIN1250_quotedblleft	0x93		/*  147		*/
#   define WIN1250_quotedblright 0x94		/*  148		*/
#   define WIN1250_bullet	0x95		/*  149		*/
#   define WIN1250_endash	0x96		/*  150		*/
#   define WIN1250_emdash	0x97		/*  151		*/
#   define WIN1250__tilde_152	0x98		/*  152		*/
#   define WIN1250_trademark	0x99		/*  153		*/
#   define WIN1250_scaron	0x9a		/*  154		*/
#   define WIN1250_guilsinglright 0x9b		/*  155		*/
#   define WIN1250_sacute	0x9c		/*  156		*/
#   define WIN1250_tcaron	0x9d		/*  157		*/
#   define WIN1250_zcaron	0x9e		/*  158		*/
#   define WIN1250_zacute	0x9f		/*  159		*/
#   define WIN1250_nobreakspace	0xa0		/*  160		*/
#   define WIN1250_caron	0xa1		/*  161		*/
#   define WIN1250_breve	0xa2		/*  162		*/
#   define WIN1250_Lslash	0xa3		/*  163		*/
#   define WIN1250_currency	0xa4		/*  164		*/
#   define WIN1250_Aogonek	0xa5		/*  165		*/
#   define WIN1250_brokenbar	0xa6		/*  166		*/
#   define WIN1250_section	0xa7		/*  167		*/
#   define WIN1250_dieresis	0xa8		/*  168		*/
#   define WIN1250_copyright	0xa9		/*  169		*/
#   define WIN1250_Scedilla	0xaa		/*  170		*/
#   define WIN1250_guillemotleft 0xab		/*  171		*/
#   define WIN1250_logicalnot	0xac		/*  172		*/
#   define WIN1250_hyphen	0xad		/*  173		*/
#   define WIN1250_registered	0xae		/*  174		*/
#   define WIN1250_Zdotaccent	0xaf		/*  175		*/
#   define WIN1250_degree	0xb0		/*  176		*/
#   define WIN1250_plusminus	0xb1		/*  177		*/
#   define WIN1250_ogonek	0xb2		/*  178		*/
#   define WIN1250_lslash	0xb3		/*  179		*/
#   define WIN1250_acute	0xb4		/*  180		*/
#   define WIN1250_mu		0xb5		/*  181		*/
#   define WIN1250_paragraph	0xb6		/*  182		*/
#   define WIN1250_periodcentered 0xb7		/*  183		*/
#   define WIN1250_cedilla	0xb8		/*  184		*/
#   define WIN1250_aogonek	0xb9		/*  185		*/
#   define WIN1250_scedilla	0xba		/*  186		*/
#   define WIN1250_guillemotright 0xbb		/*  187		*/
#   define WIN1250_Lcaron	0xbc		/*  188		*/
#   define WIN1250_hungarumlaut	0xbd		/*  189		*/
#   define WIN1250_lcaron	0xbe		/*  190		*/
#   define WIN1250_zdotaccent	0xbf		/*  191		*/
#   define WIN1250_Racute	0xc0		/*  192		*/
#   define WIN1250_Aacute	0xc1		/*  193		*/
#   define WIN1250_Acircumflex	0xc2		/*  194		*/
#   define WIN1250_Abreve	0xc3		/*  195		*/
#   define WIN1250_Adieresis	0xc4		/*  195		*/
#   define WIN1250_Lacute	0xc5		/*  197		*/
#   define WIN1250_Cacute	0xc6		/*  198		*/
#   define WIN1250_Ccedilla	0xc7		/*  199		*/
#   define WIN1250_Ccaron	0xc8		/*  200		*/
#   define WIN1250_Eacute	0xc9		/*  201		*/
#   define WIN1250_Eogonek	0xca		/*  202		*/
#   define WIN1250_Edieresis	0xcb		/*  202		*/
#   define WIN1250_Ecaron	0xcc		/*  204		*/
#   define WIN1250_Iacute	0xcd		/*  204		*/
#   define WIN1250_Icircumflex	0xce		/*  204		*/
#   define WIN1250_Dcaron	0xcf		/*  207		*/
#   define WIN1250_Dcroat	0xd0		/*  208		*/
#   define WIN1250_Nacute	0xd1		/*  209		*/
#   define WIN1250_Ncaron	0xd2		/*  210		*/
#   define WIN1250_Oacute	0xd3		/*  211		*/
#   define WIN1250_Ocircumflex	0xd4		/*  212		*/
#   define WIN1250_Ohungarumlaut 0xd5		/*  213		*/
#   define WIN1250_Odieresis	0xd6		/*  214		*/
#   define WIN1250_multiply	0xd7		/*  215		*/
#   define WIN1250_Rcaron	0xd8		/*  216		*/
#   define WIN1250_Uring	0xd9		/*  217		*/
#   define WIN1250_Uacute	0xda		/*  218		*/
#   define WIN1250_Uhungarumlaut 0xdb		/*  219		*/
#   define WIN1250_Udieresis	0xdc		/*  220		*/
#   define WIN1250_Yacute	0xdd		/*  221		*/
#   define WIN1250_Tcedilla	0xde		/*  222		*/
#   define WIN1250_germandbls	0xdf		/*  223		*/
#   define WIN1250_racute	0xe0		/*  224		*/
#   define WIN1250_aacute	0xe1		/*  225		*/
#   define WIN1250_acircumflex	0xe2		/*  226		*/
#   define WIN1250_abreve	0xe3		/*  227		*/
#   define WIN1250_adieresis	0xe4		/*  228		*/
#   define WIN1250_lacute	0xe5		/*  229		*/
#   define WIN1250_cacute	0xe6		/*  230		*/
#   define WIN1250_ccedilla	0xe7		/*  231		*/
#   define WIN1250_ccaron	0xe8		/*  232		*/
#   define WIN1250_eacute	0xe9		/*  233		*/
#   define WIN1250_eogonek	0xea		/*  234		*/
#   define WIN1250_edieresis	0xeb		/*  235		*/
#   define WIN1250_ecaron	0xec		/*  236		*/
#   define WIN1250_iacute	0xed		/*  237		*/
#   define WIN1250_icircumflex	0xee		/*  238		*/
#   define WIN1250_dcaron	0xef		/*  239		*/
#   define WIN1250_dcroat	0xf0		/*  240		*/
#   define WIN1250_nacute	0xf1		/*  241		*/
#   define WIN1250_ncaron	0xf2		/*  242		*/
#   define WIN1250_oacute	0xf3		/*  243		*/
#   define WIN1250_ocircumflex	0xf4		/*  244		*/
#   define WIN1250_ohungarumlaut 0xf5		/*  245		*/
#   define WIN1250_odieresis	0xf6		/*  246		*/
#   define WIN1250_divide	0xf7		/*  247		*/
#   define WIN1250_rcaron	0xf8		/*  248		*/
#   define WIN1250_uring	0xf9		/*  249		*/
#   define WIN1250_uacute	0xfa		/*  250		*/
#   define WIN1250_uhungarumlaut 0xfb		/*  251		*/
#   define WIN1250_udieresis	0xfc		/*  252		*/
#   define WIN1250_yacute	0xfd		/*  252		*/
#   define WIN1250_tcedilla	0xfe		/*  254		*/
#   define WIN1250_dotaccent	0xff		/*  255		*/

/************************************************************************/
/*									*/
/*  Names for the characters in the Windows 1253 code page.		*/
/*  (Greek)								*/
/*									*/
/************************************************************************/

#   define	WIN1253__0x00			0x00	/*    0		*/
#   define	WIN1253__0x01			0x01	/*    1		*/
#   define	WIN1253__0x02			0x02	/*    2		*/
#   define	WIN1253__0x03			0x03	/*    3		*/
#   define	WIN1253__0x04			0x04	/*    4		*/
#   define	WIN1253__0x05			0x05	/*    5		*/
#   define	WIN1253__0x06			0x06	/*    6		*/
#   define	WIN1253__0x07			0x07	/*    7		*/
#   define	WIN1253__0x08			0x08	/*    8		*/
#   define	WIN1253__0x09			0x09	/*    9		*/
#   define	WIN1253__0x0A			0x0a	/*   10		*/
#   define	WIN1253__0x0B			0x0b	/*   11		*/
#   define	WIN1253__0x0C			0x0c	/*   12		*/
#   define	WIN1253__0x0D			0x0d	/*   13		*/
#   define	WIN1253__0x0E			0x0e	/*   14		*/
#   define	WIN1253__0x0F			0x0f	/*   15		*/
#   define	WIN1253__0x10			0x10	/*   16		*/
#   define	WIN1253__0x11			0x11	/*   17		*/
#   define	WIN1253__0x12			0x12	/*   18		*/
#   define	WIN1253__0x13			0x13	/*   19		*/
#   define	WIN1253__0x14			0x14	/*   20		*/
#   define	WIN1253__0x15			0x15	/*   21		*/
#   define	WIN1253__0x16			0x16	/*   22		*/
#   define	WIN1253__0x17			0x17	/*   23		*/
#   define	WIN1253__0x18			0x18	/*   24		*/
#   define	WIN1253__0x19			0x19	/*   25		*/
#   define	WIN1253__0x1A			0x1a	/*   26		*/
#   define	WIN1253__0x1B			0x1b	/*   27		*/
#   define	WIN1253__0x1C			0x1c	/*   28		*/
#   define	WIN1253__0x1D			0x1d	/*   29		*/
#   define	WIN1253__0x1E			0x1e	/*   30		*/
#   define	WIN1253__0x1F			0x1f	/*   31		*/
#   define	WIN1253_space			0x20	/*   32		*/
#   define	WIN1253_exclam			0x21	/*   33		*/
#   define	WIN1253_quotedbl		0x22	/*   34		*/
#   define	WIN1253_numbersign		0x23	/*   35		*/
#   define	WIN1253_dollar			0x24	/*   36		*/
#   define	WIN1253_percent			0x25	/*   37		*/
#   define	WIN1253_ampersand		0x26	/*   38		*/
#   define	WIN1253_quotesingle		0x27	/*   39		*/
#   define	WIN1253_parenleft		0x28	/*   40		*/
#   define	WIN1253_parenright		0x29	/*   41		*/
#   define	WIN1253_asterisk		0x2a	/*   42		*/
#   define	WIN1253_plus			0x2b	/*   43		*/
#   define	WIN1253_comma			0x2c	/*   44		*/
#   define	WIN1253_minus			0x2d	/*   45		*/
#   define	WIN1253_period			0x2e	/*   46		*/
#   define	WIN1253_slash			0x2f	/*   47		*/
#   define	WIN1253_zero			0x30	/*   48		*/
#   define	WIN1253_one			0x31	/*   49		*/
#   define	WIN1253_two			0x32	/*   50		*/
#   define	WIN1253_three			0x33	/*   51		*/
#   define	WIN1253_four			0x34	/*   52		*/
#   define	WIN1253_five			0x35	/*   53		*/
#   define	WIN1253_six			0x36	/*   54		*/
#   define	WIN1253_seven			0x37	/*   55		*/
#   define	WIN1253_eight			0x38	/*   56		*/
#   define	WIN1253_nine			0x39	/*   57		*/
#   define	WIN1253_colon			0x3a	/*   58		*/
#   define	WIN1253_semicolon		0x3b	/*   59		*/
#   define	WIN1253_less			0x3c	/*   60		*/
#   define	WIN1253_equal			0x3d	/*   61		*/
#   define	WIN1253_greater			0x3e	/*   62		*/
#   define	WIN1253_question		0x3f	/*   63		*/
#   define	WIN1253_at			0x40	/*   64		*/
#   define	WIN1253_A			0x41	/*   65		*/
#   define	WIN1253_B			0x42	/*   66		*/
#   define	WIN1253_C			0x43	/*   67		*/
#   define	WIN1253_D			0x44	/*   68		*/
#   define	WIN1253_E			0x45	/*   69		*/
#   define	WIN1253_F			0x46	/*   70		*/
#   define	WIN1253_G			0x47	/*   71		*/
#   define	WIN1253_H			0x48	/*   72		*/
#   define	WIN1253_I			0x49	/*   73		*/
#   define	WIN1253_J			0x4a	/*   74		*/
#   define	WIN1253_K			0x4b	/*   75		*/
#   define	WIN1253_L			0x4c	/*   76		*/
#   define	WIN1253_M			0x4d	/*   77		*/
#   define	WIN1253_N			0x4e	/*   78		*/
#   define	WIN1253_O			0x4f	/*   79		*/
#   define	WIN1253_P			0x50	/*   80		*/
#   define	WIN1253_Q			0x51	/*   81		*/
#   define	WIN1253_R			0x52	/*   82		*/
#   define	WIN1253_S			0x53	/*   83		*/
#   define	WIN1253_T			0x54	/*   84		*/
#   define	WIN1253_U			0x55	/*   85		*/
#   define	WIN1253_V			0x56	/*   86		*/
#   define	WIN1253_W			0x57	/*   87		*/
#   define	WIN1253_X			0x58	/*   88		*/
#   define	WIN1253_Y			0x59	/*   89		*/
#   define	WIN1253_Z			0x5a	/*   90		*/
#   define	WIN1253_bracketleft		0x5b	/*   91		*/
#   define	WIN1253_backslash		0x5c	/*   92		*/
#   define	WIN1253_bracketright		0x5d	/*   93		*/
#   define	WIN1253_asciicircum		0x5e	/*   94		*/
#   define	WIN1253_underscore		0x5f	/*   95		*/
#   define	WIN1253_quoteleft		0x60	/*   96		*/
#   define	WIN1253_a			0x61	/*   97		*/
#   define	WIN1253_b			0x62	/*   98		*/
#   define	WIN1253_c			0x63	/*   99		*/
#   define	WIN1253_d			0x64	/*  100		*/
#   define	WIN1253_e			0x65	/*  101		*/
#   define	WIN1253_f			0x66	/*  102		*/
#   define	WIN1253_g			0x67	/*  103		*/
#   define	WIN1253_h			0x68	/*  104		*/
#   define	WIN1253_i			0x69	/*  105		*/
#   define	WIN1253_j			0x6a	/*  106		*/
#   define	WIN1253_k			0x6b	/*  107		*/
#   define	WIN1253_l			0x6c	/*  108		*/
#   define	WIN1253_m			0x6d	/*  109		*/
#   define	WIN1253_n			0x6e	/*  110		*/
#   define	WIN1253_o			0x6f	/*  111		*/
#   define	WIN1253_p			0x70	/*  112		*/
#   define	WIN1253_q			0x71	/*  113		*/
#   define	WIN1253_r			0x72	/*  114		*/
#   define	WIN1253_s			0x73	/*  115		*/
#   define	WIN1253_t			0x74	/*  116		*/
#   define	WIN1253_u			0x75	/*  117		*/
#   define	WIN1253_v			0x76	/*  118		*/
#   define	WIN1253_w			0x77	/*  119		*/
#   define	WIN1253_x			0x78	/*  120		*/
#   define	WIN1253_y			0x79	/*  121		*/
#   define	WIN1253_z			0x7a	/*  122		*/
#   define	WIN1253_braceleft		0x7b	/*  123		*/
#   define	WIN1253_bar			0x7c	/*  124		*/
#   define	WIN1253_braceright		0x7d	/*  125		*/
#   define	WIN1253_asciitilde		0x7e	/*  126		*/
#   define	WIN1253__0x7F			0x7f	/*  127		*/
#   define	WIN1253__0x80			0x80	/*  128		*/
#   define	WIN1253__0x81			0x81	/*  129		*/
#   define	WIN1253_quotesinglbase		0x82	/*  130		*/
#   define	WIN1253_florin			0x83	/*  131		*/
#   define	WIN1253_quotedblbase		0x84	/*  132		*/
#   define	WIN1253_ellipsis		0x85	/*  133		*/
#   define	WIN1253_dagger			0x86	/*  134		*/
#   define	WIN1253_daggerdbl		0x87	/*  135		*/
#   define	WIN1253__0x88			0x88	/*  136		*/
#   define	WIN1253_perthousand		0x89	/*  137		*/
#   define	WIN1253__0x8A			0x8a	/*  138		*/
#   define	WIN1253_guilsinglleft		0x8b	/*  139		*/
#   define	WIN1253__0x8C			0x8c	/*  140		*/
#   define	WIN1253__0x8D			0x8d	/*  141		*/
#   define	WIN1253__0x8E			0x8e	/*  142		*/
#   define	WIN1253__0x8F			0x8f	/*  143		*/
#   define	WIN1253__0x90			0x90	/*  144		*/
#   define	WIN1253__quoteleft		0x91	/*  145		*/
#   define	WIN1253_quoteright		0x92	/*  146		*/
#   define	WIN1253_quotedblleft		0x93	/*  147		*/
#   define	WIN1253_quotedblright		0x94	/*  148		*/
#   define	WIN1253_bullet			0x95	/*  149		*/
#   define	WIN1253_endash			0x96	/*  150		*/
#   define	WIN1253_emdash			0x97	/*  151		*/
#   define	WIN1253__0x98			0x98	/*  152		*/
#   define	WIN1253_trademark		0x99	/*  153		*/
#   define	WIN1253__0x9A			0x9a	/*  154		*/
#   define	WIN1253_guilsinglright		0x9b	/*  155		*/
#   define	WIN1253__0x9C			0x9c	/*  156		*/
#   define	WIN1253__0x9D			0x9d	/*  157		*/
#   define	WIN1253__0x9E			0x9e	/*  158		*/
#   define	WIN1253__0x9F			0x9f	/*  159		*/
#   define	WIN1253_nobreakspace		0xa0	/*  160		*/
#   define	WIN1253_dieresistonos		0xa1	/*  161		*/
#   define	WIN1253_Alphatonos		0xa2	/*  162		*/
#   define	WIN1253_sterling		0xa3	/*  163		*/
#   define	WIN1253_currency		0xa4	/*  164		*/
#   define	WIN1253_yen			0xa5	/*  165		*/
#   define	WIN1253_brokenbar		0xa6	/*  166		*/
#   define	WIN1253_section			0xa7	/*  167		*/
#   define	WIN1253_dieresis		0xa8	/*  168		*/
#   define	WIN1253_copyright		0xa9	/*  169		*/
#   define	WIN1253__0xAA			0xaa	/*  170		*/
#   define	WIN1253_guillemotleft		0xab	/*  171		*/
#   define	WIN1253_logicalnot		0xac	/*  172		*/
#   define	WIN1253_hyphen			0xad	/*  173		*/
#   define	WIN1253_registered		0xae	/*  174		*/
#   define	WIN1253_afii00208		0xaf	/*  175		*/
#   define	WIN1253_degree			0xb0	/*  176		*/
#   define	WIN1253_plusminus		0xb1	/*  177		*/
#   define	WIN1253_twosuperior		0xb2	/*  178		*/
#   define	WIN1253_threesuperior		0xb3	/*  179		*/
#   define	WIN1253_tonos			0xb4	/*  180		*/
#   define	WIN1253__mu			0xb5	/*  181		*/
#   define	WIN1253_paragraph		0xb6	/*  182		*/
#   define	WIN1253_periodcentered		0xb7	/*  183		*/
#   define	WIN1253_Epsilontonos		0xb8	/*  184		*/
#   define	WIN1253_Etatonos		0xb9	/*  185		*/
#   define	WIN1253_Iotatonos		0xba	/*  186		*/
#   define	WIN1253_guillemotright		0xbb	/*  187		*/
#   define	WIN1253_Omicrontonos		0xbc	/*  188		*/
#   define	WIN1253_onehalf			0xbd	/*  189		*/
#   define	WIN1253_Upsilontonos		0xbe	/*  190		*/
#   define	WIN1253_Omegatonos		0xbf	/*  191		*/
#   define	WIN1253_iotadieresistonos	0xc0	/*  192		*/
#   define	WIN1253_Alpha			0xc1	/*  193		*/
#   define	WIN1253_Beta			0xc2	/*  194		*/
#   define	WIN1253_Gamma			0xc3	/*  195		*/
#   define	WIN1253_Delta			0xc4	/*  196		*/
#   define	WIN1253_Epsilon			0xc5	/*  197		*/
#   define	WIN1253_Zeta			0xc6	/*  198		*/
#   define	WIN1253_Eta			0xc7	/*  199		*/
#   define	WIN1253_Theta			0xc8	/*  200		*/
#   define	WIN1253_Iota			0xc9	/*  201		*/
#   define	WIN1253_Kappa			0xca	/*  202		*/
#   define	WIN1253_Lambda			0xcb	/*  203		*/
#   define	WIN1253_Mu			0xcc	/*  204		*/
#   define	WIN1253_Nu			0xcd	/*  205		*/
#   define	WIN1253_Xi			0xce	/*  206		*/
#   define	WIN1253_Omicron			0xcf	/*  207		*/
#   define	WIN1253_Pi			0xd0	/*  208		*/
#   define	WIN1253_Rho			0xd1	/*  209		*/
#   define	WIN1253__0xD2			0xd2	/*  210		*/
#   define	WIN1253_Sigma			0xd3	/*  211		*/
#   define	WIN1253_Tau			0xd4	/*  212		*/
#   define	WIN1253_Upsilon			0xd5	/*  213		*/
#   define	WIN1253_Phi			0xd6	/*  214		*/
#   define	WIN1253_Chi			0xd7	/*  215		*/
#   define	WIN1253_Psi			0xd8	/*  216		*/
#   define	WIN1253_Omega			0xd9	/*  217		*/
#   define	WIN1253_Iotadieresis		0xda	/*  218		*/
#   define	WIN1253_Upsilondieresis		0xdb	/*  219		*/
#   define	WIN1253_alphatonos		0xdc	/*  220		*/
#   define	WIN1253_epsilontonos		0xdd	/*  221		*/
#   define	WIN1253_etatonos		0xde	/*  222		*/
#   define	WIN1253_iotatonos		0xdf	/*  223		*/
#   define	WIN1253_upsilondieresistonos	0xe0	/*  224		*/
#   define	WIN1253_alpha			0xe1	/*  225		*/
#   define	WIN1253_beta			0xe2	/*  226		*/
#   define	WIN1253_gamma			0xe3	/*  227		*/
#   define	WIN1253_delta			0xe4	/*  228		*/
#   define	WIN1253_epsilon			0xe5	/*  229		*/
#   define	WIN1253_zeta			0xe6	/*  230		*/
#   define	WIN1253_eta			0xe7	/*  231		*/
#   define	WIN1253_theta			0xe8	/*  232		*/
#   define	WIN1253_iota			0xe9	/*  233		*/
#   define	WIN1253_kappa			0xea	/*  234		*/
#   define	WIN1253_lambda			0xeb	/*  235		*/
#   define	WIN1253_mu			0xec	/*  236		*/
#   define	WIN1253_nu			0xed	/*  237		*/
#   define	WIN1253_xi			0xee	/*  238		*/
#   define	WIN1253_omicron			0xef	/*  239		*/
#   define	WIN1253_pi			0xf0	/*  240		*/
#   define	WIN1253_rho			0xf1	/*  241		*/
#   define	WIN1253_sigma1			0xf2	/*  242		*/
#   define	WIN1253_sigma			0xf3	/*  243		*/
#   define	WIN1253_tau			0xf4	/*  244		*/
#   define	WIN1253_upsilon			0xf5	/*  245		*/
#   define	WIN1253_phi			0xf6	/*  246		*/
#   define	WIN1253_chi			0xf7	/*  247		*/
#   define	WIN1253_psi			0xf8	/*  248		*/
#   define	WIN1253_omega			0xf9	/*  249		*/
#   define	WIN1253_iotadieresis		0xfa	/*  250		*/
#   define	WIN1253_upsilondieresis		0xfb	/*  251		*/
#   define	WIN1253_omicrontonos		0xfc	/*  252		*/
#   define	WIN1253_upsilontonos		0xfd	/*  253		*/
#   define	WIN1253_omegatonos		0xfe	/*  254		*/

/************************************************************************/
/*									*/
/*  Names for the characters in the ITC Zapf Dingbats collection.	*/
/*									*/
/************************************************************************/

#   define	DINGBATS__0x00	0x00	/*   0  */
#   define	DINGBATS__0x01	0x01	/*   1  */
#   define	DINGBATS__0x02	0x02	/*   2  */
#   define	DINGBATS__0x03	0x03	/*   3  */
#   define	DINGBATS__0x04	0x04	/*   4  */
#   define	DINGBATS__0x05	0x05	/*   5  */
#   define	DINGBATS__0x06	0x06	/*   6  */
#   define	DINGBATS__0x07	0x07	/*   7  */
#   define	DINGBATS__0x08	0x08	/*   8  */
#   define	DINGBATS__0x09	0x09	/*   9  */
#   define	DINGBATS__0x0a	0x0a	/*  10  */
#   define	DINGBATS__0x0b	0x0b	/*  11  */
#   define	DINGBATS__0x0c	0x0c	/*  12  */
#   define	DINGBATS__0x0d	0x0d	/*  13  */
#   define	DINGBATS__0x0e	0x0e	/*  14  */
#   define	DINGBATS__0x0f	0x0f	/*  15  */
#   define	DINGBATS__0x10	0x10	/*  16  */
#   define	DINGBATS__0x11	0x11	/*  17  */
#   define	DINGBATS__0x12	0x12	/*  18  */
#   define	DINGBATS__0x13	0x13	/*  19  */
#   define	DINGBATS__0x14	0x14	/*  20  */
#   define	DINGBATS__0x15	0x15	/*  21  */
#   define	DINGBATS__0x16	0x16	/*  22  */
#   define	DINGBATS__0x17	0x17	/*  23  */
#   define	DINGBATS__0x18	0x18	/*  24  */
#   define	DINGBATS__0x19	0x19	/*  25  */
#   define	DINGBATS__0x1a	0x1a	/*  26  */
#   define	DINGBATS__0x1b	0x1b	/*  27  */
#   define	DINGBATS__0x1c	0x1c	/*  28  */
#   define	DINGBATS__0x1d	0x1d	/*  29  */
#   define	DINGBATS__0x1e	0x1e	/*  30  */
#   define	DINGBATS__0x1f	0x1f	/*  31  */
#   define	DINGBATS_space	0x20	/*  32  */
#   define	DINGBATS_a1	0x21	/*  33  */
#   define	DINGBATS_a2	0x22	/*  34  */
#   define	DINGBATS_a202	0x23	/*  35  */
#   define	DINGBATS_a3	0x24	/*  36  */
#   define	DINGBATS_a4	0x25	/*  37  */
#   define	DINGBATS_a5	0x26	/*  38  */
#   define	DINGBATS_a119	0x27	/*  39  */
#   define	DINGBATS_a118	0x28	/*  40  */
#   define	DINGBATS_a117	0x29	/*  41  */
#   define	DINGBATS_a11	0x2a	/*  42  */
#   define	DINGBATS_a12	0x2b	/*  43  */
#   define	DINGBATS_a13	0x2c	/*  44  */
#   define	DINGBATS_a14	0x2d	/*  45  */
#   define	DINGBATS_a15	0x2e	/*  46  */
#   define	DINGBATS_a16	0x2f	/*  47  */
#   define	DINGBATS_a105	0x30	/*  48  */
#   define	DINGBATS_a17	0x31	/*  49  */
#   define	DINGBATS_a18	0x32	/*  50  */
#   define	DINGBATS_a19	0x33	/*  51  */
#   define	DINGBATS_a20	0x34	/*  52  */
#   define	DINGBATS_a21	0x35	/*  53  */
#   define	DINGBATS_a22	0x36	/*  54  */
#   define	DINGBATS_a23	0x37	/*  55  */
#   define	DINGBATS_a24	0x38	/*  56  */
#   define	DINGBATS_a25	0x39	/*  57  */
#   define	DINGBATS_a26	0x3a	/*  58  */
#   define	DINGBATS_a27	0x3b	/*  59  */
#   define	DINGBATS_a28	0x3c	/*  60  */
#   define	DINGBATS_a6	0x3d	/*  61  */
#   define	DINGBATS_a7	0x3e	/*  62  */
#   define	DINGBATS_a8	0x3f	/*  63  */
#   define	DINGBATS_a9	0x40	/*  64  */
#   define	DINGBATS_a10	0x41	/*  65  */
#   define	DINGBATS_a29	0x42	/*  66  */
#   define	DINGBATS_a30	0x43	/*  67  */
#   define	DINGBATS_a31	0x44	/*  68  */
#   define	DINGBATS_a32	0x45	/*  69  */
#   define	DINGBATS_a33	0x46	/*  70  */
#   define	DINGBATS_a34	0x47	/*  71  */
#   define	DINGBATS_a35	0x48	/*  72  */
#   define	DINGBATS_a36	0x49	/*  73  */
#   define	DINGBATS_a37	0x4a	/*  74  */
#   define	DINGBATS_a38	0x4b	/*  75  */
#   define	DINGBATS_a39	0x4c	/*  76  */
#   define	DINGBATS_a40	0x4d	/*  77  */
#   define	DINGBATS_a41	0x4e	/*  78  */
#   define	DINGBATS_a42	0x4f	/*  79  */
#   define	DINGBATS_a43	0x50	/*  80  */
#   define	DINGBATS_a44	0x51	/*  81  */
#   define	DINGBATS_a45	0x52	/*  82  */
#   define	DINGBATS_a46	0x53	/*  83  */
#   define	DINGBATS_a47	0x54	/*  84  */
#   define	DINGBATS_a48	0x55	/*  85  */
#   define	DINGBATS_a49	0x56	/*  86  */
#   define	DINGBATS_a50	0x57	/*  87  */
#   define	DINGBATS_a51	0x58	/*  88  */
#   define	DINGBATS_a52	0x59	/*  89  */
#   define	DINGBATS_a53	0x5a	/*  90  */
#   define	DINGBATS_a54	0x5b	/*  91  */
#   define	DINGBATS_a55	0x5c	/*  92  */
#   define	DINGBATS_a56	0x5d	/*  93  */
#   define	DINGBATS_a57	0x5e	/*  94  */
#   define	DINGBATS_a58	0x5f	/*  95  */
#   define	DINGBATS_a59	0x60	/*  96  */
#   define	DINGBATS_a60	0x61	/*  97  */
#   define	DINGBATS_a61	0x62	/*  98  */
#   define	DINGBATS_a62	0x63	/*  99  */
#   define	DINGBATS_a63	0x64	/* 100  */
#   define	DINGBATS_a64	0x65	/* 101  */
#   define	DINGBATS_a65	0x66	/* 102  */
#   define	DINGBATS_a66	0x67	/* 103  */
#   define	DINGBATS_a67	0x68	/* 104  */
#   define	DINGBATS_a68	0x69	/* 105  */
#   define	DINGBATS_a69	0x6a	/* 106  */
#   define	DINGBATS_a70	0x6b	/* 107  */
#   define	DINGBATS_a71	0x6c	/* 108  */
#   define	DINGBATS_a72	0x6d	/* 109  */
#   define	DINGBATS_a73	0x6e	/* 110  */
#   define	DINGBATS_a74	0x6f	/* 111  */
#   define	DINGBATS_a203	0x70	/* 112  */
#   define	DINGBATS_a75	0x71	/* 113  */
#   define	DINGBATS_a204	0x72	/* 114  */
#   define	DINGBATS_a76	0x73	/* 115  */
#   define	DINGBATS_a77	0x74	/* 116  */
#   define	DINGBATS_a78	0x75	/* 117  */
#   define	DINGBATS_a79	0x76	/* 118  */
#   define	DINGBATS_a81	0x77	/* 119  */
#   define	DINGBATS_a82	0x78	/* 120  */
#   define	DINGBATS_a83	0x79	/* 121  */
#   define	DINGBATS_a84	0x7a	/* 122  */
#   define	DINGBATS_a97	0x7b	/* 123  */
#   define	DINGBATS_a98	0x7c	/* 124  */
#   define	DINGBATS_a99	0x7d	/* 125  */
#   define	DINGBATS_a100	0x7e	/* 126  */
#   define	DINGBATS__0x7f	0x7f	/* 127  */
#   define	DINGBATS__0x80	0x80	/* 128  */
#   define	DINGBATS__0x81	0x81	/* 129  */
#   define	DINGBATS__0x82	0x82	/* 130  */
#   define	DINGBATS__0x83	0x83	/* 131  */
#   define	DINGBATS__0x84	0x84	/* 132  */
#   define	DINGBATS__0x85	0x85	/* 133  */
#   define	DINGBATS__0x86	0x86	/* 134  */
#   define	DINGBATS__0x87	0x87	/* 135  */
#   define	DINGBATS__0x88	0x88	/* 136  */
#   define	DINGBATS__0x89	0x89	/* 137  */
#   define	DINGBATS__0x8a	0x8a	/* 138  */
#   define	DINGBATS__0x8b	0x8b	/* 139  */
#   define	DINGBATS__0x8c	0x8c	/* 140  */
#   define	DINGBATS__0x8d	0x8d	/* 141  */
#   define	DINGBATS__0x8e	0x8e	/* 142  */
#   define	DINGBATS__0x8f	0x8f	/* 143  */
#   define	DINGBATS__0x90	0x90	/* 144  */
#   define	DINGBATS__0x91	0x91	/* 145  */
#   define	DINGBATS__0x92	0x92	/* 146  */
#   define	DINGBATS__0x93	0x93	/* 147  */
#   define	DINGBATS__0x94	0x94	/* 148  */
#   define	DINGBATS__0x95	0x95	/* 149  */
#   define	DINGBATS__0x96	0x96	/* 150  */
#   define	DINGBATS__0x97	0x97	/* 151  */
#   define	DINGBATS__0x98	0x98	/* 152  */
#   define	DINGBATS__0x99	0x99	/* 153  */
#   define	DINGBATS__0x9a	0x9a	/* 154  */
#   define	DINGBATS__0x9b	0x9b	/* 155  */
#   define	DINGBATS__0x9c	0x9c	/* 156  */
#   define	DINGBATS__0x9d	0x9d	/* 157  */
#   define	DINGBATS__0x9e	0x9e	/* 158  */
#   define	DINGBATS__0x9f	0x9f	/* 159  */
#   define	DINGBATS__0xa0	0xa0	/* 160  */
#   define	DINGBATS_a101	0xa1	/* 161  */
#   define	DINGBATS_a102	0xa2	/* 162  */
#   define	DINGBATS_a103	0xa3	/* 163  */
#   define	DINGBATS_a104	0xa4	/* 164  */
#   define	DINGBATS_a106	0xa5	/* 165  */
#   define	DINGBATS_a107	0xa6	/* 166  */
#   define	DINGBATS_a108	0xa7	/* 167  */
#   define	DINGBATS_a112	0xa8	/* 168  */
#   define	DINGBATS_a111	0xa9	/* 169  */
#   define	DINGBATS_a110	0xaa	/* 170  */
#   define	DINGBATS_a109	0xab	/* 171  */
#   define	DINGBATS_a120	0xac	/* 172  */
#   define	DINGBATS_a121	0xad	/* 173  */
#   define	DINGBATS_a122	0xae	/* 174  */
#   define	DINGBATS_a123	0xaf	/* 175  */
#   define	DINGBATS_a124	0xb0	/* 176  */
#   define	DINGBATS_a125	0xb1	/* 177  */
#   define	DINGBATS_a126	0xb2	/* 178  */
#   define	DINGBATS_a127	0xb3	/* 179  */
#   define	DINGBATS_a128	0xb4	/* 180  */
#   define	DINGBATS_a129	0xb5	/* 181  */
#   define	DINGBATS_a130	0xb6	/* 182  */
#   define	DINGBATS_a131	0xb7	/* 183  */
#   define	DINGBATS_a132	0xb8	/* 184  */
#   define	DINGBATS_a133	0xb9	/* 185  */
#   define	DINGBATS_a134	0xba	/* 186  */
#   define	DINGBATS_a135	0xbb	/* 187  */
#   define	DINGBATS_a136	0xbc	/* 188  */
#   define	DINGBATS_a137	0xbd	/* 189  */
#   define	DINGBATS_a138	0xbe	/* 190  */
#   define	DINGBATS_a139	0xbf	/* 191  */
#   define	DINGBATS_a140	0xc0	/* 192  */
#   define	DINGBATS_a141	0xc1	/* 193  */
#   define	DINGBATS_a142	0xc2	/* 194  */
#   define	DINGBATS_a143	0xc3	/* 195  */
#   define	DINGBATS_a144	0xc4	/* 196  */
#   define	DINGBATS_a145	0xc5	/* 197  */
#   define	DINGBATS_a146	0xc6	/* 198  */
#   define	DINGBATS_a147	0xc7	/* 199  */
#   define	DINGBATS_a148	0xc8	/* 200  */
#   define	DINGBATS_a149	0xc9	/* 201  */
#   define	DINGBATS_a150	0xca	/* 202  */
#   define	DINGBATS_a151	0xcb	/* 203  */
#   define	DINGBATS_a152	0xcc	/* 204  */
#   define	DINGBATS_a153	0xcd	/* 205  */
#   define	DINGBATS_a154	0xce	/* 206  */
#   define	DINGBATS_a155	0xcf	/* 207  */
#   define	DINGBATS_a156	0xd0	/* 208  */
#   define	DINGBATS_a157	0xd1	/* 209  */
#   define	DINGBATS_a158	0xd2	/* 210  */
#   define	DINGBATS_a159	0xd3	/* 211  */
#   define	DINGBATS_a160	0xd4	/* 212  */
#   define	DINGBATS_a161	0xd5	/* 213  */
#   define	DINGBATS_a163	0xd6	/* 214  */
#   define	DINGBATS_a164	0xd7	/* 215  */
#   define	DINGBATS_a196	0xd8	/* 216  */
#   define	DINGBATS_a165	0xd9	/* 217  */
#   define	DINGBATS_a192	0xda	/* 218  */
#   define	DINGBATS_a166	0xdb	/* 219  */
#   define	DINGBATS_a167	0xdc	/* 220  */
#   define	DINGBATS_a168	0xdd	/* 221  */
#   define	DINGBATS_a169	0xde	/* 222  */
#   define	DINGBATS_a170	0xdf	/* 223  */
#   define	DINGBATS_a171	0xe0	/* 224  */
#   define	DINGBATS_a172	0xe1	/* 225  */
#   define	DINGBATS_a173	0xe2	/* 226  */
#   define	DINGBATS_a162	0xe3	/* 227  */
#   define	DINGBATS_a174	0xe4	/* 228  */
#   define	DINGBATS_a175	0xe5	/* 229  */
#   define	DINGBATS_a176	0xe6	/* 230  */
#   define	DINGBATS_a177	0xe7	/* 231  */
#   define	DINGBATS_a178	0xe8	/* 232  */
#   define	DINGBATS_a179	0xe9	/* 233  */
#   define	DINGBATS_a193	0xea	/* 234  */
#   define	DINGBATS_a180	0xeb	/* 235  */
#   define	DINGBATS_a199	0xec	/* 236  */
#   define	DINGBATS_a181	0xed	/* 237  */
#   define	DINGBATS_a200	0xee	/* 238  */
#   define	DINGBATS_a182	0xef	/* 239  */
#   define	DINGBATS__0xf0	0xf0	/* 240  */
#   define	DINGBATS_a201	0xf1	/* 241  */
#   define	DINGBATS_a183	0xf2	/* 242  */
#   define	DINGBATS_a184	0xf3	/* 243  */
#   define	DINGBATS_a197	0xf4	/* 244  */
#   define	DINGBATS_a185	0xf5	/* 245  */
#   define	DINGBATS_a194	0xf6	/* 246  */
#   define	DINGBATS_a198	0xf7	/* 247  */
#   define	DINGBATS_a186	0xf8	/* 248  */
#   define	DINGBATS_a195	0xf9	/* 249  */
#   define	DINGBATS_a187	0xfa	/* 250  */
#   define	DINGBATS_a188	0xfb	/* 251  */
#   define	DINGBATS_a189	0xfc	/* 252  */
#   define	DINGBATS_a190	0xfd	/* 253  */
#   define	DINGBATS_a191	0xfe	/* 254  */
#   define	DINGBATS__0xff	0xff	/* 255  */

/************************************************************************/
/*									*/
/*  Latin 5: Turkish (ISO 8859-9)					*/
/*									*/
/************************************************************************/

#   define	ISO9_space		0x20	/*   32		*/
#   define	ISO9_exclam		0x21	/*   33		*/
#   define	ISO9_quotedbl		0x22	/*   34		*/
#   define	ISO9_numbersign		0x23	/*   35		*/
#   define	ISO9_dollar		0x24	/*   36		*/
#   define	ISO9_percent		0x25	/*   37		*/
#   define	ISO9_ampersand		0x26	/*   38		*/
#   define	ISO9_quotesingle	0x27	/*   39		*/
#   define	ISO9_parenleft		0x28	/*   40		*/
#   define	ISO9_parenright		0x29	/*   41		*/
#   define	ISO9_asterisk		0x2a	/*   42		*/
#   define	ISO9_plus		0x2b	/*   43		*/
#   define	ISO9_comma		0x2c	/*   44		*/
#   define	ISO9_minus		0x2d	/*   45		*/
#   define	ISO9_period		0x2e	/*   46		*/
#   define	ISO9_slash		0x2f	/*   47		*/
#   define	ISO9_zero		0x30	/*   48		*/
#   define	ISO9_one		0x31	/*   49		*/
#   define	ISO9_two		0x32	/*   50		*/
#   define	ISO9_three		0x33	/*   51		*/
#   define	ISO9_four		0x34	/*   52		*/
#   define	ISO9_five		0x35	/*   53		*/
#   define	ISO9_six		0x36	/*   54		*/
#   define	ISO9_seven		0x37	/*   55		*/
#   define	ISO9_eight		0x38	/*   56		*/
#   define	ISO9_nine		0x39	/*   57		*/
#   define	ISO9_colon		0x3a	/*   58		*/
#   define	ISO9_semicolon		0x3b	/*   59		*/
#   define	ISO9_less		0x3c	/*   60		*/
#   define	ISO9_equal		0x3d	/*   61		*/
#   define	ISO9_greater		0x3e	/*   62		*/
#   define	ISO9_question		0x3f	/*   63		*/
#   define	ISO9_at			0x40	/*   64		*/
#   define	ISO9_A			0x41	/*   65		*/
#   define	ISO9_B			0x42	/*   66		*/
#   define	ISO9_C			0x43	/*   67		*/
#   define	ISO9_D			0x44	/*   68		*/
#   define	ISO9_E			0x45	/*   69		*/
#   define	ISO9_F			0x46	/*   70		*/
#   define	ISO9_G			0x47	/*   71		*/
#   define	ISO9_H			0x48	/*   72		*/
#   define	ISO9_I			0x49	/*   73		*/
#   define	ISO9_J			0x4a	/*   74		*/
#   define	ISO9_K			0x4b	/*   75		*/
#   define	ISO9_L			0x4c	/*   76		*/
#   define	ISO9_M			0x4d	/*   77		*/
#   define	ISO9_N			0x4e	/*   78		*/
#   define	ISO9_O			0x4f	/*   79		*/
#   define	ISO9_P			0x50	/*   80		*/
#   define	ISO9_Q			0x51	/*   81		*/
#   define	ISO9_R			0x52	/*   82		*/
#   define	ISO9_S			0x53	/*   83		*/
#   define	ISO9_T			0x54	/*   84		*/
#   define	ISO9_U			0x55	/*   85		*/
#   define	ISO9_V			0x56	/*   86		*/
#   define	ISO9_W			0x57	/*   87		*/
#   define	ISO9_X			0x58	/*   88		*/
#   define	ISO9_Y			0x59	/*   89		*/
#   define	ISO9_Z			0x5a	/*   90		*/
#   define	ISO9_bracketleft	0x5b	/*   91		*/
#   define	ISO9_backslash		0x5c	/*   92		*/
#   define	ISO9_bracketright	0x5d	/*   93		*/
#   define	ISO9_asciicircum	0x5e	/*   94		*/
#   define	ISO9_underscore		0x5f	/*   95		*/
#   define	ISO9_grave		0x60	/*   96		*/
#   define	ISO9_a			0x61	/*   97		*/
#   define	ISO9_b			0x62	/*   98		*/
#   define	ISO9_c			0x63	/*   99		*/
#   define	ISO9_d			0x64	/*  100		*/
#   define	ISO9_e			0x65	/*  101		*/
#   define	ISO9_f			0x66	/*  102		*/
#   define	ISO9_g			0x67	/*  103		*/
#   define	ISO9_h			0x68	/*  104		*/
#   define	ISO9_i			0x69	/*  105		*/
#   define	ISO9_j			0x6a	/*  106		*/
#   define	ISO9_k			0x6b	/*  107		*/
#   define	ISO9_l			0x6c	/*  108		*/
#   define	ISO9_m			0x6d	/*  109		*/
#   define	ISO9_n			0x6e	/*  110		*/
#   define	ISO9_o			0x6f	/*  111		*/
#   define	ISO9_p			0x70	/*  112		*/
#   define	ISO9_q			0x71	/*  113		*/
#   define	ISO9_r			0x72	/*  114		*/
#   define	ISO9_s			0x73	/*  115		*/
#   define	ISO9_t			0x74	/*  116		*/
#   define	ISO9_u			0x75	/*  117		*/
#   define	ISO9_v			0x76	/*  118		*/
#   define	ISO9_w			0x77	/*  119		*/
#   define	ISO9_x			0x78	/*  120		*/
#   define	ISO9_y			0x79	/*  121		*/
#   define	ISO9_z			0x7a	/*  122		*/
#   define	ISO9_braceleft		0x7b	/*  123		*/
#   define	ISO9_bar		0x7c	/*  124		*/
#   define	ISO9_braceright		0x7d	/*  125		*/
#   define	ISO9_asciitilde		0x7e	/*  126		*/
#   define	ISO9_nobreakspace	0xa0	/*  160		*/
#   define	ISO9_exclamdown		0xa1	/*  161		*/
#   define	ISO9_cent		0xa2	/*  162		*/
#   define	ISO9_sterling		0xa3	/*  163		*/
#   define	ISO9_currency		0xa4	/*  164		*/
#   define	ISO9_yen		0xa5	/*  165		*/
#   define	ISO9_brokenbar		0xa6	/*  166		*/
#   define	ISO9_section		0xa7	/*  167		*/
#   define	ISO9_dieresis		0xa8	/*  168		*/
#   define	ISO9_copyright		0xa9	/*  169		*/
#   define	ISO9_ordfeminine	0xaa	/*  170		*/
#   define	ISO9_guillemotleft	0xab	/*  171		*/
#   define	ISO9_logicalnot		0xac	/*  172		*/
#   define	ISO9_hyphen		0xad	/*  173		*/
#   define	ISO9_registered		0xae	/*  174		*/
#   define	ISO9_macron		0xaf	/*  175		*/
#   define	ISO9_degree		0xb0	/*  176		*/
#   define	ISO9_plusminus		0xb1	/*  177		*/
#   define	ISO9_twosuperior	0xb2	/*  178		*/
#   define	ISO9_threesuperior	0xb3	/*  179		*/
#   define	ISO9_acute		0xb4	/*  180		*/
#   define	ISO9_mu			0xb5	/*  181		*/
#   define	ISO9_paragraph		0xb6	/*  182		*/
#   define	ISO9_periodcentered	0xb7	/*  183		*/
#   define	ISO9_cedilla		0xb8	/*  184		*/
#   define	ISO9_onesuperior	0xb9	/*  185		*/
#   define	ISO9_ordmasculine	0xba	/*  186		*/
#   define	ISO9_guillemotright	0xbb	/*  187		*/
#   define	ISO9_onequarter		0xbc	/*  188		*/
#   define	ISO9_onehalf		0xbd	/*  189		*/
#   define	ISO9_threequarters	0xbe	/*  190		*/
#   define	ISO9_questiondown	0xbf	/*  191		*/
#   define	ISO9_Agrave		0xc0	/*  192		*/
#   define	ISO9_Aacute		0xc1	/*  193		*/
#   define	ISO9_Acircumflex	0xc2	/*  194		*/
#   define	ISO9_Atilde		0xc3	/*  195		*/
#   define	ISO9_Adieresis		0xc4	/*  196		*/
#   define	ISO9_Aring		0xc5	/*  197		*/
#   define	ISO9_AE			0xc6	/*  198		*/
#   define	ISO9_Ccedilla		0xc7	/*  199		*/
#   define	ISO9_Egrave		0xc8	/*  200		*/
#   define	ISO9_Eacute		0xc9	/*  201		*/
#   define	ISO9_Ecircumflex	0xca	/*  202		*/
#   define	ISO9_Edieresis		0xcb	/*  203		*/
#   define	ISO9_Igrave		0xcc	/*  204		*/
#   define	ISO9_Iacute		0xcd	/*  205		*/
#   define	ISO9_Icircumflex	0xce	/*  206		*/
#   define	ISO9_Idieresis		0xcf	/*  207		*/
#   define	ISO9_Gbreve		0xd0	/*  208		*/
#   define	ISO9_Ntilde		0xd1	/*  209		*/
#   define	ISO9_Ograve		0xd2	/*  210		*/
#   define	ISO9_Oacute		0xd3	/*  211		*/
#   define	ISO9_Ocircumflex	0xd4	/*  212		*/
#   define	ISO9_Otilde		0xd5	/*  213		*/
#   define	ISO9_Odieresis		0xd6	/*  214		*/
#   define	ISO9_multiply		0xd7	/*  215		*/
#   define	ISO9_Oslash		0xd8	/*  216		*/
#   define	ISO9_Ugrave		0xd9	/*  217		*/
#   define	ISO9_Uacute		0xda	/*  218		*/
#   define	ISO9_Ucircumflex	0xdb	/*  219		*/
#   define	ISO9_Udieresis		0xdc	/*  220		*/
#   define	ISO9_Idotaccent		0xdd	/*  221		*/
#   define	ISO9_Scedilla		0xde	/*  222		*/
#   define	ISO9_germandbls		0xdf	/*  223		*/
#   define	ISO9_agrave		0xe0	/*  224		*/
#   define	ISO9_aacute		0xe1	/*  225		*/
#   define	ISO9_acircumflex	0xe2	/*  226		*/
#   define	ISO9_atilde		0xe3	/*  227		*/
#   define	ISO9_adieresis		0xe4	/*  228		*/
#   define	ISO9_aring		0xe5	/*  229		*/
#   define	ISO9_ae			0xe6	/*  230		*/
#   define	ISO9_ccedilla		0xe7	/*  231		*/
#   define	ISO9_egrave		0xe8	/*  232		*/
#   define	ISO9_eacute		0xe9	/*  233		*/
#   define	ISO9_ecircumflex	0xea	/*  234		*/
#   define	ISO9_edieresis		0xeb	/*  235		*/
#   define	ISO9_igrave		0xec	/*  236		*/
#   define	ISO9_iacute		0xed	/*  237		*/
#   define	ISO9_icircumflex	0xee	/*  238		*/
#   define	ISO9_idieresis		0xef	/*  239		*/
#   define	ISO9_gbreve		0xf0	/*  240		*/
#   define	ISO9_ntilde		0xf1	/*  241		*/
#   define	ISO9_ograve		0xf2	/*  242		*/
#   define	ISO9_oacute		0xf3	/*  243		*/
#   define	ISO9_ocircumflex	0xf4	/*  244		*/
#   define	ISO9_otilde		0xf5	/*  245		*/
#   define	ISO9_odieresis		0xf6	/*  246		*/
#   define	ISO9_divide		0xf7	/*  247		*/
#   define	ISO9_oslash		0xf8	/*  248		*/
#   define	ISO9_ugrave		0xf9	/*  249		*/
#   define	ISO9_uacute		0xfa	/*  250		*/
#   define	ISO9_ucircumflex	0xfb	/*  251		*/
#   define	ISO9_udieresis		0xfc	/*  252		*/
#   define	ISO9_dotlessi		0xfd	/*  253		*/
#   define	ISO9_scedilla		0xfe	/*  254		*/
#   define	ISO9_ydieresis		0xff	/*  255		*/

#   define	ISO9_isupper(c)	((ASCII_isupper((c)))||\
				  ((c)>=ISO9_Agrave&&(c)<=ISO9_Odieresis)||\
				  ((c)>=ISO9_Oslash&&(c)<=ISO9_germandbls))

#   define	ISO9_islower(c)	((ASCII_islower((c)))||\
				  ((c)>=ISO9_germandbls&&(c)<=ISO9_odieresis)||\
				  ((c)>=ISO9_oslash&&(c)<=ISO9_ydieresis))

#   define	ISO9_isalpha(c)	(ISO9_isupper((c))||ISO9_islower((c)))

#   define	ISO9_isdigit(c)	(ASCII_isdigit((c)))

#   define	ISO9_isalnum(c)	(ISO9_isupper((c))||ISO9_islower((c))||\
				  ISO9_isdigit((c)))

#   define	ISO9_tolower(c) \
	    (((ISO9_isupper(c)&&(c)!=ISO9_germandbls&&(c)!=ISO9_Idotaccent))?\
				    ((c)+0x20): \
				    ( (c)==ISO9_Idotaccent?ISO9_i:(c)))

#   define	ISO9_toupper(c)	\
	    (((ISO9_isupper(c)&&(c)!=ISO9_germandbls&&(c)!=ISO9_dotlessi))?\
				    ((c)-0x20): \
				    ( (c)==ISO9_dotlessi?ISO9_I:(c)))

/************************************************************************/
/*									*/
/*  Names for the characters in the Windows 1254 code page.		*/
/*  (Turkish)								*/
/*									*/
/************************************************************************/

#   define	WIN1253__0x00			0x00	/*    0		*/

#   define	WIN1254__0x00		0x00	/*    0		*/
#   define	WIN1254__0x01		0x01	/*    1		*/
#   define	WIN1254__0x02		0x02	/*    2		*/
#   define	WIN1254__0x03		0x03	/*    3		*/
#   define	WIN1254__0x04		0x04	/*    4		*/
#   define	WIN1254__0x05		0x05	/*    5		*/
#   define	WIN1254__0x06		0x06	/*    6		*/
#   define	WIN1254__0x07		0x07	/*    7		*/
#   define	WIN1254__0x08		0x08	/*    8		*/
#   define	WIN1254__0x09		0x09	/*    9		*/
#   define	WIN1254__0x0A		0x0a	/*   10		*/
#   define	WIN1254__0x0B		0x0b	/*   11		*/
#   define	WIN1254__0x0C		0x0c	/*   12		*/
#   define	WIN1254__0x0D		0x0d	/*   13		*/
#   define	WIN1254__0x0E		0x0e	/*   14		*/
#   define	WIN1254__0x0F		0x0f	/*   15		*/
#   define	WIN1254__0x10		0x10	/*   16		*/
#   define	WIN1254__0x11		0x11	/*   17		*/
#   define	WIN1254__0x12		0x12	/*   18		*/
#   define	WIN1254__0x13		0x13	/*   19		*/
#   define	WIN1254__0x14		0x14	/*   20		*/
#   define	WIN1254__0x15		0x15	/*   21		*/
#   define	WIN1254__0x16		0x16	/*   22		*/
#   define	WIN1254__0x17		0x17	/*   23		*/
#   define	WIN1254__0x18		0x18	/*   24		*/
#   define	WIN1254__0x19		0x19	/*   25		*/
#   define	WIN1254__0x1A		0x1a	/*   26		*/
#   define	WIN1254__0x1B		0x1b	/*   27		*/
#   define	WIN1254__0x1C		0x1c	/*   28		*/
#   define	WIN1254__0x1D		0x1d	/*   29		*/
#   define	WIN1254__0x1E		0x1e	/*   30		*/
#   define	WIN1254__0x1F		0x1f	/*   31		*/
#   define	WIN1254_space		0x20	/*   32		*/
#   define	WIN1254_exclam		0x21	/*   33		*/
#   define	WIN1254_quotedbl	0x22	/*   34		*/
#   define	WIN1254_numbersign	0x23	/*   35		*/
#   define	WIN1254_dollar		0x24	/*   36		*/
#   define	WIN1254_percent		0x25	/*   37		*/
#   define	WIN1254_ampersand	0x26	/*   38		*/
#   define	WIN1254_quotesingle	0x27	/*   39		*/
#   define	WIN1254_parenleft	0x28	/*   40		*/
#   define	WIN1254_parenright	0x29	/*   41		*/
#   define	WIN1254_asterisk	0x2a	/*   42		*/
#   define	WIN1254_plus		0x2b	/*   43		*/
#   define	WIN1254_comma		0x2c	/*   44		*/
#   define	WIN1254_minus		0x2d	/*   45		*/
#   define	WIN1254_period		0x2e	/*   46		*/
#   define	WIN1254_slash		0x2f	/*   47		*/
#   define	WIN1254_zero		0x30	/*   48		*/
#   define	WIN1254_one		0x31	/*   49		*/
#   define	WIN1254_two		0x32	/*   50		*/
#   define	WIN1254_three		0x33	/*   51		*/
#   define	WIN1254_four		0x34	/*   52		*/
#   define	WIN1254_five		0x35	/*   53		*/
#   define	WIN1254_six		0x36	/*   54		*/
#   define	WIN1254_seven		0x37	/*   55		*/
#   define	WIN1254_eight		0x38	/*   56		*/
#   define	WIN1254_nine		0x39	/*   57		*/
#   define	WIN1254_colon		0x3a	/*   58		*/
#   define	WIN1254_semicolon	0x3b	/*   59		*/
#   define	WIN1254_less		0x3c	/*   60		*/
#   define	WIN1254_equal		0x3d	/*   61		*/
#   define	WIN1254_greater		0x3e	/*   62		*/
#   define	WIN1254_question	0x3f	/*   63		*/
#   define	WIN1254_at		0x40	/*   64		*/
#   define	WIN1254_A		0x41	/*   65		*/
#   define	WIN1254_B		0x42	/*   66		*/
#   define	WIN1254_C		0x43	/*   67		*/
#   define	WIN1254_D		0x44	/*   68		*/
#   define	WIN1254_E		0x45	/*   69		*/
#   define	WIN1254_F		0x46	/*   70		*/
#   define	WIN1254_G		0x47	/*   71		*/
#   define	WIN1254_H		0x48	/*   72		*/
#   define	WIN1254_I		0x49	/*   73		*/
#   define	WIN1254_J		0x4a	/*   74		*/
#   define	WIN1254_K		0x4b	/*   75		*/
#   define	WIN1254_L		0x4c	/*   76		*/
#   define	WIN1254_M		0x4d	/*   77		*/
#   define	WIN1254_N		0x4e	/*   78		*/
#   define	WIN1254_O		0x4f	/*   79		*/
#   define	WIN1254_P		0x50	/*   80		*/
#   define	WIN1254_Q		0x51	/*   81		*/
#   define	WIN1254_R		0x52	/*   82		*/
#   define	WIN1254_S		0x53	/*   83		*/
#   define	WIN1254_T		0x54	/*   84		*/
#   define	WIN1254_U		0x55	/*   85		*/
#   define	WIN1254_V		0x56	/*   86		*/
#   define	WIN1254_W		0x57	/*   87		*/
#   define	WIN1254_X		0x58	/*   88		*/
#   define	WIN1254_Y		0x59	/*   89		*/
#   define	WIN1254_Z		0x5a	/*   90		*/
#   define	WIN1254_bracketleft	0x5b	/*   91		*/
#   define	WIN1254_backslash	0x5c	/*   92		*/
#   define	WIN1254_bracketright	0x5d	/*   93		*/
#   define	WIN1254_asciicircum	0x5e	/*   94		*/
#   define	WIN1254_underscore	0x5f	/*   95		*/
#   define	WIN1254_quoteleft	0x60	/*   96		*/
#   define	WIN1254_a		0x61	/*   97		*/
#   define	WIN1254_b		0x62	/*   98		*/
#   define	WIN1254_c		0x63	/*   99		*/
#   define	WIN1254_d		0x64	/*  100		*/
#   define	WIN1254_e		0x65	/*  101		*/
#   define	WIN1254_f		0x66	/*  102		*/
#   define	WIN1254_g		0x67	/*  103		*/
#   define	WIN1254_h		0x68	/*  104		*/
#   define	WIN1254_i		0x69	/*  105		*/
#   define	WIN1254_j		0x6a	/*  106		*/
#   define	WIN1254_k		0x6b	/*  107		*/
#   define	WIN1254_l		0x6c	/*  108		*/
#   define	WIN1254_m		0x6d	/*  109		*/
#   define	WIN1254_n		0x6e	/*  110		*/
#   define	WIN1254_o		0x6f	/*  111		*/
#   define	WIN1254_p		0x70	/*  112		*/
#   define	WIN1254_q		0x71	/*  113		*/
#   define	WIN1254_r		0x72	/*  114		*/
#   define	WIN1254_s		0x73	/*  115		*/
#   define	WIN1254_t		0x74	/*  116		*/
#   define	WIN1254_u		0x75	/*  117		*/
#   define	WIN1254_v		0x76	/*  118		*/
#   define	WIN1254_w		0x77	/*  119		*/
#   define	WIN1254_x		0x78	/*  120		*/
#   define	WIN1254_y		0x79	/*  121		*/
#   define	WIN1254_z		0x7a	/*  122		*/
#   define	WIN1254_braceleft	0x7b	/*  123		*/
#   define	WIN1254_bar		0x7c	/*  124		*/
#   define	WIN1254_braceright	0x7d	/*  125		*/
#   define	WIN1254_asciitilde	0x7e	/*  126		*/
#   define	WIN1254__0x7F		0x7f	/*  127		*/
#   define	WIN1254__0x80		0x80	/*  128		*/
#   define	WIN1254__0x81		0x81	/*  129		*/
#   define	WIN1254_quotesinglbase	0x82	/*  130		*/
#   define	WIN1254_florin		0x83	/*  131		*/
#   define	WIN1254_quotedblbase	0x84	/*  132		*/
#   define	WIN1254_ellipsis	0x85	/*  133		*/
#   define	WIN1254_dagger		0x86	/*  134		*/
#   define	WIN1254_daggerdbl	0x87	/*  135		*/
#   define	WIN1254_circumflex	0x88	/*  136		*/
#   define	WIN1254_perthousand	0x89	/*  137		*/
#   define	WIN1254_Scaron		0x8a	/*  138		*/
#   define	WIN1254_guilsinglleft	0x8b	/*  139		*/
#   define	WIN1254_OE		0x8c	/*  140		*/
#   define	WIN1254__0x8D		0x8d	/*  141		*/
#   define	WIN1254__0x8E		0x8e	/*  142		*/
#   define	WIN1254__0x8F		0x8f	/*  143		*/
#   define	WIN1254__0x90		0x90	/*  144		*/
#   define	WIN1254__quoteleft	0x91	/*  145		*/
#   define	WIN1254_quoteright	0x92	/*  146		*/
#   define	WIN1254_quotedblleft	0x93	/*  147		*/
#   define	WIN1254_quotedblright	0x94	/*  148		*/
#   define	WIN1254_bullet		0x95	/*  149		*/
#   define	WIN1254_endash		0x96	/*  150		*/
#   define	WIN1254_emdash		0x97	/*  151		*/
#   define	WIN1254_tilde		0x98	/*  152		*/
#   define	WIN1254_trademark	0x99	/*  153		*/
#   define	WIN1254_scaron		0x9a	/*  154		*/
#   define	WIN1254_guilsinglright	0x9b	/*  155		*/
#   define	WIN1254_oe		0x9c	/*  156		*/
#   define	WIN1254__0x9D		0x9d	/*  157		*/
#   define	WIN1254__0x9E		0x9e	/*  158		*/
#   define	WIN1254_Ydieresis	0x9f	/*  159		*/
#   define	WIN1254_nobreakspace	0xa0	/*  160		*/
#   define	WIN1254_exclamdown	0xa1	/*  161		*/
#   define	WIN1254_cent		0xa2	/*  162		*/
#   define	WIN1254_sterling	0xa3	/*  163		*/
#   define	WIN1254_currency	0xa4	/*  164		*/
#   define	WIN1254_yen		0xa5	/*  165		*/
#   define	WIN1254_brokenbar	0xa6	/*  166		*/
#   define	WIN1254_section		0xa7	/*  167		*/
#   define	WIN1254_dieresis	0xa8	/*  168		*/
#   define	WIN1254_copyright	0xa9	/*  169		*/
#   define	WIN1254_ordfeminine	0xaa	/*  170		*/
#   define	WIN1254_guillemotleft	0xab	/*  171		*/
#   define	WIN1254_logicalnot	0xac	/*  172		*/
#   define	WIN1254_hyphen		0xad	/*  173		*/
#   define	WIN1254_registered	0xae	/*  174		*/
#   define	WIN1254_macron		0xaf	/*  175		*/
#   define	WIN1254_degree		0xb0	/*  176		*/
#   define	WIN1254_plusminus	0xb1	/*  177		*/
#   define	WIN1254_twosuperior	0xb2	/*  178		*/
#   define	WIN1254_threesuperior	0xb3	/*  179		*/
#   define	WIN1254_acute		0xb4	/*  180		*/
#   define	WIN1254_mu		0xb5	/*  181		*/
#   define	WIN1254_paragraph	0xb6	/*  182		*/
#   define	WIN1254_periodcentered	0xb7	/*  183		*/
#   define	WIN1254_cedilla		0xb8	/*  184		*/
#   define	WIN1254_onesuperior	0xb9	/*  185		*/
#   define	WIN1254_ordmasculine	0xba	/*  186		*/
#   define	WIN1254_guillemotright	0xbb	/*  187		*/
#   define	WIN1254_onequarter	0xbc	/*  188		*/
#   define	WIN1254_onehalf		0xbd	/*  189		*/
#   define	WIN1254_threequarters	0xbe	/*  190		*/
#   define	WIN1254_questiondown	0xbf	/*  191		*/
#   define	WIN1254_Agrave		0xc0	/*  192		*/
#   define	WIN1254_Aacute		0xc1	/*  193		*/
#   define	WIN1254_Acircumflex	0xc2	/*  194		*/
#   define	WIN1254_Atilde		0xc3	/*  195		*/
#   define	WIN1254_Adieresis	0xc4	/*  196		*/
#   define	WIN1254_Aring		0xc5	/*  197		*/
#   define	WIN1254_AE		0xc6	/*  198		*/
#   define	WIN1254_Ccedilla	0xc7	/*  199		*/
#   define	WIN1254_Egrave		0xc8	/*  200		*/
#   define	WIN1254_Eacute		0xc9	/*  201		*/
#   define	WIN1254_Ecircumflex	0xca	/*  202		*/
#   define	WIN1254_Edieresis	0xcb	/*  203		*/
#   define	WIN1254_Igrave		0xcc	/*  204		*/
#   define	WIN1254_Iacute		0xcd	/*  205		*/
#   define	WIN1254_Icircumflex	0xce	/*  206		*/
#   define	WIN1254_Idieresis	0xcf	/*  207		*/
#   define	WIN1254_Gbreve		0xd0	/*  208		*/
#   define	WIN1254_Ntilde		0xd1	/*  209		*/
#   define	WIN1254_Ograve		0xd2	/*  210		*/
#   define	WIN1254_Oacute		0xd3	/*  211		*/
#   define	WIN1254_Ocircumflex	0xd4	/*  212		*/
#   define	WIN1254_Otilde		0xd5	/*  213		*/
#   define	WIN1254_Odieresis	0xd6	/*  214		*/
#   define	WIN1254_multiply	0xd7	/*  215		*/
#   define	WIN1254_Oslash		0xd8	/*  216		*/
#   define	WIN1254_Ugrave		0xd9	/*  217		*/
#   define	WIN1254_Uacute		0xda	/*  218		*/
#   define	WIN1254_Ucircumflex	0xdb	/*  219		*/
#   define	WIN1254_Udieresis	0xdc	/*  220		*/
#   define	WIN1254_Idotaccent	0xdd	/*  221		*/
#   define	WIN1254_Scedilla	0xde	/*  222		*/
#   define	WIN1254_germandbls	0xdf	/*  223		*/
#   define	WIN1254_agrave		0xe0	/*  224		*/
#   define	WIN1254_aacute		0xe1	/*  225		*/
#   define	WIN1254_acircumflex	0xe2	/*  226		*/
#   define	WIN1254_atilde		0xe3	/*  227		*/
#   define	WIN1254_adieresis	0xe4	/*  228		*/
#   define	WIN1254_aring		0xe5	/*  229		*/
#   define	WIN1254_ae		0xe6	/*  230		*/
#   define	WIN1254_ccedilla	0xe7	/*  231		*/
#   define	WIN1254_egrave		0xe8	/*  232		*/
#   define	WIN1254_eacute		0xe9	/*  233		*/
#   define	WIN1254_ecircumflex	0xea	/*  234		*/
#   define	WIN1254_edieresis	0xeb	/*  235		*/
#   define	WIN1254_igrave		0xec	/*  236		*/
#   define	WIN1254_iacute		0xed	/*  237		*/
#   define	WIN1254_icircumflex	0xee	/*  238		*/
#   define	WIN1254_idieresis	0xef	/*  239		*/
#   define	WIN1254_gbreve		0xf0	/*  240		*/
#   define	WIN1254_ntilde		0xf1	/*  241		*/
#   define	WIN1254_ograve		0xf2	/*  242		*/
#   define	WIN1254_oacute		0xf3	/*  243		*/
#   define	WIN1254_ocircumflex	0xf4	/*  244		*/
#   define	WIN1254_otilde		0xf5	/*  245		*/
#   define	WIN1254_odieresis	0xf6	/*  246		*/
#   define	WIN1254_divide		0xf7	/*  247		*/
#   define	WIN1254_oslash		0xf8	/*  248		*/
#   define	WIN1254_ugrave		0xf9	/*  249		*/
#   define	WIN1254_uacute		0xfa	/*  250		*/
#   define	WIN1254_ucircumflex	0xfb	/*  251		*/
#   define	WIN1254_udieresis	0xfc	/*  252		*/
#   define	WIN1254_dotlessi	0xfd	/*  253		*/
#   define	WIN1254_scedilla	0xfe	/*  254		*/
#   define	WIN1254_ydieresis	0xff	/*  255		*/

#   define	ISO10__0x00		0x00	/*    0		*/
#   define	ISO10__0x01		0x01	/*    1		*/
#   define	ISO10__0x02		0x02	/*    2		*/
#   define	ISO10__0x03		0x03	/*    3		*/
#   define	ISO10__0x04		0x04	/*    4		*/
#   define	ISO10__0x05		0x05	/*    5		*/
#   define	ISO10__0x06		0x06	/*    6		*/
#   define	ISO10__0x07		0x07	/*    7		*/
#   define	ISO10__0x08		0x08	/*    8		*/
#   define	ISO10__0x09		0x09	/*    9		*/
#   define	ISO10__0x0A		0x0a	/*   10		*/
#   define	ISO10__0x0B		0x0b	/*   11		*/
#   define	ISO10__0x0C		0x0c	/*   12		*/
#   define	ISO10__0x0D		0x0d	/*   13		*/
#   define	ISO10__0x0E		0x0e	/*   14		*/
#   define	ISO10__0x0F		0x0f	/*   15		*/
#   define	ISO10__0x10		0x10	/*   16		*/
#   define	ISO10__0x11		0x11	/*   17		*/
#   define	ISO10__0x12		0x12	/*   18		*/
#   define	ISO10__0x13		0x13	/*   19		*/
#   define	ISO10__0x14		0x14	/*   20		*/
#   define	ISO10__0x15		0x15	/*   21		*/
#   define	ISO10__0x16		0x16	/*   22		*/
#   define	ISO10__0x17		0x17	/*   23		*/
#   define	ISO10__0x18		0x18	/*   24		*/
#   define	ISO10__0x19		0x19	/*   25		*/
#   define	ISO10__0x1A		0x1a	/*   26		*/
#   define	ISO10__0x1B		0x1b	/*   27		*/
#   define	ISO10__0x1C		0x1c	/*   28		*/
#   define	ISO10__0x1D		0x1d	/*   29		*/
#   define	ISO10__0x1E		0x1e	/*   30		*/
#   define	ISO10__0x1F		0x1f	/*   31		*/
#   define	ISO10_space		0x20	/*   32		*/
#   define	ISO10_exclam		0x21	/*   33		*/
#   define	ISO10_quotedbl		0x22	/*   34		*/
#   define	ISO10_numbersign	0x23	/*   35		*/
#   define	ISO10_dollar		0x24	/*   36		*/
#   define	ISO10_percent		0x25	/*   37		*/
#   define	ISO10_ampersand		0x26	/*   38		*/
#   define	ISO10_quotesingle	0x27	/*   39		*/
#   define	ISO10_parenleft		0x28	/*   40		*/
#   define	ISO10_parenright	0x29	/*   41		*/
#   define	ISO10_asterisk		0x2a	/*   42		*/
#   define	ISO10_plus		0x2b	/*   43		*/
#   define	ISO10_comma		0x2c	/*   44		*/
#   define	ISO10_minus		0x2d	/*   45		*/
#   define	ISO10_period		0x2e	/*   46		*/
#   define	ISO10_slash		0x2f	/*   47		*/
#   define	ISO10_zero		0x30	/*   48		*/
#   define	ISO10_one		0x31	/*   49		*/
#   define	ISO10_two		0x32	/*   50		*/
#   define	ISO10_three		0x33	/*   51		*/
#   define	ISO10_four		0x34	/*   52		*/
#   define	ISO10_five		0x35	/*   53		*/
#   define	ISO10_six		0x36	/*   54		*/
#   define	ISO10_seven		0x37	/*   55		*/
#   define	ISO10_eight		0x38	/*   56		*/
#   define	ISO10_nine		0x39	/*   57		*/
#   define	ISO10_colon		0x3a	/*   58		*/
#   define	ISO10_semicolon		0x3b	/*   59		*/
#   define	ISO10_less		0x3c	/*   60		*/
#   define	ISO10_equal		0x3d	/*   61		*/
#   define	ISO10_greater		0x3e	/*   62		*/
#   define	ISO10_question		0x3f	/*   63		*/
#   define	ISO10_at		0x40	/*   64		*/
#   define	ISO10_A			0x41	/*   65		*/
#   define	ISO10_B			0x42	/*   66		*/
#   define	ISO10_C			0x43	/*   67		*/
#   define	ISO10_D			0x44	/*   68		*/
#   define	ISO10_E			0x45	/*   69		*/
#   define	ISO10_F			0x46	/*   70		*/
#   define	ISO10_G			0x47	/*   71		*/
#   define	ISO10_H			0x48	/*   72		*/
#   define	ISO10_I			0x49	/*   73		*/
#   define	ISO10_J			0x4a	/*   74		*/
#   define	ISO10_K			0x4b	/*   75		*/
#   define	ISO10_L			0x4c	/*   76		*/
#   define	ISO10_M			0x4d	/*   77		*/
#   define	ISO10_N			0x4e	/*   78		*/
#   define	ISO10_O			0x4f	/*   79		*/
#   define	ISO10_P			0x50	/*   80		*/
#   define	ISO10_Q			0x51	/*   81		*/
#   define	ISO10_R			0x52	/*   82		*/
#   define	ISO10_S			0x53	/*   83		*/
#   define	ISO10_T			0x54	/*   84		*/
#   define	ISO10_U			0x55	/*   85		*/
#   define	ISO10_V			0x56	/*   86		*/
#   define	ISO10_W			0x57	/*   87		*/
#   define	ISO10_X			0x58	/*   88		*/
#   define	ISO10_Y			0x59	/*   89		*/
#   define	ISO10_Z			0x5a	/*   90		*/
#   define	ISO10_bracketleft	0x5b	/*   91		*/
#   define	ISO10_backslash		0x5c	/*   92		*/
#   define	ISO10_bracketright	0x5d	/*   93		*/
#   define	ISO10_asciicircum	0x5e	/*   94		*/
#   define	ISO10_underscore	0x5f	/*   95		*/
#   define	ISO10_grave		0x60	/*   96		*/
#   define	ISO10_a			0x61	/*   97		*/
#   define	ISO10_b			0x62	/*   98		*/
#   define	ISO10_c			0x63	/*   99		*/
#   define	ISO10_d			0x64	/*  100		*/
#   define	ISO10_e			0x65	/*  101		*/
#   define	ISO10_f			0x66	/*  102		*/
#   define	ISO10_g			0x67	/*  103		*/
#   define	ISO10_h			0x68	/*  104		*/
#   define	ISO10_i			0x69	/*  105		*/
#   define	ISO10_j			0x6a	/*  106		*/
#   define	ISO10_k			0x6b	/*  107		*/
#   define	ISO10_l			0x6c	/*  108		*/
#   define	ISO10_m			0x6d	/*  109		*/
#   define	ISO10_n			0x6e	/*  110		*/
#   define	ISO10_o			0x6f	/*  111		*/
#   define	ISO10_p			0x70	/*  112		*/
#   define	ISO10_q			0x71	/*  113		*/
#   define	ISO10_r			0x72	/*  114		*/
#   define	ISO10_s			0x73	/*  115		*/
#   define	ISO10_t			0x74	/*  116		*/
#   define	ISO10_u			0x75	/*  117		*/
#   define	ISO10_v			0x76	/*  118		*/
#   define	ISO10_w			0x77	/*  119		*/
#   define	ISO10_x			0x78	/*  120		*/
#   define	ISO10_y			0x79	/*  121		*/
#   define	ISO10_z			0x7a	/*  122		*/
#   define	ISO10_braceleft		0x7b	/*  123		*/
#   define	ISO10_bar		0x7c	/*  124		*/
#   define	ISO10_braceright	0x7d	/*  125		*/
#   define	ISO10_asciitilde	0x7e	/*  126		*/
#   define	ISO10__0x7F		0x7f	/*  127		*/
#   define	ISO10__0x80		0x80	/*  128		*/
#   define	ISO10__0x81		0x81	/*  129		*/
#   define	ISO10__0x82		0x82	/*  130		*/
#   define	ISO10__0x83		0x83	/*  131		*/
#   define	ISO10__0x84		0x84	/*  132		*/
#   define	ISO10__0x85		0x85	/*  133		*/
#   define	ISO10__0x86		0x86	/*  134		*/
#   define	ISO10__0x87		0x87	/*  135		*/
#   define	ISO10__0x88		0x88	/*  136		*/
#   define	ISO10__0x89		0x89	/*  137		*/
#   define	ISO10__0x8A		0x8a	/*  138		*/
#   define	ISO10__0x8B		0x8b	/*  139		*/
#   define	ISO10__0x8C		0x8c	/*  140		*/
#   define	ISO10__0x8D		0x8d	/*  141		*/
#   define	ISO10__0x8E		0x8e	/*  142		*/
#   define	ISO10__0x8F		0x8f	/*  143		*/
#   define	ISO10__0x90		0x90	/*  144		*/
#   define	ISO10__0x91		0x91	/*  145		*/
#   define	ISO10__0x92		0x92	/*  146		*/
#   define	ISO10__0x93		0x93	/*  147		*/
#   define	ISO10__0x94		0x94	/*  148		*/
#   define	ISO10__0x95		0x95	/*  149		*/
#   define	ISO10__0x96		0x96	/*  150		*/
#   define	ISO10__0x97		0x97	/*  151		*/
#   define	ISO10__0x98		0x98	/*  152		*/
#   define	ISO10__0x99		0x99	/*  153		*/
#   define	ISO10__0x9A		0x9a	/*  154		*/
#   define	ISO10__0x9B		0x9b	/*  155		*/
#   define	ISO10__0x9C		0x9c	/*  156		*/
#   define	ISO10__0x9D		0x9d	/*  157		*/
#   define	ISO10__0x9E		0x9e	/*  158		*/
#   define	ISO10__0x9F		0x9f	/*  159		*/
#   define	ISO10_nobreakspace	0xa0	/*  160		*/
#   define	ISO10_Aogonek		0xa1	/*  161		*/
#   define	ISO10_Emacron		0xa2	/*  162		*/
#   define	ISO10_Gcommaaccent	0xa3	/*  163		*/
#   define	ISO10_Imacron		0xa4	/*  164		*/
#   define	ISO10_Itilde		0xa5	/*  165		*/
#   define	ISO10_Kcommaaccent	0xa6	/*  166		*/
#   define	ISO10_section		0xa7	/*  167		*/
#   define	ISO10_Lcommaaccent	0xa8	/*  168		*/
#   define	ISO10_Dcroat		0xa9	/*  169		*/
#   define	ISO10_Scaron		0xaa	/*  170		*/
#   define	ISO10_Tbar		0xab	/*  171		*/
#   define	ISO10_Zcaron		0xac	/*  172		*/
#   define	ISO10_hyphen		0xad	/*  173		*/
#   define	ISO10_Umacron		0xae	/*  174		*/
#   define	ISO10_Eng		0xaf	/*  175		*/
#   define	ISO10_degree		0xb0	/*  176		*/
#   define	ISO10_aogonek		0xb1	/*  177		*/
#   define	ISO10_emacron		0xb2	/*  178		*/
#   define	ISO10_gcommaaccent	0xb3	/*  179		*/
#   define	ISO10_imacron		0xb4	/*  180		*/
#   define	ISO10_itilde		0xb5	/*  181		*/
#   define	ISO10_kcommaaccent	0xb6	/*  182		*/
#   define	ISO10_periodcentered	0xb7	/*  183		*/
#   define	ISO10_lcommaaccent	0xb8	/*  184		*/
#   define	ISO10_dcroat		0xb9	/*  185		*/
#   define	ISO10_scaron		0xba	/*  186		*/
#   define	ISO10_tbar		0xbb	/*  187		*/
#   define	ISO10_zcaron		0xbc	/*  188		*/
#   define	ISO10_afii00208		0xbd	/*  189		*/
#   define	ISO10_umacron		0xbe	/*  190		*/
#   define	ISO10_eng		0xbf	/*  191		*/
#   define	ISO10_Amacron		0xc0	/*  192		*/
#   define	ISO10_Aacute		0xc1	/*  193		*/
#   define	ISO10_Acircumflex	0xc2	/*  194		*/
#   define	ISO10_Atilde		0xc3	/*  195		*/
#   define	ISO10_Adieresis		0xc4	/*  196		*/
#   define	ISO10_Aring		0xc5	/*  197		*/
#   define	ISO10_AE		0xc6	/*  198		*/
#   define	ISO10_Iogonek		0xc7	/*  199		*/
#   define	ISO10_Ccaron		0xc8	/*  200		*/
#   define	ISO10_Eacute		0xc9	/*  201		*/
#   define	ISO10_Eogonek		0xca	/*  202		*/
#   define	ISO10_Edieresis		0xcb	/*  203		*/
#   define	ISO10_Edotaccent	0xcc	/*  204		*/
#   define	ISO10_Iacute		0xcd	/*  205		*/
#   define	ISO10_Icircumflex	0xce	/*  206		*/
#   define	ISO10_Idieresis		0xcf	/*  207		*/
#   define	ISO10_Eth		0xd0	/*  208		*/
#   define	ISO10_Ncommaaccent	0xd1	/*  209		*/
#   define	ISO10_Omacron		0xd2	/*  210		*/
#   define	ISO10_Oacute		0xd3	/*  211		*/
#   define	ISO10_Ocircumflex	0xd4	/*  212		*/
#   define	ISO10_Otilde		0xd5	/*  213		*/
#   define	ISO10_Odieresis		0xd6	/*  214		*/
#   define	ISO10_Utilde		0xd7	/*  215		*/
#   define	ISO10_Oslash		0xd8	/*  216		*/
#   define	ISO10_Uogonek		0xd9	/*  217		*/
#   define	ISO10_Uacute		0xda	/*  218		*/
#   define	ISO10_Ucircumflex	0xdb	/*  219		*/
#   define	ISO10_Udieresis		0xdc	/*  220		*/
#   define	ISO10_Yacute		0xdd	/*  221		*/
#   define	ISO10_Thorn		0xde	/*  222		*/
#   define	ISO10_germandbls	0xdf	/*  223		*/
#   define	ISO10_amacron		0xe0	/*  224		*/
#   define	ISO10_aacute		0xe1	/*  225		*/
#   define	ISO10_acircumflex	0xe2	/*  226		*/
#   define	ISO10_atilde		0xe3	/*  227		*/
#   define	ISO10_adieresis		0xe4	/*  228		*/
#   define	ISO10_aring		0xe5	/*  229		*/
#   define	ISO10_ae		0xe6	/*  230		*/
#   define	ISO10_iogonek		0xe7	/*  231		*/
#   define	ISO10_ccaron		0xe8	/*  232		*/
#   define	ISO10_eacute		0xe9	/*  233		*/
#   define	ISO10_eogonek		0xea	/*  234		*/
#   define	ISO10_edieresis		0xeb	/*  235		*/
#   define	ISO10_edotaccent	0xec	/*  236		*/
#   define	ISO10_iacute		0xed	/*  237		*/
#   define	ISO10_icircumflex	0xee	/*  238		*/
#   define	ISO10_idieresis		0xef	/*  239		*/
#   define	ISO10_eth		0xf0	/*  240		*/
#   define	ISO10_ncommaaccent	0xf1	/*  241		*/
#   define	ISO10_omacron		0xf2	/*  242		*/
#   define	ISO10_oacute		0xf3	/*  243		*/
#   define	ISO10_ocircumflex	0xf4	/*  244		*/
#   define	ISO10_otilde		0xf5	/*  245		*/
#   define	ISO10_odieresis		0xf6	/*  246		*/
#   define	ISO10_utilde		0xf7	/*  247		*/
#   define	ISO10_oslash		0xf8	/*  248		*/
#   define	ISO10_uogonek		0xf9	/*  249		*/
#   define	ISO10_uacute		0xfa	/*  250		*/
#   define	ISO10_ucircumflex	0xfb	/*  251		*/
#   define	ISO10_udieresis		0xfc	/*  252		*/
#   define	ISO10_yacute		0xfd	/*  253		*/
#   define	ISO10_thorn		0xfe	/*  254		*/
#   define	ISO10_kgreenlandic	0xff	/*  255		*/

#   define	WIN1257__0x00		0x00	/*    0		*/
#   define	WIN1257__0x01		0x01	/*    1		*/
#   define	WIN1257__0x02		0x02	/*    2		*/
#   define	WIN1257__0x03		0x03	/*    3		*/
#   define	WIN1257__0x04		0x04	/*    4		*/
#   define	WIN1257__0x05		0x05	/*    5		*/
#   define	WIN1257__0x06		0x06	/*    6		*/
#   define	WIN1257__0x07		0x07	/*    7		*/
#   define	WIN1257__0x08		0x08	/*    8		*/
#   define	WIN1257__0x09		0x09	/*    9		*/
#   define	WIN1257__0x0A		0x0a	/*   10		*/
#   define	WIN1257__0x0B		0x0b	/*   11		*/
#   define	WIN1257__0x0C		0x0c	/*   12		*/
#   define	WIN1257__0x0D		0x0d	/*   13		*/
#   define	WIN1257__0x0E		0x0e	/*   14		*/
#   define	WIN1257__0x0F		0x0f	/*   15		*/
#   define	WIN1257__0x10		0x10	/*   16		*/
#   define	WIN1257__0x11		0x11	/*   17		*/
#   define	WIN1257__0x12		0x12	/*   18		*/
#   define	WIN1257__0x13		0x13	/*   19		*/
#   define	WIN1257__0x14		0x14	/*   20		*/
#   define	WIN1257__0x15		0x15	/*   21		*/
#   define	WIN1257__0x16		0x16	/*   22		*/
#   define	WIN1257__0x17		0x17	/*   23		*/
#   define	WIN1257__0x18		0x18	/*   24		*/
#   define	WIN1257__0x19		0x19	/*   25		*/
#   define	WIN1257__0x1A		0x1a	/*   26		*/
#   define	WIN1257__0x1B		0x1b	/*   27		*/
#   define	WIN1257__0x1C		0x1c	/*   28		*/
#   define	WIN1257__0x1D		0x1d	/*   29		*/
#   define	WIN1257__0x1E		0x1e	/*   30		*/
#   define	WIN1257__0x1F		0x1f	/*   31		*/
#   define	WIN1257_space		0x20	/*   32		*/
#   define	WIN1257_exclam		0x21	/*   33		*/
#   define	WIN1257_quotedbl	0x22	/*   34		*/
#   define	WIN1257_numbersign	0x23	/*   35		*/
#   define	WIN1257_dollar		0x24	/*   36		*/
#   define	WIN1257_percent		0x25	/*   37		*/
#   define	WIN1257_ampersand	0x26	/*   38		*/
#   define	WIN1257_quotesingle	0x27	/*   39		*/
#   define	WIN1257_parenleft	0x28	/*   40		*/
#   define	WIN1257_parenright	0x29	/*   41		*/
#   define	WIN1257_asterisk	0x2a	/*   42		*/
#   define	WIN1257_plus		0x2b	/*   43		*/
#   define	WIN1257_comma		0x2c	/*   44		*/
#   define	WIN1257_minus		0x2d	/*   45		*/
#   define	WIN1257_period		0x2e	/*   46		*/
#   define	WIN1257_slash		0x2f	/*   47		*/
#   define	WIN1257_zero		0x30	/*   48		*/
#   define	WIN1257_one		0x31	/*   49		*/
#   define	WIN1257_two		0x32	/*   50		*/
#   define	WIN1257_three		0x33	/*   51		*/
#   define	WIN1257_four		0x34	/*   52		*/
#   define	WIN1257_five		0x35	/*   53		*/
#   define	WIN1257_six		0x36	/*   54		*/
#   define	WIN1257_seven		0x37	/*   55		*/
#   define	WIN1257_eight		0x38	/*   56		*/
#   define	WIN1257_nine		0x39	/*   57		*/
#   define	WIN1257_colon		0x3a	/*   58		*/
#   define	WIN1257_semicolon	0x3b	/*   59		*/
#   define	WIN1257_less		0x3c	/*   60		*/
#   define	WIN1257_equal		0x3d	/*   61		*/
#   define	WIN1257_greater		0x3e	/*   62		*/
#   define	WIN1257_question	0x3f	/*   63		*/
#   define	WIN1257_at		0x40	/*   64		*/
#   define	WIN1257_A		0x41	/*   65		*/
#   define	WIN1257_B		0x42	/*   66		*/
#   define	WIN1257_C		0x43	/*   67		*/
#   define	WIN1257_D		0x44	/*   68		*/
#   define	WIN1257_E		0x45	/*   69		*/
#   define	WIN1257_F		0x46	/*   70		*/
#   define	WIN1257_G		0x47	/*   71		*/
#   define	WIN1257_H		0x48	/*   72		*/
#   define	WIN1257_I		0x49	/*   73		*/
#   define	WIN1257_J		0x4a	/*   74		*/
#   define	WIN1257_K		0x4b	/*   75		*/
#   define	WIN1257_L		0x4c	/*   76		*/
#   define	WIN1257_M		0x4d	/*   77		*/
#   define	WIN1257_N		0x4e	/*   78		*/
#   define	WIN1257_O		0x4f	/*   79		*/
#   define	WIN1257_P		0x50	/*   80		*/
#   define	WIN1257_Q		0x51	/*   81		*/
#   define	WIN1257_R		0x52	/*   82		*/
#   define	WIN1257_S		0x53	/*   83		*/
#   define	WIN1257_T		0x54	/*   84		*/
#   define	WIN1257_U		0x55	/*   85		*/
#   define	WIN1257_V		0x56	/*   86		*/
#   define	WIN1257_W		0x57	/*   87		*/
#   define	WIN1257_X		0x58	/*   88		*/
#   define	WIN1257_Y		0x59	/*   89		*/
#   define	WIN1257_Z		0x5a	/*   90		*/
#   define	WIN1257_bracketleft	0x5b	/*   91		*/
#   define	WIN1257_backslash	0x5c	/*   92		*/
#   define	WIN1257_bracketright	0x5d	/*   93		*/
#   define	WIN1257_asciicircum	0x5e	/*   94		*/
#   define	WIN1257_underscore	0x5f	/*   95		*/
#   define	WIN1257_quoteleft	0x60	/*   96		*/
#   define	WIN1257_a		0x61	/*   97		*/
#   define	WIN1257_b		0x62	/*   98		*/
#   define	WIN1257_c		0x63	/*   99		*/
#   define	WIN1257_d		0x64	/*  100		*/
#   define	WIN1257_e		0x65	/*  101		*/
#   define	WIN1257_f		0x66	/*  102		*/
#   define	WIN1257_g		0x67	/*  103		*/
#   define	WIN1257_h		0x68	/*  104		*/
#   define	WIN1257_i		0x69	/*  105		*/
#   define	WIN1257_j		0x6a	/*  106		*/
#   define	WIN1257_k		0x6b	/*  107		*/
#   define	WIN1257_l		0x6c	/*  108		*/
#   define	WIN1257_m		0x6d	/*  109		*/
#   define	WIN1257_n		0x6e	/*  110		*/
#   define	WIN1257_o		0x6f	/*  111		*/
#   define	WIN1257_p		0x70	/*  112		*/
#   define	WIN1257_q		0x71	/*  113		*/
#   define	WIN1257_r		0x72	/*  114		*/
#   define	WIN1257_s		0x73	/*  115		*/
#   define	WIN1257_t		0x74	/*  116		*/
#   define	WIN1257_u		0x75	/*  117		*/
#   define	WIN1257_v		0x76	/*  118		*/
#   define	WIN1257_w		0x77	/*  119		*/
#   define	WIN1257_x		0x78	/*  120		*/
#   define	WIN1257_y		0x79	/*  121		*/
#   define	WIN1257_z		0x7a	/*  122		*/
#   define	WIN1257_braceleft	0x7b	/*  123		*/
#   define	WIN1257_bar		0x7c	/*  124		*/
#   define	WIN1257_braceright	0x7d	/*  125		*/
#   define	WIN1257_asciitilde	0x7e	/*  126		*/
#   define	WIN1257__0x7F		0x7f	/*  127		*/
#   define	WIN1257__0x80		0x80	/*  128		*/
#   define	WIN1257__0x81		0x81	/*  129		*/
#   define	WIN1257_quotesinglbase	0x82	/*  130		*/
#   define	WIN1257__0x83		0x83	/*  131		*/
#   define	WIN1257_quotedblbase	0x84	/*  132		*/
#   define	WIN1257_ellipsis	0x85	/*  133		*/
#   define	WIN1257_dagger		0x86	/*  134		*/
#   define	WIN1257_daggerdbl	0x87	/*  135		*/
#   define	WIN1257__0x88		0x88	/*  136		*/
#   define	WIN1257_perthousand	0x89	/*  137		*/
#   define	WIN1257__0x8A		0x8a	/*  138		*/
#   define	WIN1257_guilsinglleft	0x8b	/*  139		*/
#   define	WIN1257__0x8C		0x8c	/*  140		*/
#   define	WIN1257_dieresis	0x8d	/*  141		*/
#   define	WIN1257_caron		0x8e	/*  142		*/
#   define	WIN1257_cedilla		0x8f	/*  143		*/
#   define	WIN1257__0x90		0x90	/*  144		*/
#   define	WIN1257__quoteleft	0x91	/*  145		*/
#   define	WIN1257_quoteright	0x92	/*  146		*/
#   define	WIN1257_quotedblleft	0x93	/*  147		*/
#   define	WIN1257_quotedblright	0x94	/*  148		*/
#   define	WIN1257_bullet		0x95	/*  149		*/
#   define	WIN1257_endash		0x96	/*  150		*/
#   define	WIN1257_emdash		0x97	/*  151		*/
#   define	WIN1257__0x98		0x98	/*  152		*/
#   define	WIN1257_trademark	0x99	/*  153		*/
#   define	WIN1257__0x9A		0x9a	/*  154		*/
#   define	WIN1257_guilsinglright	0x9b	/*  155		*/
#   define	WIN1257__0x9C		0x9c	/*  156		*/
#   define	WIN1257_macron		0x9d	/*  157		*/
#   define	WIN1257_ogonek		0x9e	/*  158		*/
#   define	WIN1257__0x9F		0x9f	/*  159		*/
#   define	WIN1257_nobreakspace	0xa0	/*  160		*/
#   define	WIN1257__0xA1		0xa1	/*  161		*/
#   define	WIN1257_cent		0xa2	/*  162		*/
#   define	WIN1257_sterling	0xa3	/*  163		*/
#   define	WIN1257_currency	0xa4	/*  164		*/
#   define	WIN1257__0xA5		0xa5	/*  165		*/
#   define	WIN1257_brokenbar	0xa6	/*  166		*/
#   define	WIN1257_section		0xa7	/*  167		*/
#   define	WIN1257_Oslash		0xa8	/*  168		*/
#   define	WIN1257_copyright	0xa9	/*  169		*/
#   define	WIN1257_Rcommaaccent	0xaa	/*  170		*/
#   define	WIN1257_guillemotleft	0xab	/*  171		*/
#   define	WIN1257_logicalnot	0xac	/*  172		*/
#   define	WIN1257_hyphen		0xad	/*  173		*/
#   define	WIN1257_registered	0xae	/*  174		*/
#   define	WIN1257_AE		0xaf	/*  175		*/
#   define	WIN1257_degree		0xb0	/*  176		*/
#   define	WIN1257_plusminus	0xb1	/*  177		*/
#   define	WIN1257_twosuperior	0xb2	/*  178		*/
#   define	WIN1257_threesuperior	0xb3	/*  179		*/
#   define	WIN1257_acute		0xb4	/*  180		*/
#   define	WIN1257_mu		0xb5	/*  181		*/
#   define	WIN1257_paragraph	0xb6	/*  182		*/
#   define	WIN1257_periodcentered	0xb7	/*  183		*/
#   define	WIN1257_oslash		0xb8	/*  184		*/
#   define	WIN1257_onesuperior	0xb9	/*  185		*/
#   define	WIN1257_rcommaaccent	0xba	/*  186		*/
#   define	WIN1257_guillemotright	0xbb	/*  187		*/
#   define	WIN1257_onequarter	0xbc	/*  188		*/
#   define	WIN1257_onehalf		0xbd	/*  189		*/
#   define	WIN1257_threequarters	0xbe	/*  190		*/
#   define	WIN1257_ae		0xbf	/*  191		*/
#   define	WIN1257_Aogonek		0xc0	/*  192		*/
#   define	WIN1257_Iogonek		0xc1	/*  193		*/
#   define	WIN1257_Amacron		0xc2	/*  194		*/
#   define	WIN1257_Cacute		0xc3	/*  195		*/
#   define	WIN1257_Adieresis	0xc4	/*  196		*/
#   define	WIN1257_Aring		0xc5	/*  197		*/
#   define	WIN1257_Eogonek		0xc6	/*  198		*/
#   define	WIN1257_Emacron		0xc7	/*  199		*/
#   define	WIN1257_Ccaron		0xc8	/*  200		*/
#   define	WIN1257_Eacute		0xc9	/*  201		*/
#   define	WIN1257_Zacute		0xca	/*  202		*/
#   define	WIN1257_Edotaccent	0xcb	/*  203		*/
#   define	WIN1257_Gcommaaccent	0xcc	/*  204		*/
#   define	WIN1257_Kcommaaccent	0xcd	/*  205		*/
#   define	WIN1257_Imacron		0xce	/*  206		*/
#   define	WIN1257_Lcommaaccent	0xcf	/*  207		*/
#   define	WIN1257_Scaron		0xd0	/*  208		*/
#   define	WIN1257_Nacute		0xd1	/*  209		*/
#   define	WIN1257_Ncommaaccent	0xd2	/*  210		*/
#   define	WIN1257_Oacute		0xd3	/*  211		*/
#   define	WIN1257_Omacron		0xd4	/*  212		*/
#   define	WIN1257_Otilde		0xd5	/*  213		*/
#   define	WIN1257_Odieresis	0xd6	/*  214		*/
#   define	WIN1257_multiply	0xd7	/*  215		*/
#   define	WIN1257_Uogonek		0xd8	/*  216		*/
#   define	WIN1257_Lslash		0xd9	/*  217		*/
#   define	WIN1257_Sacute		0xda	/*  218		*/
#   define	WIN1257_Umacron		0xdb	/*  219		*/
#   define	WIN1257_Udieresis	0xdc	/*  220		*/
#   define	WIN1257_Zdotaccent	0xdd	/*  221		*/
#   define	WIN1257_Zcaron		0xde	/*  222		*/
#   define	WIN1257_germandbls	0xdf	/*  223		*/
#   define	WIN1257_aogonek		0xe0	/*  224		*/
#   define	WIN1257_iogonek		0xe1	/*  225		*/
#   define	WIN1257_amacron		0xe2	/*  226		*/
#   define	WIN1257_cacute		0xe3	/*  227		*/
#   define	WIN1257_adieresis	0xe4	/*  228		*/
#   define	WIN1257_aring		0xe5	/*  229		*/
#   define	WIN1257_eogonek		0xe6	/*  230		*/
#   define	WIN1257_emacron		0xe7	/*  231		*/
#   define	WIN1257_ccaron		0xe8	/*  232		*/
#   define	WIN1257_eacute		0xe9	/*  233		*/
#   define	WIN1257_zacute		0xea	/*  234		*/
#   define	WIN1257_edotaccent	0xeb	/*  235		*/
#   define	WIN1257_gcommaaccent	0xec	/*  236		*/
#   define	WIN1257_kcommaaccent	0xed	/*  237		*/
#   define	WIN1257_imacron		0xee	/*  238		*/
#   define	WIN1257_lcommaaccent	0xef	/*  239		*/
#   define	WIN1257_scaron		0xf0	/*  240		*/
#   define	WIN1257_nacute		0xf1	/*  241		*/
#   define	WIN1257_ncommaaccent	0xf2	/*  242		*/
#   define	WIN1257_oacute		0xf3	/*  243		*/
#   define	WIN1257_omacron		0xf4	/*  244		*/
#   define	WIN1257_otilde		0xf5	/*  245		*/
#   define	WIN1257_odieresis	0xf6	/*  246		*/
#   define	WIN1257_divide		0xf7	/*  247		*/
#   define	WIN1257_uogonek		0xf8	/*  248		*/
#   define	WIN1257_lslash		0xf9	/*  249		*/
#   define	WIN1257_sacute		0xfa	/*  250		*/
#   define	WIN1257_umacron		0xfb	/*  251		*/
#   define	WIN1257_udieresis	0xfc	/*  252		*/
#   define	WIN1257_zdotaccent	0xfd	/*  253		*/
#   define	WIN1257_zcaron		0xfe	/*  254		*/
#   define	WIN1257_dotaccent	0xff	/*  255		*/

#   define	ISO13_space		0x20	/*   32		*/
#   define	ISO13_exclam		0x21	/*   33		*/
#   define	ISO13_quotedbl		0x22	/*   34		*/
#   define	ISO13_numbersign	0x23	/*   35		*/
#   define	ISO13_dollar		0x24	/*   36		*/
#   define	ISO13_percent		0x25	/*   37		*/
#   define	ISO13_ampersand		0x26	/*   38		*/
#   define	ISO13_quotesingle	0x27	/*   39		*/
#   define	ISO13_parenleft		0x28	/*   40		*/
#   define	ISO13_parenright	0x29	/*   41		*/
#   define	ISO13_asterisk		0x2a	/*   42		*/
#   define	ISO13_plus		0x2b	/*   43		*/
#   define	ISO13_comma		0x2c	/*   44		*/
#   define	ISO13_minus		0x2d	/*   45		*/
#   define	ISO13_period		0x2e	/*   46		*/
#   define	ISO13_slash		0x2f	/*   47		*/
#   define	ISO13_zero		0x30	/*   48		*/
#   define	ISO13_one		0x31	/*   49		*/
#   define	ISO13_two		0x32	/*   50		*/
#   define	ISO13_three		0x33	/*   51		*/
#   define	ISO13_four		0x34	/*   52		*/
#   define	ISO13_five		0x35	/*   53		*/
#   define	ISO13_six		0x36	/*   54		*/
#   define	ISO13_seven		0x37	/*   55		*/
#   define	ISO13_eight		0x38	/*   56		*/
#   define	ISO13_nine		0x39	/*   57		*/
#   define	ISO13_colon		0x3a	/*   58		*/
#   define	ISO13_semicolon		0x3b	/*   59		*/
#   define	ISO13_less		0x3c	/*   60		*/
#   define	ISO13_equal		0x3d	/*   61		*/
#   define	ISO13_greater		0x3e	/*   62		*/
#   define	ISO13_question		0x3f	/*   63		*/
#   define	ISO13_at		0x40	/*   64		*/
#   define	ISO13_A			0x41	/*   65		*/
#   define	ISO13_B			0x42	/*   66		*/
#   define	ISO13_C			0x43	/*   67		*/
#   define	ISO13_D			0x44	/*   68		*/
#   define	ISO13_E			0x45	/*   69		*/
#   define	ISO13_F			0x46	/*   70		*/
#   define	ISO13_G			0x47	/*   71		*/
#   define	ISO13_H			0x48	/*   72		*/
#   define	ISO13_I			0x49	/*   73		*/
#   define	ISO13_J			0x4a	/*   74		*/
#   define	ISO13_K			0x4b	/*   75		*/
#   define	ISO13_L			0x4c	/*   76		*/
#   define	ISO13_M			0x4d	/*   77		*/
#   define	ISO13_N			0x4e	/*   78		*/
#   define	ISO13_O			0x4f	/*   79		*/
#   define	ISO13_P			0x50	/*   80		*/
#   define	ISO13_Q			0x51	/*   81		*/
#   define	ISO13_R			0x52	/*   82		*/
#   define	ISO13_S			0x53	/*   83		*/
#   define	ISO13_T			0x54	/*   84		*/
#   define	ISO13_U			0x55	/*   85		*/
#   define	ISO13_V			0x56	/*   86		*/
#   define	ISO13_W			0x57	/*   87		*/
#   define	ISO13_X			0x58	/*   88		*/
#   define	ISO13_Y			0x59	/*   89		*/
#   define	ISO13_Z			0x5a	/*   90		*/
#   define	ISO13_bracketleft	0x5b	/*   91		*/
#   define	ISO13_backslash		0x5c	/*   92		*/
#   define	ISO13_bracketright	0x5d	/*   93		*/
#   define	ISO13_asciicircum	0x5e	/*   94		*/
#   define	ISO13_underscore	0x5f	/*   95		*/
#   define	ISO13_grave		0x60	/*   96		*/
#   define	ISO13_a			0x61	/*   97		*/
#   define	ISO13_b			0x62	/*   98		*/
#   define	ISO13_c			0x63	/*   99		*/
#   define	ISO13_d			0x64	/*  100		*/
#   define	ISO13_e			0x65	/*  101		*/
#   define	ISO13_f			0x66	/*  102		*/
#   define	ISO13_g			0x67	/*  103		*/
#   define	ISO13_h			0x68	/*  104		*/
#   define	ISO13_i			0x69	/*  105		*/
#   define	ISO13_j			0x6a	/*  106		*/
#   define	ISO13_k			0x6b	/*  107		*/
#   define	ISO13_l			0x6c	/*  108		*/
#   define	ISO13_m			0x6d	/*  109		*/
#   define	ISO13_n			0x6e	/*  110		*/
#   define	ISO13_o			0x6f	/*  111		*/
#   define	ISO13_p			0x70	/*  112		*/
#   define	ISO13_q			0x71	/*  113		*/
#   define	ISO13_r			0x72	/*  114		*/
#   define	ISO13_s			0x73	/*  115		*/
#   define	ISO13_t			0x74	/*  116		*/
#   define	ISO13_u			0x75	/*  117		*/
#   define	ISO13_v			0x76	/*  118		*/
#   define	ISO13_w			0x77	/*  119		*/
#   define	ISO13_x			0x78	/*  120		*/
#   define	ISO13_y			0x79	/*  121		*/
#   define	ISO13_z			0x7a	/*  122		*/
#   define	ISO13_braceleft		0x7b	/*  123		*/
#   define	ISO13_bar		0x7c	/*  124		*/
#   define	ISO13_braceright	0x7d	/*  125		*/
#   define	ISO13_asciitilde	0x7e	/*  126		*/
#   define	ISO13__0x7F		0x7f	/*  127		*/
#   define	ISO13__0x80		0x80	/*  128		*/
#   define	ISO13__0x81		0x81	/*  129		*/
#   define	ISO13__0x82		0x82	/*  130		*/
#   define	ISO13__0x83		0x83	/*  131		*/
#   define	ISO13__0x84		0x84	/*  132		*/
#   define	ISO13__0x85		0x85	/*  133		*/
#   define	ISO13__0x86		0x86	/*  134		*/
#   define	ISO13__0x87		0x87	/*  135		*/
#   define	ISO13__0x88		0x88	/*  136		*/
#   define	ISO13__0x89		0x89	/*  137		*/
#   define	ISO13__0x8A		0x8a	/*  138		*/
#   define	ISO13__0x8B		0x8b	/*  139		*/
#   define	ISO13__0x8C		0x8c	/*  140		*/
#   define	ISO13__0x8D		0x8d	/*  141		*/
#   define	ISO13__0x8E		0x8e	/*  142		*/
#   define	ISO13__0x8F		0x8f	/*  143		*/
#   define	ISO13__0x90		0x90	/*  144		*/
#   define	ISO13__0x91		0x91	/*  145		*/
#   define	ISO13__0x92		0x92	/*  146		*/
#   define	ISO13__0x93		0x93	/*  147		*/
#   define	ISO13__0x94		0x94	/*  148		*/
#   define	ISO13__0x95		0x95	/*  149		*/
#   define	ISO13__0x96		0x96	/*  150		*/
#   define	ISO13__0x97		0x97	/*  151		*/
#   define	ISO13__0x98		0x98	/*  152		*/
#   define	ISO13__0x99		0x99	/*  153		*/
#   define	ISO13__0x9A		0x9a	/*  154		*/
#   define	ISO13__0x9B		0x9b	/*  155		*/
#   define	ISO13__0x9C		0x9c	/*  156		*/
#   define	ISO13__0x9D		0x9d	/*  157		*/
#   define	ISO13__0x9E		0x9e	/*  158		*/
#   define	ISO13__0x9F		0x9f	/*  159		*/
#   define	ISO13_nobreakspace	0xa0	/*  160		*/
#   define	ISO13_quotedblright	0xa1	/*  161		*/
#   define	ISO13_cent		0xa2	/*  162		*/
#   define	ISO13_sterling		0xa3	/*  163		*/
#   define	ISO13_currency		0xa4	/*  164		*/
#   define	ISO13_quotedblbase	0xa5	/*  165		*/
#   define	ISO13_brokenbar		0xa6	/*  166		*/
#   define	ISO13_section		0xa7	/*  167		*/
#   define	ISO13_Oslash		0xa8	/*  168		*/
#   define	ISO13_copyright		0xa9	/*  169		*/
#   define	ISO13_Rcommaaccent	0xaa	/*  170		*/
#   define	ISO13_guillemotleft	0xab	/*  171		*/
#   define	ISO13_logicalnot	0xac	/*  172		*/
#   define	ISO13_hyphen		0xad	/*  173		*/
#   define	ISO13_registered	0xae	/*  174		*/
#   define	ISO13_AE		0xaf	/*  175		*/
#   define	ISO13_degree		0xb0	/*  176		*/
#   define	ISO13_plusminus		0xb1	/*  177		*/
#   define	ISO13_twosuperior	0xb2	/*  178		*/
#   define	ISO13_threesuperior	0xb3	/*  179		*/
#   define	ISO13_quotedblleft	0xb4	/*  180		*/
#   define	ISO13_mu		0xb5	/*  181		*/
#   define	ISO13_paragraph		0xb6	/*  182		*/
#   define	ISO13_periodcentered	0xb7	/*  183		*/
#   define	ISO13_oslash		0xb8	/*  184		*/
#   define	ISO13_onesuperior	0xb9	/*  185		*/
#   define	ISO13_rcommaaccent	0xba	/*  186		*/
#   define	ISO13_guillemotright	0xbb	/*  187		*/
#   define	ISO13_onequarter	0xbc	/*  188		*/
#   define	ISO13_onehalf		0xbd	/*  189		*/
#   define	ISO13_threequarters	0xbe	/*  190		*/
#   define	ISO13_ae		0xbf	/*  191		*/
#   define	ISO13_Aogonek		0xc0	/*  192		*/
#   define	ISO13_Iogonek		0xc1	/*  193		*/
#   define	ISO13_Amacron		0xc2	/*  194		*/
#   define	ISO13_Cacute		0xc3	/*  195		*/
#   define	ISO13_Adieresis		0xc4	/*  196		*/
#   define	ISO13_Aring		0xc5	/*  197		*/
#   define	ISO13_Eogonek		0xc6	/*  198		*/
#   define	ISO13_Emacron		0xc7	/*  199		*/
#   define	ISO13_Ccaron		0xc8	/*  200		*/
#   define	ISO13_Eacute		0xc9	/*  201		*/
#   define	ISO13_Zacute		0xca	/*  202		*/
#   define	ISO13_Edotaccent	0xcb	/*  203		*/
#   define	ISO13_Gcommaaccent	0xcc	/*  204		*/
#   define	ISO13_Kcommaaccent	0xcd	/*  205		*/
#   define	ISO13_Imacron		0xce	/*  206		*/
#   define	ISO13_Lcommaaccent	0xcf	/*  207		*/
#   define	ISO13_Scaron		0xd0	/*  208		*/
#   define	ISO13_Nacute		0xd1	/*  209		*/
#   define	ISO13_Ncommaaccent	0xd2	/*  210		*/
#   define	ISO13_Oacute		0xd3	/*  211		*/
#   define	ISO13_Omacron		0xd4	/*  212		*/
#   define	ISO13_Otilde		0xd5	/*  213		*/
#   define	ISO13_Odieresis		0xd6	/*  214		*/
#   define	ISO13_multiply		0xd7	/*  215		*/
#   define	ISO13_Uogonek		0xd8	/*  216		*/
#   define	ISO13_Lslash		0xd9	/*  217		*/
#   define	ISO13_Sacute		0xda	/*  218		*/
#   define	ISO13_Umacron		0xdb	/*  219		*/
#   define	ISO13_Udieresis		0xdc	/*  220		*/
#   define	ISO13_Zdotaccent	0xdd	/*  221		*/
#   define	ISO13_Zcaron		0xde	/*  222		*/
#   define	ISO13_germandbls	0xdf	/*  223		*/
#   define	ISO13_aogonek		0xe0	/*  224		*/
#   define	ISO13_iogonek		0xe1	/*  225		*/
#   define	ISO13_amacron		0xe2	/*  226		*/
#   define	ISO13_cacute		0xe3	/*  227		*/
#   define	ISO13_adieresis		0xe4	/*  228		*/
#   define	ISO13_aring		0xe5	/*  229		*/
#   define	ISO13_eogonek		0xe6	/*  230		*/
#   define	ISO13_emacron		0xe7	/*  231		*/
#   define	ISO13_ccaron		0xe8	/*  232		*/
#   define	ISO13_eacute		0xe9	/*  233		*/
#   define	ISO13_zacute		0xea	/*  234		*/
#   define	ISO13_edotaccent	0xeb	/*  235		*/
#   define	ISO13_gcommaaccent	0xec	/*  236		*/
#   define	ISO13_kcommaaccent	0xed	/*  237		*/
#   define	ISO13_imacron		0xee	/*  238		*/
#   define	ISO13_lcommaaccent	0xef	/*  239		*/
#   define	ISO13_scaron		0xf0	/*  240		*/
#   define	ISO13_nacute		0xf1	/*  241		*/
#   define	ISO13_ncommaaccent	0xf2	/*  242		*/
#   define	ISO13_oacute		0xf3	/*  243		*/
#   define	ISO13_omacron		0xf4	/*  244		*/
#   define	ISO13_otilde		0xf5	/*  245		*/
#   define	ISO13_odieresis		0xf6	/*  246		*/
#   define	ISO13_divide		0xf7	/*  247		*/
#   define	ISO13_uogonek		0xf8	/*  248		*/
#   define	ISO13_lslash		0xf9	/*  249		*/
#   define	ISO13_sacute		0xfa	/*  250		*/
#   define	ISO13_umacron		0xfb	/*  251		*/
#   define	ISO13_udieresis		0xfc	/*  252		*/
#   define	ISO13_zdotaccent	0xfd	/*  253		*/
#   define	ISO13_zcaron		0xfe	/*  254		*/
#   define	ISO13_quoteright	0xff	/*  255		*/

#   define	ISO13_isupper(c) \
			((ASCII_isupper((c)))|| \
			((c)==ISO13_Oslash)||\
			((c)==ISO13_Rcommaaccent)||\
			((c)==ISO13_AE)||\
			((c)>=ISO13_Aogonek&&(c)<=ISO13_Odieresis)||\
			((c)>=ISO13_Uogonek&&(c)<=ISO13_germandbls))

#   define	ISO13_islower(c) \
			((ASCII_islower((c)))|| \
			((c)==ISO13_oslash)||\
			((c)==ISO13_rcommaaccent)||\
			((c)==ISO13_ae)||\
			((c)>=ISO13_germandbls&&(c)<=ISO13_odieresis)||\
			((c)>=ISO13_Uogonek&&(c)<=ISO13_zcaron))

#   define	ISO13_isalpha(c) \
			(ISO13_isupper((c))||ISO13_islower((c)))

#   define	ISO13_isdigit(c) \
			(ASCII_isdigit((c)))

#   define	ISO13_isalnum(c) \
		(ISO13_isupper((c))||ISO13_islower((c))|| ISO13_isdigit((c)))

#   define	ISO13_tolower(c)	(\
		ASCII_islower(c)?ASCII_tolower(c): (\
		((c)==ISO13_Oslash||(c)==ISO13_Rcommaaccent||(c)==ISO13_AE)? \
								((c)+0x10): (\
		((c)>=ISO13_Aogonek&&(c)<=ISO13_Odieresis)?((c)+0x20): (\
		((c)>=ISO13_Uogonek&&(c)<=ISO13_Zcaron)?((c)+0x20): (\
		(c) )))))

#   define	ISO13_toupper(c)	(\
		ASCII_islower(c)?ASCII_toupper(c): (\
		((c)==ISO13_oslash||(c)==ISO13_rcommaaccent||(c)==ISO13_ae)? \
								((c)-0x10): (\
		((c)>=ISO13_aogonek&&(c)<=ISO13_odieresis)?((c)-0x20): (\
		((c)>=ISO13_uogonek&&(c)<=ISO13_zcaron)?((c)-0x20): (\
		(c) )))))

/************************************************************************/
/*									*/
/*  Names for the characters in the Windows 1252 code page.		*/
/*  (Western European Latin1)						*/
/*									*/
/************************************************************************/

#   define	WIN1252_space			0x20	/*   32		*/
#   define	WIN1252_exclam			0x21	/*   33		*/
#   define	WIN1252_quotedbl		0x22	/*   34		*/
#   define	WIN1252_numbersign		0x23	/*   35		*/
#   define	WIN1252_dollar			0x24	/*   36		*/
#   define	WIN1252_percent			0x25	/*   37		*/
#   define	WIN1252_ampersand		0x26	/*   38		*/
#   define	WIN1252_quotesingle		0x27	/*   39		*/
#   define	WIN1252_parenleft		0x28	/*   40		*/
#   define	WIN1252_parenright		0x29	/*   41		*/
#   define	WIN1252_asterisk		0x2a	/*   42		*/
#   define	WIN1252_plus			0x2b	/*   43		*/
#   define	WIN1252_comma			0x2c	/*   44		*/
#   define	WIN1252_minus			0x2d	/*   45		*/
#   define	WIN1252_period			0x2e	/*   46		*/
#   define	WIN1252_slash			0x2f	/*   47		*/
#   define	WIN1252_zero			0x30	/*   48		*/
#   define	WIN1252_one			0x31	/*   49		*/
#   define	WIN1252_two			0x32	/*   50		*/
#   define	WIN1252_three			0x33	/*   51		*/
#   define	WIN1252_four			0x34	/*   52		*/
#   define	WIN1252_five			0x35	/*   53		*/
#   define	WIN1252_six			0x36	/*   54		*/
#   define	WIN1252_seven			0x37	/*   55		*/
#   define	WIN1252_eight			0x38	/*   56		*/
#   define	WIN1252_nine			0x39	/*   57		*/
#   define	WIN1252_colon			0x3a	/*   58		*/
#   define	WIN1252_semicolon		0x3b	/*   59		*/
#   define	WIN1252_less			0x3c	/*   60		*/
#   define	WIN1252_equal			0x3d	/*   61		*/
#   define	WIN1252_greater			0x3e	/*   62		*/
#   define	WIN1252_question		0x3f	/*   63		*/
#   define	WIN1252_at			0x40	/*   64		*/
#   define	WIN1252_A			0x41	/*   65		*/
#   define	WIN1252_B			0x42	/*   66		*/
#   define	WIN1252_C			0x43	/*   67		*/
#   define	WIN1252_D			0x44	/*   68		*/
#   define	WIN1252_E			0x45	/*   69		*/
#   define	WIN1252_F			0x46	/*   70		*/
#   define	WIN1252_G			0x47	/*   71		*/
#   define	WIN1252_H			0x48	/*   72		*/
#   define	WIN1252_I			0x49	/*   73		*/
#   define	WIN1252_J			0x4a	/*   74		*/
#   define	WIN1252_K			0x4b	/*   75		*/
#   define	WIN1252_L			0x4c	/*   76		*/
#   define	WIN1252_M			0x4d	/*   77		*/
#   define	WIN1252_N			0x4e	/*   78		*/
#   define	WIN1252_O			0x4f	/*   79		*/
#   define	WIN1252_P			0x50	/*   80		*/
#   define	WIN1252_Q			0x51	/*   81		*/
#   define	WIN1252_R			0x52	/*   82		*/
#   define	WIN1252_S			0x53	/*   83		*/
#   define	WIN1252_T			0x54	/*   84		*/
#   define	WIN1252_U			0x55	/*   85		*/
#   define	WIN1252_V			0x56	/*   86		*/
#   define	WIN1252_W			0x57	/*   87		*/
#   define	WIN1252_X			0x58	/*   88		*/
#   define	WIN1252_Y			0x59	/*   89		*/
#   define	WIN1252_Z			0x5a	/*   90		*/
#   define	WIN1252_bracketleft		0x5b	/*   91		*/
#   define	WIN1252_backslash		0x5c	/*   92		*/
#   define	WIN1252_bracketright		0x5d	/*   93		*/
#   define	WIN1252_asciicircum		0x5e	/*   94		*/
#   define	WIN1252_underscore		0x5f	/*   95		*/
#   define	WIN1252_quoteleft		0x60	/*   96		*/
#   define	WIN1252_a			0x61	/*   97		*/
#   define	WIN1252_b			0x62	/*   98		*/
#   define	WIN1252_c			0x63	/*   99		*/
#   define	WIN1252_d			0x64	/*  100		*/
#   define	WIN1252_e			0x65	/*  101		*/
#   define	WIN1252_f			0x66	/*  102		*/
#   define	WIN1252_g			0x67	/*  103		*/
#   define	WIN1252_h			0x68	/*  104		*/
#   define	WIN1252_i			0x69	/*  105		*/
#   define	WIN1252_j			0x6a	/*  106		*/
#   define	WIN1252_k			0x6b	/*  107		*/
#   define	WIN1252_l			0x6c	/*  108		*/
#   define	WIN1252_m			0x6d	/*  109		*/
#   define	WIN1252_n			0x6e	/*  110		*/
#   define	WIN1252_o			0x6f	/*  111		*/
#   define	WIN1252_p			0x70	/*  112		*/
#   define	WIN1252_q			0x71	/*  113		*/
#   define	WIN1252_r			0x72	/*  114		*/
#   define	WIN1252_s			0x73	/*  115		*/
#   define	WIN1252_t			0x74	/*  116		*/
#   define	WIN1252_u			0x75	/*  117		*/
#   define	WIN1252_v			0x76	/*  118		*/
#   define	WIN1252_w			0x77	/*  119		*/
#   define	WIN1252_x			0x78	/*  120		*/
#   define	WIN1252_y			0x79	/*  121		*/
#   define	WIN1252_z			0x7a	/*  122		*/
#   define	WIN1252_braceleft		0x7b	/*  123		*/
#   define	WIN1252_bar			0x7c	/*  124		*/
#   define	WIN1252_braceright		0x7d	/*  125		*/
#   define	WIN1252_asciitilde		0x7e	/*  126		*/
#   define	WIN1252_Euro			0x80	/*  128		*/
#   define	WIN1252_quotesinglbase		0x82	/*  130		*/
#   define	WIN1252_florin			0x83	/*  131		*/
#   define	WIN1252_quotedblbase		0x84	/*  132		*/
#   define	WIN1252_ellipsis		0x85	/*  133		*/
#   define	WIN1252_dagger			0x86	/*  134		*/
#   define	WIN1252_daggerdbl		0x87	/*  135		*/
#   define	WIN1252_circumflex		0x88	/*  136		*/
#   define	WIN1252_perthousand		0x89	/*  137		*/
#   define	WIN1252_Scaron			0x8a	/*  138		*/
#   define	WIN1252_guilsinglleft		0x8b	/*  139		*/
#   define	WIN1252_OE			0x8c	/*  140		*/
#   define	WIN1252__quoteleft		0x91	/*  145		*/
#   define	WIN1252_quoteright		0x92	/*  146		*/
#   define	WIN1252_quotedblleft		0x93	/*  147		*/
#   define	WIN1252_quotedblright		0x94	/*  148		*/
#   define	WIN1252_bullet			0x95	/*  149		*/
#   define	WIN1252_endash			0x96	/*  150		*/
#   define	WIN1252_emdash			0x97	/*  151		*/
#   define	WIN1252_tilde			0x98	/*  152		*/
#   define	WIN1252_trademark		0x99	/*  153		*/
#   define	WIN1252_scaron			0x9a	/*  154		*/
#   define	WIN1252_guilsinglright		0x9b	/*  155		*/
#   define	WIN1252_oe			0x9c	/*  156		*/
#   define	WIN1252_Ydieresis		0x9f	/*  159		*/
#   define	WIN1252_nobreakspace		0xa0	/*  160		*/
#   define	WIN1252_exclamdown		0xa1	/*  161		*/
#   define	WIN1252_cent			0xa2	/*  162		*/
#   define	WIN1252_sterling		0xa3	/*  163		*/
#   define	WIN1252_currency		0xa4	/*  164		*/
#   define	WIN1252_yen			0xa5	/*  165		*/
#   define	WIN1252_brokenbar		0xa6	/*  166		*/
#   define	WIN1252_section			0xa7	/*  167		*/
#   define	WIN1252_dieresis		0xa8	/*  168		*/
#   define	WIN1252_copyright		0xa9	/*  169		*/
#   define	WIN1252_ordfeminine		0xaa	/*  170		*/
#   define	WIN1252_guillemotleft		0xab	/*  171		*/
#   define	WIN1252_logicalnot		0xac	/*  172		*/
#   define	WIN1252_registered		0xae	/*  174		*/
#   define	WIN1252_macron			0xaf	/*  175		*/
#   define	WIN1252_degree			0xb0	/*  176		*/
#   define	WIN1252_plusminus		0xb1	/*  177		*/
#   define	WIN1252_twosuperior		0xb2	/*  178		*/
#   define	WIN1252_threesuperior		0xb3	/*  179		*/
#   define	WIN1252_acute			0xb4	/*  180		*/
#   define	WIN1252_mu			0xb5	/*  181		*/
#   define	WIN1252_paragraph		0xb6	/*  182		*/
#   define	WIN1252_periodcentered		0xb7	/*  183		*/
#   define	WIN1252_cedilla			0xb8	/*  184		*/
#   define	WIN1252_onesuperior		0xb9	/*  185		*/
#   define	WIN1252_ordmasculine		0xba	/*  186		*/
#   define	WIN1252_guillemotright		0xbb	/*  187		*/
#   define	WIN1252_onequarter		0xbc	/*  188		*/
#   define	WIN1252_onehalf			0xbd	/*  189		*/
#   define	WIN1252_threequarters		0xbe	/*  190		*/
#   define	WIN1252_questiondown		0xbf	/*  191		*/
#   define	WIN1252_Agrave			0xc0	/*  192		*/
#   define	WIN1252_Aacute			0xc1	/*  193		*/
#   define	WIN1252_Acircumflex		0xc2	/*  194		*/
#   define	WIN1252_Atilde			0xc3	/*  195		*/
#   define	WIN1252_Adieresis		0xc4	/*  196		*/
#   define	WIN1252_Aring			0xc5	/*  197		*/
#   define	WIN1252_AE			0xc6	/*  198		*/
#   define	WIN1252_Ccedilla		0xc7	/*  199		*/
#   define	WIN1252_Egrave			0xc8	/*  200		*/
#   define	WIN1252_Eacute			0xc9	/*  201		*/
#   define	WIN1252_Ecircumflex		0xca	/*  202		*/
#   define	WIN1252_Edieresis		0xcb	/*  203		*/
#   define	WIN1252_Igrave			0xcc	/*  204		*/
#   define	WIN1252_Iacute			0xcd	/*  205		*/
#   define	WIN1252_Icircumflex		0xce	/*  206		*/
#   define	WIN1252_Idieresis		0xcf	/*  207		*/
#   define	WIN1252_Eth			0xd0	/*  208		*/
#   define	WIN1252_Ntilde			0xd1	/*  209		*/
#   define	WIN1252_Ograve			0xd2	/*  210		*/
#   define	WIN1252_Oacute			0xd3	/*  211		*/
#   define	WIN1252_Ocircumflex		0xd4	/*  212		*/
#   define	WIN1252_Otilde			0xd5	/*  213		*/
#   define	WIN1252_Odieresis		0xd6	/*  214		*/
#   define	WIN1252_multiply		0xd7	/*  215		*/
#   define	WIN1252_Oslash			0xd8	/*  216		*/
#   define	WIN1252_Ugrave			0xd9	/*  217		*/
#   define	WIN1252_Uacute			0xda	/*  218		*/
#   define	WIN1252_Ucircumflex		0xdb	/*  219		*/
#   define	WIN1252_Udieresis		0xdc	/*  220		*/
#   define	WIN1252_Yacute			0xdd	/*  221		*/
#   define	WIN1252_Thorn			0xde	/*  222		*/
#   define	WIN1252_germandbls		0xdf	/*  223		*/
#   define	WIN1252_agrave			0xe0	/*  224		*/
#   define	WIN1252_aacute			0xe1	/*  225		*/
#   define	WIN1252_acircumflex		0xe2	/*  226		*/
#   define	WIN1252_atilde			0xe3	/*  227		*/
#   define	WIN1252_adieresis		0xe4	/*  228		*/
#   define	WIN1252_aring			0xe5	/*  229		*/
#   define	WIN1252_ae			0xe6	/*  230		*/
#   define	WIN1252_ccedilla		0xe7	/*  231		*/
#   define	WIN1252_egrave			0xe8	/*  232		*/
#   define	WIN1252_eacute			0xe9	/*  233		*/
#   define	WIN1252_ecircumflex		0xea	/*  234		*/
#   define	WIN1252_edieresis		0xeb	/*  235		*/
#   define	WIN1252_igrave			0xec	/*  236		*/
#   define	WIN1252_iacute			0xed	/*  237		*/
#   define	WIN1252_icircumflex		0xee	/*  238		*/
#   define	WIN1252_idieresis		0xef	/*  239		*/
#   define	WIN1252_eth			0xf0	/*  240		*/
#   define	WIN1252_ntilde			0xf1	/*  241		*/
#   define	WIN1252_ograve			0xf2	/*  242		*/
#   define	WIN1252_oacute			0xf3	/*  243		*/
#   define	WIN1252_ocircumflex		0xf4	/*  244		*/
#   define	WIN1252_otilde			0xf5	/*  245		*/
#   define	WIN1252_odieresis		0xf6	/*  246		*/
#   define	WIN1252_divide			0xf7	/*  247		*/
#   define	WIN1252_oslash			0xf8	/*  248		*/
#   define	WIN1252_ugrave			0xf9	/*  249		*/
#   define	WIN1252_uacute			0xfa	/*  250		*/
#   define	WIN1252_ucircumflex		0xfb	/*  251		*/
#   define	WIN1252_udieresis		0xfc	/*  252		*/
#   define	WIN1252_yacute			0xfd	/*  253		*/
#   define	WIN1252_thorn			0xfe	/*  254		*/
#   define	WIN1252_ydieresis		0xff	/*  255		*/
