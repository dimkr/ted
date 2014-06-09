/************************************************************************/
/*									*/
/*  Font Encodings							*/
/*									*/
/************************************************************************/

#   include	"appUtilConfig.h"

#   include	<stdio.h>

#   include	"psFont.h"
#   include	<charnames.h>

#   include	<appDebugon.h>

/************************************************************************/
/*									*/
/*  Indirection for variable names.					*/
/*									*/
/************************************************************************/

static const char GLYPH_A[]= "A";
static const char GLYPH_AE[]= "AE";
static const char GLYPH_AEacute[]= "AEacute";
static const char GLYPH_Aacute[]= "Aacute";
static const char GLYPH_Abreve[]= "Abreve";
static const char GLYPH_Acircumflex[]= "Acircumflex";
static const char GLYPH_Adieresis[]= "Adieresis";
static const char GLYPH_Agrave[]= "Agrave";
static const char GLYPH_Alpha[]= "Alpha";
static const char GLYPH_Alphatonos[]= "Alphatonos";
static const char GLYPH_Amacron[]= "Amacron";
static const char GLYPH_Aogonek[]= "Aogonek";
static const char GLYPH_Aring[]= "Aring";
static const char GLYPH_Aringacute[]= "Aringacute";
static const char GLYPH_Atilde[]= "Atilde";
static const char GLYPH_B[]= "B";
static const char GLYPH_Beta[]= "Beta";
static const char GLYPH_C[]= "C";
static const char GLYPH_Cacute[]= "Cacute";
static const char GLYPH_Ccaron[]= "Ccaron";
static const char GLYPH_Ccedilla[]= "Ccedilla";
static const char GLYPH_Ccircumflex[]= "Ccircumflex";
static const char GLYPH_Cdotaccent[]= "Cdotaccent";
static const char GLYPH_Chi[]= "Chi";
static const char GLYPH_D[]= "D";
static const char GLYPH_Dcaron[]= "Dcaron";
static const char GLYPH_Dcroat[]= "Dcroat";
static const char GLYPH_Delta[]= "Delta";
static const char GLYPH_E[]= "E";
static const char GLYPH_Eacute[]= "Eacute";
static const char GLYPH_Ebreve[]= "Ebreve";
static const char GLYPH_Ecaron[]= "Ecaron";
static const char GLYPH_Ecircumflex[]= "Ecircumflex";
static const char GLYPH_Edieresis[]= "Edieresis";
static const char GLYPH_Edotaccent[]= "Edotaccent";
static const char GLYPH_Egrave[]= "Egrave";
static const char GLYPH_Emacron[]= "Emacron";
static const char GLYPH_Eng[]= "Eng";
static const char GLYPH_Eogonek[]= "Eogonek";
static const char GLYPH_Epsilon[]= "Epsilon";
static const char GLYPH_Epsilontonos[]= "Epsilontonos";
static const char GLYPH_Eta[]= "Eta";
static const char GLYPH_Etatonos[]= "Etatonos";
static const char GLYPH_Eth[]= "Eth";
static const char GLYPH_Euro[]= "Euro";
static const char GLYPH_F[]= "F";
static const char GLYPH_G[]= "G";
static const char GLYPH_Gamma[]= "Gamma";
static const char GLYPH_Gbreve[]= "Gbreve";
static const char GLYPH_Gcaron[]= "Gcaron";
static const char GLYPH_Gcircumflex[]= "Gcircumflex";
static const char GLYPH_Gcommaaccent[]= "Gcommaaccent";
static const char GLYPH_Gdotaccent[]= "Gdotaccent";
static const char GLYPH_H[]= "H";
static const char GLYPH_H18533[]= "H18533";
static const char GLYPH_H18543[]= "H18543";
static const char GLYPH_H18551[]= "H18551";
static const char GLYPH_H22073[]= "H22073";
static const char GLYPH_Hbar[]= "Hbar";
static const char GLYPH_Hcircumflex[]= "Hcircumflex";
static const char GLYPH_I[]= "I";
static const char GLYPH_IJ[]= "IJ";
static const char GLYPH_Iacute[]= "Iacute";
static const char GLYPH_Ibreve[]= "Ibreve";
static const char GLYPH_Icircumflex[]= "Icircumflex";
static const char GLYPH_Idieresis[]= "Idieresis";
static const char GLYPH_Idotaccent[]= "Idotaccent";
static const char GLYPH_Ifraktur[]= "Ifraktur";
static const char GLYPH_Igrave[]= "Igrave";
static const char GLYPH_Imacron[]= "Imacron";
static const char GLYPH_Iogonek[]= "Iogonek";
static const char GLYPH_Iota[]= "Iota";
static const char GLYPH_Iotadieresis[]= "Iotadieresis";
static const char GLYPH_Iotatonos[]= "Iotatonos";
static const char GLYPH_Itilde[]= "Itilde";
static const char GLYPH_J[]= "J";
static const char GLYPH_Jcircumflex[]= "Jcircumflex";
static const char GLYPH_K[]= "K";
static const char GLYPH_Kappa[]= "Kappa";
static const char GLYPH_Kcommaaccent[]= "Kcommaaccent";
static const char GLYPH_L[]= "L";
static const char GLYPH_Lacute[]= "Lacute";
static const char GLYPH_Lambda[]= "Lambda";
static const char GLYPH_Lcaron[]= "Lcaron";
static const char GLYPH_Lcommaaccent[]= "Lcommaaccent";
static const char GLYPH_Ldot[]= "Ldot";
static const char GLYPH_Lslash[]= "Lslash";
static const char GLYPH_M[]= "M";
static const char GLYPH_Mu[]= "Mu";
static const char GLYPH_N[]= "N";
static const char GLYPH_Nacute[]= "Nacute";
static const char GLYPH_Ncaron[]= "Ncaron";
static const char GLYPH_Ncommaaccent[]= "Ncommaaccent";
static const char GLYPH_Ntilde[]= "Ntilde";
static const char GLYPH_Nu[]= "Nu";
static const char GLYPH_O[]= "O";
static const char GLYPH_OE[]= "OE";
static const char GLYPH_Oacute[]= "Oacute";
static const char GLYPH_Obreve[]= "Obreve";
static const char GLYPH_Ocircumflex[]= "Ocircumflex";
static const char GLYPH_Odieresis[]= "Odieresis";
static const char GLYPH_Ograve[]= "Ograve";
static const char GLYPH_Ohorn[]= "Ohorn";
static const char GLYPH_Ohungarumlaut[]= "Ohungarumlaut";
static const char GLYPH_Omacron[]= "Omacron";
static const char GLYPH_Omega[]= "Omega";
static const char GLYPH_Omegatonos[]= "Omegatonos";
static const char GLYPH_Omicron[]= "Omicron";
static const char GLYPH_Omicrontonos[]= "Omicrontonos";
static const char GLYPH_Oslash[]= "Oslash";
static const char GLYPH_Oslashacute[]= "Oslashacute";
static const char GLYPH_Otilde[]= "Otilde";
static const char GLYPH_P[]= "P";
static const char GLYPH_Phi[]= "Phi";
static const char GLYPH_Pi[]= "Pi";
static const char GLYPH_Psi[]= "Psi";
static const char GLYPH_Q[]= "Q";
static const char GLYPH_R[]= "R";
static const char GLYPH_Racute[]= "Racute";
static const char GLYPH_Rcaron[]= "Rcaron";
static const char GLYPH_Rcommaaccent[]= "Rcommaaccent";
static const char GLYPH_Rfraktur[]= "Rfraktur";
static const char GLYPH_Rho[]= "Rho";
static const char GLYPH_S[]= "S";
static const char GLYPH_SF010000[]= "SF010000";
static const char GLYPH_SF020000[]= "SF020000";
static const char GLYPH_SF030000[]= "SF030000";
static const char GLYPH_SF040000[]= "SF040000";
static const char GLYPH_SF050000[]= "SF050000";
static const char GLYPH_SF060000[]= "SF060000";
static const char GLYPH_SF070000[]= "SF070000";
static const char GLYPH_SF080000[]= "SF080000";
static const char GLYPH_SF090000[]= "SF090000";
static const char GLYPH_SF100000[]= "SF100000";
static const char GLYPH_SF110000[]= "SF110000";
static const char GLYPH_SF190000[]= "SF190000";
static const char GLYPH_SF200000[]= "SF200000";
static const char GLYPH_SF210000[]= "SF210000";
static const char GLYPH_SF220000[]= "SF220000";
static const char GLYPH_SF230000[]= "SF230000";
static const char GLYPH_SF240000[]= "SF240000";
static const char GLYPH_SF250000[]= "SF250000";
static const char GLYPH_SF260000[]= "SF260000";
static const char GLYPH_SF270000[]= "SF270000";
static const char GLYPH_SF280000[]= "SF280000";
static const char GLYPH_SF360000[]= "SF360000";
static const char GLYPH_SF370000[]= "SF370000";
static const char GLYPH_SF380000[]= "SF380000";
static const char GLYPH_SF390000[]= "SF390000";
static const char GLYPH_SF400000[]= "SF400000";
static const char GLYPH_SF410000[]= "SF410000";
static const char GLYPH_SF420000[]= "SF420000";
static const char GLYPH_SF430000[]= "SF430000";
static const char GLYPH_SF440000[]= "SF440000";
static const char GLYPH_SF450000[]= "SF450000";
static const char GLYPH_SF460000[]= "SF460000";
static const char GLYPH_SF470000[]= "SF470000";
static const char GLYPH_SF480000[]= "SF480000";
static const char GLYPH_SF490000[]= "SF490000";
static const char GLYPH_SF500000[]= "SF500000";
static const char GLYPH_SF510000[]= "SF510000";
static const char GLYPH_SF520000[]= "SF520000";
static const char GLYPH_SF530000[]= "SF530000";
static const char GLYPH_SF540000[]= "SF540000";
static const char GLYPH_Sacute[]= "Sacute";
static const char GLYPH_Scaron[]= "Scaron";
static const char GLYPH_Scedilla[]= "Scedilla";
static const char GLYPH_Scircumflex[]= "Scircumflex";
static const char GLYPH_Scommaaccent[]= "Scommaaccent";
static const char GLYPH_Sigma[]= "Sigma";
static const char GLYPH_T[]= "T";
static const char GLYPH_Tau[]= "Tau";
static const char GLYPH_Tbar[]= "Tbar";
static const char GLYPH_Tcaron[]= "Tcaron";
static const char GLYPH_Tcommaaccent[]= "Tcommaaccent";
static const char GLYPH_Theta[]= "Theta";
static const char GLYPH_Thorn[]= "Thorn";
static const char GLYPH_U[]= "U";
static const char GLYPH_Uacute[]= "Uacute";
static const char GLYPH_Ubreve[]= "Ubreve";
static const char GLYPH_Ucircumflex[]= "Ucircumflex";
static const char GLYPH_Udieresis[]= "Udieresis";
static const char GLYPH_Ugrave[]= "Ugrave";
static const char GLYPH_Uhorn[]= "Uhorn";
static const char GLYPH_Uhungarumlaut[]= "Uhungarumlaut";
static const char GLYPH_Umacron[]= "Umacron";
static const char GLYPH_Uogonek[]= "Uogonek";
static const char GLYPH_Upsilon[]= "Upsilon";
static const char GLYPH_Upsilon1[]= "Upsilon1";
static const char GLYPH_Upsilondieresis[]= "Upsilondieresis";
static const char GLYPH_Upsilontonos[]= "Upsilontonos";
static const char GLYPH_Uring[]= "Uring";
static const char GLYPH_Utilde[]= "Utilde";
static const char GLYPH_V[]= "V";
static const char GLYPH_W[]= "W";
static const char GLYPH_Wacute[]= "Wacute";
static const char GLYPH_Wcircumflex[]= "Wcircumflex";
static const char GLYPH_Wdieresis[]= "Wdieresis";
static const char GLYPH_Wgrave[]= "Wgrave";
static const char GLYPH_X[]= "X";
static const char GLYPH_Xi[]= "Xi";
static const char GLYPH_Y[]= "Y";
static const char GLYPH_Yacute[]= "Yacute";
static const char GLYPH_Ycircumflex[]= "Ycircumflex";
static const char GLYPH_Ydieresis[]= "Ydieresis";
static const char GLYPH_Ygrave[]= "Ygrave";
static const char GLYPH_Z[]= "Z";
static const char GLYPH_Zacute[]= "Zacute";
static const char GLYPH_Zcaron[]= "Zcaron";
static const char GLYPH_Zdotaccent[]= "Zdotaccent";
static const char GLYPH_Zeta[]= "Zeta";
static const char GLYPH_a[]= "a";
static const char GLYPH_aacute[]= "aacute";
static const char GLYPH_abreve[]= "abreve";
static const char GLYPH_acircumflex[]= "acircumflex";
static const char GLYPH_acute[]= "acute";
static const char GLYPH_acutecomb[]= "acutecomb";
static const char GLYPH_adieresis[]= "adieresis";
static const char GLYPH_ae[]= "ae";
static const char GLYPH_aeacute[]= "aeacute";
static const char GLYPH_afii00208[]= "afii00208";
static const char GLYPH_afii10017[]= "afii10017";
static const char GLYPH_afii10018[]= "afii10018";
static const char GLYPH_afii10019[]= "afii10019";
static const char GLYPH_afii10020[]= "afii10020";
static const char GLYPH_afii10021[]= "afii10021";
static const char GLYPH_afii10022[]= "afii10022";
static const char GLYPH_afii10023[]= "afii10023";
static const char GLYPH_afii10024[]= "afii10024";
static const char GLYPH_afii10025[]= "afii10025";
static const char GLYPH_afii10026[]= "afii10026";
static const char GLYPH_afii10027[]= "afii10027";
static const char GLYPH_afii10028[]= "afii10028";
static const char GLYPH_afii10029[]= "afii10029";
static const char GLYPH_afii10030[]= "afii10030";
static const char GLYPH_afii10031[]= "afii10031";
static const char GLYPH_afii10032[]= "afii10032";
static const char GLYPH_afii10033[]= "afii10033";
static const char GLYPH_afii10034[]= "afii10034";
static const char GLYPH_afii10035[]= "afii10035";
static const char GLYPH_afii10036[]= "afii10036";
static const char GLYPH_afii10037[]= "afii10037";
static const char GLYPH_afii10038[]= "afii10038";
static const char GLYPH_afii10039[]= "afii10039";
static const char GLYPH_afii10040[]= "afii10040";
static const char GLYPH_afii10041[]= "afii10041";
static const char GLYPH_afii10042[]= "afii10042";
static const char GLYPH_afii10043[]= "afii10043";
static const char GLYPH_afii10044[]= "afii10044";
static const char GLYPH_afii10045[]= "afii10045";
static const char GLYPH_afii10046[]= "afii10046";
static const char GLYPH_afii10047[]= "afii10047";
static const char GLYPH_afii10048[]= "afii10048";
static const char GLYPH_afii10049[]= "afii10049";
static const char GLYPH_afii10050[]= "afii10050";
static const char GLYPH_afii10051[]= "afii10051";
static const char GLYPH_afii10052[]= "afii10052";
static const char GLYPH_afii10053[]= "afii10053";
static const char GLYPH_afii10054[]= "afii10054";
static const char GLYPH_afii10055[]= "afii10055";
static const char GLYPH_afii10056[]= "afii10056";
static const char GLYPH_afii10057[]= "afii10057";
static const char GLYPH_afii10058[]= "afii10058";
static const char GLYPH_afii10059[]= "afii10059";
static const char GLYPH_afii10060[]= "afii10060";
static const char GLYPH_afii10061[]= "afii10061";
static const char GLYPH_afii10062[]= "afii10062";
static const char GLYPH_afii10065[]= "afii10065";
static const char GLYPH_afii10066[]= "afii10066";
static const char GLYPH_afii10067[]= "afii10067";
static const char GLYPH_afii10068[]= "afii10068";
static const char GLYPH_afii10069[]= "afii10069";
static const char GLYPH_afii10070[]= "afii10070";
static const char GLYPH_afii10071[]= "afii10071";
static const char GLYPH_afii10072[]= "afii10072";
static const char GLYPH_afii10073[]= "afii10073";
static const char GLYPH_afii10074[]= "afii10074";
static const char GLYPH_afii10075[]= "afii10075";
static const char GLYPH_afii10076[]= "afii10076";
static const char GLYPH_afii10077[]= "afii10077";
static const char GLYPH_afii10078[]= "afii10078";
static const char GLYPH_afii10079[]= "afii10079";
static const char GLYPH_afii10080[]= "afii10080";
static const char GLYPH_afii10081[]= "afii10081";
static const char GLYPH_afii10082[]= "afii10082";
static const char GLYPH_afii10083[]= "afii10083";
static const char GLYPH_afii10084[]= "afii10084";
static const char GLYPH_afii10085[]= "afii10085";
static const char GLYPH_afii10086[]= "afii10086";
static const char GLYPH_afii10087[]= "afii10087";
static const char GLYPH_afii10088[]= "afii10088";
static const char GLYPH_afii10089[]= "afii10089";
static const char GLYPH_afii10090[]= "afii10090";
static const char GLYPH_afii10091[]= "afii10091";
static const char GLYPH_afii10092[]= "afii10092";
static const char GLYPH_afii10093[]= "afii10093";
static const char GLYPH_afii10094[]= "afii10094";
static const char GLYPH_afii10095[]= "afii10095";
static const char GLYPH_afii10096[]= "afii10096";
static const char GLYPH_afii10097[]= "afii10097";
static const char GLYPH_afii10098[]= "afii10098";
static const char GLYPH_afii10099[]= "afii10099";
static const char GLYPH_afii10100[]= "afii10100";
static const char GLYPH_afii10101[]= "afii10101";
static const char GLYPH_afii10102[]= "afii10102";
static const char GLYPH_afii10103[]= "afii10103";
static const char GLYPH_afii10104[]= "afii10104";
static const char GLYPH_afii10105[]= "afii10105";
static const char GLYPH_afii10106[]= "afii10106";
static const char GLYPH_afii10107[]= "afii10107";
static const char GLYPH_afii10108[]= "afii10108";
static const char GLYPH_afii10109[]= "afii10109";
static const char GLYPH_afii10110[]= "afii10110";
static const char GLYPH_afii10145[]= "afii10145";
static const char GLYPH_afii10146[]= "afii10146";
static const char GLYPH_afii10147[]= "afii10147";
static const char GLYPH_afii10148[]= "afii10148";
static const char GLYPH_afii10193[]= "afii10193";
static const char GLYPH_afii10194[]= "afii10194";
static const char GLYPH_afii10195[]= "afii10195";
static const char GLYPH_afii10196[]= "afii10196";
static const char GLYPH_afii10846[]= "afii10846";
static const char GLYPH_afii299[]= "afii299";
static const char GLYPH_afii300[]= "afii300";
static const char GLYPH_afii301[]= "afii301";
static const char GLYPH_afii57381[]= "afii57381";
static const char GLYPH_afii57388[]= "afii57388";
static const char GLYPH_afii57392[]= "afii57392";
static const char GLYPH_afii57393[]= "afii57393";
static const char GLYPH_afii57394[]= "afii57394";
static const char GLYPH_afii57395[]= "afii57395";
static const char GLYPH_afii57396[]= "afii57396";
static const char GLYPH_afii57397[]= "afii57397";
static const char GLYPH_afii57398[]= "afii57398";
static const char GLYPH_afii57399[]= "afii57399";
static const char GLYPH_afii57400[]= "afii57400";
static const char GLYPH_afii57401[]= "afii57401";
static const char GLYPH_afii57403[]= "afii57403";
static const char GLYPH_afii57407[]= "afii57407";
static const char GLYPH_afii57409[]= "afii57409";
static const char GLYPH_afii57410[]= "afii57410";
static const char GLYPH_afii57411[]= "afii57411";
static const char GLYPH_afii57412[]= "afii57412";
static const char GLYPH_afii57413[]= "afii57413";
static const char GLYPH_afii57414[]= "afii57414";
static const char GLYPH_afii57415[]= "afii57415";
static const char GLYPH_afii57416[]= "afii57416";
static const char GLYPH_afii57417[]= "afii57417";
static const char GLYPH_afii57418[]= "afii57418";
static const char GLYPH_afii57419[]= "afii57419";
static const char GLYPH_afii57420[]= "afii57420";
static const char GLYPH_afii57421[]= "afii57421";
static const char GLYPH_afii57422[]= "afii57422";
static const char GLYPH_afii57423[]= "afii57423";
static const char GLYPH_afii57424[]= "afii57424";
static const char GLYPH_afii57425[]= "afii57425";
static const char GLYPH_afii57426[]= "afii57426";
static const char GLYPH_afii57427[]= "afii57427";
static const char GLYPH_afii57428[]= "afii57428";
static const char GLYPH_afii57429[]= "afii57429";
static const char GLYPH_afii57430[]= "afii57430";
static const char GLYPH_afii57431[]= "afii57431";
static const char GLYPH_afii57432[]= "afii57432";
static const char GLYPH_afii57433[]= "afii57433";
static const char GLYPH_afii57434[]= "afii57434";
static const char GLYPH_afii57440[]= "afii57440";
static const char GLYPH_afii57441[]= "afii57441";
static const char GLYPH_afii57442[]= "afii57442";
static const char GLYPH_afii57443[]= "afii57443";
static const char GLYPH_afii57444[]= "afii57444";
static const char GLYPH_afii57445[]= "afii57445";
static const char GLYPH_afii57446[]= "afii57446";
static const char GLYPH_afii57448[]= "afii57448";
static const char GLYPH_afii57449[]= "afii57449";
static const char GLYPH_afii57450[]= "afii57450";
static const char GLYPH_afii57451[]= "afii57451";
static const char GLYPH_afii57452[]= "afii57452";
static const char GLYPH_afii57453[]= "afii57453";
static const char GLYPH_afii57454[]= "afii57454";
static const char GLYPH_afii57455[]= "afii57455";
static const char GLYPH_afii57456[]= "afii57456";
static const char GLYPH_afii57457[]= "afii57457";
static const char GLYPH_afii57458[]= "afii57458";
static const char GLYPH_afii57470[]= "afii57470";
static const char GLYPH_afii57505[]= "afii57505";
static const char GLYPH_afii57506[]= "afii57506";
static const char GLYPH_afii57507[]= "afii57507";
static const char GLYPH_afii57508[]= "afii57508";
static const char GLYPH_afii57509[]= "afii57509";
static const char GLYPH_afii57511[]= "afii57511";
static const char GLYPH_afii57512[]= "afii57512";
static const char GLYPH_afii57513[]= "afii57513";
static const char GLYPH_afii57514[]= "afii57514";
static const char GLYPH_afii57519[]= "afii57519";
static const char GLYPH_afii57534[]= "afii57534";
static const char GLYPH_afii57636[]= "afii57636";
static const char GLYPH_afii57645[]= "afii57645";
static const char GLYPH_afii57658[]= "afii57658";
static const char GLYPH_afii57664[]= "afii57664";
static const char GLYPH_afii57665[]= "afii57665";
static const char GLYPH_afii57666[]= "afii57666";
static const char GLYPH_afii57667[]= "afii57667";
static const char GLYPH_afii57668[]= "afii57668";
static const char GLYPH_afii57669[]= "afii57669";
static const char GLYPH_afii57670[]= "afii57670";
static const char GLYPH_afii57671[]= "afii57671";
static const char GLYPH_afii57672[]= "afii57672";
static const char GLYPH_afii57673[]= "afii57673";
static const char GLYPH_afii57674[]= "afii57674";
static const char GLYPH_afii57675[]= "afii57675";
static const char GLYPH_afii57676[]= "afii57676";
static const char GLYPH_afii57677[]= "afii57677";
static const char GLYPH_afii57678[]= "afii57678";
static const char GLYPH_afii57679[]= "afii57679";
static const char GLYPH_afii57680[]= "afii57680";
static const char GLYPH_afii57681[]= "afii57681";
static const char GLYPH_afii57682[]= "afii57682";
static const char GLYPH_afii57683[]= "afii57683";
static const char GLYPH_afii57684[]= "afii57684";
static const char GLYPH_afii57685[]= "afii57685";
static const char GLYPH_afii57686[]= "afii57686";
static const char GLYPH_afii57687[]= "afii57687";
static const char GLYPH_afii57688[]= "afii57688";
static const char GLYPH_afii57689[]= "afii57689";
static const char GLYPH_afii57690[]= "afii57690";
static const char GLYPH_afii57716[]= "afii57716";
static const char GLYPH_afii57717[]= "afii57717";
static const char GLYPH_afii57718[]= "afii57718";
static const char GLYPH_afii57793[]= "afii57793";
static const char GLYPH_afii57794[]= "afii57794";
static const char GLYPH_afii57795[]= "afii57795";
static const char GLYPH_afii57796[]= "afii57796";
static const char GLYPH_afii57797[]= "afii57797";
static const char GLYPH_afii57798[]= "afii57798";
static const char GLYPH_afii57799[]= "afii57799";
static const char GLYPH_afii57800[]= "afii57800";
static const char GLYPH_afii57801[]= "afii57801";
static const char GLYPH_afii57802[]= "afii57802";
static const char GLYPH_afii57803[]= "afii57803";
static const char GLYPH_afii57804[]= "afii57804";
static const char GLYPH_afii57806[]= "afii57806";
static const char GLYPH_afii57807[]= "afii57807";
static const char GLYPH_afii57839[]= "afii57839";
static const char GLYPH_afii57841[]= "afii57841";
static const char GLYPH_afii57842[]= "afii57842";
static const char GLYPH_afii57929[]= "afii57929";
static const char GLYPH_afii61248[]= "afii61248";
static const char GLYPH_afii61289[]= "afii61289";
static const char GLYPH_afii61352[]= "afii61352";
static const char GLYPH_afii61573[]= "afii61573";
static const char GLYPH_afii61574[]= "afii61574";
static const char GLYPH_afii61575[]= "afii61575";
static const char GLYPH_afii61664[]= "afii61664";
static const char GLYPH_afii63167[]= "afii63167";
static const char GLYPH_afii64937[]= "afii64937";
static const char GLYPH_agrave[]= "agrave";
static const char GLYPH_aleph[]= "aleph";
static const char GLYPH_alpha[]= "alpha";
static const char GLYPH_alphatonos[]= "alphatonos";
static const char GLYPH_amacron[]= "amacron";
static const char GLYPH_ampersand[]= "ampersand";
static const char GLYPH_angle[]= "angle";
static const char GLYPH_angleleft[]= "angleleft";
static const char GLYPH_angleright[]= "angleright";
static const char GLYPH_anoteleia[]= "anoteleia";
static const char GLYPH_aogonek[]= "aogonek";
static const char GLYPH_approxequal[]= "approxequal";
static const char GLYPH_aring[]= "aring";
static const char GLYPH_aringacute[]= "aringacute";
static const char GLYPH_arrowboth[]= "arrowboth";
static const char GLYPH_arrowdblboth[]= "arrowdblboth";
static const char GLYPH_arrowdbldown[]= "arrowdbldown";
static const char GLYPH_arrowdblleft[]= "arrowdblleft";
static const char GLYPH_arrowdblright[]= "arrowdblright";
static const char GLYPH_arrowdblup[]= "arrowdblup";
static const char GLYPH_arrowdown[]= "arrowdown";
static const char GLYPH_arrowleft[]= "arrowleft";
static const char GLYPH_arrowright[]= "arrowright";
static const char GLYPH_arrowup[]= "arrowup";
static const char GLYPH_arrowupdn[]= "arrowupdn";
static const char GLYPH_arrowupdnbse[]= "arrowupdnbse";
static const char GLYPH_asciicircum[]= "asciicircum";
static const char GLYPH_asciitilde[]= "asciitilde";
static const char GLYPH_asterisk[]= "asterisk";
static const char GLYPH_asteriskmath[]= "asteriskmath";
static const char GLYPH_at[]= "at";
static const char GLYPH_atilde[]= "atilde";
static const char GLYPH_b[]= "b";
static const char GLYPH_backslash[]= "backslash";
static const char GLYPH_bar[]= "bar";
static const char GLYPH_beta[]= "beta";
static const char GLYPH_block[]= "block";
static const char GLYPH_braceleft[]= "braceleft";
static const char GLYPH_braceright[]= "braceright";
static const char GLYPH_bracketleft[]= "bracketleft";
static const char GLYPH_bracketright[]= "bracketright";
static const char GLYPH_breve[]= "breve";
static const char GLYPH_brokenbar[]= "brokenbar";
static const char GLYPH_bullet[]= "bullet";
static const char GLYPH_c[]= "c";
static const char GLYPH_cacute[]= "cacute";
static const char GLYPH_caron[]= "caron";
static const char GLYPH_carriagereturn[]= "carriagereturn";
static const char GLYPH_ccaron[]= "ccaron";
static const char GLYPH_ccedilla[]= "ccedilla";
static const char GLYPH_ccircumflex[]= "ccircumflex";
static const char GLYPH_cdotaccent[]= "cdotaccent";
static const char GLYPH_cedilla[]= "cedilla";
static const char GLYPH_cent[]= "cent";
static const char GLYPH_chi[]= "chi";
static const char GLYPH_circle[]= "circle";
static const char GLYPH_circlemultiply[]= "circlemultiply";
static const char GLYPH_circleplus[]= "circleplus";
static const char GLYPH_circumflex[]= "circumflex";
static const char GLYPH_club[]= "club";
static const char GLYPH_colon[]= "colon";
static const char GLYPH_colonmonetary[]= "colonmonetary";
static const char GLYPH_comma[]= "comma";
static const char GLYPH_congruent[]= "congruent";
static const char GLYPH_copyright[]= "copyright";
static const char GLYPH_currency[]= "currency";
static const char GLYPH_d[]= "d";
static const char GLYPH_dagger[]= "dagger";
static const char GLYPH_daggerdbl[]= "daggerdbl";
static const char GLYPH_dcaron[]= "dcaron";
static const char GLYPH_dcroat[]= "dcroat";
static const char GLYPH_degree[]= "degree";
static const char GLYPH_delta[]= "delta";
static const char GLYPH_diamond[]= "diamond";
static const char GLYPH_dieresis[]= "dieresis";
static const char GLYPH_dieresistonos[]= "dieresistonos";
static const char GLYPH_divide[]= "divide";
static const char GLYPH_dkshade[]= "dkshade";
static const char GLYPH_dnblock[]= "dnblock";
static const char GLYPH_dollar[]= "dollar";
static const char GLYPH_dong[]= "dong";
static const char GLYPH_dotaccent[]= "dotaccent";
static const char GLYPH_dotbelowcomb[]= "dotbelowcomb";
static const char GLYPH_dotlessi[]= "dotlessi";
static const char GLYPH_dotmath[]= "dotmath";
static const char GLYPH_e[]= "e";
static const char GLYPH_eacute[]= "eacute";
static const char GLYPH_ebreve[]= "ebreve";
static const char GLYPH_ecaron[]= "ecaron";
static const char GLYPH_ecircumflex[]= "ecircumflex";
static const char GLYPH_edieresis[]= "edieresis";
static const char GLYPH_edotaccent[]= "edotaccent";
static const char GLYPH_egrave[]= "egrave";
static const char GLYPH_eight[]= "eight";
static const char GLYPH_element[]= "element";
static const char GLYPH_ellipsis[]= "ellipsis";
static const char GLYPH_emacron[]= "emacron";
static const char GLYPH_emdash[]= "emdash";
static const char GLYPH_emptyset[]= "emptyset";
static const char GLYPH_endash[]= "endash";
static const char GLYPH_eng[]= "eng";
static const char GLYPH_eogonek[]= "eogonek";
static const char GLYPH_epsilon[]= "epsilon";
static const char GLYPH_epsilontonos[]= "epsilontonos";
static const char GLYPH_equal[]= "equal";
static const char GLYPH_equivalence[]= "equivalence";
static const char GLYPH_estimated[]= "estimated";
static const char GLYPH_eta[]= "eta";
static const char GLYPH_etatonos[]= "etatonos";
static const char GLYPH_eth[]= "eth";
static const char GLYPH_exclam[]= "exclam";
static const char GLYPH_exclamdbl[]= "exclamdbl";
static const char GLYPH_exclamdown[]= "exclamdown";
static const char GLYPH_existential[]= "existential";
static const char GLYPH_f[]= "f";
static const char GLYPH_female[]= "female";
static const char GLYPH_figuredash[]= "figuredash";
static const char GLYPH_filledbox[]= "filledbox";
static const char GLYPH_filledrect[]= "filledrect";
static const char GLYPH_five[]= "five";
static const char GLYPH_fiveeighths[]= "fiveeighths";
static const char GLYPH_florin[]= "florin";
static const char GLYPH_four[]= "four";
static const char GLYPH_fraction[]= "fraction";
static const char GLYPH_franc[]= "franc";
static const char GLYPH_g[]= "g";
static const char GLYPH_gamma[]= "gamma";
static const char GLYPH_gbreve[]= "gbreve";
static const char GLYPH_gcaron[]= "gcaron";
static const char GLYPH_gcircumflex[]= "gcircumflex";
static const char GLYPH_gcommaaccent[]= "gcommaaccent";
static const char GLYPH_gdotaccent[]= "gdotaccent";
static const char GLYPH_germandbls[]= "germandbls";
static const char GLYPH_gradient[]= "gradient";
static const char GLYPH_grave[]= "grave";
static const char GLYPH_gravecomb[]= "gravecomb";
static const char GLYPH_greater[]= "greater";
static const char GLYPH_greaterequal[]= "greaterequal";
static const char GLYPH_guillemotleft[]= "guillemotleft";
static const char GLYPH_guillemotright[]= "guillemotright";
static const char GLYPH_guilsinglleft[]= "guilsinglleft";
static const char GLYPH_guilsinglright[]= "guilsinglright";
static const char GLYPH_h[]= "h";
static const char GLYPH_hbar[]= "hbar";
static const char GLYPH_hcircumflex[]= "hcircumflex";
static const char GLYPH_heart[]= "heart";
static const char GLYPH_hookabovecomb[]= "hookabovecomb";
static const char GLYPH_house[]= "house";
static const char GLYPH_hungarumlaut[]= "hungarumlaut";
static const char GLYPH_hyphen[]= "hyphen";
static const char GLYPH_i[]= "i";
static const char GLYPH_iacute[]= "iacute";
static const char GLYPH_ibreve[]= "ibreve";
static const char GLYPH_icircumflex[]= "icircumflex";
static const char GLYPH_idieresis[]= "idieresis";
static const char GLYPH_igrave[]= "igrave";
static const char GLYPH_ij[]= "ij";
static const char GLYPH_imacron[]= "imacron";
static const char GLYPH_infinity[]= "infinity";
static const char GLYPH_integral[]= "integral";
static const char GLYPH_integralbt[]= "integralbt";
static const char GLYPH_integraltp[]= "integraltp";
static const char GLYPH_intersection[]= "intersection";
static const char GLYPH_invbullet[]= "invbullet";
static const char GLYPH_invcircle[]= "invcircle";
static const char GLYPH_invsmileface[]= "invsmileface";
static const char GLYPH_iogonek[]= "iogonek";
static const char GLYPH_iota[]= "iota";
static const char GLYPH_iotadieresis[]= "iotadieresis";
static const char GLYPH_iotadieresistonos[]= "iotadieresistonos";
static const char GLYPH_iotatonos[]= "iotatonos";
static const char GLYPH_itilde[]= "itilde";
static const char GLYPH_j[]= "j";
static const char GLYPH_jcircumflex[]= "jcircumflex";
static const char GLYPH_k[]= "k";
static const char GLYPH_kappa[]= "kappa";
static const char GLYPH_kcommaaccent[]= "kcommaaccent";
static const char GLYPH_kgreenlandic[]= "kgreenlandic";
static const char GLYPH_l[]= "l";
static const char GLYPH_lacute[]= "lacute";
static const char GLYPH_lambda[]= "lambda";
static const char GLYPH_lcaron[]= "lcaron";
static const char GLYPH_lcommaaccent[]= "lcommaaccent";
static const char GLYPH_ldot[]= "ldot";
static const char GLYPH_less[]= "less";
static const char GLYPH_lessequal[]= "lessequal";
static const char GLYPH_lfblock[]= "lfblock";
static const char GLYPH_lira[]= "lira";
static const char GLYPH_logicaland[]= "logicaland";
static const char GLYPH_logicalnot[]= "logicalnot";
static const char GLYPH_logicalor[]= "logicalor";
static const char GLYPH_longs[]= "longs";
static const char GLYPH_lozenge[]= "lozenge";
static const char GLYPH_lslash[]= "lslash";
static const char GLYPH_ltshade[]= "ltshade";
static const char GLYPH_m[]= "m";
static const char GLYPH_macron[]= "macron";
static const char GLYPH_male[]= "male";
static const char GLYPH_minus[]= "minus";
static const char GLYPH_minute[]= "minute";
static const char GLYPH_mu[]= "mu";
static const char GLYPH_multiply[]= "multiply";
static const char GLYPH_musicalnote[]= "musicalnote";
static const char GLYPH_musicalnotedbl[]= "musicalnotedbl";
static const char GLYPH_n[]= "n";
static const char GLYPH_nacute[]= "nacute";
static const char GLYPH_napostrophe[]= "napostrophe";
static const char GLYPH_ncaron[]= "ncaron";
static const char GLYPH_ncommaaccent[]= "ncommaaccent";
static const char GLYPH_nine[]= "nine";
static const char GLYPH_notelement[]= "notelement";
static const char GLYPH_notequal[]= "notequal";
static const char GLYPH_notsubset[]= "notsubset";
static const char GLYPH_ntilde[]= "ntilde";
static const char GLYPH_nu[]= "nu";
static const char GLYPH_numbersign[]= "numbersign";
static const char GLYPH_o[]= "o";
static const char GLYPH_oacute[]= "oacute";
static const char GLYPH_obreve[]= "obreve";
static const char GLYPH_ocircumflex[]= "ocircumflex";
static const char GLYPH_odieresis[]= "odieresis";
static const char GLYPH_oe[]= "oe";
static const char GLYPH_ogonek[]= "ogonek";
static const char GLYPH_ograve[]= "ograve";
static const char GLYPH_ohorn[]= "ohorn";
static const char GLYPH_ohungarumlaut[]= "ohungarumlaut";
static const char GLYPH_omacron[]= "omacron";
static const char GLYPH_omega[]= "omega";
static const char GLYPH_omega1[]= "omega1";
static const char GLYPH_omegatonos[]= "omegatonos";
static const char GLYPH_omicron[]= "omicron";
static const char GLYPH_omicrontonos[]= "omicrontonos";
static const char GLYPH_one[]= "one";
static const char GLYPH_onedotenleader[]= "onedotenleader";
static const char GLYPH_oneeighth[]= "oneeighth";
static const char GLYPH_onehalf[]= "onehalf";
static const char GLYPH_onequarter[]= "onequarter";
static const char GLYPH_onethird[]= "onethird";
static const char GLYPH_openbullet[]= "openbullet";
static const char GLYPH_ordfeminine[]= "ordfeminine";
static const char GLYPH_ordmasculine[]= "ordmasculine";
static const char GLYPH_orthogonal[]= "orthogonal";
static const char GLYPH_oslash[]= "oslash";
static const char GLYPH_oslashacute[]= "oslashacute";
static const char GLYPH_otilde[]= "otilde";
static const char GLYPH_p[]= "p";
static const char GLYPH_paragraph[]= "paragraph";
static const char GLYPH_parenleft[]= "parenleft";
static const char GLYPH_parenright[]= "parenright";
static const char GLYPH_partialdiff[]= "partialdiff";
static const char GLYPH_percent[]= "percent";
static const char GLYPH_period[]= "period";
static const char GLYPH_periodcentered[]= "periodcentered";
static const char GLYPH_perpendicular[]= "perpendicular";
static const char GLYPH_perthousand[]= "perthousand";
static const char GLYPH_peseta[]= "peseta";
static const char GLYPH_phi[]= "phi";
static const char GLYPH_phi1[]= "phi1";
static const char GLYPH_pi[]= "pi";
static const char GLYPH_plus[]= "plus";
static const char GLYPH_plusminus[]= "plusminus";
static const char GLYPH_prescription[]= "prescription";
static const char GLYPH_product[]= "product";
static const char GLYPH_propersubset[]= "propersubset";
static const char GLYPH_propersuperset[]= "propersuperset";
static const char GLYPH_proportional[]= "proportional";
static const char GLYPH_psi[]= "psi";
static const char GLYPH_q[]= "q";
static const char GLYPH_question[]= "question";
static const char GLYPH_questiondown[]= "questiondown";
static const char GLYPH_quotedbl[]= "quotedbl";
static const char GLYPH_quotedblbase[]= "quotedblbase";
static const char GLYPH_quotedblleft[]= "quotedblleft";
static const char GLYPH_quotedblright[]= "quotedblright";
static const char GLYPH_quoteleft[]= "quoteleft";
static const char GLYPH_quotereversed[]= "quotereversed";
static const char GLYPH_quoteright[]= "quoteright";
static const char GLYPH_quotesinglbase[]= "quotesinglbase";
static const char GLYPH_quotesingle[]= "quotesingle";
static const char GLYPH_r[]= "r";
static const char GLYPH_racute[]= "racute";
static const char GLYPH_radical[]= "radical";
static const char GLYPH_rcaron[]= "rcaron";
static const char GLYPH_rcommaaccent[]= "rcommaaccent";
static const char GLYPH_reflexsubset[]= "reflexsubset";
static const char GLYPH_reflexsuperset[]= "reflexsuperset";
static const char GLYPH_registered[]= "registered";
static const char GLYPH_revlogicalnot[]= "revlogicalnot";
static const char GLYPH_rho[]= "rho";
static const char GLYPH_ring[]= "ring";
static const char GLYPH_rtblock[]= "rtblock";
static const char GLYPH_s[]= "s";
static const char GLYPH_sacute[]= "sacute";
static const char GLYPH_scaron[]= "scaron";
static const char GLYPH_scedilla[]= "scedilla";
static const char GLYPH_scircumflex[]= "scircumflex";
static const char GLYPH_scommaaccent[]= "scommaaccent";
static const char GLYPH_second[]= "second";
static const char GLYPH_section[]= "section";
static const char GLYPH_semicolon[]= "semicolon";
static const char GLYPH_seven[]= "seven";
static const char GLYPH_seveneighths[]= "seveneighths";
static const char GLYPH_shade[]= "shade";
static const char GLYPH_sigma[]= "sigma";
static const char GLYPH_sigma1[]= "sigma1";
static const char GLYPH_similar[]= "similar";
static const char GLYPH_six[]= "six";
static const char GLYPH_slash[]= "slash";
static const char GLYPH_smileface[]= "smileface";
static const char GLYPH_space[]= "space";
static const char GLYPH_spade[]= "spade";
static const char GLYPH_sterling[]= "sterling";
static const char GLYPH_suchthat[]= "suchthat";
static const char GLYPH_summation[]= "summation";
static const char GLYPH_sun[]= "sun";
static const char GLYPH_t[]= "t";
static const char GLYPH_tau[]= "tau";
static const char GLYPH_tbar[]= "tbar";
static const char GLYPH_tcaron[]= "tcaron";
static const char GLYPH_tcommaaccent[]= "tcommaaccent";
static const char GLYPH_therefore[]= "therefore";
static const char GLYPH_theta[]= "theta";
static const char GLYPH_theta1[]= "theta1";
static const char GLYPH_thorn[]= "thorn";
static const char GLYPH_three[]= "three";
static const char GLYPH_threeeighths[]= "threeeighths";
static const char GLYPH_threequarters[]= "threequarters";
static const char GLYPH_tilde[]= "tilde";
static const char GLYPH_tildecomb[]= "tildecomb";
static const char GLYPH_tonos[]= "tonos";
static const char GLYPH_trademark[]= "trademark";
static const char GLYPH_triagdn[]= "triagdn";
static const char GLYPH_triaglf[]= "triaglf";
static const char GLYPH_triagrt[]= "triagrt";
static const char GLYPH_triagup[]= "triagup";
static const char GLYPH_two[]= "two";
static const char GLYPH_twodotenleader[]= "twodotenleader";
static const char GLYPH_twothirds[]= "twothirds";
static const char GLYPH_u[]= "u";
static const char GLYPH_uacute[]= "uacute";
static const char GLYPH_ubreve[]= "ubreve";
static const char GLYPH_ucircumflex[]= "ucircumflex";
static const char GLYPH_udieresis[]= "udieresis";
static const char GLYPH_ugrave[]= "ugrave";
static const char GLYPH_uhorn[]= "uhorn";
static const char GLYPH_uhungarumlaut[]= "uhungarumlaut";
static const char GLYPH_umacron[]= "umacron";
static const char GLYPH_underscore[]= "underscore";
static const char GLYPH_underscoredbl[]= "underscoredbl";
static const char GLYPH_union[]= "union";
static const char GLYPH_universal[]= "universal";
static const char GLYPH_uogonek[]= "uogonek";
static const char GLYPH_upblock[]= "upblock";
static const char GLYPH_upsilon[]= "upsilon";
static const char GLYPH_upsilondieresis[]= "upsilondieresis";
static const char GLYPH_upsilondieresistonos[]= "upsilondieresistonos";
static const char GLYPH_upsilontonos[]= "upsilontonos";
static const char GLYPH_uring[]= "uring";
static const char GLYPH_utilde[]= "utilde";
static const char GLYPH_v[]= "v";
static const char GLYPH_w[]= "w";
static const char GLYPH_wacute[]= "wacute";
static const char GLYPH_wcircumflex[]= "wcircumflex";
static const char GLYPH_wdieresis[]= "wdieresis";
static const char GLYPH_weierstrass[]= "weierstrass";
static const char GLYPH_wgrave[]= "wgrave";
static const char GLYPH_x[]= "x";
static const char GLYPH_xi[]= "xi";
static const char GLYPH_y[]= "y";
static const char GLYPH_yacute[]= "yacute";
static const char GLYPH_ycircumflex[]= "ycircumflex";
static const char GLYPH_ydieresis[]= "ydieresis";
static const char GLYPH_yen[]= "yen";
static const char GLYPH_ygrave[]= "ygrave";
static const char GLYPH_z[]= "z";
static const char GLYPH_zacute[]= "zacute";
static const char GLYPH_zcaron[]= "zcaron";
static const char GLYPH_zdotaccent[]= "zdotaccent";
static const char GLYPH_zero[]= "zero";
static const char GLYPH_zeta[]= "zeta";

/*EXTRA*/

static const char GLYPH_onesuperior[]=		"onesuperior";
static const char GLYPH_twosuperior[]=		"twosuperior";
static const char GLYPH_threesuperior[]=	"threesuperior";
static const char GLYPH_radicalex[]=		"radicalex";
static const char GLYPH_arrowvertex[]=		"arrowvertex";
static const char GLYPH_arrowhorizex[]=		"arrowhorizex";
static const char GLYPH_registerserif[]=	"registerserif";
static const char GLYPH_copyrightserif[]=	"copyrightserif";
static const char GLYPH_trademarkserif[]=	"trademarkserif";
static const char GLYPH_registersans[]=		"registersans";
static const char GLYPH_copyrightsans[]=	"copyrightsans";
static const char GLYPH_trademarksans[]=	"trademarksans";
static const char GLYPH_parenlefttp[]=		"parenlefttp";
static const char GLYPH_parenleftex[]=		"parenleftex";
static const char GLYPH_parenleftbt[]=		"parenleftbt";
static const char GLYPH_bracketlefttp[]=	"bracketlefttp";
static const char GLYPH_bracketleftex[]=	"bracketleftex";
static const char GLYPH_bracketleftbt[]=	"bracketleftbt";
static const char GLYPH_bracelefttp[]=		"bracelefttp";
static const char GLYPH_braceleftmid[]=		"braceleftmid";
static const char GLYPH_braceleftbt[]=		"braceleftbt";
static const char GLYPH_braceex[]=		"braceex";
static const char GLYPH_integralex[]=		"integralex";
static const char GLYPH_parenrighttp[]=		"parenrighttp";
static const char GLYPH_parenrightex[]=		"parenrightex";
static const char GLYPH_parenrightbt[]=		"parenrightbt";
static const char GLYPH_bracketrighttp[]=	"bracketrighttp";
static const char GLYPH_bracketrightex[]=	"bracketrightex";
static const char GLYPH_bracketrightbt[]=	"bracketrightbt";
static const char GLYPH_bracerighttp[]=		"bracerighttp";
static const char GLYPH_bracerightmid[]=	"bracerightmid";
static const char GLYPH_bracerightbt[]=		"bracerightbt";

static const char GLYPH_nobreakspace[]=		"nobreakspace";

static const char GLYPH_a1[]=			"a1";
static const char GLYPH_a2[]=			"a2";
static const char GLYPH_a202[]=			"a202";
static const char GLYPH_a3[]=			"a3";
static const char GLYPH_a4[]=			"a4";
static const char GLYPH_a5[]=			"a5";
static const char GLYPH_a119[]=			"a119";
static const char GLYPH_a118[]=			"a118";
static const char GLYPH_a117[]=			"a117";
static const char GLYPH_a11[]=			"a11";
static const char GLYPH_a12[]=			"a12";
static const char GLYPH_a13[]=			"a13";
static const char GLYPH_a14[]=			"a14";
static const char GLYPH_a15[]=			"a15";
static const char GLYPH_a16[]=			"a16";
static const char GLYPH_a105[]=			"a105";
static const char GLYPH_a17[]=			"a17";
static const char GLYPH_a18[]=			"a18";
static const char GLYPH_a19[]=			"a19";
static const char GLYPH_a20[]=			"a20";
static const char GLYPH_a21[]=			"a21";
static const char GLYPH_a22[]=			"a22";
static const char GLYPH_a23[]=			"a23";
static const char GLYPH_a24[]=			"a24";
static const char GLYPH_a25[]=			"a25";
static const char GLYPH_a26[]=			"a26";
static const char GLYPH_a27[]=			"a27";
static const char GLYPH_a28[]=			"a28";
static const char GLYPH_a6[]=			"a6";
static const char GLYPH_a7[]=			"a7";
static const char GLYPH_a8[]=			"a8";
static const char GLYPH_a9[]=			"a9";
static const char GLYPH_a10[]=			"a10";
static const char GLYPH_a29[]=			"a29";
static const char GLYPH_a30[]=			"a30";
static const char GLYPH_a31[]=			"a31";
static const char GLYPH_a32[]=			"a32";
static const char GLYPH_a33[]=			"a33";
static const char GLYPH_a34[]=			"a34";
static const char GLYPH_a35[]=			"a35";
static const char GLYPH_a36[]=			"a36";
static const char GLYPH_a37[]=			"a37";
static const char GLYPH_a38[]=			"a38";
static const char GLYPH_a39[]=			"a39";
static const char GLYPH_a40[]=			"a40";
static const char GLYPH_a41[]=			"a41";
static const char GLYPH_a42[]=			"a42";
static const char GLYPH_a43[]=			"a43";
static const char GLYPH_a44[]=			"a44";
static const char GLYPH_a45[]=			"a45";
static const char GLYPH_a46[]=			"a46";
static const char GLYPH_a47[]=			"a47";
static const char GLYPH_a48[]=			"a48";
static const char GLYPH_a49[]=			"a49";
static const char GLYPH_a50[]=			"a50";
static const char GLYPH_a51[]=			"a51";
static const char GLYPH_a52[]=			"a52";
static const char GLYPH_a53[]=			"a53";
static const char GLYPH_a54[]=			"a54";
static const char GLYPH_a55[]=			"a55";
static const char GLYPH_a56[]=			"a56";
static const char GLYPH_a57[]=			"a57";
static const char GLYPH_a58[]=			"a58";
static const char GLYPH_a59[]=			"a59";
static const char GLYPH_a60[]=			"a60";
static const char GLYPH_a61[]=			"a61";
static const char GLYPH_a62[]=			"a62";
static const char GLYPH_a63[]=			"a63";
static const char GLYPH_a64[]=			"a64";
static const char GLYPH_a65[]=			"a65";
static const char GLYPH_a66[]=			"a66";
static const char GLYPH_a67[]=			"a67";
static const char GLYPH_a68[]=			"a68";
static const char GLYPH_a69[]=			"a69";
static const char GLYPH_a70[]=			"a70";
static const char GLYPH_a71[]=			"a71";
static const char GLYPH_a72[]=			"a72";
static const char GLYPH_a73[]=			"a73";
static const char GLYPH_a74[]=			"a74";
static const char GLYPH_a203[]=			"a203";
static const char GLYPH_a75[]=			"a75";
static const char GLYPH_a204[]=			"a204";
static const char GLYPH_a76[]=			"a76";
static const char GLYPH_a77[]=			"a77";
static const char GLYPH_a78[]=			"a78";
static const char GLYPH_a79[]=			"a79";
static const char GLYPH_a81[]=			"a81";
static const char GLYPH_a82[]=			"a82";
static const char GLYPH_a83[]=			"a83";
static const char GLYPH_a84[]=			"a84";
static const char GLYPH_a97[]=			"a97";
static const char GLYPH_a98[]=			"a98";
static const char GLYPH_a99[]=			"a99";
static const char GLYPH_a100[]=			"a100";
static const char GLYPH_a101[]=			"a101";
static const char GLYPH_a102[]=			"a102";
static const char GLYPH_a103[]=			"a103";
static const char GLYPH_a104[]=			"a104";
static const char GLYPH_a106[]=			"a106";
static const char GLYPH_a107[]=			"a107";
static const char GLYPH_a108[]=			"a108";
static const char GLYPH_a112[]=			"a112";
static const char GLYPH_a111[]=			"a111";
static const char GLYPH_a110[]=			"a110";
static const char GLYPH_a109[]=			"a109";
static const char GLYPH_a120[]=			"a120";
static const char GLYPH_a121[]=			"a121";
static const char GLYPH_a122[]=			"a122";
static const char GLYPH_a123[]=			"a123";
static const char GLYPH_a124[]=			"a124";
static const char GLYPH_a125[]=			"a125";
static const char GLYPH_a126[]=			"a126";
static const char GLYPH_a127[]=			"a127";
static const char GLYPH_a128[]=			"a128";
static const char GLYPH_a129[]=			"a129";
static const char GLYPH_a130[]=			"a130";
static const char GLYPH_a131[]=			"a131";
static const char GLYPH_a132[]=			"a132";
static const char GLYPH_a133[]=			"a133";
static const char GLYPH_a134[]=			"a134";
static const char GLYPH_a135[]=			"a135";
static const char GLYPH_a136[]=			"a136";
static const char GLYPH_a137[]=			"a137";
static const char GLYPH_a138[]=			"a138";
static const char GLYPH_a139[]=			"a139";
static const char GLYPH_a140[]=			"a140";
static const char GLYPH_a141[]=			"a141";
static const char GLYPH_a142[]=			"a142";
static const char GLYPH_a143[]=			"a143";
static const char GLYPH_a144[]=			"a144";
static const char GLYPH_a145[]=			"a145";
static const char GLYPH_a146[]=			"a146";
static const char GLYPH_a147[]=			"a147";
static const char GLYPH_a148[]=			"a148";
static const char GLYPH_a149[]=			"a149";
static const char GLYPH_a150[]=			"a150";
static const char GLYPH_a151[]=			"a151";
static const char GLYPH_a152[]=			"a152";
static const char GLYPH_a153[]=			"a153";
static const char GLYPH_a154[]=			"a154";
static const char GLYPH_a155[]=			"a155";
static const char GLYPH_a156[]=			"a156";
static const char GLYPH_a157[]=			"a157";
static const char GLYPH_a158[]=			"a158";
static const char GLYPH_a159[]=			"a159";
static const char GLYPH_a160[]=			"a160";
static const char GLYPH_a161[]=			"a161";
static const char GLYPH_a163[]=			"a163";
static const char GLYPH_a164[]=			"a164";
static const char GLYPH_a196[]=			"a196";
static const char GLYPH_a165[]=			"a165";
static const char GLYPH_a192[]=			"a192";
static const char GLYPH_a166[]=			"a166";
static const char GLYPH_a167[]=			"a167";
static const char GLYPH_a168[]=			"a168";
static const char GLYPH_a169[]=			"a169";
static const char GLYPH_a170[]=			"a170";
static const char GLYPH_a171[]=			"a171";
static const char GLYPH_a172[]=			"a172";
static const char GLYPH_a173[]=			"a173";
static const char GLYPH_a162[]=			"a162";
static const char GLYPH_a174[]=			"a174";
static const char GLYPH_a175[]=			"a175";
static const char GLYPH_a176[]=			"a176";
static const char GLYPH_a177[]=			"a177";
static const char GLYPH_a178[]=			"a178";
static const char GLYPH_a179[]=			"a179";
static const char GLYPH_a193[]=			"a193";
static const char GLYPH_a180[]=			"a180";
static const char GLYPH_a199[]=			"a199";
static const char GLYPH_a181[]=			"a181";
static const char GLYPH_a200[]=			"a200";
static const char GLYPH_a182[]=			"a182";
static const char GLYPH_a201[]=			"a201";
static const char GLYPH_a183[]=			"a183";
static const char GLYPH_a184[]=			"a184";
static const char GLYPH_a197[]=			"a197";
static const char GLYPH_a185[]=			"a185";
static const char GLYPH_a194[]=			"a194";
static const char GLYPH_a198[]=			"a198";
static const char GLYPH_a186[]=			"a186";
static const char GLYPH_a195[]=			"a195";
static const char GLYPH_a187[]=			"a187";
static const char GLYPH_a188[]=			"a188";
static const char GLYPH_a189[]=			"a189";
static const char GLYPH_a190[]=			"a190";
static const char GLYPH_a191[]=			"a191";

/************************************************************************/
/*									*/
/*  ISO Latin1 (ISO 8859-1) to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char *	const psIsoLatin1GlyphNames[256]=
    {
    (const char *)0,		/*  nul		0	*/
    (const char *)0,		/*  soh		1	*/
    (const char *)0,		/*  stx		2	*/
    (const char *)0,		/*  etx		3	*/
    (const char *)0,		/*  eot		4	*/
    (const char *)0,		/*  enq		5	*/
    (const char *)0,		/*  ack		6	*/
    (const char *)0,		/*  bel		7	*/
    (const char *)0,		/*  bs		8	*/
    (const char *)0,		/*  ht		9	*/
    (const char *)0,		/*  nl		10	*/
    (const char *)0,		/*  vt		11	*/
    (const char *)0,		/*  np		12	*/
    (const char *)0,		/*  cr		13	*/
    (const char *)0,		/*  so		14	*/
    (const char *)0,		/*  si		15	*/
    (const char *)0,		/*  dle		16	*/
    (const char *)0,		/*  dc1		17	*/
    (const char *)0,		/*  dc2		18	*/
    (const char *)0,		/*  dc3		19	*/
    (const char *)0,		/*  dc4		20	*/
    (const char *)0,		/*  nak		21	*/
    (const char *)0,		/*  syn		22	*/
    (const char *)0,		/*  etb		23	*/
    (const char *)0,		/*  can		24	*/
    (const char *)0,		/*  em		25	*/
    (const char *)0,		/*  sub		26	*/
    (const char *)0,		/*  esc		27	*/
    (const char *)0,		/*  fs		28	*/
    (const char *)0,		/*  gs		29	*/
    (const char *)0,		/*  rs		30	*/
    (const char *)0,		/*  us		31	*/
    GLYPH_space,		/*  32			*/
    GLYPH_exclam,		/*  33			*/
    GLYPH_quotedbl,		/*  34			*/
    GLYPH_numbersign,		/*  35			*/
    GLYPH_dollar,		/*  36			*/
    GLYPH_percent,		/*  37			*/
    GLYPH_ampersand,		/*  38			*/
    GLYPH_quoteright,		/*  39			*/
    GLYPH_parenleft,		/*  40			*/
    GLYPH_parenright,		/*  41			*/
    GLYPH_asterisk,		/*  42			*/
    GLYPH_plus,			/*  43			*/
    GLYPH_comma,		/*  44			*/
    GLYPH_minus,		/*  45			*/
    GLYPH_period,		/*  46			*/
    GLYPH_slash,		/*  47			*/
    GLYPH_zero,			/*  48			*/
    GLYPH_one,			/*  49			*/
    GLYPH_two,			/*  50			*/
    GLYPH_three,		/*  51			*/
    GLYPH_four,			/*  52			*/
    GLYPH_five,			/*  53			*/
    GLYPH_six,			/*  54			*/
    GLYPH_seven,		/*  55			*/
    GLYPH_eight,		/*  56			*/
    GLYPH_nine,			/*  57			*/
    GLYPH_colon,		/*  58			*/
    GLYPH_semicolon,		/*  59			*/
    GLYPH_less,			/*  60			*/
    GLYPH_equal,		/*  61			*/
    GLYPH_greater,		/*  62			*/
    GLYPH_question,		/*  63			*/
    GLYPH_at,			/*  64			*/
    GLYPH_A,			/*  65			*/
    GLYPH_B,			/*  66			*/
    GLYPH_C,			/*  67			*/
    GLYPH_D,			/*  68			*/
    GLYPH_E,			/*  69			*/
    GLYPH_F,			/*  70			*/
    GLYPH_G,			/*  71			*/
    GLYPH_H,			/*  72			*/
    GLYPH_I,			/*  73			*/
    GLYPH_J,			/*  74			*/
    GLYPH_K,			/*  75			*/
    GLYPH_L,			/*  76			*/
    GLYPH_M,			/*  77			*/
    GLYPH_N,			/*  78			*/
    GLYPH_O,			/*  79			*/
    GLYPH_P,			/*  80			*/
    GLYPH_Q,			/*  81			*/
    GLYPH_R,			/*  82			*/
    GLYPH_S,			/*  83			*/
    GLYPH_T,			/*  84			*/
    GLYPH_U,			/*  85			*/
    GLYPH_V,			/*  86			*/
    GLYPH_W,			/*  87			*/
    GLYPH_X,			/*  88			*/
    GLYPH_Y,			/*  89			*/
    GLYPH_Z,			/*  90			*/
    GLYPH_bracketleft,		/*  91			*/
    GLYPH_backslash,		/*  92			*/
    GLYPH_bracketright,		/*  93			*/
    GLYPH_asciicircum,		/*  94			*/
    GLYPH_underscore,		/*  95			*/
    GLYPH_quoteleft,		/*  96			*/
    GLYPH_a,			/*  97			*/
    GLYPH_b,			/*  98			*/
    GLYPH_c,			/*  99			*/
    GLYPH_d,			/*  100			*/
    GLYPH_e,			/*  101			*/
    GLYPH_f,			/*  102			*/
    GLYPH_g,			/*  103			*/
    GLYPH_h,			/*  104			*/
    GLYPH_i,			/*  105			*/
    GLYPH_j,			/*  106			*/
    GLYPH_k,			/*  107			*/
    GLYPH_l,			/*  108			*/
    GLYPH_m,			/*  109			*/
    GLYPH_n,			/*  110			*/
    GLYPH_o,			/*  111			*/
    GLYPH_p,			/*  112			*/
    GLYPH_q,			/*  113			*/
    GLYPH_r,			/*  114			*/
    GLYPH_s,			/*  115			*/
    GLYPH_t,			/*  116			*/
    GLYPH_u,			/*  117			*/
    GLYPH_v,			/*  118			*/
    GLYPH_w,			/*  119			*/
    GLYPH_x,			/*  120			*/
    GLYPH_y,			/*  121			*/
    GLYPH_z,			/*  122			*/
    GLYPH_braceleft,		/*  123			*/
    GLYPH_bar,			/*  124			*/
    GLYPH_braceright,		/*  125			*/
    GLYPH_asciitilde,		/*  126			*/
    (char *)0,			/*  127			*/
    (char *)0,			/*  128			*/
    (char *)0,			/*  129			*/
    (char *)0,			/*  130			*/
    (char *)0,			/*  131			*/
    (char *)0,			/*  132			*/
    (char *)0,			/*  133			*/
    (char *)0,			/*  134			*/
    (char *)0,			/*  135			*/
    (char *)0,			/*  136			*/
    (char *)0,			/*  137			*/
    (char *)0,			/*  138			*/
    (char *)0,			/*  139			*/
    (char *)0,			/*  140			*/
    (char *)0,			/*  141			*/
    (char *)0,			/*  142			*/
    (char *)0,			/*  143			*/
    GLYPH_dotlessi,		/*  144			*/
    GLYPH_grave,		/*  145			*/
    GLYPH_acute,		/*  146			*/
    GLYPH_circumflex,		/*  147			*/
    GLYPH_tilde,		/*  148			*/
    GLYPH_macron,		/*  149			*/
    (char *)0,			/*  150			*/
    (char *)0,			/*  151			*/
    GLYPH_dieresis,		/*  152			*/
    (char *)0,			/*  153			*/
    GLYPH_ring,			/*  154			*/
    GLYPH_cedilla,		/*  155			*/
    (char *)0,			/*  156			*/
    GLYPH_hungarumlaut,		/*  157			*/
    GLYPH_ogonek,		/*  158			*/
    GLYPH_caron,		/*  159			*/
    GLYPH_nobreakspace,		/*  160			*/
    GLYPH_exclamdown,		/*  161			*/
    GLYPH_cent,			/*  162			*/
    GLYPH_sterling,		/*  163			*/
    GLYPH_currency,		/*  164			*/
    GLYPH_yen,			/*  165			*/
    GLYPH_brokenbar,		/*  166			*/
    GLYPH_section,		/*  167			*/
    GLYPH_dieresis,		/*  168			*/
    GLYPH_copyright,		/*  169			*/
    GLYPH_ordfeminine,		/*  170			*/
    GLYPH_guillemotleft,	/*  171			*/
    GLYPH_logicalnot,		/*  172			*/
    GLYPH_hyphen,		/*  173			*/
    GLYPH_registered,		/*  174			*/
    GLYPH_macron,		/*  175			*/
    GLYPH_degree,		/*  176			*/
    GLYPH_plusminus,		/*  177			*/
    GLYPH_twosuperior,		/*  178			*/
    GLYPH_threesuperior,	/*  179			*/
    GLYPH_acute,		/*  180			*/
    GLYPH_mu,			/*  181			*/
    GLYPH_paragraph,		/*  182			*/
    GLYPH_periodcentered,	/*  183			*/
    GLYPH_cedilla,		/*  184			*/
    GLYPH_onesuperior,		/*  185			*/
    GLYPH_ordmasculine,		/*  186			*/
    GLYPH_guillemotright,	/*  187			*/
    GLYPH_onequarter,		/*  188			*/
    GLYPH_onehalf,		/*  189			*/
    GLYPH_threequarters,	/*  190			*/
    GLYPH_questiondown,		/*  191			*/
    GLYPH_Agrave,		/*  192			*/
    GLYPH_Aacute,		/*  193			*/
    GLYPH_Acircumflex,		/*  194			*/
    GLYPH_Atilde,		/*  195			*/
    GLYPH_Adieresis,		/*  196			*/
    GLYPH_Aring,		/*  197			*/
    GLYPH_AE,			/*  198			*/
    GLYPH_Ccedilla,		/*  199			*/
    GLYPH_Egrave,		/*  200			*/
    GLYPH_Eacute,		/*  201			*/
    GLYPH_Ecircumflex,		/*  202			*/
    GLYPH_Edieresis,		/*  203			*/
    GLYPH_Igrave,		/*  204			*/
    GLYPH_Iacute,		/*  205			*/
    GLYPH_Icircumflex,		/*  206			*/
    GLYPH_Idieresis,		/*  207			*/
    GLYPH_Eth,			/*  208			*/
    GLYPH_Ntilde,		/*  209			*/
    GLYPH_Ograve,		/*  210			*/
    GLYPH_Oacute,		/*  211			*/
    GLYPH_Ocircumflex,		/*  212			*/
    GLYPH_Otilde,		/*  213			*/
    GLYPH_Odieresis,		/*  214			*/
    GLYPH_multiply,		/*  215			*/
    GLYPH_Oslash,		/*  216			*/
    GLYPH_Ugrave,		/*  217			*/
    GLYPH_Uacute,		/*  218			*/
    GLYPH_Ucircumflex,		/*  219			*/
    GLYPH_Udieresis,		/*  220			*/
    GLYPH_Yacute,		/*  221			*/
    GLYPH_Thorn,		/*  222			*/
    GLYPH_germandbls,		/*  223			*/
    GLYPH_agrave,		/*  224			*/
    GLYPH_aacute,		/*  225			*/
    GLYPH_acircumflex,		/*  226			*/
    GLYPH_atilde,		/*  227			*/
    GLYPH_adieresis,		/*  228			*/
    GLYPH_aring,		/*  229			*/
    GLYPH_ae,			/*  230			*/
    GLYPH_ccedilla,		/*  231			*/
    GLYPH_egrave,		/*  232			*/
    GLYPH_eacute,		/*  233			*/
    GLYPH_ecircumflex,		/*  234			*/
    GLYPH_edieresis,		/*  235			*/
    GLYPH_igrave,		/*  236			*/
    GLYPH_iacute,		/*  237			*/
    GLYPH_icircumflex,		/*  238			*/
    GLYPH_idieresis,		/*  239			*/
    GLYPH_eth,			/*  240			*/
    GLYPH_ntilde,		/*  241			*/
    GLYPH_ograve,		/*  242			*/
    GLYPH_oacute,		/*  243			*/
    GLYPH_ocircumflex,		/*  244			*/
    GLYPH_otilde,		/*  245			*/
    GLYPH_odieresis,		/*  246			*/
    GLYPH_divide,		/*  247			*/
    GLYPH_oslash,		/*  248			*/
    GLYPH_ugrave,		/*  249			*/
    GLYPH_uacute,		/*  250			*/
    GLYPH_ucircumflex,		/*  251			*/
    GLYPH_udieresis,		/*  252			*/
    GLYPH_yacute,		/*  253			*/
    GLYPH_thorn,		/*  254			*/
    GLYPH_ydieresis,		/*  255			*/
    };

const char *	const psIsoLatin15GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_minus,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_quoteleft,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_exclamdown,		/*  161		*/
    GLYPH_cent,			/*  162		*/
    GLYPH_sterling,		/*  163		*/
    GLYPH_Euro,			/*  164		*/
    GLYPH_yen,			/*  165		*/
    GLYPH_Scaron,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_scaron,		/*  168		*/
    GLYPH_copyright,		/*  169		*/
    GLYPH_ordfeminine,		/*  170		*/
    GLYPH_guillemotleft,	/*  171		*/
    GLYPH_logicalnot,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_registered,		/*  174		*/
    GLYPH_macron,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_plusminus,		/*  177		*/
    GLYPH_twosuperior,		/*  178		*/
    GLYPH_threesuperior,	/*  179		*/
    GLYPH_Zcaron,		/*  180		*/
    GLYPH_mu,			/*  181		*/
    GLYPH_paragraph,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_zcaron,		/*  184		*/
    GLYPH_onesuperior,		/*  185		*/
    GLYPH_ordmasculine,		/*  186		*/
    GLYPH_guillemotright,	/*  187		*/
    GLYPH_OE,			/*  188		*/
    GLYPH_oe,			/*  189		*/
    GLYPH_Ydieresis,		/*  190		*/
    GLYPH_questiondown,		/*  191		*/
    GLYPH_Agrave,		/*  192		*/
    GLYPH_Aacute,		/*  193		*/
    GLYPH_Acircumflex,		/*  194		*/
    GLYPH_Atilde,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Aring,		/*  197		*/
    GLYPH_AE,			/*  198		*/
    GLYPH_Ccedilla,		/*  199		*/
    GLYPH_Egrave,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Ecircumflex,		/*  202		*/
    GLYPH_Edieresis,		/*  203		*/
    GLYPH_Igrave,		/*  204		*/
    GLYPH_Iacute,		/*  205		*/
    GLYPH_Icircumflex,		/*  206		*/
    GLYPH_Idieresis,		/*  207		*/
    GLYPH_Eth,			/*  208		*/
    GLYPH_Ntilde,		/*  209		*/
    GLYPH_Ograve,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Ocircumflex,		/*  212		*/
    GLYPH_Otilde,		/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_multiply,		/*  215		*/
    GLYPH_Oslash,		/*  216		*/
    GLYPH_Ugrave,		/*  217		*/
    GLYPH_Uacute,		/*  218		*/
    GLYPH_Ucircumflex,		/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Yacute,		/*  221		*/
    GLYPH_Thorn,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_agrave,		/*  224		*/
    GLYPH_aacute,		/*  225		*/
    GLYPH_acircumflex,		/*  226		*/
    GLYPH_atilde,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_aring,		/*  229		*/
    GLYPH_ae,			/*  230		*/
    GLYPH_ccedilla,		/*  231		*/
    GLYPH_egrave,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_ecircumflex,		/*  234		*/
    GLYPH_edieresis,		/*  235		*/
    GLYPH_igrave,		/*  236		*/
    GLYPH_iacute,		/*  237		*/
    GLYPH_icircumflex,		/*  238		*/
    GLYPH_idieresis,		/*  239		*/
    GLYPH_eth,			/*  240		*/
    GLYPH_ntilde,		/*  241		*/
    GLYPH_ograve,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_ocircumflex,		/*  244		*/
    GLYPH_otilde,		/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_divide,		/*  247		*/
    GLYPH_oslash,		/*  248		*/
    GLYPH_ugrave,		/*  249		*/
    GLYPH_uacute,		/*  250		*/
    GLYPH_ucircumflex,		/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_yacute,		/*  253		*/
    GLYPH_thorn,		/*  254		*/
    GLYPH_ydieresis,		/*  255		*/
    } ;

/************************************************************************/
/*									*/
/*  Postscript symbol encoding to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psSymbolGlyphNames[256]=
    {
    (const char *)0,		/*  0	*/
    (const char *)0,		/*  1	*/
    (const char *)0,		/*  2	*/
    (const char *)0,		/*  3	*/
    (const char *)0,		/*  4	*/
    (const char *)0,		/*  5	*/
    (const char *)0,		/*  6	*/
    (const char *)0,		/*  7	*/
    (const char *)0,		/*  8	*/
    (const char *)0,		/*  9	*/
    (const char *)0,		/*  10	*/
    (const char *)0,		/*  11	*/
    (const char *)0,		/*  12	*/
    (const char *)0,		/*  13	*/
    (const char *)0,		/*  14	*/
    (const char *)0,		/*  15	*/
    (const char *)0,		/*  16	*/
    (const char *)0,		/*  17	*/
    (const char *)0,		/*  18	*/
    (const char *)0,		/*  19	*/
    (const char *)0,		/*  20	*/
    (const char *)0,		/*  21	*/
    (const char *)0,		/*  22	*/
    (const char *)0,		/*  23	*/
    (const char *)0,		/*  24	*/
    (const char *)0,		/*  25	*/
    (const char *)0,		/*  26	*/
    (const char *)0,		/*  27	*/
    (const char *)0,		/*  28	*/
    (const char *)0,		/*  29	*/
    (const char *)0,		/*  30	*/
    (const char *)0,		/*  31	*/
    GLYPH_space,		/*  32	*/
    GLYPH_exclam,		/*  33	*/
    GLYPH_universal,		/*  34	*/
    GLYPH_numbersign,		/*  35	*/
    GLYPH_existential,		/*  36	*/
    GLYPH_percent,		/*  37	*/
    GLYPH_ampersand,		/*  38	*/
    GLYPH_suchthat,		/*  39	*/
    GLYPH_parenleft,		/*  40	*/
    GLYPH_parenright,		/*  41	*/
    GLYPH_asteriskmath,		/*  42	*/
    GLYPH_plus,			/*  43	*/
    GLYPH_comma,		/*  44	*/
    GLYPH_minus,		/*  45	*/
    GLYPH_period,		/*  46	*/
    GLYPH_slash,		/*  47	*/
    GLYPH_zero,			/*  48	*/
    GLYPH_one,			/*  49	*/
    GLYPH_two,			/*  50	*/
    GLYPH_three,		/*  51	*/
    GLYPH_four,			/*  52	*/
    GLYPH_five,			/*  53	*/
    GLYPH_six,			/*  54	*/
    GLYPH_seven,		/*  55	*/
    GLYPH_eight,		/*  56	*/
    GLYPH_nine,			/*  57	*/
    GLYPH_colon,		/*  58	*/
    GLYPH_semicolon,		/*  59	*/
    GLYPH_less,			/*  60	*/
    GLYPH_equal,		/*  61	*/
    GLYPH_greater,		/*  62	*/
    GLYPH_question,		/*  63	*/
    GLYPH_congruent,		/*  64	*/
    GLYPH_Alpha,		/*  65	*/
    GLYPH_Beta,			/*  66	*/
    GLYPH_Chi,			/*  67	*/
    GLYPH_Delta,		/*  68	*/
    GLYPH_Epsilon,		/*  69	*/
    GLYPH_Phi,			/*  70	*/
    GLYPH_Gamma,		/*  71	*/
    GLYPH_Eta,			/*  72	*/
    GLYPH_Iota,			/*  73	*/
    GLYPH_theta1,		/*  74	*/
    GLYPH_Kappa,		/*  75	*/
    GLYPH_Lambda,		/*  76	*/
    GLYPH_Mu,			/*  77	*/
    GLYPH_Nu,			/*  78	*/
    GLYPH_Omicron,		/*  79	*/
    GLYPH_Pi,			/*  80	*/
    GLYPH_Theta,		/*  81	*/
    GLYPH_Rho,			/*  82	*/
    GLYPH_Sigma,		/*  83	*/
    GLYPH_Tau,			/*  84	*/
    GLYPH_Upsilon,		/*  85	*/
    GLYPH_sigma1,		/*  86	*/
    GLYPH_Omega,		/*  87	*/
    GLYPH_Xi,			/*  88	*/
    GLYPH_Psi,			/*  89	*/
    GLYPH_Zeta,			/*  90	*/
    GLYPH_bracketleft,		/*  91	*/
    GLYPH_therefore,		/*  92	*/
    GLYPH_bracketright,		/*  93	*/
    GLYPH_perpendicular,	/*  94	*/
    GLYPH_underscore,		/*  95	*/
    GLYPH_radicalex,		/*  96	*/
    GLYPH_alpha,		/*  97	*/
    GLYPH_beta,			/*  98	*/
    GLYPH_chi,			/*  99	*/
    GLYPH_delta,		/*  100	*/
    GLYPH_epsilon,		/*  101	*/
    GLYPH_phi,			/*  102	*/
    GLYPH_gamma,		/*  103	*/
    GLYPH_eta,			/*  104	*/
    GLYPH_iota,			/*  105	*/
    GLYPH_phi1,			/*  106	*/
    GLYPH_kappa,		/*  107	*/
    GLYPH_lambda,		/*  108	*/
    GLYPH_mu,			/*  109	*/
    GLYPH_nu,			/*  110	*/
    GLYPH_omicron,		/*  111	*/
    GLYPH_pi,			/*  112	*/
    GLYPH_theta,		/*  113	*/
    GLYPH_rho,			/*  114	*/
    GLYPH_sigma,		/*  115	*/
    GLYPH_tau,			/*  116	*/
    GLYPH_upsilon,		/*  117	*/
    GLYPH_omega1,		/*  118	*/
    GLYPH_omega,		/*  119	*/
    GLYPH_xi,			/*  120	*/
    GLYPH_psi,			/*  121	*/
    GLYPH_zeta,			/*  122	*/
    GLYPH_braceleft,		/*  123	*/
    GLYPH_bar,			/*  124	*/
    GLYPH_braceright,		/*  125	*/
    GLYPH_similar,		/*  126	*/
    (const char *)0,		/*  127	*/
    (const char *)0,		/*  128	*/
    (const char *)0,		/*  129	*/
    (const char *)0,		/*  130	*/
    (const char *)0,		/*  131	*/
    (const char *)0,		/*  132	*/
    (const char *)0,		/*  133	*/
    (const char *)0,		/*  134	*/
    (const char *)0,		/*  135	*/
    (const char *)0,		/*  136	*/
    (const char *)0,		/*  137	*/
    (const char *)0,		/*  138	*/
    (const char *)0,		/*  139	*/
    (const char *)0,		/*  140	*/
    (const char *)0,		/*  141	*/
    (const char *)0,		/*  142	*/
    (const char *)0,		/*  143	*/
    (const char *)0,		/*  144	*/
    (const char *)0,		/*  145	*/
    (const char *)0,		/*  146	*/
    (const char *)0,		/*  147	*/
    (const char *)0,		/*  148	*/
    (const char *)0,		/*  149	*/
    (const char *)0,		/*  150	*/
    (const char *)0,		/*  151	*/
    (const char *)0,		/*  152	*/
    (const char *)0,		/*  153	*/
    (const char *)0,		/*  154	*/
    (const char *)0,		/*  155	*/
    (const char *)0,		/*  156	*/
    (const char *)0,		/*  157	*/
    (const char *)0,		/*  158	*/
    (const char *)0,		/*  159	*/
    (const char *)0,		/*  160	*/
    GLYPH_Upsilon1,		/*  161	*/
    GLYPH_minute,		/*  162	*/
    GLYPH_lessequal,		/*  163	*/
    GLYPH_fraction,		/*  164	*/
    GLYPH_infinity,		/*  165	*/
    GLYPH_florin,		/*  166	*/
    GLYPH_club,			/*  167	*/
    GLYPH_diamond,		/*  168	*/
    GLYPH_heart,		/*  169	*/
    GLYPH_spade,		/*  170	*/
    GLYPH_arrowboth,		/*  171	*/
    GLYPH_arrowleft,		/*  172	*/
    GLYPH_arrowup,		/*  173	*/
    GLYPH_arrowright,		/*  174	*/
    GLYPH_arrowdown,		/*  175	*/
    GLYPH_degree,		/*  176	*/
    GLYPH_plusminus,		/*  177	*/
    GLYPH_second,		/*  178	*/
    GLYPH_greaterequal,		/*  179	*/
    GLYPH_multiply,		/*  180	*/
    GLYPH_proportional,		/*  181	*/
    GLYPH_partialdiff,		/*  182	*/
    GLYPH_bullet,		/*  183	*/
    GLYPH_divide,		/*  184	*/
    GLYPH_notequal,		/*  185	*/
    GLYPH_equivalence,		/*  186	*/
    GLYPH_approxequal,		/*  187	*/
    GLYPH_ellipsis,		/*  188	*/
    GLYPH_arrowvertex,		/*  189	*/
    GLYPH_arrowhorizex,		/*  190	*/
    GLYPH_carriagereturn,	/*  191	*/
    GLYPH_aleph,		/*  192	*/
    GLYPH_Ifraktur,		/*  193	*/
    GLYPH_Rfraktur,		/*  194	*/
    GLYPH_weierstrass,		/*  195	*/
    GLYPH_circlemultiply,	/*  196	*/
    GLYPH_circleplus,		/*  197	*/
    GLYPH_emptyset,		/*  198	*/
    GLYPH_intersection,		/*  199	*/
    GLYPH_union,		/*  200	*/
    GLYPH_propersuperset,	/*  201	*/
    GLYPH_reflexsuperset,	/*  202	*/
    GLYPH_notsubset,		/*  203	*/
    GLYPH_propersubset,		/*  204	*/
    GLYPH_reflexsubset,		/*  205	*/
    GLYPH_element,		/*  206	*/
    GLYPH_notelement,		/*  207	*/
    GLYPH_angle,		/*  208	*/
    GLYPH_gradient,		/*  209	*/
    GLYPH_registerserif,	/*  210	*/
    GLYPH_copyrightserif,	/*  211	*/
    GLYPH_trademarkserif,	/*  212	*/
    GLYPH_product,		/*  213	*/
    GLYPH_radical,		/*  214	*/
    GLYPH_dotmath,		/*  215	*/
    GLYPH_logicalnot,		/*  216	*/
    GLYPH_logicaland,		/*  217	*/
    GLYPH_logicalor,		/*  218	*/
    GLYPH_arrowdblboth,		/*  219	*/
    GLYPH_arrowdblleft,		/*  220	*/
    GLYPH_arrowdblup,		/*  221	*/
    GLYPH_arrowdblright,	/*  222	*/
    GLYPH_arrowdbldown,		/*  223	*/
    GLYPH_lozenge,		/*  224	*/
    GLYPH_angleleft,		/*  225	*/
    GLYPH_registersans,		/*  226	*/
    GLYPH_copyrightsans,	/*  227	*/
    GLYPH_trademarksans,	/*  228	*/
    GLYPH_summation,		/*  229	*/
    GLYPH_parenlefttp,		/*  230	*/
    GLYPH_parenleftex,		/*  231	*/
    GLYPH_parenleftbt,		/*  232	*/
    GLYPH_bracketlefttp,	/*  233	*/
    GLYPH_bracketleftex,	/*  234	*/
    GLYPH_bracketleftbt,	/*  235	*/
    GLYPH_bracelefttp,		/*  236	*/
    GLYPH_braceleftmid,		/*  237	*/
    GLYPH_braceleftbt,		/*  238	*/
    GLYPH_braceex,		/*  239	*/
    (const char *)0,		/*  240	*/
    GLYPH_angleright,		/*  241	*/
    GLYPH_integral,		/*  242	*/
    GLYPH_integraltp,		/*  243	*/
    GLYPH_integralex,		/*  244	*/
    GLYPH_integralbt,		/*  245	*/
    GLYPH_parenrighttp,		/*  246	*/
    GLYPH_parenrightex,		/*  247	*/
    GLYPH_parenrightbt,		/*  248	*/
    GLYPH_bracketrighttp,	/*  249	*/
    GLYPH_bracketrightex,	/*  250	*/
    GLYPH_bracketrightbt,	/*  251	*/
    GLYPH_bracerighttp,		/*  252	*/
    GLYPH_bracerightmid,	/*  253	*/
    GLYPH_bracerightbt,		/*  254	*/
    (const char *)0,		/*  255	*/
    };

/************************************************************************/
/*									*/
/*  ISO Latin2 (ISO 8859-2) to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin2GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quoteright,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_Aogonek,		/*  161		*/
    GLYPH_breve,		/*  162		*/
    GLYPH_Lslash,		/*  163		*/
    GLYPH_currency,		/*  164		*/
    GLYPH_Lcaron,		/*  165		*/
    GLYPH_Sacute,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_dieresis,		/*  168		*/
    GLYPH_Scaron,		/*  169		*/
    GLYPH_Scedilla,		/*  170		*/
    GLYPH_Tcaron,		/*  171		*/
    GLYPH_Zacute,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_Zcaron,		/*  174		*/
    GLYPH_Zdotaccent,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_aogonek,		/*  177		*/
    GLYPH_ogonek,		/*  178		*/
    GLYPH_lslash,		/*  179		*/
    GLYPH_acute,		/*  180		*/
    GLYPH_lcaron,		/*  181		*/
    GLYPH_sacute,		/*  182		*/
    GLYPH_caron,		/*  183		*/
    GLYPH_cedilla,		/*  184		*/
    GLYPH_scaron,		/*  185		*/
    GLYPH_scedilla,		/*  186		*/
    GLYPH_tcaron,		/*  187		*/
    GLYPH_zacute,		/*  188		*/
    GLYPH_hungarumlaut,		/*  189		*/
    GLYPH_zcaron,		/*  190		*/
    GLYPH_zdotaccent,		/*  191		*/
    GLYPH_Racute,		/*  192		*/
    GLYPH_Aacute,		/*  193		*/
    GLYPH_Acircumflex,		/*  194		*/
    GLYPH_Abreve,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Lacute,		/*  197		*/
    GLYPH_Cacute,		/*  198		*/
    GLYPH_Ccedilla,		/*  199		*/
    GLYPH_Ccaron,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Eogonek,		/*  202		*/
    GLYPH_Edieresis,		/*  203		*/
    GLYPH_Ecaron,		/*  204		*/
    GLYPH_Iacute,		/*  205		*/
    GLYPH_Icircumflex,		/*  206		*/
    GLYPH_Dcaron,		/*  207		*/
    GLYPH_Dcroat,		/*  208		*/
    GLYPH_Nacute,		/*  209		*/
    GLYPH_Ncaron,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Ocircumflex,		/*  212		*/
    GLYPH_Ohungarumlaut,	/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_multiply,		/*  215		*/
    GLYPH_Rcaron,		/*  216		*/
    GLYPH_Uring,		/*  217		*/
    GLYPH_Uacute,		/*  218		*/
    GLYPH_Uhungarumlaut,	/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Yacute,		/*  221		*/
    GLYPH_Tcommaaccent,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_racute,		/*  224		*/
    GLYPH_aacute,		/*  225		*/
    GLYPH_acircumflex,		/*  226		*/
    GLYPH_abreve,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_lacute,		/*  229		*/
    GLYPH_cacute,		/*  230		*/
    GLYPH_ccedilla,		/*  231		*/
    GLYPH_ccaron,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_eogonek,		/*  234		*/
    GLYPH_edieresis,		/*  235		*/
    GLYPH_ecaron,		/*  236		*/
    GLYPH_iacute,		/*  237		*/
    GLYPH_icircumflex,		/*  238		*/
    GLYPH_dcaron,		/*  239		*/
    GLYPH_dcroat,		/*  240		*/
    GLYPH_nacute,		/*  241		*/
    GLYPH_ncaron,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_ocircumflex,		/*  244		*/
    GLYPH_ohungarumlaut,	/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_divide,		/*  247		*/
    GLYPH_rcaron,		/*  248		*/
    GLYPH_uring,		/*  249		*/
    GLYPH_uacute,		/*  250		*/
    GLYPH_uhungarumlaut,	/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_yacute,		/*  253		*/
    GLYPH_tcommaaccent,		/*  254		*/
    GLYPH_dotaccent,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  Cyrillic (ISO 8859-5: Cyrillic) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char *	const psIsoLatin5GlyphNames[256]=
    {
    (const char *)0,		/*    0			*/
    (const char *)0,		/*    1			*/
    (const char *)0,		/*    2			*/
    (const char *)0,		/*    3			*/
    (const char *)0,		/*    4			*/
    (const char *)0,		/*    5			*/
    (const char *)0,		/*    6			*/
    (const char *)0,		/*    7			*/
    (const char *)0,		/*    8			*/
    (const char *)0,		/*    9			*/
    (const char *)0,		/*   10			*/
    (const char *)0,		/*   11			*/
    (const char *)0,		/*   12			*/
    (const char *)0,		/*   13			*/
    (const char *)0,		/*   14			*/
    (const char *)0,		/*   15			*/
    (const char *)0,		/*   16			*/
    (const char *)0,		/*   17			*/
    (const char *)0,		/*   18			*/
    (const char *)0,		/*   19			*/
    (const char *)0,		/*   20			*/
    (const char *)0,		/*   21			*/
    (const char *)0,		/*   22			*/
    (const char *)0,		/*   23			*/
    (const char *)0,		/*   24			*/
    (const char *)0,		/*   25			*/
    (const char *)0,		/*   26			*/
    (const char *)0,		/*   27			*/
    (const char *)0,		/*   28			*/
    (const char *)0,		/*   29			*/
    (const char *)0,		/*   30			*/
    (const char *)0,		/*   31			*/
    GLYPH_space,		/*   32= 0x20		*/
    GLYPH_exclam,		/*   33= 0x21		*/
    GLYPH_quotedbl,		/*   34= 0x22		*/
    GLYPH_numbersign,		/*   35= 0x23		*/
    GLYPH_dollar,		/*   36= 0x24		*/
    GLYPH_percent,		/*   37= 0x25		*/
    GLYPH_ampersand,		/*   38= 0x26		*/
    GLYPH_quotesingle,		/*   39= 0x27		*/
    GLYPH_parenleft,		/*   40= 0x28		*/
    GLYPH_parenright,		/*   41= 0x29		*/
    GLYPH_asterisk,		/*   42= 0x2a		*/
    GLYPH_plus,			/*   43= 0x2b		*/
    GLYPH_comma,		/*   44= 0x2c		*/
    GLYPH_minus,		/*   45= 0x2d		*/
    GLYPH_period,		/*   46= 0x2e		*/
    GLYPH_slash,		/*   47= 0x2f		*/
    GLYPH_zero,			/*   48= 0x30		*/
    GLYPH_one,			/*   49= 0x31		*/
    GLYPH_two,			/*   50= 0x32		*/
    GLYPH_three,		/*   51= 0x33		*/
    GLYPH_four,			/*   52= 0x34		*/
    GLYPH_five,			/*   53= 0x35		*/
    GLYPH_six,			/*   54= 0x36		*/
    GLYPH_seven,		/*   55= 0x37		*/
    GLYPH_eight,		/*   56= 0x38		*/
    GLYPH_nine,			/*   57= 0x39		*/
    GLYPH_colon,		/*   58= 0x3a		*/
    GLYPH_semicolon,		/*   59= 0x3b		*/
    GLYPH_less,			/*   60= 0x3c		*/
    GLYPH_equal,		/*   61= 0x3d		*/
    GLYPH_greater,		/*   62= 0x3e		*/
    GLYPH_question,		/*   63= 0x3f		*/
    GLYPH_at,			/*   64= 0x40		*/
    GLYPH_A,			/*   65= 0x41		*/
    GLYPH_B,			/*   66= 0x42		*/
    GLYPH_C,			/*   67= 0x43		*/
    GLYPH_D,			/*   68= 0x44		*/
    GLYPH_E,			/*   69= 0x45		*/
    GLYPH_F,			/*   70= 0x46		*/
    GLYPH_G,			/*   71= 0x47		*/
    GLYPH_H,			/*   72= 0x48		*/
    GLYPH_I,			/*   73= 0x49		*/
    GLYPH_J,			/*   74= 0x4a		*/
    GLYPH_K,			/*   75= 0x4b		*/
    GLYPH_L,			/*   76= 0x4c		*/
    GLYPH_M,			/*   77= 0x4d		*/
    GLYPH_N,			/*   78= 0x4e		*/
    GLYPH_O,			/*   79= 0x4f		*/
    GLYPH_P,			/*   80= 0x50		*/
    GLYPH_Q,			/*   81= 0x51		*/
    GLYPH_R,			/*   82= 0x52		*/
    GLYPH_S,			/*   83= 0x53		*/
    GLYPH_T,			/*   84= 0x54		*/
    GLYPH_U,			/*   85= 0x55		*/
    GLYPH_V,			/*   86= 0x56		*/
    GLYPH_W,			/*   87= 0x57		*/
    GLYPH_X,			/*   88= 0x58		*/
    GLYPH_Y,			/*   89= 0x59		*/
    GLYPH_Z,			/*   90= 0x5a		*/
    GLYPH_bracketleft,		/*   91= 0x5b		*/
    GLYPH_backslash,		/*   92= 0x5c		*/
    GLYPH_bracketright,		/*   93= 0x5d		*/
    GLYPH_asciicircum,		/*   94= 0x5e		*/
    GLYPH_underscore,		/*   95= 0x5f		*/
    GLYPH_grave,		/*   96= 0x60		*/
    GLYPH_a,			/*   97= 0x61		*/
    GLYPH_b,			/*   98= 0x62		*/
    GLYPH_c,			/*   99= 0x63		*/
    GLYPH_d,			/*  100= 0x64		*/
    GLYPH_e,			/*  101= 0x65		*/
    GLYPH_f,			/*  102= 0x66		*/
    GLYPH_g,			/*  103= 0x67		*/
    GLYPH_h,			/*  104= 0x68		*/
    GLYPH_i,			/*  105= 0x69		*/
    GLYPH_j,			/*  106= 0x6a		*/
    GLYPH_k,			/*  107= 0x6b		*/
    GLYPH_l,			/*  108= 0x6c		*/
    GLYPH_m,			/*  109= 0x6d		*/
    GLYPH_n,			/*  110= 0x6e		*/
    GLYPH_o,			/*  111= 0x6f		*/
    GLYPH_p,			/*  112= 0x70		*/
    GLYPH_q,			/*  113= 0x71		*/
    GLYPH_r,			/*  114= 0x72		*/
    GLYPH_s,			/*  115= 0x73		*/
    GLYPH_t,			/*  116= 0x74		*/
    GLYPH_u,			/*  117= 0x75		*/
    GLYPH_v,			/*  118= 0x76		*/
    GLYPH_w,			/*  119= 0x77		*/
    GLYPH_x,			/*  120= 0x78		*/
    GLYPH_y,			/*  121= 0x79		*/
    GLYPH_z,			/*  122= 0x7a		*/
    GLYPH_braceleft,		/*  123= 0x7b		*/
    GLYPH_bar,			/*  124= 0x7c		*/
    GLYPH_braceright,		/*  125= 0x7d		*/
    GLYPH_asciitilde,		/*  126= 0x7e		*/
    (const char *)0,		/*  127			*/
    (const char *)0,		/*  128			*/
    (const char *)0,		/*  129			*/
    (const char *)0,		/*  130			*/
    (const char *)0,		/*  131			*/
    (const char *)0,		/*  132			*/
    (const char *)0,		/*  133			*/
    (const char *)0,		/*  134			*/
    (const char *)0,		/*  135			*/
    (const char *)0,		/*  136			*/
    (const char *)0,		/*  137			*/
    (const char *)0,		/*  138			*/
    (const char *)0,		/*  139			*/
    (const char *)0,		/*  140			*/
    (const char *)0,		/*  141			*/
    (const char *)0,		/*  142			*/
    (const char *)0,		/*  143			*/
    (const char *)0,		/*  144			*/
    (const char *)0,		/*  145			*/
    (const char *)0,		/*  146			*/
    (const char *)0,		/*  147			*/
    (const char *)0,		/*  148			*/
    (const char *)0,		/*  149			*/
    (const char *)0,		/*  150			*/
    (const char *)0,		/*  151			*/
    (const char *)0,		/*  152			*/
    (const char *)0,		/*  153			*/
    (const char *)0,		/*  154			*/
    (const char *)0,		/*  155			*/
    (const char *)0,		/*  156			*/
    (const char *)0,		/*  157			*/
    (const char *)0,		/*  158			*/
    (const char *)0,		/*  159			*/
    GLYPH_nobreakspace,		/*  160= 0xa0		*/
    GLYPH_afii10023,		/*  161= 0xa1		*/
    GLYPH_afii10051,		/*  162= 0xa2		*/
    GLYPH_afii10052,		/*  163= 0xa3		*/
    GLYPH_afii10053,		/*  164= 0xa4		*/
    GLYPH_afii10054,		/*  165= 0xa5		*/
    GLYPH_afii10055,		/*  166= 0xa6		*/
    GLYPH_afii10056,		/*  167= 0xa7		*/
    GLYPH_afii10057,		/*  168= 0xa8		*/
    GLYPH_afii10058,		/*  169= 0xa9		*/
    GLYPH_afii10059,		/*  170= 0xaa		*/
    GLYPH_afii10060,		/*  171= 0xab		*/
    GLYPH_afii10061,		/*  172= 0xac		*/
    GLYPH_hyphen,		/*  173= 0xad		*/
    GLYPH_afii10062,		/*  174= 0xae		*/
    GLYPH_afii10145,		/*  175= 0xaf		*/
    GLYPH_afii10017,		/*  176= 0xb0		*/
    GLYPH_afii10018,		/*  177= 0xb1		*/
    GLYPH_afii10019,		/*  178= 0xb2		*/
    GLYPH_afii10020,		/*  179= 0xb3		*/
    GLYPH_afii10021,		/*  180= 0xb4		*/
    GLYPH_afii10022,		/*  181= 0xb5		*/
    GLYPH_afii10024,		/*  182= 0xb6		*/
    GLYPH_afii10025,		/*  183= 0xb7		*/
    GLYPH_afii10026,		/*  184= 0xb8		*/
    GLYPH_afii10027,		/*  185= 0xb9		*/
    GLYPH_afii10028,		/*  186= 0xba		*/
    GLYPH_afii10029,		/*  187= 0xbb		*/
    GLYPH_afii10030,		/*  188= 0xbc		*/
    GLYPH_afii10031,		/*  189= 0xbd		*/
    GLYPH_afii10032,		/*  190= 0xbe		*/
    GLYPH_afii10033,		/*  191= 0xbf		*/
    GLYPH_afii10034,		/*  192= 0xc0		*/
    GLYPH_afii10035,		/*  193= 0xc1		*/
    GLYPH_afii10036,		/*  194= 0xc2		*/
    GLYPH_afii10037,		/*  195= 0xc3		*/
    GLYPH_afii10038,		/*  196= 0xc4		*/
    GLYPH_afii10039,		/*  197= 0xc5		*/
    GLYPH_afii10040,		/*  198= 0xc6		*/
    GLYPH_afii10041,		/*  199= 0xc7		*/
    GLYPH_afii10042,		/*  200= 0xc8		*/
    GLYPH_afii10043,		/*  201= 0xc9		*/
    GLYPH_afii10044,		/*  202= 0xca		*/
    GLYPH_afii10045,		/*  203= 0xcb		*/
    GLYPH_afii10046,		/*  204= 0xcc		*/
    GLYPH_afii10047,		/*  205= 0xcd		*/
    GLYPH_afii10048,		/*  206= 0xce		*/
    GLYPH_afii10049,		/*  207= 0xcf		*/
    GLYPH_afii10065,		/*  208= 0xd0		*/
    GLYPH_afii10066,		/*  209= 0xd1		*/
    GLYPH_afii10067,		/*  210= 0xd2		*/
    GLYPH_afii10068,		/*  211= 0xd3		*/
    GLYPH_afii10069,		/*  212= 0xd4		*/
    GLYPH_afii10070,		/*  213= 0xd5		*/
    GLYPH_afii10072,		/*  214= 0xd6		*/
    GLYPH_afii10073,		/*  215= 0xd7		*/
    GLYPH_afii10074,		/*  216= 0xd8		*/
    GLYPH_afii10075,		/*  217= 0xd9		*/
    GLYPH_afii10076,		/*  218= 0xda		*/
    GLYPH_afii10077,		/*  219= 0xdb		*/
    GLYPH_afii10078,		/*  220= 0xdc		*/
    GLYPH_afii10079,		/*  221= 0xdd		*/
    GLYPH_afii10080,		/*  222= 0xde		*/
    GLYPH_afii10081,		/*  223= 0xdf		*/
    GLYPH_afii10082,		/*  224= 0xe0		*/
    GLYPH_afii10083,		/*  225= 0xe1		*/
    GLYPH_afii10084,		/*  226= 0xe2		*/
    GLYPH_afii10085,		/*  227= 0xe3		*/
    GLYPH_afii10086,		/*  228= 0xe4		*/
    GLYPH_afii10087,		/*  229= 0xe5		*/
    GLYPH_afii10088,		/*  230= 0xe6		*/
    GLYPH_afii10089,		/*  231= 0xe7		*/
    GLYPH_afii10090,		/*  232= 0xe8		*/
    GLYPH_afii10091,		/*  233= 0xe9		*/
    GLYPH_afii10092,		/*  234= 0xea		*/
    GLYPH_afii10093,		/*  235= 0xeb		*/
    GLYPH_afii10094,		/*  236= 0xec		*/
    GLYPH_afii10095,		/*  237= 0xed		*/
    GLYPH_afii10096,		/*  238= 0xee		*/
    GLYPH_afii10097,		/*  239= 0xef		*/
    GLYPH_afii61352,		/*  240= 0xf0		*/
    GLYPH_afii10071,		/*  241= 0xf1		*/
    GLYPH_afii10099,		/*  242= 0xf2		*/
    GLYPH_afii10100,		/*  243= 0xf3		*/
    GLYPH_afii10101,		/*  244= 0xf4		*/
    GLYPH_afii10102,		/*  245= 0xf5		*/
    GLYPH_afii10103,		/*  246= 0xf6		*/
    GLYPH_afii10104,		/*  247= 0xf7		*/
    GLYPH_afii10105,		/*  248= 0xf8		*/
    GLYPH_afii10106,		/*  249= 0xf9		*/
    GLYPH_afii10107,		/*  250= 0xfa		*/
    GLYPH_afii10108,		/*  251= 0xfb		*/
    GLYPH_afii10109,		/*  252= 0xfc		*/
    GLYPH_section,		/*  253= 0xfd		*/
    GLYPH_afii10110,		/*  254= 0xfe		*/
    GLYPH_afii10193,		/*  255= 0xff		*/
    };

/************************************************************************/
/*									*/
/*  Greek (ISO 8859-7: Greek) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin7GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_afii64937,		/*  161		*/
    GLYPH_afii57929,		/*  162		*/
    GLYPH_sterling,		/*  163		*/
    (const char *)0,		/*  164		*/
    (const char *)0,		/*  165		*/
    GLYPH_brokenbar,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_dieresis,		/*  168		*/
    GLYPH_copyright,		/*  169		*/
    (const char *)0,		/*  170		*/
    GLYPH_guillemotleft,	/*  171		*/
    GLYPH_logicalnot,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    (const char *)0,		/*  174		*/
    GLYPH_afii00208,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_plusminus,		/*  177		*/
    GLYPH_twosuperior,		/*  178		*/
    GLYPH_threesuperior,	/*  179		*/
    GLYPH_tonos,		/*  180		*/
    GLYPH_dieresistonos,	/*  181		*/
    GLYPH_Alphatonos,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_Epsilontonos,		/*  184		*/
    GLYPH_Etatonos,		/*  185		*/
    GLYPH_Iotatonos,		/*  186		*/
    GLYPH_guillemotright,	/*  187		*/
    GLYPH_Omicrontonos,		/*  188		*/
    GLYPH_onehalf,		/*  189		*/
    GLYPH_Upsilontonos,		/*  190		*/
    GLYPH_Omegatonos,		/*  191		*/
    GLYPH_iotadieresistonos,	/*  192		*/
    GLYPH_Alpha,		/*  193		*/
    GLYPH_Beta,			/*  194		*/
    GLYPH_Gamma,		/*  195		*/
    GLYPH_Delta,		/*  196		*/
    GLYPH_Epsilon,		/*  197		*/
    GLYPH_Zeta,			/*  198		*/
    GLYPH_Eta,			/*  199		*/
    GLYPH_Theta,		/*  200		*/
    GLYPH_Iota,			/*  201		*/
    GLYPH_Kappa,		/*  202		*/
    GLYPH_Lambda,		/*  203		*/
    GLYPH_Mu,			/*  204		*/
    GLYPH_Nu,			/*  205		*/
    GLYPH_Xi,			/*  206		*/
    GLYPH_Omicron,		/*  207		*/
    GLYPH_Pi,			/*  208		*/
    GLYPH_Rho,			/*  209		*/
    (const char *)0,		/*  210		*/
    GLYPH_Sigma,		/*  211		*/
    GLYPH_Tau,			/*  212		*/
    GLYPH_Upsilon,		/*  213		*/
    GLYPH_Phi,			/*  214		*/
    GLYPH_Chi,			/*  215		*/
    GLYPH_Psi,			/*  216		*/
    GLYPH_Omega,		/*  217		*/
    GLYPH_Iotadieresis,		/*  218		*/
    GLYPH_Upsilondieresis,	/*  219		*/
    GLYPH_alphatonos,		/*  220		*/
    GLYPH_epsilontonos,		/*  221		*/
    GLYPH_etatonos,		/*  222		*/
    GLYPH_iotatonos,		/*  223		*/
    GLYPH_upsilondieresistonos,	/*  224		*/
    GLYPH_alpha,		/*  225		*/
    GLYPH_beta,			/*  226		*/
    GLYPH_gamma,		/*  227		*/
    GLYPH_delta,		/*  228		*/
    GLYPH_epsilon,		/*  229		*/
    GLYPH_zeta,			/*  230		*/
    GLYPH_eta,			/*  231		*/
    GLYPH_theta,		/*  232		*/
    GLYPH_iota,			/*  233		*/
    GLYPH_kappa,		/*  234		*/
    GLYPH_lambda,		/*  235		*/
    GLYPH_mu,			/*  236		*/
    GLYPH_nu,			/*  237		*/
    GLYPH_xi,			/*  238		*/
    GLYPH_omicron,		/*  239		*/
    GLYPH_pi,			/*  240		*/
    GLYPH_rho,			/*  241		*/
    GLYPH_sigma1,		/*  242		*/
    GLYPH_sigma,		/*  243		*/
    GLYPH_tau,			/*  244		*/
    GLYPH_upsilon,		/*  245		*/
    GLYPH_phi,			/*  246		*/
    GLYPH_chi,			/*  247		*/
    GLYPH_psi,			/*  248		*/
    GLYPH_omega,		/*  249		*/
    GLYPH_iotadieresis,		/*  250		*/
    GLYPH_upsilondieresis,	/*  251		*/
    GLYPH_omicrontonos,		/*  252		*/
    GLYPH_upsilontonos,		/*  253		*/
    GLYPH_omegatonos,		/*  254		*/
    (const char *)0,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  ISO Latin9 (ISO 8859-9: Turkish) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin9GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_exclamdown,		/*  161		*/
    GLYPH_cent,			/*  162		*/
    GLYPH_sterling,		/*  163		*/
    GLYPH_currency,		/*  164		*/
    GLYPH_yen,			/*  165		*/
    GLYPH_brokenbar,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_dieresis,		/*  168		*/
    GLYPH_copyright,		/*  169		*/
    GLYPH_ordfeminine,		/*  170		*/
    GLYPH_guillemotleft,	/*  171		*/
    GLYPH_logicalnot,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_registered,		/*  174		*/
    GLYPH_macron,		/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_plusminus,		/*  177		*/
    GLYPH_twosuperior,		/*  178		*/
    GLYPH_threesuperior,	/*  179		*/
    GLYPH_acute,		/*  180		*/
    GLYPH_mu,			/*  181		*/
    GLYPH_paragraph,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_cedilla,		/*  184		*/
    GLYPH_onesuperior,		/*  185		*/
    GLYPH_ordmasculine,		/*  186		*/
    GLYPH_guillemotright,	/*  187		*/
    GLYPH_onequarter,		/*  188		*/
    GLYPH_onehalf,		/*  189		*/
    GLYPH_threequarters,	/*  190		*/
    GLYPH_questiondown,		/*  191		*/
    GLYPH_Agrave,		/*  192		*/
    GLYPH_Aacute,		/*  193		*/
    GLYPH_Acircumflex,		/*  194		*/
    GLYPH_Atilde,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Aring,		/*  197		*/
    GLYPH_AE,			/*  198		*/
    GLYPH_Ccedilla,		/*  199		*/
    GLYPH_Egrave,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Ecircumflex,		/*  202		*/
    GLYPH_Edieresis,		/*  203		*/
    GLYPH_Igrave,		/*  204		*/
    GLYPH_Iacute,		/*  205		*/
    GLYPH_Icircumflex,		/*  206		*/
    GLYPH_Idieresis,		/*  207		*/
    GLYPH_Gbreve,		/*  208		*/
    GLYPH_Ntilde,		/*  209		*/
    GLYPH_Ograve,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Ocircumflex,		/*  212		*/
    GLYPH_Otilde,		/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_multiply,		/*  215		*/
    GLYPH_Oslash,		/*  216		*/
    GLYPH_Ugrave,		/*  217		*/
    GLYPH_Uacute,		/*  218		*/
    GLYPH_Ucircumflex,		/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Idotaccent,		/*  221		*/
    GLYPH_Scedilla,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_agrave,		/*  224		*/
    GLYPH_aacute,		/*  225		*/
    GLYPH_acircumflex,		/*  226		*/
    GLYPH_atilde,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_aring,		/*  229		*/
    GLYPH_ae,			/*  230		*/
    GLYPH_ccedilla,		/*  231		*/
    GLYPH_egrave,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_ecircumflex,		/*  234		*/
    GLYPH_edieresis,		/*  235		*/
    GLYPH_igrave,		/*  236		*/
    GLYPH_iacute,		/*  237		*/
    GLYPH_icircumflex,		/*  238		*/
    GLYPH_idieresis,		/*  239		*/
    GLYPH_gbreve,		/*  240		*/
    GLYPH_ntilde,		/*  241		*/
    GLYPH_ograve,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_ocircumflex,		/*  244		*/
    GLYPH_otilde,		/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_divide,		/*  247		*/
    GLYPH_oslash,		/*  248		*/
    GLYPH_ugrave,		/*  249		*/
    GLYPH_uacute,		/*  250		*/
    GLYPH_ucircumflex,		/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_dotlessi,		/*  253		*/
    GLYPH_scedilla,		/*  254		*/
    GLYPH_ydieresis,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  ISO Latin7 (ISO 8859-10: Nordic) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin10GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_Aogonek,		/*  161		*/
    GLYPH_Emacron,		/*  162		*/
    GLYPH_Gcommaaccent,		/*  163		*/
    GLYPH_Imacron,		/*  164		*/
    GLYPH_Itilde,		/*  165		*/
    GLYPH_Kcommaaccent,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_Lcommaaccent,		/*  168		*/
    GLYPH_Dcroat,		/*  169		*/
    GLYPH_Scaron,		/*  170		*/
    GLYPH_Tbar,			/*  171		*/
    GLYPH_Zcaron,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_Umacron,		/*  174		*/
    GLYPH_Eng,			/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_aogonek,		/*  177		*/
    GLYPH_emacron,		/*  178		*/
    GLYPH_gcommaaccent,		/*  179		*/
    GLYPH_imacron,		/*  180		*/
    GLYPH_itilde,		/*  181		*/
    GLYPH_kcommaaccent,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_lcommaaccent,		/*  184		*/
    GLYPH_dcroat,		/*  185		*/
    GLYPH_scaron,		/*  186		*/
    GLYPH_tbar,			/*  187		*/
    GLYPH_zcaron,		/*  188		*/
    GLYPH_afii00208,		/*  189		*/
    GLYPH_umacron,		/*  190		*/
    GLYPH_eng,			/*  191		*/
    GLYPH_Amacron,		/*  192		*/
    GLYPH_Aacute,		/*  193		*/
    GLYPH_Acircumflex,		/*  194		*/
    GLYPH_Atilde,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Aring,		/*  197		*/
    GLYPH_AE,			/*  198		*/
    GLYPH_Iogonek,		/*  199		*/
    GLYPH_Ccaron,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Eogonek,		/*  202		*/
    GLYPH_Edieresis,		/*  203		*/
    GLYPH_Edotaccent,		/*  204		*/
    GLYPH_Iacute,		/*  205		*/
    GLYPH_Icircumflex,		/*  206		*/
    GLYPH_Idieresis,		/*  207		*/
    GLYPH_Eth,			/*  208		*/
    GLYPH_Ncommaaccent,		/*  209		*/
    GLYPH_Omacron,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Ocircumflex,		/*  212		*/
    GLYPH_Otilde,		/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_Utilde,		/*  215		*/
    GLYPH_Oslash,		/*  216		*/
    GLYPH_Uogonek,		/*  217		*/
    GLYPH_Uacute,		/*  218		*/
    GLYPH_Ucircumflex,		/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Yacute,		/*  221		*/
    GLYPH_Thorn,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_amacron,		/*  224		*/
    GLYPH_aacute,		/*  225		*/
    GLYPH_acircumflex,		/*  226		*/
    GLYPH_atilde,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_aring,		/*  229		*/
    GLYPH_ae,			/*  230		*/
    GLYPH_iogonek,		/*  231		*/
    GLYPH_ccaron,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_eogonek,		/*  234		*/
    GLYPH_edieresis,		/*  235		*/
    GLYPH_edotaccent,		/*  236		*/
    GLYPH_iacute,		/*  237		*/
    GLYPH_icircumflex,		/*  238		*/
    GLYPH_idieresis,		/*  239		*/
    GLYPH_eth,			/*  240		*/
    GLYPH_ncommaaccent,		/*  241		*/
    GLYPH_omacron,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_ocircumflex,		/*  244		*/
    GLYPH_otilde,		/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_utilde,		/*  247		*/
    GLYPH_oslash,		/*  248		*/
    GLYPH_uogonek,		/*  249		*/
    GLYPH_uacute,		/*  250		*/
    GLYPH_ucircumflex,		/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_yacute,		/*  253		*/
    GLYPH_thorn,		/*  254		*/
    GLYPH_kgreenlandic,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  ISO Baltic (ISO 8859-13: Baltic) to postscript glyph names.		*/
/*									*/
/************************************************************************/

const char * const	psIsoLatin13GlyphNames[256]=
    {
    (const char *)0,		/*    0		*/
    (const char *)0,		/*    1		*/
    (const char *)0,		/*    2		*/
    (const char *)0,		/*    3		*/
    (const char *)0,		/*    4		*/
    (const char *)0,		/*    5		*/
    (const char *)0,		/*    6		*/
    (const char *)0,		/*    7		*/
    (const char *)0,		/*    8		*/
    (const char *)0,		/*    9		*/
    (const char *)0,		/*   10		*/
    (const char *)0,		/*   11		*/
    (const char *)0,		/*   12		*/
    (const char *)0,		/*   13		*/
    (const char *)0,		/*   14		*/
    (const char *)0,		/*   15		*/
    (const char *)0,		/*   16		*/
    (const char *)0,		/*   17		*/
    (const char *)0,		/*   18		*/
    (const char *)0,		/*   19		*/
    (const char *)0,		/*   20		*/
    (const char *)0,		/*   21		*/
    (const char *)0,		/*   22		*/
    (const char *)0,		/*   23		*/
    (const char *)0,		/*   24		*/
    (const char *)0,		/*   25		*/
    (const char *)0,		/*   26		*/
    (const char *)0,		/*   27		*/
    (const char *)0,		/*   28		*/
    (const char *)0,		/*   29		*/
    (const char *)0,		/*   30		*/
    (const char *)0,		/*   31		*/
    GLYPH_space,		/*   32		*/
    GLYPH_exclam,		/*   33		*/
    GLYPH_quotedbl,		/*   34		*/
    GLYPH_numbersign,		/*   35		*/
    GLYPH_dollar,		/*   36		*/
    GLYPH_percent,		/*   37		*/
    GLYPH_ampersand,		/*   38		*/
    GLYPH_quotesingle,		/*   39		*/
    GLYPH_parenleft,		/*   40		*/
    GLYPH_parenright,		/*   41		*/
    GLYPH_asterisk,		/*   42		*/
    GLYPH_plus,			/*   43		*/
    GLYPH_comma,		/*   44		*/
    GLYPH_hyphen,		/*   45		*/
    GLYPH_period,		/*   46		*/
    GLYPH_slash,		/*   47		*/
    GLYPH_zero,			/*   48		*/
    GLYPH_one,			/*   49		*/
    GLYPH_two,			/*   50		*/
    GLYPH_three,		/*   51		*/
    GLYPH_four,			/*   52		*/
    GLYPH_five,			/*   53		*/
    GLYPH_six,			/*   54		*/
    GLYPH_seven,		/*   55		*/
    GLYPH_eight,		/*   56		*/
    GLYPH_nine,			/*   57		*/
    GLYPH_colon,		/*   58		*/
    GLYPH_semicolon,		/*   59		*/
    GLYPH_less,			/*   60		*/
    GLYPH_equal,		/*   61		*/
    GLYPH_greater,		/*   62		*/
    GLYPH_question,		/*   63		*/
    GLYPH_at,			/*   64		*/
    GLYPH_A,			/*   65		*/
    GLYPH_B,			/*   66		*/
    GLYPH_C,			/*   67		*/
    GLYPH_D,			/*   68		*/
    GLYPH_E,			/*   69		*/
    GLYPH_F,			/*   70		*/
    GLYPH_G,			/*   71		*/
    GLYPH_H,			/*   72		*/
    GLYPH_I,			/*   73		*/
    GLYPH_J,			/*   74		*/
    GLYPH_K,			/*   75		*/
    GLYPH_L,			/*   76		*/
    GLYPH_M,			/*   77		*/
    GLYPH_N,			/*   78		*/
    GLYPH_O,			/*   79		*/
    GLYPH_P,			/*   80		*/
    GLYPH_Q,			/*   81		*/
    GLYPH_R,			/*   82		*/
    GLYPH_S,			/*   83		*/
    GLYPH_T,			/*   84		*/
    GLYPH_U,			/*   85		*/
    GLYPH_V,			/*   86		*/
    GLYPH_W,			/*   87		*/
    GLYPH_X,			/*   88		*/
    GLYPH_Y,			/*   89		*/
    GLYPH_Z,			/*   90		*/
    GLYPH_bracketleft,		/*   91		*/
    GLYPH_backslash,		/*   92		*/
    GLYPH_bracketright,		/*   93		*/
    GLYPH_asciicircum,		/*   94		*/
    GLYPH_underscore,		/*   95		*/
    GLYPH_grave,		/*   96		*/
    GLYPH_a,			/*   97		*/
    GLYPH_b,			/*   98		*/
    GLYPH_c,			/*   99		*/
    GLYPH_d,			/*  100		*/
    GLYPH_e,			/*  101		*/
    GLYPH_f,			/*  102		*/
    GLYPH_g,			/*  103		*/
    GLYPH_h,			/*  104		*/
    GLYPH_i,			/*  105		*/
    GLYPH_j,			/*  106		*/
    GLYPH_k,			/*  107		*/
    GLYPH_l,			/*  108		*/
    GLYPH_m,			/*  109		*/
    GLYPH_n,			/*  110		*/
    GLYPH_o,			/*  111		*/
    GLYPH_p,			/*  112		*/
    GLYPH_q,			/*  113		*/
    GLYPH_r,			/*  114		*/
    GLYPH_s,			/*  115		*/
    GLYPH_t,			/*  116		*/
    GLYPH_u,			/*  117		*/
    GLYPH_v,			/*  118		*/
    GLYPH_w,			/*  119		*/
    GLYPH_x,			/*  120		*/
    GLYPH_y,			/*  121		*/
    GLYPH_z,			/*  122		*/
    GLYPH_braceleft,		/*  123		*/
    GLYPH_bar,			/*  124		*/
    GLYPH_braceright,		/*  125		*/
    GLYPH_asciitilde,		/*  126		*/
    (const char *)0,		/*  127		*/
    (const char *)0,		/*  128		*/
    (const char *)0,		/*  129		*/
    (const char *)0,		/*  130		*/
    (const char *)0,		/*  131		*/
    (const char *)0,		/*  132		*/
    (const char *)0,		/*  133		*/
    (const char *)0,		/*  134		*/
    (const char *)0,		/*  135		*/
    (const char *)0,		/*  136		*/
    (const char *)0,		/*  137		*/
    (const char *)0,		/*  138		*/
    (const char *)0,		/*  139		*/
    (const char *)0,		/*  140		*/
    (const char *)0,		/*  141		*/
    (const char *)0,		/*  142		*/
    (const char *)0,		/*  143		*/
    (const char *)0,		/*  144		*/
    (const char *)0,		/*  145		*/
    (const char *)0,		/*  146		*/
    (const char *)0,		/*  147		*/
    (const char *)0,		/*  148		*/
    (const char *)0,		/*  149		*/
    (const char *)0,		/*  150		*/
    (const char *)0,		/*  151		*/
    (const char *)0,		/*  152		*/
    (const char *)0,		/*  153		*/
    (const char *)0,		/*  154		*/
    (const char *)0,		/*  155		*/
    (const char *)0,		/*  156		*/
    (const char *)0,		/*  157		*/
    (const char *)0,		/*  158		*/
    (const char *)0,		/*  159		*/
    GLYPH_nobreakspace,		/*  160		*/
    GLYPH_quotedblright,	/*  161		*/
    GLYPH_cent,			/*  162		*/
    GLYPH_sterling,		/*  163		*/
    GLYPH_currency,		/*  164		*/
    GLYPH_quotedblbase,		/*  165		*/
    GLYPH_brokenbar,		/*  166		*/
    GLYPH_section,		/*  167		*/
    GLYPH_Oslash,		/*  168		*/
    GLYPH_copyright,		/*  169		*/
    GLYPH_Rcommaaccent,		/*  170		*/
    GLYPH_guillemotleft,	/*  171		*/
    GLYPH_logicalnot,		/*  172		*/
    GLYPH_hyphen,		/*  173		*/
    GLYPH_registered,		/*  174		*/
    GLYPH_AE,			/*  175		*/
    GLYPH_degree,		/*  176		*/
    GLYPH_plusminus,		/*  177		*/
    GLYPH_twosuperior,		/*  178		*/
    GLYPH_threesuperior,	/*  179		*/
    GLYPH_quotedblleft,		/*  180		*/
    GLYPH_mu,			/*  181		*/
    GLYPH_paragraph,		/*  182		*/
    GLYPH_periodcentered,	/*  183		*/
    GLYPH_oslash,		/*  184		*/
    GLYPH_onesuperior,		/*  185		*/
    GLYPH_rcommaaccent,		/*  186		*/
    GLYPH_guillemotright,	/*  187		*/
    GLYPH_onequarter,		/*  188		*/
    GLYPH_onehalf,		/*  189		*/
    GLYPH_threequarters,	/*  190		*/
    GLYPH_ae,			/*  191		*/
    GLYPH_Aogonek,		/*  192		*/
    GLYPH_Iogonek,		/*  193		*/
    GLYPH_Amacron,		/*  194		*/
    GLYPH_Cacute,		/*  195		*/
    GLYPH_Adieresis,		/*  196		*/
    GLYPH_Aring,		/*  197		*/
    GLYPH_Eogonek,		/*  198		*/
    GLYPH_Emacron,		/*  199		*/
    GLYPH_Ccaron,		/*  200		*/
    GLYPH_Eacute,		/*  201		*/
    GLYPH_Zacute,		/*  202		*/
    GLYPH_Edotaccent,		/*  203		*/
    GLYPH_Gcommaaccent,		/*  204		*/
    GLYPH_Kcommaaccent,		/*  205		*/
    GLYPH_Imacron,		/*  206		*/
    GLYPH_Lcommaaccent,		/*  207		*/
    GLYPH_Scaron,		/*  208		*/
    GLYPH_Nacute,		/*  209		*/
    GLYPH_Ncommaaccent,		/*  210		*/
    GLYPH_Oacute,		/*  211		*/
    GLYPH_Omacron,		/*  212		*/
    GLYPH_Otilde,		/*  213		*/
    GLYPH_Odieresis,		/*  214		*/
    GLYPH_multiply,		/*  215		*/
    GLYPH_Uogonek,		/*  216		*/
    GLYPH_Lslash,		/*  217		*/
    GLYPH_Sacute,		/*  218		*/
    GLYPH_Umacron,		/*  219		*/
    GLYPH_Udieresis,		/*  220		*/
    GLYPH_Zdotaccent,		/*  221		*/
    GLYPH_Zcaron,		/*  222		*/
    GLYPH_germandbls,		/*  223		*/
    GLYPH_aogonek,		/*  224		*/
    GLYPH_iogonek,		/*  225		*/
    GLYPH_amacron,		/*  226		*/
    GLYPH_cacute,		/*  227		*/
    GLYPH_adieresis,		/*  228		*/
    GLYPH_aring,		/*  229		*/
    GLYPH_eogonek,		/*  230		*/
    GLYPH_emacron,		/*  231		*/
    GLYPH_ccaron,		/*  232		*/
    GLYPH_eacute,		/*  233		*/
    GLYPH_zacute,		/*  234		*/
    GLYPH_edotaccent,		/*  235		*/
    GLYPH_gcommaaccent,		/*  236		*/
    GLYPH_kcommaaccent,		/*  237		*/
    GLYPH_imacron,		/*  238		*/
    GLYPH_lcommaaccent,		/*  239		*/
    GLYPH_scaron,		/*  240		*/
    GLYPH_nacute,		/*  241		*/
    GLYPH_ncommaaccent,		/*  242		*/
    GLYPH_oacute,		/*  243		*/
    GLYPH_omacron,		/*  244		*/
    GLYPH_otilde,		/*  245		*/
    GLYPH_odieresis,		/*  246		*/
    GLYPH_divide,		/*  247		*/
    GLYPH_uogonek,		/*  248		*/
    GLYPH_lslash,		/*  249		*/
    GLYPH_sacute,		/*  250		*/
    GLYPH_umacron,		/*  251		*/
    GLYPH_udieresis,		/*  252		*/
    GLYPH_zdotaccent,		/*  253		*/
    GLYPH_zcaron,		/*  254		*/
    GLYPH_quoteright,		/*  255		*/
    };

/************************************************************************/
/*									*/
/*  Adobe Cyrillic Encoding to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psCyrillicGlyphNames[256]=
    {
    (char *)0,			/*  0x00=   0  */
    (char *)0,			/*  0x01=   1  */
    (char *)0,			/*  0x02=   2  */
    (char *)0,			/*  0x03=   3  */
    (char *)0,			/*  0x04=   4  */
    (char *)0,			/*  0x05=   5  */
    (char *)0,			/*  0x06=   6  */
    (char *)0,			/*  0x07=   7  */
    (char *)0,			/*  0x08=   8  */
    (char *)0,			/*  0x09=   9  */
    (char *)0,			/*  0x0a=  10  */
    (char *)0,			/*  0x0b=  11  */
    (char *)0,			/*  0x0c=  12  */
    (char *)0,			/*  0x0d=  13  */
    (char *)0,			/*  0x0e=  14  */
    (char *)0,			/*  0x0f=  15  */
    (char *)0,			/*  0x10=  16  */
    (char *)0,			/*  0x11=  17  */
    (char *)0,			/*  0x12=  18  */
    (char *)0,			/*  0x13=  19  */
    (char *)0,			/*  0x14=  20  */
    (char *)0,			/*  0x15=  21  */
    (char *)0,			/*  0x16=  22  */
    (char *)0,			/*  0x17=  23  */
    (char *)0,			/*  0x18=  24  */
    (char *)0,			/*  0x19=  25  */
    (char *)0,			/*  0x1a=  26  */
    (char *)0,			/*  0x1b=  27  */
    (char *)0,			/*  0x1c=  28  */
    (char *)0,			/*  0x1d=  29  */
    (char *)0,			/*  0x1e=  30  */
    (char *)0,			/*  0x1f=  31  */
    GLYPH_space,		/*  0x20=  32  */
    GLYPH_exclam,		/*  0x21=  33  */
    GLYPH_quotedbl,		/*  0x22=  34  */
    GLYPH_numbersign,		/*  0x23=  35  */
    GLYPH_dollar,		/*  0x24=  36  */
    GLYPH_percent,		/*  0x25=  37  */
    GLYPH_ampersand,		/*  0x26=  38  */
    GLYPH_quotesingle,		/*  0x27=  39  */
    GLYPH_parenleft,		/*  0x28=  40  */
    GLYPH_parenright,		/*  0x29=  41  */
    GLYPH_asterisk,		/*  0x2a=  42  */
    GLYPH_plus,			/*  0x2b=  43  */
    GLYPH_comma,		/*  0x2c=  44  */
    GLYPH_hyphen,		/*  0x2d=  45  */
    GLYPH_period,		/*  0x2e=  46  */
    GLYPH_slash,		/*  0x2f=  47  */
    GLYPH_zero,			/*  0x30=  48  */
    GLYPH_one,			/*  0x31=  49  */
    GLYPH_two,			/*  0x32=  50  */
    GLYPH_three,		/*  0x33=  51  */
    GLYPH_four,			/*  0x34=  52  */
    GLYPH_five,			/*  0x35=  53  */
    GLYPH_six,			/*  0x36=  54  */
    GLYPH_seven,		/*  0x37=  55  */
    GLYPH_eight,		/*  0x38=  56  */
    GLYPH_nine,			/*  0x39=  57  */
    GLYPH_colon,		/*  0x3a=  58  */
    GLYPH_semicolon,		/*  0x3b=  59  */
    GLYPH_less,			/*  0x3c=  60  */
    GLYPH_equal,		/*  0x3d=  61  */
    GLYPH_greater,		/*  0x3e=  62  */
    GLYPH_question,		/*  0x3f=  63  */
    GLYPH_at,			/*  0x40=  64  */
    GLYPH_A,			/*  0x41=  65  */
    GLYPH_B,			/*  0x42=  66  */
    GLYPH_C,			/*  0x43=  67  */
    GLYPH_D,			/*  0x44=  68  */
    GLYPH_E,			/*  0x45=  69  */
    GLYPH_F,			/*  0x46=  70  */
    GLYPH_G,			/*  0x47=  71  */
    GLYPH_H,			/*  0x48=  72  */
    GLYPH_I,			/*  0x49=  73  */
    GLYPH_J,			/*  0x4a=  74  */
    GLYPH_K,			/*  0x4b=  75  */
    GLYPH_L,			/*  0x4c=  76  */
    GLYPH_M,			/*  0x4d=  77  */
    GLYPH_N,			/*  0x4e=  78  */
    GLYPH_O,			/*  0x4f=  79  */
    GLYPH_P,			/*  0x50=  80  */
    GLYPH_Q,			/*  0x51=  81  */
    GLYPH_R,			/*  0x52=  82  */
    GLYPH_S,			/*  0x53=  83  */
    GLYPH_T,			/*  0x54=  84  */
    GLYPH_U,			/*  0x55=  85  */
    GLYPH_V,			/*  0x56=  86  */
    GLYPH_W,			/*  0x57=  87  */
    GLYPH_X,			/*  0x58=  88  */
    GLYPH_Y,			/*  0x59=  89  */
    GLYPH_Z,			/*  0x5a=  90  */
    GLYPH_bracketleft,		/*  0x5b=  91  */
    GLYPH_backslash,		/*  0x5c=  92  */
    GLYPH_bracketright,		/*  0x5d=  93  */
    GLYPH_asciicircum,		/*  0x5e=  94  */
    GLYPH_underscore,		/*  0x5f=  95  */
    GLYPH_grave,		/*  0x60=  96  */
    GLYPH_a,			/*  0x61=  97  */
    GLYPH_b,			/*  0x62=  98  */
    GLYPH_c,			/*  0x63=  99  */
    GLYPH_d,			/*  0x64= 100  */
    GLYPH_e,			/*  0x65= 101  */
    GLYPH_f,			/*  0x66= 102  */
    GLYPH_g,			/*  0x67= 103  */
    GLYPH_h,			/*  0x68= 104  */
    GLYPH_i,			/*  0x69= 105  */
    GLYPH_j,			/*  0x6a= 106  */
    GLYPH_k,			/*  0x6b= 107  */
    GLYPH_l,			/*  0x6c= 108  */
    GLYPH_m,			/*  0x6d= 109  */
    GLYPH_n,			/*  0x6e= 110  */
    GLYPH_o,			/*  0x6f= 111  */
    GLYPH_p,			/*  0x70= 112  */
    GLYPH_q,			/*  0x71= 113  */
    GLYPH_r,			/*  0x72= 114  */
    GLYPH_s,			/*  0x73= 115  */
    GLYPH_t,			/*  0x74= 116  */
    GLYPH_u,			/*  0x75= 117  */
    GLYPH_v,			/*  0x76= 118  */
    GLYPH_w,			/*  0x77= 119  */
    GLYPH_x,			/*  0x78= 120  */
    GLYPH_y,			/*  0x79= 121  */
    GLYPH_z,			/*  0x7a= 122  */
    GLYPH_braceleft,		/*  0x7b= 123  */
    GLYPH_bar,			/*  0x7c= 124  */
    GLYPH_braceright,		/*  0x7d= 125  */
    GLYPH_asciitilde,		/*  0x7e= 126  */
    (char *)0,			/*  0x7f= 127  */
    GLYPH_afii10051,		/*  0x80= 128  */
    GLYPH_afii10052,		/*  0x81= 129  */
    GLYPH_quotesinglbase,	/*  0x82= 130  */
    GLYPH_afii10100,		/*  0x83= 131  */
    GLYPH_quotedblbase,		/*  0x84= 132  */
    GLYPH_ellipsis,		/*  0x85= 133  */
    GLYPH_dagger,		/*  0x86= 134  */
    GLYPH_daggerdbl,		/*  0x87= 135  */
    (char *)0,			/*  0x88= 136  */
    GLYPH_perthousand,		/*  0x89= 137  */
    GLYPH_afii10058,		/*  0x8a= 138  */
    GLYPH_guilsinglleft,	/*  0x8b= 139  */
    GLYPH_afii10059,		/*  0x8c= 140  */
    GLYPH_afii10061,		/*  0x8d= 141  */
    GLYPH_afii10060,		/*  0x8e= 142  */
    GLYPH_afii10145,		/*  0x8f= 143  */
    GLYPH_afii10099,		/*  0x90= 144  */
    GLYPH_quoteleft,		/*  0x91= 145  */
    GLYPH_quoteright,		/*  0x92= 146  */
    GLYPH_quotedblleft,		/*  0x93= 147  */
    GLYPH_quotedblright,	/*  0x94= 148  */
    GLYPH_bullet,		/*  0x95= 149  */
    GLYPH_endash,		/*  0x96= 150  */
    GLYPH_emdash,		/*  0x97= 151  */
    (char *)0,			/*  0x98= 152  */
    GLYPH_trademark,		/*  0x99= 153  */
    GLYPH_afii10106,		/*  0x9a= 154  */
    GLYPH_guilsinglright,	/*  0x9b= 155  */
    GLYPH_afii10107,		/*  0x9c= 156  */
    GLYPH_afii10109,		/*  0x9d= 157  */
    GLYPH_afii10108,		/*  0x9e= 158  */
    GLYPH_afii10193,		/*  0x9f= 159  */
    GLYPH_nobreakspace,		/*  0xa0= 160  */
    GLYPH_afii10062,		/*  0xa1= 161  */
    GLYPH_afii10110,		/*  0xa2= 162  */
    GLYPH_afii10057,		/*  0xa3= 163  */
    GLYPH_currency,		/*  0xa4= 164  */
    GLYPH_afii10050,		/*  0xa5= 165  */
    GLYPH_brokenbar,		/*  0xa6= 166  */
    GLYPH_section,		/*  0xa7= 167  */
    GLYPH_afii10023,		/*  0xa8= 168  */
    GLYPH_copyright,		/*  0xa9= 169  */
    GLYPH_afii10053,		/*  0xaa= 170  */
    GLYPH_guillemotleft,	/*  0xab= 171  */
    GLYPH_logicalnot,		/*  0xac= 172  */
    GLYPH_hyphen,		/*  0xad= 173  */
    GLYPH_registered,		/*  0xae= 174  */
    GLYPH_afii10056,		/*  0xaf= 175  */
    GLYPH_degree,		/*  0xb0= 176  */
    GLYPH_plusminus,		/*  0xb1= 177  */
    GLYPH_afii10055,		/*  0xb2= 178  */
    GLYPH_afii10103,		/*  0xb3= 179  */
    GLYPH_afii10098,		/*  0xb4= 180  */
    GLYPH_mu,			/*  0xb5= 181  */
    GLYPH_paragraph,		/*  0xb6= 182  */
    GLYPH_periodcentered,	/*  0xb7= 183  */
    GLYPH_afii10071,		/*  0xb8= 184  */
    GLYPH_afii61352,		/*  0xb9= 185  */
    GLYPH_afii10101,		/*  0xba= 186  */
    GLYPH_guillemotright,	/*  0xbb= 187  */
    GLYPH_afii10105,		/*  0xbc= 188  */
    GLYPH_afii10054,		/*  0xbd= 189  */
    GLYPH_afii10102,		/*  0xbe= 190  */
    GLYPH_afii10104,		/*  0xbf= 191  */
    GLYPH_afii10017,		/*  0xc0= 192  */
    GLYPH_afii10018,		/*  0xc1= 193  */
    GLYPH_afii10019,		/*  0xc2= 194  */
    GLYPH_afii10020,		/*  0xc3= 195  */
    GLYPH_afii10021,		/*  0xc4= 196  */
    GLYPH_afii10022,		/*  0xc5= 197  */
    GLYPH_afii10024,		/*  0xc6= 198  */
    GLYPH_afii10025,		/*  0xc7= 199  */
    GLYPH_afii10026,		/*  0xc8= 200  */
    GLYPH_afii10027,		/*  0xc9= 201  */
    GLYPH_afii10028,		/*  0xca= 202  */
    GLYPH_afii10029,		/*  0xcb= 203  */
    GLYPH_afii10030,		/*  0xcc= 204  */
    GLYPH_afii10031,		/*  0xcd= 205  */
    GLYPH_afii10032,		/*  0xce= 206  */
    GLYPH_afii10033,		/*  0xcf= 207  */
    GLYPH_afii10034,		/*  0xd0= 208  */
    GLYPH_afii10035,		/*  0xd1= 209  */
    GLYPH_afii10036,		/*  0xd2= 210  */
    GLYPH_afii10037,		/*  0xd3= 211  */
    GLYPH_afii10038,		/*  0xd4= 212  */
    GLYPH_afii10039,		/*  0xd5= 213  */
    GLYPH_afii10040,		/*  0xd6= 214  */
    GLYPH_afii10041,		/*  0xd7= 215  */
    GLYPH_afii10042,		/*  0xd8= 216  */
    GLYPH_afii10043,		/*  0xd9= 217  */
    GLYPH_afii10044,		/*  0xda= 218  */
    GLYPH_afii10045,		/*  0xdb= 219  */
    GLYPH_afii10046,		/*  0xdc= 220  */
    GLYPH_afii10047,		/*  0xdd= 221  */
    GLYPH_afii10048,		/*  0xde= 222  */
    GLYPH_afii10049,		/*  0xdf= 223  */
    GLYPH_afii10065,		/*  0xe0= 224  */
    GLYPH_afii10066,		/*  0xe1= 225  */
    GLYPH_afii10067,		/*  0xe2= 226  */
    GLYPH_afii10068,		/*  0xe3= 227  */
    GLYPH_afii10069,		/*  0xe4= 228  */
    GLYPH_afii10070,		/*  0xe5= 229  */
    GLYPH_afii10072,		/*  0xe6= 230  */
    GLYPH_afii10073,		/*  0xe7= 231  */
    GLYPH_afii10074,		/*  0xe8= 232  */
    GLYPH_afii10075,		/*  0xe9= 233  */
    GLYPH_afii10076,		/*  0xea= 234  */
    GLYPH_afii10077,		/*  0xeb= 235  */
    GLYPH_afii10078,		/*  0xec= 236  */
    GLYPH_afii10079,		/*  0xed= 237  */
    GLYPH_afii10080,		/*  0xee= 238  */
    GLYPH_afii10081,		/*  0xef= 239  */
    GLYPH_afii10082,		/*  0xf0= 240  */
    GLYPH_afii10083,		/*  0xf1= 241  */
    GLYPH_afii10084,		/*  0xf2= 242  */
    GLYPH_afii10085,		/*  0xf3= 243  */
    GLYPH_afii10086,		/*  0xf4= 244  */
    GLYPH_afii10087,		/*  0xf5= 245  */
    GLYPH_afii10088,		/*  0xf6= 246  */
    GLYPH_afii10089,		/*  0xf7= 247  */
    GLYPH_afii10090,		/*  0xf8= 248  */
    GLYPH_afii10091,		/*  0xf9= 249  */
    GLYPH_afii10092,		/*  0xfa= 250  */
    GLYPH_afii10093,		/*  0xfb= 251  */
    GLYPH_afii10094,		/*  0xfc= 252  */
    GLYPH_afii10095,		/*  0xfd= 253  */
    GLYPH_afii10096,		/*  0xfe= 254  */
    GLYPH_afii10097,		/*  0xff= 255  */
    };

/************************************************************************/
/*									*/
/*  Adobe ITC Zapf Dingbats to postscript glyph names.			*/
/*									*/
/************************************************************************/

const char * const	psDingbatGlyphNames[256]=
    {
    (char *)0,		/*  0x00=   0  */
    (char *)0,		/*  0x01=   1  */
    (char *)0,		/*  0x02=   2  */
    (char *)0,		/*  0x03=   3  */
    (char *)0,		/*  0x04=   4  */
    (char *)0,		/*  0x05=   5  */
    (char *)0,		/*  0x06=   6  */
    (char *)0,		/*  0x07=   7  */
    (char *)0,		/*  0x08=   8  */
    (char *)0,		/*  0x09=   9  */
    (char *)0,		/*  0x0a=  10  */
    (char *)0,		/*  0x0b=  11  */
    (char *)0,		/*  0x0c=  12  */
    (char *)0,		/*  0x0d=  13  */
    (char *)0,		/*  0x0e=  14  */
    (char *)0,		/*  0x0f=  15  */
    (char *)0,		/*  0x10=  16  */
    (char *)0,		/*  0x11=  17  */
    (char *)0,		/*  0x12=  18  */
    (char *)0,		/*  0x13=  19  */
    (char *)0,		/*  0x14=  20  */
    (char *)0,		/*  0x15=  21  */
    (char *)0,		/*  0x16=  22  */
    (char *)0,		/*  0x17=  23  */
    (char *)0,		/*  0x18=  24  */
    (char *)0,		/*  0x19=  25  */
    (char *)0,		/*  0x1a=  26  */
    (char *)0,		/*  0x1b=  27  */
    (char *)0,		/*  0x1c=  28  */
    (char *)0,		/*  0x1d=  29  */
    (char *)0,		/*  0x1e=  30  */
    (char *)0,		/*  0x1f=  31  */
    GLYPH_space,	/*  0x20=  32  */
    GLYPH_a1,		/*  0x21=  33  */
    GLYPH_a2,		/*  0x22=  34  */
    GLYPH_a202,		/*  0x23=  35  */
    GLYPH_a3,		/*  0x24=  36  */
    GLYPH_a4,		/*  0x25=  37  */
    GLYPH_a5,		/*  0x26=  38  */
    GLYPH_a119,		/*  0x27=  39  */
    GLYPH_a118,		/*  0x28=  40  */
    GLYPH_a117,		/*  0x29=  41  */
    GLYPH_a11,		/*  0x2a=  42  */
    GLYPH_a12,		/*  0x2b=  43  */
    GLYPH_a13,		/*  0x2c=  44  */
    GLYPH_a14,		/*  0x2d=  45  */
    GLYPH_a15,		/*  0x2e=  46  */
    GLYPH_a16,		/*  0x2f=  47  */
    GLYPH_a105,		/*  0x30=  48  */
    GLYPH_a17,		/*  0x31=  49  */
    GLYPH_a18,		/*  0x32=  50  */
    GLYPH_a19,		/*  0x33=  51  */
    GLYPH_a20,		/*  0x34=  52  */
    GLYPH_a21,		/*  0x35=  53  */
    GLYPH_a22,		/*  0x36=  54  */
    GLYPH_a23,		/*  0x37=  55  */
    GLYPH_a24,		/*  0x38=  56  */
    GLYPH_a25,		/*  0x39=  57  */
    GLYPH_a26,		/*  0x3a=  58  */
    GLYPH_a27,		/*  0x3b=  59  */
    GLYPH_a28,		/*  0x3c=  60  */
    GLYPH_a6,		/*  0x3d=  61  */
    GLYPH_a7,		/*  0x3e=  62  */
    GLYPH_a8,		/*  0x3f=  63  */
    GLYPH_a9,		/*  0x40=  64  */
    GLYPH_a10,		/*  0x41=  65  */
    GLYPH_a29,		/*  0x42=  66  */
    GLYPH_a30,		/*  0x43=  67  */
    GLYPH_a31,		/*  0x44=  68  */
    GLYPH_a32,		/*  0x45=  69  */
    GLYPH_a33,		/*  0x46=  70  */
    GLYPH_a34,		/*  0x47=  71  */
    GLYPH_a35,		/*  0x48=  72  */
    GLYPH_a36,		/*  0x49=  73  */
    GLYPH_a37,		/*  0x4a=  74  */
    GLYPH_a38,		/*  0x4b=  75  */
    GLYPH_a39,		/*  0x4c=  76  */
    GLYPH_a40,		/*  0x4d=  77  */
    GLYPH_a41,		/*  0x4e=  78  */
    GLYPH_a42,		/*  0x4f=  79  */
    GLYPH_a43,		/*  0x50=  80  */
    GLYPH_a44,		/*  0x51=  81  */
    GLYPH_a45,		/*  0x52=  82  */
    GLYPH_a46,		/*  0x53=  83  */
    GLYPH_a47,		/*  0x54=  84  */
    GLYPH_a48,		/*  0x55=  85  */
    GLYPH_a49,		/*  0x56=  86  */
    GLYPH_a50,		/*  0x57=  87  */
    GLYPH_a51,		/*  0x58=  88  */
    GLYPH_a52,		/*  0x59=  89  */
    GLYPH_a53,		/*  0x5a=  90  */
    GLYPH_a54,		/*  0x5b=  91  */
    GLYPH_a55,		/*  0x5c=  92  */
    GLYPH_a56,		/*  0x5d=  93  */
    GLYPH_a57,		/*  0x5e=  94  */
    GLYPH_a58,		/*  0x5f=  95  */
    GLYPH_a59,		/*  0x60=  96  */
    GLYPH_a60,		/*  0x61=  97  */
    GLYPH_a61,		/*  0x62=  98  */
    GLYPH_a62,		/*  0x63=  99  */
    GLYPH_a63,		/*  0x64= 100  */
    GLYPH_a64,		/*  0x65= 101  */
    GLYPH_a65,		/*  0x66= 102  */
    GLYPH_a66,		/*  0x67= 103  */
    GLYPH_a67,		/*  0x68= 104  */
    GLYPH_a68,		/*  0x69= 105  */
    GLYPH_a69,		/*  0x6a= 106  */
    GLYPH_a70,		/*  0x6b= 107  */
    GLYPH_a71,		/*  0x6c= 108  */
    GLYPH_a72,		/*  0x6d= 109  */
    GLYPH_a73,		/*  0x6e= 110  */
    GLYPH_a74,		/*  0x6f= 111  */
    GLYPH_a203,		/*  0x70= 112  */
    GLYPH_a75,		/*  0x71= 113  */
    GLYPH_a204,		/*  0x72= 114  */
    GLYPH_a76,		/*  0x73= 115  */
    GLYPH_a77,		/*  0x74= 116  */
    GLYPH_a78,		/*  0x75= 117  */
    GLYPH_a79,		/*  0x76= 118  */
    GLYPH_a81,		/*  0x77= 119  */
    GLYPH_a82,		/*  0x78= 120  */
    GLYPH_a83,		/*  0x79= 121  */
    GLYPH_a84,		/*  0x7a= 122  */
    GLYPH_a97,		/*  0x7b= 123  */
    GLYPH_a98,		/*  0x7c= 124  */
    GLYPH_a99,		/*  0x7d= 125  */
    GLYPH_a100,		/*  0x7e= 126  */
    (char *)0,		/*  0x7f= 127  */
    (char *)0,		/*  0x80= 128  */
    (char *)0,		/*  0x81= 129  */
    (char *)0,		/*  0x82= 130  */
    (char *)0,		/*  0x83= 131  */
    (char *)0,		/*  0x84= 132  */
    (char *)0,		/*  0x85= 133  */
    (char *)0,		/*  0x86= 134  */
    (char *)0,		/*  0x87= 135  */
    (char *)0,		/*  0x88= 136  */
    (char *)0,		/*  0x89= 137  */
    (char *)0,		/*  0x8a= 138  */
    (char *)0,		/*  0x8b= 139  */
    (char *)0,		/*  0x8c= 140  */
    (char *)0,		/*  0x8d= 141  */
    (char *)0,		/*  0x8e= 142  */
    (char *)0,		/*  0x8f= 143  */
    (char *)0,		/*  0x90= 144  */
    (char *)0,		/*  0x91= 145  */
    (char *)0,		/*  0x92= 146  */
    (char *)0,		/*  0x93= 147  */
    (char *)0,		/*  0x94= 148  */
    (char *)0,		/*  0x95= 149  */
    (char *)0,		/*  0x96= 150  */
    (char *)0,		/*  0x97= 151  */
    (char *)0,		/*  0x98= 152  */
    (char *)0,		/*  0x99= 153  */
    (char *)0,		/*  0x9a= 154  */
    (char *)0,		/*  0x9b= 155  */
    (char *)0,		/*  0x9c= 156  */
    (char *)0,		/*  0x9d= 157  */
    (char *)0,		/*  0x9e= 158  */
    (char *)0,		/*  0x9f= 159  */
    (char *)0,		/*  0xa0= 160  */
    GLYPH_a101,		/*  0xa1= 161  */
    GLYPH_a102,		/*  0xa2= 162  */
    GLYPH_a103,		/*  0xa3= 163  */
    GLYPH_a104,		/*  0xa4= 164  */
    GLYPH_a106,		/*  0xa5= 165  */
    GLYPH_a107,		/*  0xa6= 166  */
    GLYPH_a108,		/*  0xa7= 167  */
    GLYPH_a112,		/*  0xa8= 168  */
    GLYPH_a111,		/*  0xa9= 169  */
    GLYPH_a110,		/*  0xaa= 170  */
    GLYPH_a109,		/*  0xab= 171  */
    GLYPH_a120,		/*  0xac= 172  */
    GLYPH_a121,		/*  0xad= 173  */
    GLYPH_a122,		/*  0xae= 174  */
    GLYPH_a123,		/*  0xaf= 175  */
    GLYPH_a124,		/*  0xb0= 176  */
    GLYPH_a125,		/*  0xb1= 177  */
    GLYPH_a126,		/*  0xb2= 178  */
    GLYPH_a127,		/*  0xb3= 179  */
    GLYPH_a128,		/*  0xb4= 180  */
    GLYPH_a129,		/*  0xb5= 181  */
    GLYPH_a130,		/*  0xb6= 182  */
    GLYPH_a131,		/*  0xb7= 183  */
    GLYPH_a132,		/*  0xb8= 184  */
    GLYPH_a133,		/*  0xb9= 185  */
    GLYPH_a134,		/*  0xba= 186  */
    GLYPH_a135,		/*  0xbb= 187  */
    GLYPH_a136,		/*  0xbc= 188  */
    GLYPH_a137,		/*  0xbd= 189  */
    GLYPH_a138,		/*  0xbe= 190  */
    GLYPH_a139,		/*  0xbf= 191  */
    GLYPH_a140,		/*  0xc0= 192  */
    GLYPH_a141,		/*  0xc1= 193  */
    GLYPH_a142,		/*  0xc2= 194  */
    GLYPH_a143,		/*  0xc3= 195  */
    GLYPH_a144,		/*  0xc4= 196  */
    GLYPH_a145,		/*  0xc5= 197  */
    GLYPH_a146,		/*  0xc6= 198  */
    GLYPH_a147,		/*  0xc7= 199  */
    GLYPH_a148,		/*  0xc8= 200  */
    GLYPH_a149,		/*  0xc9= 201  */
    GLYPH_a150,		/*  0xca= 202  */
    GLYPH_a151,		/*  0xcb= 203  */
    GLYPH_a152,		/*  0xcc= 204  */
    GLYPH_a153,		/*  0xcd= 205  */
    GLYPH_a154,		/*  0xce= 206  */
    GLYPH_a155,		/*  0xcf= 207  */
    GLYPH_a156,		/*  0xd0= 208  */
    GLYPH_a157,		/*  0xd1= 209  */
    GLYPH_a158,		/*  0xd2= 210  */
    GLYPH_a159,		/*  0xd3= 211  */
    GLYPH_a160,		/*  0xd4= 212  */
    GLYPH_a161,		/*  0xd5= 213  */
    GLYPH_a163,		/*  0xd6= 214  */
    GLYPH_a164,		/*  0xd7= 215  */
    GLYPH_a196,		/*  0xd8= 216  */
    GLYPH_a165,		/*  0xd9= 217  */
    GLYPH_a192,		/*  0xda= 218  */
    GLYPH_a166,		/*  0xdb= 219  */
    GLYPH_a167,		/*  0xdc= 220  */
    GLYPH_a168,		/*  0xdd= 221  */
    GLYPH_a169,		/*  0xde= 222  */
    GLYPH_a170,		/*  0xdf= 223  */
    GLYPH_a171,		/*  0xe0= 224  */
    GLYPH_a172,		/*  0xe1= 225  */
    GLYPH_a173,		/*  0xe2= 226  */
    GLYPH_a162,		/*  0xe3= 227  */
    GLYPH_a174,		/*  0xe4= 228  */
    GLYPH_a175,		/*  0xe5= 229  */
    GLYPH_a176,		/*  0xe6= 230  */
    GLYPH_a177,		/*  0xe7= 231  */
    GLYPH_a178,		/*  0xe8= 232  */
    GLYPH_a179,		/*  0xe9= 233  */
    GLYPH_a193,		/*  0xea= 234  */
    GLYPH_a180,		/*  0xeb= 235  */
    GLYPH_a199,		/*  0xec= 236  */
    GLYPH_a181,		/*  0xed= 237  */
    GLYPH_a200,		/*  0xee= 238  */
    GLYPH_a182,		/*  0xef= 239  */
    (char *)0,		/*  0xf0= 240  */
    GLYPH_a201,		/*  0xf1= 241  */
    GLYPH_a183,		/*  0xf2= 242  */
    GLYPH_a184,		/*  0xf3= 243  */
    GLYPH_a197,		/*  0xf4= 244  */
    GLYPH_a185,		/*  0xf5= 245  */
    GLYPH_a194,		/*  0xf6= 246  */
    GLYPH_a198,		/*  0xf7= 247  */
    GLYPH_a186,		/*  0xf8= 248  */
    GLYPH_a195,		/*  0xf9= 249  */
    GLYPH_a187,		/*  0xfa= 250  */
    GLYPH_a188,		/*  0xfb= 251  */
    GLYPH_a189,		/*  0xfc= 252  */
    GLYPH_a190,		/*  0xfd= 253  */
    GLYPH_a191,		/*  0xfe= 254  */
    (char *)0,		/*  0xff= 255  */
    };

/************************************************************************/
/*									*/
/*  Alternate glyphs or glyph names as I have encontered them in	*/
/*  actual afm files.							*/
/*									*/
/************************************************************************/

static const char GLYPH_Cdot[]= "Cdot";
static const char GLYPH_Dslash[]= "Dslash";
static const char GLYPH_increment[]= "increment";
static const char GLYPH_Edot[]= "Edot";
static const char GLYPH_euro[]= "euro";
static const char GLYPH_Gcedilla[]= "Gcedilla";
static const char GLYPH_Gdot[]= "Gdot";
static const char GLYPH_blackcircle[]= "blackcircle";
static const char GLYPH_blacksmallsquare[]= "blacksmallsquare";
static const char GLYPH_whitesmallsquare[]= "whitesmallsquare";
static const char GLYPH_whitesquare[]= "whitesquare";
static const char GLYPH_Idot[]= "Idot";
static const char GLYPH_Kcedilla[]= "Kcedilla";
static const char GLYPH_Lcedilla[]= "Lcedilla";
static const char GLYPH_Ldotaccent[]= "Ldotaccent";
static const char GLYPH_Ncedilla[]= "Ncedilla";
static const char GLYPH_Odblacute[]= "Odblacute";
static const char GLYPH_Ohm[]= "Ohm";
static const char GLYPH_Ostrokeacute[]= "Ostrokeacute";
static const char GLYPH_Rcedilla[]= "Rcedilla";
static const char GLYPH_Tcedilla[]= "Tcedilla";
static const char GLYPH_Udblacute[]= "Udblacute";
static const char GLYPH_Upsilonhooksymbol[]= "Upsilonhooksymbol";
static const char GLYPH_Zdot[]= "Zdot";
static const char GLYPH_acutecmb[]= "acutecmb";
static const char GLYPH_horizontalbar[]= "horizontalbar";
static const char GLYPH_Acyrillic[]= "Acyrillic";
static const char GLYPH_Becyrillic[]= "Becyrillic";
static const char GLYPH_Vecyrillic[]= "Vecyrillic";
static const char GLYPH_Gecyrillic[]= "Gecyrillic";
static const char GLYPH_Decyrillic[]= "Decyrillic";
static const char GLYPH_Iecyrillic[]= "Iecyrillic";
static const char GLYPH_Iocyrillic[]= "Iocyrillic";
static const char GLYPH_Zhecyrillic[]= "Zhecyrillic";
static const char GLYPH_Zecyrillic[]= "Zecyrillic";
static const char GLYPH_Iicyrillic[]= "Iicyrillic";
static const char GLYPH_Iishortcyrillic[]= "Iishortcyrillic";
static const char GLYPH_Kacyrillic[]= "Kacyrillic";
static const char GLYPH_Elcyrillic[]= "Elcyrillic";
static const char GLYPH_Emcyrillic[]= "Emcyrillic";
static const char GLYPH_Encyrillic[]= "Encyrillic";
static const char GLYPH_Ocyrillic[]= "Ocyrillic";
static const char GLYPH_Pecyrillic[]= "Pecyrillic";
static const char GLYPH_Ercyrillic[]= "Ercyrillic";
static const char GLYPH_Escyrillic[]= "Escyrillic";
static const char GLYPH_Tecyrillic[]= "Tecyrillic";
static const char GLYPH_Ucyrillic[]= "Ucyrillic";
static const char GLYPH_Efcyrillic[]= "Efcyrillic";
static const char GLYPH_Khacyrillic[]= "Khacyrillic";
static const char GLYPH_Tsecyrillic[]= "Tsecyrillic";
static const char GLYPH_Checyrillic[]= "Checyrillic";
static const char GLYPH_Shacyrillic[]= "Shacyrillic";
static const char GLYPH_Shchacyrillic[]= "Shchacyrillic";
static const char GLYPH_Hardsigncyrillic[]= "Hardsigncyrillic";
static const char GLYPH_Yericyrillic[]= "Yericyrillic";
static const char GLYPH_Softsigncyrillic[]= "Softsigncyrillic";
static const char GLYPH_Ereversedcyrillic[]= "Ereversedcyrillic";
static const char GLYPH_IUcyrillic[]= "IUcyrillic";
static const char GLYPH_IAcyrillic[]= "IAcyrillic";
static const char GLYPH_Gheupturncyrillic[]= "Gheupturncyrillic";
static const char GLYPH_Djecyrillic[]= "Djecyrillic";
static const char GLYPH_Gjecyrillic[]= "Gjecyrillic";
static const char GLYPH_Ecyrillic[]= "Ecyrillic";
static const char GLYPH_Dzecyrillic[]= "Dzecyrillic";
static const char GLYPH_Icyrillic[]= "Icyrillic";
static const char GLYPH_Yicyrillic[]= "Yicyrillic";
static const char GLYPH_Jecyrillic[]= "Jecyrillic";
static const char GLYPH_Ljecyrillic[]= "Ljecyrillic";
static const char GLYPH_Njecyrillic[]= "Njecyrillic";
static const char GLYPH_Tshecyrillic[]= "Tshecyrillic";
static const char GLYPH_Kjecyrillic[]= "Kjecyrillic";
static const char GLYPH_Ushortcyrillic[]= "Ushortcyrillic";
static const char GLYPH_acyrillic[]= "acyrillic";
static const char GLYPH_becyrillic[]= "becyrillic";
static const char GLYPH_vecyrillic[]= "vecyrillic";
static const char GLYPH_gecyrillic[]= "gecyrillic";
static const char GLYPH_decyrillic[]= "decyrillic";
static const char GLYPH_iecyrillic[]= "iecyrillic";
static const char GLYPH_iocyrillic[]= "iocyrillic";
static const char GLYPH_zhecyrillic[]= "zhecyrillic";
static const char GLYPH_zecyrillic[]= "zecyrillic";
static const char GLYPH_iicyrillic[]= "iicyrillic";
static const char GLYPH_iishortcyrillic[]= "iishortcyrillic";
static const char GLYPH_kacyrillic[]= "kacyrillic";
static const char GLYPH_elcyrillic[]= "elcyrillic";
static const char GLYPH_emcyrillic[]= "emcyrillic";
static const char GLYPH_encyrillic[]= "encyrillic";
static const char GLYPH_ocyrillic[]= "ocyrillic";
static const char GLYPH_pecyrillic[]= "pecyrillic";
static const char GLYPH_ercyrillic[]= "ercyrillic";
static const char GLYPH_escyrillic[]= "escyrillic";
static const char GLYPH_tecyrillic[]= "tecyrillic";
static const char GLYPH_ucyrillic[]= "ucyrillic";
static const char GLYPH_efcyrillic[]= "efcyrillic";
static const char GLYPH_khacyrillic[]= "khacyrillic";
static const char GLYPH_tsecyrillic[]= "tsecyrillic";
static const char GLYPH_checyrillic[]= "checyrillic";
static const char GLYPH_shacyrillic[]= "shacyrillic";
static const char GLYPH_shchacyrillic[]= "shchacyrillic";
static const char GLYPH_hardsigncyrillic[]= "hardsigncyrillic";
static const char GLYPH_yericyrillic[]= "yericyrillic";
static const char GLYPH_softsigncyrillic[]= "softsigncyrillic";
static const char GLYPH_ereversedcyrillic[]= "ereversedcyrillic";
static const char GLYPH_iucyrillic[]= "iucyrillic";
static const char GLYPH_iacyrillic[]= "iacyrillic";
static const char GLYPH_gheupturncyrillic[]= "gheupturncyrillic";
static const char GLYPH_djecyrillic[]= "djecyrillic";
static const char GLYPH_gjecyrillic[]= "gjecyrillic";
static const char GLYPH_ecyrillic[]= "ecyrillic";
static const char GLYPH_dzecyrillic[]= "dzecyrillic";
static const char GLYPH_icyrillic[]= "icyrillic";
static const char GLYPH_yicyrillic[]= "yicyrillic";
static const char GLYPH_jecyrillic[]= "jecyrillic";
static const char GLYPH_ljecyrillic[]= "ljecyrillic";
static const char GLYPH_njecyrillic[]= "njecyrillic";
static const char GLYPH_tshecyrillic[]= "tshecyrillic";
static const char GLYPH_kjecyrillic[]= "kjecyrillic";
static const char GLYPH_ushortcyrillic[]= "ushortcyrillic";
static const char GLYPH_Dzhecyrillic[]= "Dzhecyrillic";
static const char GLYPH_Yatcyrillic[]= "Yatcyrillic";
static const char GLYPH_Fitacyrillic[]= "Fitacyrillic";
static const char GLYPH_Izhitsacyrillic[]= "Izhitsacyrillic";
static const char GLYPH_dzhecyrillic[]= "dzhecyrillic";
static const char GLYPH_yatcyrillic[]= "yatcyrillic";
static const char GLYPH_fitacyrillic[]= "fitacyrillic";
static const char GLYPH_izhitsacyrillic[]= "izhitsacyrillic";
static const char GLYPH_schwacyrillic[]= "schwacyrillic";
static const char GLYPH_percentarabic[]= "percentarabic";
static const char GLYPH_commaarabic[]= "commaarabic";
static const char GLYPH_zeroarabic[]= "zeroarabic";
static const char GLYPH_zerohackarabic[]= "zerohackarabic";
static const char GLYPH_onearabic[]= "onearabic";
static const char GLYPH_onehackarabic[]= "onehackarabic";
static const char GLYPH_twoarabic[]= "twoarabic";
static const char GLYPH_twohackarabic[]= "twohackarabic";
static const char GLYPH_threearabic[]= "threearabic";
static const char GLYPH_threehackarabic[]= "threehackarabic";
static const char GLYPH_fourarabic[]= "fourarabic";
static const char GLYPH_fourhackarabic[]= "fourhackarabic";
static const char GLYPH_fivearabic[]= "fivearabic";
static const char GLYPH_fivehackarabic[]= "fivehackarabic";
static const char GLYPH_sixarabic[]= "sixarabic";
static const char GLYPH_sixhackarabic[]= "sixhackarabic";
static const char GLYPH_sevenarabic[]= "sevenarabic";
static const char GLYPH_sevenhackarabic[]= "sevenhackarabic";
static const char GLYPH_eightarabic[]= "eightarabic";
static const char GLYPH_eighthackarabic[]= "eighthackarabic";
static const char GLYPH_ninearabic[]= "ninearabic";
static const char GLYPH_ninehackarabic[]= "ninehackarabic";
static const char GLYPH_semicolonarabic[]= "semicolonarabic";
static const char GLYPH_questionarabic[]= "questionarabic";
static const char GLYPH_hamzaarabic[]= "hamzaarabic";
static const char GLYPH_hamzalowarabic[]= "hamzalowarabic";
static const char GLYPH_alefmaddaabovearabic[]= "alefmaddaabovearabic";
static const char GLYPH_alefhamzaabovearabic[]= "alefhamzaabovearabic";
static const char GLYPH_wawhamzaabovearabic[]= "wawhamzaabovearabic";
static const char GLYPH_alefhamzabelowarabic[]= "alefhamzabelowarabic";
static const char GLYPH_yehhamzaabovearabic[]= "yehhamzaabovearabic";
static const char GLYPH_alefarabic[]= "alefarabic";
static const char GLYPH_beharabic[]= "beharabic";
static const char GLYPH_tehmarbutaarabic[]= "tehmarbutaarabic";
static const char GLYPH_teharabic[]= "teharabic";
static const char GLYPH_theharabic[]= "theharabic";
static const char GLYPH_jeemarabic[]= "jeemarabic";
static const char GLYPH_haharabic[]= "haharabic";
static const char GLYPH_khaharabic[]= "khaharabic";
static const char GLYPH_dalarabic[]= "dalarabic";
static const char GLYPH_thalarabic[]= "thalarabic";
static const char GLYPH_reharabic[]= "reharabic";
static const char GLYPH_zainarabic[]= "zainarabic";
static const char GLYPH_seenarabic[]= "seenarabic";
static const char GLYPH_sheenarabic[]= "sheenarabic";
static const char GLYPH_sadarabic[]= "sadarabic";
static const char GLYPH_dadarabic[]= "dadarabic";
static const char GLYPH_taharabic[]= "taharabic";
static const char GLYPH_zaharabic[]= "zaharabic";
static const char GLYPH_ainarabic[]= "ainarabic";
static const char GLYPH_ghainarabic[]= "ghainarabic";
static const char GLYPH_kashidaautoarabic[]= "kashidaautoarabic";
static const char GLYPH_kashidaautonosidebearingarabic[]= "kashidaautonosidebearingarabic";
static const char GLYPH_tatweelarabic[]= "tatweelarabic";
static const char GLYPH_feharabic[]= "feharabic";
static const char GLYPH_qafarabic[]= "qafarabic";
static const char GLYPH_kafarabic[]= "kafarabic";
static const char GLYPH_lamarabic[]= "lamarabic";
static const char GLYPH_meemarabic[]= "meemarabic";
static const char GLYPH_noonarabic[]= "noonarabic";
static const char GLYPH_wawarabic[]= "wawarabic";
static const char GLYPH_alefmaksuraarabic[]= "alefmaksuraarabic";
static const char GLYPH_yeharabic[]= "yeharabic";
static const char GLYPH_fathatanarabic[]= "fathatanarabic";
static const char GLYPH_dammatanaltonearabic[]= "dammatanaltonearabic";
static const char GLYPH_dammatanarabic[]= "dammatanarabic";
static const char GLYPH_kasratanarabic[]= "kasratanarabic";
static const char GLYPH_fathaarabic[]= "fathaarabic";
static const char GLYPH_fathalowarabic[]= "fathalowarabic";
static const char GLYPH_dammaarabic[]= "dammaarabic";
static const char GLYPH_dammalowarabic[]= "dammalowarabic";
static const char GLYPH_kasraarabic[]= "kasraarabic";
static const char GLYPH_shaddaarabic[]= "shaddaarabic";
static const char GLYPH_sukunarabic[]= "sukunarabic";
static const char GLYPH_heharabic[]= "heharabic";
static const char GLYPH_veharabic[]= "veharabic";
static const char GLYPH_peharabic[]= "peharabic";
static const char GLYPH_tcheharabic[]= "tcheharabic";
static const char GLYPH_jeharabic[]= "jeharabic";
static const char GLYPH_gafarabic[]= "gafarabic";
static const char GLYPH_tteharabic[]= "tteharabic";
static const char GLYPH_ddalarabic[]= "ddalarabic";
static const char GLYPH_rreharabic[]= "rreharabic";
static const char GLYPH_noonghunnaarabic[]= "noonghunnaarabic";
static const char GLYPH_yehbarreearabic[]= "yehbarreearabic";
static const char GLYPH_newsheqelsign[]= "newsheqelsign";
static const char GLYPH_sheqel[]= "sheqel";
static const char GLYPH_sheqelhebrew[]= "sheqelhebrew";
static const char GLYPH_maqafhebrew[]= "maqafhebrew";
static const char GLYPH_sofpasuqhebrew[]= "sofpasuqhebrew";
static const char GLYPH_alef[]= "alef";
static const char GLYPH_alefhebrew[]= "alefhebrew";
static const char GLYPH_bet[]= "bet";
static const char GLYPH_bethebrew[]= "bethebrew";
static const char GLYPH_gimel[]= "gimel";
static const char GLYPH_gimelhebrew[]= "gimelhebrew";
static const char GLYPH_dalet[]= "dalet";
static const char GLYPH_dalethebrew[]= "dalethebrew";
static const char GLYPH_he[]= "he";
static const char GLYPH_hehebrew[]= "hehebrew";
static const char GLYPH_vav[]= "vav";
static const char GLYPH_vavhebrew[]= "vavhebrew";
static const char GLYPH_zayin[]= "zayin";
static const char GLYPH_zayinhebrew[]= "zayinhebrew";
static const char GLYPH_het[]= "het";
static const char GLYPH_hethebrew[]= "hethebrew";
static const char GLYPH_tet[]= "tet";
static const char GLYPH_tethebrew[]= "tethebrew";
static const char GLYPH_yod[]= "yod";
static const char GLYPH_yodhebrew[]= "yodhebrew";
static const char GLYPH_finalkaf[]= "finalkaf";
static const char GLYPH_finalkafhebrew[]= "finalkafhebrew";
static const char GLYPH_kaf[]= "kaf";
static const char GLYPH_kafhebrew[]= "kafhebrew";
static const char GLYPH_lamed[]= "lamed";
static const char GLYPH_lamedhebrew[]= "lamedhebrew";
static const char GLYPH_finalmem[]= "finalmem";
static const char GLYPH_finalmemhebrew[]= "finalmemhebrew";
static const char GLYPH_mem[]= "mem";
static const char GLYPH_memhebrew[]= "memhebrew";
static const char GLYPH_finalnun[]= "finalnun";
static const char GLYPH_finalnunhebrew[]= "finalnunhebrew";
static const char GLYPH_nun[]= "nun";
static const char GLYPH_nunhebrew[]= "nunhebrew";
static const char GLYPH_samekh[]= "samekh";
static const char GLYPH_samekhhebrew[]= "samekhhebrew";
static const char GLYPH_ayin[]= "ayin";
static const char GLYPH_ayinhebrew[]= "ayinhebrew";
static const char GLYPH_finalpe[]= "finalpe";
static const char GLYPH_finalpehebrew[]= "finalpehebrew";
static const char GLYPH_pe[]= "pe";
static const char GLYPH_pehebrew[]= "pehebrew";
static const char GLYPH_finaltsadi[]= "finaltsadi";
static const char GLYPH_finaltsadihebrew[]= "finaltsadihebrew";
static const char GLYPH_tsadi[]= "tsadi";
static const char GLYPH_tsadihebrew[]= "tsadihebrew";
static const char GLYPH_qof[]= "qof";
static const char GLYPH_qofhebrew[]= "qofhebrew";
static const char GLYPH_resh[]= "resh";
static const char GLYPH_reshhebrew[]= "reshhebrew";
static const char GLYPH_shin[]= "shin";
static const char GLYPH_shinhebrew[]= "shinhebrew";
static const char GLYPH_tav[]= "tav";
static const char GLYPH_tavhebrew[]= "tavhebrew";
static const char GLYPH_vavvavhebrew[]= "vavvavhebrew";
static const char GLYPH_vavyodhebrew[]= "vavyodhebrew";
static const char GLYPH_yodyodhebrew[]= "yodyodhebrew";
static const char GLYPH_hiriq[]= "hiriq";
static const char GLYPH_hiriq14[]= "hiriq14";
static const char GLYPH_hiriq21[]= "hiriq21";
static const char GLYPH_hiriq2d[]= "hiriq2d";
static const char GLYPH_hiriqhebrew[]= "hiriqhebrew";
static const char GLYPH_hiriqnarrowhebrew[]= "hiriqnarrowhebrew";
static const char GLYPH_hiriqquarterhebrew[]= "hiriqquarterhebrew";
static const char GLYPH_hiriqwidehebrew[]= "hiriqwidehebrew";
static const char GLYPH_tsere[]= "tsere";
static const char GLYPH_tsere12[]= "tsere12";
static const char GLYPH_tsere1e[]= "tsere1e";
static const char GLYPH_tsere2b[]= "tsere2b";
static const char GLYPH_tserehebrew[]= "tserehebrew";
static const char GLYPH_tserenarrowhebrew[]= "tserenarrowhebrew";
static const char GLYPH_tserequarterhebrew[]= "tserequarterhebrew";
static const char GLYPH_tserewidehebrew[]= "tserewidehebrew";
static const char GLYPH_segol[]= "segol";
static const char GLYPH_segol13[]= "segol13";
static const char GLYPH_segol1f[]= "segol1f";
static const char GLYPH_segol2c[]= "segol2c";
static const char GLYPH_segolhebrew[]= "segolhebrew";
static const char GLYPH_segolnarrowhebrew[]= "segolnarrowhebrew";
static const char GLYPH_segolquarterhebrew[]= "segolquarterhebrew";
static const char GLYPH_segolwidehebrew[]= "segolwidehebrew";
static const char GLYPH_qubuts[]= "qubuts";
static const char GLYPH_qubuts18[]= "qubuts18";
static const char GLYPH_qubuts25[]= "qubuts25";
static const char GLYPH_qubuts31[]= "qubuts31";
static const char GLYPH_qubutshebrew[]= "qubutshebrew";
static const char GLYPH_qubutsnarrowhebrew[]= "qubutsnarrowhebrew";
static const char GLYPH_qubutsquarterhebrew[]= "qubutsquarterhebrew";
static const char GLYPH_qubutswidehebrew[]= "qubutswidehebrew";
static const char GLYPH_qamats[]= "qamats";
static const char GLYPH_qamats10[]= "qamats10";
static const char GLYPH_qamats1a[]= "qamats1a";
static const char GLYPH_qamats1c[]= "qamats1c";
static const char GLYPH_qamats27[]= "qamats27";
static const char GLYPH_qamats29[]= "qamats29";
static const char GLYPH_qamats33[]= "qamats33";
static const char GLYPH_qamatsde[]= "qamatsde";
static const char GLYPH_qamatshebrew[]= "qamatshebrew";
static const char GLYPH_qamatsnarrowhebrew[]= "qamatsnarrowhebrew";
static const char GLYPH_qamatsqatanhebrew[]= "qamatsqatanhebrew";
static const char GLYPH_qamatsqatannarrowhebrew[]= "qamatsqatannarrowhebrew";
static const char GLYPH_qamatsqatanquarterhebrew[]= "qamatsqatanquarterhebrew";
static const char GLYPH_qamatsqatanwidehebrew[]= "qamatsqatanwidehebrew";
static const char GLYPH_qamatsquarterhebrew[]= "qamatsquarterhebrew";
static const char GLYPH_qamatswidehebrew[]= "qamatswidehebrew";
static const char GLYPH_patah[]= "patah";
static const char GLYPH_patah11[]= "patah11";
static const char GLYPH_patah1d[]= "patah1d";
static const char GLYPH_patah2a[]= "patah2a";
static const char GLYPH_patahhebrew[]= "patahhebrew";
static const char GLYPH_patahnarrowhebrew[]= "patahnarrowhebrew";
static const char GLYPH_patahquarterhebrew[]= "patahquarterhebrew";
static const char GLYPH_patahwidehebrew[]= "patahwidehebrew";
static const char GLYPH_sheva[]= "sheva";
static const char GLYPH_sheva115[]= "sheva115";
static const char GLYPH_sheva15[]= "sheva15";
static const char GLYPH_sheva22[]= "sheva22";
static const char GLYPH_sheva2e[]= "sheva2e";
static const char GLYPH_shevahebrew[]= "shevahebrew";
static const char GLYPH_shevanarrowhebrew[]= "shevanarrowhebrew";
static const char GLYPH_shevaquarterhebrew[]= "shevaquarterhebrew";
static const char GLYPH_shevawidehebrew[]= "shevawidehebrew";
static const char GLYPH_hatafpatah[]= "hatafpatah";
static const char GLYPH_hatafpatah16[]= "hatafpatah16";
static const char GLYPH_hatafpatah23[]= "hatafpatah23";
static const char GLYPH_hatafpatah2f[]= "hatafpatah2f";
static const char GLYPH_hatafpatahhebrew[]= "hatafpatahhebrew";
static const char GLYPH_hatafpatahnarrowhebrew[]= "hatafpatahnarrowhebrew";
static const char GLYPH_hatafpatahquarterhebrew[]= "hatafpatahquarterhebrew";
static const char GLYPH_hatafpatahwidehebrew[]= "hatafpatahwidehebrew";
static const char GLYPH_hatafsegol[]= "hatafsegol";
static const char GLYPH_hatafsegol17[]= "hatafsegol17";
static const char GLYPH_hatafsegol24[]= "hatafsegol24";
static const char GLYPH_hatafsegol30[]= "hatafsegol30";
static const char GLYPH_hatafsegolhebrew[]= "hatafsegolhebrew";
static const char GLYPH_hatafsegolnarrowhebrew[]= "hatafsegolnarrowhebrew";
static const char GLYPH_hatafsegolquarterhebrew[]= "hatafsegolquarterhebrew";
static const char GLYPH_hatafsegolwidehebrew[]= "hatafsegolwidehebrew";
static const char GLYPH_hatafqamats[]= "hatafqamats";
static const char GLYPH_hatafqamats1b[]= "hatafqamats1b";
static const char GLYPH_hatafqamats28[]= "hatafqamats28";
static const char GLYPH_hatafqamats34[]= "hatafqamats34";
static const char GLYPH_hatafqamatshebrew[]= "hatafqamatshebrew";
static const char GLYPH_hatafqamatsnarrowhebrew[]= "hatafqamatsnarrowhebrew";
static const char GLYPH_hatafqamatsquarterhebrew[]= "hatafqamatsquarterhebrew";
static const char GLYPH_hatafqamatswidehebrew[]= "hatafqamatswidehebrew";
static const char GLYPH_sindothebrew[]= "sindothebrew";
static const char GLYPH_shindothebrew[]= "shindothebrew";
static const char GLYPH_holam[]= "holam";
static const char GLYPH_holam19[]= "holam19";
static const char GLYPH_holam26[]= "holam26";
static const char GLYPH_holam32[]= "holam32";
static const char GLYPH_holamhebrew[]= "holamhebrew";
static const char GLYPH_holamnarrowhebrew[]= "holamnarrowhebrew";
static const char GLYPH_holamquarterhebrew[]= "holamquarterhebrew";
static const char GLYPH_holamwidehebrew[]= "holamwidehebrew";
static const char GLYPH_dagesh[]= "dagesh";
static const char GLYPH_dageshhebrew[]= "dageshhebrew";
static const char GLYPH_siluqhebrew[]= "siluqhebrew";
static const char GLYPH_siluqlefthebrew[]= "siluqlefthebrew";
static const char GLYPH_rafe[]= "rafe";
static const char GLYPH_rafehebrew[]= "rafehebrew";
static const char GLYPH_paseqhebrew[]= "paseqhebrew";
static const char GLYPH_apostrophemod[]= "apostrophemod";
static const char GLYPH_careof[]= "careof";
static const char GLYPH_lsquare[]= "lsquare";
static const char GLYPH_numero[]= "numero";
static const char GLYPH_zerowidthnonjoiner[]= "zerowidthnonjoiner";
static const char GLYPH_asteriskaltonearabic[]= "asteriskaltonearabic";
static const char GLYPH_asteriskarabic[]= "asteriskarabic";
static const char GLYPH_commareversedmod[]= "commareversedmod";
static const char GLYPH_dblarrowleft[]= "dblarrowleft";
static const char GLYPH_arrowleftdbl[]= "arrowleftdbl";
static const char GLYPH_dblarrowright[]= "dblarrowright";
static const char GLYPH_arrowupdownbase[]= "arrowupdownbase";
static const char GLYPH_verticalbar[]= "verticalbar";
static const char GLYPH_cdot[]= "cdot";
static const char GLYPH_whitecircle[]= "whitecircle";
static const char GLYPH_timescircle[]= "timescircle";
static const char GLYPH_pluscircle[]= "pluscircle";
static const char GLYPH_clubsuitblack[]= "clubsuitblack";
static const char GLYPH_colonsign[]= "colonsign";
static const char GLYPH_approximatelyequal[]= "approximatelyequal";
static const char GLYPH_dmacron[]= "dmacron";
static const char GLYPH_dialytikatonos[]= "dialytikatonos";
static const char GLYPH_shadedark[]= "shadedark";
static const char GLYPH_dotbelowcmb[]= "dotbelowcmb";
static const char GLYPH_edot[]= "edot";
static const char GLYPH_thereexists[]= "thereexists";
static const char GLYPH_venus[]= "venus";
static const char GLYPH_blacksquare[]= "blacksquare";
static const char GLYPH_blackrectangle[]= "blackrectangle";
static const char GLYPH_gcedilla[]= "gcedilla";
static const char GLYPH_gdot[]= "gdot";
static const char GLYPH_nabla[]= "nabla";
static const char GLYPH_gravecmb[]= "gravecmb";
static const char GLYPH_heartsuitblack[]= "heartsuitblack";
static const char GLYPH_hookcmb[]= "hookcmb";
static const char GLYPH_integralbottom[]= "integralbottom";
static const char GLYPH_integraltop[]= "integraltop";
static const char GLYPH_bulletinverse[]= "bulletinverse";
static const char GLYPH_whitecircleinverse[]= "whitecircleinverse";
static const char GLYPH_blacksmilingface[]= "blacksmilingface";
static const char GLYPH_kcedilla[]= "kcedilla";
static const char GLYPH_lcedilla[]= "lcedilla";
static const char GLYPH_ldotaccent[]= "ldotaccent";
static const char GLYPH_afii08941[]= "afii08941";
static const char GLYPH_slong[]= "slong";
static const char GLYPH_shadelight[]= "shadelight";
static const char GLYPH_overscore[]= "overscore";
static const char GLYPH_mars[]= "mars";
static const char GLYPH_mu1[]= "mu1";
static const char GLYPH_eighthnotebeamed[]= "eighthnotebeamed";
static const char GLYPH_quoterightn[]= "quoterightn";
static const char GLYPH_ncedilla[]= "ncedilla";
static const char GLYPH_notelementof[]= "notelementof";
static const char GLYPH_odblacute[]= "odblacute";
static const char GLYPH_pisymbolgreek[]= "pisymbolgreek";
static const char GLYPH_whitebullet[]= "whitebullet";
static const char GLYPH_rightangle[]= "rightangle";
static const char GLYPH_ostrokeacute[]= "ostrokeacute";
static const char GLYPH_middot[]= "middot";
static const char GLYPH_phisymbolgreek[]= "phisymbolgreek";
static const char GLYPH_subset[]= "subset";
static const char GLYPH_superset[]= "superset";
static const char GLYPH_quoteleftreversed[]= "quoteleftreversed";
static const char GLYPH_rcedilla[]= "rcedilla";
static const char GLYPH_subsetorequal[]= "subsetorequal";
static const char GLYPH_supersetorequal[]= "supersetorequal";
static const char GLYPH_logicalnotreversed[]= "logicalnotreversed";
static const char GLYPH_shademedium[]= "shademedium";
static const char GLYPH_sigmafinal[]= "sigmafinal";
static const char GLYPH_tildeoperator[]= "tildeoperator";
static const char GLYPH_whitesmilingface[]= "whitesmilingface";
static const char GLYPH_spacehackarabic[]= "spacehackarabic";
static const char GLYPH_spadesuitblack[]= "spadesuitblack";
static const char GLYPH_compass[]= "compass";
static const char GLYPH_tcedilla[]= "tcedilla";
static const char GLYPH_thetasymbolgreek[]= "thetasymbolgreek";
static const char GLYPH_ilde[]= "ilde";
static const char GLYPH_tildecmb[]= "tildecmb";
static const char GLYPH_blackdownpointingtriangle[]= "blackdownpointingtriangle";
static const char GLYPH_blackleftpointingpointer[]= "blackleftpointingpointer";
static const char GLYPH_blackrightpointingpointer[]= "blackrightpointingpointer";
static const char GLYPH_blackuppointingtriangle[]= "blackuppointingtriangle";
static const char GLYPH_twodotleader[]= "twodotleader";
static const char GLYPH_udblacute[]= "udblacute";
static const char GLYPH_dbllowline[]= "dbllowline";
static const char GLYPH_forall[]= "forall";
static const char GLYPH_zdot[]= "zdot";

/* EXTRA */

static const char GLYPH_apostrophe[]= "apostrophe";
static const char GLYPH_fullstop[]= "fullstop";
static const char GLYPH_lowline[]= "lowline";
static const char GLYPH_Lstroke[]= "Lstroke";
static const char GLYPH_diaeresis[]= "diaeresis";
static const char GLYPH_Zabovedot[]= "Zabovedot";
static const char GLYPH_lstroke[]= "lstroke";
static const char GLYPH_doubleacute[]= "doubleacute";
static const char GLYPH_zabovedot[]= "zabovedot";
static const char GLYPH_Adiaeresis[]= "Adiaeresis";
static const char GLYPH_Ediaeresis[]= "Ediaeresis";
static const char GLYPH_Dstroke[]= "Dstroke";
static const char GLYPH_Odoubleacute[]= "Odoubleacute";
static const char GLYPH_Odiaeresis[]= "Odiaeresis";
static const char GLYPH_Udoubleacute[]= "Udoubleacute";
static const char GLYPH_Udiaeresis[]= "Udiaeresis";
static const char GLYPH_ssharp[]= "ssharp";
static const char GLYPH_adiaeresis[]= "adiaeresis";
static const char GLYPH_ediaeresis[]= "ediaeresis";
static const char GLYPH_dstroke[]= "dstroke";
static const char GLYPH_odoubleacute[]= "odoubleacute";
static const char GLYPH_odiaeresis[]= "odiaeresis";
static const char GLYPH_division[]= "division";
static const char GLYPH_udoubleacute[]= "udoubleacute";
static const char GLYPH_udiaeresis[]= "udiaeresis";
static const char GLYPH_abovedot[]= "abovedot";
static const char GLYPH_Scedilla1[]= "Scedilla1";
static const char GLYPH_scedilla1[]= "scedilla1";
static const char GLYPH_Tcedilla1[]= "Tcedilla1";
static const char GLYPH_tcedilla1[]= "tcedilla1";
static const char GLYPH_Dmacron[]= "Dmacron";
static const char GLYPH_Ohungar[]= "Ohungar";
static const char GLYPH_Uhungar[]= "Uhungar";
static const char GLYPH_ohungar[]= "ohungar";
static const char GLYPH_uhungar[]= "uhungar";
static const char GLYPH_sfthyphen[]= "sfthyphen";
static const char GLYPH_commaaccent[]= "commaaccent";
static const char GLYPH_nonbreakingspace[]= "nonbreakingspace";

/*  AFII's for greek */
static const char GLYPH_afii9936[]= "afii9936";
static const char GLYPH_afii9937[]= "afii9937";
static const char GLYPH_afii9938[]= "afii9938";
static const char GLYPH_afii9939[]= "afii9939";
static const char GLYPH_afii9940[]= "afii9940";
static const char GLYPH_afii9941[]= "afii9941";
static const char GLYPH_afii9942[]= "afii9942";
static const char GLYPH_afii9977[]= "afii9977";
static const char GLYPH_afii9793[]= "afii9793";
static const char GLYPH_afii9794[]= "afii9794";
static const char GLYPH_afii9796[]= "afii9796";
static const char GLYPH_afii9797[]= "afii9797";
static const char GLYPH_afii9798[]= "afii9798";
static const char GLYPH_afii9801[]= "afii9801";
static const char GLYPH_afii9802[]= "afii9802";
static const char GLYPH_afii9803[]= "afii9803";
static const char GLYPH_afii9804[]= "afii9804";
static const char GLYPH_afii9805[]= "afii9805";
static const char GLYPH_afii9806[]= "afii9806";
static const char GLYPH_afii9807[]= "afii9807";
static const char GLYPH_afii9808[]= "afii9808";
static const char GLYPH_afii9809[]= "afii9809";
static const char GLYPH_afii9810[]= "afii9810";
static const char GLYPH_afii9811[]= "afii9811";
static const char GLYPH_afii9813[]= "afii9813";
static const char GLYPH_afii9814[]= "afii9814";
static const char GLYPH_afii9816[]= "afii9816";
static const char GLYPH_afii9817[]= "afii9817";
static const char GLYPH_afii9818[]= "afii9818";
static const char GLYPH_afii9819[]= "afii9819";
static const char GLYPH_afii9820[]= "afii9820";
static const char GLYPH_afii9821[]= "afii9821";
static const char GLYPH_afii9943[]= "afii9943";
static const char GLYPH_afii9944[]= "afii9944";
static const char GLYPH_afii9968[]= "afii9968";
static const char GLYPH_afii9969[]= "afii9969";
static const char GLYPH_afii9970[]= "afii9970";
static const char GLYPH_afii9971[]= "afii9971";
static const char GLYPH_afii9978[]= "afii9978";
static const char GLYPH_afii9825[]= "afii9825";
static const char GLYPH_afii9826[]= "afii9826";
static const char GLYPH_afii9828[]= "afii9828";
static const char GLYPH_afii9829[]= "afii9829";
static const char GLYPH_afii9830[]= "afii9830";
static const char GLYPH_afii9833[]= "afii9833";
static const char GLYPH_afii9834[]= "afii9834";
static const char GLYPH_afii9835[]= "afii9835";
static const char GLYPH_afii9836[]= "afii9836";
static const char GLYPH_afii9837[]= "afii9837";
static const char GLYPH_afii9838[]= "afii9838";
static const char GLYPH_afii9839[]= "afii9839";
static const char GLYPH_afii9840[]= "afii9840";
static const char GLYPH_afii9841[]= "afii9841";
static const char GLYPH_afii9842[]= "afii9842";
static const char GLYPH_afii9843[]= "afii9843";
static const char GLYPH_afii9845[]= "afii9845";
static const char GLYPH_afii9847[]= "afii9847";
static const char GLYPH_afii9846[]= "afii9846";
static const char GLYPH_afii9848[]= "afii9848";
static const char GLYPH_afii9849[]= "afii9849";
static const char GLYPH_afii9850[]= "afii9850";
static const char GLYPH_afii9851[]= "afii9851";
static const char GLYPH_afii9852[]= "afii9852";
static const char GLYPH_afii9853[]= "afii9853";
static const char GLYPH_afii9975[]= "afii9975";
static const char GLYPH_afii9976[]= "afii9976";
static const char GLYPH_afii9972[]= "afii9972";
static const char GLYPH_afii9973[]= "afii9973";
static const char GLYPH_afii9974[]= "afii9974";

static const char GLYPH_dieresisacute[]= "dieresisacute";

static const char GLYPH_uni037e[]= "uni037e";

/************************************************************************/
/*									*/
/*  Glyph names that can be surrogates for the official names.		*/
/*  It is irrelevant whether it is an acceptable substitution or an	*/
/*  alternative name for the same glyph.				*/
/*									*/
/*  Given the low number of alternatives per official glyph name, it	*/
/*  is not worth the effort to optimize the data structure. The simple	*/
/*  binary search will make less than one additional string comparison	*/
/*  on the average and the loop over the alternatives for one official	*/
/*  name makes only a few extra comparisons. Those could even be	*/
/*  replaced with pointer comparisons to get the same speed.		*/
/*									*/
/************************************************************************/

# if 0

:
: script to sort:
:

sed	-e 's/^[ 	]*{[ 	]*GLYPH_//'	\
	-e 's/,[ 	]*GLYPH_/ /'		\
	-e 's/[ 	]}.*$//'		\
	alts					|
	sort +0 +1 |
	while read official surrogate
	do
	    printf '	{ GLYPH_%s, GLYPH_%s },\n' $official $surrogate
	done

# endif

typedef struct AlternateGlyphName
    {
    const char *	agnOfficialName;
    const char *	agnSurrogateName;
    } AlternateGlyphName;

const AlternateGlyphName PS_AlternateNames[]=
    {
	{ GLYPH_Adieresis, GLYPH_Adiaeresis },
	{ GLYPH_Alpha, GLYPH_afii9793 },
	{ GLYPH_Alphatonos, GLYPH_afii9936 },
	{ GLYPH_Beta, GLYPH_afii9794 },
	{ GLYPH_Cdotaccent, GLYPH_Cdot },
	{ GLYPH_Chi, GLYPH_afii9819 },
	{ GLYPH_Dcroat, GLYPH_Dmacron },
	{ GLYPH_Dcroat, GLYPH_Dslash },
	{ GLYPH_Dcroat, GLYPH_Dstroke },
	{ GLYPH_Dcroat, GLYPH_Eth },
	{ GLYPH_Delta, GLYPH_afii9797 },
	{ GLYPH_Delta, GLYPH_increment },
	{ GLYPH_Edieresis, GLYPH_Ediaeresis },
	{ GLYPH_Edotaccent, GLYPH_Edot },
	{ GLYPH_Epsilon, GLYPH_afii9798 },
	{ GLYPH_Epsilontonos, GLYPH_afii9937 },
	{ GLYPH_Eta, GLYPH_afii9802 },
	{ GLYPH_Etatonos, GLYPH_afii9938 },
	{ GLYPH_Eth, GLYPH_Dcroat },
	{ GLYPH_Euro, GLYPH_euro },
	{ GLYPH_Gamma, GLYPH_afii9796 },
	{ GLYPH_Gcommaaccent, GLYPH_Gcedilla },
	{ GLYPH_Gdotaccent, GLYPH_Gdot },
	{ GLYPH_H18533, GLYPH_blackcircle },
	{ GLYPH_H18543, GLYPH_blacksmallsquare },
	{ GLYPH_H18551, GLYPH_whitesmallsquare },
	{ GLYPH_H22073, GLYPH_whitesquare },
	{ GLYPH_Idotaccent, GLYPH_Idot },
	{ GLYPH_Iota, GLYPH_afii9804 },
	{ GLYPH_Iotadieresis, GLYPH_afii9943 },
	{ GLYPH_Iotatonos, GLYPH_afii9939 },
	{ GLYPH_Kappa, GLYPH_afii9805 },
	{ GLYPH_Kcommaaccent, GLYPH_Kcedilla },
	{ GLYPH_Lambda, GLYPH_afii9806 },
	{ GLYPH_Lcommaaccent, GLYPH_Lcedilla },
	{ GLYPH_Ldot, GLYPH_Ldotaccent },
	{ GLYPH_Lslash, GLYPH_Lstroke },
	{ GLYPH_Mu, GLYPH_afii9807 },
	{ GLYPH_Ncommaaccent, GLYPH_Ncedilla },
	{ GLYPH_Nu, GLYPH_afii9808 },
	{ GLYPH_Odieresis, GLYPH_Odiaeresis },
	{ GLYPH_Ohungarumlaut, GLYPH_Odblacute },
	{ GLYPH_Ohungarumlaut, GLYPH_Odoubleacute },
	{ GLYPH_Ohungarumlaut, GLYPH_Ohungar },
	{ GLYPH_Omega, GLYPH_Ohm },
	{ GLYPH_Omega, GLYPH_afii9821 },
	{ GLYPH_Omegatonos, GLYPH_afii9942 },
	{ GLYPH_Omicron, GLYPH_afii9810 },
	{ GLYPH_Omicrontonos, GLYPH_afii9940 },
	{ GLYPH_Oslashacute, GLYPH_Ostrokeacute },
	{ GLYPH_Phi, GLYPH_afii9818 },
	{ GLYPH_Pi, GLYPH_afii9811 },
	{ GLYPH_Psi, GLYPH_afii9820 },
	{ GLYPH_Rcommaaccent, GLYPH_Rcedilla },
	{ GLYPH_Rho, GLYPH_afii9813 },
	{ GLYPH_Scedilla, GLYPH_Scedilla1 },
	{ GLYPH_Scedilla, GLYPH_Scommaaccent },
	{ GLYPH_Sigma, GLYPH_afii9814 },
	{ GLYPH_Tau, GLYPH_afii9816 },
	{ GLYPH_Tcommaaccent, GLYPH_Tcedilla },
	{ GLYPH_Tcommaaccent, GLYPH_Tcedilla1 },
	{ GLYPH_Theta, GLYPH_afii9803 },
	{ GLYPH_Udieresis, GLYPH_Udiaeresis },
	{ GLYPH_Uhungarumlaut, GLYPH_Udblacute },
	{ GLYPH_Uhungarumlaut, GLYPH_Udoubleacute },
	{ GLYPH_Uhungarumlaut, GLYPH_Uhungar },
	{ GLYPH_Upsilon, GLYPH_afii9817 },
	{ GLYPH_Upsilon1, GLYPH_Upsilonhooksymbol },
	{ GLYPH_Upsilondieresis, GLYPH_afii9944 },
	{ GLYPH_Upsilontonos, GLYPH_afii9941 },
	{ GLYPH_Xi, GLYPH_afii9809 },
	{ GLYPH_Zdotaccent, GLYPH_Zabovedot },
	{ GLYPH_Zdotaccent, GLYPH_Zdot },
	{ GLYPH_Zeta, GLYPH_afii9801 },
	{ GLYPH_acutecomb, GLYPH_acutecmb },
	{ GLYPH_adieresis, GLYPH_adiaeresis },
	{ GLYPH_afii00208, GLYPH_endash },
	{ GLYPH_afii00208, GLYPH_horizontalbar },
	{ GLYPH_afii10017, GLYPH_Acyrillic },
	{ GLYPH_afii10018, GLYPH_Becyrillic },
	{ GLYPH_afii10019, GLYPH_Vecyrillic },
	{ GLYPH_afii10020, GLYPH_Gecyrillic },
	{ GLYPH_afii10021, GLYPH_Decyrillic },
	{ GLYPH_afii10022, GLYPH_Iecyrillic },
	{ GLYPH_afii10023, GLYPH_Iocyrillic },
	{ GLYPH_afii10024, GLYPH_Zhecyrillic },
	{ GLYPH_afii10025, GLYPH_Zecyrillic },
	{ GLYPH_afii10026, GLYPH_Iicyrillic },
	{ GLYPH_afii10027, GLYPH_Iishortcyrillic },
	{ GLYPH_afii10028, GLYPH_Kacyrillic },
	{ GLYPH_afii10029, GLYPH_Elcyrillic },
	{ GLYPH_afii10030, GLYPH_Emcyrillic },
	{ GLYPH_afii10031, GLYPH_Encyrillic },
	{ GLYPH_afii10032, GLYPH_Ocyrillic },
	{ GLYPH_afii10033, GLYPH_Pecyrillic },
	{ GLYPH_afii10034, GLYPH_Ercyrillic },
	{ GLYPH_afii10035, GLYPH_Escyrillic },
	{ GLYPH_afii10036, GLYPH_Tecyrillic },
	{ GLYPH_afii10037, GLYPH_Ucyrillic },
	{ GLYPH_afii10038, GLYPH_Efcyrillic },
	{ GLYPH_afii10039, GLYPH_Khacyrillic },
	{ GLYPH_afii10040, GLYPH_Tsecyrillic },
	{ GLYPH_afii10041, GLYPH_Checyrillic },
	{ GLYPH_afii10042, GLYPH_Shacyrillic },
	{ GLYPH_afii10043, GLYPH_Shchacyrillic },
	{ GLYPH_afii10044, GLYPH_Hardsigncyrillic },
	{ GLYPH_afii10045, GLYPH_Yericyrillic },
	{ GLYPH_afii10046, GLYPH_Softsigncyrillic },
	{ GLYPH_afii10047, GLYPH_Ereversedcyrillic },
	{ GLYPH_afii10048, GLYPH_IUcyrillic },
	{ GLYPH_afii10049, GLYPH_IAcyrillic },
	{ GLYPH_afii10050, GLYPH_Gheupturncyrillic },
	{ GLYPH_afii10051, GLYPH_Djecyrillic },
	{ GLYPH_afii10052, GLYPH_Gjecyrillic },
	{ GLYPH_afii10053, GLYPH_Ecyrillic },
	{ GLYPH_afii10054, GLYPH_Dzecyrillic },
	{ GLYPH_afii10055, GLYPH_Icyrillic },
	{ GLYPH_afii10056, GLYPH_Yicyrillic },
	{ GLYPH_afii10057, GLYPH_Jecyrillic },
	{ GLYPH_afii10058, GLYPH_Ljecyrillic },
	{ GLYPH_afii10059, GLYPH_Njecyrillic },
	{ GLYPH_afii10060, GLYPH_Tshecyrillic },
	{ GLYPH_afii10061, GLYPH_Kjecyrillic },
	{ GLYPH_afii10062, GLYPH_Ushortcyrillic },
	{ GLYPH_afii10065, GLYPH_acyrillic },
	{ GLYPH_afii10066, GLYPH_becyrillic },
	{ GLYPH_afii10067, GLYPH_vecyrillic },
	{ GLYPH_afii10068, GLYPH_gecyrillic },
	{ GLYPH_afii10069, GLYPH_decyrillic },
	{ GLYPH_afii10070, GLYPH_iecyrillic },
	{ GLYPH_afii10071, GLYPH_iocyrillic },
	{ GLYPH_afii10072, GLYPH_zhecyrillic },
	{ GLYPH_afii10073, GLYPH_zecyrillic },
	{ GLYPH_afii10074, GLYPH_iicyrillic },
	{ GLYPH_afii10075, GLYPH_iishortcyrillic },
	{ GLYPH_afii10076, GLYPH_kacyrillic },
	{ GLYPH_afii10077, GLYPH_elcyrillic },
	{ GLYPH_afii10078, GLYPH_emcyrillic },
	{ GLYPH_afii10079, GLYPH_encyrillic },
	{ GLYPH_afii10080, GLYPH_ocyrillic },
	{ GLYPH_afii10081, GLYPH_pecyrillic },
	{ GLYPH_afii10082, GLYPH_ercyrillic },
	{ GLYPH_afii10083, GLYPH_escyrillic },
	{ GLYPH_afii10084, GLYPH_tecyrillic },
	{ GLYPH_afii10085, GLYPH_ucyrillic },
	{ GLYPH_afii10086, GLYPH_efcyrillic },
	{ GLYPH_afii10087, GLYPH_khacyrillic },
	{ GLYPH_afii10088, GLYPH_tsecyrillic },
	{ GLYPH_afii10089, GLYPH_checyrillic },
	{ GLYPH_afii10090, GLYPH_shacyrillic },
	{ GLYPH_afii10091, GLYPH_shchacyrillic },
	{ GLYPH_afii10092, GLYPH_hardsigncyrillic },
	{ GLYPH_afii10093, GLYPH_yericyrillic },
	{ GLYPH_afii10094, GLYPH_softsigncyrillic },
	{ GLYPH_afii10095, GLYPH_ereversedcyrillic },
	{ GLYPH_afii10096, GLYPH_iucyrillic },
	{ GLYPH_afii10097, GLYPH_iacyrillic },
	{ GLYPH_afii10098, GLYPH_gheupturncyrillic },
	{ GLYPH_afii10099, GLYPH_djecyrillic },
	{ GLYPH_afii10100, GLYPH_gjecyrillic },
	{ GLYPH_afii10101, GLYPH_ecyrillic },
	{ GLYPH_afii10102, GLYPH_dzecyrillic },
	{ GLYPH_afii10103, GLYPH_icyrillic },
	{ GLYPH_afii10104, GLYPH_yicyrillic },
	{ GLYPH_afii10105, GLYPH_jecyrillic },
	{ GLYPH_afii10106, GLYPH_ljecyrillic },
	{ GLYPH_afii10107, GLYPH_njecyrillic },
	{ GLYPH_afii10108, GLYPH_tshecyrillic },
	{ GLYPH_afii10109, GLYPH_kjecyrillic },
	{ GLYPH_afii10110, GLYPH_ushortcyrillic },
	{ GLYPH_afii10145, GLYPH_Dzhecyrillic },
	{ GLYPH_afii10146, GLYPH_Yatcyrillic },
	{ GLYPH_afii10147, GLYPH_Fitacyrillic },
	{ GLYPH_afii10148, GLYPH_Izhitsacyrillic },
	{ GLYPH_afii10193, GLYPH_dzhecyrillic },
	{ GLYPH_afii10194, GLYPH_yatcyrillic },
	{ GLYPH_afii10195, GLYPH_fitacyrillic },
	{ GLYPH_afii10196, GLYPH_izhitsacyrillic },
	{ GLYPH_afii10846, GLYPH_schwacyrillic },
	{ GLYPH_afii57381, GLYPH_percentarabic },
	{ GLYPH_afii57388, GLYPH_commaarabic },
	{ GLYPH_afii57392, GLYPH_zeroarabic },
	{ GLYPH_afii57392, GLYPH_zerohackarabic },
	{ GLYPH_afii57393, GLYPH_onearabic },
	{ GLYPH_afii57393, GLYPH_onehackarabic },
	{ GLYPH_afii57394, GLYPH_twoarabic },
	{ GLYPH_afii57394, GLYPH_twohackarabic },
	{ GLYPH_afii57395, GLYPH_threearabic },
	{ GLYPH_afii57395, GLYPH_threehackarabic },
	{ GLYPH_afii57396, GLYPH_fourarabic },
	{ GLYPH_afii57396, GLYPH_fourhackarabic },
	{ GLYPH_afii57397, GLYPH_fivearabic },
	{ GLYPH_afii57397, GLYPH_fivehackarabic },
	{ GLYPH_afii57398, GLYPH_sixarabic },
	{ GLYPH_afii57398, GLYPH_sixhackarabic },
	{ GLYPH_afii57399, GLYPH_sevenarabic },
	{ GLYPH_afii57399, GLYPH_sevenhackarabic },
	{ GLYPH_afii57400, GLYPH_eightarabic },
	{ GLYPH_afii57400, GLYPH_eighthackarabic },
	{ GLYPH_afii57401, GLYPH_ninearabic },
	{ GLYPH_afii57401, GLYPH_ninehackarabic },
	{ GLYPH_afii57403, GLYPH_semicolonarabic },
	{ GLYPH_afii57407, GLYPH_questionarabic },
	{ GLYPH_afii57409, GLYPH_hamzaarabic },
	{ GLYPH_afii57409, GLYPH_hamzalowarabic },
	{ GLYPH_afii57410, GLYPH_alefmaddaabovearabic },
	{ GLYPH_afii57411, GLYPH_alefhamzaabovearabic },
	{ GLYPH_afii57412, GLYPH_wawhamzaabovearabic },
	{ GLYPH_afii57413, GLYPH_alefhamzabelowarabic },
	{ GLYPH_afii57414, GLYPH_yehhamzaabovearabic },
	{ GLYPH_afii57415, GLYPH_alefarabic },
	{ GLYPH_afii57416, GLYPH_beharabic },
	{ GLYPH_afii57417, GLYPH_tehmarbutaarabic },
	{ GLYPH_afii57418, GLYPH_teharabic },
	{ GLYPH_afii57419, GLYPH_theharabic },
	{ GLYPH_afii57420, GLYPH_jeemarabic },
	{ GLYPH_afii57421, GLYPH_haharabic },
	{ GLYPH_afii57422, GLYPH_khaharabic },
	{ GLYPH_afii57423, GLYPH_dalarabic },
	{ GLYPH_afii57424, GLYPH_thalarabic },
	{ GLYPH_afii57425, GLYPH_reharabic },
	{ GLYPH_afii57426, GLYPH_zainarabic },
	{ GLYPH_afii57427, GLYPH_seenarabic },
	{ GLYPH_afii57428, GLYPH_sheenarabic },
	{ GLYPH_afii57429, GLYPH_sadarabic },
	{ GLYPH_afii57430, GLYPH_dadarabic },
	{ GLYPH_afii57431, GLYPH_taharabic },
	{ GLYPH_afii57432, GLYPH_zaharabic },
	{ GLYPH_afii57433, GLYPH_ainarabic },
	{ GLYPH_afii57434, GLYPH_ghainarabic },
	{ GLYPH_afii57440, GLYPH_kashidaautoarabic },
	{ GLYPH_afii57440, GLYPH_kashidaautonosidebearingarabic },
	{ GLYPH_afii57440, GLYPH_tatweelarabic },
	{ GLYPH_afii57441, GLYPH_feharabic },
	{ GLYPH_afii57442, GLYPH_qafarabic },
	{ GLYPH_afii57443, GLYPH_kafarabic },
	{ GLYPH_afii57444, GLYPH_lamarabic },
	{ GLYPH_afii57445, GLYPH_meemarabic },
	{ GLYPH_afii57446, GLYPH_noonarabic },
	{ GLYPH_afii57448, GLYPH_wawarabic },
	{ GLYPH_afii57449, GLYPH_alefmaksuraarabic },
	{ GLYPH_afii57450, GLYPH_yeharabic },
	{ GLYPH_afii57451, GLYPH_fathatanarabic },
	{ GLYPH_afii57452, GLYPH_dammatanaltonearabic },
	{ GLYPH_afii57452, GLYPH_dammatanarabic },
	{ GLYPH_afii57453, GLYPH_kasratanarabic },
	{ GLYPH_afii57454, GLYPH_fathaarabic },
	{ GLYPH_afii57454, GLYPH_fathalowarabic },
	{ GLYPH_afii57455, GLYPH_dammaarabic },
	{ GLYPH_afii57455, GLYPH_dammalowarabic },
	{ GLYPH_afii57456, GLYPH_kasraarabic },
	{ GLYPH_afii57457, GLYPH_shaddaarabic },
	{ GLYPH_afii57458, GLYPH_sukunarabic },
	{ GLYPH_afii57470, GLYPH_heharabic },
	{ GLYPH_afii57505, GLYPH_veharabic },
	{ GLYPH_afii57506, GLYPH_peharabic },
	{ GLYPH_afii57507, GLYPH_tcheharabic },
	{ GLYPH_afii57508, GLYPH_jeharabic },
	{ GLYPH_afii57509, GLYPH_gafarabic },
	{ GLYPH_afii57511, GLYPH_tteharabic },
	{ GLYPH_afii57512, GLYPH_ddalarabic },
	{ GLYPH_afii57513, GLYPH_rreharabic },
	{ GLYPH_afii57514, GLYPH_noonghunnaarabic },
	{ GLYPH_afii57519, GLYPH_yehbarreearabic },
	{ GLYPH_afii57636, GLYPH_newsheqelsign },
	{ GLYPH_afii57636, GLYPH_sheqel },
	{ GLYPH_afii57636, GLYPH_sheqelhebrew },
	{ GLYPH_afii57645, GLYPH_maqafhebrew },
	{ GLYPH_afii57658, GLYPH_sofpasuqhebrew },
	{ GLYPH_afii57664, GLYPH_alef },
	{ GLYPH_afii57664, GLYPH_alefhebrew },
	{ GLYPH_afii57665, GLYPH_bet },
	{ GLYPH_afii57665, GLYPH_bethebrew },
	{ GLYPH_afii57666, GLYPH_gimel },
	{ GLYPH_afii57666, GLYPH_gimelhebrew },
	{ GLYPH_afii57667, GLYPH_dalet },
	{ GLYPH_afii57667, GLYPH_dalethebrew },
	{ GLYPH_afii57668, GLYPH_he },
	{ GLYPH_afii57668, GLYPH_hehebrew },
	{ GLYPH_afii57669, GLYPH_vav },
	{ GLYPH_afii57669, GLYPH_vavhebrew },
	{ GLYPH_afii57670, GLYPH_zayin },
	{ GLYPH_afii57670, GLYPH_zayinhebrew },
	{ GLYPH_afii57671, GLYPH_het },
	{ GLYPH_afii57671, GLYPH_hethebrew },
	{ GLYPH_afii57672, GLYPH_tet },
	{ GLYPH_afii57672, GLYPH_tethebrew },
	{ GLYPH_afii57673, GLYPH_yod },
	{ GLYPH_afii57673, GLYPH_yodhebrew },
	{ GLYPH_afii57674, GLYPH_finalkaf },
	{ GLYPH_afii57674, GLYPH_finalkafhebrew },
	{ GLYPH_afii57675, GLYPH_kaf },
	{ GLYPH_afii57675, GLYPH_kafhebrew },
	{ GLYPH_afii57676, GLYPH_lamed },
	{ GLYPH_afii57676, GLYPH_lamedhebrew },
	{ GLYPH_afii57677, GLYPH_finalmem },
	{ GLYPH_afii57677, GLYPH_finalmemhebrew },
	{ GLYPH_afii57678, GLYPH_mem },
	{ GLYPH_afii57678, GLYPH_memhebrew },
	{ GLYPH_afii57679, GLYPH_finalnun },
	{ GLYPH_afii57679, GLYPH_finalnunhebrew },
	{ GLYPH_afii57680, GLYPH_nun },
	{ GLYPH_afii57680, GLYPH_nunhebrew },
	{ GLYPH_afii57681, GLYPH_samekh },
	{ GLYPH_afii57681, GLYPH_samekhhebrew },
	{ GLYPH_afii57682, GLYPH_ayin },
	{ GLYPH_afii57682, GLYPH_ayinhebrew },
	{ GLYPH_afii57683, GLYPH_finalpe },
	{ GLYPH_afii57683, GLYPH_finalpehebrew },
	{ GLYPH_afii57684, GLYPH_pe },
	{ GLYPH_afii57684, GLYPH_pehebrew },
	{ GLYPH_afii57685, GLYPH_finaltsadi },
	{ GLYPH_afii57685, GLYPH_finaltsadihebrew },
	{ GLYPH_afii57686, GLYPH_tsadi },
	{ GLYPH_afii57686, GLYPH_tsadihebrew },
	{ GLYPH_afii57687, GLYPH_qof },
	{ GLYPH_afii57687, GLYPH_qofhebrew },
	{ GLYPH_afii57688, GLYPH_resh },
	{ GLYPH_afii57688, GLYPH_reshhebrew },
	{ GLYPH_afii57689, GLYPH_shin },
	{ GLYPH_afii57689, GLYPH_shinhebrew },
	{ GLYPH_afii57690, GLYPH_tav },
	{ GLYPH_afii57690, GLYPH_tavhebrew },
	{ GLYPH_afii57716, GLYPH_vavvavhebrew },
	{ GLYPH_afii57717, GLYPH_vavyodhebrew },
	{ GLYPH_afii57718, GLYPH_yodyodhebrew },
	{ GLYPH_afii57793, GLYPH_hiriq },
	{ GLYPH_afii57793, GLYPH_hiriq14 },
	{ GLYPH_afii57793, GLYPH_hiriq21 },
	{ GLYPH_afii57793, GLYPH_hiriq2d },
	{ GLYPH_afii57793, GLYPH_hiriqhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqnarrowhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqquarterhebrew },
	{ GLYPH_afii57793, GLYPH_hiriqwidehebrew },
	{ GLYPH_afii57794, GLYPH_tsere },
	{ GLYPH_afii57794, GLYPH_tsere12 },
	{ GLYPH_afii57794, GLYPH_tsere1e },
	{ GLYPH_afii57794, GLYPH_tsere2b },
	{ GLYPH_afii57794, GLYPH_tserehebrew },
	{ GLYPH_afii57794, GLYPH_tserenarrowhebrew },
	{ GLYPH_afii57794, GLYPH_tserequarterhebrew },
	{ GLYPH_afii57794, GLYPH_tserewidehebrew },
	{ GLYPH_afii57795, GLYPH_segol },
	{ GLYPH_afii57795, GLYPH_segol13 },
	{ GLYPH_afii57795, GLYPH_segol1f },
	{ GLYPH_afii57795, GLYPH_segol2c },
	{ GLYPH_afii57795, GLYPH_segolhebrew },
	{ GLYPH_afii57795, GLYPH_segolnarrowhebrew },
	{ GLYPH_afii57795, GLYPH_segolquarterhebrew },
	{ GLYPH_afii57795, GLYPH_segolwidehebrew },
	{ GLYPH_afii57796, GLYPH_qubuts },
	{ GLYPH_afii57796, GLYPH_qubuts18 },
	{ GLYPH_afii57796, GLYPH_qubuts25 },
	{ GLYPH_afii57796, GLYPH_qubuts31 },
	{ GLYPH_afii57796, GLYPH_qubutshebrew },
	{ GLYPH_afii57796, GLYPH_qubutsnarrowhebrew },
	{ GLYPH_afii57796, GLYPH_qubutsquarterhebrew },
	{ GLYPH_afii57796, GLYPH_qubutswidehebrew },
	{ GLYPH_afii57797, GLYPH_qamats },
	{ GLYPH_afii57797, GLYPH_qamats10 },
	{ GLYPH_afii57797, GLYPH_qamats1a },
	{ GLYPH_afii57797, GLYPH_qamats1c },
	{ GLYPH_afii57797, GLYPH_qamats27 },
	{ GLYPH_afii57797, GLYPH_qamats29 },
	{ GLYPH_afii57797, GLYPH_qamats33 },
	{ GLYPH_afii57797, GLYPH_qamatsde },
	{ GLYPH_afii57797, GLYPH_qamatshebrew },
	{ GLYPH_afii57797, GLYPH_qamatsnarrowhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatannarrowhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanquarterhebrew },
	{ GLYPH_afii57797, GLYPH_qamatsqatanwidehebrew },
	{ GLYPH_afii57797, GLYPH_qamatsquarterhebrew },
	{ GLYPH_afii57797, GLYPH_qamatswidehebrew },
	{ GLYPH_afii57798, GLYPH_patah },
	{ GLYPH_afii57798, GLYPH_patah11 },
	{ GLYPH_afii57798, GLYPH_patah1d },
	{ GLYPH_afii57798, GLYPH_patah2a },
	{ GLYPH_afii57798, GLYPH_patahhebrew },
	{ GLYPH_afii57798, GLYPH_patahnarrowhebrew },
	{ GLYPH_afii57798, GLYPH_patahquarterhebrew },
	{ GLYPH_afii57798, GLYPH_patahwidehebrew },
	{ GLYPH_afii57799, GLYPH_sheva },
	{ GLYPH_afii57799, GLYPH_sheva115 },
	{ GLYPH_afii57799, GLYPH_sheva15 },
	{ GLYPH_afii57799, GLYPH_sheva22 },
	{ GLYPH_afii57799, GLYPH_sheva2e },
	{ GLYPH_afii57799, GLYPH_shevahebrew },
	{ GLYPH_afii57799, GLYPH_shevanarrowhebrew },
	{ GLYPH_afii57799, GLYPH_shevaquarterhebrew },
	{ GLYPH_afii57799, GLYPH_shevawidehebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatah },
	{ GLYPH_afii57800, GLYPH_hatafpatah16 },
	{ GLYPH_afii57800, GLYPH_hatafpatah23 },
	{ GLYPH_afii57800, GLYPH_hatafpatah2f },
	{ GLYPH_afii57800, GLYPH_hatafpatahhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahnarrowhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahquarterhebrew },
	{ GLYPH_afii57800, GLYPH_hatafpatahwidehebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegol },
	{ GLYPH_afii57801, GLYPH_hatafsegol17 },
	{ GLYPH_afii57801, GLYPH_hatafsegol24 },
	{ GLYPH_afii57801, GLYPH_hatafsegol30 },
	{ GLYPH_afii57801, GLYPH_hatafsegolhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolnarrowhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolquarterhebrew },
	{ GLYPH_afii57801, GLYPH_hatafsegolwidehebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamats },
	{ GLYPH_afii57802, GLYPH_hatafqamats1b },
	{ GLYPH_afii57802, GLYPH_hatafqamats28 },
	{ GLYPH_afii57802, GLYPH_hatafqamats34 },
	{ GLYPH_afii57802, GLYPH_hatafqamatshebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatsnarrowhebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatsquarterhebrew },
	{ GLYPH_afii57802, GLYPH_hatafqamatswidehebrew },
	{ GLYPH_afii57803, GLYPH_sindothebrew },
	{ GLYPH_afii57804, GLYPH_shindothebrew },
	{ GLYPH_afii57806, GLYPH_holam },
	{ GLYPH_afii57806, GLYPH_holam19 },
	{ GLYPH_afii57806, GLYPH_holam26 },
	{ GLYPH_afii57806, GLYPH_holam32 },
	{ GLYPH_afii57806, GLYPH_holamhebrew },
	{ GLYPH_afii57806, GLYPH_holamnarrowhebrew },
	{ GLYPH_afii57806, GLYPH_holamquarterhebrew },
	{ GLYPH_afii57806, GLYPH_holamwidehebrew },
	{ GLYPH_afii57807, GLYPH_dagesh },
	{ GLYPH_afii57807, GLYPH_dageshhebrew },
	{ GLYPH_afii57839, GLYPH_siluqhebrew },
	{ GLYPH_afii57839, GLYPH_siluqlefthebrew },
	{ GLYPH_afii57841, GLYPH_rafe },
	{ GLYPH_afii57841, GLYPH_rafehebrew },
	{ GLYPH_afii57842, GLYPH_paseqhebrew },
	{ GLYPH_afii57929, GLYPH_apostrophemod },
	{ GLYPH_afii57929, GLYPH_quoteright },
	{ GLYPH_afii61248, GLYPH_careof },
	{ GLYPH_afii61289, GLYPH_lsquare },
	{ GLYPH_afii61352, GLYPH_numero },
	{ GLYPH_afii61664, GLYPH_zerowidthnonjoiner },
	{ GLYPH_afii63167, GLYPH_asteriskaltonearabic },
	{ GLYPH_afii63167, GLYPH_asteriskarabic },
	{ GLYPH_afii64937, GLYPH_commareversedmod },
	{ GLYPH_afii64937, GLYPH_quoteleft },
	{ GLYPH_alpha, GLYPH_afii9825 },
	{ GLYPH_alphatonos, GLYPH_afii9968 },
	{ GLYPH_arrowdblboth, GLYPH_dblarrowleft },
	{ GLYPH_arrowdblleft, GLYPH_arrowleftdbl },
	{ GLYPH_arrowdblright, GLYPH_dblarrowright },
	{ GLYPH_arrowupdnbse, GLYPH_arrowupdownbase },
	{ GLYPH_asciitilde, GLYPH_tilde },
	{ GLYPH_bar, GLYPH_verticalbar },
	{ GLYPH_beta, GLYPH_afii9826 },
	{ GLYPH_brokenbar, GLYPH_bar },
	{ GLYPH_cdotaccent, GLYPH_cdot },
	{ GLYPH_cedilla, GLYPH_commaaccent },
	{ GLYPH_chi, GLYPH_afii9851 },
	{ GLYPH_circle, GLYPH_whitecircle },
	{ GLYPH_circlemultiply, GLYPH_timescircle },
	{ GLYPH_circleplus, GLYPH_pluscircle },
	{ GLYPH_club, GLYPH_clubsuitblack },
	{ GLYPH_colonmonetary, GLYPH_colonsign },
	{ GLYPH_congruent, GLYPH_approximatelyequal },
	{ GLYPH_dcroat, GLYPH_dmacron },
	{ GLYPH_dcroat, GLYPH_dstroke },
	{ GLYPH_degree, GLYPH_ring },
	{ GLYPH_delta, GLYPH_afii9829 },
	{ GLYPH_dieresis, GLYPH_diaeresis },
	{ GLYPH_dieresistonos, GLYPH_dialytikatonos },
	{ GLYPH_dieresistonos, GLYPH_dieresisacute },
	{ GLYPH_divide, GLYPH_division },
	{ GLYPH_dkshade, GLYPH_shadedark },
	{ GLYPH_dotaccent, GLYPH_abovedot },
	{ GLYPH_dotbelowcomb, GLYPH_dotbelowcmb },
	{ GLYPH_edieresis, GLYPH_ediaeresis },
	{ GLYPH_edotaccent, GLYPH_edot },
	{ GLYPH_epsilon, GLYPH_afii9830 },
	{ GLYPH_epsilontonos, GLYPH_afii9969 },
	{ GLYPH_eta, GLYPH_afii9834 },
	{ GLYPH_etatonos, GLYPH_afii9970 },
	{ GLYPH_existential, GLYPH_thereexists },
	{ GLYPH_female, GLYPH_venus },
	{ GLYPH_filledbox, GLYPH_blacksquare },
	{ GLYPH_filledrect, GLYPH_blackrectangle },
	{ GLYPH_gamma, GLYPH_afii9828 },
	{ GLYPH_gcommaaccent, GLYPH_gcedilla },
	{ GLYPH_gdotaccent, GLYPH_gdot },
	{ GLYPH_germandbls, GLYPH_ssharp },
	{ GLYPH_gradient, GLYPH_nabla },
	{ GLYPH_grave, GLYPH_quoteleft },
	{ GLYPH_gravecomb, GLYPH_gravecmb },
	{ GLYPH_heart, GLYPH_heartsuitblack },
	{ GLYPH_hookabovecomb, GLYPH_hookcmb },
	{ GLYPH_hungarumlaut, GLYPH_doubleacute },
	{ GLYPH_hyphen, GLYPH_sfthyphen },
	{ GLYPH_integralbt, GLYPH_integralbottom },
	{ GLYPH_integraltp, GLYPH_integraltop },
	{ GLYPH_invbullet, GLYPH_bulletinverse },
	{ GLYPH_invcircle, GLYPH_whitecircleinverse },
	{ GLYPH_invsmileface, GLYPH_blacksmilingface },
	{ GLYPH_iota, GLYPH_afii9836 },
	{ GLYPH_iotadieresis, GLYPH_afii9975 },
	{ GLYPH_iotadieresistonos, GLYPH_afii9977 },
	{ GLYPH_iotatonos, GLYPH_afii9971 },
	{ GLYPH_kappa, GLYPH_afii9837 },
	{ GLYPH_kcommaaccent, GLYPH_kcedilla },
	{ GLYPH_lambda, GLYPH_afii9838 },
	{ GLYPH_lcommaaccent, GLYPH_lcedilla },
	{ GLYPH_ldot, GLYPH_ldotaccent },
	{ GLYPH_lira, GLYPH_afii08941 },
	{ GLYPH_longs, GLYPH_slong },
	{ GLYPH_lslash, GLYPH_lstroke },
	{ GLYPH_ltshade, GLYPH_shadelight },
	{ GLYPH_macron, GLYPH_overscore },
	{ GLYPH_male, GLYPH_mars },
	{ GLYPH_minus, GLYPH_hyphen },
	{ GLYPH_mu, GLYPH_afii9839 },
	{ GLYPH_mu, GLYPH_mu1 },
	{ GLYPH_musicalnotedbl, GLYPH_eighthnotebeamed },
	{ GLYPH_napostrophe, GLYPH_quoterightn },
	{ GLYPH_ncommaaccent, GLYPH_ncedilla },
	{ GLYPH_nobreakspace, GLYPH_nonbreakingspace },
	{ GLYPH_nobreakspace, GLYPH_space },
	{ GLYPH_notelement, GLYPH_notelementof },
	{ GLYPH_nu, GLYPH_afii9840 },
	{ GLYPH_odieresis, GLYPH_odiaeresis },
	{ GLYPH_ohungarumlaut, GLYPH_odblacute },
	{ GLYPH_ohungarumlaut, GLYPH_odoubleacute },
	{ GLYPH_ohungarumlaut, GLYPH_ohungar },
	{ GLYPH_omega, GLYPH_afii9853 },
	{ GLYPH_omega1, GLYPH_pisymbolgreek },
	{ GLYPH_omegatonos, GLYPH_afii9974 },
	{ GLYPH_omicron, GLYPH_afii9842 },
	{ GLYPH_omicrontonos, GLYPH_afii9972 },
	{ GLYPH_openbullet, GLYPH_whitebullet },
	{ GLYPH_orthogonal, GLYPH_rightangle },
	{ GLYPH_oslashacute, GLYPH_ostrokeacute },
	{ GLYPH_period, GLYPH_fullstop },
	{ GLYPH_periodcentered, GLYPH_middot },
	{ GLYPH_phi, GLYPH_afii9850 },
	{ GLYPH_phi1, GLYPH_phisymbolgreek },
	{ GLYPH_pi, GLYPH_afii9843 },
	{ GLYPH_propersubset, GLYPH_subset },
	{ GLYPH_propersuperset, GLYPH_superset },
	{ GLYPH_psi, GLYPH_afii9852 },
	{ GLYPH_quoteleft, GLYPH_grave },
	{ GLYPH_quotereversed, GLYPH_quoteleftreversed },
	{ GLYPH_quoteright, GLYPH_apostrophe },
	{ GLYPH_quoteright, GLYPH_quotesingle },
	{ GLYPH_quotesingle, GLYPH_quoteright },
	{ GLYPH_rcommaaccent, GLYPH_rcedilla },
	{ GLYPH_reflexsubset, GLYPH_subsetorequal },
	{ GLYPH_reflexsuperset, GLYPH_supersetorequal },
	{ GLYPH_revlogicalnot, GLYPH_logicalnotreversed },
	{ GLYPH_rho, GLYPH_afii9845 },
	{ GLYPH_ring, GLYPH_degree },
	{ GLYPH_scedilla, GLYPH_scedilla1 },
	{ GLYPH_scedilla, GLYPH_scommaaccent },
	{ GLYPH_semicolon, GLYPH_uni037e },
	{ GLYPH_shade, GLYPH_shademedium },
	{ GLYPH_sigma, GLYPH_afii9846 },
	{ GLYPH_sigma1, GLYPH_afii9847 },
	{ GLYPH_sigma1, GLYPH_sigmafinal },
	{ GLYPH_similar, GLYPH_tildeoperator },
	{ GLYPH_smileface, GLYPH_whitesmilingface },
	{ GLYPH_space, GLYPH_spacehackarabic },
	{ GLYPH_spade, GLYPH_spadesuitblack },
	{ GLYPH_sun, GLYPH_compass },
	{ GLYPH_tau, GLYPH_afii9848 },
	{ GLYPH_tcommaaccent, GLYPH_tcedilla },
	{ GLYPH_tcommaaccent, GLYPH_tcedilla1 },
	{ GLYPH_theta, GLYPH_afii9835 },
	{ GLYPH_theta1, GLYPH_thetasymbolgreek },
	{ GLYPH_tilde, GLYPH_ilde },
	{ GLYPH_tildecomb, GLYPH_tildecmb },
	{ GLYPH_tonos, GLYPH_acute },
	{ GLYPH_triagdn, GLYPH_blackdownpointingtriangle },
	{ GLYPH_triaglf, GLYPH_blackleftpointingpointer },
	{ GLYPH_triagrt, GLYPH_blackrightpointingpointer },
	{ GLYPH_triagup, GLYPH_blackuppointingtriangle },
	{ GLYPH_twodotenleader, GLYPH_twodotleader },
	{ GLYPH_udieresis, GLYPH_udiaeresis },
	{ GLYPH_uhungarumlaut, GLYPH_udblacute },
	{ GLYPH_uhungarumlaut, GLYPH_udoubleacute },
	{ GLYPH_uhungarumlaut, GLYPH_uhungar },
	{ GLYPH_underscore, GLYPH_lowline },
	{ GLYPH_underscoredbl, GLYPH_dbllowline },
	{ GLYPH_universal, GLYPH_forall },
	{ GLYPH_upsilon, GLYPH_afii9849 },
	{ GLYPH_upsilondieresis, GLYPH_afii9976 },
	{ GLYPH_upsilondieresistonos, GLYPH_afii9978 },
	{ GLYPH_upsilontonos, GLYPH_afii9973 },
	{ GLYPH_xi, GLYPH_afii9841 },
	{ GLYPH_zdotaccent, GLYPH_zabovedot },
	{ GLYPH_zdotaccent, GLYPH_zdot },
	{ GLYPH_zeta, GLYPH_afii9833 },
    };

/************************************************************************/
/*									*/
/*  See whether the font contains an alternative for a missing glyph.	*/
/*									*/
/************************************************************************/

int psFindAlternate(	const AfmFontInfo *	afi,
			const char *		glyphName )
    {
    const AlternateGlyphName *	agn= PS_AlternateNames;

    int				l;
    int				r;
    int				m;

    int				cmp;

#   if 0
    static int			sortChecked= 0;

    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);

    if  ( ! sortChecked )
	{
	for ( m= 1; m < r; m++ )
	    {
	    cmp= strcmp( agn[m].agnOfficialName, agn[m- 1].agnOfficialName );
	    if  ( cmp > 0 )
		{ continue;	}

	    if  ( cmp < 0 )
		{
		SSDEB(agn[m].agnOfficialName,agn[m- 1].agnOfficialName);
		return -1;
		}

	    cmp= strcmp( agn[m].agnSurrogateName, agn[m- 1].agnSurrogateName );
	    if  ( cmp > 0 )
		{ continue;	}

	    SSDEB(agn[m].agnOfficialName,agn[m- 1].agnOfficialName);
	    SSDEB(agn[m].agnSurrogateName,agn[m- 1].agnSurrogateName);
	    return -1;
	    }

	sortChecked= 1;
	}
#   endif

    l= 0;
    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);
    m= ( l+ r )/ 2;

    while( m != l )
	{
	cmp= strcmp( agn[m].agnOfficialName, glyphName );

	if  ( cmp >= 0 )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    cmp= strcmp( agn[m].agnOfficialName, glyphName );
    if  ( cmp < 0 )
	{ m++; }


    agn += m;
    r= sizeof(PS_AlternateNames)/sizeof(AlternateGlyphName);

    while( m < r && ! strcmp( agn->agnOfficialName, glyphName ) )
	{
	int			j;
	AfmCharMetric *		acm;

	acm= afi->afiMetrics;
	for ( j= 0; j < afi->afiMetricCount; acm++, j++ )
	    {
	    if  ( acm->acmN					&&
		  ! strcmp( acm->acmN, agn->agnSurrogateName )	)
		{ break;	}
	    }

	if  ( j < afi->afiMetricCount )
	    { return j;	}

	agn++; m++;
	}

    return -1;
    }

/************************************************************************/
/*									*/
/*  Translate unicode values to glyph names.				*/
/*									*/
/************************************************************************/

typedef struct UnicodeToGlyphName
    {
    unsigned short	utgUnicode;
    const char *	utgGlyphName;
    } UnicodeToGlyphName;

static const UnicodeToGlyphName psUnicodeToGlyphNames[]=
{
    { 0x0020, GLYPH_space },
    { 0x0021, GLYPH_exclam },
    { 0x0022, GLYPH_quotedbl },
    { 0x0023, GLYPH_numbersign },
    { 0x0024, GLYPH_dollar },
    { 0x0025, GLYPH_percent },
    { 0x0026, GLYPH_ampersand },
    { 0x0027, GLYPH_quotesingle },
    { 0x0028, GLYPH_parenleft },
    { 0x0029, GLYPH_parenright },
    { 0x002a, GLYPH_asterisk },
    { 0x002b, GLYPH_plus },
    { 0x002c, GLYPH_comma },
    { 0x002d, GLYPH_hyphen },
    { 0x002e, GLYPH_period },
    { 0x002f, GLYPH_slash },
    { 0x0030, GLYPH_zero },
    { 0x0030, GLYPH_zero },
    { 0x0031, GLYPH_one },
    { 0x0032, GLYPH_two },
    { 0x0033, GLYPH_three },
    { 0x0034, GLYPH_four },
    { 0x0035, GLYPH_five },
    { 0x0036, GLYPH_six },
    { 0x0037, GLYPH_seven },
    { 0x0038, GLYPH_eight },
    { 0x0039, GLYPH_nine },
    { 0x003a, GLYPH_colon },
    { 0x003b, GLYPH_semicolon },
    { 0x003c, GLYPH_less },
    { 0x003d, GLYPH_equal },
    { 0x003e, GLYPH_greater },
    { 0x003f, GLYPH_question },
    { 0x0040, GLYPH_at },
    { 0x0041, GLYPH_A },
    { 0x0042, GLYPH_B },
    { 0x0043, GLYPH_C },
    { 0x0044, GLYPH_D },
    { 0x0045, GLYPH_E },
    { 0x0046, GLYPH_F },
    { 0x0047, GLYPH_G },
    { 0x0048, GLYPH_H },
    { 0x0049, GLYPH_I },
    { 0x004a, GLYPH_J },
    { 0x004b, GLYPH_K },
    { 0x004c, GLYPH_L },
    { 0x004d, GLYPH_M },
    { 0x004e, GLYPH_N },
    { 0x004f, GLYPH_O },
    { 0x0050, GLYPH_P },
    { 0x0051, GLYPH_Q },
    { 0x0052, GLYPH_R },
    { 0x0053, GLYPH_S },
    { 0x0054, GLYPH_T },
    { 0x0055, GLYPH_U },
    { 0x0056, GLYPH_V },
    { 0x0057, GLYPH_W },
    { 0x0058, GLYPH_X },
    { 0x0059, GLYPH_Y },
    { 0x005a, GLYPH_Z },
    { 0x005b, GLYPH_bracketleft },
    { 0x005c, GLYPH_backslash },
    { 0x005d, GLYPH_bracketright },
    { 0x005e, GLYPH_asciicircum },
    { 0x005f, GLYPH_underscore },
    { 0x0060, GLYPH_grave },
    { 0x0061, GLYPH_a },
    { 0x0062, GLYPH_b },
    { 0x0063, GLYPH_c },
    { 0x0064, GLYPH_d },
    { 0x0065, GLYPH_e },
    { 0x0066, GLYPH_f },
    { 0x0067, GLYPH_g },
    { 0x0068, GLYPH_h },
    { 0x0069, GLYPH_i },
    { 0x006a, GLYPH_j },
    { 0x006b, GLYPH_k },
    { 0x006c, GLYPH_l },
    { 0x006d, GLYPH_m },
    { 0x006e, GLYPH_n },
    { 0x006f, GLYPH_o },
    { 0x0070, GLYPH_p },
    { 0x0071, GLYPH_q },
    { 0x0072, GLYPH_r },
    { 0x0073, GLYPH_s },
    { 0x0074, GLYPH_t },
    { 0x0075, GLYPH_u },
    { 0x0076, GLYPH_v },
    { 0x0077, GLYPH_w },
    { 0x0077, GLYPH_w },
    { 0x0078, GLYPH_x },
    { 0x0078, GLYPH_x },
    { 0x0079, GLYPH_y },
    { 0x0079, GLYPH_y },
    { 0x007a, GLYPH_z },
    { 0x007a, GLYPH_z },
    { 0x007b, GLYPH_braceleft },
    { 0x007c, GLYPH_bar },
    { 0x007d, GLYPH_braceright },
    { 0x007e, GLYPH_asciitilde },
    { 0x00a0, GLYPH_nobreakspace },
    { 0x00a1, GLYPH_exclamdown },
    { 0x00a2, GLYPH_cent },
    { 0x00a3, GLYPH_sterling },
    { 0x00a4, GLYPH_currency },
    { 0x00a5, GLYPH_yen },
    { 0x00a5, GLYPH_yen },
    { 0x00a6, GLYPH_brokenbar },
    { 0x00a7, GLYPH_section },
    { 0x00a8, GLYPH_dieresis },
    { 0x00a9, GLYPH_copyright },
    { 0x00aa, GLYPH_ordfeminine },
    { 0x00ab, GLYPH_guillemotleft },
    { 0x00ac, GLYPH_logicalnot },
    { 0x00ae, GLYPH_registered },
    { 0x00af, GLYPH_macron },
    { 0x00b0, GLYPH_degree },
    { 0x00b1, GLYPH_plusminus },
    { 0x00b2, GLYPH_twosuperior },
    { 0x00b3, GLYPH_threesuperior },
    { 0x00b4, GLYPH_acute },
    { 0x00b5, GLYPH_mu },
    { 0x00b6, GLYPH_paragraph },
    { 0x00b7, GLYPH_periodcentered },
    { 0x00b8, GLYPH_cedilla },
    { 0x00b9, GLYPH_onesuperior },
    { 0x00ba, GLYPH_ordmasculine },
    { 0x00bb, GLYPH_guillemotright },
    { 0x00bc, GLYPH_onequarter },
    { 0x00bd, GLYPH_onehalf },
    { 0x00be, GLYPH_threequarters },
    { 0x00bf, GLYPH_questiondown },
    { 0x00c0, GLYPH_Agrave },
    { 0x00c1, GLYPH_Aacute },
    { 0x00c2, GLYPH_Acircumflex },
    { 0x00c3, GLYPH_Atilde },
    { 0x00c4, GLYPH_Adieresis },
    { 0x00c5, GLYPH_Aring },
    { 0x00c6, GLYPH_AE },
    { 0x00c7, GLYPH_Ccedilla },
    { 0x00c8, GLYPH_Egrave },
    { 0x00c9, GLYPH_Eacute },
    { 0x00ca, GLYPH_Ecircumflex },
    { 0x00cb, GLYPH_Edieresis },
    { 0x00cc, GLYPH_Igrave },
    { 0x00cd, GLYPH_Iacute },
    { 0x00ce, GLYPH_Icircumflex },
    { 0x00cf, GLYPH_Idieresis },
    { 0x00d0, GLYPH_Eth },
    { 0x00d1, GLYPH_Ntilde },
    { 0x00d2, GLYPH_Ograve },
    { 0x00d3, GLYPH_Oacute },
    { 0x00d4, GLYPH_Ocircumflex },
    { 0x00d5, GLYPH_Otilde },
    { 0x00d6, GLYPH_Odieresis },
    { 0x00d7, GLYPH_multiply },
    { 0x00d8, GLYPH_Oslash },
    { 0x00d9, GLYPH_Ugrave },
    { 0x00da, GLYPH_Uacute },
    { 0x00db, GLYPH_Ucircumflex },
    { 0x00dc, GLYPH_Udieresis },
    { 0x00dd, GLYPH_Yacute },
    { 0x00de, GLYPH_Thorn },
    { 0x00df, GLYPH_germandbls },
    { 0x00e0, GLYPH_agrave },
    { 0x00e1, GLYPH_aacute },
    { 0x00e2, GLYPH_acircumflex },
    { 0x00e3, GLYPH_atilde },
    { 0x00e4, GLYPH_adieresis },
    { 0x00e5, GLYPH_aring },
    { 0x00e6, GLYPH_ae },
    { 0x00e7, GLYPH_ccedilla },
    { 0x00e8, GLYPH_egrave },
    { 0x00e9, GLYPH_eacute },
    { 0x00ea, GLYPH_ecircumflex },
    { 0x00eb, GLYPH_edieresis },
    { 0x00ec, GLYPH_igrave },
    { 0x00ed, GLYPH_iacute },
    { 0x00ee, GLYPH_icircumflex },
    { 0x00ef, GLYPH_idieresis },
    { 0x00f0, GLYPH_eth },
    { 0x00f1, GLYPH_ntilde },
    { 0x00f2, GLYPH_ograve },
    { 0x00f3, GLYPH_oacute },
    { 0x00f4, GLYPH_ocircumflex },
    { 0x00f5, GLYPH_otilde },
    { 0x00f6, GLYPH_odieresis },
    { 0x00f7, GLYPH_divide },
    { 0x00f8, GLYPH_oslash },
    { 0x00f9, GLYPH_ugrave },
    { 0x00fa, GLYPH_uacute },
    { 0x00fb, GLYPH_ucircumflex },
    { 0x00fc, GLYPH_udieresis },
    { 0x00fd, GLYPH_yacute },
    { 0x00fd, GLYPH_yacute },
    { 0x00fe, GLYPH_thorn },
    { 0x00ff, GLYPH_ydieresis },
    { 0x00ff, GLYPH_ydieresis },
    { 0x0100, GLYPH_Amacron },
    { 0x0101, GLYPH_amacron },
    { 0x0102, GLYPH_Abreve },
    { 0x0103, GLYPH_abreve },
    { 0x0104, GLYPH_Aogonek },
    { 0x0105, GLYPH_aogonek },
    { 0x0106, GLYPH_Cacute },
    { 0x0107, GLYPH_cacute },
    { 0x0108, GLYPH_Ccircumflex },
    { 0x0109, GLYPH_ccircumflex },
    { 0x010a, GLYPH_Cdotaccent },
    { 0x010b, GLYPH_cdotaccent },
    { 0x010c, GLYPH_Ccaron },
    { 0x010d, GLYPH_ccaron },
    { 0x010e, GLYPH_Dcaron },
    { 0x010f, GLYPH_dcaron },
    { 0x0110, GLYPH_Dcroat },
    { 0x0111, GLYPH_dcroat },
    { 0x0112, GLYPH_Emacron },
    { 0x0113, GLYPH_emacron },
    { 0x0114, GLYPH_Ebreve },
    { 0x0115, GLYPH_ebreve },
    { 0x0116, GLYPH_Edotaccent },
    { 0x0117, GLYPH_edotaccent },
    { 0x0118, GLYPH_Eogonek },
    { 0x0119, GLYPH_eogonek },
    { 0x011a, GLYPH_Ecaron },
    { 0x011b, GLYPH_ecaron },
    { 0x011c, GLYPH_Gcircumflex },
    { 0x011d, GLYPH_gcircumflex },
    { 0x011e, GLYPH_Gbreve },
    { 0x011f, GLYPH_gbreve },
    { 0x0120, GLYPH_Gdotaccent },
    { 0x0121, GLYPH_gdotaccent },
    { 0x0122, GLYPH_Gcommaaccent },
    { 0x0123, GLYPH_gcommaaccent },
    { 0x0124, GLYPH_Hcircumflex },
    { 0x0125, GLYPH_hcircumflex },
    { 0x0126, GLYPH_Hbar },
    { 0x0127, GLYPH_hbar },
    { 0x0128, GLYPH_Itilde },
    { 0x0129, GLYPH_itilde },
    { 0x012a, GLYPH_Imacron },
    { 0x012b, GLYPH_imacron },
    { 0x012c, GLYPH_Ibreve },
    { 0x012d, GLYPH_ibreve },
    { 0x012e, GLYPH_Iogonek },
    { 0x012f, GLYPH_iogonek },
    { 0x0130, GLYPH_Idotaccent },
    { 0x0131, GLYPH_dotlessi },
    { 0x0132, GLYPH_IJ },
    { 0x0133, GLYPH_ij },
    { 0x0134, GLYPH_Jcircumflex },
    { 0x0135, GLYPH_jcircumflex },
    { 0x0136, GLYPH_Kcommaaccent },
    { 0x0137, GLYPH_kcommaaccent },
    { 0x0138, GLYPH_kgreenlandic },
    { 0x0139, GLYPH_Lacute },
    { 0x013a, GLYPH_lacute },
    { 0x013b, GLYPH_Lcommaaccent },
    { 0x013c, GLYPH_lcommaaccent },
    { 0x013d, GLYPH_Lcaron },
    { 0x013e, GLYPH_lcaron },
    { 0x013f, GLYPH_Ldot },
    { 0x0140, GLYPH_ldot },
    { 0x0141, GLYPH_Lslash },
    { 0x0142, GLYPH_lslash },
    { 0x0143, GLYPH_Nacute },
    { 0x0144, GLYPH_nacute },
    { 0x0145, GLYPH_Ncommaaccent },
    { 0x0146, GLYPH_ncommaaccent },
    { 0x0147, GLYPH_Ncaron },
    { 0x0148, GLYPH_ncaron },
    { 0x0149, GLYPH_napostrophe },
    { 0x014a, GLYPH_Eng },
    { 0x014b, GLYPH_eng },
    { 0x014c, GLYPH_Omacron },
    { 0x014d, GLYPH_omacron },
    { 0x014e, GLYPH_Obreve },
    { 0x014f, GLYPH_obreve },
    { 0x0150, GLYPH_Ohungarumlaut },
    { 0x0151, GLYPH_ohungarumlaut },
    { 0x0152, GLYPH_OE },
    { 0x0153, GLYPH_oe },
    { 0x0154, GLYPH_Racute },
    { 0x0155, GLYPH_racute },
    { 0x0156, GLYPH_Rcommaaccent },
    { 0x0157, GLYPH_rcommaaccent },
    { 0x0158, GLYPH_Rcaron },
    { 0x0159, GLYPH_rcaron },
    { 0x015a, GLYPH_Sacute },
    { 0x015b, GLYPH_sacute },
    { 0x015c, GLYPH_Scircumflex },
    { 0x015d, GLYPH_scircumflex },
    { 0x015e, GLYPH_Scedilla },
    { 0x015f, GLYPH_scedilla },
    { 0x0160, GLYPH_Scaron },
    { 0x0161, GLYPH_scaron },
    { 0x0162, GLYPH_Tcommaaccent },
    { 0x0163, GLYPH_tcommaaccent },
    { 0x0164, GLYPH_Tcaron },
    { 0x0165, GLYPH_tcaron },
    { 0x0166, GLYPH_Tbar },
    { 0x0167, GLYPH_tbar },
    { 0x0168, GLYPH_Utilde },
    { 0x0169, GLYPH_utilde },
    { 0x016a, GLYPH_Umacron },
    { 0x016b, GLYPH_umacron },
    { 0x016c, GLYPH_Ubreve },
    { 0x016d, GLYPH_ubreve },
    { 0x016e, GLYPH_Uring },
    { 0x016f, GLYPH_uring },
    { 0x0170, GLYPH_Uhungarumlaut },
    { 0x0171, GLYPH_uhungarumlaut },
    { 0x0172, GLYPH_Uogonek },
    { 0x0173, GLYPH_uogonek },
    { 0x0174, GLYPH_Wcircumflex },
    { 0x0175, GLYPH_wcircumflex },
    { 0x0175, GLYPH_wcircumflex },
    { 0x0176, GLYPH_Ycircumflex },
    { 0x0177, GLYPH_ycircumflex },
    { 0x0177, GLYPH_ycircumflex },
    { 0x0178, GLYPH_Ydieresis },
    { 0x0179, GLYPH_Zacute },
    { 0x017a, GLYPH_zacute },
    { 0x017a, GLYPH_zacute },
    { 0x017b, GLYPH_Zdotaccent },
    { 0x017c, GLYPH_zdotaccent },
    { 0x017c, GLYPH_zdotaccent },
    { 0x017d, GLYPH_Zcaron },
    { 0x017e, GLYPH_zcaron },
    { 0x017e, GLYPH_zcaron },
    { 0x017f, GLYPH_longs },
    { 0x0192, GLYPH_florin },
    { 0x01a0, GLYPH_Ohorn },
    { 0x01a1, GLYPH_ohorn },
    { 0x01af, GLYPH_Uhorn },
    { 0x01b0, GLYPH_uhorn },
    { 0x01e6, GLYPH_Gcaron },
    { 0x01e7, GLYPH_gcaron },
    { 0x01fa, GLYPH_Aringacute },
    { 0x01fb, GLYPH_aringacute },
    { 0x01fc, GLYPH_AEacute },
    { 0x01fd, GLYPH_aeacute },
    { 0x01fe, GLYPH_Oslashacute },
    { 0x01ff, GLYPH_oslashacute },
    { 0x0218, GLYPH_Scommaaccent },
    { 0x0219, GLYPH_scommaaccent },
    { 0x02bc, GLYPH_afii57929 },
    { 0x02bd, GLYPH_afii64937 },
    { 0x02c6, GLYPH_circumflex },
    { 0x02c7, GLYPH_caron },
    { 0x02d8, GLYPH_breve },
    { 0x02d9, GLYPH_dotaccent },
    { 0x02da, GLYPH_ring },
    { 0x02db, GLYPH_ogonek },
    { 0x02dc, GLYPH_tilde },
    { 0x02dd, GLYPH_hungarumlaut },
    { 0x0300, GLYPH_gravecomb },
    { 0x0301, GLYPH_acutecomb },
    { 0x0303, GLYPH_tildecomb },
    { 0x0309, GLYPH_hookabovecomb },
    { 0x0323, GLYPH_dotbelowcomb },
    { 0x0384, GLYPH_tonos },
    { 0x0385, GLYPH_dieresistonos },
    { 0x0386, GLYPH_Alphatonos },
    { 0x0387, GLYPH_anoteleia },
    { 0x0388, GLYPH_Epsilontonos },
    { 0x0389, GLYPH_Etatonos },
    { 0x038a, GLYPH_Iotatonos },
    { 0x038c, GLYPH_Omicrontonos },
    { 0x038e, GLYPH_Upsilontonos },
    { 0x038f, GLYPH_Omegatonos },
    { 0x0390, GLYPH_iotadieresistonos },
    { 0x0391, GLYPH_Alpha },
    { 0x0392, GLYPH_Beta },
    { 0x0393, GLYPH_Gamma },
    { 0x0395, GLYPH_Epsilon },
    { 0x0396, GLYPH_Zeta },
    { 0x0397, GLYPH_Eta },
    { 0x0398, GLYPH_Theta },
    { 0x0399, GLYPH_Iota },
    { 0x039a, GLYPH_Kappa },
    { 0x039b, GLYPH_Lambda },
    { 0x039c, GLYPH_Mu },
    { 0x039d, GLYPH_Nu },
    { 0x039e, GLYPH_Xi },
    { 0x039f, GLYPH_Omicron },
    { 0x03a0, GLYPH_Pi },
    { 0x03a1, GLYPH_Rho },
    { 0x03a3, GLYPH_Sigma },
    { 0x03a4, GLYPH_Tau },
    { 0x03a5, GLYPH_Upsilon },
    { 0x03a6, GLYPH_Phi },
    { 0x03a7, GLYPH_Chi },
    { 0x03a8, GLYPH_Psi },
    { 0x03aa, GLYPH_Iotadieresis },
    { 0x03ab, GLYPH_Upsilondieresis },
    { 0x03ac, GLYPH_alphatonos },
    { 0x03ad, GLYPH_epsilontonos },
    { 0x03ae, GLYPH_etatonos },
    { 0x03af, GLYPH_iotatonos },
    { 0x03b0, GLYPH_upsilondieresistonos },
    { 0x03b1, GLYPH_alpha },
    { 0x03b2, GLYPH_beta },
    { 0x03b3, GLYPH_gamma },
    { 0x03b4, GLYPH_delta },
    { 0x03b5, GLYPH_epsilon },
    { 0x03b6, GLYPH_zeta },
    { 0x03b6, GLYPH_zeta },
    { 0x03b7, GLYPH_eta },
    { 0x03b8, GLYPH_theta },
    { 0x03b9, GLYPH_iota },
    { 0x03ba, GLYPH_kappa },
    { 0x03bb, GLYPH_lambda },
    { 0x03bd, GLYPH_nu },
    { 0x03be, GLYPH_xi },
    { 0x03be, GLYPH_xi },
    { 0x03bf, GLYPH_omicron },
    { 0x03c0, GLYPH_pi },
    { 0x03c1, GLYPH_rho },
    { 0x03c2, GLYPH_sigma1 },
    { 0x03c3, GLYPH_sigma },
    { 0x03c4, GLYPH_tau },
    { 0x03c5, GLYPH_upsilon },
    { 0x03c6, GLYPH_phi },
    { 0x03c7, GLYPH_chi },
    { 0x03c8, GLYPH_psi },
    { 0x03c9, GLYPH_omega },
    { 0x03ca, GLYPH_iotadieresis },
    { 0x03cb, GLYPH_upsilondieresis },
    { 0x03cc, GLYPH_omicrontonos },
    { 0x03cd, GLYPH_upsilontonos },
    { 0x03ce, GLYPH_omegatonos },
    { 0x03d1, GLYPH_theta1 },
    { 0x03d2, GLYPH_Upsilon1 },
    { 0x03d5, GLYPH_phi1 },
    { 0x03d6, GLYPH_omega1 },
    { 0x0401, GLYPH_afii10023 },
    { 0x0402, GLYPH_afii10051 },
    { 0x0403, GLYPH_afii10052 },
    { 0x0404, GLYPH_afii10053 },
    { 0x0405, GLYPH_afii10054 },
    { 0x0406, GLYPH_afii10055 },
    { 0x0407, GLYPH_afii10056 },
    { 0x0408, GLYPH_afii10057 },
    { 0x0409, GLYPH_afii10058 },
    { 0x040a, GLYPH_afii10059 },
    { 0x040b, GLYPH_afii10060 },
    { 0x040c, GLYPH_afii10061 },
    { 0x040e, GLYPH_afii10062 },
    { 0x040f, GLYPH_afii10145 },
    { 0x0410, GLYPH_afii10017 },
    { 0x0411, GLYPH_afii10018 },
    { 0x0412, GLYPH_afii10019 },
    { 0x0413, GLYPH_afii10020 },
    { 0x0414, GLYPH_afii10021 },
    { 0x0415, GLYPH_afii10022 },
    { 0x0416, GLYPH_afii10024 },
    { 0x0417, GLYPH_afii10025 },
    { 0x0418, GLYPH_afii10026 },
    { 0x0419, GLYPH_afii10027 },
    { 0x041a, GLYPH_afii10028 },
    { 0x041b, GLYPH_afii10029 },
    { 0x041c, GLYPH_afii10030 },
    { 0x041d, GLYPH_afii10031 },
    { 0x041e, GLYPH_afii10032 },
    { 0x041f, GLYPH_afii10033 },
    { 0x0420, GLYPH_afii10034 },
    { 0x0421, GLYPH_afii10035 },
    { 0x0422, GLYPH_afii10036 },
    { 0x0423, GLYPH_afii10037 },
    { 0x0424, GLYPH_afii10038 },
    { 0x0425, GLYPH_afii10039 },
    { 0x0426, GLYPH_afii10040 },
    { 0x0427, GLYPH_afii10041 },
    { 0x0428, GLYPH_afii10042 },
    { 0x0429, GLYPH_afii10043 },
    { 0x042a, GLYPH_afii10044 },
    { 0x042b, GLYPH_afii10045 },
    { 0x042c, GLYPH_afii10046 },
    { 0x042d, GLYPH_afii10047 },
    { 0x042e, GLYPH_afii10048 },
    { 0x042f, GLYPH_afii10049 },
    { 0x0430, GLYPH_afii10065 },
    { 0x0431, GLYPH_afii10066 },
    { 0x0432, GLYPH_afii10067 },
    { 0x0433, GLYPH_afii10068 },
    { 0x0434, GLYPH_afii10069 },
    { 0x0435, GLYPH_afii10070 },
    { 0x0436, GLYPH_afii10072 },
    { 0x0437, GLYPH_afii10073 },
    { 0x0438, GLYPH_afii10074 },
    { 0x0439, GLYPH_afii10075 },
    { 0x043a, GLYPH_afii10076 },
    { 0x043b, GLYPH_afii10077 },
    { 0x043c, GLYPH_afii10078 },
    { 0x043d, GLYPH_afii10079 },
    { 0x043e, GLYPH_afii10080 },
    { 0x043f, GLYPH_afii10081 },
    { 0x0440, GLYPH_afii10082 },
    { 0x0441, GLYPH_afii10083 },
    { 0x0442, GLYPH_afii10084 },
    { 0x0443, GLYPH_afii10085 },
    { 0x0444, GLYPH_afii10086 },
    { 0x0445, GLYPH_afii10087 },
    { 0x0446, GLYPH_afii10088 },
    { 0x0447, GLYPH_afii10089 },
    { 0x0448, GLYPH_afii10090 },
    { 0x0449, GLYPH_afii10091 },
    { 0x044a, GLYPH_afii10092 },
    { 0x044b, GLYPH_afii10093 },
    { 0x044c, GLYPH_afii10094 },
    { 0x044d, GLYPH_afii10095 },
    { 0x044e, GLYPH_afii10096 },
    { 0x044f, GLYPH_afii10097 },
    { 0x0451, GLYPH_afii10071 },
    { 0x0452, GLYPH_afii10099 },
    { 0x0453, GLYPH_afii10100 },
    { 0x0454, GLYPH_afii10101 },
    { 0x0455, GLYPH_afii10102 },
    { 0x0456, GLYPH_afii10103 },
    { 0x0457, GLYPH_afii10104 },
    { 0x0458, GLYPH_afii10105 },
    { 0x0459, GLYPH_afii10106 },
    { 0x045a, GLYPH_afii10107 },
    { 0x045b, GLYPH_afii10108 },
    { 0x045c, GLYPH_afii10109 },
    { 0x045e, GLYPH_afii10110 },
    { 0x045f, GLYPH_afii10193 },
    { 0x0462, GLYPH_afii10146 },
    { 0x0463, GLYPH_afii10194 },
    { 0x0472, GLYPH_afii10147 },
    { 0x0473, GLYPH_afii10195 },
    { 0x0474, GLYPH_afii10148 },
    { 0x0475, GLYPH_afii10196 },
    { 0x0490, GLYPH_afii10050 },
    { 0x0491, GLYPH_afii10098 },
    { 0x04d9, GLYPH_afii10846 },
    { 0x05b0, GLYPH_afii57799 },
    { 0x05b1, GLYPH_afii57801 },
    { 0x05b2, GLYPH_afii57800 },
    { 0x05b3, GLYPH_afii57802 },
    { 0x05b4, GLYPH_afii57793 },
    { 0x05b5, GLYPH_afii57794 },
    { 0x05b6, GLYPH_afii57795 },
    { 0x05b7, GLYPH_afii57798 },
    { 0x05b8, GLYPH_afii57797 },
    { 0x05b9, GLYPH_afii57806 },
    { 0x05bb, GLYPH_afii57796 },
    { 0x05bc, GLYPH_afii57807 },
    { 0x05bd, GLYPH_afii57839 },
    { 0x05be, GLYPH_afii57645 },
    { 0x05bf, GLYPH_afii57841 },
    { 0x05c0, GLYPH_afii57842 },
    { 0x05c1, GLYPH_afii57804 },
    { 0x05c2, GLYPH_afii57803 },
    { 0x05c3, GLYPH_afii57658 },
    { 0x05d0, GLYPH_afii57664 },
    { 0x05d1, GLYPH_afii57665 },
    { 0x05d2, GLYPH_afii57666 },
    { 0x05d3, GLYPH_afii57667 },
    { 0x05d4, GLYPH_afii57668 },
    { 0x05d5, GLYPH_afii57669 },
    { 0x05d6, GLYPH_afii57670 },
    { 0x05d7, GLYPH_afii57671 },
    { 0x05d8, GLYPH_afii57672 },
    { 0x05d9, GLYPH_afii57673 },
    { 0x05da, GLYPH_afii57674 },
    { 0x05db, GLYPH_afii57675 },
    { 0x05dc, GLYPH_afii57676 },
    { 0x05dd, GLYPH_afii57677 },
    { 0x05de, GLYPH_afii57678 },
    { 0x05df, GLYPH_afii57679 },
    { 0x05e0, GLYPH_afii57680 },
    { 0x05e1, GLYPH_afii57681 },
    { 0x05e2, GLYPH_afii57682 },
    { 0x05e3, GLYPH_afii57683 },
    { 0x05e4, GLYPH_afii57684 },
    { 0x05e5, GLYPH_afii57685 },
    { 0x05e6, GLYPH_afii57686 },
    { 0x05e7, GLYPH_afii57687 },
    { 0x05e8, GLYPH_afii57688 },
    { 0x05e9, GLYPH_afii57689 },
    { 0x05ea, GLYPH_afii57690 },
    { 0x05f0, GLYPH_afii57716 },
    { 0x05f1, GLYPH_afii57717 },
    { 0x05f2, GLYPH_afii57718 },
    { 0x060c, GLYPH_afii57388 },
    { 0x061b, GLYPH_afii57403 },
    { 0x061f, GLYPH_afii57407 },
    { 0x0621, GLYPH_afii57409 },
    { 0x0622, GLYPH_afii57410 },
    { 0x0623, GLYPH_afii57411 },
    { 0x0624, GLYPH_afii57412 },
    { 0x0625, GLYPH_afii57413 },
    { 0x0626, GLYPH_afii57414 },
    { 0x0627, GLYPH_afii57415 },
    { 0x0628, GLYPH_afii57416 },
    { 0x0629, GLYPH_afii57417 },
    { 0x062a, GLYPH_afii57418 },
    { 0x062b, GLYPH_afii57419 },
    { 0x062c, GLYPH_afii57420 },
    { 0x062d, GLYPH_afii57421 },
    { 0x062e, GLYPH_afii57422 },
    { 0x062f, GLYPH_afii57423 },
    { 0x0630, GLYPH_afii57424 },
    { 0x0631, GLYPH_afii57425 },
    { 0x0632, GLYPH_afii57426 },
    { 0x0633, GLYPH_afii57427 },
    { 0x0634, GLYPH_afii57428 },
    { 0x0635, GLYPH_afii57429 },
    { 0x0636, GLYPH_afii57430 },
    { 0x0637, GLYPH_afii57431 },
    { 0x0638, GLYPH_afii57432 },
    { 0x0639, GLYPH_afii57433 },
    { 0x063a, GLYPH_afii57434 },
    { 0x0640, GLYPH_afii57440 },
    { 0x0641, GLYPH_afii57441 },
    { 0x0642, GLYPH_afii57442 },
    { 0x0643, GLYPH_afii57443 },
    { 0x0644, GLYPH_afii57444 },
    { 0x0645, GLYPH_afii57445 },
    { 0x0646, GLYPH_afii57446 },
    { 0x0647, GLYPH_afii57470 },
    { 0x0648, GLYPH_afii57448 },
    { 0x0649, GLYPH_afii57449 },
    { 0x064a, GLYPH_afii57450 },
    { 0x064b, GLYPH_afii57451 },
    { 0x064c, GLYPH_afii57452 },
    { 0x064d, GLYPH_afii57453 },
    { 0x064e, GLYPH_afii57454 },
    { 0x064f, GLYPH_afii57455 },
    { 0x0650, GLYPH_afii57456 },
    { 0x0651, GLYPH_afii57457 },
    { 0x0652, GLYPH_afii57458 },
    { 0x0660, GLYPH_afii57392 },
    { 0x0661, GLYPH_afii57393 },
    { 0x0662, GLYPH_afii57394 },
    { 0x0663, GLYPH_afii57395 },
    { 0x0664, GLYPH_afii57396 },
    { 0x0665, GLYPH_afii57397 },
    { 0x0666, GLYPH_afii57398 },
    { 0x0667, GLYPH_afii57399 },
    { 0x0668, GLYPH_afii57400 },
    { 0x0669, GLYPH_afii57401 },
    { 0x066a, GLYPH_afii57381 },
    { 0x066d, GLYPH_afii63167 },
    { 0x0679, GLYPH_afii57511 },
    { 0x067e, GLYPH_afii57506 },
    { 0x0686, GLYPH_afii57507 },
    { 0x0688, GLYPH_afii57512 },
    { 0x0691, GLYPH_afii57513 },
    { 0x0698, GLYPH_afii57508 },
    { 0x06a4, GLYPH_afii57505 },
    { 0x06af, GLYPH_afii57509 },
    { 0x06ba, GLYPH_afii57514 },
    { 0x06d2, GLYPH_afii57519 },
    { 0x06d5, GLYPH_afii57534 },
    { 0x1e80, GLYPH_Wgrave },
    { 0x1e81, GLYPH_wgrave },
    { 0x1e81, GLYPH_wgrave },
    { 0x1e82, GLYPH_Wacute },
    { 0x1e83, GLYPH_wacute },
    { 0x1e83, GLYPH_wacute },
    { 0x1e84, GLYPH_Wdieresis },
    { 0x1e85, GLYPH_wdieresis },
    { 0x1e85, GLYPH_wdieresis },
    { 0x1ef2, GLYPH_Ygrave },
    { 0x1ef3, GLYPH_ygrave },
    { 0x1ef3, GLYPH_ygrave },
    { 0x200c, GLYPH_afii61664 },
    { 0x200d, GLYPH_afii301 },
    { 0x200e, GLYPH_afii299 },
    { 0x200f, GLYPH_afii300 },
    { 0x2012, GLYPH_figuredash },
    { 0x2013, GLYPH_endash },
    { 0x2014, GLYPH_emdash },
    { 0x2015, GLYPH_afii00208 },
    { 0x2017, GLYPH_underscoredbl },
    { 0x2018, GLYPH_quoteleft },
    { 0x2019, GLYPH_quoteright },
    { 0x201a, GLYPH_quotesinglbase },
    { 0x201b, GLYPH_quotereversed },
    { 0x201c, GLYPH_quotedblleft },
    { 0x201d, GLYPH_quotedblright },
    { 0x201e, GLYPH_quotedblbase },
    { 0x2020, GLYPH_dagger },
    { 0x2021, GLYPH_daggerdbl },
    { 0x2022, GLYPH_bullet },
    { 0x2024, GLYPH_onedotenleader },
    { 0x2025, GLYPH_twodotenleader },
    { 0x2026, GLYPH_ellipsis },
    { 0x202c, GLYPH_afii61573 },
    { 0x202d, GLYPH_afii61574 },
    { 0x202e, GLYPH_afii61575 },
    { 0x2030, GLYPH_perthousand },
    { 0x2032, GLYPH_minute },
    { 0x2033, GLYPH_second },
    { 0x2039, GLYPH_guilsinglleft },
    { 0x203a, GLYPH_guilsinglright },
    { 0x203c, GLYPH_exclamdbl },
    { 0x2044, GLYPH_fraction },
    { 0x20a1, GLYPH_colonmonetary },
    { 0x20a3, GLYPH_franc },
    { 0x20a4, GLYPH_lira },
    { 0x20a7, GLYPH_peseta },
    { 0x20aa, GLYPH_afii57636 },
    { 0x20ab, GLYPH_dong },
    { 0x20ac, GLYPH_Euro },
    { 0x2105, GLYPH_afii61248 },
    { 0x2111, GLYPH_Ifraktur },
    { 0x2113, GLYPH_afii61289 },
    { 0x2116, GLYPH_afii61352 },
    { 0x2118, GLYPH_weierstrass },
    { 0x2118, GLYPH_weierstrass },
    { 0x211c, GLYPH_Rfraktur },
    { 0x211e, GLYPH_prescription },
    { 0x2122, GLYPH_trademark },
    { 0x2126, GLYPH_Omega },
    { 0x212e, GLYPH_estimated },
    { 0x2135, GLYPH_aleph },
    { 0x2153, GLYPH_onethird },
    { 0x2154, GLYPH_twothirds },
    { 0x215b, GLYPH_oneeighth },
    { 0x215c, GLYPH_threeeighths },
    { 0x215d, GLYPH_fiveeighths },
    { 0x215e, GLYPH_seveneighths },
    { 0x2190, GLYPH_arrowleft },
    { 0x2191, GLYPH_arrowup },
    { 0x2192, GLYPH_arrowright },
    { 0x2193, GLYPH_arrowdown },
    { 0x2194, GLYPH_arrowboth },
    { 0x2195, GLYPH_arrowupdn },
    { 0x21a8, GLYPH_arrowupdnbse },
    { 0x21b5, GLYPH_carriagereturn },
    { 0x21d0, GLYPH_arrowdblleft },
    { 0x21d1, GLYPH_arrowdblup },
    { 0x21d2, GLYPH_arrowdblright },
    { 0x21d3, GLYPH_arrowdbldown },
    { 0x21d4, GLYPH_arrowdblboth },
    { 0x2200, GLYPH_universal },
    { 0x2202, GLYPH_partialdiff },
    { 0x2203, GLYPH_existential },
    { 0x2205, GLYPH_emptyset },
    { 0x2206, GLYPH_Delta },
    { 0x2207, GLYPH_gradient },
    { 0x2208, GLYPH_element },
    { 0x2209, GLYPH_notelement },
    { 0x220b, GLYPH_suchthat },
    { 0x220f, GLYPH_product },
    { 0x2211, GLYPH_summation },
    { 0x2212, GLYPH_minus },
    { 0x2217, GLYPH_asteriskmath },
    { 0x221a, GLYPH_radical },
    { 0x221d, GLYPH_proportional },
    { 0x221e, GLYPH_infinity },
    { 0x221f, GLYPH_orthogonal },
    { 0x2220, GLYPH_angle },
    { 0x2227, GLYPH_logicaland },
    { 0x2228, GLYPH_logicalor },
    { 0x2229, GLYPH_intersection },
    { 0x222a, GLYPH_union },
    { 0x222b, GLYPH_integral },
    { 0x2234, GLYPH_therefore },
    { 0x223c, GLYPH_similar },
    { 0x2245, GLYPH_congruent },
    { 0x2248, GLYPH_approxequal },
    { 0x2260, GLYPH_notequal },
    { 0x2261, GLYPH_equivalence },
    { 0x2264, GLYPH_lessequal },
    { 0x2265, GLYPH_greaterequal },
    { 0x2282, GLYPH_propersubset },
    { 0x2283, GLYPH_propersuperset },
    { 0x2284, GLYPH_notsubset },
    { 0x2286, GLYPH_reflexsubset },
    { 0x2287, GLYPH_reflexsuperset },
    { 0x2295, GLYPH_circleplus },
    { 0x2297, GLYPH_circlemultiply },
    { 0x22a5, GLYPH_perpendicular },
    { 0x22c5, GLYPH_dotmath },
    { 0x2302, GLYPH_house },
    { 0x2310, GLYPH_revlogicalnot },
    { 0x2320, GLYPH_integraltp },
    { 0x2321, GLYPH_integralbt },
    { 0x2329, GLYPH_angleleft },
    { 0x232a, GLYPH_angleright },
    { 0x2500, GLYPH_SF100000 },
    { 0x2502, GLYPH_SF110000 },
    { 0x250c, GLYPH_SF010000 },
    { 0x2510, GLYPH_SF030000 },
    { 0x2514, GLYPH_SF020000 },
    { 0x2518, GLYPH_SF040000 },
    { 0x251c, GLYPH_SF080000 },
    { 0x2524, GLYPH_SF090000 },
    { 0x252c, GLYPH_SF060000 },
    { 0x2534, GLYPH_SF070000 },
    { 0x253c, GLYPH_SF050000 },
    { 0x2550, GLYPH_SF430000 },
    { 0x2551, GLYPH_SF240000 },
    { 0x2552, GLYPH_SF510000 },
    { 0x2553, GLYPH_SF520000 },
    { 0x2554, GLYPH_SF390000 },
    { 0x2555, GLYPH_SF220000 },
    { 0x2556, GLYPH_SF210000 },
    { 0x2557, GLYPH_SF250000 },
    { 0x2558, GLYPH_SF500000 },
    { 0x2559, GLYPH_SF490000 },
    { 0x255a, GLYPH_SF380000 },
    { 0x255b, GLYPH_SF280000 },
    { 0x255c, GLYPH_SF270000 },
    { 0x255d, GLYPH_SF260000 },
    { 0x255e, GLYPH_SF360000 },
    { 0x255f, GLYPH_SF370000 },
    { 0x2560, GLYPH_SF420000 },
    { 0x2561, GLYPH_SF190000 },
    { 0x2562, GLYPH_SF200000 },
    { 0x2563, GLYPH_SF230000 },
    { 0x2564, GLYPH_SF470000 },
    { 0x2565, GLYPH_SF480000 },
    { 0x2566, GLYPH_SF410000 },
    { 0x2567, GLYPH_SF450000 },
    { 0x2568, GLYPH_SF460000 },
    { 0x2569, GLYPH_SF400000 },
    { 0x256a, GLYPH_SF540000 },
    { 0x256b, GLYPH_SF530000 },
    { 0x256c, GLYPH_SF440000 },
    { 0x2580, GLYPH_upblock },
    { 0x2584, GLYPH_dnblock },
    { 0x2588, GLYPH_block },
    { 0x258c, GLYPH_lfblock },
    { 0x2590, GLYPH_rtblock },
    { 0x2591, GLYPH_ltshade },
    { 0x2592, GLYPH_shade },
    { 0x2593, GLYPH_dkshade },
    { 0x25a0, GLYPH_filledbox },
    { 0x25a1, GLYPH_H22073 },
    { 0x25aa, GLYPH_H18543 },
    { 0x25ab, GLYPH_H18551 },
    { 0x25ac, GLYPH_filledrect },
    { 0x25b2, GLYPH_triagup },
    { 0x25ba, GLYPH_triagrt },
    { 0x25bc, GLYPH_triagdn },
    { 0x25c4, GLYPH_triaglf },
    { 0x25ca, GLYPH_lozenge },
    { 0x25cb, GLYPH_circle },
    { 0x25cf, GLYPH_H18533 },
    { 0x25d8, GLYPH_invbullet },
    { 0x25d9, GLYPH_invcircle },
    { 0x25e6, GLYPH_openbullet },
    { 0x263a, GLYPH_smileface },
    { 0x263b, GLYPH_invsmileface },
    { 0x263c, GLYPH_sun },
    { 0x2640, GLYPH_female },
    { 0x2642, GLYPH_male },
    { 0x2660, GLYPH_spade },
    { 0x2663, GLYPH_club },
    { 0x2665, GLYPH_heart },
    { 0x2666, GLYPH_diamond },
    { 0x266a, GLYPH_musicalnote },
    { 0x266b, GLYPH_musicalnotedbl },
};

const char * psUnicodeToGlyphName(	int	unicode )
    {
    static char	scratch[20];

    int		l= 0;
    int		r= sizeof(psUnicodeToGlyphNames)/sizeof(UnicodeToGlyphName);
    int		m= ( l+ r )/ 2;

    while( m != l )
	{
	if  ( psUnicodeToGlyphNames[m].utgUnicode > unicode )
	    { r= m;	}
	else{ l= m;	}

	m= ( l+ r )/ 2;
	}

    if  ( psUnicodeToGlyphNames[m].utgUnicode == unicode )
	{ return  psUnicodeToGlyphNames[m].utgGlyphName;	}

    sprintf( scratch, "uni%04x", unicode );

    return scratch;
    }

/************************************************************************/
/*									*/
/*  Standard apple encoding as used in a true type file.		*/
/*									*/
/************************************************************************/

static const char GLYPH_nonmarkingreturn[]= "nonmarkingreturn";
static const char GLYPH_fi[]= "fi";
static const char GLYPH_fl[]= "fl";
static const char GLYPH_apple[]= "apple";

const char * const psAppleTtfGlyphNames[258]=
{
    ".notdef",
    ".null",
    GLYPH_nonmarkingreturn,
    GLYPH_space,
    GLYPH_exclam,
    GLYPH_quotedbl,
    GLYPH_numbersign,
    GLYPH_dollar,
    GLYPH_percent,
    GLYPH_ampersand,
    GLYPH_quotesingle,
    GLYPH_parenleft,
    GLYPH_parenright,
    GLYPH_asterisk,
    GLYPH_plus,
    GLYPH_comma,
    GLYPH_hyphen,
    GLYPH_period,
    GLYPH_slash,
    GLYPH_zero,
    GLYPH_one,
    GLYPH_two,
    GLYPH_three,
    GLYPH_four,
    GLYPH_five,
    GLYPH_six,
    GLYPH_seven,
    GLYPH_eight,
    GLYPH_nine,
    GLYPH_colon,
    GLYPH_semicolon,
    GLYPH_less,
    GLYPH_equal,
    GLYPH_greater,
    GLYPH_question,
    GLYPH_at,
    GLYPH_A,
    GLYPH_B,
    GLYPH_C,
    GLYPH_D,
    GLYPH_E,
    GLYPH_F,
    GLYPH_G,
    GLYPH_H,
    GLYPH_I,
    GLYPH_J,
    GLYPH_K,
    GLYPH_L,
    GLYPH_M,
    GLYPH_N,
    GLYPH_O,
    GLYPH_P,
    GLYPH_Q,
    GLYPH_R,
    GLYPH_S,
    GLYPH_T,
    GLYPH_U,
    GLYPH_V,
    GLYPH_W,
    GLYPH_X,
    GLYPH_Y,
    GLYPH_Z,
    GLYPH_bracketleft,
    GLYPH_backslash,
    GLYPH_bracketright,
    GLYPH_asciicircum,
    GLYPH_underscore,
    GLYPH_grave,
    GLYPH_a,
    GLYPH_b,
    GLYPH_c,
    GLYPH_d,
    GLYPH_e,
    GLYPH_f,
    GLYPH_g,
    GLYPH_h,
    GLYPH_i,
    GLYPH_j,
    GLYPH_k,
    GLYPH_l,
    GLYPH_m,
    GLYPH_n,
    GLYPH_o,
    GLYPH_p,
    GLYPH_q,
    GLYPH_r,
    GLYPH_s,
    GLYPH_t,
    GLYPH_u,
    GLYPH_v,
    GLYPH_w,
    GLYPH_x,
    GLYPH_y,
    GLYPH_z,
    GLYPH_braceleft,
    GLYPH_bar,
    GLYPH_braceright,
    GLYPH_asciitilde,
    GLYPH_Adieresis,
    GLYPH_Aring,
    GLYPH_Ccedilla,
    GLYPH_Eacute,
    GLYPH_Ntilde,
    GLYPH_Odieresis,
    GLYPH_Udieresis,
    GLYPH_aacute,
    GLYPH_agrave,
    GLYPH_acircumflex,
    GLYPH_adieresis,
    GLYPH_atilde,
    GLYPH_aring,
    GLYPH_ccedilla,
    GLYPH_eacute,
    GLYPH_egrave,
    GLYPH_ecircumflex,
    GLYPH_edieresis,
    GLYPH_iacute,
    GLYPH_igrave,
    GLYPH_icircumflex,
    GLYPH_idieresis,
    GLYPH_ntilde,
    GLYPH_oacute,
    GLYPH_ograve,
    GLYPH_ocircumflex,
    GLYPH_odieresis,
    GLYPH_otilde,
    GLYPH_uacute,
    GLYPH_ugrave,
    GLYPH_ucircumflex,
    GLYPH_udieresis,
    GLYPH_dagger,
    GLYPH_degree,
    GLYPH_cent,
    GLYPH_sterling,
    GLYPH_section,
    GLYPH_bullet,
    GLYPH_paragraph,
    GLYPH_germandbls,
    GLYPH_registered,
    GLYPH_copyright,
    GLYPH_trademark,
    GLYPH_acute,
    GLYPH_dieresis,
    GLYPH_notequal,
    GLYPH_AE,
    GLYPH_Oslash,
    GLYPH_infinity,
    GLYPH_plusminus,
    GLYPH_lessequal,
    GLYPH_greaterequal,
    GLYPH_yen,
    GLYPH_mu,
    GLYPH_partialdiff,
    GLYPH_summation,
    GLYPH_product,
    GLYPH_pi,
    GLYPH_integral,
    GLYPH_ordfeminine,
    GLYPH_ordmasculine,
    GLYPH_Omega,
    GLYPH_ae,
    GLYPH_oslash,
    GLYPH_questiondown,
    GLYPH_exclamdown,
    GLYPH_logicalnot,
    GLYPH_radical,
    GLYPH_florin,
    GLYPH_approxequal,
    GLYPH_Delta,
    GLYPH_guillemotleft,
    GLYPH_guillemotright,
    GLYPH_ellipsis,
    GLYPH_nonbreakingspace,
    GLYPH_Agrave,
    GLYPH_Atilde,
    GLYPH_Otilde,
    GLYPH_OE,
    GLYPH_oe,
    GLYPH_endash,
    GLYPH_emdash,
    GLYPH_quotedblleft,
    GLYPH_quotedblright,
    GLYPH_quoteleft,
    GLYPH_quoteright,
    GLYPH_divide,
    GLYPH_lozenge,
    GLYPH_ydieresis,
    GLYPH_Ydieresis,
    GLYPH_fraction,
    GLYPH_currency,
    GLYPH_guilsinglleft,
    GLYPH_guilsinglright,
    GLYPH_fi,
    GLYPH_fl,
    GLYPH_daggerdbl,
    GLYPH_periodcentered,
    GLYPH_quotesinglbase,
    GLYPH_quotedblbase,
    GLYPH_perthousand,
    GLYPH_Acircumflex,
    GLYPH_Ecircumflex,
    GLYPH_Aacute,
    GLYPH_Edieresis,
    GLYPH_Egrave,
    GLYPH_Iacute,
    GLYPH_Icircumflex,
    GLYPH_Idieresis,
    GLYPH_Igrave,
    GLYPH_Oacute,
    GLYPH_Ocircumflex,
    GLYPH_apple,
    GLYPH_Ograve,
    GLYPH_Uacute,
    GLYPH_Ucircumflex,
    GLYPH_Ugrave,
    GLYPH_dotlessi,
    GLYPH_circumflex,
    GLYPH_tilde,
    GLYPH_macron,
    GLYPH_breve,
    GLYPH_dotaccent,
    GLYPH_ring,
    GLYPH_cedilla,
    GLYPH_hungarumlaut,
    GLYPH_ogonek,
    GLYPH_caron,
    GLYPH_Lslash,
    GLYPH_lslash,
    GLYPH_Scaron,
    GLYPH_scaron,
    GLYPH_Zcaron,
    GLYPH_zcaron,
    GLYPH_brokenbar,
    GLYPH_Eth,
    GLYPH_eth,
    GLYPH_Yacute,
    GLYPH_yacute,
    GLYPH_Thorn,
    GLYPH_thorn,
    GLYPH_minus,
    GLYPH_multiply,
    GLYPH_onesuperior,
    GLYPH_twosuperior,
    GLYPH_threesuperior,
    GLYPH_onehalf,
    GLYPH_onequarter,
    GLYPH_threequarters,
    GLYPH_franc,
    GLYPH_Gbreve,
    GLYPH_gbreve,
    GLYPH_Idotaccent,
    GLYPH_Scedilla,
    GLYPH_scedilla,
    GLYPH_Cacute,
    GLYPH_cacute,
    GLYPH_Ccaron,
    GLYPH_ccaron,
    GLYPH_dcroat,
};

