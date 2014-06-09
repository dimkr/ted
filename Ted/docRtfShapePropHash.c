#   include <appUtilConfig.h>

#   include <utilJenkinsHash.h>

#   include "docRtfTagHash.h"

#   define ub4 UtilUint32
#   define ub2 UtilUint16
#   define ub1 unsigned char

#   define CHECKSTATE 8

/* smax= 512, blen= 256 */

/* small adjustments to _a_ to make values distinct */
static const ub2 tab[] = {
469,0,0,0,28,65,154,469,243,468,162,65,0,65,468,230,
230,258,0,258,0,0,258,0,404,154,28,468,65,0,325,36,
0,0,36,0,468,154,0,258,0,6,0,154,258,469,404,154,
328,471,324,395,325,28,243,28,36,154,230,0,28,243,481,258,
154,62,0,0,28,468,258,258,395,395,0,258,258,0,6,0,
0,468,0,234,258,0,0,243,28,0,0,243,468,0,154,0,
0,157,6,0,469,258,0,462,404,154,258,243,327,468,6,162,
324,162,0,479,258,162,154,162,260,28,324,469,243,162,0,28,
230,468,6,319,319,0,264,258,162,471,0,26,28,0,328,65,
328,0,324,0,258,328,146,65,243,258,468,162,157,0,0,162,
0,481,234,0,419,243,0,264,230,6,154,162,0,258,0,154,
154,157,325,65,324,243,399,204,275,325,28,484,28,462,146,0,
184,258,154,243,469,184,419,234,328,0,0,162,0,146,243,0,
258,162,256,0,0,258,345,484,194,243,258,352,468,0,327,243,
329,28,154,258,0,337,171,317,258,277,154,0,379,471,419,161,
258,343,0,0,324,154,468,288,270,365,260,499,328,0,0,146,
};

/* map back array */

static int mapBack[]=
    {
     348L,	/*     0: "lineColumnBackColorExtK" */
     264L,	/*     1: "pictureFillCrMod" */
     215L,	/*     2: "gtextFUnderline" */
     455L,	/*     3: "lineTopType" */
      33L,	/*     4: "fHidden" */
     268L,	/*     5: "shapePath" */
     262L,	/*     6: "pictureBrightness" */
     211L,	/*     7: "gtextFNormalize" */
     387L,	/*     8: "lineLeftEndArrowhead" */
     285L,	/*     9: "c3DExtrudePlane" */
      44L,	/*    10: "fNoLineDrawDash" */
     454L,	/*    11: "lineTopStyle" */
     230L,	/*    12: "c3DRotationAxisY" */
     103L,	/*    13: "lineBottomFillShape" */
     315L,	/*    14: "lineBottomBackColorExtCMY" */
     289L,	/*    15: "c3DOriginY" */
     203L,	/*    16: "gtextAlign" */
      12L,	/*    17: "posrelh" */
     196L,	/*    18: "fshadowObscured" */
     307L,	/*    19: "fillColorExtK" */
     372L,	/*    20: "lineColumnWidth" */
     350L,	/*    21: "lineColumnColor" */
     337L,	/*    22: "lineBottomStartArrowhead" */
     445L,	/*    23: "lineTopFillBlipFlags" */
     249L,	/*    24: "dxyCalloutGap" */
     133L,	/*    25: "fillToBottom" */
      69L,	/*    26: "lineFillHeight" */
      50L,	/*    27: "fPseudoInline" */
     459L,	/*    28: "shadowColorExtK" */
     446L,	/*    29: "lineTopFillDztype" */
     257L,	/*    30: "fCalloutDropAuto" */
     172L,	/*    31: "lidRegroup" */
     388L,	/*    32: "lineLeftEndCapStyle" */
     161L,	/*    33: "adjust5Value" */
      24L,	/*    34: "hspMaster" */
     163L,	/*    35: "adjust7Value" */
     176L,	/*    36: "groupBottom" */
     429L,	/*    37: "lineTopBackColor" */
      51L,	/*    38: "dxTextLeft" */
     199L,	/*    39: "shadowColor" */
     423L,	/*    40: "lineRightStartArrowLength" */
      11L,	/*    41: "posrelv" */
     170L,	/*    42: "relBottom" */
     240L,	/*    43: "c3DFillX" */
     319L,	/*    44: "lineBottomColorExt" */
     174L,	/*    45: "groupTop" */
     358L,	/*    46: "lineColumnEndArrowWidth" */
     361L,	/*    47: "lineColumnFillBlipFlags" */
      66L,	/*    48: "lineType" */
     122L,	/*    49: "fillType" */
     206L,	/*    50: "gtextFKern" */
       6L,	/*    51: "pibName" */
     290L,	/*    52: "c3DRenderMode" */
     408L,	/*    53: "lineRightColorExtCMY" */
     322L,	/*    54: "lineBottomColorExtMod" */
     302L,	/*    55: "c3DExtrusionColorExtCMY" */
     349L,	/*    56: "lineColumnBackColorExtMod" */
      67L,	/*    57: "lineFillBlipFlags" */
     234L,	/*    58: "c3DXViewpoint" */
     309L,	/*    59: "lineBackColorExt" */
     224L,	/*    60: "c3DExtrudeBackward" */
     437L,	/*    61: "lineTopColorExtK" */
     390L,	/*    62: "lineLeftFillDztype" */
     329L,	/*    63: "lineBottomFillBlipFlags" */
     391L,	/*    64: "lineLeftFillHeight" */
     338L,	/*    65: "lineBottomStyle" */
      65L,	/*    66: "fFitTextToShape" */
     232L,	/*    67: "fC3DRotationCenterAut" */
     158L,	/*    68: "adjust2Value" */
      14L,	/*    69: "fLayoutInCell" */
     306L,	/*    70: "fillColorExtCMY" */
     205L,	/*    71: "gtextFVertical" */
     287L,	/*    72: "c3DKeyIntensity" */
     375L,	/*    73: "lineLeftBackColorExtCMY" */
     291L,	/*    74: "c3DRotationAngle" */
     261L,	/*    75: "pictureContrast" */
     190L,	/*    76: "shadowPerspectiveX" */
     298L,	/*    77: "c3DTolerance" */
      94L,	/*    78: "fLeftHitTestLine" */
     165L,	/*    79: "adjust9Value" */
     370L,	/*    80: "lineColumnStyle" */
     373L,	/*    81: "lineLeftBackColor" */
     401L,	/*    82: "lineRightBackColor" */
     107L,	/*    83: "fRightInsetPen" */
     247L,	/*    84: "fc3DRotationCenterAuto" */
     136L,	/*    85: "fillShapeOriginX" */
     460L,	/*    86: "shadowColorExtMod" */
     405L,	/*    87: "lineRightBackColorExtMod" */
     266L,	/*    88: "fCalloutMinusX" */
     280L,	/*    89: "dzColMargin" */
       8L,	/*    90: "pib" */
     448L,	/*    91: "lineTopFillWidth" */
     229L,	/*    92: "c3DRotationAxisX" */
     180L,	/*    93: "shadowType" */
     318L,	/*    94: "lineBottomColor" */
      26L,	/*    95: "xLimo" */
      86L,	/*    96: "fLineUseShapeAnchor" */
     248L,	/*    97: "spcot" */
      93L,	/*    98: "fColumnHitTestLine" */
      99L,	/*    99: "lineColumnFillShape" */
     146L,	/*   100: "fillShape" */
     378L,	/*   101: "lineLeftColor" */
     344L,	/*   102: "lineColorExtMod" */
     320L,	/*   103: "lineBottomColorExtCMY" */
      62L,	/*   104: "fRotateText" */
     395L,	/*   105: "lineLeftStartArrowLength" */
     384L,	/*   106: "lineLeftDashing" */
      42L,	/*   107: "fNoFillHitTest" */
      63L,	/*   108: "fSelectText" */
     303L,	/*   109: "c3DExtrusionColorExtK" */
     179L,	/*   110: "fRelFlipV" */
     127L,	/*   111: "fillHeight" */
     433L,	/*   112: "lineTopBackColorExtMod" */
     374L,	/*   113: "lineLeftBackColorExt" */
     385L,	/*   114: "lineLeftEndArrowLength" */
     330L,	/*   115: "lineBottomFillDztype" */
     312L,	/*   116: "lineBackColorExtMod" */
     357L,	/*   117: "lineColumnEndArrowLength" */
     155L,	/*   118: "geoRight" */
     394L,	/*   119: "lineLeftOpacity" */
     352L,	/*   120: "lineColumnColorExtCMY" */
     260L,	/*   121: "pictureActive" */
     355L,	/*   122: "lineColumnCrMod" */
      40L,	/*   123: "fHitTestLine" */
     428L,	/*   124: "lineRightWidth" */
     461L,	/*   125: "shadowHighlightExt" */
      38L,	/*   126: "fEditedWrap" */
     293L,	/*   127: "c3DRotationCenterY" */
     386L,	/*   128: "lineLeftEndArrowWidth" */
     299L,	/*   129: "c3DXRotationAngle" */
     223L,	/*   130: "c3DExtrudeForward" */
     252L,	/*   131: "dxyCalloutDropSpecified" */
       4L,	/*   132: "gtextUNICODE" */
     347L,	/*   133: "lineColumnBackColorExtCMY" */
     117L,	/*   134: "fLockText" */
     119L,	/*   135: "fLockAgainstGrouping" */
     450L,	/*   136: "lineTopOpacity" */
      59L,	/*   137: "txflTextFlow" */
     231L,	/*   138: "c3DRotationAxisZ" */
     110L,	/*   139: "lineBackColor" */
     436L,	/*   140: "lineTopColorExtCMY" */
       7L,	/*   141: "fillBlipName" */
      75L,	/*   142: "lineStartArrowWidth" */
     466L,	/*   143: "c3DExtrusionColor" */
     409L,	/*   144: "lineRightColorExtK" */
     339L,	/*   145: "lineBottomType" */
     160L,	/*   146: "adjust4Value" */
     331L,	/*   147: "lineBottomFillHeight" */
     225L,	/*   148: "f3D" */
     323L,	/*   149: "lineBottomCrMod" */
     340L,	/*   150: "lineBottomWidth" */
     151L,	/*   151: "fillBackColor" */
     183L,	/*   152: "shadowOffsetY" */
     314L,	/*   153: "lineBottomBackColorExt" */
      98L,	/*   154: "lineFillShape" */
     228L,	/*   155: "fc3DLightFace" */
     369L,	/*   156: "lineColumnStartArrowhead" */
     434L,	/*   157: "lineTopColor" */
     443L,	/*   158: "lineTopEndArrowhead" */
     422L,	/*   159: "lineRightOpacity" */
     194L,	/*   160: "shadowOriginY" */
       1L,	/*   161: "pWrapPolygonVertices" */
     411L,	/*   162: "lineRightCrMod" */
     400L,	/*   163: "lineLeftWidth" */
     255L,	/*   164: "fCalloutAccentBar" */
     321L,	/*   165: "lineBottomColorExtK" */
     157L,	/*   166: "adjustValue" */
     362L,	/*   167: "lineColumnFillDztype" */
     131L,	/*   168: "fillToTop" */
     178L,	/*   169: "fRelFlipH" */
      89L,	/*   170: "fLeftLine" */
     343L,	/*   171: "lineColorExtK" */
     208L,	/*   172: "gtextFStretch" */
      80L,	/*   173: "lineFillDztype" */
     115L,	/*   174: "fLockCropping" */
       0L,	/*   175: "pVerticies" */
     406L,	/*   176: "lineRightColor" */
      35L,	/*   177: "fArrowheadsOK" */
      34L,	/*   178: "fReallyHidden" */
     389L,	/*   179: "lineLeftFillBlipFlags" */
     426L,	/*   180: "lineRightStyle" */
     297L,	/*   181: "c3DSpecularAmt" */
      71L,	/*   182: "lineStyle" */
      16L,	/*   183: "fChangePage" */
     425L,	/*   184: "lineRightStartArrowhead" */
     128L,	/*   185: "fillAngle" */
     235L,	/*   186: "c3DYViewpoint" */
     365L,	/*   187: "lineColumnMiterLimit" */
     311L,	/*   188: "lineBackColorExtK" */
      91L,	/*   189: "fRightLine" */
     171L,	/*   190: "relRotation" */
     204L,	/*   191: "fGtext" */
     237L,	/*   192: "c3DKeyX" */
      53L,	/*   193: "dxTextRight" */
     444L,	/*   194: "lineTopEndCapStyle" */
     169L,	/*   195: "relRight" */
     246L,	/*   196: "fc3DConstrainRotation" */
     412L,	/*   197: "lineRightDashing" */
      60L,	/*   198: "cdirFont" */
     243L,	/*   199: "fc3DParallel" */
     464L,	/*   200: "shadowHighlightExtMod" */
     447L,	/*   201: "lineTopFillHeight" */
     173L,	/*   202: "groupLeft" */
     105L,	/*   203: "fLeftInsetPen" */
     278L,	/*   204: "dgmt" */
     333L,	/*   205: "lineBottomMiterLimit" */
     356L,	/*   206: "lineColumnDashing" */
     226L,	/*   207: "fc3DMetallic" */
     187L,	/*   208: "shadowScaleYToX" */
     123L,	/*   209: "fillOpacity" */
     419L,	/*   210: "lineRightFillHeight" */
     416L,	/*   211: "lineRightEndCapStyle" */
     341L,	/*   212: "lineColorExt" */
     396L,	/*   213: "lineLeftStartArrowWidth" */
     273L,	/*   214: "cropFromBottom" */
     233L,	/*   215: "c3DRotationCenterZ" */
      13L,	/*   216: "fUseShapeAnchor" */
     138L,	/*   217: "fillDztype" */
     325L,	/*   218: "lineBottomEndArrowLength" */
     167L,	/*   219: "relLeft" */
     191L,	/*   220: "shadowPerspectiveY" */
     335L,	/*   221: "lineBottomStartArrowLength" */
     100L,	/*   222: "lineLeftFillShape" */
       9L,	/*   223: "posv" */
     222L,	/*   224: "c3DEdgeThickness" */
     121L,	/*   225: "fLockPosition" */
     271L,	/*   226: "cropFromRight" */
     359L,	/*   227: "lineColumnEndArrowhead" */
     148L,	/*   228: "fFillOK" */
     239L,	/*   229: "c3DKeyZ" */
      54L,	/*   230: "dyTextBottom" */
     270L,	/*   231: "cropFromLeft" */
     258L,	/*   232: "fCalloutLengthSpecified" */
     353L,	/*   233: "lineColumnColorExtK" */
      84L,	/*   234: "fline" */
     463L,	/*   235: "shadowHighlightExtK" */
     241L,	/*   236: "c3DFillY" */
      43L,	/*   237: "fNoHitTestPicture" */
     112L,	/*   238: "fLockRotation" */
     162L,	/*   239: "adjust6Value" */
     150L,	/*   240: "fillColor" */
     111L,	/*   241: "lineCrMod" */
     220L,	/*   242: "pictureGray" */
     301L,	/*   243: "c3DExtrusionColorExt" */
      87L,	/*   244: "fColumnLineOK" */
     453L,	/*   245: "lineTopStartArrowhead" */
     166L,	/*   246: "adjust10Value" */
     274L,	/*   247: "bWMode" */
     201L,	/*   248: "cxk" */
      97L,	/*   249: "fBottomHitTestLine" */
     250L,	/*   250: "spcoa" */
      23L,	/*   251: "dyWrapDistBottom" */
     137L,	/*   252: "fillShapeOriginY" */
     381L,	/*   253: "lineLeftColorExtK" */
     134L,	/*   254: "fillOriginX" */
     145L,	/*   255: "fFilled" */
      15L,	/*   256: "fAllowOverlap" */
     414L,	/*   257: "lineRightEndArrowWidth" */
     276L,	/*   258: "fPreferRelativeResize" */
      92L,	/*   259: "fBottomLine" */
     238L,	/*   260: "c3DKeyY" */
      55L,	/*   261: "scaleText" */
      70L,	/*   262: "lineWidth" */
      20L,	/*   263: "dxWrapDistLeft" */
     197L,	/*   264: "fShadowOK" */
     364L,	/*   265: "lineColumnFillWidth" */
     379L,	/*   266: "lineLeftColorExt" */
     316L,	/*   267: "lineBottomBackColorExtK" */
     144L,	/*   268: "fillShadeType" */
     413L,	/*   269: "lineRightEndArrowLength" */
     332L,	/*   270: "lineBottomFillWidth" */
      95L,	/*   271: "fTopHitTestLine" */
      27L,	/*   272: "yLimo" */
     269L,	/*   273: "lineOpacity" */
      18L,	/*   274: "ShapeType" */
     465L,	/*   275: "pictureTransparent" */
      36L,	/*   276: "fBackground" */
     376L,	/*   277: "lineLeftBackColorExtK" */
     440L,	/*   278: "lineTopDashing" */
      82L,	/*   279: "lineMiterLimit" */
      25L,	/*   280: "hspNext" */
     456L,	/*   281: "lineTopWidth" */
     210L,	/*   282: "gtextFBestFit" */
     207L,	/*   283: "gtextFTight" */
     380L,	/*   284: "lineLeftColorExtCMY" */
     198L,	/*   285: "shadowCrMod" */
     392L,	/*   286: "lineLeftFillWidth" */
     129L,	/*   287: "fillFocus" */
     457L,	/*   288: "shadowColorExt" */
     164L,	/*   289: "adjust8Value" */
     185L,	/*   290: "shadowSecondOffsetY" */
     265L,	/*   291: "pictureLineCrMod" */
     326L,	/*   292: "lineBottomEndArrowWidth" */
      48L,	/*   293: "fPreferRelativeSize" */
     407L,	/*   294: "lineRightColorExt" */
     308L,	/*   295: "fillColorExtMod" */
     462L,	/*   296: "shadowHighlightExtCMY" */
     259L,	/*   297: "pictureId" */
     256L,	/*   298: "fCalloutTextBorder" */
     402L,	/*   299: "lineRightBackColorExt" */
     156L,	/*   300: "geoBottom" */
     399L,	/*   301: "lineLeftType" */
      30L,	/*   302: "fFlipH" */
      58L,	/*   303: "anchorText" */
     242L,	/*   304: "c3DFillZ" */
      37L,	/*   305: "fDeleteAttachedObject" */
     140L,	/*   306: "fillRectTop" */
      61L,	/*   307: "fAutoTextMargin" */
     435L,	/*   308: "lineTopColorExt" */
      74L,	/*   309: "lineEndArrowhead" */
      83L,	/*   310: "fLine" */
     354L,	/*   311: "lineColumnColorExtMod" */
     218L,	/*   312: "gtextFStrikethrough" */
     451L,	/*   313: "lineTopStartArrowLength" */
      52L,	/*   314: "dyTextTop" */
     135L,	/*   315: "fillOriginY" */
     403L,	/*   316: "lineRightBackColorExtCMY" */
     288L,	/*   317: "c3DOriginX" */
     263L,	/*   318: "pictureDblCrMod" */
      64L,	/*   319: "fFitShapeToText" */
     132L,	/*   320: "fillToRight" */
      79L,	/*   321: "lineEndCapStyle" */
     189L,	/*   322: "shadowScaleYToY" */
     236L,	/*   323: "c3DZViewpoint" */
     219L,	/*   324: "pibFlags" */
     377L,	/*   325: "lineLeftBackColorExtMod" */
      41L,	/*   326: "fInitiator" */
     404L,	/*   327: "lineRightBackColorExtK" */
     254L,	/*   328: "fCallout" */
     430L,	/*   329: "lineTopBackColorExt" */
      57L,	/*   330: "WrapText" */
     217L,	/*   331: "gtextFSmallcaps" */
       3L,	/*   332: "WzName" */
     192L,	/*   333: "shadowWeight" */
      90L,	/*   334: "fTopLine" */
     367L,	/*   335: "lineColumnStartArrowLength" */
      77L,	/*   336: "lineEndArrowWidth" */
     102L,	/*   337: "lineRightFillShape" */
     126L,	/*   338: "fillWidth" */
     118L,	/*   339: "fLockAdjustHandles" */
     175L,	/*   340: "groupRight" */
      96L,	/*   341: "fRightHitTestLine" */
     421L,	/*   342: "lineRightMiterLimit" */
     284L,	/*   343: "c3DDiffuseAmt" */
     438L,	/*   344: "lineTopColorExtMod" */
     336L,	/*   345: "lineBottomStartArrowWidth" */
      29L,	/*   346: "fFlipV" */
     209L,	/*   347: "gtextFShrinkFit" */
     267L,	/*   348: "fCalloutMinusY" */
     153L,	/*   349: "geoLeft" */
     275L,	/*   350: "f3DOK" */
     431L,	/*   351: "lineTopBackColorExtCMY" */
      45L,	/*   352: "fOleIcon" */
     277L,	/*   353: "fRecolorFillAsPicture" */
     202L,	/*   354: "cxstyle" */
     351L,	/*   355: "lineColumnColorExt" */
     124L,	/*   356: "fillBackOpacity" */
     143L,	/*   357: "fillShadePreset" */
      17L,	/*   358: "shapeType" */
     177L,	/*   359: "fRelChangePage" */
     439L,	/*   360: "lineTopCrMod" */
     279L,	/*   361: "ccol" */
     424L,	/*   362: "lineRightStartArrowWidth" */
     116L,	/*   363: "fLockVerticies" */
     272L,	/*   364: "cropFromTop" */
     214L,	/*   365: "gtextFItalic" */
     186L,	/*   366: "shadowScaleXToX" */
      73L,	/*   367: "lineStartArrowhead" */
     200L,	/*   368: "shadowHighlight" */
      19L,	/*   369: "rotation" */
      28L,	/*   370: "fIsBullet" */
     154L,	/*   371: "geoTop" */
     305L,	/*   372: "fillColorExt" */
     141L,	/*   373: "fillRectRight" */
     181L,	/*   374: "shadowOpacity" */
     371L,	/*   375: "lineColumnType" */
     227L,	/*   376: "fc3DUseExtrusionColor" */
     188L,	/*   377: "shadowScaleXToY" */
     313L,	/*   378: "lineBottomBackColor" */
     216L,	/*   379: "gtextFShadow" */
     420L,	/*   380: "lineRightFillWidth" */
     139L,	/*   381: "fillRectLeft" */
     120L,	/*   382: "fLockShapeType" */
     327L,	/*   383: "lineBottomEndArrowhead" */
     393L,	/*   384: "lineLeftMiterLimit" */
     410L,	/*   385: "lineRightColorExtMod" */
       2L,	/*   386: "pSegmentInfo" */
     467L,	/*   387: "c3DCrMod" */
      76L,	/*   388: "lineStartArrowLength" */
      81L,	/*   389: "lineJoinStyle" */
     286L,	/*   390: "c3DFillIntensity" */
     245L,	/*   391: "fc3DFillHarsh" */
     106L,	/*   392: "fTopInsetPen" */
     449L,	/*   393: "lineTopMiterLimit" */
      56L,	/*   394: "lTxid" */
     101L,	/*   395: "lineTopFillShape" */
     334L,	/*   396: "lineBottomOpacity" */
     195L,	/*   397: "fShadow" */
      72L,	/*   398: "lineDashing" */
     282L,	/*   399: "fFakeMaster" */
     108L,	/*   400: "fBottomInsetPen" */
     417L,	/*   401: "lineRightFillBlipFlags" */
      21L,	/*   402: "dyWrapDistTop" */
     294L,	/*   403: "c3DShininess" */
     244L,	/*   404: "fc3DKeyHarsh" */
     125L,	/*   405: "fillblipflags" */
     418L,	/*   406: "lineRightFillDztype" */
     182L,	/*   407: "shadowOffsetX" */
      78L,	/*   408: "lineEndArrowLength" */
     130L,	/*   409: "fillToLeft" */
     366L,	/*   410: "lineColumnOpacity" */
     324L,	/*   411: "lineBottomDashing" */
     304L,	/*   412: "c3DExtrusionColorExtMod" */
     212L,	/*   413: "gtextFDxMeasure" */
     253L,	/*   414: "dxyCalloutLengthSpecified" */
      39L,	/*   415: "fHitTestFill" */
      46L,	/*   416: "fOnDblClickNotify" */
     147L,	/*   417: "fillUseRect" */
     221L,	/*   418: "pictureBiLevel" */
     251L,	/*   419: "spcod" */
     152L,	/*   420: "fillCrMod" */
     310L,	/*   421: "lineBackColorExtCMY" */
     109L,	/*   422: "lineColor" */
     328L,	/*   423: "lineBottomEndCapStyle" */
      88L,	/*   424: "fColumnLine" */
     427L,	/*   425: "lineRightType" */
     114L,	/*   426: "fLockAgainstSelect" */
     281L,	/*   427: "fScriptAnchor" */
     415L,	/*   428: "lineRightEndArrowhead" */
     458L,	/*   429: "shadowColorExtCMY" */
     193L,	/*   430: "shadowOriginX" */
     213L,	/*   431: "gtextFBold" */
     452L,	/*   432: "lineTopStartArrowWidth" */
     104L,	/*   433: "fInsetPen" */
     397L,	/*   434: "lineLeftStartArrowhead" */
      31L,	/*   435: "fBehindDocument" */
      22L,	/*   436: "dxWrapDistRight" */
     363L,	/*   437: "lineColumnFillHeight" */
     345L,	/*   438: "lineColumnBackColor" */
     382L,	/*   439: "lineLeftColorExtMod" */
     283L,	/*   440: "c3DAmbientIntensity" */
     360L,	/*   441: "lineColumnEndCapStyle" */
     113L,	/*   442: "fLockAspectRatio" */
     441L,	/*   443: "lineTopEndArrowLength" */
     442L,	/*   444: "lineTopEndArrowWidth" */
     342L,	/*   445: "lineColorExtCMY" */
      10L,	/*   446: "posh" */
       5L,	/*   447: "gtextFont" */
     159L,	/*   448: "adjust3Value" */
     149L,	/*   449: "fFillShadeShapeOK" */
     317L,	/*   450: "lineBottomBackColorExtMod" */
     292L,	/*   451: "c3DRotationCenterX" */
      47L,	/*   452: "fOneD" */
     383L,	/*   453: "lineLeftCrMod" */
     184L,	/*   454: "shadowSecondOffsetX" */
     346L,	/*   455: "lineColumnBackColorExt" */
     300L,	/*   456: "c3DYRotationAngle" */
     398L,	/*   457: "lineLeftStyle" */
     295L,	/*   458: "c3DSkewAmount" */
     432L,	/*   459: "lineTopBackColorExtK" */
      85L,	/*   460: "fLineOK" */
     368L,	/*   461: "lineColumnStartArrowWidth" */
     168L,	/*   462: "relTop" */
     296L,	/*   463: "c3DSkewAngle" */
      32L,	/*   464: "fIsButton" */
      68L,	/*   465: "lineFillWidth" */
      49L,	/*   466: "fPrint" */
     142L,	/*   467: "fillRectBottom" */
    };

/* The hash function */

unsigned long docRtfShapePropHash(const unsigned char * key, int len )
{
    const ub4 initlev= 0x7b82e09b;
    ub4 rsl= 0;
    ub4 val = utilJenkinsHash( key, len, initlev );
    rsl = ((val>>25)^tab[val&0xff]);
    return rsl;
}

int docRtfShapePropIndex( const unsigned char * key, int len )
{
    ub4 rsl= docRtfShapePropHash( key, len );

    if  ( rsl >= sizeof(mapBack)/sizeof(int) )
	{ return -1;	}

    return mapBack[rsl];
}

